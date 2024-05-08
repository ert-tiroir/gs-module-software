#pragma once
#include "av-core/structs.h"
#include <string>

struct CoreModule {
private:
    int fd_module_output = -1;
    int fd_module_input  = -1;
    int fd_logger        = -1;

    const char* path_module_output;
    const char* path_module_input;
    const char* path_logger;

    const char* module_name;

    void start ();
    void close ();
    void handshake ();
public:
    CoreModule (const char* module_name, const char* path_module_output, const char* path_module_input, const char* path_logger);
    const char* get_name ();

    bool ready ();

    int read_logger      (char* buffer, int size);
    int read_from_module (char* buffer, int size);
    int write_to_module  (const char* buffer, int size);

    std::string read_string_from_module (bool* found);
    std::string read_string_logger (bool* found);
    bool write_string_to_module (std::string &buffer);
};
