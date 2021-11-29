#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <cstddef>

class TcpServer 
{
public:
    TcpServer(int port);
    ~TcpServer();

    void open(int port);
    size_t xmit(const void *buf, size_t len);
    size_t recv(void *buf, size_t buf_size);

private:
    int port;
    int socket_fd; 
};

#endif
