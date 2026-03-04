# Add executable file
add_executable(
    app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

# Link the library to the target
target_link_libraries(
    app
    PRIVATE
    logger
)