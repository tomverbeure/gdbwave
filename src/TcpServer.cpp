
#include <exception>
#include <iostream>
#include <sstream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "TcpServer.h"

TcpServer::TcpServer(int port) : port(port)
{
    open(port);
}

TcpServer::~TcpServer()
{
    close(socket_fd);
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
        throw runtime_error("Socket creation failed.");
    }

#if 0
    int                 sock_opt = 1;
    int setsockopt_rc = ::setsockopt(
        server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,     // fails on macOS
        &sock_opt, sizeof(sock_opt)
    );

    if (setsockopt_rc) {
        cerr << setsockopt_rc << endl;
        throw runtime_error("setsockopt failed.");
    }
#endif

    sock_addr.sin_family      = AF_INET;
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    sock_addr.sin_port        = htons(this->port);
    if (::bind(server_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0) {
        throw runtime_error("bind failed.");
    }

    cout << "Waiting for connection on port " << this->port << "..." << endl;
    if (::listen(server_fd, 3) < 0) {
        throw runtime_error("listen failed.");
    }

    socket_fd = ::accept(server_fd, (struct sockaddr *)&sock_addr, &sock_addr_len);
    if (socket_fd < 0) {
        throw runtime_error("accept failed.");
    }

    cout << "Connected!" << endl;
}

size_t TcpServer::xmit(const void *buf, size_t len)
{
    // FIXME: how to do error handling?
    int ret = ::send(socket_fd, buf, len, 0);

    return ret;
}

size_t TcpServer::recv(void *buf, size_t buf_size)
{
    int ret = ::read(socket_fd, buf, buf_size);

    return ret;
}



