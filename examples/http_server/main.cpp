//
//  main.cpp
//  acatl
//
//  BSD 3-Clause License
//  Copyright (c) 2019, Lars-Christian Fürstenberg
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

#include "connection.h"

#include <acatl/logging.h>
#include <acatl/numeric_cast.h>
#include <acatl_application/application.h>
#include <acatl_application/command_line_options.h>
#include <acatl_network/async_server.h>
#include <acatl_network/network_helper.h>

#include <asio/signal_set.hpp>

#include <fstream>
#include <iostream>

ACATL_DECLARE_EXCEPTION(ConfigurationException, acatl::Exception);

struct ServerConfig
{
  ServerConfig()
  {
  }

  ServerConfig(const std::string& host, acatl::net::Port port)
  : _host(host)
  , _port(port)
  {
  }

  bool _enable{true};
  std::string _host{"127.0.0.1"};
  acatl::net::Port _port{8080};
};

struct SecureServerConfig : public ServerConfig
{
  SecureServerConfig()
  : ServerConfig("127.0.0.1", 8443)
#if defined __APPLE__
  , _caFilePath(fs::path("/usr/local/etc/openssl/cert.pem"))
#endif
  {
  }

  fs::path _caFilePath;
  fs::path _keyFilePath{"./certs/key.pem"};
  fs::path _caCertFilePath{"./certs/server.pem"};
  bool _noVerify{false};
};

class HttpServer : public acatl::Application
{
public:
  HttpServer(int argc, char** argv)
  : acatl::Application(argc, argv)
  , _numThreads(1)
  {
  }

private:
  typedef Connection<acatl::net::SecureSocket> SecureConnectionType;
  typedef acatl::net::AsyncServer<SecureConnectionType, asio::ssl::context, HttpConfig> SecureServerType;
  typedef std::shared_ptr<SecureServerType> SecureServerTypePtr;

  typedef Connection<acatl::net::Socket> ConnectionType;
  typedef acatl::net::AsyncServer<ConnectionType, acatl::net::NullContext, HttpConfig> ServerType;
  typedef std::shared_ptr<ServerType> ServerTypePtr;

  bool setUp(const acatl::StringVector& args) override
  {
    // clang-format off
        acatl::CommandLineOptions options("http_server", {
            {
                "Help", {
                    {"", "help", 1, 1, "display this help and exit"}
                }
            },
            {
                "Options", {
                    {"p", "port", "<PORT>", 0, 1, "http port to connect to"},
                    {"s", "secure-port", "<PORT>", 0, 1, "https port to connect to"},
                    {"h", "host", "<HOST>", 0, 1, "host"},
                    {"d", "doc-root-path", "<PATH>", 0, 1, "document root path"},
                    {"", "ca-path", "<PATH>", 0, 1, "path to ca store"},
                    {"", "cert-path", "<PATH>", 0, 1, "path to cert file"},
                    {"c", "config-path", "<PATH>", 0, 1, "path to config file"},
                    {"n", "threads", "<NUMBER>", 0, 1, "number of threads to spawn"}
                }
            }
        });
    // clang-format on

    std::stringstream ss;
    auto ret = options.parse(args, ss);

    if(!ret) {
      std::cerr << ss.str() << std::endl;
      options.usage(std::cerr);
      return false;
    }

    if(options.count("help") > 0) {
      options.usage(std::cerr);
      return false;
    }

    auto path = fs::absolute(args[0]).parent_path() / "http_server.json";
    if(options.count("config-path") > 0) {
      path = options.option("config-path").value<fs::path>();
    }

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

      _httpConfig._rootPath = jsonConfiguration.value("docroot-path", "./docroot");
      _httpConfig._domain = jsonConfiguration.value("domain", acatl::net::gethostname());
      if(jsonConfiguration.find("http") != jsonConfiguration.end()) {
        const json& server = jsonConfiguration["http"];
        _config._enable = server.value("enable", true);
        _config._host = server.value("host", "127.0.0.1");
        _config._port = server.value("port", static_cast<acatl::net::Port>(8080));
      }
      if(jsonConfiguration.find("https") != jsonConfiguration.end()) {
        const json& server = jsonConfiguration["https"];
        _secureConfig._enable = server.value("enable", true);
        _secureConfig._host = server.value("host", "127.0.0.1");
        _secureConfig._port = server.value("port", static_cast<acatl::net::Port>(8443));
        _secureConfig._caFilePath = server.value("ca-file-path", "/usr/local/etc/openssl/cert.pem");
        _secureConfig._keyFilePath = server.value("key-file-path", "./certs/key.pem");
        _secureConfig._caCertFilePath = server.value("ca-cert-file-path", "./certs/server.pem");
        _secureConfig._noVerify = server.value("no-verify", false);
      }
    } else {
      ACATL_THROW(ConfigurationException, "Configuration cannot be loaded");
    }

    if(options.count("port") > 0) {
      _config._port = options.option("port").value<uint16_t>();
    }
    if(options.count("host") > 0) {
      _config._host = options.option("host").value<std::string>();
      _secureConfig._host = options.option("host").value<std::string>();
    }
    if(options.count("docroot-path") > 0) {
      _httpConfig._rootPath = options.option("docroot-path").value<fs::path>();
    }
    if(options.count("secure-port") > 0) {
      _secureConfig._port = options.option("secure-port").value<uint16_t>();
    }
    if(options.count("ca-path") > 0) {
      _secureConfig._caFilePath = options.option("ca-path").value<fs::path>();
    }
    if(options.count("key-path") > 0) {
      _secureConfig._keyFilePath = options.option("key-path").value<fs::path>();
    }
    if(options.count("cert-path") > 0) {
      _secureConfig._caCertFilePath = options.option("cert-path").value<fs::path>();
    }
    if(options.count("threads") > 0) {
      _numThreads = options.option("threads").value<uint32_t>();
    } else {
      _numThreads = std::thread::hardware_concurrency();
    }
    if(_numThreads == 0) {
      _numThreads = 1;
    }

    return true;
  }

  int doRun() override
  {
    acatl::net::IoContextPool ioContextPool(_numThreads);

    asio::signal_set signals(ioContextPool.get(), SIGINT, SIGTERM);
    signals.async_wait([&ioContextPool](const std::error_code& ec, int signal_number) {
      if(signal_number == SIGINT || signal_number == SIGTERM) {
        ioContextPool.stop();
      }
    });

    SecureServerTypePtr secureServer;
    asio::ssl::context sslContext{asio::ssl::context::tlsv12_server};
    if(_secureConfig._enable) {
      sslContext.use_certificate_chain_file(_secureConfig._caCertFilePath);
      sslContext.use_private_key_file(_secureConfig._keyFilePath, asio::ssl::context::pem);
      secureServer = std::make_shared<SecureServerType>(
        ioContextPool, sslContext, _secureConfig._host, _secureConfig._port, _httpConfig);
    }

    ServerTypePtr server;
    acatl::net::NullContext context;
    if(_config._enable) {
      server = std::make_shared<ServerType>(ioContextPool, context, _secureConfig._host, _config._port, _httpConfig);
    }

    ioContextPool.run();

    return 0;
  }

  void tearDown(int exitCode) override
  {
    ACATL_DEBUGLOG(HttpServer, 1, "Terminating application");
  }

  SecureServerConfig _secureConfig;
  ServerConfig _config;
  uint32_t _numThreads;
  HttpConfig _httpConfig;
};

int main(int argc, char** argv)
{
  HttpServer server{argc, argv};
  return server.run();
}
