#include "rcs.h"
#include "ucp.h"
#include "manager.cc"
#include <cstring>
#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <stdio.h>

//used to allocate an RCS socket
//Returns a socket descriptor (positive integer) on success

static Manager manager;

int checksum(char *buf) {
    int i = 0;
    int checksumVal = 0;
    while (buf[i] != '\0') {
        checksumVal += buf[i];
        i++;
    }
    return checksumVal;
}

//used to allocate an RCS socket. No arguments.
//Returns a socket descriptor (positive integer) on success
int rcsSocket()
{
    int sockfd = ucpSocket();
    return manager.registerRcsConnection(sockfd);
}

//binds an RCS socket (first argument) to the address structure (second argument)
//If the port component of the second parameter is specified as 0, the call should choose a port and fill that into the port portion of the second argument
//Returns 0 on success
int rcsBind(int index, struct sockaddr_in *addr)
{
    manager.sockets.at(index).addr = addr;
    return ucpBind(manager.sockets.at(index).sockfd, addr);
}

//fills in the address information into the second argument with which an RCS socket (first argument) has been bound via a call to rcsBind()
//Returns 0 on success
int rcsGetSockName(int index, struct sockaddr_in *addr)
{
    addr->sin_addr.s_addr = manager.sockets.at(index).addr->sin_addr.s_addr;
    return ucpGetSockName(manager.sockets.at(index).sockfd, addr);
}

//marks an RCS socket (the argument) as listening for connection requests
//Returns 0 on success
int rcsListen(int index)
{
    manager.sockets.at(index).receiving = 1;
    return 0;
}

//accepts a connection request on a socket (the first argument)
//This is a blocking call while awaiting connection requests. The call is unblocked when a connection request is received
//The address of the client is filled into the second argument
//Returns a descriptor to a new RCS socket that can be used to rcsSend() and rcsRecv() with the client
int rcsAccept(int index, struct sockaddr_in *addr)
{
    //Test if currently receiving
    if (manager.sockets.at(index).receiving < 1) {
        return -1;
    }

    char buf[8];
    int syn = 0;

    //While we haven't received a syn yet...
    while (syn <= 0 || strcmp(buf, "syn") != 0) {
        syn = ucpRecvFrom(manager.sockets.at(index).sockfd, buf, 4, addr);
    }

    int returnSock = rcsSocket();
    manager.sockets.at(returnSock).addr = addr;

    int synack = 0;
    //Respond with a synack
    while (synack <= 0) {
        synack = ucpSendTo(manager.sockets.at(returnSock).sockfd, "synack", 8, addr);
    }

    int ack = 0;
    //While we haven't received an ack yet
    while (ack <= 0 || strcmp(buf, "ack") != 0) {
        ack = ucpRecvFrom(manager.sockets.at(returnSock).sockfd, buf, 4, addr);
    }

    return returnSock;
}

//connects a client to a server
//The socket (first argument) must have been bound beforehand using rcsBind()
//The second argument identifies the server to which connection should be attempted
//Returns 0 on success
int rcsConnect(int index, const struct sockaddr_in *addr)
{
    char buf[8];
    int syn = ucpSendTo(manager.sockets.at(index).sockfd, "syn", 4, (sockaddr_in*)addr);
    //Send a syn to start the connection

    int synack = 0;
    while(synack <= 0 || strcmp((char *)buf, "synack") != 0) {
        synack = ucpRecvFrom(manager.sockets.at(index).sockfd, buf, 8, (sockaddr_in*)addr);
    }

    int ack = 0;
    while (ack <= 0) {
        ack = ucpSendTo(manager.sockets.at(index).sockfd, "ack", 4, (sockaddr_in*)addr);
    }

    return 0;
}


