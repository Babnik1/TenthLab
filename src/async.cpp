/// @file async.cpp
///
/// @brief Внешний интерфейс библиотеки libasync.
///

#include "async.h"

#include <condition_variable>
#include <cstddef>
#include <ctime>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "console_observer.h"
#include "file_observer.h"
#include "iobserver.h"
#include "parser.h"

namespace {

/// @brief Один завершённый (статический или динамический) блок команд, готовый к выводу.
struct Block 
{
    std::vector< std::string > commands;
    std::time_t timestamp;
};

/// @brief Простая потокобезопасная очередь с ожиданием.
template < typename T >
class BlockingQueue 
{
public:
    void push( T value ) 
    {
        {
            std::lock_guard<std::mutex> lock( mtx_ );
            queue_.push( std::move( value ) );
        }
        cv_.notify_one();
    }

    bool pop( T& value ) 
    {
        std::unique_lock< std::mutex > lock( mtx_ );
        cv_.wait( lock, [this] { return stopped_ || !queue_.empty(); } );
        if ( queue_.empty() ) 
        {
            return false;
        }
        value = std::move( queue_.front() );
        queue_.pop();
        return true;
    }

    void stop() {
        {
            std::lock_guard< std::mutex > lock( mtx_ );
            stopped_ = true;
        }
        cv_.notify_all();
    }

private:
    std::queue< T > queue_;
    std::mutex mtx_;
    std::condition_variable cv_;
    bool stopped_ = false;
};

/// @brief Наблюдатель, которым подписывается Parser каждого контекста.
/// @details Сам ничего не выводит - просто передаёт готовый блок в общий пул
///          фоновых потоков библиотеки.
class QueueingObserver : public IObserver 
{
public:
    void onBlock( const std::vector< std::string >& commands, std::time_t timestamp ) override;
};

/// @brief Обработчик потоков.
/// @details  Владеет тремя дополнительными потоками программы (сверх того
///           потока, из которого вызывают Connect/Receive/Disconnect):
///            - log            - один поток, печатает блоки в консоль;
///            - file1 / file2  - два потока, разбирают ОДНУ общую очередь
///            блоков и пишут их в файлы; какой из двух блок обработает,
///            заранее не определено - оба забирают из одной очереди, 
///            вместо ручного деления на чётные и нечётные команды.
class WorkerManager 
{
public:
    static WorkerManager& instance() 
    {
        static WorkerManager manager;
        return manager;
    }

    void submit( std::shared_ptr< const Block > block ) 
    {
        logQueue_.push( block );
        fileQueue_.push( std::move( block ) );
    }

    WorkerManager( const WorkerManager& ) = delete;
    WorkerManager& operator=( const WorkerManager& ) = delete;

private:
    WorkerManager()
        : logThread_( [ this ] { logLoop(); } )
        , fileThread1_( [ this ] { fileLoop(); } )
        , fileThread2_( [ this ] { fileLoop(); } )
    {}

    ~WorkerManager() 
    {
        logQueue_.stop();
        fileQueue_.stop();
        logThread_.join();
        fileThread1_.join();
        fileThread2_.join();
    }

    void logLoop() 
    {
        std::shared_ptr< const Block > block;
        while ( logQueue_.pop( block ) ) 
        {
            consoleObserver_.onBlock( block->commands, block->timestamp );
        }
    }

    void fileLoop() 
    {
        std::shared_ptr< const Block > block;
        while ( fileQueue_.pop( block ) ) 
        {
            fileObserver_.onBlock( block->commands, block->timestamp );
        }
    }

    BlockingQueue< std::shared_ptr< const Block > > logQueue_;
    BlockingQueue< std::shared_ptr< const Block > > fileQueue_;

    ConsoleObserver consoleObserver_;
    FileObserver fileObserver_;

    std::thread logThread_;
    std::thread fileThread1_;
    std::thread fileThread2_;
};

void QueueingObserver::onBlock( const std::vector<std::string>& commands, std::time_t timestamp ) 
{
    auto block = std::make_shared< Block >( Block{ commands, timestamp } );
    WorkerManager::instance().submit( std::move( block ) );
}

} // namespace

/// @brief Непрозрачный для внешнего кода контекст одного "подключения".
/// @details Внешний код видит его только как void*, поэтому определение
///          целиком находится в .cpp и не тянет parser.h/iobserver.h в
///          публичный async.h.
struct Context 
{
    Parser parser;
    std::string accumulator;
    std::mutex mtx;
    std::shared_ptr< QueueingObserver > observer;

    explicit Context( std::size_t blockSize )
        : parser( blockSize )
        , observer( std::make_shared< QueueingObserver >() )
    {
        parser.subscribe( observer );
    }
};

void* Connect( std::size_t blockSize ) 
{
    return new Context( blockSize );
}

void Receive( void* context, char* buf, std::size_t len ) 
{
    Context* ctx = static_cast< Context* >( context );
    std::lock_guard< std::mutex > lock( ctx->mtx );

    ctx->accumulator.append( buf, len );

    std::size_t pos;
    while ( ( pos = ctx->accumulator.find( '\n' ) ) != std::string::npos ) 
    {
        std::string cmd = ctx->accumulator.substr( 0, pos );
        ctx->accumulator.erase( 0, pos + 1 );
        ctx->parser.receiveLine( cmd );
    }
}

void Disconnect( void* context ) 
{
    Context* ctx = static_cast< Context* >( context );
    {
        std::lock_guard< std::mutex > lock( ctx->mtx );
        ctx->parser.receiveEof();
    }
    delete ctx;
}
