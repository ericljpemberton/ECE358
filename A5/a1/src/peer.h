#ifndef PEER
#define PEER

#include "contentStructure.h"
#include "mybind.h"
#include "pickIp.h"
#include "operations.h"

#include <iostream>
#include <cstdlib>
#include <strings.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h> //close

/*
struct addrinfo {
   int              ai_flags;
   int              ai_family;
   int              ai_socktype;
   int              ai_protocol;
   socklen_t        ai_addrlen;
   struct sockaddr *ai_addr;
   char            *ai_canonname;
   struct addrinfo *ai_next;
};

int getaddrinfo(const char *node, const char *service,
                const struct addrinfo *hints,
                struct addrinfo **res);

void freeaddrinfo(struct addrinfo *res);

const char *gai_strerror(int errcode);

*/

class Peer {
private:
   //std::vector<Peer*> peerlist;
   ContentStructure container;

   int sockfd, numPeers;
   unsigned int numContent, nextId;

   sockaddr_in my_server_info;
   sockaddr_in leftPeer;
   sockaddr_in rightPeer;

   void init() {
      // Initialize the network "addpeer"
      sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (sockfd < 0) {
         perror("ERROR opening socket");
         exit(1);
      }
      bzero((char *) &my_server_info, sizeof(my_server_info));
      my_server_info.sin_family = AF_INET;
      if (pickServerIPAddr(&my_server_info.sin_addr) < 0) {
         perror("ERROR picking ip");
         exit(1);
      }
      if (mybind(sockfd, &my_server_info) < 0) {
         perror("ERROR on binding");
         exit(1);
      }

      leftPeer = rightPeer = my_server_info;
   }

   void removePeerCommand(std::string c) {
      //ship our content off to left
      //TODO: LET US BALANCE THE CONTENTS

      exit(0);
   }

   void addContentCommand(std::string c) {
      int id = nextId++;
      numContent++;
      container.addContent(c, id);
      std::cout << id << std::endl; 

      //check if only peer
      if (my_server_info.sin_addr.s_addr == leftPeer.sin_addr.s_addr && my_server_info.sin_port == leftPeer.sin_port) {
         return;
      }
      //connect to left
      int sockfd = socket(PF_INET, SOCK_STREAM, 0);
      if (connect(sockfd, (struct sockaddr *)&leftPeer, sizeof(struct sockaddr)) < 0) {
         std::cerr << "Error: updating neighbour's counts" << std::endl;
         exit(-1);
      }
      //build and send "update numbers" message
      char msg [11 + sizeof(sockaddr_in)] = {CHANGE_NUMPEERS_CONTENT, ':', '\0'};
      memcpy(&msg[2], &my_server_info, sizeof(sockaddr_in));
      memcpy(&msg[2 + sizeof(sockaddr_in)], &nextId, sizeof(nextId));
      memcpy(&msg[6 + sizeof(sockaddr_in)], &numContent, sizeof(numContent));
      msg[10 + sizeof(my_server_info)] = '\0';
      int bytes_sent = send(sockfd, msg, sizeof(msg), 0); //error check?
      close(sockfd);
   }

   void removeContentCommand(int uniqueId) {
      if (container.removeContent(uniqueId) != 0) {
         //check if left peer was the one contacted by god, if not
         //ask left peer if they have it via non-god message
         //if left peer was contacted by god,
         std::cout << "Error: no such content" << std::endl;
      }
      else {
         numContent--;

         //check if only peer
         if (my_server_info.sin_addr.s_addr == leftPeer.sin_addr.s_addr && my_server_info.sin_port == leftPeer.sin_port) {
            return;
         }
         //connect to left
         int sockfd = socket(PF_INET, SOCK_STREAM, 0);
         if (connect(sockfd, (struct sockaddr *)&leftPeer, sizeof(struct sockaddr)) < 0) {
            std::cerr << "Error: updating neighbour's counts" << std::endl;
            exit(-1);
         }

         //build and send "update numbers" message
         char msg [11 + sizeof(sockaddr_in)] = {CHANGE_NUMPEERS_CONTENT, ':', '\0'};
         memcpy(&msg[2], &my_server_info, sizeof(my_server_info));
         memcpy(&msg[2 + sizeof(sockaddr_in)], &nextId, sizeof(nextId));
         memcpy(&msg[6 + sizeof(sockaddr_in)], &numContent, sizeof(numContent));
         msg[10 + sizeof(my_server_info)] = '\0';
         int bytes_sent = send(sockfd, msg, sizeof(msg), 0); //error check?
         close(sockfd);
      }
   }

