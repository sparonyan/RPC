
#include <iostream>
#include "RemoteFileSystem.h"
#include "Shared.h"

using namespace std;

int main () {

    /*
     * char *host,
       short port,
       unsigned long auth_token,
        struct timeval *timeout)
     */
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    char* h = "127.0.0.1";
    RemoteFileSystem rm(h, 1777, 42L, &tv);
    char* m = "r";
    rm.open("1.txt", m);

    cout << "Hello World!" << endl;



    return 0;
}