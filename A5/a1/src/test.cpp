#include <arpa/inet.h>
#include <netdb.h>

#include <iostream>
#include <cstdlib>
#include <unistd.h>

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[]) {
   int sockfd;
   sockaddr_in dest_addr;   // will hold the destination addr

   sockfd = socket(PF_INET, SOCK_STREAM, 0); // do some error checking!

   dest_addr.sin_family = AF_INET;          // host byte order
   dest_addr.sin_port = htons(atoi(argv[2]));   // short, network byte order
   dest_addr.sin_addr.s_addr = inet_addr(argv[1]);
   memset(&(dest_addr.sin_zero), '\0', 8);  // zero the rest of the struct

   // don't forget to error check the connect()!
   if (connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr)) < 0) {
     exit(-1);
   } 

   char *msg = "Dicks!\0";
   int len, bytes_sent;
   len = strlen(msg);
   bytes_sent = send(sockfd, msg, len, 0);

   close(sockfd);
    // ...
}
