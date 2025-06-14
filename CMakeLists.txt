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

#################
#     CPACK     #
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
)
set(CPACK_GENERATOR "NSIS")
set(CPACK_PROJECT_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "shuretokki")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")
set(CPACK_PACKAGE_CONTACT "tri.r.utomo@gmail.com")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "${PROJECT_NAME}")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-win64")

set(CPACK_NSIS_DISPLAY_NAME "${PROJECT_NAME}")
set(CPACK_NSIS_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_NSIS_MODIFY_PATH ON)

set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/assets/256x256.ico")
set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/assets/256x256.ico")

set(CPACK_NSIS_CREATE_ICONS_EXTRA
  "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${PROJECT_NAME}.lnk' '$INSTDIR\\\\bin\\\\${PROJECT_NAME}.exe'"
)
set(CPACK_NSIS_DELETE_ICONS_EXTRA
  "Delete '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${PROJECT_NAME}.lnk'"
)

include(CPack)
#     -----     #
################