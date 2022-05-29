#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>


#define SERVPORT "32768"
#define BACKLOG 10
#define BUFFER_SIZE 65536

int main(void) {
	int status, socketfd, yes, listener;
	char ipstr[INET_ADDRSTRLEN];
	void *addr;
	struct addrinfo hints, *servinfo;
	struct sockaddr_storage their_addr;
	socklen_t their_addr_size = sizeof their_addr;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	if ((status = getaddrinfo(NULL, SERVPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return status;
	}

	socketfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	if (connect(socketfd, servinfo->ai_addr, servinfo->ai_addrlen) < 0) {
		perror("connect");
	}

	freeaddrinfo(servinfo);
	
	char buffer[BUFFER_SIZE] = {'\0'};

	while (1) {
		printf("Your message: %s", buffer);
		fgets(buffer, BUFFER_SIZE, stdin);

		if (send(socketfd, buffer, strlen(buffer), 0) == 0) {
			perror("send");
		} 
		bzero(buffer, sizeof buffer);

		if (recv(socketfd, buffer, BUFFER_SIZE, 0) == 0) {
			perror("recv");
		} else {
			printf("Message from server: %s", buffer);
			if (strcmp("exit\n", buffer) == 0) {
				printf("Server closed\n");
				break;
			}
		}
		bzero(buffer, sizeof buffer);
	}

	close(socketfd);
	return 0;
}
