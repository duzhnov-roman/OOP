cmake_minimum_required(VERSION 3.16)
project(HelloWorldApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_executable(HelloWorldApp
    main.cpp
    mywidget.cpp
    mywidget.h
)

target_link_libraries(HelloWorldApp PRIVATE Qt6::Widgets)