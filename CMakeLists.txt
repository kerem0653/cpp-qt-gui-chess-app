cmake_minimum_required(VERSION 3.16)

project(Chess_Bot LANGUAGES CXX)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Chess_Bot main.cpp
    game.h game.cpp
    board.h board.cpp
    mainwindow.h mainwindow.cpp mainwindow.ui
    resources.qrc
    ruleengine.h ruleengine.cpp



)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(Chess_Bot PRIVATE -O3)
elseif(MSVC)
    target_compile_options(Chess_Bot PRIVATE /O2)
endif()
target_link_libraries(Chess_Bot PRIVATE Qt6::Widgets)
include(GNUInstallDirs)
install(TARGETS Chess_Bot
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
