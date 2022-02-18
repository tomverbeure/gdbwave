
#include <exception>
#include <iostream>
#include <sstream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "TcpServer.h"
#include "Logger.h"

using namespace std;

TcpServer::TcpServer(int port) : port(port)
{
    open(port);
}

TcpServer::~TcpServer()
{
    close(socket_fd);
    close(server_fd);
}

void TcpServer::open(int port)
{
    // Based on testbench from Hazard3 project:
    // https://github.com/Wren6991/Hazard3/blob/c1f17b0b23d7e1a52241663bfb53568c89440f2d/test/sim/openocd/tb.cpp#L98 

    int                 server_fd;
    struct sockaddr_in  sock_addr;
    socklen_t           sock_addr_len = sizeof(sock_addr);

    server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        throw runtime_error("Socket creation failed.");
    }

#if !defined(__APPLE__)
    // Both SO_REUSEADDR and SO_REUSEPORT are required to allow the TCP server to restart again immediately
    // after closing it, but one way or the other, that doesn't work on macOS...
    int sock_opt = 1;
    int setsockopt_rc = ::setsockopt(
        server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,     // fails on macOS
        &sock_opt, sizeof(sock_opt)
    );

    if (setsockopt_rc) {
        cerr << setsockopt_rc << endl;
        perror("setsockopt failed");
        throw runtime_error("setsockopt failed.");
    }
#endif

    sock_addr.sin_family      = AF_INET;
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    sock_addr.sin_port        = htons(this->port);
    if (::bind(server_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0) {
        perror("bind failed");
        throw runtime_error("bind failed.");
    }

    LOG_INFO("Waiting for connection on port %d...", this->port);
    if (::listen(server_fd, 3) < 0) {
        perror("listen failed");
        throw runtime_error("listen failed.");
    }

    socket_fd = ::accept(server_fd, (struct sockaddr *)&sock_addr, &sock_addr_len);
    if (socket_fd < 0) {
        perror("accept failed");
        throw runtime_error("accept failed.");
    }

    LOG_INFO("Connected!");
}

ssize_t TcpServer::xmit(const void *buf, size_t len)
{
#if !defined(__APPLE__)
    ssize_t ret = ::send(socket_fd, buf, len, MSG_NOSIGNAL);
#else
    ssize_t ret = ::send(socket_fd, buf, len, 0);
#endif
    return ret;
}

ssize_t TcpServer::recv(void *buf, size_t buf_size)
{
    ssize_t ret = ::read(socket_fd, buf, buf_size);
    return ret;
}

#if 0
void tcpTest()
{
    TcpServer tcpServer(3333);

    unsigned char rxbuf[256];
    int ret;

    while( (ret = tcpServer.recv(rxbuf, 256)) > 0){
        cout << "ret:" <<  ret << endl;
        if (ret > 0){
            tcpServer.xmit("hhh", 3);
            tcpServer.xmit(rxbuf, ret);
        }
    }

    if (ret == 0){
        cout << "Connection closed..." << endl;
    }
    else{
        cout << "Connection error: " << ret << endl;
    }
}
#endif

