#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <syslog.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>

#define PORT 9000
#define MAX_BUFFER_SIZE 4000000

int server_socket, client_socket;
volatile sig_atomic_t exit_flag = 0;
FILE *data_file;

void sig_handler(int signo) {
    if (signo == SIGINT || signo == SIGTERM) {
        syslog(LOG_INFO, "Caught signal, exiting");
        exit_flag = 1;
    }
}


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

void log_accepted_connection(int client_socket) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    if (getpeername(client_socket, (struct sockaddr *)&client_addr, &client_addr_len) == 0) {
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);

        syslog(LOG_INFO, "Accepted connection from %s", client_ip);
    } else {
        perror("getpeername");
    }
}

void log_closed_connection(struct sockaddr_in client_addr) {
    char ip_buffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), ip_buffer, INET_ADDRSTRLEN);
    syslog(LOG_INFO, "Closed connection from %s", ip_buffer);
}

int receiveAndStoreData(int client_socket) {
  data_file = fopen("/var/tmp/aesdsocketdata", "a");
  if (data_file == NULL) {
        perror("Failed to open data file");
        return -1;
    }

    char buffer[MAX_BUFFER_SIZE];
    ssize_t bytes_received;

    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0) > 0)) {
        // Search for newline character to separate data packets
        for (int i = 0; i < bytes_received; i++) {
            if (buffer[i] == '\n') {
                // Append the packet to the data file
                fwrite(buffer, 1, i + 1, data_file);
                // Move the remaining data to the beginning of the buffer
                memmove(buffer, buffer + i + 1, bytes_received - i - 1);
                // Adjust the bytes_received count
                bytes_received -= i + 1;
            }
        }
    }

    if (bytes_received < 0) {
        perror("Data receive error");
    }

    fclose(data_file);
    return 0;
}

void send_data(void){
	char str_buffer[MAX_BUFFER_SIZE];
	data_file = fopen("/var/tmp/aesdsocketdata", "r");
	if (data_file == NULL) {
    syslog(LOG_ERR, "Failed to open aesdsocketdata file for reading: %s.",strerror(errno));
    return;
  }
  while(fgets(str_buffer, MAX_BUFFER_SIZE, data_file)){
		send(client_socket, str_buffer, strlen(str_buffer), MSG_WAITALL);
	}
 	fclose(data_file);
}

int main() {
  openlog("aesdsocket", LOG_PID, LOG_DAEMON);
    
  signal(SIGINT, sig_handler);
  signal(SIGTERM, sig_handler);

  
  struct sockaddr_in server_addr, client_address;
  socklen_t client_addr_len = sizeof(client_address);

  // Create a socket
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // Bind the socket to a specific port
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr) == -1)) {
      perror("bind");
      close(server_socket);
      exit(EXIT_FAILURE);
  }

  // Listen for incoming connections
  if (listen(server_socket, 1) == -1) {
      perror("listen");
      close(server_socket);
      exit(EXIT_FAILURE);
  }
  
  data_file = fopen("/var/tmp/aesdsocketdata", "w");
  fclose(data_file);

  // Accept a connection
  while (!exit_flag) {
    client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_addr_len);
    if (client_socket < 0) {
      perror("Accept failed");
        return -1;
      }

    // Log accepted connection
    log_accepted_connection(client_socket);

    // Receive and store data from the client
    if (receiveAndStoreData(client_socket) != 0) {
      perror("Failed to receive and store data");
        return -1;
    }
    send_data();

    // Log closed connection
    log_closed_connection(client_address);
    close(client_socket);
    }
  return 0;  
}

