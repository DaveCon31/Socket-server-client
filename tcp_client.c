#include <stido.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_SIZE 512

int main() {

	//creating a socket
	int network_socket;
	network_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	//address for the socket
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(8900);    //understands data from port number 
	server_address.sin_addr.s_addr = INADDR_ANY;    //shortcut for local 0.0.0.0 
	
	int status = connect(network_socket, (struct sockaddr* ) &server_address, sizeof(server_address));
	if (status == -1) {
		perror("Connection error: "\n);
	}
	
	//receive data from server
	char server_response[BUFFER_SIZE];    //buffer
	recv(network_socket, &server_response, sizeof(server_response), 0);
	
	return 0;
}
