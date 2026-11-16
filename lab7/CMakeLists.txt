cmake_minimum_required(VERSION 3.16)
project(ShapeCollection LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(ShapeCollection
    main.cpp
    shape.h
    widgetcanvas.h
    widgetcanvas.cpp
    shapescene.h
    shapescene.cpp
)

target_link_libraries(ShapeCollection PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(MSVC)
    target_compile_options(ShapeCollection PRIVATE /W4 /utf-8)
else()
    target_compile_options(ShapeCollection PRIVATE -Wall -Wextra -Wpedantic)
endif()