   //msg format - "l:{sockaddr}:key"
   void nongodLookupContentCommand(char *msg) {
      int uniqueId;
      memcpy(&uniqueId, &msg[3 + sizeof(my_server_info)], sizeof(int));
      std::string content = container.lookupContent(uniqueId);
      if (content != "") {
         std::cout << content << std::endl;
         return;
      }

      //check if left is god-touched
      sockaddr_in godTouched;
      memcpy(&godTouched, &msg[2], sizeof(sockaddr_in));
      if (godTouched.sin_addr.s_addr == leftPeer.sin_addr.s_addr && godTouched.sin_port == leftPeer.sin_port) {
         std::cout << "Error: no such content" << std::endl;
         return;
      }

      //connect to left
      int sockfd = socket(PF_INET, SOCK_STREAM, 0);
      if (connect(sockfd, (struct sockaddr *)&leftPeer, sizeof(struct sockaddr)) < 0) {
         std::cerr << "Error: updating neighbours, could not find left" << std::endl;
         exit(-1);
      }

      //build and send "nongod" lookup message
      int bytes_sent = send(sockfd, msg, sizeof(msg), 0); //error check?
      close(sockfd);
   }

   void lookupContentCommand(int uniqueId) {
      std::string content = container.lookupContent(uniqueId);
      if (content != "") {
         std::cout << content << std::endl;
         return;
      }

      //check if i am only peer
      if (my_server_info.sin_addr.s_addr == leftPeer.sin_addr.s_addr && my_server_info.sin_port == leftPeer.sin_port) {
         std::cout << "Error: no such content" << std::endl;
         return;
      }

      //connect to left
      int sockfd = socket(PF_INET, SOCK_STREAM, 0);
      if (connect(sockfd, (struct sockaddr *)&leftPeer, sizeof(struct sockaddr)) < 0) {
         std::cerr << "Error: updating neighbours, could not find left" << std::endl;
         exit(-1);
      }

      //build and send "nongod" lookup message
      char c[512] = {NONGOD_LOOKUP_CONTENT, ':', '\0'} ;
      memcpy(&c[2], &(my_server_info), sizeof(my_server_info));
      c[2 + sizeof(my_server_info)] = ':';
      memcpy(&c[3 + sizeof(my_server_info)], &uniqueId, sizeof(int));
      c[7 + sizeof(my_server_info)] = '\0';
      int bytes_sent = send(sockfd, c, sizeof(c), 0); //error check?
      close(sockfd);
   }

   //move

   //this is a message from another peer saying "set me as your x"
   //parse the void* for sockaddr_in data, save as left or right
   //return to that peer saying what your old info was
   void changeNeighbourCommand(int which, char *peerData, int senderSocket) {
      //send "old info" message
      char response[8 + sizeof(sockaddr_in)];
      memcpy(&response[0], which ? &rightPeer : &leftPeer, sizeof(sockaddr_in));
      memcpy(&response[sizeof(sockaddr_in)], &nextId, sizeof(nextId));
      memcpy(&response[sizeof(sockaddr_in) + 4], &numContent, sizeof(numContent));

      unsigned long addr = 0;
      unsigned short port = 0;
      memcpy(&addr, &peerData[0], sizeof(addr));
      memcpy(&port, &peerData[5], sizeof(port));

      int bytes_sent = send(senderSocket, (char *)response, sizeof(sockaddr_in) + 8, 0); //error check?
      close(senderSocket);

      //store their info
      if (which == LEFT) {
         leftPeer.sin_port = port;
         leftPeer.sin_addr.s_addr = addr;
      }
      else {
         rightPeer.sin_port = port;
         rightPeer.sin_addr.s_addr = addr;
      }
   }

   void nongodChangeNumPeersCommand(char *msg) {
      memcpy(&nextId, &msg[2 + sizeof(sockaddr_in)], sizeof(nextId));
      memcpy(&numContent, &msg[6 + sizeof(sockaddr_in)], sizeof(numContent));

      //check if left is god-touched
      sockaddr_in godTouched;
      memcpy(&godTouched, &msg[2], sizeof(sockaddr_in));
      if (godTouched.sin_addr.s_addr == leftPeer.sin_addr.s_addr && godTouched.sin_port == leftPeer.sin_port) {
         return;
      }

      //connect to left
      int sockfd = socket(PF_INET, SOCK_STREAM, 0);
      if (connect(sockfd, (struct sockaddr *)&leftPeer, sizeof(struct sockaddr)) < 0) {
         std::cerr << "Error: updating neighbours, could not find left" << std::endl;
         exit(-1);
      }

      //build and send "nongod" lookup message
      int bytes_sent = send(sockfd, msg, sizeof(msg), 0); //error check?
      close(sockfd);
   }

