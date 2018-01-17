//#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <stdio.h>


int main(int argc, char* argv[]) {
	// ���� �ý����� ȯ�� ���� 
	// �� �������� ����? ������ �ִ� ����� ����? �����ͱ׷� �ִ� ũ���?
	WSADATA wsaData;

	// ws2_32.lib load
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartUp fail!!\n\n");
	}

	// ����� ���� �԰�, �ּ� ����
	// IPv4��? TCP��? ��Ʈ�� ����̳�?
	SOCKADDR_IN addrIn;
	int port = 8912;
	addrIn.sin_family = AF_INET;
	addrIn.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrIn.sin_port = htons(port);

	// Ŭ���̾�Ʈ�� ������ ���� �غ�, Ŭ���̾�Ʈ�� � ü��� �ְ������ ����
	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// ���Ͽ� �ּ� �Ҵ�
	if (bind(s, (SOCKADDR*)&addrIn, sizeof(addrIn)) == SOCKET_ERROR) {
		printf("Bind Error!!!\n\n");
	}
	else {
		// bind���� ����� ���� �ȿ��� ��� ����, �Ϲ������� 5
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