#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUF_SIZE 5

void childProcess(int,int);

int main (int argc, char* argv[]){

   //Socket Parameters
   int sockfd, newsockfd, portno;
   socklen_t clilen;
   struct sockaddr_in serv_addr, cli_addr;

   //IO Parameters
   int n;
  
   //Buffers 
   char buffer[BUF_SIZE], keybuf[BUF_SIZE];

   //Establish listening socket
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
  
   //If error listening exit 
   if(sockfd < 0){
      fprintf(stderr, "Server: Socket error\n");
      return 1;
   }

   //Prep server address parameters
   memset(&serv_addr, 0, sizeof(struct sockaddr_in));
   portno = atoi(argv[1]);
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);

   //If server fails to bind, exit
   if(bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0){
      fprintf(stderr, "Server: Error binding\n");
      return 1;
   }

   //Set up socket to allow 5 connections at a time
   listen(sockfd, 5);
   clilen = sizeof(cli_addr);

   while(strcmp(buffer, "quit")){

      //Accept a connection from client through sockfd
      newsockfd = accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);

      //If connection fails update user, but server stays online
      if(newsockfd < 0){
	 fprintf(stderr, "Server: Error on accept connection\n");
      }

      //If connection is successful fork of child encryption process
      pid_t pid = fork();

      //Child process
      if (pid == 0){
	 int cipherfsize;
	 char identifier;
	 read(newsockfd, &identifier, sizeof(char));
   	 if(identifier != '<'){
	    identifier = 0;
	    write(newsockfd, &identifier, sizeof(char));
	    exit(1);
	 } else {
	    identifier = 1;
	    write(newsockfd, &identifier, sizeof(char));
	 }

	 //Read in expected ciphertext filesize
	 read(newsockfd, &cipherfsize, sizeof(int));
//	 printf("Server: File size received %i\n", cipherfsize);
	 //Return expected ciphertext filesize for validation
	 write(newsockfd, &cipherfsize, sizeof(int));
	 childProcess(newsockfd, cipherfsize);
	 exit(0);

      //Else wait for completed child processes and keep listening for connections
      } else {
	 int status;
	 waitpid(-1, &status, WNOHANG); 
	 continue;
      }	 
 }

   close(newsockfd);
   close(sockfd);

   return 0;
}

void childProcess(int newsockfd, int cipherfsize){

   int n, nProgress, nTemp;
   char* buffer, * keybuf;

   //Malloc temporary memory for received chunks
   buffer = malloc(BUF_SIZE * sizeof(char));
   keybuf = malloc(BUF_SIZE * sizeof(char));

   nProgress = 0;
   while(nProgress < cipherfsize){

      //Zero buffers
      bzero(buffer, BUF_SIZE);
      bzero(keybuf, BUF_SIZE);

      //Read ciphertext chunk from client
      n = read(newsockfd, buffer, BUF_SIZE);

      //If error, stop listening
      if (n < 0){
	 fprintf(stderr, "Server: Error reading from socket\n");
	 break;
      }
//      printf("Server: Received message %s\n", buffer);
      
      //Write received ciphertext back to client
      nTemp = write(newsockfd, buffer, n);

      if (n < 0){
	 fprintf(stderr, "Server: Error writing to socket\n");
	 break;
      }
      
      //Read Key
      nTemp = read(newsockfd, keybuf, n);

//      printf("Server Received: Key %s\n", keybuf);
      
      //if Read in key chunk < ciphertext chunk, can't encode; stop listening 
      if (nTemp < n){
	 fprintf(stderr, "Server: Key too small\n");
	 break;
      }

      //Decode ciphertext to plaintext
      int i = 0;
      for(i = 0; i < strlen(buffer) - 1; i++){
	 //Sub value 91 for space ('A' = 65, 'B' = 66.. '[' = 91 = ' ')
	 if(buffer[i] == ' '){
	    buffer[i] = 91;
	 } 
	 //Sub value 91 for space character
	 if(keybuf[i] == ' '){
	    keybuf[i] = 91;
	 }
	 //Change range from 65-91 to 0-27
	 buffer[i] = buffer[i] - 65; 

	 //If buffer[i] is outside 0-27, unencryptable character
	 if(buffer[i] < 0 || buffer[i] >= 27){
	    fprintf(stderr, 
		    "Server: False character identified '%c' at location %i\n", 
		    buffer[i] + 65, i);
	    fprintf(stderr, "Character as integer: %i\n", buffer[i] + 65);
	    close(newsockfd);
	    break;
	 }
	 //Subtract key value (in range 0-27) to buffer[i]
	 buffer[i] = buffer[i] - (keybuf[i] - 65);
	 if(buffer[i] < 0){
	    buffer[i] += 27;
	 }
	 //If buffer[i] + keybuf[i] > 27, roll value over to beginning of range
	 buffer[i] = buffer[i] % 27;
	 //Change from range 0-27 back to 65-91
	 buffer[i] = buffer[i] + 65;
	 //Sub ' ' for 91
	 if(buffer[i] == 91){
	    buffer[i] = ' ';
	 }
      }

      //Write encoded chunk back to client
      nTemp = write(newsockfd, buffer, n);
      
      nProgress += n;
   }
  
   //Free local memory 
   free(buffer);
   free(keybuf);

   return;
}
