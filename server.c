#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#define PORT 2567


void doprocessing (int sock);
int checkEnding(char * buffer);
void writeInfoToFile(char * buffer, int portno);


int main( int argc, char *argv[] ) {
   int sockfd, newsockfd, portno, clilen;
   char buffer[133];
   struct sockaddr_in serv_addr, cli_addr;
   int n, pid;
   
   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = PORT;
   
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
   
   /* Now start listening for the clients, here
      * process will go in sleep mode and will wait
      * for the incoming connection
   */
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   while (1) {
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		
      if (newsockfd < 0) {
         perror("ERROR on accept");
         exit(1);
      }
      
      /* Create child process */
      pid = fork();
		
      if (pid < 0) {
         perror("ERROR on fork");
         exit(1);
      }
      
      if (pid == 0) {
         /* This is the client process */
         close(sockfd);
         doprocessing(newsockfd);
         exit(0);
      }
      else {
         close(newsockfd);
      }
		
   } /* end of while */
}

void doprocessing (int sock) {
   int n;
   char buffer[133];
   bzero(buffer,133);
   //const char * ending = "\r\n";
   char * accept_mes = "+ACCEPTED\r\n";
   char * error_mes = "-PROTOCOL ERROR\r\n";

   n = read(sock,buffer,133);
   
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }
   
   if(checkEnding(buffer)==-1){
   	n = write(sock, error_mes, strlen(error_mes));
   	bzero(buffer,133);
   	writeInfoToFile(error_mes, PORT);
   	printf("Here is the message: %s\n",buffer);
   }else{
    n = write(sock, accept_mes, strlen(accept_mes));
    writeInfoToFile(accept_mes, PORT);
    printf("Here is the message: %s\n",buffer);
   }
   
   

   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
	
}

int checkEnding(char * buffer){
	const char * res;
	res = strchr(buffer, '\\');
	if(res == NULL) return -1;
	if(strcmp("\\r\\n\n", res)==0){
		return 0;
	}
	
	return -1;
}

void writeInfoToFile(char * buffer, int portno){
	FILE * fp;
	char text[24];
	time_t now = time(NULL);
	struct tm * t = localtime(&now);
	strftime(text, sizeof(text)-1, "%Y-%m-%d %H:%M:%S", t);
	text[23] = 0;
	char  port_s[10];  
	sprintf(port_s, "%d", portno);

	const char * filename = "logfile.txt";
	fp = fopen(filename, "a+");
	if(!fp) perror("Unable to open the file");
	fputs("\n", fp);
	fputs(text, fp); //date and time 
	fputs("\t", fp);
	fputs("OUT\tport number: ", fp);
	fputs(port_s, fp);
	fputs("\tmessage: ", fp);
	fputs(buffer, fp); //buffer
	fclose(fp);
}

