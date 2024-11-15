# Client-Server Architecture with XOR Encryption and Decryption

## Summary

This system demonstrates a client-server architecture where the client encrypts an image using the XOR encryption algorithm, sends the encrypted data to the server, and the server decrypts it to restore the original image. The client and server communicate over TCP sockets, with the server listening on port 8080. The XOR encryption algorithm is symmetric, meaning the same key is used for both encryption and decryption. The system saves the original image, the encrypted image (in hexadecimal form), and the decrypted image to files.

## Overview

This document explains the client-server architecture using sockets and the XOR encryption and decryption algorithm. It provides the necessary steps to implement, run, and test the system, where the client sends an image to the server, encrypting the image with the XOR algorithm, and the server decrypts it and saves it.

### Key Concepts

1. **Client-Server Architecture**:
   - **Client**: The client is responsible for reading an image file, encrypting it using the XOR algorithm, and sending it to the server.
   - **Server**: The server listens for incoming connections from the client, receives the encrypted image data, decrypts it using XOR, and saves the resulting image.

2. **XOR Encryption and Decryption**:
   - XOR (Exclusive OR) is a simple binary operation that can be used for encryption and decryption. The same operation is applied for both encryption and decryption, making it a symmetric algorithm.
   - In this system, the client applies XOR encryption on the image data before sending it, and the server applies XOR decryption to restore the original image.

## Files Involved

1. **Client Code**:
   - Reads the image file.
   - Encrypts the data using XOR.
   - Sends the encrypted data to the server.

2. **Server Code**:
   - Listens for incoming connections.
   - Receives the encrypted image data.
   - Decrypts the data using XOR.
   - Saves the decrypted image.

## Steps to Implement and Test

Make sure you have both the `client.c` and `server.c` files. Follow these steps to compile, run, and test the system:

### 1. Compile and Execute the Server

The server needs to be started first, as it listens for incoming connections. **It is crucial to specify the XOR key when running the server**. This key is required for both encryption and decryption. Here's an example of how to compile and run the server:

```bash
gcc -o server server.c
./server AA
```

The AA is the XOR key (in hexadecimal format) that the server will use for decryption.
After executing the server, it will display:

```bash
Server listening on port 8080...
```

### 2. Compile and Execute the Client

Once the server is running, you can run the client. You must specify the XOR key (the same one that was defined on the server) and the image file path when running the client. The client will read the image, encrypt it, and send the encrypted data to the server.

```bash
gcc -o client client.c
./client AA ../Images/diagram1.jpg
```

The AA is the XOR key (in hexadecimal format) that the client will use for encryption.
After executing the client, it will display:

```bash
Original image saved as 'original_image.txt'
Encrypted image saved as 'encrypted_image.txt' and sent to the server
```

### 3. Check the Server Output

The server will receive the encrypted image, decrypt it using the XOR key AA, and save the resulting image. It will display:

```bash
Decrypted image saved as 'decrypted_image.jpg'
```

### 4. Verify the Image Files

original_image.txt: Contains the hexadecimal representation of the original image data.

encrypted_image.txt: Contains the hexadecimal representation of the encrypted image data.

received_image.jpg: The decrypted image saved by the server.