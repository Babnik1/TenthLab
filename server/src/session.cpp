/// @file session.cpp
///
/// @brief Одно клиентское соединение. Реализация.
///

#include "session.h"

#include <iostream>

Session::Session( boost::asio::ip::tcp::socket socket, std::size_t bulkSize )
    : socket_( std::move( socket ) )
    , context_( Connect( bulkSize ) )
{
}

Session::~Session()
{
    // Даже если соединение было разорвано клиентом посреди статического
    // блока, Disconnect() сообщит парсеру о конце данных и "дожмёт"
    // накопленные, но ещё не выведенные команды - см. requirements.
    Disconnect( context_ );
}

void Session::Start()
{
    DoRead();
}

void Session::DoRead()
{
    auto self = shared_from_this();
    socket_.async_read_some(
        boost::asio::buffer( buffer_ ),
        [ this, self ]( const boost::system::error_code& ec, std::size_t length )
        {
            if ( ec )
            {
                // Клиент отключился или произошла ошибка сети - дальше не
                // читаем. Сама сессия (и её контекст libasync) будет жить,
                // пока существует хотя бы один shared_ptr на неё; как
                // только эта лямбда - последний держатель - разрушится,
                // отработает деструктор Session и вызовет Disconnect().
                boost::system::error_code ignored;
                socket_.shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored );
                socket_.close( ignored );
                return;
            }

            Receive( context_, buffer_.data(), length );
            DoRead();
        } );
}
