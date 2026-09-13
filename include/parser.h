#pragma once

/// @file parser.h
///
/// @brief Парсер комманд.
///

#include <cstddef>
#include <ctime>
#include <memory>
#include <string>
#include <vector>

#include "iobserver.h"

/// @brief Класс парсера пакетных команд.
class Parser 
{
public:
    explicit Parser( std::size_t blockSize );

    /// @brief Подписать наблюдателя на события завершения блока.
    /// @param[in] observer Наблюдатель.
    void subscribe( const std::shared_ptr<IObserver>& observer );

    /// @brief Обработать очередную строку входных данных (одна команда,
    /// @param[in] line Строка данных.
    void receiveLine( const std::string& line );

    /// @brief Сообщить парсеру о том, что входные данные закончились.
    void receiveEof();

private:
    /// @brief Уведомить наблюдателя, о появлении данных.
    /// @param[in] commands Строка с командами.
    /// @param[in] timestamp Временная точка регистрации первой команды.
    void notify( const std::vector< std::string >& commands, std::time_t timestamp );

    /// @brief Обработать команду.
    /// @brief cmd Команда.
    void handleCommand( const std::string& cmd );

    /// @brief Обработать открывающую скобку.
    void handleOpenBrace();

    /// @brief Обработать закрывающую скобку.
    void handleCloseBrace();

    /// @brief Очистить статический блок.
    void flushStatic();

    std::size_t blockSize_;
    std::vector< std::shared_ptr< IObserver > > observers_;

    // Состояние статического блока.
    std::vector< std::string > staticBuffer_;
    std::time_t staticStartTime_ = 0;

    // Состояние динамического блока.
    int dynamicDepth_ = 0;
    std::vector< std::string > dynamicBuffer_;
    std::time_t dynamicStartTime_ = 0;
};
