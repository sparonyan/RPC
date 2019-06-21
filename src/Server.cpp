// Server.cpp
// Project: Remote Procedure Calls
// Implemented: Satine Paronyan.

#include <netinet/in.h>
#include <iostream> // cout, cerr
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h> // unlink
#include <sys/stat.h>  //chmod
#include <stdio.h> //rename
#include <cstring> //memcpy
#include <string.h> // memset

#include "Shared.h"

extern int errno;

char* copyToString(const char *str, int size) {
    char * res = (char *)malloc(size + 1);
    memcpy(res, str, size);
    res[size] = 0;
    return res;
}

// Processes request open the file. Send a response package in return.
void processOpen(int socketfd, struct sockaddr_in *socket, socklen_t slen, const char *buf, ssize_t bufSize) {
    std::cout << "Server received open request\n";
    OpenFileRequest* req = deserializeOpenFileRequest(buf, size_t(bufSize));
    if (req == nullptr) {
        std::cerr << "Error process open request\n";
        return;
    }
    char * fname = copyToString(req->pathname, req->pathlen);

    int fd = open(fname, req->mode);
    OpenFileResponse * response;
    if (fd == -1) {
        response = new OpenFileResponse(req->seqnum, fd, errno);
    } else {
        response = new OpenFileResponse(req->seqnum, fd, 0);
    }

    char buff[BUFLEN];
    size_t rv = response->serialize(buff, BUFLEN);


    if ((sendto(socketfd, buff, rv, 0, (struct sockaddr*)socket, slen)) < 0 ) {
        perror("Error, send file open message\n");
    } else {
        std::cout << "Server send file open response\n";
    }

    delete req;
    delete response;
    free(fname);
}


// Processes the request for reading from file. Reads data than send a response
// package with read bytes.
void processRead(int socketfd, struct sockaddr_in *socket, socklen_t slen, const char *buf, ssize_t bufSize) {
    std::cout << "Server received read request\n";
    ReadFileRequest* request = deserializeReadRequest(buf, size_t(bufSize));
    if (request == nullptr) {
        std::cerr << "Error process read request\n";
        return;
    }

    // Read required bytes from file. The maximum amount
    // of bytes to be read from file can be MAXREAD
    char readBuf[MAXREADWRITE];
    ssize_t read_in;
    if (request->readSize > MAXREADWRITE) { //read less than BUGLEN (65000) since there some bytes will go to struct
         read_in = read(request->fd_, readBuf, MAXREADWRITE);
    } else {
        read_in = read(request->fd_, readBuf, request->readSize);
    }

    // Build response packet and send it
    ReadFileResponse *response;
    if (read_in == -1) {
        response = new ReadFileResponse(request->seqnum, errno, readBuf, read_in);
    } else {
        response = new ReadFileResponse(request->seqnum, 0, readBuf, read_in);
    }

    char buffResponse[BUFLEN];
    size_t rv = response->serializeReadResponse(buffResponse, BUFLEN);

    if ((sendto(socketfd, buffResponse, rv, 0, (struct sockaddr*)socket, slen)) < 0 ) {
        perror("Error, send read data response\n");
    } else {
        std::cout << "Server send response on read request\n";
    }

    delete request;
    delete response;
}


// Processes write request. Sends a response package back.
void processWrite(int socketfd, struct sockaddr_in *socket, socklen_t slen, const char *buf, ssize_t bufSize) {
    std::cout << "Server received write request\n";
    WriteRequest *request = deserializeWriteRequest(buf, size_t(bufSize));
    if (request == nullptr) {
        std::cerr << "Error process write request\n";
        return;
    }

    //Write required bytes to file
    ssize_t write_out = write(request->fd_, request->wrDataBuf, request->writeSize);
    WriteResponse *response;
    if (write_out == -1) {
        response = new WriteResponse(request->seqnum, errno, write_out);
    } else {
        response = new WriteResponse(request->seqnum, 0, write_out);
    }

    char respbuf[BUFLEN];
    size_t rv = response->serializeWriteResponse(respbuf, BUFLEN);

    if ((sendto(socketfd, respbuf, rv, 0, (struct sockaddr*)socket, slen)) < 0 ) {
        perror("Error, send written data response\n");
    } else {
        std::cout << "Server send response on write request\n";
    }

    delete request;
    delete response;
}


