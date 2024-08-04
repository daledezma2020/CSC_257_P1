Dan Ledezma
CSC 257 
October 6, 2023

For this lab I implemented a networked game of Rock Paper Scissors that works over TCP. For this I used a few existing C headers such as sys/socket.h and arpa/inet.h. Both my client
and my server.c files consist of a main function with a few helper functions. The main two helper functions to look at are nameExchange and moveExchange which take a user string input and then wait for the opponent's input. My program initializes name and move strings for the client and server and then plugs them into these functions to make the game take place. It communicates the opponent's name and move and then after those have been done it initiates a countdown implemented using time.h and then uses a few if statements to determine who won the game.

To run my program you must enter ./client <port> on one end and ./server <port> on the other with the same port. It is important that you do the server first so that the client can connect. Then you will be prompted to enter a name and move. Your options for moves are: r, p, or s written those ways. After hitting enter the countdown will begin and you will shortly see the game's results.

EXAMPLE INPUTS:

CLIENT:

./client 25565
Socket created successfully
Connected with server successfully
GIVE ME YOUR NAME: will
Server's name is: dan
Enter your move (r for rock, p for paper, s for scissors): s
Countdown: 3
Countdown: 2
Countdown: 1
dan's move is: r
YOU LOSE :(

SERVER: 

./server 25565
Socket created successfully
Done with binding

Searching for opponent.....
GIVE ME YOUR NAME: dan
client's name is: will
Enter your move (r for rock, p for paper, s for scissors): r
Countdown: 3
Countdown: 2
Countdown: 1
will's move is: s
YOU WIN!
