cmake_minimum_required(VERSION 3.15)

project(
  ctask
  VERSION 1.1
  LANGUAGES CXX)

set(SOURCE_FILES
    src/main.cpp)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(${PROJECT_NAME} 
  ${CMAKE_SOURCE_DIR}/assets/pkg.rc            
  ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES
                      # RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
                      CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED ON
)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
