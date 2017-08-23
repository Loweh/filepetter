#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
	WSADATA wsa;
	SOCKET sock, clientsock;
	struct sockaddr_in server, client;
	int packetsize = 10;
	int protocolsize = 6;
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
		return 3;
	};

	listen(sock, 3);

	int clientsize = sizeof(server);
	clientsock = accept(sock, (struct sockaddr *)&client, &clientsize);

	char *file; //temporary
	int chunks = 0; //also temporary
	int isAlive = 1;
	while (isAlive) {
		char *buffer = (char *)malloc(packetsize + protocolsize);
		returnvalue = recv(clientsock, buffer, packetsize + protocolsize, 0);
		if (returnvalue > 0) {
			if (buffer[0] == '1') {
				int filesize = 0;
				for (int i = 0; i < 4; i++) {
					int filesizebyte = buffer[i + 1];
					filesizebyte = filesizebyte << 8 * (3 - i);
					filesize = filesize | filesizebyte;
				};

				file = (char *)malloc(filesize);
			};

			strncpy(file + (10 * chunks), buffer + protocolsize - 1, packetsize);
			chunks++;
		} else {
			isAlive = false;
			free(buffer);
		};
	};

	printf("%s", file);
	while (1) {
		//more debugging stuff
	};
	closesocket(sock);
	WSACleanup();
	return 0;
};