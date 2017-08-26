#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>
#pragma comment(lib, "ws2_32.lib")

struct socketData {
	WSADATA wsa;
	SOCKET sock;
	struct sockaddr_in server;
	char *serverip = "127.0.0.1";
	int port = 2127;
};

struct fileTransferData {
	FILE *file;
	int packetid = 1;
	int totalsize;
	int chunks;
	int chunksize = 507; //size of each packet
	int protocolsize = 5; //1 character for packetid, 4 for length  of rawfile
	char *filename;
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

	sockdata.server.sin_addr.s_addr = inet_addr(sockdata.serverip);
	sockdata.server.sin_family = AF_INET;
	sockdata.server.sin_port = htons(sockdata.port);

	returnvalue = connect(sockdata.sock, (struct sockaddr *)&sockdata.server, sizeof(sockdata.server));
	if (returnvalue == SOCKET_ERROR) {
		return 3;
	};

	ftdata.filename = "test.txt";
	ftdata.file = fopen(ftdata.filename, "rb");
	if (ftdata.file == NULL) {
		return 4;
	};

	fseek(ftdata.file, 0, SEEK_END);
	ftdata.totalsize = ftell(ftdata.file);
	rewind(ftdata.file);

	ftdata.rawfile = (char *)malloc(ftdata.totalsize);
	fread(ftdata.rawfile, ftdata.totalsize, 1, ftdata.file);
	fclose(ftdata.file);

	returnvalue = ftdata.totalsize % ftdata.chunksize;
	if (returnvalue == 0) {
		ftdata.chunks = (ftdata.totalsize / ftdata.chunksize) + 1; //this is to account for the first packet, which sends the filename
	}
	else {
		ftdata.chunks = (ftdata.totalsize / ftdata.chunksize) + 2; //would be + 1 to round up, but again, must account for the first packet
	};

	for (int i = 0; i < ftdata.chunks; i++) {
		int marker = (i - 1) * ftdata.chunksize;
		char * chunk = (char *)malloc(ftdata.chunksize + ftdata.protocolsize);

		if (i == 0) {
			chunk[0] = ftdata.packetid + '0'; //for some reason this works

			//convert ftdata.totalsize to a series of 4 chars
			for (int i = 0; i < ftdata.protocolsize - 1; i++) {
				unsigned int totalsizebyte = ftdata.totalsize;
				totalsizebyte = totalsizebyte << 8 * (i);
				totalsizebyte = totalsizebyte >> 24;
				chunk[i + 1] = (char)totalsizebyte;
			};

			strncpy(chunk + ftdata.protocolsize, ftdata.filename, ftdata.chunksize);
		} else { //keep packets the same size without resending packet data
			for (int y = 0; y < 5; y++) {
				chunk[y] = '.';
			};

			strncpy(chunk + ftdata.protocolsize, ftdata.rawfile + marker, ftdata.chunksize);
		};

		send(sockdata.sock, chunk, ftdata.chunksize + ftdata.protocolsize, 0);
	};

	free(ftdata.rawfile);

	closesocket(sockdata.sock);
	WSACleanup();
	return 0;
};