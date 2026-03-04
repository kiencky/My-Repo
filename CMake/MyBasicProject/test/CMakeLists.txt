# Create excutable file
add_executable(
    testlog
    ${CMAKE_CURRENT_SOURCE_DIR}/src/test.cpp
)

# Link library to the target
target_link_libraries(
    testlog
    PRIVATE
    logger
)

# Define test program
add_test(
    NAME
    LoggerTest
    COMMAND
    testlog
)