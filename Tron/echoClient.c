#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() {
	// Create socket
	int client = socket(AF_INET, SOCK_STREAM, 0);

	if(client < 0) {
		perror("socket");
		return 1;
	}

	// Server address
	sockaddr_in server();
	server.sin_family = AF_INET;
	server.sin_port = htons(9001);

	// Connect to localhost
	if(inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) <= 0) {
		perror("inet_pton);
		close(client);
		return 1;
	}

	// Connect
	if(connect(client, (sockaddr*)&server, sizeof(server)) < 0) {
		perror("connect");
		close(client);
		return 1;
	}

	std::cout << "Connected to server!\n";
	char buffer[1024];

	while(true) {
		std::cout << "Enter message: ";
		std::cin.getline(buffer, sizeof(buffer));

		// Exit if user enters "quit"
		if(strcmp(buffer, "quit") == 0) {
			break;
		}

		// Send message
		ssize_t bytes_sent = send(
			client,
			buffer,
			strlen(buffer),
			0
		);

		if(bytes_sent < 0) {
			perror("send");
			break;
		}

		// Receive echo
		ssize_t bytes_received = recv(
			client,
			buffer,
			sizeof(buffer) - 1,
			0
		);

		if(bytes_received <= 0) {
			std::cout << "Server disconnected.\n";
			break;
		}

		buffer[bytes_received] = '\0';

		std::cout << "Server echoed: " << buffer << "\n";
	}

	close(client);
	std::cout << "Disconnected.\n";
	return 0;
}
