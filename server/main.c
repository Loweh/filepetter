#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

struct socketData {
	int port = 2127;
	WSADATA wsa;
	SOCKET sock, clientsock;
	struct sockaddr_in server, client;
};

struct fileTransferData {
	int chunksize = 507;
	int protocolsize = 5;
	int filesize = 0;
	int bytesreceived = 0;
	char* filename;
	char *rawfile;
};

int main() {
	socketData sockdata;
	fileTransferData ftdata;
	int returnvalue;

	returnvalue = WSAStartup(MAKEWORD(2, 2), &sockdata.wsa);
	if (returnvalue != 0) {
		return 1;
	};

	sockdata.sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockdata.sock == INVALID_SOCKET) {
		return 2;
	};

	sockdata.server.sin_addr.s_addr = INADDR_ANY;
	sockdata.server.sin_family = AF_INET;
	sockdata.server.sin_port = htons(sockdata.port);

	returnvalue = bind(sockdata.sock, (struct sockaddr *)&sockdata.server, sizeof(sockdata.server));
	if (returnvalue == SOCKET_ERROR) {
		return 3;
	};

	listen(sockdata.sock, 3);

	int clientsize = sizeof(sockdata.server);
	sockdata.clientsock = accept(sockdata.sock, (struct sockaddr *)&sockdata.client, &clientsize);

	int isAlive = 1;
	while (isAlive) {
		char *buffer = (char *)malloc(ftdata.chunksize + ftdata.protocolsize);
		returnvalue = recv(sockdata.clientsock, buffer, ftdata.chunksize + ftdata.protocolsize, 0);
		if (returnvalue > 0) {
			if (buffer[0] == '1') {
				for (int i = 0; i < 4; i++) {
					int filesizebyte = (unsigned char)buffer[i + 1];
					filesizebyte = filesizebyte << 8 * (3 - i);
					ftdata.filesize = ftdata.filesize | filesizebyte;
				};

				ftdata.filename = buffer + ftdata.protocolsize;
				ftdata.rawfile = (char *)malloc(ftdata.filesize);
			}
			else {
				strncpy(ftdata.rawfile + ftdata.bytesreceived, buffer + ftdata.protocolsize, ftdata.chunksize);
				ftdata.bytesreceived = ftdata.bytesreceived + ftdata.chunksize;
			};

			if (ftdata.bytesreceived >= ftdata.filesize) {
				FILE *file = fopen(ftdata.filename, "wb");
				fwrite(ftdata.rawfile, ftdata.filesize, 1, file);
				fclose(file);
			};
		} else {
			isAlive = false;
			free(buffer);
		};
	};

	closesocket(sockdata.sock);
	WSACleanup();
	return 0;
};