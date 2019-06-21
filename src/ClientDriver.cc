// ClientDriver.cc
// Project: Remote Procedure Calls
// Implemented: Satine Paronyan.

#include <iostream>
#include <fcntl.h>
#include <cstring> //memcpy, strlen
#include "RemoteFileSystem.h"
#include "Shared.h"

using namespace std;

// Simple client driver to test RemoteFileSystem.
int main (int argc, char *argv[]) {
    if (argc < 4) {
        std::cerr << "Error, command line arguments\n";
        exit(EXIT_FAILURE);
    }

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    char* h = argv[1];
    short port = atoi(argv[2]);
    char* filename = argv[3];

    RemoteFileSystem rm(h, port, 42L, &tv);

    const char* m = "r+";
    RemoteFileSystem::File *file = rm.open(filename, (char *)m);
    if (file == nullptr) {
        std::cerr << "Failed open the file\n";
        exit(EXIT_FAILURE);
    }

    char buf[65000];

    // read from file
    ssize_t readBytes = file->read(buf, 8);

    if (readBytes == -1) {
        std::cerr << "Error, read from file";
        exit(EXIT_FAILURE);
    }

    std::cout << "Read data in buffer\n";
    for (int i = 0; i < (int)readBytes; i++) {
        cout << buf[i];
    }
    cout << endl;

    // Reposition pointer
    std::cout << "Repositioning file pointer 8 bytes from current\n";
    off_t pos = file->lseek(8, SEEK_CUR);
    if (pos == -1) {
        std::cerr << "Error, reposition file pointer\n";
        exit(EXIT_FAILURE);
    }
    std::cout << "Repositioned file pointer to " << pos << " bytes\n";

    // write fo file
    const char *ch_write = "   ABCDEFGHI";
    ssize_t write_out = file->write((char *)ch_write, 9);
    if (write_out == -1) {
        std::cerr << "Error, write to file\n";
        exit(EXIT_FAILURE);
    }
    cout << "Number of bytes written to file: " << write_out << endl;

    mode_t mode = S_IRUSR;
    // try rename file name that is passed as an argument
    int rv = rm.chmod(filename, mode);
    if (rv == -1) {
        std::cerr << "Error, reset file mode\n";
        exit(EXIT_FAILURE);
    }
    cout << "File mode has been reset to read only\n";

    cout << "Trying to write into file with read only mode\n";
    // this fill output error
    ssize_t write_out2 = file->write((char *)ch_write, 9);
    if (write_out2 > 0) {
        cout << "Number of bytes written to file: " << write_out2 << endl;
    }

    size_t length = strlen(filename);
    char *newFName = (char*)malloc(length + 2);
    memcpy(newFName, filename, length);
    newFName[length] = '1';
    newFName[length + 1] = 0;
    cout << "Renaming file\n";

    int ren = rename(filename, newFName);
    if (ren == -1) {
        std::cerr << "Error, rename file\n";
        exit(EXIT_FAILURE);
    }
    cout << "File reneamed!\n";

    free(newFName);
    delete file;
    return 0;
}
