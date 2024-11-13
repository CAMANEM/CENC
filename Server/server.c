#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 4096

// Function to decrypt data using XOR
void xor_decrypt(char *data, size_t size, unsigned char key) {
    for (size_t i = 0; i < size; i++) {
        data[i] ^= key;
    }
}

int main(int argc, char *argv[]) {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];
    FILE *file;
    unsigned char XOR_KEY;

    // Check if the XOR key was passed as an argument
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <XOR_key_in_hexadecimal>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Convert the XOR key from hexadecimal string to numeric value
    XOR_KEY = (unsigned char)strtol(argv[1], NULL, 16);

    // Create the socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error creating the socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Error binding");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Error listening");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept an incoming connection
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("Error accepting the connection");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Open the file to save the received image
    file = fopen("decrypted_image.jpg", "wb");
    if (file == NULL) {
        perror("Error opening the file");
        close(new_socket);
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Receive the image in chunks
    ssize_t bytes_read;
    while ((bytes_read = read(new_socket, buffer, BUFFER_SIZE)) > 0) {
        xor_decrypt(buffer, bytes_read, XOR_KEY);  // Decrypt the received data
        fwrite(buffer, sizeof(char), bytes_read, file);
    }

    printf("Decrypted image saved as 'decrypted_image.jpg'\n");

    // Close the file and sockets
    fclose(file);
    close(new_socket);
    close(server_fd);

    return 0;
}
