#include "async.h"

#include <cerrno>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>

// Демонстрационный клиент библиотеки libasync.

int main( int argc, char* argv[] ) 
{
    if ( argc != 3 ) 
    {
        std::cerr << "Usage: <port> <block size>" << std::endl;
        return 1;
    }

    long blockSize = 0;
    int port = 0;
    try 
    {
        blockSize = std::stol( argv[ 1 ] );
        port = std::stoi( argv[ 2 ] );

    } 
    catch ( const std::exception& ) 
    {
        std::cerr << "Error: N must be a positive integer." << std::endl;
        return 1;
    }

    if ( blockSize <= 0 || port <= 0 ) 
    {
        std::cerr << "Error: N must be a positive integer." << std::endl;
        return 1;
    }

    void* ctx = Connect( static_cast< std::size_t >( blockSize ) );

    const std::size_t CHUNK_SIZE = 4096;
    std::vector< char > buffer( CHUNK_SIZE );

    for (;;) 
    {
        ssize_t bytesRead = ::read( STDIN_FILENO, buffer.data(), buffer.size() );
        if ( bytesRead > 0 ) 
        {
            Receive( ctx, buffer.data(), static_cast< std::size_t >( bytesRead ) );
        } 
        else if ( bytesRead == 0 ) 
        {
            break; 
        } 
        else if ( errno != EINTR ) 
        {
            std::cerr << "Error reading stdin." << std::endl;
            break;
        }
    }

    Disconnect( ctx );

    return 0;
}
