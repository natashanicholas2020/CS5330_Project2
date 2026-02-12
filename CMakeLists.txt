cmake_minimum_required(VERSION 3.16)
project(Project2App)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(SOURCES
    main.cpp
    Project2Window.cpp
    feature_utils.cpp
    matcher_utils.cpp
)

set(HEADERS
    Project2Window.h
    feature_utils.h
    matcher_utils.h
)

qt_standard_project_setup()

qt_add_executable(Project2App
    MANUAL_FINALIZATION
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(Project2App PRIVATE Qt6::Widgets ${OpenCV_LIBS})

qt_finalize_executable(Project2App)
