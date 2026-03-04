#ifndef __LOGGER_H__
#define __LOGGER_H__

#include<iostream>
#include<string>

class Logging
{
public:
    void log( const std::string& message );
    void warn( const std::string& message );
    void error( const std::string& message );
};

#endif