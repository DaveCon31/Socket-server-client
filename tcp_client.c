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
#define REQUEST "developer.txt"
#define NOFILE "File not found"
#define ACK "Acknwoledge"

void init_setup(int argc, char **argv)
{
	if (argc < 2)
		exit(-1);
	if (PORT_NUMBER < 1023) {
		printf("Well known port selected. Not allowed! exiting ...\n");
		exit(-1);
	}
	return;
}

void recv_file(int sockfd)
{
	FILE *fp;
	int bytes_recv = 0;
	char recv_buffer[SIZE];
	memset(recv_buffer, 0, sizeof(recv_buffer));
	
	fp = fopen("received_file.txt", "ab");
	if (fp == NULL) {
		perror("File descriptor error");
		return;
	}
	
	while ((bytes_recv = read(sockfd, recv_buffer, SIZE)) > 0) {
		printf("Bytes received %d\n", bytes_recv);
		fwrite(recv_buffer, 1, bytes_recv, fp);
	}
	
	if (bytes_recv < 0)
		perror("Read error");
		
	fclose(fp);
	return;
}

int main(int argc, char *argv[]) 
{
	init_setup(argc, argv);
	int server_socket = 0, ret = 0;
	char buff[SIZE];
	struct sockaddr_in server_address;
	memset(buff, 0, sizeof(buff));
	
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket == -1) {
		perror("Server socket creation error");
		exit(-1);
	}
	
	/* address for the socket */
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT_NUMBER);    //understands data from port number 
	server_address.sin_addr.s_addr = inet_addr(LOCAL_HOST);
	
	ret = connect(server_socket, (struct sockaddr* ) &server_address, sizeof(server_address));
	if (ret == -1) {
		perror("Connect error");
		exit(-1);
	}
	
	ret = send(server_socket, REQUEST, sizeof(REQUEST), 0);    //request to server
	if (ret == -1) {
		perror("send request error");
		exit(-1);
	}
	
	ret = recv(server_socket, &buff, sizeof(buff), 0);
	if (ret == -1) {
		perror("recv files available error");
		exit(-1);
	}
	
	if (strcmp(buff, NOFILE) == 0)
		printf("%s\n", NOFILE);
	else {
		ret = send(server_socket, ACK, sizeof(ACK), 0);
		if (ret == -1) {
			perror("send ACK error");
			exit(-1);
		}
		recv_file(server_socket);
	}
	
	close(server_socket);
	return 0;
}
