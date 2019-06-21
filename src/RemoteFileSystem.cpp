// RemoteFileSystem.cpp
// Project: Remote Procedure Calls
// Implemented: Satine Paronyan.

#include <cstdio>
#include <netdb.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <stdio.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/types.h>

#include "Shared.h"
#include "RemoteFileSystem.h"



uint64_t generateRandomNum() {
    uint64_t result;
    result = rand();
    result = (result << 32) | rand();
    return result;
}


ssize_t sendWithRetry(int sockfd, char *buf, int bufsize) {
    for (int i = 1; i <=2; i++) {
        if ((sendto(sockfd, buf, bufsize, 0, (struct sockaddr *) nullptr,
                    sizeof(struct sockaddr_in))) < 0) {
            return -1;
        }

        ssize_t rtv;
        if ((rtv = recvfrom(sockfd, (void *) buf, BUFLEN, 0,
                            (struct sockaddr *) nullptr, nullptr)) == -1) {
            if (errno == EAGAIN) {
                continue;
            }
            return -1;
        }
        return rtv;
    }
    return -1;
}


RemoteFileSystem::File::File(RemoteFileSystem* filesystem, uint64_t fd) {
    this->fs = filesystem;
    this->fd = fd;
}


RemoteFileSystem::File::~File() {
    uint64_t sq = generateRandomNum();
    CloseFileRequest request(fs->token, sq, fd);

    char buf[BUFLEN];
    size_t rv = request.serializeCloseRequest(buf, BUFLEN);

    ssize_t rtv = sendWithRetry(fs->sockfd, buf, rv);

    CloseFileResponse *response = deserializeResponse(buf, rtv);
    // cannot through exception if something went wrong witch close()
    if (response != nullptr) {
        delete response;
    }
}


RemoteFileSystem::RemoteFileSystem(char *host,
                                   short port,
                                   unsigned long auth_token,
                                   struct timeval *timeout) {

    token = auth_token;

    // create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        throw "Create RemoteFileSystem\n";
    }


    memset((char *)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port); //converts to network byte order
    int rv;

    // convert IPv4 numbers-and-dots notation into binary form (in network byte order)
    if (inet_aton(host, &servaddr.sin_addr) == 0) {
        throw "Create RemoteFileSystem\n";
    }


    //set timeout
    struct timeval *tv = timeout;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, tv, sizeof(struct timeval)) == -1)
    {
        throw "Create RemoteFileSystem\n";
    }

    // connect to server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        close(sockfd);
        throw "Create RemoteFileSystem\n";
    }

}

RemoteFileSystem::~RemoteFileSystem() {
    close(sockfd);
}



RemoteFileSystem::File* RemoteFileSystem::open(const char *pathname, char *mode) {
    // if not throw exception
    uint64_t sq = generateRandomNum();

    OpenFileRequest msg(token, sq, strlen(pathname), pathname, *mode);

    char buf[BUFLEN];
    size_t rv = msg.serialize(buf, BUFLEN);

    ssize_t rtv = sendWithRetry(sockfd, buf, rv);

    if (rtv == -1) {
        return nullptr;
    }

    OpenFileResponse *resp = deserializeOpenFileResponse(buf, rtv);
    if (resp == nullptr) {
        return nullptr;
    }

    if ((resp->errocode != 0) || sq != resp->seqnum) {
        delete resp;
        return nullptr;
    }

    uint64_t f = resp->fd;
    delete resp;
    return new RemoteFileSystem::File(this, f);

}


ssize_t RemoteFileSystem::File::read(void *buf, size_t count) {
    if (count <= 0) {
        return 0;
    }

    uint64_t sq = generateRandomNum();
    ReadFileRequest rd(fs->token, sq, fd, count);

    char buff[BUFLEN];
    size_t rv = rd.serializeReadRequest(buff, BUFLEN);

    ssize_t rtv = sendWithRetry(fs->sockfd, buff, rv);

    if (rtv == -1) {
        return -1;
    }

    ReadFileResponse *resp = deserializeReadResponse(buff, rtv);
    if (resp == nullptr)
    {
        return -1;
    }
    if (sq != resp->seqnum) {
        delete resp;
        return -1;
    }
    if (resp->num_bytes_read == 0) {
        delete resp;
        return 0; // eof
    }
    if (resp->errocode != 0) {
        delete resp;
        return -1;
    }

    ssize_t read_in = resp->num_bytes_read;
    memcpy((char *)buf, resp->rDataBuf, resp->num_bytes_read);
    delete resp;
    return read_in;
}


