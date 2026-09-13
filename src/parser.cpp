/// @file parser.cpp
///
/// @brief Парсер комманд.
///

#include "parser.h"

Parser::Parser(std::size_t blockSize)
    : blockSize_(blockSize)
{
}

void Parser::subscribe(const std::shared_ptr<IObserver>& observer)
{
    observers_.push_back( observer );
}

void Parser::notify(const std::vector<std::string>& commands, std::time_t timestamp)
{
    for (const auto& observer : observers_) {
        observer->onBlock(commands, timestamp);
    }
}

void Parser::flushStatic()
{
    if (!staticBuffer_.empty()) {
        notify(staticBuffer_, staticStartTime_);
        staticBuffer_.clear();
    }
}

void Parser::handleCommand( const std::string& cmd )
{
    if (dynamicDepth_ > 0) {
        if (dynamicBuffer_.empty()) {
            dynamicStartTime_ = std::time(nullptr);
        }
        dynamicBuffer_.push_back(cmd);
        return;
    }

    if (staticBuffer_.empty()) {
        staticStartTime_ = std::time(nullptr);
    }
    staticBuffer_.push_back(cmd);
    if (staticBuffer_.size() == blockSize_) {
        flushStatic();
    }
}

void Parser::handleOpenBrace()
{
    if (dynamicDepth_ == 0) {
        // Открытие динамического блока принудительно завершает
        // текущий статический блок (даже неполный).
        flushStatic();
        dynamicBuffer_.clear();
        dynamicDepth_ = 1;
    } else {
        // Вложенная "{" - сама по себе не команда, только увеличивает
        // глубину вложенности, чтобы найти парную закрывающую скобку.
        ++dynamicDepth_;
    }
}

void Parser::handleCloseBrace()
{
    if (dynamicDepth_ == 0) {
        // Скобка без пары - некорректные данные, безопасно игнорируем.
        return;
    }

    --dynamicDepth_;
    if (dynamicDepth_ == 0) {
        if (!dynamicBuffer_.empty()) {
            notify(dynamicBuffer_, dynamicStartTime_);
        }
        dynamicBuffer_.clear();
    }
}

void Parser::receiveLine(const std::string& line)
{
    if (line == "{") {
        handleOpenBrace();
    } else if (line == "}") {
        handleCloseBrace();
    } else {
        handleCommand(line);
    }
}

void Parser::receiveEof()
{
    if (dynamicDepth_ > 0) {
        // Незавершённый динамический блок целиком отбрасывается.
        dynamicBuffer_.clear();
        dynamicDepth_ = 0;
    } else {
        flushStatic();
    }
}
