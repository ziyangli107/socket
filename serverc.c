#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <math.h>
#define PORT "23838"

#define MAXDATASIZE 1024


int main(void)
{
	int sockfd;  
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_in client_addr;
	socklen_t sin_size;
	int yes=1;
	int rv;
	char buf[MAXDATASIZE];
	int numbytes,send;
	float  signal,bandwidth,size,noise,capacity,velocity,length;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;	
	hints.ai_socktype = SOCK_DGRAM; //use UDP
	hints.ai_flags = AI_PASSIVE; // use my IP

	

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	printf("The Server C is up and running using UDP on port <%s>\n",PORT); //Boot up


	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}



	while(1) {  // main accept() loop
		sin_size = sizeof client_addr;

		if ((numbytes = recvfrom(sockfd, buf, MAXDATASIZE-1, 0, (struct sockaddr*) &client_addr, &sin_size)) == -1) {
	
	    		perror("recv");
	    		exit(1);
		}
		buf[numbytes] = '\0';
		char seg[] = ",";
		char charlist[16][64];
		int i = 0;
		char *substr = strtok(buf, seg);
		while(substr !=NULL)
		{
			strcpy(charlist[i],substr);
			i++;

			substr = strtok(NULL,seg);
		}		
		bandwidth = atof(charlist[1]);
		length = atof(charlist[2]);
		velocity = atof(charlist[3]);
		noise = atof(charlist[4]);
		size = atof(charlist[5]);
		signal = atof(charlist[6]);

		printf("The server C received link information of link<%s>, file size<%.0f>, and signal power<%.2f>\n",charlist[0],size,signal);

		float newsignal,newnoise;
		newsignal =pow(10,signal/10.0)/1000;
		newnoise =pow(10,noise/10.0)/1000;	
		capacity = bandwidth*pow(10,6)*(log10(1+newsignal/newnoise)/log10(2));	
		float tt = size*1000/capacity ;
		float tp = length*1000/(velocity*(pow(10,4)));
		float delay = (tt+tp);
		char *trans = (char *) malloc(8);
		char *prop = (char *) malloc(8);
		char *total = (char *)malloc(8);
		char *data = (char *) malloc(8 *3 + 2);
		gcvt(tt, 8, trans);
		gcvt(tp, 8, prop);
		gcvt(delay, 8, total);
		strcpy(data,trans);
		strcat(data," ");
		strcat(data,prop);
		strcat(data," ");
		strcat(data,total);

		printf("The server C finished the caculation for link<%s>\n",charlist[0]);
		send = sendto(sockfd, data, strlen(data), 0, (struct sockaddr*) &client_addr , sin_size);
		printf("The server C finished sending the output to AWS\n");
		
	}

	return 0;
}

