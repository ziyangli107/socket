/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>


#define PORT "25838" // the port client will be connecting to 

#define MAXDATASIZE 1024 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	if (argc != 4) {
	    fprintf(stderr,"input error\n");
	    exit(1);
	}
	char *link = argv[1];
	char *size = argv[2];
	char *power = argv[3];
	int intLink = atoi(argv[1]);
	int intSize = atoi(argv[2]);
	int intPower = atoi(argv[3]);
	
	printf("The client is up and running\n"); //Boot up

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;


	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
    	
	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}
	//Send the input to AWS
	printf("The client sent ID=%d, size=%s, and power=%s to AWS\n",intLink,size,power); 

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	//printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure
	char * msg = (char *) malloc(strlen(link) + strlen(size) + +strlen(power) + 2);
	strcpy(msg,link);
	strcat(msg," ");
	strcat(msg,size);
	strcat(msg," ");
	strcat(msg,power);

	send(sockfd,msg,strlen(msg),0);
/*
	int len1 = strlen(argv[1]);
	send(sockfd,argv[1],len1,0);
	send(sockfd," ",1,0);
	int len2 = strlen(argv[2]);
	send(sockfd,argv[2],len2,0);
	send(sockfd," ",1,0);
	int len3 = strlen(argv[3]);
	send(sockfd,argv[3],len3,0);
*/
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}

	buf[numbytes] = '\0';

	if(numbytes == 1)
	{
		printf("Found no matches for link<%d>\n",intLink);
	} 
	else
	{
		float delay =atof(buf);
		printf("The delay for link<%d> is <%.2f>ms\n",intLink,delay);
	} 
	close(sockfd);

	return 0;
}

