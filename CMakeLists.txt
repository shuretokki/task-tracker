cmake_minimum_required(VERSION 3.15)

project(
  task-tracker
  VERSION 1.0
  LANGUAGES CXX)

set(SOURCE_FILES
    src/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)
