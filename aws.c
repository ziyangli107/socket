/*
** server.c -- a stream socket server demo
*/

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

#define PORT "25838"  // the port users will be connecting to
#define MONITOR_PORT "26838"
#define SERVERA_PORT 21838
#define SERVERB_PORT 22838
#define SERVERC_PORT 23838
#define TOA_PORT "24838"
#define TOB_PORT "28838"
#define TOC_PORT "27838"
#define BACKLOG 1024	 // how many pending connections queue will hold
#define MAXDATASIZE 4096
//char tempmsg[100];
void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

char *serverAB(char *link)
{

	int sockfdA,sockfdB;  
	int sendA,sendB;
	int receiveA,receiveB;
	char bufA[MAXDATASIZE],bufB[MAXDATASIZE];
	struct addrinfo hints, *servinfoA, *servinfoB,*p;
	struct sockaddr_in server_addrA, server_addrB;
	socklen_t lenA,lenB;
	int rvA,rvB;
	int yes = 1;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = 0 ;
	rvA = getaddrinfo(NULL, TOA_PORT, &hints, &servinfoA);
	rvB = getaddrinfo(NULL, TOB_PORT, &hints, &servinfoB);

	for(p = servinfoA; p != NULL; p = p->ai_next) {
		if ((sockfdA = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfdA, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfdA, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfdA);
			perror("server: bind");
			continue;
		}

		break;
	}	
	freeaddrinfo(servinfoA); // all done with this structure
	for(p = servinfoB; p != NULL; p = p->ai_next) {
		if ((sockfdB = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfdB, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfdB, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfdB);
			perror("server: bind");
			continue;
		}

		break;
	}
	freeaddrinfo(servinfoB); // all done with this structure
	memset(&server_addrA, 0, sizeof server_addrA);
	server_addrA.sin_family = AF_INET;
	server_addrA.sin_port = htons(SERVERA_PORT);
	server_addrA.sin_addr.s_addr = inet_addr("127.0.0.1");
	lenA = sizeof(server_addrA);

	memset(&server_addrB, 0, sizeof server_addrB);
	server_addrB.sin_family = AF_INET;
	server_addrB.sin_port = htons(SERVERB_PORT);
	server_addrB.sin_addr.s_addr = inet_addr("127.0.0.1");
	lenB = sizeof(server_addrB);

	sendA = sendto(sockfdA, link, strlen(link), 0, (struct sockaddr*) &server_addrA , lenA);
	printf("The AWS sent link ID =<%s> to Backend-server A using UDP over port <%s>\n",link,TOA_PORT);
	sendB = sendto(sockfdB, link, strlen(link), 0, (struct sockaddr*) &server_addrB , lenB);
	printf("The AWS sent link ID =<%s> to Backend-server B using UDP over port <%s>\n",link,TOB_PORT);

	receiveA = recvfrom(sockfdA, bufA, MAXDATASIZE-1, 0, (struct sockaddr*) &server_addrA, &lenA);
	bufA[receiveA - 1] = '\0';
	receiveB = recvfrom(sockfdB, bufB, MAXDATASIZE-1, 0, (struct sockaddr*) &server_addrB, &lenB);
	bufB[receiveB - 1] = '\0';

	char *des;
	des = (char *)malloc(32);

	if( receiveA != 4)
	{
		strcpy(des, bufA);
		printf("The AWS received <1> matches from Backend-server <A> using UDP over port<%s>\n",TOA_PORT);
		printf("The AWS received <0> matches from Backend-server <B> using UDP over port<%s>\n",TOB_PORT);
		//strcpy(tempmsg , bufA);
		//return tempmsg;
	} 
	else if( receiveB != 4)
	{
		printf("The AWS received <0> matches from Backend-server <A> using UDP over port<%s>\n",TOA_PORT);
		printf("The AWS received <1> matches from Backend-server <B> using UDP over port<%s>\n",TOB_PORT);
		strcpy(des, bufB);
		//strcpy(tempmsg , bufB);
		//return tempmsg;
	} 
	else
	{
		printf("The AWS received <0> matches from Backend-server <A> using UDP over port<%s>\n",TOA_PORT);
		printf("The AWS received <0> matches from Backend-server <B> using UDP over port<%s>\n",TOB_PORT);
		strcpy(des, "none");
	}
	close(sockfdA);
	close(sockfdB);
	return des;
}


