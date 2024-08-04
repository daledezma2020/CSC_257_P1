#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
// #include <stdbool.h>

void zeroOut(char *s, int length);

int nameExchange(char *client_name, char *server_name,int client_sock);

int moveExchange(char *client_move, char *server_move, int socket_desc);

// bool moveCheck(char *server_move);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);  // Convert the command-line argument to an integer

    int socket_desc, client_sock;
    socklen_t client_size;
    struct sockaddr_in server_addr, client_addr;
    
    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    
    if(socket_desc < 0){
        printf("Error while creating socket\n");
        return -1;
    }
    //printf("Socket created successfully\n");
    
    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // Bind to the set port and IP:
    if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
        printf("Couldn't bind to the port\n");
        return -1;
    }
    //printf("Done with binding\n");
    
    // Listen for clients:
    if(listen(socket_desc, 1) < 0){
        printf("Error while listening\n");
        return -1;
    }
    printf("\nSearching for opponent.....\n");
    
    // Accept an incoming connection:
    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
    
    if (client_sock < 0){
        printf("Can't accept\n");
        return -1;
    }
    //printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    char client_name[10], server_name[10];

    zeroOut(client_name,10);
    zeroOut(server_name,10);

    int i = nameExchange(client_name, server_name, client_sock);

    printf("client's name is: %s\n", client_name);

    char client_move[2];
    char server_move[2];

    zeroOut(client_move,2);
    zeroOut(server_move,2);

    printf("Enter your move (r for rock, p for paper, s for scissors): ");
    fgets(server_move, sizeof(server_move), stdin);

    // Remove the trailing newline character if present
    size_t len = strlen(server_move);
    if (len > 0 && server_move[len - 1] == '\n') {
        server_move[len - 1] = '\0';
    }

    if (strlen(server_move) != 1 || (server_move[0] != 'r' && server_move[0] != 'p' && server_move[0] != 's')) {
        printf("Invalid move. Please enter r, p, or s.\n");
        return -1;
    }

    //server_move[0] = 'p';

    if(send(client_sock, &server_move, 2, 0) < 0){
        printf("Unable to send message\n");
        return -1;
    }

    if(recv(client_sock, &client_move, 2, 0) < 0){
        printf("Error while receiving client's msg\n");
        return -1;
    }

    // moveExchange(client_move,server_move,socket_desc);

    // printf("server move sent: %c\n", server_move[0]); 
    // printf("client move received: %c\n", client_move[0]);

    struct timespec remaining, request = { 1, 0 }; 
    
    for (int i = 3; i >= 1; i--) {
        printf("Countdown: %d\n", i);
        int response = nanosleep(&request, &remaining);
        
        // Check if nanosleep was interrupted prematurely
        while (response == -1 && remaining.tv_sec > 0) {
            response = nanosleep(&remaining, &remaining);
        }
        
        // Update the request time for the next iteration
        request.tv_sec = 1; // Set countdown to 1 second
        request.tv_nsec = 0;
    }

    //printf("client move: %s\nServer move: %s\n",client_move,server_move);
    printf("%s's move is: %c\n",client_name,client_move[0]);
    //
    //WINNING POSSIBILITIES
    //
    if (client_move[0] == 'r') { // If client chose 'r'
        if (server_move[0] == 'r') { // If the server (you) chose 'r'
            printf("YOU TIED :(\n");
        }
        else if (server_move[0] == 's') { // If the server (you) chose 's'
            printf("YOU LOSE :(\n");
        }
        else if (server_move[0] == 'p') { // If the server (you) chose 'p'
            printf("YOU WIN!\n");
        }
    }
    else if (client_move[0] == 'p') { // If client chose 'p'
        if (server_move[0] == 'p') { // If the server (you) chose 'p'
            printf("YOU TIED :(\n");
        }
        else if (server_move[0] == 'r') { // If the server (you) chose 'r'
            printf("YOU LOSE :(\n");
        }
        else if (server_move[0] == 's') { // If the server (you) chose 's'
            printf("YOU WIN!\n");
        }
    }
    else if (client_move[0] == 's') { // If client chose 's'
        if (server_move[0] == 's') { // If the server (you) chose 's'
            printf("YOU TIED :(\n");
        }
        else if (server_move[0] == 'p') { // If the server (you) chose 'p'
            printf("YOU LOSE :(\n");
        }
        else if (server_move[0] == 'r') { // If the server (you) chose 'r'
            printf("YOU WIN!\n");
        }
    }
    else {
        printf("error, no move received\n");
    }


    // Closing the socket:
    close(client_sock);
    close(socket_desc);
    
    return 0;
}

void zeroOut(char *s, int length){
    for(int i = 0;i < length; i++){
        s[i] = 0;
    }
}

int nameExchange(char *client_name, char *server_name,int client_sock){
    printf("GIVE ME YOUR NAME: ");

    fgets(server_name,10,stdin); //Server's name

    size_t len = strlen(server_name);
    if (len > 0 && server_name[len - 1] == '\n') {
        server_name[len - 1] = '\0';
    }

    if(send(client_sock, server_name, strlen(server_name), 0) < 0){
        printf("Unable to send message\n");
        return -1;
    }

    if(recv(client_sock, client_name, sizeof(client_name), 0) < 0){
        printf("Error while receiving client's msg\n");
        return -1;
    }

   return 0;
}

// int moveExchange(char *client_move, char *server_move, int client_sock) {
//     printf("GIVE ME YOUR MOVE: ");
//     char server_input[2];
//     fgets(server_input, sizeof(server_input), stdin);

//     // Extract the first character from user input
//     *server_move = server_input[0]; // Copy the character to server_move

//     // Clear the input buffer (consume remaining characters, including newline)
//     int c;
//     while ((c = getchar()) != '\n' && c != EOF);

//     // printf("server move sent: %c\n", *server_move); // Use %c to print a single character

//     printf("length of sent move: %lu\n",strlen(client_move));
//     if (send(client_sock, server_move, 1, 0) < 0) { // Send the character, not the pointer
//         printf("Unable to send message\n");
//         return -1;
//     }

//     if (recv(client_sock, client_move, 1, 0) < 0) { // Receive data into client_move
//         printf("Error while receiving server's move\n");
//         return -1;
//     }

//     return 0;
// }


// bool moveCheck(char *server_move){
//     if(strcmp(server_move,"r") == 0 || strcmp(server_move,"p") == 0 || strcmp(server_move,"s") == 0){
//         //printf("Check true");
//         //printf("%s",server_move);
//         return true;
//     }
//     return false;
// }
