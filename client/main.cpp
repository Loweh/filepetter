#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

int main() {
	WSADATA wsa;
	SOCKET sock;
	struct sockaddr_in server;
	int returnvalue;

	returnvalue = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (returnvalue != 0) {
		return 1;
	};

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		return 2;
	};

	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(2127);

	returnvalue = connect(sock, (struct sockaddr *)&server, sizeof(server));
	if (returnvalue == SOCKET_ERROR) {
		return 3;
	};

	char buffer[2];
	buffer[0] = 'a';
	returnvalue = send(sock, buffer, sizeof(buffer), 0);
	if (returnvalue == SOCKET_ERROR) {
		return 4;
	};
	while (true) {
		//just so i can make sure packets go between the two
	};

	closesocket(sock);
	WSACleanup();
	return 0;
};