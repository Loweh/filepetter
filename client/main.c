#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>
#pragma comment(lib, "ws2_32.lib")

struct fileTransferData {
	int totalsize;
	int chunks;
	int chunksize = 10; //size of each packet
	char *rawfile;
	char **segments;
};

int main() {
	WSADATA wsa;
	SOCKET sock;
	struct sockaddr_in server;
	FILE *testfile;
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

	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_family = AF_INET;
	server.sin_port = htons(2127);

	returnvalue = connect(sock, (struct sockaddr *)&server, sizeof(server));
	if (returnvalue == SOCKET_ERROR) {
		return 3;
	};

	testfile = fopen("test.txt", "r+");
	if (testfile == NULL) {
		return 4;
	};

	fseek(testfile, 0, SEEK_END);
	ftdata.totalsize = ftell(testfile);
	rewind(testfile);

	ftdata.rawfile = (char *)malloc(ftdata.totalsize + 1);
	fread(ftdata.rawfile, ftdata.totalsize, 1, testfile);
	ftdata.rawfile[ftdata.totalsize] = '\0';

	returnvalue = ftdata.totalsize % ftdata.chunksize;
	if (returnvalue == 0) {
		ftdata.chunks = ftdata.totalsize / ftdata.chunksize;
	}
	else {
		ftdata.chunks = (ftdata.totalsize / ftdata.chunksize) + 1;
	};

	ftdata.segments = (char **)malloc(sizeof(char *) * ftdata.chunks);
	for (int i = 0; i < ftdata.chunks; i++) {
		int marker = i * ftdata.chunksize;
		char* chunk = (char *)malloc(ftdata.chunksize + 1);

		strncpy(chunk, ftdata.rawfile + marker, ftdata.chunksize);
		ftdata.segments[i] = chunk;
		chunk[10] = '\0';
	};

	for (int i = 0; i < ftdata.chunks; i++) {
		printf("%s\n", ftdata.segments[i]);
	};

	/*char buffer[2];
	buffer[0] = 'a';
	returnvalue = send(sock, buffer, sizeof(buffer), 0);
	if (returnvalue == SOCKET_ERROR) {
		return 4;
	};*/
	while (true) {
		//just so i can make sure packets go between the two
	};

	closesocket(sock);
	WSACleanup();
	return 0;
};
