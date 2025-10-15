# Explicitly list source files (avoids GLOB issues)
set(SOURCES
    keyboard.cpp
    med.cpp
    terminal.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Modern way to set compile options on a specific target
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -pedantic
    -g
)

# Apply the Git hash to the target
target_compile_definitions(${PROJECT_NAME} PRIVATE
    "-DGIT_COMMIT_HASH=\"${GIT_COMMIT_HASH}\""
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)