cmake_minimum_required(VERSION 3.16)
project(PhoneBook LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
if(QT_VERSION_MAJOR EQUAL 5)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Widgets)
else()
    find_package(Qt6 REQUIRED COMPONENTS Widgets)
endif()

add_library(phonebook_core
    contact.h
    validation.h validation.cpp
    contactstore.h contactstore.cpp
)
target_link_libraries(phonebook_core PUBLIC Qt${QT_VERSION_MAJOR}::Core)
target_include_directories(phonebook_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(PhoneBook
    main.cpp
    mainwindow.h mainwindow.cpp
    contactdialog.h contactdialog.cpp
)
target_link_libraries(PhoneBook PRIVATE phonebook_core Qt${QT_VERSION_MAJOR}::Widgets)

foreach(target phonebook_core PhoneBook)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /utf-8)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()

# Проверки включаются отдельно: -DBUILD_TESTING=ON.
option(BUILD_TESTING "Build validation and file tests" OFF)
if(BUILD_TESTING)
    enable_testing()
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)
    add_executable(phonebook_tests tests/tst_phonebook.cpp)
    set_target_properties(phonebook_tests PROPERTIES AUTOMOC ON)
    target_link_libraries(phonebook_tests PRIVATE phonebook_core Qt${QT_VERSION_MAJOR}::Test)
    if(MSVC)
        target_compile_options(phonebook_tests PRIVATE /utf-8)
    endif()
    add_test(NAME phonebook_tests COMMAND phonebook_tests)
endif()
