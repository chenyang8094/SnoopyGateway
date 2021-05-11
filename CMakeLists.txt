cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

project(SnoopyGateway VERSION 1.0)
set(SnoopyGateway_VERSION 1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")

include_directories(${PROJECT_SOURCE_DIR}/deps/)
include_directories(${PROJECT_SOURCE_DIR}/deps/lib/include)
include_directories(${PROJECT_SOURCE_DIR}/devices)
include_directories(${PROJECT_SOURCE_DIR}/sensor)
include_directories(${PROJECT_SOURCE_DIR}/src)

link_directories(${PROJECT_SOURCE_DIR}/deps/lib/lib)

add_subdirectory(sensor)
add_subdirectory(devices)
add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()