// Changes the file mode bits. Sends response package back.
void processChmod(int socketfd, struct sockaddr_in *socket, socklen_t slen, const char *buf, ssize_t bufSize) {
    std::cout << "Server received chmod request\n";
    ChmodRequest *request = deserializeChmodRequest(buf, size_t(bufSize));
    if (request == nullptr) {
        std::cerr << "Error process chmod request\n";
        return;
    }

    char * fname = copyToString(request->pathname, request->pathlen);

    int chrv = chmod(fname, request->mode);
    ChmodResponse *response;
    if (chrv == -1) {
        perror("chmod - reset mode\n");
        response = new ChmodResponse(request->seqnum, -1, errno);
    } else {
        response = new ChmodResponse(request->seqnum, 0, 0);
    }

    char respbuf[BUFLEN];
    size_t rv = response->serializeResponse(respbuf, BUFLEN);

    if ((sendto(socketfd, respbuf, rv, 0, (struct sockaddr*)socket, slen)) < 0 ) {
        perror("Error, send chmod response\n");
    } else {
        std::cout << "Server send response on chmod request\n";
    }

    delete request;
    delete response;
    free(fname);
}


// Repositions file pointer. Sends response package back.
void processLseek(int socketfd, struct sockaddr_in *socket, socklen_t slen, const char *buf, ssize_t bufSize) {
    std::cout << "Server received lseek request\n";
    // process received package
    LseekRequest *request = deserializeLseekRequest(buf, size_t(bufSize));
    if (request == nullptr) {
        std::cerr << "Error process lseek request\n";
        return;
    }

    // Reposition file pointer
    off_t position = lseek(request->fd_, request->offset, request->whence);

    // sent response package
    LseekResponse *response;
    if (position == -1) {
        response = new LseekResponse(request->seqnum, position, errno);
    } else {
        response = new LseekResponse(request->seqnum, position, 0);
    }

    char respbuf[BUFLEN];
    size_t rv = response->serializeLseekResponse(respbuf, BUFLEN);

    if ((sendto(socketfd, respbuf, rv, 0, (struct sockaddr*)socket, slen)) < 0 ) {
        perror("Error, send lseek response\n");
    } else {
        std::cout << "Server send response on lseek\n";
    }

    delete request;
    delete response;
}


// Removes a link to file. Sends response package back.
void processUnlink(int socketfd, struct sockaddr_in *socket, socklen_t slen, const char *buf, ssize_t bufSize) {
    std::cout << "Server received unlink request\n";
    // process received package
    UnlinkRequest *request = deserializeUnlinkRequest(buf, size_t(bufSize));
    if (request == nullptr) {
        std::cerr << "Error process unlink request\n";
        return;
    }

    char * fname = copyToString(request->pathname, request->pathlen);
    int unlink_rv = unlink(fname);
    UnlinkResponse *response;
    if (unlink_rv == -1) {
        response = new UnlinkResponse(request->seqnum, unlink_rv, errno);
    } else {
        response = new UnlinkResponse(request->seqnum, unlink_rv, 0);
    }

    char respbuf[BUFLEN];
    size_t rv = response->serializeResponse(respbuf, BUFLEN);

    if ((sendto(socketfd, respbuf, rv, 0, (struct sockaddr*)socket, slen)) < 0 ) {
        perror("Error, send unlink response\n");
    } else {
        std::cout << "Server send response on unlink\n";
    }

    delete request;
    delete response;
    free(fname);
}


