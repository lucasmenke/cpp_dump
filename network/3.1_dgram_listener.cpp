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

int main()
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    char s[INET6_ADDRSTRLEN];
    struct sockaddr_storage their_addr;
    struct addrinfo *servinfo, *p, hints = {};

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use local ip

    int gai_status = getaddrinfo(NULL, PORT.c_str(), &hints, &servinfo);
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
            perror("listener: socket");
            continue;
        }

        int bind_status = bind(sockfd, p->ai_addr, p->ai_addrlen);
        if (bind_status == -1)
        {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);

    printf("listener: waiting to recvfrom...\n");

    socklen_t addr_len = sizeof(their_addr);
    // recvfrom: recive data from a unconnected datagram socket (UDP)
    //      - buf: buffer for the recived data to be stored
    //      - MAXDATASIZE - 1: max. number of byted to receive (99)
    //      - their_addr: contains information about the sender
    numbytes = recvfrom(sockfd, buf, MAXDATASIZE - 1, 0, (struct sockaddr *)&their_addr, &addr_len);
    if (numbytes == -1)
    {
        perror("recvfrom");
        exit(1);
    }

    printf("listener: got packet from %s\n",
           inet_ntop(their_addr.ss_family,
                     get_in_addr((struct sockaddr *)&their_addr), s, sizeof s));
    printf("listener: packet is %d bytes long\n", numbytes);
    buf[numbytes] = '\0';
    printf("listener: packet contains \"%s\"\n", buf);

    close(sockfd);
    return 0;
}