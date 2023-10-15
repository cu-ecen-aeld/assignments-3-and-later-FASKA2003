#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <syslog.h>
#include <unistd.h>

#define PORT 9000

int createSocket() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        return -1;
    }
    return server_socket;
}

int bindSocket(int server_socket) {
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Socket bind failed");
        return -1;
    }
    syslog(LOG_INFO, "Socket bound to port %d", PORT);
    return 0;
}

int listenForConnections(int server_socket) {
    if (listen(server_socket, 1) < 0) {
        perror("Socket listen failed");
        return -1;
    }
    syslog(LOG_INFO, "Listening on port %d", PORT);
    return 0;
}

int acceptConnection(int server_socket) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
    if (client_socket < 0) {
        perror("Connection accept failed");
    }
    return client_socket;
}

int main() {
    int server_socket = createSocket();
    if (server_socket < 0) {
        return -1;
    }

    if (bindSocket(server_socket) < 0) {
        close(server_socket);
        return -1;
    }

    if (listenForConnections(server_socket) < 0) {
        close(server_socket);
        return -1;
    }

    int client_socket = acceptConnection(server_socket);
    if (client_socket < 0) {
        close(server_socket);
        return -1;
    }

    
    return 0;
}
