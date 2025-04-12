cmake_minimum_required(VERSION 3.5)

project(Portefoelje LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Portefoelje main.cpp
    character.h character.cpp
    keyboard.h keyboard.cpp)

include(GNUInstallDirs)
install(TARGETS Portefoelje
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
