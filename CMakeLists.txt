cmake_minimum_required(VERSION 3.21)
project(RoboticsTaskTracker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(libpqxx CONFIG REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_library(tracker_core STATIC
    src/storage.cpp
)

target_include_directories(tracker_core PUBLIC src)
target_link_libraries(tracker_core PUBLIC libpqxx::pqxx)

add_executable(RoboticsTaskTracker
    src/main.cpp
    src/console_ui.cpp
)
target_link_libraries(RoboticsTaskTracker PRIVATE tracker_core)