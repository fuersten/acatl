//
//  main.cpp
//  acatl_mqtt
//
//  BSD 3-Clause License
//  Copyright (c) 2017, Lars-Christian Fürstenberg
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted
//  provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//  conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of
//  conditions and the following disclaimer in the documentation and/or other materials provided
//  with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors may be used to
//  endorse or promote products derived from this software without specific prior written
//  permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
//  AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//

#include <acatl/filesystem.h>
#include <acatl/logging.h>
#include <acatl/numeric_cast.h>
#include <acatl/string_helper.h>

#include <acatl_network/async_server.h>
#include <acatl_network/network_helper.h>

#include <asio/signal_set.hpp>

#include <acatl_application/application.h>

#include <acatl_mqtt/mqtt_session_manager.h>
#include <acatl_mqtt/mqtt_subscription_tree_manager.h>

#include "connection.h"

#include <fstream>
#include <vector>

ACATL_DECLARE_EXCEPTION(ConfigurationException, acatl::Exception);

namespace fs = acatl::filesystem;


class MQTTBroker : public acatl::Application
{
public:
  typedef Connection<acatl::net::SecureSocket> SecureConnectionType;
  typedef acatl::net::AsyncServer<SecureConnectionType, asio::ssl::context, MQTTContext> SecureServerType;
  typedef std::shared_ptr<SecureServerType> SecureServerTypePtr;

  typedef Connection<acatl::net::Socket> ConnectionType;
  typedef acatl::net::AsyncServer<ConnectionType, acatl::net::NullContext, MQTTContext> ServerType;
  typedef std::shared_ptr<ServerType> ServerTypePtr;

  MQTTBroker(int argc, char **argv)
  : acatl::Application(argc, argv)
  {}

  ~MQTTBroker()
  {
  }

  virtual int	doRun()
  {
    acatl::net::IoContextPool ioContextPool(std::thread::hardware_concurrency());

    asio::signal_set signals(ioContextPool.get(), SIGINT, SIGTERM);
    signals.async_wait([&ioContextPool](const std::error_code& ec, int signal_number) {
      if(signal_number == SIGINT || signal_number == SIGTERM) {
        ioContextPool.stop();
      }
    });

    SecureServerTypePtr secureServer;
    asio::ssl::context sslContext{asio::ssl::context::tlsv12_server};
    if(_configuration.hasSecureMQTT()) {
      sslContext.use_certificate_chain_file(_configuration._caCertFilePath);
      sslContext.use_private_key_file(_configuration._keyFilePath, asio::ssl::context::pem);
      secureServer = std::make_shared<SecureServerType>(
                                                        ioContextPool, sslContext, _configuration._secureHost, _configuration._securePort, _mqttContext);
    }

    ServerTypePtr server;
    acatl::net::NullContext context;
    if(_configuration.hasMQTT()) {
      server = std::make_shared<ServerType>(ioContextPool, context, _configuration._host, _configuration._port, _mqttContext);
    }

    if(!server && !secureServer) {
      ACATL_THROW(ConfigurationException, "You have to configure at least one server type");
    }

    ioContextPool.run();

    return 0;
  }

  virtual bool setUp(const acatl::StringVector& args)
  {
    fs::path path = fs::absolute(args[0]).parent_path() / "mqtt_broker.json";
    std::error_code ec;
    if(fs::exists(path, ec) && !ec) {
      json jsonConfiguration;
      std::ifstream config{path};
      if(config.good()) {
        jsonConfiguration = json::parse(config);
      } else {
        ACATL_THROW(ConfigurationException, "Configuration file cannot be opened");
      }

      acatl::Logging::init(jsonConfiguration);
      _configuration.init(jsonConfiguration);
    } else {
      // default configuration
      acatl::Logging::init(json());
      _configuration.setDefaults();
    }

    return true;
  }

  virtual void tearDown(int exitCode)
  {
    ACATL_CLASSLOG(MQTTBroker, 1, "exit code: " << exitCode);
  }

private:

  class Configuration
  {
  public:
    Configuration()
    : _port(0)
    , _securePort(0)
    {
    }

    void setDefaults()
    {
      _secureHost = "127.0.0.1";
      _securePort = 8883;
      _caFilePath = "/usr/local/etc/libressl/cert.pem";
      _keyFilePath = ".cakey.pem";
      _caCertFilePath = ".cacert.pem";
      _noVerify = false;

      _host = "127.0.0.1";
      _port = 1883;
    }

    void init(const json& config)
    {
      if(config.find("secure-mqtt") != config.end()) {
        const json& secure_mqtt = config["secure-mqtt"];
        _secureHost = secure_mqtt.value("host", "127.0.0.1");
        _securePort = secure_mqtt.value("port", static_cast<acatl::net::Port>(8883));
        _caFilePath = secure_mqtt.value("ca-file-path", "/usr/local/etc/openssl/cert.pem");
        _keyFilePath = secure_mqtt.value("key-file-path", "./certs/key.pem");
        _caCertFilePath = secure_mqtt.value("ca-cert-file-path", "./certs/server.pem");
        _noVerify = secure_mqtt.value("no-verify", false);
      }

      if(config.find("mqtt") != config.end()) {
        const json& mqtt = config["mqtt"];
        _host = mqtt.value("host", "127.0.0.1");
        _port = mqtt.value("port", static_cast<acatl::net::Port>(1883));
      }
    }

    bool hasSecureMQTT() const
    {
      return _securePort != 0;
    }

    bool hasMQTT() const
    {
      return _port != 0;
    }

    std::string _host;
    acatl::net::Port _port;

    std::string _secureHost;
    acatl::net::Port _securePort;
    fs::path _caFilePath;
    fs::path _keyFilePath;
    fs::path _caCertFilePath;
    bool _noVerify;
  };

  Configuration _configuration;
  acatl::mqtt::SubscriptionTreeManager _subscriptionTreeManager;
  acatl::mqtt::SessionManager _sessionManager;
  MQTTContext _mqttContext{_subscriptionTreeManager, _sessionManager};
};


int main(int argc, char** argv) {
  MQTTBroker app(argc, argv);
  app.run();
  return 0;
}
