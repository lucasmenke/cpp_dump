/*
** server_stream.cpp -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

// BACKLOG: number of connections allowed in the queue
//      - incoming connections in the queue are waiting until they are accepted
const int BACKLOG = 10;
const std::string PORT = "3490";

// signal child handler -> signal sent to a parent process when one of its child prccesses terminates / stops
//      - allows the parent to do cleanup tasks
void sigchld_handler(int s)
{
    // waitpid() might overwrite errno -> temporarily save & restore it
    int saved_errno = errno;

    // waitpid: returns child process PID (> 0) when child process has been terminated
    // -1: waits for child process
    // WNOHANG: with no hang -> waitpid() returns immediately if there are no child processes that have terminated (non-blocking behavior)
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;

    errno = saved_errno;
}

// void *: returns pointer (data type is not specified)
// struct sockaddr *sa: pointer to the generic structure sockaddr which can hold IPv4 & IPv6
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        // returns pointer to IPv4 address
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    // returns pointer to IPv6 address
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main()
{
    int sockfd, newfd;                         // listen on sockfd, new connection on newfd
    struct addrinfo *servinfo, *p, hints = {}; // servinfo -> server info
    struct sockaddr_storage their_addr;        // connector's address information
    struct sigaction sa;                       // sigaction -> signal action
    char s[INET6_ADDRSTRLEN];

    hints.ai_family = AF_UNSPEC;     // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE;     // fill in my own IP

    int gai_status = getaddrinfo(NULL, PORT.c_str(), &hints, &servinfo);
    if (gai_status != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_status));
        return 1;
    }

    // loop through all the IP addresses getaddrinfo() gave us
    for (p = servinfo; p != NULL; p->ai_next)
    {
        // socket file descriptor
        // creates a listening socket
        //      - used to accept incoming connections from clients
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1)
        {
            // prints out errno -> errno is a global integer that holds the error
            // automatically translates errno error code into error msg
            perror("server: socket");
            exit(1);
        }

        // 2. argument: level at which the option is defined
        //      - SOL_SOCKET: socket level -> protocol independent
        // 3. argument: option to set
        //      - SO_REUSEADDR: allows reuse of local addresses (bind multiple sockets to the same locla address)
        bool yes = true; // yes -> option is enabled
        int sockopt_status = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (sockopt_status == -1)
        {
            perror("setsocketopt");
            exit(1);
        }

        // binds the socket to our own ip on port 3490
        //      - because p = servinfo -> getaddrinfo() -> used hints.ai_flags = AI_PASSIVE;
        // all ports under 1024 are reserved for the system unless you are super user (su)
        int bind_status = bind(sockfd, p->ai_addr, p->ai_addrlen);
        if (bind_status == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        // break as soon as the first bind was succesfull
        break;
    }

    // no more need for this struct after first bind
    freeaddrinfo(servinfo);

    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    int listen_status = listen(sockfd, BACKLOG);
    if (listen_status == -1)
    {
        perror("listen");
        exit(1);
    }

    // set the signal action handler to our custom functin which reaps all dead processes
    //      - when SIGCHILD signal is recived the our handler will be called
    sa.sa_handler = sigchld_handler;
    // signals can be blocked -> temporarily ignored by the process
    //      - sigemptyset() -> initialize the sa_mask signal set to be empty
    //              - sa_mask -> set of signals that are currently blocked
    sigemptyset(&sa.sa_mask);
    // SA_RESTART -> certain system calls interrupted by this signal should be automatically restarted after the signal handler returns
    sa.sa_flags = SA_RESTART;
    // registers our signal handler (sigchld_handler)
    // ensures that the sigchld_handler function is called to handle SIGCHLD signals and properly reaps terminated child processes
    int sigaction_status = sigaction(SIGCHLD, &sa, NULL);
    if (sigaction_status == -1)
    {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while (1)
    {
        socklen_t sin_size = sizeof(their_addr);
        // once a connection is accepted, the server creates a new socket (client socket) specifically for communication with the new client
        // each accepted connection returns a seperate file descriptor
        // sockaddr_storage their_addr -> generic structure to hold IPv4 & IPv6 (will be casted to the needed structure e.g. (struct sockaddr *)&their_addr)
        newfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (newfd == -1)
        {
            perror("accept");
            continue;
        }

        // ntop: network to presentation
        //      - get_in_addr(): extracts the address (in binary) from the sockaddr struct
        //      - inet_ntop(): converts this binary address into a human-readable string
        //      - s: resulting string is stored in the character array s
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);

        // this ensures that the code is running as a child process
        //      - fork() tries to create a child process
        if (!fork())
        {
            // child doesn't need the listening socket anymore
            close(sockfd);

            // sending a msg to the client
            // send has a max. limit of bytes to send -> split up data into smaller chunks
            int send_status = send(newfd, "Hello World!", 13, 0);
            if (send_status == -1)
            {
                perror("send");
            }

            // closes the client socket
            close(newfd);
            // similar to close() is shutdown() -> int shutddown_status = shutdown(newfd, 2);
            // 0 = Further receives are disallowed
            // 1 = Further sends are disallowed
            // 2 = Further sends and receives are disallowed (similar to close(newfd))
            // shutdown() doesn't close the file descriptor -> just changes usability
            //      - close() closes the file descriptor

            exit(0);
        }

        // parent process doesn't need this because it already handed it over to the child process
        //      - fork() did this
        close(newfd);
    }

    return 0;
}