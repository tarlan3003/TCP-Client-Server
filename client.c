#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>


int checkEnding(char * buffer);
void writeInfoToFile(char * buffer, int portno);

int main(int argc, char *argv[]) {
   int sockfd, portno, n;
   struct sockaddr_in serv_addr;
   struct hostent *server;
   //const char * ending = "\r\n";

   
   char buffer[133];
   
   if(argc < 3){
   	  perror("Please enter port number and host name\n");
   	  exit(0);
   }
	
   portno = atoi(argv[1]);
   
   /* Create a socket point */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
	
   server = gethostbyname(argv[2]);
   
   if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
   }
   
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(portno);
   
   /* Now connect to the server */
   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR connecting");
      exit(1);
   }
   printf("OK\r\n");
   
   /* Now ask for a message from the user, this message
      * will be read by server
   */
   
   printf("Please enter the message: ");
   bzero(buffer,133);
   fgets(buffer,133,stdin);
   
   writeInfoToFile(buffer, portno);
   /* Send message to the server */
   n = write(sockfd, buffer, strlen(buffer));
   
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
   
   /* Now read server response */
   bzero(buffer,133);
   n = read(sockfd, buffer, 133);
   
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }
	
   printf("%s\n",buffer);
   
   return 0;
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
	fputs("IN\tport number: ", fp);
	fputs(port_s, fp);
	fputs("\tmessage: ", fp);
	fputs(buffer, fp); //buffer
	fclose(fp);
}


