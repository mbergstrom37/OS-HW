#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
	int server = socket(AF_INET, SOCK_STREAM, 0);
	if(server < 0) {
		std::cout << "Socket Setup Error\n";
		return 1;
	}

	// Setup socket server address
	sockaddr_in_address{};
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(9001);

	// Bind to the port
	if(bind(server,(sockaddr*)&address,sizeof(address)) < 0) {
		std::cout << "Could not bind to address\n";
		close(server);
		return 1;
	}

	if(listen(server,5) < 0) {
		perror("listen");
		close(server);
		return 1;
	}

	std::cout << "Echo server now listening\n";

	while(true) {
		sockaddr_in_client{};
		socklen_t client_size = sizeof(client);

		// This is the line that blocks and waits for a client to connect.
		int client_s = accept(server,(sockaddr*)&client,&client_size);
		if(client_s < 0) {
			perror("Accept");
			continue;
		}
		char buffer[1024];
		while(true) {
			ssize_t bytes = recv(client_s,buffer,sizeof(buffer),0);
			if(bytes <= 0) {
				break;
			}
			buffer[bytes] = '\0';
			std::cout << "Recv: "<< buffer << std::endl;
			send(client_s,buffer,bytes,0);
		}
		std::cout << "Client Disconnected\n";
	}
	close(server);
	return 0;
}
