//#include <sys/types.h>
//#include <sys/socket.h>
//#include <unistd.h>

#include "rcs.h"
#include "ucp.h"

//used to allocate an RCS socket
//Returns a socket descriptor (positive integer) on success
int rcsSocket() 
{
    return socket(AF_INET, SOCK_STREAM, 0);
    // ^ illegal
}

//binds an RCS socket (first argument) to the address structure (second argument)
//If the port component of the second parameter is specified as 0, the call should choose a port and fill that into the port portion of the second argument
//Returns 0 on success
int rcsBind(int sockfd, struct sockaddr_in *addr) 
{
    return (ucpBind(sockfd, addr));
}


//fills in the address information into the second argument with which an RCS socket (first argument) has been bound via a call to rcsBind()
//Returns 0 on success
int rcsGetSockName(int sockfd, struct sockaddr_in *addr) 
{
    socklen_t len = (socklen_t)sizeof(struct sockaddr_in);
    return(getsockname(sockfd, (struct sockaddr *)addr, &len));
    // ^ illegal
}

//marks an RCS socket (the argument) as listening for connection requests
//Returns 0 on success
int rcsListen(int sockfd)
{
    return listen(sockfd, 0); 
    // ^ illegal
}

//accepts a connection request on a socket (the first argument)
//This is a blocking call while awaiting connection requests. The call is unblocked when a connection request is received
//The address of the client is filled into the second argument
//Returns a descriptor to a new RCS socket that can be used to rcsSend() and rcsRecv() with the client
int rcsAccept(int sockfd, struct sockaddr_in *addr)
{
    socklen_t len = (socklen_t)sizeof(struct sockaddr_in);
    return(accept(sockfd, (struct sockaddr *)addr, &len));
    // ^ illegal
}

//connects a client to a server
//The socket (first argument) must have been bound beforehand using rcsBind()
//The second argument identifies the server to which connection should be attempted
//Returns 0 on success
int rcsConnect(int sockfd, const struct sockaddr_in *addr)
{
    socklen_t len = (socklen_t)sizeof(struct sockaddr_in);
    return(connect(sockfd, (struct sockaddr *)addr, len));
    // ^ illegal
}


//blocks awaiting data on a socket (first argument)
//Presumably, the socket is one that has been returned by a prior call to rcsAccept(), or on which rcsConnect() has been successfully called
//The second argument is the buffer which is filled with received data
//The maximum amount of data that may be written is identified by the third argument
//Returns the actual amount of data received. “Amount” is the number of bytes
//Data is sent and received reliably, so any byte that is returned by this call should be what was sent, and in the correct order
int rcsRecv(int sockfd, void *buf, int len)
{
    return (int) recv(sockfd, buf, len, 0);
    // ^ illegal
}


//blocks sending data
//The first argument is a socket descriptor that has been returned by a prior call to rcsAccept(), or on which rcsConnect() has been successfully called
//The second argument is the buffer that contains the data to be sent. The third argument is the number of bytes to be sent
//Returns the actual number of bytes sent
//If rcsSend() returns with a non-negative return value, then we know that so many bytes were reliably received by the other end
int rcsSend(int sockfd, void *buf, int len)
{
    return (int) send(sockfd, buf, len, 0);
    // ^ illegal
}

//closes an RCS socket descriptor
//Returns 0 on success.
int rcsClose(int sockfd)
{
    return close(sockfd); 
    // ^ illegal
}
