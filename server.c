#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MY_PORT "32768"
#define BACKLOG 10
#define BUFFER_SIZE 65536

int main(void) {
	int status, socketfd, new_fd, yes, listener;
	char ipstr[INET_ADDRSTRLEN];
	void *addr;
	struct addrinfo hints, *servinfo;
	struct sockaddr_storage their_addr;
	socklen_t addr_size = sizeof their_addr;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	if ((status = getaddrinfo(NULL, MY_PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return status;
	}

	setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);

	socketfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);


	if ((status = bind(socketfd, servinfo->ai_addr, servinfo->ai_addrlen)) < 0) {
		fprintf(stderr, "bind: error");
	}

	if ((listen(socketfd, BACKLOG) < 0)) {
		fprintf(stderr, "error while listening");
	}

	freeaddrinfo(servinfo);

	addr_size = sizeof their_addr;
	if ((new_fd = accept(socketfd, (struct sockaddr*)&their_addr, &addr_size)) < 0) {
		perror("accept");
	}

	char buffer[BUFFER_SIZE] = {'\0'};
	
	while (1) {
		if (recv(new_fd, buffer, sizeof buffer, 0) == 0) {
			perror("recv");
		} else {
			printf("Message from client: %sYour message: ", buffer);
			bzero(buffer, sizeof buffer);
		}
		fgets(buffer, BUFFER_SIZE, stdin);
		if (send(new_fd, buffer, strlen(buffer), 0) == 0)
			perror("send");
		if (strcmp("exit\n", buffer) == 0) {
			printf("Server exit...");	
			break;
		}
		bzero(buffer, sizeof buffer);
	}
	close(socketfd);
	return 0;
}
