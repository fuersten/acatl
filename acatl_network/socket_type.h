//
//  socket_type.h
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

#ifndef acatl_socket_type_h
#define acatl_socket_type_h

#include <asio.hpp>
#include <asio/ssl.hpp>

namespace acatl
{
  namespace net
  {
    typedef std::function<void(const std::error_code&)> HandshakeCompletionFunc;

    class Socket
    {
    public:
      Socket(asio::io_context& context)
      : _socket(context)
      {
      }

      Socket(Socket&& socket)
      : _socket(std::move(socket._socket))
      {
      }

      Socket& operator=(Socket&& other) = default;

      void handshake(HandshakeCompletionFunc completionFunc)
      {
        std::error_code ec;
        completionFunc(ec);
      }

      asio::ip::tcp::socket& operator()()
      {
        return _socket;
      }

      asio::ip::tcp::socket& lowest_layer()
      {
        return _socket;
      }

    private:
      asio::ip::tcp::socket _socket;
    };

    class SecureSocket
    {
    public:
      SecureSocket(asio::io_context& context, asio::ssl::context& sslContext)
      : _socket(std::make_unique<asio::ssl::stream<asio::ip::tcp::socket>>(context, sslContext))
      {
      }

      SecureSocket(SecureSocket&& socket)
      : _socket(std::move(socket._socket))
      {
      }

      SecureSocket& operator=(SecureSocket&& other) = default;

      void handshake(HandshakeCompletionFunc completionFunc)
      {
        _socket->async_handshake(asio::ssl::stream_base::server,
                                 [completionFunc](const asio::error_code& ec) { completionFunc(ec); });
      }

      asio::ssl::stream<asio::ip::tcp::socket>& operator()()
      {
        return *_socket;
      }

      asio::ssl::stream<asio::ip::tcp::socket>::lowest_layer_type& lowest_layer()
      {
        return _socket->lowest_layer();
      }

    private:
      std::unique_ptr<asio::ssl::stream<asio::ip::tcp::socket>> _socket;
    };

    struct NullContext
    {
    };

    namespace detail
    {
      template<typename SocketType, typename ContextType>
      struct make_socket
      {
        static SocketType impl(asio::io_context& ioContext, ContextType& sslContext)
        {
          SocketType socket{ioContext, sslContext};
          return socket;
        }
      };

      template<typename SocketType>
      struct make_socket<SocketType, NullContext>
      {
        static SocketType impl(asio::io_context& ioContext, NullContext& context)
        {
          SocketType socket{ioContext};
          return socket;
        }
      };
    }

    template<typename SocketType, typename ContextType>
    static SocketType make_socket(asio::io_context& ioContext, ContextType& sslContext)
    {
      return detail::make_socket<SocketType, ContextType>::impl(ioContext, sslContext);
    }
  }
}

#endif
