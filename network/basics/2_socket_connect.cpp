#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int main()
{
    struct addrinfo *clinfo, hints = {}; // clinfo -> client info
    hints.ai_family = AF_UNSPEC;         // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;     // TCP

    // fills our clinfo structure with data
    int gai_status = getaddrinfo("www.google.de", "80", &hints, &clinfo);
    if (gai_status != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_status));
    }

    // socket file descriptor
    // socket returns a socket file descriptor which can be used to make stuff with the socket
    int sockfd = socket(clinfo->ai_family, clinfo->ai_socktype, clinfo->ai_protocol);
    if (sockfd == -1)
    {

        // prints out errno -> errno is a global integer that holds the error
        // automatically translates errno error code into error msg
        perror("socket");
    }

    // attempts to connect to a remote host using the socket file descriptor, ip address & it's size
    int connect_status = connect(sockfd, clinfo->ai_addr, clinfo->ai_addrlen);
    if (connect_status == -1)
    {
        perror("connect");
    }

    return 0;
}