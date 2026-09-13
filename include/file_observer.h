#pragma once

/// @file file_observer.h
///
/// @brief Наблюдатель, выводящий в файл.
///

#include <map>
#include <mutex>

#include "iobserver.h"


/// @brief Класс наблюдателя, выводящего в файл.
class FileObserver : public IObserver 
{
public:
    /// @brief Вывусти завершённый пакет команд в консоль.
    /// @param[in] commands Строка с командами.
    /// @param[in] timestamp Временная точка регистрации первой команды.
    void onBlock( const std::vector< std::string >& commands, std::time_t timestamp ) override;

private:
    std::map<std::time_t, int> usedTimestamps_;
    std::mutex mtx_;
};
