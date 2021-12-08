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
		exit(1);
	if (PORT_NUMBER < 1023) {
		printf("Well known port selected. Not allowed! exiting ...\n");
		exit(1);
	}
	return;
}

void check(int e)
{
	if (e == -1) {
		perror("Error");
		exit(1);
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
	//fclose(fp);
	return;
}

int main(int argc, char *argv[]) 
{
	init_setup(argc, argv);
	int client_socket = 0;
	int checker = 0;
	char buff[SIZE];
	struct sockaddr_in server_address;
	memset(buff, 0, sizeof(buff));
	
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	check(client_socket);
	
	/* address for the socket */
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT_NUMBER);    //understands data from port number 
	server_address.sin_addr.s_addr = inet_addr(LOCAL_HOST);
	
	checker = connect(client_socket, (struct sockaddr* ) &server_address, sizeof(server_address));
	check(checker);
	
	/* send request to server */
	checker = send(client_socket, REQUEST, sizeof(REQUEST), 0);
	//check(checker);
	checker = recv(client_socket, &buff, sizeof(buff), 0);
	if (strcmp(buff, NOFILE) == 0) 
		printf("%s\n", NOFILE);
	else {
		send(client_socket, ACK, sizeof(ACK), 0);
		recv_file(client_socket);
	}
	
	close(client_socket);
	return 0;
}
