// Shared.cpp
// Project: Remote Procedure Calls
// Implemented: Satine Paronyan.

#include <cstring> //memcpy
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

OpenFileRequest* deserializeOpenFileRequest(const char *buf, size_t size) {
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
    if (size < OBJ_SIZE) {
        return size_t(-1);
    }
    size_t cur = 0;
    memcpy(buf + cur, &seqnum, 8);
    cur += 8;
    memcpy(buf + cur, &fd, 8);
    cur += 8;
    memcpy(buf + cur, &errocode, 8);
    cur += 8;

    return cur;
}

OpenFileResponse* deserializeOpenFileResponse(const char *buf, size_t size) {
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



ReadFileRequest::ReadFileRequest(uint64_t token, uint64_t seqnum, uint64_t fd_, uint16_t readSize) {
    this->token = token;
    this->seqnum = seqnum;
    this->fd_ = fd_;
    this->readSize = readSize;
}

size_t ReadFileRequest::serializeReadRequest(char * buf, size_t size) {
    if (size < OBJ_SIZE) {
        return size_t(-1);
    }
    size_t cur = 0;
    buf[cur++] = OPCODE_READFILE;
    memcpy(buf + cur, &token, 8);
    cur += 8;
    memcpy(buf + cur, &seqnum, 8);
    cur += 8;
    memcpy(buf + cur, &fd_, 8);
    cur += 8;
    memcpy(buf + cur, &readSize, 2);
    cur += 2;
    return cur;
}

ReadFileRequest* deserializeReadRequest(const char *buf, size_t size) {
    if (size < ReadFileRequest::OBJ_SIZE) {
        return nullptr;
    }

    uint64_t tk;
    uint64_t sq;
    uint64_t fd;
    uint16_t rdSize;

    size_t cur = 1;
    memcpy(&tk, buf + cur, 8);
    cur += 8;
    memcpy(&sq, buf + cur, 8);
    cur += 8;
    memcpy(&fd, buf + cur, 8);
    cur += 8;
    memcpy(&rdSize, buf + cur, 2);
    cur += 2;

    return new ReadFileRequest(tk, sq, fd, rdSize);
}


ReadFileResponse::ReadFileResponse(uint64_t seqnum, uint64_t errocode, const char *buf, uint16_t size) {
    this->seqnum = seqnum;
    this->errocode = errocode;
    this->rDataBuf = buf;
    this->num_bytes_read = size;
}


size_t ReadFileResponse::serializeReadResponse(char *buf, size_t size) {
    if (size < BUFLEN) {
        return size_t(-1);
    }

    size_t cur = 0;
    memcpy(buf + cur, &seqnum, 8);
    cur += 8;
    memcpy(buf + cur, &errocode, 8);
    cur += 8;
    memcpy(buf + cur, &num_bytes_read, 2);
    cur += 2;
    memcpy(buf + cur, rDataBuf, num_bytes_read);
    cur += num_bytes_read;
    return cur;
}

ReadFileResponse* deserializeReadResponse(const char *buf, size_t size) {
    if (size < ReadFileResponse::OBJ_SIZE) {  //19
        return nullptr;
    }

    uint64_t sq;
    uint64_t ercode;
    uint16_t num_b_read;
    const char *readData;

    size_t cur = 0;
    memcpy(&sq, buf + cur, 8);
    cur += 8;
    memcpy(&ercode, buf + cur, 8);
    cur += 8;
    memcpy(&num_b_read, buf + cur, 2);
    cur += 2;
    readData = buf + cur;

    return new ReadFileResponse(sq, ercode, readData, num_b_read);
}



WriteRequest::WriteRequest(uint64_t token, uint64_t seqnum, uint64_t fd_, uint16_t writeSize, const char *buf) {
    this->token = token;
    this->seqnum = seqnum;
    this->fd_ = fd_;
    this->writeSize = writeSize;
    this->wrDataBuf = buf;
}

size_t WriteRequest::serializeWriteRequest(char *buf, size_t size) {
    if (size < BUFLEN) {
        return size_t(-1);
    }

    size_t cur = 0;
    buf[cur++] = OPCODE_WRITEFILE;
    memcpy(buf + cur, &token, 8);
    cur += 8;
    memcpy(buf + cur, &seqnum, 8);
    cur += 8;
    memcpy(buf + cur, &fd_, 8);
    cur += 8;
    memcpy(buf + cur, &writeSize, 2);
    cur += 2;
    memcpy(buf + cur, wrDataBuf, writeSize);
    cur += writeSize;
    return cur;
}

WriteRequest* deserializeWriteRequest(const char *buf, size_t size) {
    if (size < WriteRequest::OBJ_SIZE) {
        return nullptr;
    }
    uint64_t tk;
    uint64_t sq;
    uint64_t fd;
    uint16_t wrSize;
    const char *wrData;

    size_t cur = 1;
    memcpy(&tk, buf + cur, 8);
    cur += 8;
    memcpy(&sq, buf + cur, 8);
    cur += 8;
    memcpy(&fd, buf + cur, 8);
    cur += 8;
    memcpy(&wrSize, buf + cur, 2);
    cur += 2;
    wrData = buf + cur;
    return new WriteRequest(tk, sq, fd, wrSize, wrData);
}



WriteResponse::WriteResponse(uint64_t seqnum, uint64_t errorcode, uint16_t num_bytes_written) {
    this->seqnum = seqnum;
    this->errorcode = errorcode;
    this->num_bytes_written = num_bytes_written;
}

size_t WriteResponse::serializeWriteResponse(char *buf, size_t size) {
    if (size < OBJ_SIZE) {
        return size_t(-1);
    }
    size_t cur = 0;
    memcpy(buf + cur, &seqnum, 8);
    cur += 8;
    memcpy(buf + cur, &errorcode, 8);
    cur += 8;
    memcpy(buf + cur, &num_bytes_written, 2);
    cur += 2;
    return cur;
}

WriteResponse* deserializeWriteResponse(const char *buf, size_t size) {
    if (size < WriteResponse::OBJ_SIZE) {
        return nullptr;
    }
    uint64_t sq;
    uint64_t err;
    uint16_t wrsize;

    size_t cur = 0;
    memcpy(&sq, buf + cur, 8);
    cur += 8;
    memcpy(&err, buf + cur, 8);
    cur += 8;
    memcpy(&wrsize, buf + cur, 2);
    return new WriteResponse(sq, err, wrsize);
}


LseekRequest::LseekRequest(uint64_t token, uint64_t seqnum, uint64_t fd_, int64_t offset, int64_t whence) {
    this->token = token;
    this->seqnum = seqnum;
    this->fd_ = fd_;
    this->offset = offset;
    this->whence = whence;
}

size_t LseekRequest::serializeLseekRequest(char *buf, size_t size) {
    if (size < OBJ_SIZE) {
        return size_t(-1);
    }
    size_t cur = 0;
    buf[cur++] = OPCODE_LSEEKFILE;
    memcpy(buf + cur, &token, 8);
    cur += 8;
    memcpy(buf + cur, &seqnum, 8);
    cur += 8;
    memcpy(buf + cur, &fd_, 8);
    cur += 8;
    memcpy(buf + cur, &offset, 8);
    cur += 8;
    memcpy(buf + cur, &whence, 8);
    cur += 8;
    return cur;
}

LseekRequest* deserializeLseekRequest(const char *buf, size_t size) {
    if (size < LseekRequest::OBJ_SIZE) {
        return nullptr;
    }
    uint64_t tk;
    uint64_t sq;
    uint64_t fd;
    int64_t offs;
    int64_t whce;

    size_t cur = 1;
    memcpy(&tk, buf + cur, 8);
    cur += 8;
    memcpy(&sq, buf + cur, 8);
    cur += 8;
    memcpy(&fd, buf + cur, 8);
    cur += 8;
    memcpy(&offs, buf + cur, 8);
    cur += 8;
    memcpy(&whce, buf + cur, 8);
    return new LseekRequest(tk, sq, fd, offs, whce);
}

LseekResponse::LseekResponse(uint64_t seqnum, int64_t curPos, uint64_t errorcode) {
    this->seqnum = seqnum;
    this->curPos = curPos;
    this->errorcode = errorcode;
}

size_t LseekResponse::serializeLseekResponse(char *buf, size_t size) {
    if (size < OBJ_SIZE) {
        return size_t(-1);
    }
    size_t cur = 0;
    memcpy(buf + cur, &seqnum, 8);
    cur += 8;
    memcpy(buf + cur, &curPos, 8);
    cur += 8;
    memcpy(buf + cur, &errorcode, 8);
    cur += 8;
    return cur;
}

LseekResponse* deserializeLseekResponse(const char *buf, size_t size) {
    if (size < LseekResponse::OBJ_SIZE) {
        return nullptr;
    }
    uint64_t sq;
    int64_t curPos;
    uint64_t error;

    size_t cur = 0;
    memcpy(&sq, buf + cur, 8);
    cur += 8;
    memcpy(&curPos, buf + cur, 8);
    cur += 8;
    memcpy(&error, buf + cur, 8);
    return new LseekResponse(sq, curPos, error);
}



ChmodRequest::ChmodRequest(uint64_t token, uint64_t seqnum, uint16_t pathlen, const char *pathname, int32_t mode) {
    this->token = token;
    this->seqnum = seqnum;
    this->pathlen = pathlen;
    this->pathname = pathname;
    this->mode = mode;
}

size_t ChmodRequest::serializeChmodRequest(char *buf, size_t size) {
    if (size < OBJ_SIZE + this->pathlen) {
        return size_t(-1);
    }
    size_t cur = 0;
    buf[cur++] = OPCODE_CHMODFILE;
    memcpy(buf + cur, &token, 8);
    cur += 8;
    memcpy(buf + cur, &seqnum, 8);
    cur += 8;
    memcpy(buf + cur, &pathlen, 2);
    cur += 2;
    memcpy(buf + cur, pathname, size_t(pathlen));
    cur += pathlen;
    memcpy(buf + cur, &mode, sizeof(mode));
    cur += sizeof(mode);

    return cur;
}

ChmodRequest* deserializeChmodRequest(const char *buf, size_t size) {
    if (size < ChmodRequest::OBJ_SIZE) {
        return nullptr;
    }

    uint64_t tk;
    uint64_t sq;
    uint16_t plen;
    const char *pname;
    uint32_t md;

    size_t cur = 1;
    memcpy(&tk, buf + cur, 8);
    cur += 8;
    memcpy(&sq, buf + cur, 8);
    cur += 8;
    memcpy(&plen, buf + cur, 2);
    cur += 2;
    pname = buf + cur;
    cur += plen;
    memcpy(&md, buf + cur, 4);

    return new ChmodRequest(tk, sq, plen, pname, md);
}

SimpleResponse::SimpleResponse(uint64_t seqnum, int32_t retval, uint64_t errorcode) {
    this->seqnum = seqnum;
    this->retval = retval;
    this->errorcode = errorcode;
}

size_t SimpleResponse::serializeResponse(char *buf, size_t size) {
    if (size < OBJ_SIZE) {
        return size_t(-1);
    }
    size_t cur = 0;
    memcpy(buf + cur, &seqnum, 8);
    cur += 8;
    memcpy(buf + cur, &retval, 4);
    cur += 4;
    memcpy(buf + cur, &errorcode, 8);
    cur += 8;
    return cur;
}

ChmodResponse* deserializeResponse(const char *buf, size_t size) {
    if (size < ChmodResponse::OBJ_SIZE) {
        return nullptr;
    }
    uint64_t sq;
    int32_t rv;
    uint64_t error;

    size_t cur = 0;
    memcpy(&sq, buf + cur, 8);
    cur += 8;
    memcpy(&rv, buf + cur, 4);
    cur += 4;
    memcpy(&error, buf + cur, 8);
    return new ChmodResponse(sq, rv, error);
}


UnlinkRequest::UnlinkRequest(uint64_t token, uint64_t seqnum, uint16_t pathlen, const char *pathname) {
    this->token = token;
    this->seqnum = seqnum;
    this->pathlen = pathlen;
    this->pathname = pathname;
}

size_t UnlinkRequest::serializeUnlinkRequest(char *buf, size_t size) {
    if (size < OBJ_SIZE + this->pathlen) {
        return size_t(-1);
    }
    size_t cur = 0;
    buf[cur++] = OPCODE_UNLINKFILE;
    memcpy(buf + cur, &token, 8);
    cur += 8;
    memcpy(buf + cur, &seqnum, 8);
    cur += 8;
    memcpy(buf + cur, &pathlen, 2);
    cur += 2;
    memcpy(buf + cur, pathname, size_t(pathlen));
    cur += pathlen;

    return cur;
}

UnlinkRequest* deserializeUnlinkRequest(const char *buf, size_t size) {
    if (size < UnlinkRequest::OBJ_SIZE) {
        return nullptr;
    }
    uint64_t tk;
    uint64_t sq;
    uint16_t plen;
    const char *pname;

    size_t cur = 1;
    memcpy(&tk, buf + cur, 8);
    cur += 8;
    memcpy(&sq, buf + cur, 8);
    cur += 8;
    memcpy(&plen, buf + cur, 2);
    cur += 2;
    pname = buf + cur;

    return new UnlinkRequest(tk, sq, plen, pname);
}


RenameRequest::RenameRequest(uint64_t token, uint64_t seqnum,
            uint16_t oldpathlen, const char *oldpathname,
            uint16_t newpathlen, const char *newpathname) {
    this->token = token;
    this->seqnum = seqnum;
    this->oldpathlen = oldpathlen;
    this->oldpathname = oldpathname;
    this->newpathlen = newpathlen;
    this->newpathname = newpathname;
}

size_t RenameRequest::serializeRenameRequest(char *buf, size_t size) {
    if (size < OBJ_SIZE + this->oldpathlen +this->newpathlen) {
        return size_t(-1);
    }
    size_t cur = 0;
    buf[cur++] = OPCODE_RENAMEFILE;
    memcpy(buf + cur, &token, 8);
    cur += 8;
    memcpy(buf + cur, &seqnum, 8);
    cur += 8;
    memcpy(buf + cur, &oldpathlen, 2);
    cur += 2;
    memcpy(buf + cur, oldpathname, size_t(oldpathlen));
    cur += oldpathlen;
    memcpy(buf + cur, &newpathname, 2);
    cur += 2;
    memcpy(buf + cur, newpathname, size_t(newpathlen));
    cur += newpathlen;

    return cur;
}

RenameRequest* deserializeRenameRequest(const char *buf, size_t size) {
    if (size < RenameRequest::OBJ_SIZE) {
        return nullptr;
    }
    uint64_t tk;
    uint64_t sq;
    uint16_t oldplen;
    const char *oldpname;
    uint16_t newplen;
    const char *newpname;

    size_t cur = 1;
    memcpy(&tk, buf + cur, 8);
    cur += 8;
    memcpy(&sq, buf + cur, 8);
    cur += 8;
    memcpy(&oldplen, buf + cur, 2);
    cur += 2;
    oldpname = buf + cur;
    cur += oldplen;
    memcpy(&newplen, buf + cur, 2);
    cur += 2;
    newpname = buf + cur;

    return new RenameRequest(tk, sq, oldplen, oldpname, newplen, newpname);
}


CloseFileRequest::CloseFileRequest(uint64_t token, uint64_t seqnum, uint64_t fd) {
    this->token = token;
    this->seqnum = seqnum;
    this->fd = fd;
}

size_t CloseFileRequest::serializeCloseRequest(char *buf, size_t size) {
    if (size < OBJ_SIZE) {
        return size_t(-1);
    }
    size_t cur = 0;
    buf[cur++] = OPCODE_CLOSEFILE;
    memcpy(buf + cur, &token, 8);
    cur += 8;
    memcpy(buf + cur, &seqnum, 8);
    cur += 8;
    memcpy(buf + cur, &fd, 8);
    cur += 8;
    return cur;
}

CloseFileRequest* deserializeCloseRequest(const char *buf, size_t size) {
    uint64_t tk;
    uint64_t sq;
    uint64_t fdes;

    size_t cur = 1;
    memcpy(&tk, buf + cur, 8);
    cur += 8;
    memcpy(&sq, buf + cur, 8);
    cur += 8;
    memcpy(&fdes, buf + cur, 8);
    return new CloseFileRequest(tk, sq, fdes);
}
