
#include <string>
#include <queue>

#define IP "127.0.0.1"
#define PORT 5042
#define CAMPORT 5420

struct socket_t {
    int fd;
    std::queue<std::string> to_send;

    socket_t (int module);
    void push (std::string str);
    void tick ();
    std::string recv ();
};

int create_vsocket (const char* ip, int port);
