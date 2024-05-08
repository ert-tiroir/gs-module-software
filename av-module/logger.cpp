
#include "av-module/logger.h"
#include <cstring>
#include <string>
#include <iostream>

ModuleLogger::ModuleLogger (CoreTarget* core) {
    this->core = core;
}

void ModuleLogger::append (std::string &buffer) {
    this->string += buffer;
}
void ModuleLogger::append (const char* buffer, int size) {
    std::string str(size, '.');
    for (int i = 0; i < size; i ++) str[i] = buffer[i];

    append(str);
}

void ModuleLogger::flush (LogLevel level) {
    std::string str_level(1, (int) level);
    std::string final_string = str_level + this->string;
    core->write_string_logger(final_string);
    this->string = "";
}

ModuleLogger& operator<< (ModuleLogger& logger, std::string& buffer) {
    logger.append(buffer);
    return logger;
}
ModuleLogger& operator<< (ModuleLogger& logger, const char* buffer) {
    logger.append(buffer, strlen(buffer));
    return logger;
}
ModuleLogger& operator<< (ModuleLogger& logger, int value) {
    std::string str = std::to_string(value);
    logger.append( str );
    return logger;
}
ModuleLogger& operator<< (ModuleLogger& logger, long long value) {
    std::string str = std::to_string(value);
    logger.append( str );
    return logger;
}
ModuleLogger& operator<< (ModuleLogger& logger, LogLevel value) {
    logger.flush( value );
    return logger;
}
