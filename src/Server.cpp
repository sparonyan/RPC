//
// Created by Satine Paronyan on 6/7/19.
//

#include <netinet/in.h>
//#include <cstdlib>
//#include <cstdio>
//#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>

#include "Shared.h"

#define BUFLEN 65536  //max size of packet
#define PORT 1777

extern int errno;

void processOpen(int sockefdt, struct sockaddr_in *socket, socklen_t slen, const char *buf, ssize_t bufSize) {
    std::cout << "Server received open request\n";
    OpenFileRequest* req = desirializeOpenFileRequest(buf, size_t(bufSize));
    if (req == nullptr) {
        std::cerr << "Error process open request\n";
        return;
    }
    char * fname = (char *)malloc(req->pathlen + 1);
    memcpy(fname, req->pathname, req->pathlen);
    fname[req->pathlen] = 0;

    int fd = open(fname, req->mode);
    OpenFileResponse * response;
    if (fd == -1) {
        response = new OpenFileResponse(req->seqnum, fd, errno);
    } else {
        response = new OpenFileResponse(req->seqnum, fd, 0);
    }

    char buff[BUFLEN];
    size_t rv = response->serialize(buff, BUFLEN);



    if ((sendto(sockefdt, buff, rv, 0, (struct sockaddr*)socket, slen)) < 0 ) {
        perror("Error, send open message\n");
    } else {
        std::cout << "Server send response\n";
    }

    delete req;
    delete response;
    free(fname);
}


int main() {

    struct sockaddr_in sockfd_me, sockfd_other;
    socklen_t slen = sizeof(sockfd_other);
    int sockfd;
    char buf[BUFLEN];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error create server socket\n");
        exit(EXIT_FAILURE);
    }

    memset((char *)&sockfd_me, 0, sizeof(sockfd_me));

    sockfd_me.sin_family = AF_INET;
    sockfd_me.sin_port = htons(PORT);
    sockfd_me.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr*)&sockfd_me, sizeof(sockfd_me)) == -1) {
        perror("Error, bind in server'n");
        exit(EXIT_FAILURE);
    }

    while (true) {
        ssize_t rv;
        if ((rv = recvfrom(sockfd, (void *)buf, BUFLEN, 0, (struct sockaddr*)&sockfd_other, &slen)) == -1) {
            std::cerr << "Error, server receive from\n";
            continue;
        }
        std::cout << "received\n";
        if (rv == 0) {
            std::cerr << "Empty packet received\n";
            continue;
        }

        switch(buf[0]) {
            case OPCODE_OPENFILE:
                processOpen(sockfd, &sockfd_other, slen, buf, rv);
                break;
            default:
                std::cerr << "Invalid packet received\n";
        }
    }


    std::cout << "Server done\n";
    return 0;


}