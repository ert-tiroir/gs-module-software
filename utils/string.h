
#include <string>


int read_all (int fd, char* buffer, int size);
int read_all_or_nothing (int fd, char* buffer, int size);
int write_all (int fd, char* buffer, int size);
int write_all_or_nothing (int fd, char* buffer, int size);

std::string read_string (int fd, bool* found);
bool write_string (int fd, std::string &str);
