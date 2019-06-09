//
// Created by Satine Paronyan on 6/7/19.
//

#include <cstring>
#include "Shared.h"


OpenFileRequest::OpenFileRequest(uint64_t token, uint64_t seqnum, uint16_t pathlen, const char *pathname, uint8_t mode) {
    this->token = token;
    this->seqnum = seqnum;
    this->pathlen = pathlen;
    this->pathname = pathname;
    this->mode = mode;
}

size_t OpenFileRequest::serialize(char * buf, size_t size) {
    if (size < OBJ_SIZE + this->pathlen) {
        return size_t(-1);
    }

    size_t cur = 0;
    buf[cur++] = OPCODE_OPENFILE;
    memcpy(buf + cur, &token, 8);
    cur += 8;

    memcpy(buf + cur, &seqnum, 8);
    cur += 8;

    memcpy(buf + cur, &pathlen, 2);
    cur += 2;

    memcpy(buf + cur, pathname, size_t(pathlen));
    cur += pathlen;

    buf[cur++] = mode;

    return cur;
}


OpenFileRequest* desirializeOpenFileRequest(const char *buf, size_t size) {
    if (size < OpenFileRequest::OBJ_SIZE) {
        return nullptr;
    }

    uint64_t tk;
    uint64_t sq;
    uint16_t plen;
    const char *pname;
    uint8_t md;

    size_t cur = 1;
    memcpy(&tk, buf + cur, 8);
    cur += 8;

    memcpy(&sq, buf + cur, 8);
    cur += 8;

    memcpy(&plen, buf + cur, 2);
    cur += 2;

    pname= buf + cur;
    cur += plen;

    md = buf[cur++];

    return new OpenFileRequest(tk, sq, plen, pname, md);
}


OpenFileResponse::OpenFileResponse(uint64_t seqnum, uint64_t fd, uint64_t error) {
    this->seqnum = seqnum;
    this->fd = fd;
    this->errocode = error;
}

size_t OpenFileResponse::serialize(char *buf, size_t size) {

    size_t cur = 0;
    memcpy(buf + cur, &seqnum, 8);
    cur += 8;
    memcpy(buf + cur, &fd, 8);
    cur += 8;
    memcpy(buf + cur, &errocode, 8);
    cur += 8;

    return cur;
}

OpenFileResponse* desirializeOpenFileResponse(const char *buf, size_t size) {
    if (size < OpenFileResponse::OBJ_SIZE) {
        return nullptr;
    }

    uint64_t sq;
    uint64_t _fd;
    uint64_t err;

    size_t cur = 0;
    memcpy(&sq, buf + cur, 8);
    cur += 8;
    memcpy(&_fd, buf + cur, 8);
    cur += 8;
    memcpy(&err, buf + cur, 8);
    cur += 8;

    return new OpenFileResponse(sq, _fd, err);
}