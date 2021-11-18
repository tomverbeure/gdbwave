#ifndef TCP_SERVER_H
#define TCP_SERVER_H

using namespace std;

class TcpServer 
{
public:
    TcpServer(int port);
    ~TcpServer();

    void open(int port);


private:
    int port;
    int socket_fd; 
};

#endif
