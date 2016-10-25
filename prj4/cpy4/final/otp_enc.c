
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER 1000000

int main(int argc, char** argv)
{
	char buff1[BUFFER];
	char buff2[BUFFER];
	char buff3[1];
	
	int port;		// port number
	int sockfd;		// socket id
	int fd;			// file descriptor
	int i;			// iterator
	int trans;		// transmitting
	int ack;		// reciving
	int textLength;		// size of string to encode 
	int keyLength;		// size of key

	struct sockaddr_in serv_addr;	// server address
	struct hostent *server;		// server
	
	if(argc < 4 )	// usage check
	{
		printf("Usage: otp_enc <textfile> <keyfile> <port>\n");
		exit(1);
	}


	sscanf(argv[3], "%d", &port);	// get port form commandline
	if(port < 0 || port > 65000)	// check port number
	{
		printf("otp_enc: %d is an invalid port\n", port);
		exit(2);
	}

	
	fd = open(argv[1], O_RDONLY);	// open file with text to encode
	if( fd < 0)			// check for open properly
	{
		printf("otp_enc: cannot open %s\n", argv[1]);
		exit(1);
	}	

	textLength = read(fd,buff1, BUFFER);	// read text into buffer
	
	for(i = 0; i < textLength - 1; i++)	// for the whole string
	{
		if((int) buff1[i] > 90 || ((int) buff1[i] < 65 && (int) buff1[i] != 32)) // assert all are uppercase or space
		{
			printf("otp_enc: %s contains invalid characters\n", argv[1]);
			exit(1);
		}
	}

			
	close(fd);	// close file

	fd = open(argv[2], O_RDONLY); 	// open key file
	if( fd < 0)			// check that it opened properly
	{
		printf("otp_enc: cannot open %s\n", argv[2]);
		exit(1);
	}	

	

	keyLength = read(fd,buff2, BUFFER);	// read key file into 2nd buffer  and get its length
	
	for(i = 0; i < keyLength - 1; i++)	// assert characters are upper case or space
	{
		if((int) buff2[i] > 90 || ((int) buff2[i] < 65 && (int) buff2[i] != 32))
		{
			printf("otp_enc: %s contains invalid characters\n", argv[2]);
			exit(1);
		}
	}

			
	close(fd);	// close key file

					
					
	if(keyLength < textLength)	// assert key is longer than text
	{
		printf("otp_enc: key is too short\n");
		exit(1);
	}

							
	sockfd = socket(AF_INET, SOCK_STREAM, 0);	// initalize socket
	if(sockfd < 0)					// assert proper initialization
	{
		printf("otp_enc: could not establish connection on port %d\b", port);
		exit(2);
	}

							
	memset(&serv_addr, '\0', sizeof(serv_addr));	// fill server structure with NULL

	server = gethostbyname("localhost");		// get value of local host
	if(server == NULL)				// assert that this worked
	{
		printf("otp_enc: could not connect to otp_enc_d\n");
		exit(2);
	}

	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
		server->h_length);
	serv_addr.sin_port = htons(port);

	if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)	// make a connection
	{										// assert success
		printf("otp_enc: could not connect to otp_enc_d on port %d\n", port);
		exit(2);
	}
	
	trans = write(sockfd, buff1, textLength - 1);	// transmit the text to the connection
	if(trans < textLength -1)			// assert all sent
	{
		printf("otp_enc: send failure on port %d\n", port);
		exit(2);
	}

				
	memset(buff3, 0, 1);	// set buff3 to zero
	
	ack = read(sockfd, buff3, 1);	// get acknoledgement
	if(ack < 0)			// assert ack received
	{
		printf("otp_enc: no acknoledgment received form otp_enc_d\n");
		exit(2);
	}

							
	trans = write(sockfd, buff2, keyLength -1);	// now send the key
	if(trans < keyLength - 1)			// assert all sent
	{
		printf("otp_enc: faild to send key on port %d\n", port);
		exit(2);
	}


	memset(buff1, 0, BUFFER);	// set entire buffer1 to 0
	
	
	// wait for response	
	do	
	{
		ack = read(sockfd, buff1, textLength -1);	// receive reponce
	}
	while(ack > 0);	

	if(ack < 0)  // if the read fails, error
	{
		printf("otp_enc: error occoured while retreving cyphertext\n");
		exit(2);
	}

	for(i = 0; i < textLength -1; i++) 	// loop to print to received string
	{
		printf("%c", buff1[i]);
	}
	printf("\n");	// clear terminal
			
	close(sockfd);	// close connection
		
	return 0;
 

} 
