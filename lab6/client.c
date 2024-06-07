#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_CLIENT_SERVER "/tmp/client-server_fifo" //Defining the FIFO path for communication from Client to Server
#define FIFO_SERVER_CLIENT "/tmp/server-client_fifo" //Defining the FIFO path for communication from Server to Client
#define BUFFER_SIZE 1024  // Setting a Buffer Size 1024 based on Lab Document

struct packet {
    int nbytes;
    char payload[BUFFER_SIZE];
};

int main(int argc, char *argv[]) {
    // Checking if the correct number of command-line arguments is provided
    
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename> [Please Provide a File Name]\n", argv[0]);  // Printing a usage message to stderr
        exit(EXIT_FAILURE);
    }

    printf("Client started\n");  // A Print message indicating that the client has started

    // Checking if FIFOs exist
    if (access(FIFO_CLIENT_SERVER, F_OK) == -1 || access(FIFO_SERVER_CLIENT, F_OK) == -1) {
        fprintf(stderr, "One or both FIFOs are missing. Exiting.\n");
        exit(EXIT_FAILURE);
    }

    printf("FIFOs exist\n");

    // Open FIFOs
    int client_to_server_fd = open(FIFO_CLIENT_SERVER, O_WRONLY);   // Open FIFO for writing from client to server
    int server_to_client_fd = open(FIFO_SERVER_CLIENT, O_RDONLY);   // Open FIFO for reading from server to client

    // Check if FIFOs opened successfully
    if (client_to_server_fd == -1 || server_to_client_fd == -1) {
        perror("Error opening FIFOs");
        exit(EXIT_FAILURE);
    }

    printf("FIFOs opened\n");

    // Send filename to server
    char *filename = argv[1];
    write(client_to_server_fd, filename, strlen(filename) + 1);
    printf("Filename sent to server: %s\n", filename);

    // Open file to read and send to server
    int file_fd = open(filename, O_RDONLY);
    if (file_fd == -1) {
        perror("Error opening file");
        close(client_to_server_fd);
        close(server_to_client_fd);
        exit(EXIT_FAILURE);
    }

    printf("File opened\n");

    struct packet pkt;
    int nbytes;
    while ((nbytes = read(file_fd, pkt.payload, BUFFER_SIZE)) > 0) {
        pkt.nbytes = nbytes;
        write(client_to_server_fd, &pkt, sizeof(pkt));
        printf("Sent %d bytes to server\n", nbytes);
    }

    // Send end of file message
    pkt.nbytes = 0;
    write(client_to_server_fd, &pkt, sizeof(pkt));
    printf("End of file sent\n");

    // Receive server acknowledgment
    read(server_to_client_fd, &pkt, sizeof(pkt));
    printf("Server response: %s\n", pkt.payload);

    // Close file descriptors
    close(file_fd);
    close(client_to_server_fd);
    close(server_to_client_fd);

    printf("Client finished\n");

    return 0;
}

