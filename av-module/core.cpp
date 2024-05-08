
#include "av-module/core.h"
#include "utils/string.h"
#include "utils/consts.h"

extern "C" {
    #include <stdio.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
}

const auto __close = close;

const int MAX_RETRY = 10;

void CoreTarget::start () {
    fd_logger        = open(path_logger, O_WRONLY | O_NONBLOCK);
    fd_module_output = open(path_module_output, O_WRONLY | O_NONBLOCK);
    fd_module_input  = open(path_module_input, O_RDONLY | O_NONBLOCK);

    if (fd_logger == -1 || fd_module_input == -1 || fd_module_output == -1) {
        close();
        return ;
    }

    unsigned int buffer[1];
    buffer[0] = HANDSHAKE;

    write_all(fd_module_output, (char*) buffer, sizeof(unsigned int));
}

void CoreTarget::close () {
    if (fd_logger        != -1) __close(fd_logger);
    if (fd_module_input  != -1) __close(fd_module_input);
    if (fd_module_output != -1) __close(fd_module_output);

    fd_logger        = -1;
    fd_module_input  = -1;
    fd_module_output = -1;
}

CoreTarget::CoreTarget (const char* path_module_output, const char* path_module_input, const char* path_logger) {
    this->path_module_output = path_module_output;
    this->path_module_input  = path_module_input;
    this->path_logger        = path_logger;

    this->start();
}

bool CoreTarget::ready () {
    return fd_logger        != -1
        && fd_module_input  != -1
        && fd_module_output != -1;
}

int CoreTarget::read_from_core (char* buffer, int size) {
    if (!ready()) return 0;
    return read(fd_module_input, buffer, size);
}
int CoreTarget::write_logger (const char* buffer, int size) {
    if (!ready()) return 0;
    return write(fd_logger, buffer, size);
}
int CoreTarget::write_to_core (const char* buffer, int size) {
    if (!ready()) return 0;
    return write(fd_module_output, buffer, size);
}

std::string CoreTarget::read_string_from_core (bool* valid) {
    if (!ready()) {
        *valid = false;
        return "";
    }

    return read_string(fd_module_input, valid);
}
bool CoreTarget::write_string_to_core (std::string &buffer) {
    if (!ready()) return false;
    return write_string(fd_module_output, buffer);
}
bool CoreTarget::write_string_logger (std::string &buffer) {
    if (!ready()) return false;
    return write_string(fd_logger, buffer);
}

