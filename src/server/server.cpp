/// @file server.h
///
/// @brief Класс TCP сервера. Реализация.
///

#include <boost/asio/io_context.hpp>
#include <boost/system/detail/error_code.hpp>
#include <memory>
#include <iostream>
#include "server.h"


Server::Server( short port, std::shared_ptr< Handler > handler )
    : ioContext_{}
    , acceptor_{ ioContext_, boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4(), port ) }
    , handler_{ handler }
{
    // sessionsManager_ = std::make_unique< SessionsManager >();
}

void Server::Start()
{
    std::cout << "Server was started...\n";
    DoAccept();
    ioContext_.run();
}

void Server::Stop()
{
    std::cout << "Stopping server...\n";

    boost::system::error_code error;

    acceptor_.close( error );

    if ( error )
    {
        std::cout <<
            "Failed to stop server: "
            << error.message() << "\n";
    }

    ioContext_.stop();
}

void Server::DoAccept()
{
    SessionId id = nextSessionId_++;
    auto socket = std::make_shared< boost::asio::ip::tcp::socket >( ioContext_ );
    acceptor_.async_accept( *socket, [ this, socket, id ]( const boost::system::error_code& error )
        {
            if ( !error )
            {
                std::cout << "Client " << socket->remote_endpoint().address() << " connected succesfully\n";
                auto session = std::make_shared< Session >( id, std::move( *socket ), handler_ );
                sessions_.emplace(
                    session->GetId(), 
                    session );
                session->SetSelf( session );
                session->Start();
            }
            else
            {
                std::cout << "Connection error." << error.message() << "\n";
            }

            if ( !acceptor_.is_open() )
            {
               std::cout << "Server was stopped";
                return;
            }
            DoAccept();
        } );                                    
}

void Server::RemoveSession( SessionId id )
{
    sessions_.erase( id );
}



