#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <thread>

/*Port number to be used*/
#define MyPort "47000"
#define Backlog 50

void handle_connection(int client_socket, int server_socket) {
    /*Buffer to accept recieved message*/
    char buf[BUFSIZ];
    /*String used to create and send answer*/
    std::string buf_send;
    /*Variables to check size of data recieved and sent*/
    size_t bytes_recv, bytes_sent;

    /*Recieve data and write it to buf, if bytes_recv == -1 then an error has occured*/
    if ((bytes_recv = recv(client_socket, buf, BUFSIZ - 1, 0)) == -1) {
        std::cerr << strerror(errno) << "Error Recieving Http Request" << std::endl;
        close(client_socket);
        return;
    }

    /*Print recieved message in server terminal*/
    std::cout << "Message recieved: " << std::endl;
    std::cout << buf << std::endl;

    /*Create answer*/
    buf_send = "Http/1.1 200 OK\r\nContent-Type: Unknown\r\n\r\n";
    buf_send += buf;

    /*Print confirmation of reception and answer in server terminal*/
    std::cout << "Answering..." << std::endl;

    /*Send answer to client*/
    if ((bytes_sent = send(client_socket, buf_send.c_str(), buf_send.size(), 0)) == -1) {
        std::cerr << strerror(errno) << "Error Sending Http Response" << std::endl;
        close(client_socket);
        return;
    }
    /*Check if the whole message was sent*/
    if (bytes_sent != buf_send.size()) {
        std::cerr << "Response string is too long" << std::endl;
        close(client_socket);
        return;
    }
    /*Close send/recv socket*/
    close(client_socket);
}

int main() {
    /*sockaddr struct to save address of client*/
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    /*Variables to save and use address and info of server*/
    struct addrinfo hints, *res;
    /*Socket fds*/
    int server_socket, client_fd;

    /*Ensure hints is empty*/
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET6;  /*Ensure only IPv6 is used*/
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;  /*Use own IP address*/

     /*Generate server address info*/
    if (getaddrinfo(NULL, MyPort, &hints, &res) != 0) {
        std::cerr << "Error filling Address Structs" << std::endl;
        return EXIT_FAILURE;
    }

    /*Create socket for Clients to connect to*/
    if ((server_socket = socket(PF_INET6, SOCK_STREAM, 0)) == -1) {
        std::cerr << strerror(errno) << "Error creating socket" << std::endl;
        close(server_socket);
        return EXIT_FAILURE;
    }

    /*Bind server to Server socket*/
    if (bind(server_socket, res->ai_addr, res->ai_addrlen) == -1) {
        std::cerr << strerror(errno) << "Error binding socket" << std::endl;
        close(server_socket);
        return EXIT_FAILURE;
    }

    /*Tell server to listen for clients on the server socket*/
    if (listen(server_socket, Backlog) == -1) {
        std::cerr << strerror(errno) << "Error listening" << std::endl;
        close(server_socket);
        return EXIT_FAILURE;
    }

    /*Infinite loop for clients to connect and send/recv*/
    while (true) {
        std::cout << "Waiting for connections..." << std::endl;

        /*Wait for a client connection and accept when it appears*/
        addr_size = sizeof their_addr;
        if ((client_fd = accept(server_socket, (struct sockaddr*)&their_addr, &addr_size)) == -1) {
            std::cerr << strerror(errno) << "Error accepting client" << std::endl;
            close(server_socket);
            return EXIT_FAILURE;
        }
        std::cout << "Now connected" << std::endl;

        /*Handle the client connection in a thread, so multiple threads can run at once*/
        std::thread(handle_connection, client_fd, server_socket).detach();
    }
    /*Close the server socket and free the memory space of res*/
    close(server_socket);
    freeaddrinfo(res);
    return EXIT_SUCCESS;
}