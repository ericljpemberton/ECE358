#ifndef TEMPTEMPTEMPTEMP
#define TEMPTEMPTEMPTEMP


#include <iostream>
#include <cstdlib>
#include <strings.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h> //close
//TODO: trim above

#include <vector>
#include "net_util.h"
#include "ucp.h"
#include "pickIp.h"

#define NEWCONN 0
#define BOUND 1
#define LISTENING 2
#define ACCEPTING 3
#define CONNECTING 4
#define CONNECTED 5
#define RECEIVING 6
#define CLOSED -1

#define milliSecs 1000


struct RcsConnection {
    int status;                     //current of rcs socket
    struct sockaddr_in *UcpTarget;  //target we want to send and receive packets with
    int UcpRcsPairing;              //index of far side

    RcsConnection(int state) {
        status = state;
        UcpRcsPairing = -1;
    }
};

struct Message {
    int indexMessageIsFor;  //index
    void *buf;              //contents of message (no footer)
    int len;                //number of received bytes

    Message(int length) {
        len = length;
    }
};

class Manager {
    int sockfd;                 //UCP Socket for entire client
    struct sockaddr_in addr;    //info for UCP socket binding

    std::vector<RcsConnection> registered;
    std::vector<Message*> queue;

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

        ucpSetSockRecvTimeout(sockfd, milliSecs);

        //TODO: fork a thread to constantly consume messages
        /*
        while (true) {
            receiveMessages();
        }
            */

    }

    void receiveMessages() {
        //TODO 
        int index;
        void *buf;
        int len;
        int bytes = ucpRecvFrom(sockfd, buf, len, registered[index].UcpTarget);
        Message *m = new Message(bytes);
        if (bytes > len); //do something?
        memcpy(m->buf, buf, len);
        queue.push_back(m);
    }

    int checkMessageQueue(int index, void *buf, int len) {
        for (int i = 0; i < queue.size(); ++i) {
            //if len < message.len
            if (queue[i]->indexMessageIsFor == index) {
                changeRcsConnectionState(index, CONNECTED);
                memcpy(buf, queue[i]->buf, queue[i]->len);
                int temp = queue[i]->len;
                queue.erase(queue.begin()+i);
                return temp;
            }
        }
        return -1; //no message in queue
    }

    int registerRcsConnection() {
        //TODO: validate target
        registered.push_back(RcsConnection(NEWCONN));
        return registered.size() - 1;
    }

    void registerRcsUcpTarget(int index, struct sockaddr_in *target) {
        registered[index].UcpTarget = target;
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

};





#endif // ! TEMPTEMPTEMPTEMP
