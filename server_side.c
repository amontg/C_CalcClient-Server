#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> // Unix uses <arpa/inet.h, Windows uses <Winsock2.h>. Server will be Unix.

#define SIZE 1024

void write_file(int sockfd){
    int n;
    FILE *fp;
    char *filename = "recv.txt";
    char buffer[SIZE];

    fp = fopen(filename, "w");
    memset(buffer, '\0', SIZE);
    n = recv(sockfd, buffer, SIZE, 0);
    printf("Receiving file...\n");
    if (n < 0){
        perror("There was an issue getting the file \n");
        return;
    }

    fprintf(fp, "%s", buffer);
    fflush(fp); // needed in order to flush buffer to disk immediately
    printf("Wrote to new file...\n");
    memset(buffer, '\0', SIZE);

    return;
}

int main(){
    char *ip = "127.0.0.1"; // on this machine, using the following port
    int port = 3333;

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr; // basic structure for handling internet addresses: https://www.gta.ufrj.br/ensino/eel878/sockets/sockaddr_inman.html
    socklen_t addr_size; // size of address
    char buffer[1024];
    int n;

    server_sock = socket(AF_INET, SOCK_STREAM, 0); 
    /*      socket(domain, type, protocol)
        domain = address family, AF_INET = IPv4
        type = socket type (SOCK_STREAM = TCP, SOCK_DGRAM = UDP, SOCK_SEQPACKET = TCP for records(?))
        protocol = the protocol being used, 0 is default for address family
    */

    // setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));

    if (server_sock < 0){
        perror("There is an error with your socket");
        exit(1);
    }
    printf("TCP Server Socket created. \n");

    memset(&server_addr, '\0', sizeof(server_addr)); 
    /*      memset(void *str, int c, size_t n). Fill memory with value
        str pointer to memory block (& is reference)
        c is parameter to pass into memory block
        n is number of bytes to be set

        = fill memory block server_addr with NUL characters meant to be replaced, for only as many bytes as the server_addr is
    */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port); // htons for big/little endian-ness stuff: https://www.gta.ufrj.br/ensino/eel878/sockets/htonsman.html
    //server_addr.sin_addr.s_addr = inet_addr(ip); // if we specify an IP address, it'll refuse any IP that doesn't match

    n = bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    /*      bind(int socket, const struct sockaddr *address, socklen_t address_len)
        socket is the socket to be bound
        address is a sockaddr struct containing the address to be bound
        address_len specifies length of sockaddr struct given in address argument

        = bind the server_addr struct address of the sizeof(server_addr struct)
    */
    if (n < 0){
        perror("There is an error with the socket binding");
        exit(1);
    }
    printf("Bound to port %d. \n", port);

    listen(server_sock, 5); // listen on given socket, backlog is used to limit outstanding connections in socket's listening queue
    printf("Listening... \n");

    while(1){
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size); // accepting the client connections
        printf("We have a client connected! \n");

        // bzero(buffer, 1024); // bzero(void *s, size_t n) = place n zero-valued bytes in the area pointed to by s. BZERO IS OBSOLETE, USE MEMSET
        //memset(buffer, '\0', 1024);
        // recv(client_sock, buffer, sizeof(buffer), 0); // receiving
        // printf("Client: %s \n", buffer);

        printf("Waiting for the file...\n");
        write_file(client_sock);

        memset(buffer, '\0', 1024); // replace 1024bytes in buffer with NUL
        strcpy(buffer, "We have the file. Thank you.\n");
        printf("Server: %s \n", buffer);
        send(client_sock, buffer, strlen(buffer), 0); // sending

        close(client_sock); // closing
        printf("Client has disconnected. \n\n");
    }

    return 0;
}