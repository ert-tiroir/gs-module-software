
#include "gs-socket.h"

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

int create_vsocket (const char* ip, int port) {
    int status, fd;
    struct sockaddr_in serv_addr;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return - 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return - 1;
    }
 
    if ((status
         = connect(fd, (struct sockaddr*)&serv_addr,
                   sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return - 1;
    }
}

socket_t::socket_t (int module) {
    std::string bf (1, module);
    this->to_send.push(bf);
    this->fd = create_vsocket(IP, PORT);
}

void socket_t::push (std::string buffer) {
    this->to_send.push(buffer);
}
void socket_t::tick () {
    if (to_send.size() == 0) return ;
    send(fd, to_send.front().data(), to_send.front().size(), 0);
    to_send.pop();
}

std::string socket_t::recv () {
    unsigned char sze_buffer[1];
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
    int res = read(fd, sze_buffer, 1);
    if (res <= 0) return "";
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) ^ O_NONBLOCK);
    char buffer[256];
    read(fd, buffer, sze_buffer[0]);

    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

    std::string str(sze_buffer[0], '.');
    for (int i = 0; i < str.size(); i ++) str[i] = buffer[i];
    return str;
}
