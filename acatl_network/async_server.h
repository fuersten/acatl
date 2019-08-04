//
//  async_server.h
//  acatl
//
//  BSD 3-Clause License
//  Copyright (c) 2018, Lars-Christian Fürstenberg
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

#ifndef acatl_async_server_h
#define acatl_async_server_h

#include <acatl_network/io_context_pool.h>
#include <acatl_network/socket_type.h>

namespace acatl
{
  namespace net
  {
    template<typename ConnectionType, typename SslContextType, typename ConfigType>
    class AsyncServer
    {
    public:
      typedef typename ConnectionType::SocketType SocketType;

      AsyncServer(acatl::net::IoContextPool& ioContextPool,
                  SslContextType& sslContext,
                  const std::string& host,
                  unsigned short port,
                  const ConfigType& config)
      : _acceptor(ioContextPool.get())
      , _socket(std::move(acatl::net::make_socket<SocketType>(ioContextPool.get(), sslContext)))
      {
        asio::ip::tcp::resolver resolver(ioContextPool.get());
        asio::ip::tcp::resolver::query query(host, std::to_string(port));
        asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
        _acceptor.open(endpoint.protocol());
        _acceptor.set_option(asio::ip::tcp::acceptor::reuse_address(true));
        _acceptor.bind(endpoint);
        _acceptor.listen();

        ACATL_DEBUGLOG(AsyncServer, 2, "Starting sever listening on port " << endpoint.port());
        do_accept(ioContextPool, sslContext, config);
      }

      ~AsyncServer()
      {
        ACATL_DEBUGLOG(AsyncServer, 2, "Stopping sever");
      }

    private:
      class HandshakeWrapper : public std::enable_shared_from_this<HandshakeWrapper>
      {
      public:
        HandshakeWrapper(SocketType&& socket)
        : _socket(std::move(socket))
        {
        }

        void handshake(const ConfigType& config)
        {
          auto self(this->shared_from_this());
          _socket.handshake([this, self, &config](const std::error_code& ec) {
            if(!ec) {
              std::make_shared<ConnectionType>(std::move(_socket), config)->start();
            }
          });
        }

      private:
        SocketType _socket;
      };

      void do_accept(acatl::net::IoContextPool& ioContextPool, SslContextType& sslContext, const ConfigType& config)
      {
        _socket = std::move(acatl::net::make_socket<SocketType>(ioContextPool.get(), sslContext));
        _acceptor.async_accept(
          _socket().lowest_layer(), [this, &ioContextPool, &sslContext, &config](std::error_code ec) {
            if(!ec) {
              asio::error_code ignored_ec;
              ACATL_DEBUGLOG(AsyncServer,
                             2,
                             "Incoming connection from "
                               << _socket.lowest_layer().remote_endpoint().address().to_string(ignored_ec));

              std::make_shared<HandshakeWrapper>(std::move(_socket))->handshake(config);
            } else {
              ACATL_DEBUGLOG(AsyncServer, 2, "Incoming connection had an error " << ec);
            }

            do_accept(ioContextPool, sslContext, config);
          });
      }

      asio::ip::tcp::acceptor _acceptor;
      SocketType _socket;
    };
  }
}

#endif
