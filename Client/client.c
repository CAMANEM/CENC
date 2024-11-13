#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 4096

// Function to encrypt data using XOR
void xor_encrypt(char *data, size_t size, unsigned char key) {
    for (size_t i = 0; i < size; i++) {
        data[i] ^= key;
    }
}

// Function to write data in hexadecimal
void write_hex(FILE *file, char *data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        fprintf(file, "%02X", (unsigned char)data[i]);  // Write each byte as hexadecimal
    }
    fprintf(file, "\n");
}

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    FILE *file, *file_encrypted, *file_original;
    unsigned char XOR_KEY;

    // Check if XOR key and image file were passed as arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <XOR_key_in_hexadecimal> <image_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Convert XOR key from hexadecimal string to numeric value
    XOR_KEY = (unsigned char)strtol(argv[1], NULL, 16);

    // Open the image file for reading
    file = fopen(argv[2], "rb");
    if (file == NULL) {
        perror("Error opening the image");
        exit(EXIT_FAILURE);
    }

    // Create the socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Error creating the socket");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Set up the server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert the IP address
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        fclose(file);
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting to the server");
        fclose(file);
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Open the file to save the original image in hexadecimal
    file_original = fopen("original_image.txt", "w");
    if (file_original == NULL) {
        perror("Error opening the file to save the original image");
        fclose(file);
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Open the file to save the encrypted image in hexadecimal
    file_encrypted = fopen("encrypted_image.txt", "w");
    if (file_encrypted == NULL) {
        perror("Error opening the file to save the encrypted image");
        fclose(file);
        fclose(file_original);
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Read and encrypt the image in chunks
    ssize_t bytes_read;
    while ((bytes_read = fread(buffer, sizeof(char), BUFFER_SIZE, file)) > 0) {
        // Save the original image as hexadecimal
        write_hex(file_original, buffer, bytes_read);

        xor_encrypt(buffer, bytes_read, XOR_KEY);  // Encrypt the data before sending

        // Save the encrypted image as hexadecimal
        write_hex(file_encrypted, buffer, bytes_read);

        // Send the encrypted data to the server
        send(sock, buffer, bytes_read, 0);
    }

    printf("Original image saved as 'original_image.txt'\n");
    printf("Encrypted image saved as 'encrypted_image.txt' and sent to the server\n");

    // Close the files and socket
    fclose(file);
    fclose(file_original);
    fclose(file_encrypted);
    close(sock);

    return 0;
}
