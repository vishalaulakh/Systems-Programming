#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

// Function to reverse bytes from the inputFile and write to the outputFile
void reverseBytes(int inputFile, int outputFile) {
    // Get the size of the file
    off_t fileSize = lseek(inputFile, 0, SEEK_END);
    // Move the file pointer back to the beginning
    lseek(inputFile, 0, SEEK_SET);

    char buffer;
    // Iterate through the file in reverse order
    for (off_t i = fileSize - 1; i >= 0; --i) {
        // Move to the i-th byte from the end
        lseek(inputFile, i, SEEK_SET);
        // Read one byte from the input file
        read(inputFile, &buffer, 1);
        // Write the byte to the output file
        write(outputFile, &buffer, 1);
    }
}

int main(int argc, char *argv[]) {
    // Check if correct number of command-line arguments is provided
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <inputFile> <outputFile>\n", argv[0]);
        return 1;
    }

    // Open the input file in read-only mode
    int inputFile = open(argv[1], O_RDONLY);
    if (inputFile == -1) {
        perror("Error opening input file");
        return 1;
    }

    // Open the output file in write-only mode, create if not exists, truncate to zero length
    int outputFile = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (outputFile == -1) {
        perror("Error opening the output file");
        // Close the input file before returning
        close(inputFile);
        return 1;
    }

    // Call the function to reverse bytes from input to output
    reverseBytes(inputFile, outputFile);

    // Close both input and output files
    close(inputFile);
    close(outputFile);

    return 0;
}