char *serverC(char *info, char *link)
{
	int sockfd;
	int send;
	int receive;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo,*p;
	struct sockaddr_in server_addr;
	socklen_t len;
	int rv;
	int yes = 1;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = 0 ;
	rv = getaddrinfo(NULL, TOC_PORT, &hints, &servinfo);


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

	memset(&server_addr, 0, sizeof server_addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVERC_PORT);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	len = sizeof(server_addr);

	send = sendto(sockfd, info, strlen(info), 0, (struct sockaddr*) &server_addr , len);	
	printf("The AWS sent link ID =<%s> to back-end server C using UDP over port <%s>\n",link,TOC_PORT);
	receive = recvfrom(sockfd, buf, MAXDATASIZE-1, 0, (struct sockaddr*) &server_addr, &len);
	buf[receive] = '\0';
	printf("The AWS received outputs from Backend-server C using UDP over port <%s>\n",TOC_PORT);
	char *des;
	des = (char *)malloc(64);
	strcpy(des,buf);
	close(sockfd);
	return des;

}
int main(void)
{
	int sockfd, new_fd, monitor_fd,new_mfd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p,*monitorinfo;
	struct sockaddr_storage client_addr,monitor_addr; // connector's address information
	socklen_t client_size,monitor_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	char s1[INET6_ADDRSTRLEN];
	int rv,mrv;
	char buf[MAXDATASIZE];

	int numbytes;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;	
	hints.ai_socktype = SOCK_STREAM; //use TCP
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	if ((mrv = getaddrinfo(NULL, MONITOR_PORT, &hints, &monitorinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(mrv));
		return 1;
	}

	printf("The AWS is up and running\n"); //Boot up
	
	// loop through all the results and bind to the first we can

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
	for(p = monitorinfo; p != NULL; p = p->ai_next) {
		if ((monitor_fd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: monitor socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(monitor_fd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server:monitor bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure
	freeaddrinfo(monitorinfo); // all done with this structure
	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}
	if (listen(monitor_fd,BACKLOG) == -1){
		perror("listen");
		exit(1);
	}
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	//accept monitor
	monitor_size = sizeof monitor_addr;
	new_mfd = accept(monitor_fd, (struct sockaddr *)&monitor_addr, &monitor_size);
	char msg[100];
	char * data;	
	while(1) {  
		//accept client
		client_size = sizeof client_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		//receive client
		if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) {
	
	    		perror("recv");
	    		exit(1);
		}
		buf[numbytes] = '\0';

		strcpy(msg,buf);
		//send to monitor
		send(new_mfd,msg,strlen(msg),0);
		memset(&msg, 0, sizeof msg);		

		char charlist[20][20];
		int i = 0;
		char *substr = strtok(buf, " ");
		while(substr !=NULL)
		{
			strcpy(charlist[i],substr);
			i++;

			substr = strtok(NULL," ");
		}
		printf("The AWS received link ID=<%s>, size=<%s>, and power=<%s> from the client using TCP over port<%s>\n",charlist[0],charlist[1],charlist[2],PORT);

		printf("The AWS sent link ID=<%s>, size=<%s>, and power=<%s> to the monitor using TCP over port<%s>\n",charlist[0],charlist[1],charlist[2],MONITOR_PORT);


		//ask server A & B
		data = serverAB(charlist[0]); 

		if(strlen(data)<10)
		{
			printf("The AWS sent No Match to the monitor and the client using TCP over ports <%s> and <%s>, respectively\n",PORT,MONITOR_PORT);
			send(new_fd, "0", 1, 0);
			send(new_mfd, "0", 1, 0);
			close(new_fd);
			free(data);
		}
		else
		{
			char * senddata = (char *) malloc(strlen(data) + strlen(charlist[1]) + +strlen(charlist[2]) + 2);
			strcpy(senddata,data);
			strcat(senddata,",");
			strcat(senddata,charlist[1]);
			strcat(senddata,",");
			strcat(senddata,charlist[2]);

			char *result;
			result = serverC(senddata,charlist[0]);
			char temp[100];
			strcpy(temp,result);


			char seg[] = " ";
			char delaylist[10][20];
			int i = 0;
			char *substr = strtok(result, seg);
			while(substr !=NULL)
			{
				strcpy(delaylist[i],substr);
				i++;
	
				substr = strtok(NULL,seg);
			}
			float totaldelay = atof(delaylist[2]);
			printf("The AWS sent delay=<%.2f> ms to the client using TCP over port <%s>\n",totaldelay,PORT);
	
			send(new_fd, delaylist[2], strlen(delaylist[2]), 0);
			printf("The AWS detailed results to the monitor using TCP over port<%s>\n",MONITOR_PORT);
			send(new_mfd, temp, strlen(temp), 0);

			free(data);
			free(result);
			close(new_fd);

		}	

	}
	close(new_mfd);
	close(sockfd);
	close(monitor_fd);
	return 0;
}

