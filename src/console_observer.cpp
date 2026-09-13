/// @file console_observer.cpp
///
/// @brief Наблюдатель, выводящий в консоль.
///


#include "console_observer.h"

#include <iostream>

void ConsoleObserver::onBlock(const std::vector<std::string>& commands, std::time_t /*timestamp*/)
{
    std::cout << "bulk: ";
    for (std::size_t i = 0; i < commands.size(); ++i) {
        std::cout << commands[i];
        if (i + 1 < commands.size()) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
}
