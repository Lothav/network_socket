#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <memory.h>

int main() {
    int sock, c_socket;
    uint16_t p_number;
    int clilen;
    char buffer[3], buffer_back[5];
    ssize_t n;
    struct sockaddr_in serv_addr, cli_addr;

    int counter = 0;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("ERROR opening socket");
        return EXIT_FAILURE;
    }

    p_number = 51515;

    serv_addr.sin_family            = AF_INET;
    serv_addr.sin_addr.s_addr       = INADDR_ANY;
    serv_addr.sin_port              = htons(p_number);

    if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR on binding");
        return EXIT_FAILURE;
    }

    listen(sock, 5);
    clilen = sizeof(cli_addr);

    struct timeval tout;
    tout.tv_sec = 1;
    tout.tv_usec = 1;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tout, sizeof(tout));

    while(1) {

        c_socket = accept(sock, (struct sockaddr *)&cli_addr, &clilen);

        if (c_socket < 0) {
            // timeout
            close(c_socket);
            continue;
        }

        n = recv( c_socket, buffer, 2, MSG_WAITALL );

        if (n < 0) {
            // timeout
            printf("T\n");
            close(c_socket);
            continue;
        }

        uint32_t counter_hs = htonl(counter);

        n = send(c_socket, &counter_hs, 4, 0);

        if (n < 0) {
            close(c_socket);
            continue;
        }

        n = recv( c_socket, buffer_back, 4 , MSG_WAITALL);

        if (n < 0 || (strlen(buffer_back) != 3)) {
            // timeout
            printf("T\n");
            close(c_socket);
            continue;
        }

        if(atoi(buffer_back) == counter) {
            counter += buffer[0] == '+' ? 1 : -1;
            counter = counter > 0 ? counter % 1000 : 0;
            printf("%i\n", counter);
        }

        close(c_socket);
    }
}