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

#define PORT "22838"

#define MAXDATASIZE 1024

#define FILE_PATH "database_b.csv"


int get_row(char *filename);
int main(void)
{
	int sockfd;  
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_in client_addr;
	socklen_t sin_size;
	int yes=1;
	int send;
	int rv;
	char buf[MAXDATASIZE];
	int numbytes;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;	
	hints.ai_socktype = SOCK_DGRAM; //use UDP
	hints.ai_flags = AI_PASSIVE; // use my IP

//read
	FILE *fp;
	char line[MAXDATASIZE];
	char temp[MAXDATASIZE];

	if((fp = fopen(FILE_PATH, "r")) ==NULL)
	{
		printf("cannot open file.\n");
	}
	char seg[] = ",";


	

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	printf("The Server B is up and running using UDP on port <%s>\n",PORT); //Boot up


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


	int num;
	int receive;
	int filerow = get_row(FILE_PATH);
	int count = 0;
	while(1) {  // main accept() loop
		sin_size = sizeof client_addr;

		if ((numbytes = recvfrom(sockfd, buf, MAXDATASIZE-1, 0, (struct sockaddr*) &client_addr, &sin_size)) == -1) {
	
	    		perror("recv");
	    		exit(1);
		}
		buf[numbytes] = '\0';
		printf("The server B  received input<%s> \n",buf);


		while(fgets(line,MAXDATASIZE,fp))
		{

			strcpy(temp,line);
			strtok(temp, seg);
			num = atoi(temp);
			receive = atoi(buf);
			if(num == receive)
			{
				send = sendto(sockfd, line, strlen(line), 0, (struct sockaddr*) &client_addr , sin_size);
				printf("The server B has found <1> match\n");
				break;
			}
			count++;
			
		}
		if(count == filerow)
		{
			send = sendto(sockfd, "none", 4, 0, (struct sockaddr*) &client_addr , sin_size);
			printf("The server B has found <0> match\n");
		}	


		printf("The Server B finished sending to the output to AWS\n");
		memset(&line, 0, sizeof line);
		memset(&temp, 0, sizeof temp);
		count = 0;
		fclose(fp);
		fp = fopen(FILE_PATH, "r");

	}
		memset(&buf, 0, sizeof buf);

	return 0;
}
int get_row(char *filename)
{
	char line[MAXDATASIZE];
	int i;
	FILE *p = fopen(filename,"r");
	while(fgets(line,MAXDATASIZE,p))
	{
		i++;
	}

	return i;
}

