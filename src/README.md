 Remote Procedure Calls

The Remote Procedure Call abstraction hides the complexity of communicating between
a client and server in a distributed application.

The objective is to make it look, as closely as possible, as if the client application is simply
calling an ordinary function in its own process space. Unfortunately, it is an imperfect ("leaky")
abstraction because the client code must deal with the multiple failure modes that may occur in a distributed system.

RPCs consists of three parts. A client-side library of function defining the application-program interface,
a serialization/communication protocol and support library, and a server-side set of handler functions.

Software development kits for implmenting RPCs typically include a Domain-Specific Language allowing the developer
to describe the function parameters. These DSLs are typically known as Data Definition Languages.

The DDL takes a description of a function's arguments and return values (in effect, a function declaration),
and generates source code for the client and source-code stubs for the server.

The client-side functions simply marshal their arguments and pass them to a communication layer that transmits
the arguments to the server and unmarshalls the result returned from the server. The communication layer can be
a pre-compiled library that is linked into the client program.

The programmer writing the server (possibly but not necessarily the same person responsible for writing
the client-side application) fills in the server-side stubs with the business logic for the particular service.

The original RPC mechanism, now called ONC-RPC, was developed by Sun Microsystems as part of their Network File System (NFS)
project to create the original fileshare. NFS was implemented partially in the kernel, so an NFS volume could be
mounted as if it were an ordinary filesystem.

With that for inspiration, your task is to implement a scaled-down userland version of a network filesystem.
You will hand-roll the service, defining the data exchange and making the necessary system calls to exchange
the data. Do not use an RPC utility.

For the purposes of the assignment, don't worry about data compression or encryption.
Simulate authentication (you are who you say you are) & authorization (you have the permission to do what you are asking)
using a 64-bit number as an "auth" token. Normally, this would be provided by an credentialing service or through
some other suitable mechanism, but you may use getrandom(2) to generate a random number.

Use UDP to exchange information. Since UDP is "unreliable", each request to the server should include the auth token
and a second 64-bit randomly-generated sequence number. Any response from the server should include the sequence
number to indicate which client request the server is responding to.

The client should maintain a timeout on each request it makes to the server. The client library may retry once
after a timeout. If the second request fails or fails to return any result, the client should report failure
(via the return value) to the application.

To avoid dealing with concurrency, you may assume the server handles only a single request at a time. Multiple
concurrent requrests will get queued up in the socket's listen queue.

You may start with this skeleton for the client side: RemoteFileSystem-skel.h

In your README, include a paragraph or two comparing the RPC and message-passing concurrency abstractions
(as implemented in this assignment and your Sleeping Barbers solution). Also include a brief discussion of
how you would modify your server and client library to handle concurrent requests.

Don't forget the BUILD script.