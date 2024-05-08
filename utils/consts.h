#pragma once

const unsigned int HANDSHAKE = 0x42424242;

enum LogLevel {
    DEBUG   = 0,
    INFO    = 1,
    SUCCESS = 2,
    WARNING = 3,
    ERROR   = 4
};

bool lessOrEquals (LogLevel A, LogLevel B);