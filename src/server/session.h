#pragma once
/// @file session.h
///
/// @brief Сетевая сессия. Объявление.
///


#include <cstdint>
#include <deque>
#include <memory>
#include <boost/asio.hpp>
#include "handler/handler.h"


/// @brief ID сессии.
using SessionId = uint64_t;

/// @brief Очередь сообщений клиенту.
using MessageQueue = std::deque< std::string >;

inline uint64_t invalidSessionId = 0 ;      /// Невалидный ID сессии.


/// @brief Класс сессии.
class Session
{
public:

    /// @brief Конструктор.
    /// @param[in] id ID сессии.
    /// @param[in] socket Сокет.
    /// @param[in] handler Указатель на обработчик.
    Session( SessionId id, boost::asio::ip::tcp::socket socket, HandlerPtr handler );

    /// @brief Старт сессии.
    void Start();

    /// @brief Получить ID сессии.
    /// @return ID сессии.
    SessionId GetId() const;

    /// @brief Отправка сообщения клиенту.
    /// @param [in] msg Сообщение.
    void Send( const std::string& msg );

    /// @brief Установка указателя на себя.
    /// @param [in] self Указатель на себя.
    void SetSelf( std::weak_ptr< Session > self );

private:

    /// @brief Прочитать сообщение от клиента.
    void Read();

    /// @brief Записать сообщение на отправку.
    void DoWrite();

    /// @brief Отключение сессии.
    void Disconnect();

    boost::asio::ip::tcp::socket socket_;   /// Сокет.
    boost::asio::streambuf buffer_;         /// Указатель на брокер.

    SessionId id_;                          /// ID сессии.
    MessageQueue outgoing_;                 /// Очередь на отправку.
    HandlerPtr handler_;                    /// Указатель на обработчик.
    std::weak_ptr< Session > self_;         /// Указатель на саму себя.
};

using SessionPtr = std::shared_ptr< Session >;
using SessionWeakPtr = std::weak_ptr< Session >;