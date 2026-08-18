# TODOs











# All
- [ ] Understanding what HTTP is 
- [ ] Understanding what a Webserver is 
- [ ] Read the RFCs defining the HTTP protocol
- [ ] test telnet and NGINX before starting

- [ ] great guid: https://beej.us/guide/bgnet/html/split/ip-addresses-structs-and-data-munging.html

### Rules
- [ ] no Crash (even out of memory or terminated unexpectedly (signals))
- [ ] Makefile ($(Name), all, clean, fclean, re)

- [~] compile: c++ -Wall -Wextra -Werror -std=c++17

- [~] use fucking c++ (use the c++ version mostly)
- [~] no libraries
- [~] HTTP version 1.0 is suggested as reference point, but not enforced.
- [~] no execve of another webserver
- [ ] Checking the value of errno to adjust the server behaviour is strictly forbidden after performing a read or write operation

- [ ] Resilience is key. Your server must remain operational at all times.
- [ ] Write tests, use tests

- [ ] I/O that can wait for data (sockets, pipes/FIFOs, etc.) must be
non-blocking and driven by a single poll() (or equivalent). Calling
read/recv or write/send on these descriptors without prior readiness
will result in a grade of 0. Regular disk files are exempt.

- [ ] We deliberately chose to offer only a subset of the HTTP RFC. In this
context, the virtual host feature is considered out of scope. But
you are allowed to implement it if you want


### Retirements
- [ ] Config file:
  - [~] location: 1. param or has a default path

- [ ] non-blocking!
  - [ ] *1* poll()/epoll() for all I/O
    - [ ] monitor reading and writing simultaneously.
    - [ ] never read/write without poll
    - [ ] use any available options (FP_SET etc.)
  - [ ] You are not required to use poll() for regular disk files;
    - [ ] read() and write() on them do not require readiness notifications.


- [ ] request: never hang indefinitely

- [ ] client:
  - [ ] handel client disconnections when necessary
  - [ ] must be able to upload files

- [ ] server: 
  - [ ] compatible with Web browser
  - [ ] Default error pages if not provided
  - [ ] can serve static fully static websites
  - [ ] multiple Ports
  
- [ ] fcntl() only for macOS and only F_SETFL, O_NONBLOCK and, FD_CLOEXEC as Flags.

- [ ] nginx: my be used to compare headers+answers
- [ ] fork only for CGI (php, python)
- [ ] HTTP Status codes must be correct
- [ ] GET, POST, DELETE implemented
- [ ] Stress test server for availability



### execute like:
- [ ] ./webserv [configuration file]




# Not written up.
### Configuration file
### Readme
### Bonus



### not sure if its a todo
- [ ] make overwrite on already used bind mount:
  - [ ] int yes=1;
  - [ ] // lose the pesky "Address already in use" error message
  - [ ] setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);









# legend

- [ ] not done
- [~] at the moment done (needs final check) 
- [x] completely done



- [ ] 
- [x] 