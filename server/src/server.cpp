/// @file server.cpp
///
/// @brief Асинхронный TCP-сервер. Реализация.
///

#include "server.h"

#include <iostream>
#include <memory>

#include "session.h"

Server::Server( boost::asio::io_context& ioContext, unsigned short port, std::size_t bulkSize )
    : acceptor_( ioContext, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), port ) )
    , bulkSize_( bulkSize )
{
    std::cout << "Server was started on port " << port
               << " (bulk size " << bulkSize_ << ")...\n";
    DoAccept();
}

void Server::DoAccept()
{
    // Ограничений на количество одновременно подключенных клиентов нет -
    // на каждое успешное подключение создаётся отдельная Session со своим
    // контекстом libasync, после чего сервер немедленно готов принимать
    // следующее соединение.
    acceptor_.async_accept(
        [ this ]( const boost::system::error_code& ec, boost::asio::ip::tcp::socket socket )
        {
            if ( !ec )
            {
                boost::system::error_code epEc;
                auto endpoint = socket.remote_endpoint( epEc );
                if ( !epEc )
                {
                    std::cout << "Client " << endpoint.address().to_string() << " connected\n";
                }

                auto session = std::make_shared< Session >( std::move( socket ), bulkSize_ );
                session->Start();
            }
            else
            {
                std::cout << "Accept error: " << ec.message() << "\n";
            }

            DoAccept();
        } );
}
