#ifndef TEMPTEMPTEMPTEMP
#define TEMPTEMPTEMPTEMP

#include <vector>

#include <iostream>
#include <cstdlib>
#include <strings.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h> //close

#include "net_util.h"
#include "ucp.h"
#include "pickIp.h"

#define NEWCONN 0
#define BOUND 1
#define LISTENING 1
#define ACCEPTING 1
#define CONNECTING 2
#define CONNECTED 2
#define RECEIVING 2
#define CLOSED -1
//TODO: make unique

struct RcsConnection {
    int status;
    int UcpIP;
    int UcpPort;
    int UcpRcsPairing;

    RcsConnection(int s) {
        status = s;
        UcpIP = UcpPort = UcpRcsPairing = -1;
    }
};

class Manager {
    int sockfd;
    struct sockaddr_in addr;

    std::vector<RcsConnection> registered;

public:
    Manager() {

        sockfd = ucpSocket();
        if (sockfd < 0) {
            //perror("ERROR opening socket");
            exit(1);
        }
        bzero((char *) &addr, sizeof(addr));
        addr.sin_family = AF_INET;
        if (pickServerIPAddr(&addr.sin_addr) < 0) {
            //perror("ERROR picking ip");
            exit(1);
        }
        if (ucpBind(sockfd, &addr) < 0) {
            //perror("ERROR on binding");
            exit(1);
        }
    }

    int registerRcsConnection() {
        registered.push_back(RcsConnection(NEWCONN));
        return registered.size() - 1;
    }


    int getUcpSocket() {
        return sockfd;
    }

    struct sockaddr_in getSockAddrIn() {
        return addr;
    }

    void changeRcsConnectionState (int index, int state) {
        registered[index].status = state;
    }

    int getRcsConnectionState(int index) {
        return registered[index].status;
    }

    int getRcsConnectionPairing(int index) {
        return registered[index].UcpRcsPairing;
    }


};





#endif // ! TEMPTEMPTEMPTEMP
