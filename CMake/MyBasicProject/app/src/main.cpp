#include "logger.h"
#include "config.h"

int main(int argc, char* argv[])
{
    Logging logger;
    logger.log("This is a log.");
    logger.warn("This is a warning.");
    logger.error("This is an error.");

    return 0;
}