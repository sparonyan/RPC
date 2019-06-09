//
// Created by Satine Paronyan on 6/1/19.
//

#include <cstdio>
#include <netdb.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <stdio.h>
#include <arpa/inet.h>
#include <iostream>
#include "Shared.h"
#include "RemoteFileSystem.h"



uint64_t generatetoken() {
    //TODO
    uint64_t result;
    
    return 42;
}


RemoteFileSystem::File::File(RemoteFileSystem* filesystem, const char *pathname, char *mode, uint64_t fd) {
    this->fs = filesystem;
    this->filename = pathname;
    this->fmode = *mode;
    this->fd = fd;
}


RemoteFileSystem::RemoteFileSystem(char *host,
                                   short port,
                                   unsigned long auth_token,
                                   struct timeval *timeout) {

    token = generatetoken();
    servaddrlen = sizeof(servaddr);

    // create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("cannot create client socket\n");
        exit(EXIT_FAILURE);
    }


    memset((char *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port); //converts to network byte order
    int rv;

    // convert IPv4 numbers-and-dots notation into binary form (in network byte order)
    if (inet_aton(host, &servaddr.sin_addr) == 0) {
        perror("error: inet_aton()\n");
        exit(EXIT_FAILURE);
    }


    //set timeout
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval)) == -1)
    {
        perror("set timeout failed\n");
        exit(EXIT_FAILURE);
    }

    // connect to server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        close(sockfd);
        perror("client: connect");
        exit(EXIT_FAILURE);
    }

}

RemoteFileSystem::~RemoteFileSystem() {
    close(sockfd);
}



RemoteFileSystem::File* RemoteFileSystem::open(const char *pathname, char *mode) {
    uint64_t sq = generatetoken();

    OpenFileRequest msg(token, sq, strlen(pathname), pathname, *mode);

    char buf[BUFLEN];
    size_t rv = msg.serialize(buf, BUFLEN);

    if ((sendto(sockfd, buf, rv, 0, (struct sockaddr*)nullptr, sizeof(servaddr))) < 0 ) {
        perror("Error, send open message\n");
        return nullptr;
    }

    ssize_t rtv;
    if ((rtv = recvfrom(sockfd, (void *)buf, BUFLEN, 0, (struct sockaddr*)nullptr, nullptr)) == -1) {
        std::cerr << "Error, server receive from\n";
        return nullptr;
    }
    std::cout << "Client received responce\n";

    // TODO timeouts
    OpenFileResponse *resp = desirializeOpenFileResponse(buf, rtv);
    if (resp == nullptr) {
        std::cerr << "Error process received response, open\n";
        return nullptr;
    }

    if (resp->errocode != 0) {
        delete resp;
        return nullptr;
    }

    uint64_t f = resp->fd;
    delete resp;
    return new RemoteFileSystem::File(this, pathname, mode, f);

}







