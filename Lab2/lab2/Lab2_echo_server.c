/* A simple echo server using TCP */
/* A simple echo server takes a string as an input (from sender or client) and returns the same string back to the client/sender */
/* Initialition for such server is "./server port_num". Once server starts, it starts listening from the port number provided. Starts listening for client input */

#include <stdio.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <strings.h>

#define SERVER_TCP_PORT 3000 // well-known port 
#define BUFLEN		256  // buffer length for send and recieve 

int echod(int);
void reaper(int);

int main(int argc, char **argv){
	
	struct sockaddr_in server, client; //server and client structure of type sockaddr_in. Contain port server is bound 							 
	int sd; //server process socket descriptor 
	int new_sd; //client socket descriptor. When new client connects to us. renew this topic
	int client_len;	//length of data/message sent 
	int port;//port 
	
	switch(argc){
	case 1:
		port = SERVER_TCP_PORT;
		break;
	case 2:
		port = atoi(argv[1]);
		break;
	default:
		fprintf(stderr, "Usage: %s [port]\n", argv[0]);
		exit(1);
	}

/*Create a stream socket*/	
//Start server process by making socket. 
//Server makes socket system call to create socket --> access TCP/IP service
//Call returns a descriptor (sd) --> sd has function similar to file descriptor
//SOCK_STREAM --> specifies that server will use TCP as transport-layer service 
//Check for error is occuring. Error checking is important for socket programming
//if there is error in creating socket, then print "Can't create a socket" message. 

	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		fprintf(stderr, "Can't creat a socket\n");
		exit(1);
	}

//Once socket is obtained/created without any errors, we fill server data structure.

	bzero((char *)&server, sizeof(struct sockaddr_in)); 
	server.sin_family = AF_INET;
	server.sin_port = htons(port);//Which port to bind to?Input(argv[1]) converted to integer and host byte order to network byte(htons)
	server.sin_addr.s_addr = htonl(INADDR_ANY); //Listen on all interfaces on local manchine (host) 0.0.0.0
	
//Once socket struture has been obtained, we bind to port providing socket created (sd) and server structure we created above. 
/*Bind an address  to the socket*/
//Server calls bind() to assign IP address and port number to  socket that was just opened above.
//Must check for errors again.

	if (bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1){
		fprintf(stderr, "Can't bind name to socket\n");
		exit(1);
	}

//Once binding is successful in binding to port without any errors, use listen() function to listen to max number of clients. 
/*queue up to 5 connect requests*/
//Server calls listen to place server in passive (server) mode.
//Tells TCP module to enqueue TCP connection with maximum queue size of 5. 

	listen(sd, 5);

	(void) signal(SIGCHLD, reaper);

//wait for client connection and then start echoing mechanism. 

	while(1){
	  client_len = sizeof(client);
	  new_sd = accept(sd, (struct sockaddr *)&client, &client_len);//accept() blocking call until client doesnt connect. Server waiting
	 
	  if(new_sd < 0){
	    fprintf(stderr, "Can't accept client \n");//If client is not connected to server successfully, print message. 
	    exit(1);
	  }
	exit(echod(new_sd));//If client is connected to server without errors, then exit and go to echod(new_sd) function. 
	}
}


/*echod program*/
//Once client is connected, exit takes you to this function. 

int echod(int sd){

	char	*bp, buf[BUFLEN];
	int 	n, bytes_to_read;
	
//Initial code had while loop to read message sent by client and then server had to send that message back.
//Replace that with write statement where server sends a message saying "hello" to client. 

	write(sd, "Hello", sizeof("Hello"));		 

//As per requirement, once the server sends the client a hello message, server must close TCP connection and exit. 

	close(sd);
	return(0);
}


/*reaper*/

void reaper(int sig){
	int	status;
	while(wait3(&status, WNOHANG, (struct rusage *)0) >= 0);
}
