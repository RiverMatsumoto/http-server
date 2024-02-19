#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>

#define PORT 8080

#define errExit(msg)        \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

int poll_msgs(int socket_fd)
{
    int ready = 0;
    nfds_t nfds = 1;
    struct pollfd *pfds;

    pfds = calloc(1, sizeof(struct pollfd));
    if (pfds == NULL)
        errExit("calloc");
    pfds->fd = socket_fd;
    pfds->events = POLLIN | POLLHUP | POLLERR;

    printf("Server started, waiting for messages\n");
    char buffer[1024];
    while (1)
    {
        ready = poll(pfds, nfds, -1);
        if (ready == -1)
            errExit("poll");

        if (pfds->revents & POLLERR)
        {
            printf("Error, Connection closed\n");
        }
        else if (pfds->revents & POLLHUP)
        {
            printf("Connection closed");
            break;
        }
        else if (pfds->revents & POLLIN)
        {
            ssize_t bytes_read = read(pfds->fd, buffer, 1024);
            if (bytes_read == 0)
            {
                printf("Connection closed\n");
                break;
            }
            else 
            {
                printf("Msg received: %s\n", buffer);
            }
        }
    }

    free(pfds);
    return 1;
}

int wait_for_new_socket(int server_fd, struct sockaddr_in *address, int *addrlen)
{
    printf("Waiting for client to connect\n");
    int socket_fd;
    if ((socket_fd = accept(server_fd, (struct sockaddr *)address, (socklen_t *)addrlen)) < 0)
    {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    return socket_fd;
}

int main(int argc, char **argv)
{
    int server_fd, socket_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsocketopt failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    socket_fd = wait_for_new_socket(server_fd, &address, &addrlen);

    while (1)
    {
        int result = poll_msgs(socket_fd);
        if (result == 1)
        {
            socket_fd = wait_for_new_socket(server_fd, &address, &addrlen);
        }
    }

    close(socket_fd);
    close(server_fd);
    return 0;
}