#include"logger.h"
#include"config.h"

void Logging::log( const std::string& message )
{
#ifdef VERBOSE_LOG
    std::cout << "[INFO]:";
#endif

#ifdef DEBUG_LOG
    std::cout << "[DEBUG]:";
#endif

    std::cout << message << std::endl;
}

void Logging::warn( const std::string& message )
{
    std::cout << "[WARNING]: " << message << std::endl;
}

void Logging::error( const std::string& message )
{
    std::cout << "[ERROR]: " << message << std::endl;
}