   int executeCommand(char *message, int senderSocket) {

      // std::string content = message.substr(2);

      switch(message[0]) {
         case ALLKEYS: // 'a'
            //std::cout << "Output all keys at node";
            for (int i = 0; i < container.s.size(); ++i) {
               std::cout << container.s[i]->unique_id << std::endl;
            }
            break;

         case ADD_PEER: //'0'
            std::cout << "Adding a peer to the network";
            break;

         case REMOVE_PEER: // '1'
            removePeerCommand(std::string(message).substr(2));
            break;

         case ADD_CONTENT: // '2'
            addContentCommand(std::string(message).substr(2));
            break;

         case REMOVE_CONTENT: // '3'
            removeContentCommand(atoi(&message[2]));
            break;

         case LOOKUP_CONTENT: // '4'
            lookupContentCommand(atoi(&message[2]));
            break;

         case MOVE_CONTENT: // '5'
            std::cout << "Moving content across the network";
            break;

         case CHANGE_NUMPEERS_CONTENT: // '6'
            nongodChangeNumPeersCommand(message);
            break;

         case CHANGE_NEIGHBOUR: // '7'
            changeNeighbourCommand(message[2]-'0', &message[4], senderSocket);
            break;

         case NONGOD_LOOKUP_CONTENT: // 'l'
            nongodLookupContentCommand(message);
            break;

         default:
            std::cerr << "Message: " << message << std::endl;
            exit(0);
      }
   }

   //this method is called after creation by the new processes
   //when we receive a kill command, we run off the end of this method and the process dies
   void run() {

      int newSocket;
      sockaddr_in remoteAddress;
      unsigned int remoteAddressLength = sizeof(sockaddr_in);
      int count = 0 , byte_count;
      char buffer[512] = {'\0'};

      listen(sockfd, 10);
      while (true) {
         newSocket = accept(sockfd, (sockaddr *)&remoteAddress, &remoteAddressLength);
         if(recv(newSocket, buffer, sizeof(buffer), 0) != 0) {
std::cout << "Received command: " << buffer << std::endl;
            executeCommand(buffer, newSocket);
            memset(buffer, '\0', 512);
         }
      }
   }

public:
   Peer() {
      init();
      std::cout << inet_ntoa(my_server_info.sin_addr) << " " << ntohs(my_server_info.sin_port) << std::endl;

      numPeers = 1;
      numContent = nextId = 0;
      //TODO: print here? return as msg?
      run();
   }

   //new peers need to set up neighbours
   Peer(char *ip, int port) {
      init();

      //connect to right
      int sockfd = socket(PF_INET, SOCK_STREAM, 0); //TODO: do some error checking!
      sockaddr_in dest_addr;

      dest_addr.sin_family = AF_INET;
      dest_addr.sin_port = htons(port);
      dest_addr.sin_addr.s_addr = inet_addr(ip);
      memset(&(dest_addr.sin_zero), '\0', 8);  // zero the rest of the struct

      if (connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr)) < 0) {
         std::cerr << "Error: no such peer" << std::endl;
         exit(-1);
      } //TODO: don't forget to error check the connect()!


      //send "hey mr right, lets set up neighbours" messages
      char c[12] = {CHANGE_NEIGHBOUR, ':', LEFT + '0', ':', '\0'} ;
      memcpy(&c[4], &(my_server_info.sin_addr.s_addr), sizeof(my_server_info.sin_addr.s_addr));
      c[8] = ':';
      memcpy(&c[9], &(my_server_info.sin_port), sizeof(my_server_info.sin_port));
      c[11] = '\0';
      int bytes_sent = send(sockfd, c, sizeof(c), 0); //error check?

      //receive sockData back, this will be my left
      int byte_count;
      char sockData[sizeof(sockaddr_in) + 8];
      byte_count = recv(sockfd, sockData, sizeof(sockData) + 8, 0);
      memcpy(&leftPeer, &sockData[0], sizeof(sockaddr_in));
      memcpy(&nextId, &sockData[sizeof(sockaddr_in)], sizeof(nextId));
      memcpy(&numContent, &sockData[sizeof(sockaddr_in) + 4], sizeof(numContent));

      //try to receive a 0
      if (recv(sockfd, sockData, sizeof(sockaddr_in), 0) != 0) {
         std::cerr << "Error: updating neighbours, received too many messages" << std::endl;
      }

      close(sockfd);
      sockfd = socket(PF_INET, SOCK_STREAM, 0);
      //connect and send to left,

      if (connect(sockfd, (struct sockaddr *)&leftPeer, sizeof(struct sockaddr)) < 0) {
         std::cerr << "Error: updating neighbours, could not find left" << std::endl;
         exit(-1);
      }

       //send "hey mr left, lets set up neighbours" messages
      c[2] = RIGHT + '0';
      bytes_sent = send(sockfd, c, sizeof(c), 0); //error check?

      memset(sockData, '\0', sizeof(sockData));
      byte_count = recv(sockfd, sockData, sizeof(sockData) + 8, 0);
      memcpy(&rightPeer, &sockData[0], sizeof(sockaddr_in));

      //try to receive a 0
      if (recv(sockfd, sockData, sizeof(sockaddr_in), 0) != 0) {
         std::cerr << "Error: updating neighbours, received too many messages" << std::endl;
      }

      std::cout << inet_ntoa(my_server_info.sin_addr) << " " << ntohs(my_server_info.sin_port) << std::endl;
      //TODO: print here? return as msg?
      close(sockfd);
      run();
   }

};

#endif
