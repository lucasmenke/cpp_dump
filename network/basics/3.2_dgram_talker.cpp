#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

const int MAXDATASIZE = 100;
const std::string PORT = "4950";

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
    struct sockaddr_storage their_addr;
    struct addrinfo *servinfo, *p, hints = {};

    if (argc != 3)
    {
        fprintf(stderr, "usage: hostname(e.g. 127.0.0.1) message(Hello World)\n");
        exit(1);
    }

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

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
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }

    numbytes = sendto(sockfd, argv[2], strlen(argv[2]), 0, p->ai_addr, p->ai_addrlen);
    if (numbytes == -1)
    {
        perror("talker: sendto");
        exit(1);
    }

    freeaddrinfo(servinfo);

    printf("talker: sent %d bytes to %s\n", numbytes, argv[1]);

    close(sockfd);
    return 0;
}