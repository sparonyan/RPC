//
// RemoteFileSystem.h
//
// Client-side remote (network) filesystem
//
// Author: Morris Bernstein
// Copyright 2019, Systems Deployment, LLC.

// Project: Remote Procedure Calls
// Modified: Satine Paronyan

#if !defined(RemoteFileSystem_H)
#define RemoteFileSystem_H


#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFLEN 65000

class RemoteFileSystem {
 public:

  // File represents an open file object
  class File {
  public:
    // Destructor closes open file.
    ~File();

   ssize_t read(void *buf, size_t count);
   ssize_t write(void *buf, size_t count);
   off_t lseek(off_t offset, int whence);

  private:
    // Only RemoteFileSystem can open a file.
    friend class RemoteFileSystem;
    File(RemoteFileSystem* filesystem, uint64_t fd);

    // Disallow copy & assignment
    File(File const &) = delete;
    void operator=(File const &) = delete;

    RemoteFileSystem *fs;
    uint64_t fd;
  };

  // Connect to remote system.  Throw error if connection cannot be
  // made.
  RemoteFileSystem(char *host,
		   short port,
		   unsigned long auth_token,
		   struct timeval *timeout);

  // Disconnect
  ~RemoteFileSystem();

  // Return new open file object.  Client is responsible for
  // deleting.
  // The argument mode must include one of the following access modes:
  // "r" - read only. File should exist in order to be opened with this mode.
  // "w" - write only. Truncate file to zero length or create text file for writing.
  //       The stream is positioned at the beginning of the file.
  // "r+" - read/write (file is not truncated). If file does not exist
  //        it will be created. The content of existing file is not truncated.
  //
  File *open(const char *pathname, char *mode);

  int chmod(const char *pathname, mode_t mode);
  int unlink(const char *pathname);
  int rename(const char *oldpath, const char *newpath);

 private:
  // File class may use private methods of the RemoteFileSystem to
  // implement their operations.  Alternatively, you can create a
  // separate Connection class that is local to your implementation.
  friend class File;

  // Disallow copy & assignment
  RemoteFileSystem(RemoteFileSystem const &) = delete;
  void operator=(RemoteFileSystem const &) = delete;

  uint64_t token;
  const char* host;
  int port;
  int sockfd;
  struct sockaddr_in servaddr;
};


#endif
