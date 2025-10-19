# src/CMakeLists.txt

# Explicitly list source files (avoids GLOB issues)
set(SOURCES
    keyboard.cpp
    med.cpp
    terminal.cpp
)
set(VERSION_HEADER_PATH "${CMAKE_CURRENT_BINARY_DIR}/version.hpp")
add_executable(${PROJECT_NAME} ${SOURCES})

# Modern way to set compile options on a specific target
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -pedantic
    -g
)
if (GIT_FOUND)
    # Scenario 1: Building from a Git repository.
    # We use CMake's version (GIT_VERSION_STRING) to configure the header.
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in" 
        "${VERSION_HEADER_PATH}" 
        @ONLY
    )
else()
    # Scenario 2: Building from an archive (no .git).
    # We rely on the git archive command having already substituted the format 
    # string in the version.hpp.in file. We just copy it.
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/version.hpp.in" 
        "${VERSION_HEADER_PATH}" 
        COPYONLY
    )
endif()

# FIX: Use GIT_VERSION_STRING instead of GIT_TAG_REPO
target_compile_definitions(${PROJECT_NAME} PRIVATE
    # This is the string that holds the version tag/hash
    "-DGIT_VERSION_STRING=\"${GIT_VERSION_STRING}\""
)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)