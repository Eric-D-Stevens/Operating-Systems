#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER 1048576

int main(int argc, char** argv)
{

	char buff1[BUFFER];
	char buff2[BUFFER];
	char buff3[BUFFER];
			
	int sockfd;	// parent socket
	int sockfd2;	// child socket
	int i;		// iterator
	int keyLength;	// length of received key
	int trans;	// length of data to send
	int id;		// process id
	int textLength;	// length of text received
	int port;	// port
	
	socklen_t sLength;	

	struct sockaddr_in server, client; // server and client declaration
	if(argc < 2)	// assert proper usage
	{
		printf("Usage: otp_enc_d <port>\n");
		exit(1);
	}

	sscanf(argv[1], "%d", &port); 	// assert vaid port
	if(port < 0 || port > 65000)
	{	
printf("\nFUCK\n");
		printf("otp_enc_d: %d is an invalid port number\n", port);
		exit(2);
	}	

	
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)	// assert parent working
	{
		printf("otp_enc_d: could not to socket on port%d\n",port);
		exit(2);
	}

	
	memset(&server, '\0', sizeof(server));	// fill struct  with NULL

	server.sin_family = AF_INET;		// initalize server
	server.sin_addr.s_addr = INADDR_ANY;	
	server.sin_port = htons(port);

	if(bind(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0)	// bind and assert no error
	{
		printf("otp_enc_d: failure to bind on port%d\n", port);
		exit(2);
	}
				
	listen(sockfd, 5);	// wait on port, allow 5 simultanious connections
	
	sLength = sizeof(client);	
	while(1)
	{	
		sockfd2 = accept(sockfd, (struct sockaddr *)&client, &sLength);	// allow second socket to accept connection
		if(sockfd2 < 0)	// assert connection
		{
			printf("otp_enc_d: failure to accept connection\n");
			
		}

		id = fork();	// for the process off
	
		if(id<0)	// assert fork success
		{
			perror("otp_enc_c: failure to fork\n");
		}
	
		if(id==0)	// if this is a child process
		{				
			memset(buff1, 0, BUFFER);			// clear buffer
			textLength = read(sockfd2, buff1, BUFFER);	// write socket to buffer
			if(textLength < 0)	// assert sucesfull write
			{
				printf("enc_otp_d: failure to read on port %d\n", port);
				exit(2);
			}
								
			trans = write(sockfd2, "^", 1);	// transmit ack character
	
			memset(buff2, 0, BUFFER);	// clear the second buffer
	
			keyLength = read(sockfd2, buff2, BUFFER);	// write key to second buffer
			
			for(i = 0; i < textLength; i++)	// for the length of the received text
			{	// assert that all characters are proper
				if((int) buff1[i] > 90 || ((int) buff1[i] < 65 && (int) buff1[i] != 32))
				{
					printf("otp_enc_d: text contains bad characters\n");
	
					exit(1);
				}
			}
					
								
			for(i = 0; i < keyLength; i++)	// for the length of the received key
			{	// assert that all the key characters are proper
				if((int) buff2[i] > 90 || ((int) buff2[i] < 65 && (int) buff2[i] != 32))
				{
					printf("otp_enc_d: key contains bad characters\n");
					exit(1);
				}
			}
	
	
			
			// assert that key is longer than text
			if(keyLength < textLength)
			{
				printf("otp_end_d: key too short\n");
				exit(1);
			}
	
			
			for(i = 0; i < textLength; i++) // for entire text
			{
				if(buff1[i] == ' '){buff1[i]='@';}	// change space to asci Z+1	
				if(buff2[i] == ' '){buff2[i]='@';}	
	
				// Normalize ASCII chars
				int c = (int) buff1[i];
				c = c - 64;
	
				int k = (int) buff2[i];
				k = k - 64;			
				
				int ctext = ((c+k)%27)+64; // generate cypher char
				
				buff3[i] = (char) ctext; // place in buffer 3
				if(buff3[i] == '@'){buff3[i] = ' ';}	// replace all Z+1 with ' '
			}
	
			
			trans = write(sockfd2, buff3, textLength);	// write cypher text back to socket
			if(trans < textLength)				// assert it was sent
			{
				printf("otp_enc_d: bad write\n");
				exit(2);
			}
	
			close(sockfd2);
			close(sockfd);		
			
			exit(0);
		
		}
		
		else close(sockfd2);
	}

	return 0;
}

