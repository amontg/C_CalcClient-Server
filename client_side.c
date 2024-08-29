#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <wsipv6ok.h>

#define SIZE 1024 // define a constant

void send_file(FILE *fp, int sock){
    int n;
    char data[SIZE];

    printf("Sending...\n");
    fgets(data, SIZE, fp);
    n = send(sock, data, sizeof(data), 0);
    if (n == -1){
        printf("There is an error sending the file: %d\n", WSAGetLastError());
        exit(1);
    }

    memset(data, '\0', SIZE);
    printf("Sent and moving on...\n");
}
 
int main(){

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData); // must be invoked on windows before socket functions

// initialize ip stuffnthings
    char *ip = "192.168.1.153"; // target ip and port
    int port = 3333;

    int sock;
    struct sockaddr_in addr;
    socklen_t addr_size;
    char buffer[1024];
    int n;

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    //InetPton(AF_INET, ip, &addr.sin_addr.s_addr);
    addr.sin_addr.s_addr = inet_addr(ip);

// initialize file sending stuffnthings
    FILE *fp;
    char *filename = "test.txt";

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET){
        printf("There is an error with your socket: %d\n", WSAGetLastError());
        exit(1);
    }
    printf("TCP server socket created. \n");

    n = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (n == SOCKET_ERROR){
        printf("There is an error with connection: %d\n", WSAGetLastError());
        exit(1);
    }
    printf("Connected to the server. \n");

    memset(buffer, '\0', 1024);
    //strcpy(buffer, "This is the client.");
    //printf("Client: %s\n", buffer);
    //send(sock, buffer, strlen(buffer), 0);

    // send the files
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("There was an error with the file: %d\n", WSAGetLastError());
        exit(1);
    }

    printf("Sending the file...\n");
    send_file(fp, sock);

    memset(buffer, '\0', 1024);
    printf("Waiting on a response...\n");
    recv(sock, buffer, sizeof(buffer), 0);
    printf("Server: %s\n", buffer);

    closesocket(sock);
    printf("Disconnected from the server.\n");

    WSACleanup();
    return 0;
}