// Renames the file. Sends response package back.
void processRename(int socketfd, struct sockaddr_in *socket, socklen_t slen, const char *buf, ssize_t bufSize) {
    std::cout << "Server received rename request\n";
    // process received package
    RenameRequest *request = deserializeRenameRequest(buf, size_t(bufSize));
    if (request == nullptr) {
        std::cerr << "Error process rename request\n";
        return;
    }

    char * fnameOld = copyToString(request->oldpathname, request->oldpathlen);
    char * fnameNew = copyToString(request->newpathname, request->newpathlen);
    int renval = rename(fnameOld, fnameNew);
    RenameResponse *response;
    if (renval == -1) {
        response = new RenameResponse(request->seqnum, renval, errno);
    } else {
        response = new RenameResponse(request->seqnum, renval, 0);
    }

    char respbuf[BUFLEN];
    size_t rv = response->serializeResponse(respbuf, BUFLEN);

    if ((sendto(socketfd, respbuf, rv, 0, (struct sockaddr*)socket, slen)) < 0 ) {
        perror("Error, send rename response\n");
    } else {
        std::cout << "Server send response on rename\n";
    }

    delete request;
    delete response;
    free(fnameOld);
    free(fnameNew);
}


void processClose(int socketfd, struct sockaddr_in *socket, socklen_t slen, const char *buf, ssize_t bufSize) {
    std::cout << "Server received close request\n";
    // process received package
    CloseFileRequest *request = deserializeCloseRequest(buf, size_t(bufSize));
    if (request == nullptr) {
        std::cerr << "Error process close file request\n";
        return;
    }

    int rv = close(request->fd);
    CloseFileResponse *response;
    if (rv == -1) {
        response = new CloseFileResponse(request->seqnum, rv, errno);
    } else {
        response = new CloseFileResponse(request->seqnum, rv, 0);
    }

    char respbuf[BUFLEN];
    size_t rtv = response->serializeResponse(respbuf, BUFLEN);
    if ((sendto(socketfd, respbuf, rtv, 0, (struct sockaddr*)socket, slen)) < 0 ) {
        perror("Error, send close file response\n");
    } else {
        std::cout << "Server send response on close\n";
    }

    delete request;
    delete response;
}



int main(int argc, char *argv[]) {

    if (argc < 2) {
        std::cerr << "Error, command line arguments\n";
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);

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
    sockfd_me.sin_port = htons(port);
    sockfd_me.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr*)&sockfd_me, sizeof(sockfd_me)) == -1) {
        perror("Error, bind in server'n");
        exit(EXIT_FAILURE);
    }

    while (true) {
        ssize_t rv;
        if ((rv = recvfrom(sockfd, (void *)buf, BUFLEN, 0, (struct sockaddr*)&sockfd_other, &slen)) == -1) {
            perror("Error, server receive from\n");
            continue;
        }
        std::cout << "received request\n";
        if (rv == 0) {
            std::cerr << "Empty packet received\n";
            continue;
        }

        switch(buf[0]) {
            case OPCODE_OPENFILE:
                processOpen(sockfd, &sockfd_other, slen, buf, rv);
                break;
            case OPCODE_READFILE:
                processRead(sockfd, &sockfd_other, slen, buf, rv);
                break;
            case OPCODE_WRITEFILE:
                processWrite(sockfd, &sockfd_other, slen, buf, rv);
                break;
            case OPCODE_LSEEKFILE:
                processLseek(sockfd, &sockfd_other, slen, buf, rv);
                break;
            case OPCODE_CHMODFILE:
                processChmod(sockfd, &sockfd_other, slen, buf, rv);
                break;
            case OPCODE_UNLINKFILE:
                processUnlink(sockfd, &sockfd_other, slen, buf, rv);
                break;
            case OPCODE_RENAMEFILE:
                processRename(sockfd, &sockfd_other, slen, buf, rv);
                break;
            case OPCODE_CLOSEFILE:
                processClose(sockfd, &sockfd_other, slen, buf, rv);
                break;
            default:
                std::cerr << "Invalid packet received\n";
                break;

        }
    }

}