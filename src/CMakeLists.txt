set(SOURCES
    keyboard.cpp
    editor.cpp
    med.cpp
    terminal.cpp
)
set(VERSION_HEADER_PATH "${CMAKE_CURRENT_BINARY_DIR}/version.hpp")
add_executable(${PROJECT_NAME} ${SOURCES})
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -pedantic
    -Werror=c++11-extensions
    -g
)
if (GIT_FOUND)
    # building from repo
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in" 
        "${VERSION_HEADER_PATH}" 
        @ONLY
    )
else()
    # building from archive
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in" 
        "${VERSION_HEADER_PATH}" 
        COPYONLY
    )
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
    "-DGIT_VERSION_STRING=\"${GIT_VERSION_STRING}\""
)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
