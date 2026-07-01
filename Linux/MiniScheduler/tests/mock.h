#pragma once

#include <gmock/gmock.h>

class MockLogger {
public:
    MOCK_METHOD(void, OnError, (), ());
};

extern MockLogger *g_mockLogger;
