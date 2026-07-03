# Libraries
no libs are authorized.

# Allow Functions (sorted)

## Process Control
execve, fork, waitpid, kill, signal

## IPC (Inter-process Communication)
pipe, dup, dup2, socketpair

## Networking (Sockets Lifecycle)
socket, bind, listen, accept, connect, send, recv

## Address Resolution & Protocols
getaddrinfo, freeaddrinfo, getprotobyname

## Socket Options & Info
setsockopt, getsockname

## Byte Order Conversion
htons, htonl, ntohs, ntohl

## I/O Multiplexing
select, poll, epoll (epoll_create, epoll_ctl, epoll_wait) kqueue (kqueue, kevent)

## File System Operations
open, close, read, write, access, stat

## Directory Handling
opendir, readdir, closedir, chdir

## File Descriptor Control
fcntl (probably only for macOS)

## Error Handling
errno, strerror, gai_strerror

