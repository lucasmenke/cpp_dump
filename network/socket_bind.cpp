#include <stdio.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main()
{
    addrinfo *res, hints = {};
    hints.ai_family = AF_UNSPEC;     // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE;     // fill in my own IP

    int gai_status = getaddrinfo(NULL, "3490", &hints, &res);
    if (gai_status != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_status));
    }

    // socket file descriptor
    // socket returns a socket file descriptor which can be used to make stuff with the socket
    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1)
    {
        // errno is a global variable that holds the error value
        fprintf(stderr, "bind error: %s\n", strerror(errno));
    }

    // binds the socket to our own ip on port 3490
    // all ports under 1024 are reserved for the system unless you are super user (su)
    int bind_status = bind(sockfd, res->ai_addr, res->ai_addrlen);
    if (bind_status == -1)
    {
        fprintf(stderr, "bind error: %s\n", strerror(errno));
    }

    return 0;
}