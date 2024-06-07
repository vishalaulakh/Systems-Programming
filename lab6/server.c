#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_CLIENT_SERVER "/tmp/client-server_fifo"
#define FIFO_SERVER_CLIENT "/tmp/server-client_fifo"
#define BUFFER_SIZE 1024

struct packet {
    int nbytes;
    char payload[BUFFER_SIZE];
};

int main() {
    printf("Server started\n");

    // Create FIFOs if not exist
    mkfifo(FIFO_CLIENT_SERVER, 0666);
    mkfifo(FIFO_SERVER_CLIENT, 0666);

    printf("FIFOs Created\n");
    printf("Waiting for client...\n"); 
    // Open FIFOs
    int client_to_server_fd = open(FIFO_CLIENT_SERVER, O_RDONLY);
    int server_to_client_fd = open(FIFO_SERVER_CLIENT, O_WRONLY);

    if (client_to_server_fd == -1 || server_to_client_fd == -1) {
        perror("Error opening FIFOs");
        exit(EXIT_FAILURE);
    }

    printf("FIFOs opened\n");

    // Receive filename from client
    char filename[256];
    read(client_to_server_fd, filename, sizeof(filename));
    printf("Received filename from client: %s\n", filename);

    // Open file for writing
    char *new_filename = "lab6_sourcefile_local_clone";
    int file_fd = open(new_filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (file_fd == -1) {
        perror("Error opening file");
        close(client_to_server_fd);
        close(server_to_client_fd);
        exit(EXIT_FAILURE);
    }

    printf("File opened for writing\n");

    struct packet pkt;
    while (1) {
        // Receive data from client
        read(client_to_server_fd, &pkt, sizeof(pkt));
        if (pkt.nbytes == 0) // End of file
            break;

        // Write data to file
        write(file_fd, pkt.payload, pkt.nbytes);
        printf("Received %d bytes from client and written to file\n", pkt.nbytes);
    }

    // Send acknowledgment to client
    strcpy(pkt.payload, "All DONE");
    write(server_to_client_fd, &pkt, sizeof(pkt));
    printf("Acknowledgment sent to client\n");

    // Close file descriptors
    close(file_fd);
    close(client_to_server_fd);
    close(server_to_client_fd);

    printf("Server finished\n");

    return 0;
}

