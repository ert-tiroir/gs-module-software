
#include "av-module/logger.h"
#include <cstring>
#include <string>
#include <iostream>

ModuleLogger::ModuleLogger (CoreTarget* core) {
    this->core = core;
}

void ModuleLogger::append (std::string &buffer) {
    std::cout << buffer << "\n";
    this->string += buffer;
}
void ModuleLogger::append (const char* buffer, int size) {
    std::cout << buffer << " " << size << "\n";
    std::string str(size, '.');
    std::cout << str << "\n";
    for (int i = 0; i < size; i ++) str[i] = buffer[i];
    std::cout << str << "\n";

    append(str);
}

void ModuleLogger::flush (LogLevel level) {
    core->write_string_logger(this->string);
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
