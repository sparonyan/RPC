//
// Created by Satine Paronyan on 6/7/19.
//

#ifndef CPLUSPLUS_SHARED_H
#define CPLUSPLUS_SHARED_H

#include <cstdint>
#include <cstdio>


#define OPCODE_OPENFILE 1


struct OpenFileRequest {
    static const int OBJ_SIZE = 20;
    uint64_t token;
    uint64_t seqnum;
    uint16_t pathlen;
    const char *pathname;
    uint8_t mode;

    OpenFileRequest(uint64_t token, uint64_t seqnum, uint16_t pathlen, const char *pathname, uint8_t mode);

    size_t serialize(char * buf, size_t size);



};


// desiaralizes receives bytes.
// Returns pointer to OpenFileRequest, nullptr in case of a failure
OpenFileRequest* desirializeOpenFileRequest(const char *buf, size_t size);



struct OpenFileResponse {
    static const int OBJ_SIZE = 24;
    uint64_t seqnum;
    uint64_t fd; //file descriptor sizes are platform dependent
    uint64_t errocode;

    OpenFileResponse(uint64_t seqnum, uint64_t fd, uint64_t error);

    size_t serialize(char * buf, size_t size);
};

OpenFileResponse* desirializeOpenFileResponse(const char *buf, size_t size);



#endif //CPLUSPLUS_SHARED_H
