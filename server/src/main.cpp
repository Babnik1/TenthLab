/// @file main.cpp
///
/// @brief Точка входа bulk_server.
///
/// Порядок запуска: bulk_server <port> <bulk_size>
///

#include <cstdlib>
#include <iostream>
#include <string>

#include <boost/asio.hpp>

#include "server.h"

int main( int argc, char* argv[] )
{
    if ( argc != 3 )
    {
        std::cerr << "Usage: bulk_server <port> <bulk_size>" << std::endl;
        return 1;
    }

    int port = 0;
    long bulkSize = 0;
    try
    {
        port = std::stoi( argv[ 1 ] );
        bulkSize = std::stol( argv[ 2 ] );
    }
    catch ( const std::exception& )
    {
        std::cerr << "Error: port and bulk_size must be positive integers." << std::endl;
        return 1;
    }

    if ( port <= 0 || port > 65535 || bulkSize <= 0 )
    {
        std::cerr << "Error: port and bulk_size must be positive integers." << std::endl;
        return 1;
    }

    try
    {
        boost::asio::io_context ioContext;
        Server server( ioContext, static_cast< unsigned short >( port ), static_cast< std::size_t >( bulkSize ) );

        ioContext.run();
    }
    catch ( const std::exception& e )
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
