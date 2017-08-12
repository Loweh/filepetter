#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
	WSADATA wsa;
	SOCKET sock;
	struct sockaddr_in server, client;
	int returnvalue;

	returnvalue = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (returnvalue != 0) {
		return 1;
	};

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		return 2;
	};

	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons(2127);

	returnvalue = bind(sock, (struct sockaddr *)&server, sizeof(server));
	if (returnvalue == SOCKET_ERROR) {
		printf("%i", WSAGetLastError());
		while (1 == 1) {
			//see things
		};
		return 3;
	};

	listen(sock, 3);

	int clientsize = sizeof(struct sockaddr_in);
	while (accept(sock, (struct sockaddr *)&client, &clientsize) != INVALID_SOCKET) {
		/*char buffer[2];
		recv(sock, buffer, sizeof(buffer), 0);
		buffer[2] = '\0';
		printf("Message received: %s\n", buffer);*/
	};

	closesocket(sock);
	WSACleanup();
	return 0;
};
