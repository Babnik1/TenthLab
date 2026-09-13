/// @file file_observer.cpp
///
/// @brief Наблюдатель, выводящий в файл.
///

#include "file_observer.h"

#include <fstream>
#include <string>

void FileObserver::onBlock(const std::vector<std::string>& commands, std::time_t timestamp)
{
    std::string filename;
    {
        std::lock_guard<std::mutex> lock(mtx_);

        filename = "bulk" + std::to_string(timestamp);

        auto it = usedTimestamps_.find(timestamp);
        if (it == usedTimestamps_.end()) {
            usedTimestamps_[timestamp] = 0;
        } else {
            ++(it->second);
            filename += "_" + std::to_string(it->second);
        }
        filename += ".log";
    }

    std::ofstream out(filename);
    out << "bulk: ";
    for (std::size_t i = 0; i < commands.size(); ++i) {
        out << commands[i];
        if (i + 1 < commands.size()) {
            out << ", ";
        }
    }
    out << std::endl;
}
