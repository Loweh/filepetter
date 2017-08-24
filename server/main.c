#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

struct fileTransferData {
	int chunksize = 10;
	int protocolsize = 6;
	int filesize = 0;
	int bytesreceived = 0;
	char *rawfile;
};

int main() {
	WSADATA wsa;
	SOCKET sock, clientsock;
	struct sockaddr_in server, client;
	fileTransferData ftdata;
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

	int isAlive = 1;
	while (isAlive) {
		char *buffer = (char *)malloc(ftdata.chunksize + ftdata.protocolsize);
		returnvalue = recv(clientsock, buffer, ftdata.chunksize + ftdata.protocolsize, 0);
		if (returnvalue > 0) {
			if (buffer[0] == '1') {
				for (int i = 0; i < 4; i++) {
					int filesizebyte = buffer[i + 1];
					filesizebyte = filesizebyte << 8 * (3 - i);
					ftdata.filesize = ftdata.filesize | filesizebyte;
				};

				ftdata.rawfile = (char *)malloc(ftdata.filesize);
			};

			strncpy(ftdata.rawfile + (ftdata.bytesreceived), buffer + ftdata.protocolsize - 1, ftdata.chunksize);
			ftdata.bytesreceived = ftdata.bytesreceived + ftdata.chunksize;

			//pretty much the only place i can probably fit this (please kill me i hate my code)
			if (ftdata.bytesreceived >= ftdata.filesize) {
				FILE *file = fopen("test.txt", "wb");
				fwrite(ftdata.rawfile, ftdata.filesize, 1, file);
				fclose(file);
			};
		} else {
			isAlive = false;
			free(buffer);
		};
	};

	closesocket(sock);
	WSACleanup();
	return 0;
};