ssize_t RemoteFileSystem::File::write(void *buf, size_t count) {
    if (count <= 0) {
        return 0;
    }
    uint64_t sq = generateRandomNum();

    uint16_t write_count = count;
    if (count > MAXREADWRITE) {
        write_count = MAXREADWRITE;
    }

    WriteRequest wr_request(fs->token, sq, fd, write_count, (char *)buf);

    char req_buf[BUFLEN];
    size_t rv = wr_request.serializeWriteRequest(req_buf, BUFLEN);

    ssize_t rtv = sendWithRetry(fs->sockfd, req_buf, rv);

    if (rtv == -1) {
        return -1;
    }

    WriteResponse *response = deserializeWriteResponse(req_buf, rtv);
    if (response == nullptr) {
        return -1;
    }
    if ((response->errorcode != 0) || (sq != response->seqnum)) {
        delete response;
        return -1;
    }
    ssize_t result = response->num_bytes_written;
    delete response;
    return result;
}


// Returns the resulting offset location as measured in bytes from the beginning
// of the file.  On error, the value (off_t) -1 is returned
// Whence: SEEK_SET, SEEK_CUR, SEEK_END
off_t RemoteFileSystem::File::lseek(off_t offset, int whence) {
    if (whence != SEEK_CUR && whence != SEEK_SET && whence != SEEK_END) {
        return -1;
    }
    uint64_t sq = generateRandomNum();
    LseekRequest request(fs->token, sq, fd, offset, whence);

    char req_buf[BUFLEN];
    size_t rv = request.serializeLseekRequest(req_buf, BUFLEN);
    ssize_t rtv = sendWithRetry(fs->sockfd, req_buf, rv);

    if (rtv == -1) {
        return -1;
    }

    LseekResponse *response = deserializeLseekResponse(req_buf, rtv);
    if (response == nullptr) {
        return -1;
    }
    if ((response->errorcode != 0) || (sq != response->seqnum)) {
        delete response;
        return -1;
    }

    off_t result = response->curPos;
    delete response;
    return result;
}


// On success, zero is returned. On error, -1 is returned.
int RemoteFileSystem::chmod(const char *pathname, mode_t mode) {
    uint64_t sq = generateRandomNum();
    ChmodRequest request(token, sq, strlen(pathname), pathname, mode);

    char buf[BUFLEN];
    size_t rv = request.serializeChmodRequest(buf, BUFLEN);

    ssize_t rtv = sendWithRetry(sockfd, buf, rv);

    if (rtv == -1) {
        return -1;
    }

    ChmodResponse *response = deserializeResponse(buf, rtv);
    if (response == nullptr) {
        return -1;
    }
    if ((response->errorcode != 0) || (sq != response->seqnum)) {
        delete response;
        return -1;
    }

    int result = response->retval;
    delete response;
    return result;
}


// Upon successful completion, 0 shall be returned. Otherwise, -1 shall be
// returned. If -1 is returned, the named file shall not be changed.
int RemoteFileSystem::unlink(const char *pathname) {
    uint64_t sq = generateRandomNum();
    UnlinkRequest request(token, sq, strlen(pathname), pathname);

    char buf[BUFLEN];
    size_t rv = request.serializeUnlinkRequest(buf, BUFLEN);

    ssize_t rtv = sendWithRetry(sockfd, buf, rv);

    if (rtv == -1) {
        return -1;
    }

    UnlinkResponse *response = deserializeResponse(buf, rtv);
    if (response == nullptr) {
        return -1;
    }

    if ((response->errorcode != 0) || (sq != response->seqnum)) {
        delete response;
        return -1;
    }
    int result = response->retval;
    delete response;
    return result;
}

// Upon successful completion, rename() shall return 0,
// otherwise, -1 is returned.
int RemoteFileSystem::rename(const char *oldpath, const char *newpath) {
    uint64_t sq = generateRandomNum();
    RenameRequest request(token, sq, strlen(oldpath), oldpath, strlen(newpath), newpath);

    char buf[BUFLEN];
    size_t rv = request.serializeRenameRequest(buf, BUFLEN);

    ssize_t rtv = sendWithRetry(sockfd, buf, rv);

    if (rtv == -1) {
        return -1;
    }

    RenameResponse *response = deserializeResponse(buf, rtv);
    if (response == nullptr) {
        return -1;
    }

    if ((response->errorcode != 0) || (sq != response->seqnum)) {
        delete response;
        return -1;
    }

    int result = response->retval;
    delete response;
    return result;
}