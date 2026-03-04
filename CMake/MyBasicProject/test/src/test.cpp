#include "logger.h"
#include "config.h"

int main( int argc, char* argv[] )
{
    Logging logger;
    logger.log("Test log.");
    logger.warn("Test warning.");
    logger.error("Test error.");

    return 0;
}