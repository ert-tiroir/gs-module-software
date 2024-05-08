
#include "av-core/module.h"

extern "C" {
    #include <stdio.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/stat.h>
    #include <sys/types.h>
}

void CoreModule::start () {
    mkfifo(path_module_output, 0666);
    mkfifo(path_module_input,  0666);
    mkfifo(path_logger,        0666);

    fd_logger = open(path_logger, O_RDONLY)
}

CoreModule::CoreModule (const char* path_module_output, const char* path_module_input, const char* path_logger) {
    this->path_module_output = path_module_output;
    this->path_module_input  = path_module_input;
    this->path_logger        = path_logger;

    this->start();
}

bool CoreModule::ready () {
    
}