//blocks awaiting data on a socket (first argument)
//Presumably, the socket is one that has been returned by a prior call to rcsAccept(), or on which rcsConnect() has been successfully called
//The second argument is the buffer which is filled with received data
//The maximum amount of data that may be written is identified by the third argument
//Returns the actual amount of data received. “Amount” is the number of bytes
//Data is sent and received reliably, so any byte that is returned by this call should be what was sent, and in the correct order
int rcsRecv(int index, void *buf, int len)
{
    while(true) {
        int numRec = ucpRecvFrom(manager.sockets.at(index).sockfd, buf, len, manager.sockets.at(index).addr);
        if (numRec <= 0) {
            continue;
        }

        memset(buf + (sizeof(char) * numRec), 0, len-numRec); //Null terminate the data

        //split buf into sequenceNumber, checksum, message
        //Check if sequence number is what we expect:
        char *receivedSequence = strtok((char *)buf, ";");
        if (receivedSequence <= 0) {
            continue;
        }
        int sequenceRec = atoi(receivedSequence);
        if (sequenceRec != manager.sockets.at(index).expectedAck) {
            continue;
        }

        //SequenceNumber passes

        char *checkSum = strtok(0, ";");
        if (checkSum == 0) {
            continue;
        }

        int check = atoi(checkSum);
        //Ensure checksum matches once we receive the data

        char *message = strtok(0, "");
        if (message == 0 || check != checksum(message)) {
            continue;
        }
 
        std::cout << "Got: " << message << std::endl;

        //Message is valid, and in order
        //Now send an ack for it.
        std::stringstream sending;
        sending << manager.sockets.at(index).expectedAck++ << ";";
        sending << checksum((char *)"ack") << ";";
        sending << "ack";

        int numSent = 0;
        while (numSent <= 0) {
            numSent = ucpSendTo(manager.sockets.at(index).sockfd, sending.str().c_str(), strlen(sending.str().c_str()), manager.sockets.at(index).addr);
        }

        return strlen(message);
    }
}

//blocks sending data
//The first argument is a socket descriptor that has been returned by a prior call to rcsAccept(), or on which rcsConnect() has been successfully called
//The second argument is the buffer that contains the data to be sent. The third argument is the number of bytes to be sent
//Returns the actual number of bytes sent
//If rcsSend() returns with a non-negative return value, then we know that so many bytes were reliably received by the other end
int rcsSend(int index, void *buf, int len)
{
    //Wrap the data to send with a checksum and sequence number.
    //uCpSend it
    //Wait for ack
    //If no ack, resend
    //Return the length of the bytes sent
    int numSent = 0;
    int numRec = 0;
    char recv[64];
    char* ack;

    std::stringstream sending;
    sending << manager.sockets.at(index).sequenceNumber++ << ";";
    sending << checksum((char *)buf) << ";";
    sending << (char *)buf;

    //Try to send the message until successful
    numSent = 0;
    while (numSent <= 0) {
        numSent = ucpSendTo(manager.sockets.at(index).sockfd, sending.str().c_str(), strlen(sending.str().c_str()), manager.sockets.at(index).addr);
    }

    while(true) {
        numRec = ucpRecvFrom(manager.sockets.at(index).sockfd, recv, 64, manager.sockets.at(index).addr);
        if(numRec <= 0) {
            //If we fail to receive an ack, resend the message.
            ucpSendTo(manager.sockets.at(index).sockfd, sending.str().c_str(), strlen(sending.str().c_str()), manager.sockets.at(index).addr);
            continue;
        }

        recv[numRec] = 0;

        strtok(recv, ";"); //Sequence number
        strtok(0, ";"); //Checksum
        ack = strtok(0, ";");
        if (ack == 0 || strcmp(ack, "ack") != 0) { //If we don't get an ack, resend
            ucpSendTo(manager.sockets.at(index).sockfd, sending.str().c_str(), strlen(sending.str().c_str()), manager.sockets.at(index).addr);
            continue;
        }

        memset((char *)buf, 0, len);

        return numSent;
    }

    memset((char *)buf, 0, len);
    return numSent;
}

//closes an RCS socket descriptor
//Returns 0 on success.
int rcsClose(int index)
{
    int status = ucpClose(manager.sockets.at(index).sockfd);
    manager.sockets.erase(manager.sockets.begin() + index);
    return status;
}