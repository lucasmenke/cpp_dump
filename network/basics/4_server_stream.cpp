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
    int sockfd, newdf;                         // listen on sockfd, new connection on newfd
    struct addrinfo *servinfo, *p, hints = {}; // servinfo -> server info
    struct sockaddr_storage their_addr;        // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    int gai_status = getaddrinfo(NULL, PORT.c_str(), &hints, &servinfo);
    if (gai_status != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_status));
        return 1;
    }

    // loop through all the IP addresses getaddrinfo() gave us
    for (p = servinfo; p != NULL; p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1)
        {
            perror("server: socket");
        }
    }
}