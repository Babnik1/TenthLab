cmake_minimum_required( VERSION 3.12 )

project( async VERSION 1.0 LANGUAGES CXX )

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

set( CMAKE_CXX_STANDARD 20 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

find_package( Threads REQUIRED )

add_library( async SHARED
        src/async.cpp
        src/console_observer.cpp
        src/file_observer.cpp
        src/parser.cpp
)

target_include_directories( async PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries( async PUBLIC Threads::Threads )

set_target_properties( async PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION 1
        PUBLIC_HEADER include/async.h
)

add_executable( async_demo demo/main.cpp )
target_link_libraries( async_demo PRIVATE async )

include( GNUInstallDirs )

install( TARGETS async
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install( TARGETS async_demo
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set( CPACK_PACKAGE_NAME "async" )
set( CPACK_PACKAGE_VERSION ${PROJECT_VERSION} )
set( CPACK_PACKAGE_CONTACT "example@example.com" )
set( CPACK_GENERATOR "DEB" )
set( CPACK_DEBIAN_PACKAGE_MAINTAINER "example@example.com" )
set( CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON )
include( CPack )
