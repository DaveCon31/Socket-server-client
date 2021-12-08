#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SIZE 1024
#define PORT_NUMBER atoi(argv[1])
#define LOCAL_HOST "127.0.0.1"
#define READY "developer.txt"
#define NOFILE "File not found"
#define YESFILE "Available"

void init_setup(int argc, char *argv[])
{
	if (argc < 2)
		exit(1);
	if (PORT_NUMBER < 1023) {
		printf("Well known port selected. Not allowed! exiting ...\n");
		exit(1);
	}
}

void check(int e)
{
	if (e == -1) {
		perror("Error");
		exit(1);
	}
}

void send_file(int socketfd)
{
	FILE *fp;
	unsigned char data[SIZE] = {0};
	int nread = 0;
	
	fp = fopen(READY, "rb");
	if (fp == NULL) {
		perror("File descriptor error");
		return;
	}
	
	while (1) {
		nread = fread(data, 1, SIZE, fp); 
		printf("Bytes read %d \n", nread);
		if (nread > 0)
			write(socketfd, &data, nread);
		
		if (nread < SIZE) {
			if (feof(fp))
				printf("End of file\n");
			if (ferror(fp))
				printf("Error reading\n");
			break;
		}
	}
}

int main(int argc, char *argv[])
{	
	init_setup(argc, argv);
	char buff_req[SIZE];
	int server_socket = 0, checker = 0;
	int client_socket = 0;
	struct sockaddr_in server_address;
	
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	check(server_socket);
	memset(&server_address, 0, sizeof(server_address));
	memset(buff_req, 0, sizeof(buff_req));
	
	/* server address */
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT_NUMBER);    //understands data from port number 
	server_address.sin_addr.s_addr = inet_addr(LOCAL_HOST);
	
	/* bind to specified IP and port */
	checker = bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));
	check(checker);
	
	checker = listen(server_socket, 10);
	check(checker);
	
	while (1) {
		client_socket = accept(server_socket, NULL, NULL);
		checker = recv(client_socket, &buff_req, sizeof(buff_req), 0);
		if (checker == -1)
			perror("Receiving request error");
		printf("Request from client: %s\n", buff_req);
		
		if (strcmp(buff_req, READY) == 0) {
			send(client_socket, &YESFILE, sizeof(YESFILE), 0);
			recv(client_socket, &buff_req, sizeof(buff_req), 0);
			printf("%s!\n",buff_req);
			send_file(client_socket);
		} else {
			send(client_socket, &NOFILE, sizeof(NOFILE), 0);
			printf("%s\n", NOFILE);
		}
		close(client_socket);
	}
	
	close(server_socket);
	return 0;
}
