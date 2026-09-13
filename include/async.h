#pragma once

/// @file async.h
///
/// @brief Внешний интерфейс библиотеки libasync.
///

#include <cstddef>

/// @brief Открытие независимого контекста обработки.
/// @param[in] blockSize Размер статического блока.
/// @return void указатель на контекст.
void* Connect( std::size_t blockSize );

/// @brief Передача сырых данных для определенного контекста.
/// @param[in] context Контекст.
/// @param[in] buf Буфер с данными.
/// @param[in] len Размер буфера.
void  Receive( void* context, char* buf, std::size_t len );

/// @brief Завершение работы с контекстом.
/// @param[in] context Контекст.
void  Disconnect( void* context );