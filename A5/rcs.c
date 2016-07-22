#include "rcs.h"
#include "manager.cc"

//used to allocate an RCS socket
//Returns a socket descriptor (positive integer) on success

static Manager manager;

//used to allocate an RCS socket. No arguments.
//Returns a socket descriptor (positive integer) on success
int rcsSocket() 
{
    return manager.registerRcsConnection();
}

//binds an RCS socket (first argument) to the address structure (second argument)
//If the port component of the second parameter is specified as 0, the call should choose a port and fill that into the port portion of the second argument
//Returns 0 on success
int rcsBind(int index, struct sockaddr_in *addr) 
{
    manager.changeRcsConnectionState(index, BOUND);
    //TODO: how the fuck do we determine the port?
    //addr->sin_port = 
    manager.registerRcsUcpTarget(index, addr);
    return 0;
}

//fills in the address information into the second argument with which an RCS socket (first argument) has been bound via a call to rcsBind()
//Returns 0 on success
int rcsGetSockName(int index, struct sockaddr_in *addr) 
{
    addr->sin_addr.s_addr = manager.getSockAddrIn().sin_addr.s_addr;
    return 0;
}

//marks an RCS socket (the argument) as listening for connection requests
//Returns 0 on success
int rcsListen(int index)
{
    manager.changeRcsConnectionState(index, LISTENING);
    return 0; 
}

//accepts a connection request on a socket (the first argument)
//This is a blocking call while awaiting connection requests. The call is unblocked when a connection request is received
//The address of the client is filled into the second argument
//Returns a descriptor to a new RCS socket that can be used to rcsSend() and rcsRecv() with the client
int rcsAccept(int index, struct sockaddr_in *addr)
{
    manager.changeRcsConnectionState(index, ACCEPTING);

    while (manager.getRcsConnectionState(index) == ACCEPTING) {
        //TODO: do handshake
        //TODO: should set registered.UcpRcsPairing to be index of far side
    }
    if (manager.getRcsConnectionState(index) != CONNECTED) return -1; //handshake did not succeed
    return index;
}

//connects a client to a server
//The socket (first argument) must have been bound beforehand using rcsBind()
//The second argument identifies the server to which connection should be attempted
//Returns 0 on success
int rcsConnect(int index, const struct sockaddr_in *addr)
{
    manager.changeRcsConnectionState(index, CONNECTING);

    while (manager.getRcsConnectionState(index) == CONNECTING) {
        //TODO: do handshake
    }
    if (manager.getRcsConnectionState(index) != CONNECTING) return -1; //handshake did not succeed
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
    manager.changeRcsConnectionState(index, RECEIVING);
    int bytes = manager.checkMessageQueue(index, buf, len);
    while (bytes < 0) {
        bytes = manager.checkMessageQueue(index, buf, len);
    }

    return bytes;
}

//blocks sending data
//The first argument is a socket descriptor that has been returned by a prior call to rcsAccept(), or on which rcsConnect() has been successfully called
//The second argument is the buffer that contains the data to be sent. The third argument is the number of bytes to be sent
//Returns the actual number of bytes sent
//If rcsSend() returns with a non-negative return value, then we know that so many bytes were reliably received by the other end
int rcsSend(int index, void *buf, int len)
{
    //TODO TODO
    //return (int) send(index, buf, len, 0);
    // ^ illegal
    return 0;
}

//closes an RCS socket descriptor
//Returns 0 on success.
int rcsClose(int index)
{
    //TODO: do handshake
    manager.changeRcsConnectionState(index, CLOSED);
    return 0;
}
