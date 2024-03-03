#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

// bla = bind, listen, accept
int main()
{
    addrinfo *servinfo, hints = {};  // servinfo -> server info
    hints.ai_family = AF_UNSPEC;     // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE;     // fill in my own IP

    int gai_status = getaddrinfo(NULL, "3490", &hints, &servinfo);
    if (gai_status != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(gai_status));
        return 1;
    }

    // socket file descriptor
    // socket returns a socket file descriptor which can be used to make stuff with the socket
    int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (sockfd == -1)
    {
        perror("socket");
        return 1;
    }

    // 1. BIND
    // binds the socket to our own ip on port 3490
    // all ports under 1024 are reserved for the system unless you are super user (su)
    int bind_status = bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
    if (bind_status == -1)
    {
        perror("bind");
        return 1;
    }

    // 2. LISTEN
    // 2. parameter: backlog -> number of connections allowed in the queue
    //      - incoming connections in the queue are waiting until they are accepted
    int listen_status = listen(sockfd, 5);
    if (listen_status == -1)
    {
        perror("listen");
        return 1;
    }

    // 3. ACCEPT
    // sockaddr_storage -> generic structure to hold IPv4 & IPv6 (will be casted to the needed structure e.g. (struct sockaddr *)&their_addr)
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof(their_addr);

    // each accepted connection returns a seperate file descriptor
    int newfd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
    if (newfd == -1)
    {
        perror("accept");
        return 1;
    }

    // 4. SEND / RECIVE
    std::string msg = "Hello World!";

    // send has a max. limit of bytes to send -> split up data into smaller chunks
    int bytes_sent = send(newfd, msg.c_str(), msg.length(), 0);
    if (bytes_sent == -1)
    {
        perror("send");
        return 1;
    }

    char buffer[1024];
    // 2. argument: incoming data gets filled into this buffer
    int bytes_rec = recv(newfd, buffer, 1024, 0);
    if (bytes_rec == -1)
    {
        perror("recive");
        return 1;
    }
    else if (bytes_rec == 0)
    {
        printf("remote side closed the connection");
        return 0;
    }

    // 5. CLOSE / SHUTDOWN
    // 0 = Further receives are disallowed
    // 1 = Further sends are disallowed
    // 2 = Further sends and receives are disallowed (like close(newfd))
    // shutdown() doesn't close the file descriptor -> just changes usability
    //      - close() closes the file descriptor
    int shutddown_status = shutdown(newfd, 2);
    if (shutddown_status == -1)
    {
        perror("shutdown");
        return 1;
    }

    int close_status = close(newfd);
    if (close_status == -1)
    {
        perror("close");
        return 1;
    }

    return 0;
}