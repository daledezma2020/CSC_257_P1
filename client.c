#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
// #include <stdbool.h>

void zeroOut(char *s, int length);

int nameExchange(char *client_name, char *server_name,int socket_desc);

int moveExchange(char *client_move, char *server_move, int socket_desc);

// bool moveCheck(char *client_move);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);  // Convert the command-line argument to an integer
    int socket_desc;
    struct sockaddr_in server_addr;
    
    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    
    if(socket_desc < 0){
        printf("Unable to create socket\n");
        return -1;
    }
    
    printf("Socket created successfully\n");
    
    // Set port and IP the same as server-side:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // Send connection request to server:
    if(connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        printf("Unable to connect\n");
        return -1;
    }
    printf("Connected with server successfully\n");

    char client_name[10], server_name[10];

    zeroOut(client_name,10);
    zeroOut(server_name,10);

    nameExchange(client_name, server_name,socket_desc); //Exchange names

    printf("Server's name is: %s\n",server_name);

    // char client_move[2], server_move[1];

    char server_move[2];
    char client_move[2];

    zeroOut(client_move, 2);
    zeroOut(server_move, 2);

    printf("Enter your move (r for rock, p for paper, s for scissors): ");
    fgets(client_move, sizeof(client_move), stdin);

    // Remove the trailing newline character if present
    size_t len = strlen(client_move);
    if (len > 0 && client_move[len - 1] == '\n') {
        client_move[len - 1] = '\0';
    }

    if (strlen(client_move) != 1 || (client_move[0] != 'r' && client_move[0] != 'p' && client_move[0] != 's')) {
        printf("Invalid move. Please enter r, p, or s.\n");
        return -1;
    }

    if (send(socket_desc, &client_move, 2, 0) < 0) {
        printf("Unable to send message\n");
        return -1;
    }

    if (recv(socket_desc, &server_move, 2, 0) < 0) {
        printf("Error while receiving server's move\n");
        return -1;
    }

    // printf("Client move sent: %c\n", client_move[0]);
    // printf("Server move received: %c\n", server_move[0]);

    //printf("client move: %s\nServer move: %s\n",client_move,server_move);

    struct timespec remaining, request = { 1, 0 }; 
    
    for (int i = 3; i >= 1; i--) {
        printf("Countdown: %d\n", i);
        int response = nanosleep(&request, &remaining);
        
        // Check if nanosleep was interrupted prematurely
        while (response == -1 && remaining.tv_sec > 0) {
            response = nanosleep(&remaining, &remaining);
        }
        
        // Update the request time for the next iteration
        request.tv_sec = 1; // Set the countdown to 1 second
        request.tv_nsec = 0;
    }
    
    //printf("client move: %s\nServer move: %s\n",client_move,server_move);
    printf("%s's move is: %c\n",server_name,server_move[0]);
    //
    //WINNING POSSIBILITIES
    //
    if (server_move[0] == 'r') { // If server chose 'rock'
        if (client_move[0] == 'r') { // If the client (you) chose 'rock'
            printf("YOU TIED :(\n");
        }
        else if (client_move[0] == 's') { // If the client (you) chose 'scissors'
            printf("YOU LOSE :(\n");
        }
        else if (client_move[0] == 'p') { // If the client (you) chose 'paper'
            printf("YOU WIN!\n");
        }
    }
    else if (server_move[0] == 'p') { // If server chose 'paper'
        if (client_move[0] == 'p') { // If the client (you) chose 'paper'
            printf("YOU TIED :(\n");
        }
        else if (client_move[0] == 'r') { // If the client (you) chose 'rock'
            printf("YOU LOSE :(\n");
        }
        else if (client_move[0] == 's') { // If the client (you) chose 'scissors'
            printf("YOU WIN!\n");
        }
    }
    else if (server_move[0] == 's') { // If server chose 'scissors'
        if (client_move[0] == 's') { // If the client (you) chose 'scissors'
            printf("YOU TIED :(\n");
        }
        else if (client_move[0] == 'p') { // If the client (you) chose 'paper'
            printf("YOU LOSE :(\n");
        }
        else if (client_move[0] == 'r') { // If the client (you) chose 'rock'
            printf("YOU WIN!\n");
        }
    }
    else {
        printf("error, no move received\n");
    }


    // Close the socket:
    close(socket_desc);
    
    return 0;
}

void zeroOut(char *s, int length){
    for(int i = 0;i < length; i++){
        s[i] = 0;
    }
}

int nameExchange(char *client_name, char *server_name,int socket_desc){
    printf("GIVE ME YOUR NAME: ");

    fgets(client_name,10,stdin); //Client's name

    size_t len = strlen(client_name);
    if (len > 0 && client_name[len - 1] == '\n') {
        client_name[len - 1] = '\0';
    }

    if(send(socket_desc, client_name, strlen(client_name), 0) < 0){
        printf("Unable to send message\n");
        return -1;
    }

    if(recv(socket_desc, server_name, sizeof(server_name), 0) < 0){
        printf("Error while receiving server's msg\n");
        return -1;
    }

   return 0;
}

// int moveExchange(char *client_move, char *server_move, int socket_desc) {
//     printf("GIVE ME YOUR MOVE: ");
//     char client_input[2]; 
//     fgets(client_input, sizeof(client_input), stdin);

//     // Extract the first character from user input
//     *client_move = client_input[0]; // Copy the character to client_move

//     // Clear the input buffer (consume remaining characters, including newline)
//     int c;
//     while ((c = getchar()) != '\n' && c != EOF);

//     // printf("client move sent: %c\n", *client_move); // Use %c to print a single character

//     printf("length of sent move: %lu\n",strlen(client_move));
//     if (send(socket_desc, client_move, 1, 0) < 0) { 
//         printf("Unable to send message\n");
//         return -1;
//     }

//     if (recv(socket_desc, server_move, 1, 0) < 0) { // Receive data into server_move
//         printf("Error while receiving server's move\n");
//         return -1;
//     }

//     return 0;
// }





// bool moveCheck(char *client_move){
//     if(strcmp(client_move,"rock") == 0 || strcmp(client_move,"paper") ==0 || strcmp(client_move,"scissors") == 0){
//             return true;
//         }
//     return false;
// }