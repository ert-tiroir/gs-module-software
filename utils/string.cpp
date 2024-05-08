
#include "utils/string.h"
#include "utils/consts.h"
#include <unistd.h>

int read_all (int fd, char* buffer, int size) {
    int offset = 0;
    while (offset != size) {
        int res = read(fd, buffer + offset, size - offset);
        if (res < 0) continue ;

        offset += res;
    }
    return size;
}
int read_all_or_nothing (int fd, char* buffer, int size) {
    int offset = read(fd, buffer, size);
    if (offset <= 0) return 0;

    int res = read_all(fd, buffer + offset, size - offset);
    if (res < 0) return 0;
    return size;
}
int write_all (int fd, char* buffer, int size) {
    int offset = 0;
    while (offset != size) {
        int res = write(fd, buffer + offset, size - offset);
        if (res < 0) return res;

        offset += res;
    }
    return size;
}
int write_all_or_nothing (int fd, char* buffer, int size) {
    int offset = write(fd, buffer, size);
    if (offset <= 0) return 0;

    int res = write_all_or_nothing(fd, buffer + offset, size - offset);
    if (res < 0) return res;

    return size;
}

std::string read_string (int fd, bool* found) {
    *found = false;
    
    unsigned int size_buffer[1];
    if (read_all_or_nothing(fd, (char*) size_buffer, sizeof(unsigned int)) == 0)
        return "";
    if (size_buffer[0] == HANDSHAKE) return "";
    if (size_buffer[0] > HANDSHAKE) size_buffer[0] --;

    *found = true;
    std::string str(size_buffer[0], '.');

    read_all(fd, str.data(), size_buffer[0]);
    return str;
}
bool write_string (int fd, std::string &str) {
    unsigned int size = str.size();
    if (size >= HANDSHAKE) size ++;
    unsigned int size_buffer[1] = { size };
    int res = write_all_or_nothing(fd, (char*) size_buffer, sizeof(unsigned int));

    if (res <= 0) return false;

    res = write_all(fd, str.data(), str.size());
    if (res <= 0) return false;

    return true;
}
