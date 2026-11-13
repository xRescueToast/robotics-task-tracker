cmake_minimum_required(VERSION 3.21)
project(RoboticsTaskTracker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

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

add_executable(RoboticsTaskTrackerGui
    resources/resources.qrc
    src/gui/gui_main.cpp
    src/gui/main_window.cpp
    src/gui/main_window.h
    src/gui/add_task_page.cpp
    src/gui/add_task_page.h
    src/gui/add_mission_page.cpp
    src/gui/add_mission_page.h
    src/gui/tree_item_delegate.cpp
    src/gui/tree_item_delegate.h
    src/gui/tree_roles.h
)

set_target_properties(RoboticsTaskTrackerGui PROPERTIES WIN32_EXECUTABLE ON)

target_link_libraries(RoboticsTaskTrackerGui PRIVATE tracker_core Qt6::Widgets)

add_custom_command(TARGET RoboticsTaskTrackerGui POST_BUILD
    COMMAND "${Qt6_DIR}/../../../bin/windeployqt.exe" --no-translations
            "$<TARGET_FILE:RoboticsTaskTrackerGui>"
)