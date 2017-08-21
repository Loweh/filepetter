#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>
#pragma comment(lib, "ws2_32.lib")

struct fileTransferData {
	int fileid;
	int totalsize;
	int chunks;
	int chunksize = 10; //size of each packet
	int protocolsize = 6; //1 character for fileid, 4 for length  of rawfile, 1 for null terminator
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

	ftdata.fileid = 1;
	ftdata.segments = (char **)malloc(sizeof(char *) * ftdata.chunks);
	for (int i = 0; i < ftdata.chunks; i++) {
		int marker = i * ftdata.chunksize;
		if (i == 0) {
			char * chunk = (char *)malloc(ftdata.chunksize + ftdata.protocolsize);
			chunk[0] = ftdata.fileid + '0'; //for some reason this works

			//convert ftdata.totalsize to a series of 4 chars
			for (int i = 0; i < ftdata.protocolsize - 1; i++) {
				unsigned int totalsizebyte = ftdata.totalsize;
				totalsizebyte = totalsizebyte << 8 * (i);
				totalsizebyte = totalsizebyte >> 24;
				chunk[i + 1] = (char)totalsizebyte;
			};

			chunk[(ftdata.chunksize + ftdata.protocolsize) - 1] = '\0'; //null terminate

			strncpy((chunk + ftdata.protocolsize) - 1, ftdata.rawfile + marker, ftdata.chunksize);
			ftdata.segments[i] = chunk;
		}
		else {
			char * chunk = (char *)malloc(ftdata.chunksize + 1);
			chunk[0] = ftdata.fileid + '0'; //for some reason this works
			chunk[ftdata.chunksize] = '\0'; //null terminate

			strncpy(chunk, ftdata.rawfile + marker, ftdata.chunksize);
			ftdata.segments[i] = chunk;
		};
	};

	for (int i = 0; i < ftdata.chunks; i++) {
		if (i == 0) {
			send(sock, ftdata.segments[i], ftdata.chunksize + ftdata.protocolsize, 0);
		}
		else {
			send(sock, ftdata.segments[i], ftdata.chunksize + 1, 0);
		};
	};

	while (1) {
		//not even sure what this is for anymore
	};

	closesocket(sock);
	WSACleanup();
	return 0;
};