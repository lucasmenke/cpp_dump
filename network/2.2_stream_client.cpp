#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// MAXDATASIZE: max. number of bytes the client can revice at once
const int MAXDATASIZE = 100;
const std::string PORT = "3490";

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    char s[INET6_ADDRSTRLEN];
    struct addrinfo *servinfo, *p, hints = {};

    if (argc != 2)
    {
        fprintf(stderr, "usage: hostname(127.0.0.1 for local network)\n");
        exit(1);
    }

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int gai_status = getaddrinfo(argv[1], PORT.c_str(), &hints, &servinfo);
    if (gai_status != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_status));
        return 1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1)
        {
            perror("client: socket");
            continue;
        }

        int con_status = connect(sockfd, p->ai_addr, p->ai_addrlen);
        if (con_status == -1)
        {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    // check if the for loop found a result to connect to
    //      - if not: return an error
    if (p == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo);

    // recv: recive data from a connected socket
    //      - buf: buffer for the recived data to be stored
    //      - MAXDATASIZE - 1: max. number of byted to receive (99)
    numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0);
    if (numbytes == -1)
    {
        perror("recv");
        exit(1);
    }

    // add a null terminator as the 100th byte
    buf[numbytes] = '\0';
    printf("client: received '%s'\n", buf);

    close(sockfd);
    return 0;
}