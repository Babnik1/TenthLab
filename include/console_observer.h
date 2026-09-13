#pragma once

/// @file console_observer.h
///
/// @brief Наблюдатель, выводящий в консоль.
///

#include "iobserver.h"


/// @brief Класс наблюдателя, выводящего в консоль.
class ConsoleObserver : public IObserver 
{
public:
    /// @brief Вывусти завершённый пакет команд в консоль.
    /// @param[in] commands Строка с командами.
    /// @param[in] timestamp Временная точка регистрации первой команды. Не используется.
    void onBlock( const std::vector< std::string >& commands, std::time_t timestamp ) override;
};
