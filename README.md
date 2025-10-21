# TCP File Upload System

**Author:** Nguyen Duc Tan Sang
**Student ID:** 20225664
**Course:** Computer Network Programming – Week 4 Assignment

---

## 🧠 Objective

This assignment implements a **TCP File Upload System** that allows multiple clients to connect to a TCP server and upload files sequentially. The project focuses on practical TCP socket programming and modular software design.

### Key Learning Goals

* Understand how TCP provides reliable communication.
* Implement a simple **client-server architecture** using sockets.
* Learn how to structure and **modularize C projects**.
* Handle **multiple clients sequentially** (using blocking sockets).
* Practice reading and writing binary file data through sockets.

---

## ⚙️ Project Structure

```
HW4/
│
├── TCP_Server/
│   ├── server.c
│   ├── logger/
│   │   ├── logger.c
│   │   └── logger.h
│   ├── message/
│   │   ├── message.c
│   │   └── message.h
│   ├── receiver/
│   │   ├── file_receiver.c
│   │   └── file_receiver.h
│   └── Makefile
│
├── TCP_Client/
│   ├── client.c
│   ├── file_transfer/
│   │   ├── file_transfer.c
│   │   └── file_transfer.h
│   ├── validation/
│   │   ├── validation.c
│   │   └── validation.h
│   └── Makefile
│
├── tests/
│   ├── test1.txt
│   ├── test2.txt
│   └── image.png
│
└── README.md
```

---

## 🏗️ Compilation

At the project root directory, simply run:

```bash
make
```

This will generate two executables:

* `server` inside `TCP_Server/`
* `client` inside `TCP_Client/`

---

## 🚀 Usage

### Run the Server

```bash
./server <Port_Number> <Storage_Directory>
```

Example:

```bash
./server 5500 uploads
```

The server will automatically create the directory if it does not exist and wait for client connections.

### Run the Client

```bash
./client <Server_IP> <Server_Port>
```

Example:

```bash
./client 127.0.0.1 5500
```

Once connected, the client can input file paths to upload. Example session:

```
Connected to server 127.0.0.1:5500 successfully!
Message from server: +OK Welcome to file server
Enter file path to upload (empty to exit): tests/test1.txt
Uploading 'test1.txt' (1024 bytes)...
+OK Successful upload
```

---

## 📂 Communication Protocol

The communication between the client and server follows a simple custom protocol:

### 1. Client sends upload command

```
UPLD <filename> <filesize>
```

### 2. Server responds

```
+OK Please send file
```

### 3. Client sends the file in binary blocks.

### 4. Server replies

```
+OK Successful upload
```

or

```
-ERR File receive failed
```

---

## 🧩 Features

* Modular design with reusable components: message, logger, validation, file handling.
* Handles multiple clients sequentially (blocking I/O model).
* Validates command format and file integrity.
* Logs every upload event with client IP and status.
* Supports any file type (text, image, compressed files, etc.).

---

## 🧪 Test Scenario

| Step | Client 1            | Client 2            | Client 3               |
| ---- | ------------------- | ------------------- | ---------------------- |
| 1    | Start Client 1      |                     |                        |
| 2    |                     | Start Client 2      |                        |
| 3    |                     |                     | Start Client 3         |
| 4    |                     |                     | Upload compressed file |
| 5    |                     | Upload image        |                        |
| 6    | Upload .txt file    |                     |                        |
| 7    | Wait for completion |                     |                        |
| 8    | Program ends        | Wait for completion | Wait for completion    |

All files should be successfully uploaded and appear in the server's storage directory.

---

## 🧰 Example Log Output

```
Server listening on port 5500...
Accepted connection from 127.0.0.1:52012
Received: UPLD test.txt 1234
+OK Please send file
File 'test.txt' received successfully.
Connection closed.
```

---

## 📘 Notes

* No multithreading or `fork()` is used; the server handles clients sequentially.
* Each module can be tested independently.
* The protocol is simple but demonstrates reliable TCP transmission.

---

## 📄 License

This project is created for educational purposes at **Hanoi University of Science and Technology (HUST)**.
All rights reserved © 2025 Nguyen Tan Sang.
