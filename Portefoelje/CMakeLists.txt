cmake_minimum_required(VERSION 3.5)

project(Portefoelje LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SQLite3 REQUIRED)

add_executable(Portefoelje main.cpp
    keyboard.h keyboard.cpp
    character.h character.cpp
    monster.h monster.cpp
    SQLdatabase.h SQLdatabase.cpp
    world.h world.cpp
    caves.h caves.cpp
    monsterfactory.h monsterfactory.cpp
    cavefactory.h cavefactory.cpp)

if (SQLite3_FOUND)
include_directories(${SQLite3_INCLUDE_DIRS})
target_link_libraries(Portefoelje ${SQLite3_LIBRARIES})
endif()

include(GNUInstallDirs)
install(TARGETS Portefoelje
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
