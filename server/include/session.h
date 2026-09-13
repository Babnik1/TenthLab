#pragma once

/// @file session.h
///
/// @brief Одно клиентское соединение. Объявление.
///

#include <array>
#include <cstddef>
#include <memory>

#include <boost/asio.hpp>

#include "async.h"

/// @brief Класс, представляющий одно TCP-соединение с клиентом.
/// @details Каждая сессия владеет собственным контекстом библиотеки
///          libasync (Connect/Receive/Disconnect), поэтому статические и
///          динамические блоки команд разных соединений обрабатываются
///          независимо друг от друга: смешивание статических блоков между
///          соединениями происходит уже на уровне вывода (общие фоновые
///          потоки WorkerManager внутри libasync), а не на уровне парсинга.
class Session : public std::enable_shared_from_this< Session >
{
public:
    /// @brief Конструктор.
    /// @param[in] socket Принятый сокет клиента.
    /// @param[in] bulkSize Размер статического блока команд.
    Session( boost::asio::ip::tcp::socket socket, std::size_t bulkSize );

    /// @brief Деструктор. Корректно завершает работу с контекстом
    ///        libasync, дожимая незавершённый статический блок.
    ~Session();

    Session( const Session& ) = delete;
    Session& operator=( const Session& ) = delete;

    /// @brief Запуск чтения данных от клиента.
    void Start();

private:
    /// @brief Запланировать очередное асинхронное чтение.
    void DoRead();

    boost::asio::ip::tcp::socket socket_;   /// Сокет клиента.
    std::array< char, 4096 > buffer_;       /// Буфер приёма сырых данных.
    void* context_;                         /// Контекст библиотеки libasync.
};

using SessionPtr = std::shared_ptr< Session >;
