//
//  connection.h
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

#ifndef acatl_mqtt_connection_h
#define acatl_mqtt_connection_h

#include <acatl/logging.h>
#include <acatl/logging.h>
#include <acatl/string_helper.h>

#include <acatl_network/http_url_parser.h>
#include <acatl_network/socket_type.h>

#include "acatl_mqtt/mqtt_processor.h"
#include "acatl_mqtt/mqtt_packet_sender.h"
#include "acatl_mqtt/mqtt_parser.h"
#include "acatl_mqtt/mqtt_serializer.h"
#include "acatl_mqtt/mqtt_utils.h"

#include <queue>


class MQTTContext
{
public:
  MQTTContext(acatl::mqtt::SubscriptionTreeManager& subscriptionTreeManager,
              acatl::mqtt::SessionManager& sessionManager)
  : _subscriptionTreeManager{subscriptionTreeManager}
  , _sessionManager{sessionManager}
  {}

  acatl::mqtt::SubscriptionTreeManager& _subscriptionTreeManager;
  acatl::mqtt::SessionManager& _sessionManager;
};


template<typename Socket>
class Connection : public acatl::mqtt::PacketSender, public std::enable_shared_from_this<Connection<Socket>>
{
public:
  typedef Socket SocketType;
    typedef std::shared_ptr<Connection> Ptr;

    Connection(SocketType&& socket, const MQTTContext& context)
    : _isSending(false)
    , _socket(std::move(socket))
    , _subscriptionTreeManager(context._subscriptionTreeManager)
    , _sessionManager(context._sessionManager)
    , _mqttProcessor(_subscriptionTreeManager, _sessionManager)
    {
        _readBuf.resize(64);
        _writeBuf.resize(64);
    }
    
    ~Connection()
    {
        ACATL_CLASSLOG(Connection, 1, "Terminating connection");
    }
    
    void start()
    {
        ACATL_CLASSLOG(Connection, 1, "Start connection");
        _mqttProcessor.setPacketSender(this->shared_from_this());
      do_read();
    }

private:
  void do_read()
  {
    auto self(this->shared_from_this());
    _socket().async_read_some(asio::buffer(_readBuf, _readBuf.size()), [self](std::error_code ec, std::size_t length) {
      if(!ec) {
        self->handle_read(length);
      } else {
        ACATL_ERRORLOG("Read error: " << ec.message());
      }
    });
  }

  void handle_read(size_t length)
  {
      ACATL_CLASSLOG(Connection, 4, "Client sends " << length << " bytes of data");
      uint16_t index = 0;

      while(index < length) {
        std::error_code errc;
        acatl::Tribool ret;

        while(ret.isIndeterminate() && index < length) {
          ret = _mqttParser.parse(static_cast<uint8_t>(_readBuf[index++]), errc);
        }

        // TODO if we have a mqtt_error::clean_session_not_set_for_empty_client_id error, the broker has to respond
        //      with a CONNACK return code 0x02 (Identifier rejected) and then close the connection
        if(ret.isFalse() || errc) {
          ACATL_ERRORLOG("Error: " << errc.message());
          // close the connection
          return;
        } else if(ret.isTrue()) {
          acatl::mqtt::ControlPacket::Ptr packet = _mqttParser.consumePacket();
          ACATL_CLASSLOG(Connection, 1, "Client sends " << packet->_header._controlPacketType);

          std::tuple<acatl::mqtt::ConnectionState, acatl::mqtt::ControlPacket::Ptr> result = _mqttProcessor.processPacket(std::move(packet), errc);
          if(std::get<1>(result)) {
            addSendPacket(std::move(std::get<1>(result)));
          }

          if(errc) {
            ACATL_ERRORLOG("Processor error: " << errc.message());
            // close the connection
            return;
          }
          if(std::get<0>(result) == acatl::mqtt::ConnectionState::Close) {
            return;
          }
        }
      }
    do_read();
  }

  void addSendPacket(acatl::mqtt::ControlPacket::Ptr packet) override
  {
    ACATL_CLASSLOG(Connection, 3, "Server enqueues " << packet->_header._controlPacketType);
    {
      std::unique_lock<std::mutex> guard(_sendMutex);
      _sendPackets.push(std::move(packet));
    }
    sendPackages();
  }

    void sendPackages()
    {
        {
            std::unique_lock<std::mutex> guard(_sendMutex);
            if(_isSending) {
                return;
            }
            
            _isSending = true;
        }
        doSendPackages();
    }
    
    void doSendPackages()
    {
        ACATL_CLASSLOG(Connection, 3, "Server starts to send pending packets");
        std::unique_lock<std::mutex> guard(_sendMutex);
        if(_sendPackets.empty()) {
            ACATL_CLASSLOG(Connection, 3, "No more pending packets");
            _isSending = false;
            return;
        }
        
        acatl::mqtt::ControlPacket::Ptr nextPacket = std::move(_sendPackets.front());
        _sendPackets.pop();
        
        ACATL_CLASSLOG(Connection, 3, "Server sends packet " << nextPacket->_header._controlPacketType);
        
        std::error_code ec;
        size_t length = 0;
        if(!_serializer.serialize(std::move(nextPacket), _writeBuf, length, ec)) {
            ACATL_ERRORLOG("Cannot serialize packet: " << ec.message());
            // TODO should terminate connection here
            _isSending = false;
            return;
        }

      do_write(length);
    }

  void do_write(std::size_t length)
  {
    if(length > 0) {
      auto self(this->shared_from_this());
      asio::async_write(_socket(), asio::buffer(_writeBuf, length), [self](std::error_code ec, std::size_t /*length*/) {
        if(!ec) {
          ACATL_CLASSLOG(Connection, 3, "Packet sending ready");
          self->doSendPackages();
        } else {
          ACATL_ERRORLOG("Write error: " << ec.message());
        }
      });
    }
  }

    std::vector<uint8_t> _readBuf;
    std::vector<uint8_t> _writeBuf;
    std::mutex _sendMutex;
    bool _isSending;
    std::queue<acatl::mqtt::ControlPacket::Ptr> _sendPackets;
    
    acatl::mqtt::MQTTParser _mqttParser;
  SocketType _socket;
  acatl::mqtt::Serializer _serializer;
    acatl::mqtt::SubscriptionTreeManager& _subscriptionTreeManager;
    acatl::mqtt::SessionManager& _sessionManager;
    acatl::mqtt::Processor _mqttProcessor;
};

#endif
