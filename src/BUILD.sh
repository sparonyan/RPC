#!/usr/bin/env bash

g++ -c Shared.cpp -o Shared.o -std=c++11
ar rcs libshared.a Shared.o

g++ -c RemoteFileSystem.cpp -o RemoteFileSystem.o -std=c++11
ar rcs libremotefilesystem.a RemoteFileSystem.o


g++ -L. -lshared -o serverRPC Server.cpp Shared.cpp -Wall -Werror -pedantic -std=c++11

g++ -I. -L. -o clientRFS ClientDriver.cc -lremotefilesystem -lshared -Wall -Werror -pedantic -std=c++11
