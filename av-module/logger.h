#pragma once
#include "av-module/core.h"

enum LogLevel {
    DEBUG   = 0,
    INFO    = 1,
    SUCCESS = 2,
    WARNING = 3,
    ERROR   = 4
};

struct ModuleLogger {
private:
    CoreTarget* core;
    std::string string = "";
public:
    ModuleLogger (CoreTarget* core);

    void append (std::string &buffer);
    void append (const char* buffer, int size);
    void flush (LogLevel level);
};

ModuleLogger& operator<< (ModuleLogger& logger, std::string& buffer);
ModuleLogger& operator<< (ModuleLogger& logger, const char* buffer);
ModuleLogger& operator<< (ModuleLogger& logger, int value);
ModuleLogger& operator<< (ModuleLogger& logger, long long value);
ModuleLogger& operator<< (ModuleLogger& logger, LogLevel level);
