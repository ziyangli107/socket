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


#define PORT "26838" // the port client will be connecting to 

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


	printf("The monitor is up and running\n"); //Boot up

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



	freeaddrinfo(servinfo); // all done with this structure

	while(1)
	{
		numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0);
		buf[numbytes] = '\0';


		char charlist[20][20];
		int i = 0;
		char *substr = strtok(buf, " ");
		while(substr !=NULL)
		{
			strcpy(charlist[i],substr);
			i++;
			substr = strtok(NULL," ");
		}
		printf("The monitor received link ID=<%s>, size=<%s>, and power=<%s> from AWS\n",charlist[0],charlist[1],charlist[2]);
		memset(&buf, 0, sizeof buf);
		numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0);
		buf[numbytes] = '\0';

		if(numbytes == 1)
		{
			printf("Found no matches for link<%s>\n",charlist[0]);
		}
		else
		{
			char delaylist[20][20];
			int j = 0;
			char *substr1 = strtok(buf, " ");
			while(substr1 !=NULL)
			{
				strcpy(delaylist[j],substr1);
				j++;
				substr1 = strtok(NULL," ");
			}

			float tt = atof(delaylist[0]);
			float tp = atof(delaylist[1]);
			float total = atof(delaylist[2]);
			printf("The result for link <%s>:\nTt=<%.2f>ms\nTp=<%.2f>ms\nDelay=<%.2f>ms\n",charlist[0],tt,tp,total);
		}
		memset(&buf, 0, sizeof buf);
	}
	close(sockfd);
	
	return 0;
}

