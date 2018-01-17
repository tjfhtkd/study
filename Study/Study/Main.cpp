//#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <stdio.h>


int main(int argc, char* argv[]) {
	// 현재 시스템의 환경 정보 
	// 몇 버전까지 쓰나? 소켓은 최대 몇개까지 가능? 데이터그램 최대 크기는?
	WSADATA wsaData;

	// ws2_32.lib load
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartUp fail!!\n\n");
	}

	// 통신을 위한 규격, 주소 정보
	// IPv4냐? TCP냐? 포트는 몇번이냐?
	SOCKADDR_IN addrIn;
	int port = 8912;
	addrIn.sin_family = AF_INET;
	addrIn.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrIn.sin_port = htons(port);

	// 클라이언트와 연결을 위한 준비, 클라이언트와 어떤 체계로 주고받을지 정함
	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// 소켓에 주소 할당
	if (bind(s, (SOCKADDR*)&addrIn, sizeof(addrIn)) == SOCKET_ERROR) {
		printf("Bind Error!!!\n\n");
	}
	else {
		// bind에서 사람이 오나 안오나 대기 시작, 일반적으로 5
		listen(s, 5);
	}

	// resources return
	if (WSACleanup() == SOCKET_ERROR) {
		printf("CleanUp fail!!!\n\n");
	}
	else {
		printf("CleanUp Socket...");
	}

	getchar();

	return 0;
}