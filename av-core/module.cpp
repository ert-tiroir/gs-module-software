
#include "av-core/module.h"
#include "utils/string.h"
#include "utils/consts.h"

extern "C" {
    #include <stdio.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
}

int __close (int fd) { return close(fd); }

void CoreModule::start () {
    mkfifo(path_module_output, 0666);
    mkfifo(path_module_input,  0666);
    mkfifo(path_logger,        0666);

    fd_logger        = open(path_logger, O_RDONLY | O_NONBLOCK);
    fd_module_output = open(path_module_output, O_RDONLY | O_NONBLOCK);

    if (fd_logger == -1 || fd_module_output == -1) {
        this->close();
        return ;
    }
}

void CoreModule::close () {
    if (fd_logger        != -1) __close(fd_logger);
    if (fd_module_input  != -1) __close(fd_module_input);
    if (fd_module_output != -1) __close(fd_module_output);

    fd_logger        = -1;
    fd_module_input  = -1;
    fd_module_output = -1;
}

void CoreModule::handshake () {
    if (fd_module_input != -1) return ;
    if (fd_module_output == -1) return ;

    unsigned int buffer[1];
    int size = read_all_or_nothing(fd_module_output, (char*) buffer, sizeof(unsigned int));
    if (size == 0) return ;

    if (buffer[0] == HANDSHAKE) {
        fd_module_input = open(path_module_input, O_WRONLY | O_NONBLOCK);

        if (fd_module_input == -1) close();
    } 
}

CoreModule::CoreModule (const char* module_name, const char* path_module_output, const char* path_module_input, const char* path_logger) {
    this->path_module_output = path_module_output;
    this->path_module_input  = path_module_input;
    this->path_logger        = path_logger;

    this->module_name = module_name;

    this->start();
}
const char* CoreModule::get_name () {
    return module_name;
}

bool CoreModule::ready () {
    if (fd_module_input == -1)
        handshake();

    return fd_module_input != -1;
}

int CoreModule::read_from_module (char* buffer, int size) {
    if (!ready()) return 0;
    return read(fd_module_output, buffer, size);
}
int CoreModule::read_logger (char* buffer, int size) {
    if (!ready()) return 0;
    return read(fd_logger, buffer, size);
}
int CoreModule::write_to_module (const char* buffer, int size) {
    if (!ready()) return 0;
    return write(fd_module_input, buffer, size);
}

std::string CoreModule::read_string_from_module (bool* valid) {
    if (!ready()) {
        *valid = false;
        return "";
    }

    return read_string(fd_module_output, valid);
}
std::string CoreModule::read_string_logger (bool* valid) {
    if (!ready()) {
        *valid = false;
        return "";
    }

    return read_string(fd_logger, valid);
}
bool CoreModule::write_string_to_module (std::string &buffer) {
    if (!ready()) return false;
    return write_string(fd_module_input, buffer);
}
