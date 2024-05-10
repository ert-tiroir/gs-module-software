
#include <string>
#include <queue>

struct socket_t {
    int fd;
    std::queue<std::string> to_send;

    socket_t (int module);
    void push (std::string str);
    void tick ();
    std::string recv ();
};

socket_t create_socket (int module);
void push_data (socket_t* socket, std::string str);
std::string read(socket_t* socket);
