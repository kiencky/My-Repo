# Add static library
add_library(
    logger
    STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/logger.cpp
)

# Include header files to the target
target_include_directories(
    logger
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/generate
)

# Set compile feature
target_compile_features(
    logger
    PUBLIC
    cxx_std_17
)

# Add compile macro to the target
if(ENABLE_VERBOSE)
    target_compile_definitions(
        logger
        PUBLIC
        VERBOSE_LOG
    )
endif()

# Add debug macro to the target
target_compile_definitions(
    logger
    PRIVATE
    $<$<CONFIG:Debug>:DEBUG_BUILD>
)