#pragma once

/// @file server.h
///
/// @brief Асинхронный TCP-сервер. Объявление.
///

#include <cstddef>

#include <boost/asio.hpp>

/// @brief Класс сервера, принимающего произвольное число одновременных
///        подключений на заданном порту (со всех интерфейсов).
class Server
{
public:
    /// @brief Конструктор. Сразу начинает асинхронный приём подключений.
    /// @param[in] ioContext Контекст ввода-вывода.
    /// @param[in] port TCP-порт для входящих соединений.
    /// @param[in] bulkSize Размер статического блока команд.
    Server( boost::asio::io_context& ioContext, unsigned short port, std::size_t bulkSize );

    Server( const Server& ) = delete;
    Server& operator=( const Server& ) = delete;

private:
    /// @brief Запланировать приём очередного подключения.
    void DoAccept();

    boost::asio::ip::tcp::acceptor acceptor_;   /// Акцептор входящих соединений.
    std::size_t bulkSize_;                      /// Размер статического блока команд.
};
