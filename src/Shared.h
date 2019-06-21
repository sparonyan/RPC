// Shared.h
// Project: Remote Procedure Calls
// Implemented: Satine Paronyan.

#ifndef CPLUSPLUS_SHARED_H
#define CPLUSPLUS_SHARED_H

#include <cstdint>
#include <cstdio>

#define BUFLEN 65000
#define MAXREADWRITE 64950

#define OPCODE_OPENFILE 1
#define OPCODE_READFILE 2
#define OPCODE_WRITEFILE 3
#define OPCODE_LSEEKFILE 4
#define OPCODE_CHMODFILE 5
#define OPCODE_UNLINKFILE 6
#define OPCODE_RENAMEFILE 7
#define OPCODE_CLOSEFILE 8


struct OpenFileRequest {
    static const size_t OBJ_SIZE = 20;
    uint64_t token;
    uint64_t seqnum;
    uint16_t pathlen;
    const char *pathname;
    uint8_t mode;

    OpenFileRequest(uint64_t token, uint64_t seqnum, uint16_t pathlen, const char *pathname, uint8_t mode);

    size_t serialize(char *buf, size_t size);
};


// desiaralizes receives bytes.
// Returns pointer to OpenFileRequest, nullptr in case of a failure
OpenFileRequest* deserializeOpenFileRequest(const char *buf, size_t size);



struct OpenFileResponse {
    static const size_t OBJ_SIZE = 24;
    uint64_t seqnum;
    uint64_t fd; //file descriptor sizes are platform dependent
    uint64_t errocode;

    OpenFileResponse(uint64_t seqnum, uint64_t fd, uint64_t error);

    size_t serialize(char *buf, size_t size);
};

OpenFileResponse* deserializeOpenFileResponse(const char *buf, size_t size);



struct ReadFileRequest {
    static const size_t OBJ_SIZE = 26;
    uint64_t token;
    uint64_t seqnum;
    uint64_t fd_;
    uint16_t readSize;

    ReadFileRequest(uint64_t token, uint64_t seqnum, uint64_t fd_, uint16_t readSize);

    size_t serializeReadRequest(char *buf, size_t size);
};

ReadFileRequest* deserializeReadRequest(const char *buf, size_t size);


struct ReadFileResponse {
    static const size_t OBJ_SIZE = 19;
    uint64_t seqnum;
    uint64_t errocode;   // 0 if no error
    uint16_t num_bytes_read;
    const char *rDataBuf; //data read from file

    ReadFileResponse(uint64_t seqnum, uint64_t errocode, const char *buf, uint16_t size);

    size_t serializeReadResponse(char *buf, size_t size);

};

ReadFileResponse* deserializeReadResponse(const char *buf, size_t size);



struct WriteRequest {
    static const size_t OBJ_SIZE = 27;
    uint64_t token;
    uint64_t seqnum;
    uint64_t fd_;
    uint16_t writeSize;
    const char *wrDataBuf; //data to be written to file

    WriteRequest(uint64_t token, uint64_t seqnum, uint64_t fd_, uint16_t writeSize, const char *buf);

    size_t serializeWriteRequest(char *buf, size_t size);
};

WriteRequest* deserializeWriteRequest(const char *buf, size_t size);


struct WriteResponse {
    static const size_t OBJ_SIZE = 18;
    uint64_t seqnum;
    uint64_t errorcode;   // 0 if no error
    uint16_t num_bytes_written;

    WriteResponse(uint64_t seqnum, uint64_t errorcode, uint16_t num_bytes_written);

    size_t serializeWriteResponse(char *buf, size_t size);

};

WriteResponse* deserializeWriteResponse(const char *buf, size_t size);


struct LseekRequest {
    static const size_t OBJ_SIZE = 27;
    uint64_t token;
    uint64_t seqnum;
    uint64_t fd_;
    int64_t offset;
    int64_t whence;

    LseekRequest(uint64_t token, uint64_t seqnum, uint64_t fd_, int64_t offset, int64_t whence);

    size_t serializeLseekRequest(char *buf, size_t size);
};

LseekRequest* deserializeLseekRequest(const char *buf, size_t size);


struct LseekResponse {
    static const size_t OBJ_SIZE = 24;
    uint64_t seqnum;
    int64_t curPos;
    uint64_t errorcode; // 0 if no error

    LseekResponse(uint64_t seqnum, int64_t curPos, uint64_t errorcode);

    size_t serializeLseekResponse(char *buf, size_t size);
};
LseekResponse* deserializeLseekResponse(const char *buf, size_t size);




struct SimpleResponse {
    static const size_t OBJ_SIZE = 20;
    uint64_t seqnum;
    int32_t retval;
    uint64_t errorcode; // 0 if no error

    SimpleResponse(uint64_t seqnum, int32_t retval, uint64_t errorcode);

    size_t serializeResponse(char *buf, size_t size);
};


struct ChmodRequest {
    static const size_t OBJ_SIZE = 23;
    uint64_t token;
    uint64_t seqnum;
    uint16_t pathlen;
    const char *pathname;
    int32_t mode;

    ChmodRequest(uint64_t token, uint64_t seqnum, uint16_t pathlen, const char *pathname, int32_t mode);

    size_t serializeChmodRequest(char *buf, size_t size);
};

ChmodRequest* deserializeChmodRequest(const char *buf, size_t size);

#define ChmodResponse SimpleResponse
ChmodResponse* deserializeResponse(const char *buf, size_t size);


struct UnlinkRequest {
    static const size_t OBJ_SIZE = 18;
    uint64_t token;
    uint64_t seqnum;
    uint16_t pathlen;
    const char * pathname;

    UnlinkRequest(uint64_t token, uint64_t seqnum, uint16_t pathlen, const char *pathname);

    size_t serializeUnlinkRequest(char *buf, size_t size);
};

UnlinkRequest* deserializeUnlinkRequest(const char *buf, size_t size);


#define UnlinkResponse SimpleResponse


struct RenameRequest {
    static const size_t OBJ_SIZE = 22;
    uint64_t token;
    uint64_t seqnum;
    uint16_t oldpathlen;
    const char *oldpathname;
    uint16_t newpathlen;
    const char *newpathname;

    RenameRequest(uint64_t token, uint64_t seqnum,
            uint16_t oldpathlen, const char *oldpathname,
            uint16_t newpathlen, const char *newpathname);

    size_t serializeRenameRequest(char *buf, size_t size);
};

RenameRequest* deserializeRenameRequest(const char *buf, size_t size);

#define RenameResponse SimpleResponse


struct CloseFileRequest {
    static const size_t OBJ_SIZE = 24;
    uint64_t token;
    uint64_t seqnum;
    uint64_t fd;

    CloseFileRequest(uint64_t token, uint64_t seqnum, uint64_t fd);

    size_t serializeCloseRequest(char *buf, size_t size);
};

CloseFileRequest* deserializeCloseRequest(const char *buf, size_t size);


#define CloseFileResponse SimpleResponse



#endif //CPLUSPLUS_SHARED_H
