#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <cstddef>

class TcpServer 
{
public:
    TcpServer(int port);
    ~TcpServer();

    void open(int port);
    ssize_t xmit(const void *buf, size_t len);
    ssize_t recv(void *buf, size_t buf_size);

private:
    int server_fd; 
    int socket_fd; 
    int port;
};

#endif
