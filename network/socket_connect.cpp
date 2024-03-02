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
    struct addrinfo *res, hints = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int gai_status = getaddrinfo("www.google.de", "80", &hints, &res);
    if (gai_status < 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_status));
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1)
    {
        fprintf(stderr, "socket: %s\n", strerror(errno));
    }

    int connect_status = connect(sockfd, res->ai_addr, res->ai_addrlen);
    if (connect_status == -1)
    {
        fprintf(stderr, "connect: %s\n", strerror(errno));
    }

    return 0;
}