#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#define BUF_SIZE 5

int main (int argc, char* argv[]){
 
   if(argc < 4){
      fprintf(stderr,"Not enough arguments.\n%s [ciphertext] [key] [port]\n", argv[0]);
      return 1;
   }

   //IO Parameters
   int cipherfd, keyfd, cipherfsize, keyfsize, nProgress, nTemp;

   //Socket Parameters 
   int sockfd, portno, n;
   struct sockaddr_in serv_addr;
   struct hostent* server;

   //Buffers
   char buffer[BUF_SIZE], keybuf[BUF_SIZE];

   //Open ciphertext file for use
   cipherfd = open(argv[1], O_RDONLY);
   if(cipherfd == -1){
      fprintf(stderr,"Client: Can't open %s\n", argv[1]);
      return 1;
   }

   //Get size of ciphertext file
   cipherfsize = lseek(cipherfd, 0, SEEK_END);
   lseek(cipherfd, 0, 0);

   //Open key file for use
   keyfd = open(argv[2], O_RDONLY);
   if(keyfd == -1){
      fprintf(stderr,"Client: Can't open %s\n", argv[2]);
      return 1;
   }
   
   //Get size of key file
   keyfsize = lseek(keyfd, 0, SEEK_END);
   lseek(keyfd, 0, 0);

   //Check if key file size is smaller than ciphertext file size
   if(keyfsize < cipherfsize){
      fprintf(stderr,"Client: Key file size too small for ciphertext file\n");
      close(cipherfd);
      close(keyfd);
      return 1;
   }
 
   //Get socket file descriptor 
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if(sockfd < 0){
      fprintf(stderr,"Client: Can't open socket\n");
      return 1;
   }

   //Use localhost as server
   server = gethostbyname("localhost");
   if (server == NULL){
      fprintf(stderr,"Client: No such host\n");
      return 1;
   }

   //Set up server address information
   memset(&serv_addr, 0, sizeof(struct sockaddr_in));
   portno = atoi(argv[3]);
   serv_addr.sin_family = AF_INET;
   bcopy((char*)server->h_addr,
	 (char*)&serv_addr.sin_addr.s_addr,
	        server->h_length);
   serv_addr.sin_port = htons(portno);

   //Connect to server
   if(connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0){
      fprintf(stderr,"Client: Connection problems\n");
      return 1;
   }

   //Write DEC identifier '<' to server
   char identifier = '<';
   write(sockfd, &identifier, sizeof(char));
   read(sockfd, &identifier, sizeof(char));
   if(!identifier){
      fprintf(stderr,"Client: Could not validate with server\n");
      return 1;
   }

   //Write file size to server
   write(sockfd, &cipherfsize, sizeof(int)); 
   read(sockfd, &nTemp, sizeof(int));

   //Validate file size is correct
   if(nTemp != cipherfsize){
      fprintf(stderr,"Client: Error in file size communication\n");
      return 1;
   }

   //Start feeding chunks of data to server    
   nProgress = 0;
   while(nProgress < cipherfsize){
//      printf("nProgress = %i, cipherfsize = %i\n", nProgress, cipherfsize);

      //Zero buffer
      bzero(buffer,BUF_SIZE);

      //Get buffer full of text from file 
      n = read(cipherfd, buffer, BUF_SIZE);

      //Write buffer to server
      n = write(sockfd, buffer, n);

//      printf("Client: Sending message \"%s\"\n", buffer);

      //If error writing to server, update user and stop sending info
      if(n < 0){
	 fprintf(stderr,"Client: Error writing to socket\n");
	 break;
      }

      //Zero buffer
      bzero(buffer,BUF_SIZE);

      //Read returned ciphertext from server
      nTemp = read(sockfd, buffer, n);
    
      if(n < 0){
	 fprintf(stderr,"Client: Error reading from socket\n");
	 break;
      } else {
//	 printf("Client Received: %s\n", buffer);
      }

      //Zero keybuffer
      bzero(keybuf, BUF_SIZE);

      //Get keybuf full of text from key file
      nTemp = read(keyfd, keybuf, n);
//      printf("nTemp: %i n: %i\n", nTemp, n);

      //If key isn't long enough, stop sending data to server
      if(nTemp < n){
	 fprintf(stderr,"Client: Key is too small\n");
	 break;
      }

      //Write keybuf to server
      nTemp = write(sockfd, keybuf, n);

//      printf("Client: Key written %s\n", keybuf);
      if(nTemp < 0){
	 fprintf(stderr,"Client: Error writing key to socket\n");
	 break;
      }

      //Read encoded key back
      nTemp = read(sockfd, buffer, n);
      printf("%s", buffer);

      nProgress += n;
//      printf("\n");
   }


   close(cipherfd);
   close(keyfd);
   close(sockfd);

   return 0;
}
