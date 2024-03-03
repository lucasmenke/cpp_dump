#include <stdio.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "usage: ./host_to_ip hostname\n");
        exit(1);
    }

    int status;
    // enough bytes to hold IPv4 or IPv6
    char ipstr[INET6_ADDRSTRLEN];
    struct addrinfo *res, *p, hints = {};

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    // performs hostname to IP lookup
    // 1. argument: host to lookup (e.g. www.google.de)
    // 2. argument: service (e.g. "http", "ftp" or port number)
    // 3. argument: requirements -> provide hints what we are looking for (e.g. IPv4 or TCP)
    // 4. argument: result -> gets filled with information about our lookup
    status = getaddrinfo(argv[1], NULL, &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    printf("IP addresses for %s:\n\n", argv[1]);

    // ai = address information
    // ai_next is from type addrinfo -> basically an addrinfo (p) that holds another addrinfo
    // this loop iterates through all ai_next members
    for (p = res; p != NULL; p = p->ai_next)
    {
        void *addr;
        std::string ipver;

        // AF = address family
        // PF = protocol family
        if (p->ai_family == AF_INET)
        {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr); // sin = socket internet
            ipver = "IPv4";
        }
        else
        {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        // ntop = network to presentation
        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        printf("  %s: %s\n", ipver.c_str(), ipstr);
    }

    freeaddrinfo(res);
    return 0;
}