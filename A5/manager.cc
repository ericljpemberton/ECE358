#include <vector>

#define NEWCONN 0
#define BOUND 1
#define LISTENING 2
#define ACCEPTING 3
#define CONNECTING 4
#define CONNECTED 5
#define RECEIVING 6
#define CLOSED -1

#define milliSecs 1000


class Socket {
    public:
        int sockfd;
        struct sockaddr_in *addr;
        int expectedAck;
        int sequenceNumber;
        int receiving;

        Socket(int sockfd) :  sockfd(sockfd), addr(), expectedAck(0), sequenceNumber(0), receiving(0) {

        }
};


class Manager {
public:
    std::vector<Socket> sockets;

    Manager() {
    }

    int registerRcsConnection(int sockfd) {
        Socket newSock = Socket(sockfd);
        sockets.push_back(newSock);
        return (sockets.size() - 1);
    }
};