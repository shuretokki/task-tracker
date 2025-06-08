cmake_minimum_required(VERSION 3.15)

project(
  ctask
  VERSION 1.0
  LANGUAGES CXX)

set(SOURCE_FILES
    src/main.cpp)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
