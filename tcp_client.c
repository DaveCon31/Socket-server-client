#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SIZE 1024
#define LOCAL_HOST "127.0.0.1"
#define NOFILE "File not found"
#define ACK "Acknwoledge"

void init_setup(int argc, char *argv[])
{
	if (argc < 3)
		exit(-1);
	if (atoi(argv[1]) < 1023) {
		printf("Well known port selected. Not allowed! exiting ...\n");
		exit(-1);
	}
	return;
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

void socket_client_config(int sockfd, int port_number)
{
	int ret = 0;
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port_number);    //understands data from port number 
	server_address.sin_addr.s_addr = inet_addr(LOCAL_HOST);
	
	ret = connect(sockfd, (struct sockaddr* ) &server_address, sizeof(server_address));
	if (ret == -1) {
		perror("Connect error");
		exit(-1);
	}
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

void file_transfer(int sockfd, char *req)
{
	int ret = 0;
	char buff[SIZE];
	memset(buff, 0, sizeof(buff));
	
	ret = send(sockfd, req, strlen(req), 0);    //send REQ to server
	if (ret == -1) {
		perror("send request error");
		exit(-1);
	}
	
	ret = recv(sockfd, &buff, sizeof(buff), 0);    //recieve RDY if file is available or not
	if (ret == -1) {
		perror("recv files available error");
		exit(-1);
	}
	
	if (strcmp(buff, NOFILE) == 0)
		printf("%s\n", buff);
	else {
		ret = send(sockfd, ACK, sizeof(ACK), 0);    //send ACK, ready to receive file
		if (ret == -1) {
			perror("send ACK error");
			exit(-1);
		}
		recv_file(sockfd);
	}
}

int main(int argc, char *argv[]) 
{
	init_setup(argc, argv);
	int server_socket = 0;
	
	server_socket = socket_creation();
	socket_client_config(server_socket, atoi(argv[1]));
	
	file_transfer(server_socket, argv[2]);
	close(server_socket);
	return 0;
}
