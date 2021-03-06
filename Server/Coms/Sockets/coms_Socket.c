#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <poll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#include "../coms.h"

struct connection_t {
	int sockFD;
};

void freeConnection(connection * con) {
	return;
}

/* addr is '\0' ended and has an addres and the host name
 * separated by '.'
 * example: 5545.hostname
 */
connection * connectToAddres(char * id) {
	connection * con = malloc(sizeof(connection));
    struct sockaddr_in host_addr;

	struct hostent * host;
	char ip[255];
	char port[6];


	int i = 0;
	while (id[i] != ':') {
		ip[i] = id[i];
		i++;
	}
	ip[i++] = '\0';

	int j = 0;
	while (id[i] != '/') {
		port[j++] = id[i];
		i++;
	}
	port[j] = '\0';

	if (strcmp(ip, "") == 0) {
		host = gethostbyname("localhost");
		if (host == NULL) {
			fprintf(stderr,"ERROR, no such host\n");
			exit(0);
		}
		bcopy((char *)host->h_addr, (char *)&host_addr.sin_addr.s_addr, host->h_length);
	} else {
		host_addr.sin_addr.s_addr = inet_addr(ip);
	}



	int portno = atoi(port);
	con->sockFD = socket(AF_INET, SOCK_STREAM, 0);
	if (con->sockFD < 0) {
		puts("ERROR opening socket");
		exit(1);
	}

	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(portno);
	if (connect(con->sockFD,(struct sockaddr *)&host_addr,sizeof(host_addr)) < 0) {
		puts("ERROR connecting");
		exit(1);
	}


	return con;
}

int openAdress(char * addr) {
	struct sockaddr_in new_addr;
	char port[255];
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);	// openning socket
	int i = 0;
	int j = 0;
	if (sockfd < 0) {
		puts("ERROR opening socket");
		exit(1);
	}

	while (addr[i++] != ':');
	while (addr[i] != '/') {
		port[j++] = addr[i];
		i++;
	}
	port[j] = '\0';

	bzero((char *) &new_addr, sizeof(new_addr));
	new_addr.sin_family = AF_INET;
	new_addr.sin_addr.s_addr = INADDR_ANY;
	new_addr.sin_port = htons(atoi(port));

	if (bind(sockfd, (struct sockaddr *) &new_addr, sizeof(new_addr)) < 0) {
		puts("ERROR on binding");
		exit(1);
	}

	return sockfd;
}

void openConnection(connection* con){

}

connection * readNewConnection(int fd) {	
	connection * con = malloc(sizeof(connection));
	struct sockaddr_in connectedSocket;
	struct pollfd poll_list[2];

	listen(fd,5);	// activate listening from socket (maximum 5 queued connections)

    poll_list[0].fd = fd;
    poll_list[0].events = POLLIN|POLLPRI;

    // poll checks if something was sent to fd
	int readSmth = poll(poll_list, (unsigned long) 1, 10);
	if(readSmth == 0) {
		return NULL;
	}

	int socklen = sizeof(connectedSocket);
	con->sockFD = accept(fd, (struct sockaddr *) &connectedSocket, &socklen); // waiting for client to connect
	if (con->sockFD < 0) {
		puts("ERROR on accept");
		return NULL;
	}
	return con;
}

int sendBytes(connection * con, char * buffer, int len) {	
	//write(con->sockFD, buffer, cant);
	send(con->sockFD, buffer, len, 0);
	return 0;
}

int receiveBytes(connection * con, char * buffer, int len) {
	//return read(con->sockFD, buffer, len);
	return recv(con->sockFD, buffer, len, 0);
}

void endConnection(connection * con) {
	close(con->sockFD);
}