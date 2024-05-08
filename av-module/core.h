#pragma once
#include <string>

struct CoreTarget {
private:
    int fd_module_output = -1;
    int fd_module_input  = -1;
    int fd_logger        = -1;

    const char* path_module_output;
    const char* path_module_input;
    const char* path_logger;

    void start ();
    void close ();
public:
    CoreTarget () = default;
    CoreTarget (const char* path_module_output, const char* path_module_input, const char* path_logger);

    bool ready ();

    int write_logger   (const char* buffer, int size);
    int write_to_core  (const char* buffer, int size);
    int read_from_core (char* buffer, int size);

    bool write_string_to_core (std::string &buffer);
    bool write_string_logger (std::string &buffer);
    std::string read_string_from_core (bool *found);
};
