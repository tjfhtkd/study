#include "stdafx.h"

#include <stdio.h>
#include <thread>
#include "MyNetClient.h"
#include "MyNetServer.h"
#include "MyNetConnector.h"


int main(int argv, char** argc)
{
	// Test //
	MyNetConnector myConnServer;
	MyNetServer myServer(&myConnServer);

	MyNetConnector myConnClient;
	MyNetClient myClient1(&myConnClient);
	MyNetClient myClient2(&myConnClient);


	printf("client1 - thread start\n");
	std::thread client1([&]() {
		printf("Connect client1\n");
		myClient1.ConnectServer();
		printf("%d\n", myConnClient.GetLastError());
	});

	myServer.StartServer();

	printf("client2 - thread start\n");
	std::thread client2([&]() {
		printf("Connect client2\n");
		myClient2.ConnectServer();
		printf("%d\n", myConnClient.GetLastError());
	});

	int loop = 2;
	while (loop)
	{
		printf("%lld\n", (ULONGLONG) myConnServer.Accept());
		loop--;
	}
	printf("Server end.\n");


	if (client1.joinable())
	{
		client1.join();
		printf("client1 end.\n");
	}

	if (client2.joinable())
	{
		client2.join();
		printf("client2 end.\n");
	}
	
	myConnClient.~MyNetConnector();
	myConnServer.~MyNetConnector();

	printf("Resource deleted. prgram end.");
	getchar();
	return 0;
}