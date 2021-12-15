#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>

#define SIZE 1024
#define LOCAL_HOST "127.0.0.1"
#define NOFILE "File not found"
#define YESFILE "Available"
#define THREAD_POOL_SIZE 10

void handle_broken_pipe(int sig)
{
	printf("Broken pipe before writing to socket, signo:%d\n",sig);
}

void init_setup(int argc, char *argv[])
{
	if (argc < 2)
		exit(-1);
	if (atoi(argv[1]) < 1023) {
		printf("Well known port selected. Not allowed! exiting ...\n");
		exit(-1);
	}
}

int socket_creation(void)
{
	int s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1) {
		perror("Server socket creation error");
		exit(-1);
	}
	return s;
}

void socket_server_config(int sockfd, int port_number)
{
	int ret = 0;
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port_number);    //understands data from port number 
	server_address.sin_addr.s_addr = inet_addr(LOCAL_HOST);

	/* bind to specified IP and port */
	ret = bind(sockfd, (struct sockaddr*) &server_address, sizeof(server_address));
	if (ret == -1) {
		perror("bind error");
		exit(-1);
	}
	
	ret = listen(sockfd, 10);
	if (ret == -1) {
		perror("listen error");
		exit(-1);
	}
}

void send_file(int sockfd, char *filename)
{
	FILE *fp;
	unsigned char data[SIZE] = {0};
	int nread = 0;
	
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		perror("");
		send(sockfd, &NOFILE, sizeof(NOFILE), 0);
		return;
	}
	
	send(sockfd, &YESFILE, sizeof(YESFILE), 0);
	recv(sockfd, &data, sizeof(data), 0);
	printf("%s \n", data);
	bzero(data, SIZE);
	
	while (1) {
		nread = fread(data, 1, SIZE, fp); 
		printf("Bytes read %d \n", nread);
		if (nread > 0) {
			signal(SIGPIPE, handle_broken_pipe);
			if (write(sockfd, &data, nread) == -1) {
				perror("Error");
				if (errno == ECONNRESET || errno == EPIPE) {
					printf("Server: continue execution...\n");
					break;
				}
			}
		}

		if (nread < SIZE) {
			if (feof(fp))
				printf("End of file\n");
			if (ferror(fp))
				printf("Error reading\n");
			break;
		}
	}
	fclose(fp);
	return;
}

void *socket_thread(void *p_client_socket)
{
	int ret = 0;
	char buff_req[SIZE];
	int client_socket = *((int*)p_client_socket);
	free(p_client_socket);
	p_client_socket = NULL;
	
	memset(buff_req, 0, sizeof(buff_req));
	ret = recv(client_socket, &buff_req, sizeof(buff_req), 0);
	if (ret == -1)
		perror("Receiving request error");
	printf("Request from client: %s\n", buff_req);
	send_file(client_socket, buff_req);
		
	bzero(buff_req, SIZE);    //resetting the buffer
	close(client_socket);
	return NULL;
}

void handle_threads(int *client_socket)
{
	pthread_t t;
	int *pclient = malloc(sizeof(int));
	*pclient = *client_socket;
	if (pthread_create(&t, NULL, socket_thread, pclient) != 0)
		perror("Pthread create");

	/* cleanup thread resources */
	if (pthread_detach(t) != 0)
		perror("Pthread detach");
}

void file_transfer(int sockfd)
{
	int client_socket = 0;
	
	while (1) {
		client_socket = accept(sockfd, NULL, NULL);
		if (client_socket == -1) {
			perror("accept error");
			exit(-1);
		}
		handle_threads(&client_socket);
	}
}

int main(int argc, char *argv[])
{	
	init_setup(argc, argv);
	int server_socket = 0;

	server_socket = socket_creation();
	socket_server_config(server_socket, atoi(argv[1]));
	
	file_transfer(server_socket);
	close(server_socket);
	return 0;
}
