/// @file session.cpp
///
/// Сетевая сессия. Реализация.
///


#include "session.h"
#include "server.h"
#include <boost/asio/impl/read.hpp>
#include <boost/asio/impl/read_until.hpp>
#include <boost/asio/impl/write.hpp>
#include <boost/system/detail/error_code.hpp>
#include <memory>
#include <iostream>


Session::Session( SessionId id,
                  boost::asio::ip::tcp::socket socket,
                  HandlerPtr handler )
            : id_{ id }
            , socket_{ std::move( socket ) }
            , handler_ { handler }
{}

SessionId Session::GetId() const
{
    return id_;
}

void Session::Start()
{
    std::string msg = "Welcome to Join Server\n"
                      "Available commands:\n"
                      "INSERT <table> <id> <name>\n"
                      "TRUNCATE <table>\n"
                      "INTERSECTION\n"
                      "SYMMETRIC_DIFFERENCE\n";
    Send( msg );
    Read();
}

void Session::Send( const std::string& msg )
{
    bool writing = !outgoing_.empty();
    outgoing_.push_back( msg );

    if ( !writing )
    {
        DoWrite();
    }
}

void Session::Read()
{
    auto buffer = std::make_shared< boost::asio::streambuf >();

    boost::asio::async_read_until( 
        socket_,
        buffer_,
        '\n',
        [ this, buffer ]( const boost::system::error_code& ec, std::size_t )
        {
            if ( ec )
            {
                Disconnect();
                return;
            }

            std::istream is( &buffer_ );
            std::string command;
            while ( std::getline( is, command ) )
            {
                if ( !command.empty() && command.back() == '\r' )
                {
                    command.pop_back();
                }

                if ( command.empty() ) continue;

                std::string response = handler_->HandleCommand( command );
                if ( !response.empty() )
                {
                    Send( response );
                }
            }
            Read();
        } 
    );
}

void Session::DoWrite()
{
    boost::asio::async_write(
        socket_,
        boost::asio::buffer( outgoing_.front() ),
        [ this ]( const boost::system::error_code& ec, std::size_t )
        {
            if ( ec )
            {
                Disconnect();
                return;
            }

            outgoing_.pop_front();
            if ( !outgoing_.empty() )
            {
                DoWrite();
            }
        }
    );
}

void Session::Disconnect()
{
    boost::system::error_code ec;
    ec = socket_.shutdown( boost::asio::ip::tcp::socket::shutdown_both,ec );
    if ( ec )
    {
        std::cout << "Failed to close socket for session: " << id_;
    }
    ec = socket_.close( ec );
    if ( ec )
    {
        std::cout << "Failed to close socket for session: " << id_;
    }
}

void Session::SetSelf( std::weak_ptr< Session > self )
{
    self_ = self;
}

