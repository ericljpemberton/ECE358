#ifndef RCS_H_
#define RCS_H_

#include <netinet/in.h>

int rcsSocket();                                                //used to allocate an RCS socket
int rcsBind(int sockfd, struct sockaddr_in *addr);              //binds an RCS socket (first argument) to the address structure (second argument)
int rcsGetSockName(int sockfd, struct sockaddr_in *addr);       //provides the address information from which an RCS socket has been bound via a call to rcsBind()
int rcsListen(int sockfd);                                      //marks an RCS socket (the argument) as listening for connection requests
int rcsAccept(int sockfd, struct sockaddr_in *addr);            //accepts a connection request on a socket (the first argument)
int rcsConnect(int sockfd, const struct sockaddr_in *addr);     //connects a client to a server
int rcsRecv(int sockfd, void *buf, int len);                    //blocks awaiting data on a socket
int rcsSend(int sockfd, void *buf, int len);                    //blocks sending data
int rcsClose(int sockfd);                                       //closes an RCS socket descriptor

#endif // RCS_H_
