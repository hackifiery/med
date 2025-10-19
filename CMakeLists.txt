cmake_minimum_required(VERSION 3.10)

project(med VERSION 0.3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
find_package(Git QUIET)

# --- Initialize Default Version ---
# This ensures GIT_VERSION_STRING is always defined, even if Git is not found.
set(GIT_VERSION_STRING "${PROJECT_VERSION}-unknown")
set(PROJECT_VERSION_STRING_FINAL "${PROJECT_VERSION}-unknown")

if (GIT_FOUND)
    message(STATUS "Git Found.")
    # Fix: Always quote variables in message()
    message(STATUS "Git Executable: \"${GIT_EXECUTABLE}\"")

    # Use a robust git describe command with --always to get the hash if no tags exist
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --tags --always --abbrev=7 --dirty=-dirty
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_VERSION_STRING_TEMP # Use a temporary variable for output
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE GIT_RESULT_CODE         # Variable to hold exit code (0=success)
    )

    # Check the result code of the Git command
    if (GIT_RESULT_CODE EQUAL 0)
        # Success: Set the final variable
        set(GIT_VERSION_STRING "${GIT_VERSION_STRING_TEMP}")
    endif()
    set(PROJECT_VERSION_STRING_FINAL "${GIT_VERSION_STRING}")
    
    # Message the final version used
    message(STATUS "Final Version String set to: ${GIT_VERSION_STRING}")

else()
    message(STATUS "Git not found. Using default version: ${GIT_VERSION_STRING}")
    message(STATUS "No Git repository found. Using static version or archive version.")
endif()

# Add the source subdirectory. GIT_VERSION_STRING is now available to it.
add_subdirectory(src)