#include "stdafx.h"
#include <time.h>
#include "StressTest.h"


StressTest::StressTest(int clientCount) 
	: m_clientCount(clientCount)
{
	//timeBeginPeriod(1);
	srand(time(NULL));
	if (0 >= clientCount)
	{
		m_clientCount = clientCount = 1;
	}

	for (int i = 0; i < clientCount; ++i)
	{
		Client* client = new Client;
		client->id = i;
		client->bRecvWait = false;
		client->sendTime = 0;
		wsprintfW(client->sendEchoMsg, L"%d ECHO\0", rand() % 1500);
		m_disconnectedClients.push_back(client);
	}
	ResetPacketCnt();
}

StressTest::~StressTest()
{
	for (Client* client : m_connectedClients)
	{
		closesocket(client->sock);
		delete client;
	}

	for (Client* client : m_disconnectedClients)
	{
		delete client;
	}
	WSACleanup();
}

bool StressTest::SetServerAddr(const wchar_t* serverAddr, int port)
{
	WSAData wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return false;
	}

	ZeroMemory(&m_addr, sizeof(m_addr));
	InetPton(AF_INET, serverAddr, &m_addr.sin_addr.S_un.S_addr);
	m_addr.sin_family	= AF_INET;
	m_addr.sin_port		= htons(port);

	return true;
}

void StressTest::Connect(void)
{
	int invalidSocketCnt = 0;
	SOCKADDR_IN addr = m_addr;
	for (Client* client : m_disconnectedClients)
	{
		client->sock = socket(AF_INET, SOCK_STREAM, 0);
		if (client->sock == INVALID_SOCKET)
		{
			invalidSocketCnt++;
		}

		client->sendTime = timeGetTime();
		if (connect(client->sock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
		{
			wprintf_s(L"<%5d> SOCK_ERROR [%d]\n", client->id, WSAGetLastError());
		}
		else
		{
			unsigned long on = true;
			ioctlsocket(client->sock, FIONBIO, &on);
			LINGER linger;
			linger.l_linger = 0;
			linger.l_onoff = 1;
			setsockopt(client->sock, SOL_SOCKET, SO_LINGER, (char*)&linger, sizeof(linger));

			//wprintf_s(L"<%5d> CONNECT RESPONSE TIME : %d\n", client->id, timeGetTime() - client->sendTime);
			m_connectedClients.push_back(client);
			m_disconnectedClients.remove(client);
			m_connClientSearchMap[client->id] = client;
		}
	}
	//wprintf_s(L"\n[INVALID SOCKET COUNT : %d]\n", invalidSocketCnt);
}

//int g_cnt;
void StressTest::Echo(void)
{
	//g_cnt = 0;
	timeval timeOut;
	timeOut.tv_sec = 0;
	timeOut.tv_usec = 0;

	auto begin	= m_connectedClients.begin();
	auto end		= m_connectedClients.end();
	while (begin != end)
	{
		auto startCursor = begin;
		// 1.  Init. fd_set
		fd_set rSet;
		fd_set wSet;
		FD_ZERO(&rSet);
		FD_ZERO(&wSet);
		int fdCnt = 0;
		while(begin != end && fdCnt < FD_SETSIZE)
		{
			FD_SET((*begin)->sock, &rSet);
			FD_SET((*begin)->sock, &wSet);
			++begin;
			++fdCnt;
		}
		
		int selectedCnt = select(0, &rSet, &wSet, nullptr, &timeOut);
		if (selectedCnt == SOCKET_ERROR || selectedCnt == 0)
		{
			continue;
		}

		while (selectedCnt > 0)
		{
			if (FD_ISSET((*startCursor)->sock, &wSet) == true)
			{
				--selectedCnt;
				if ((*startCursor)->bRecvWait == false)
				{
					st_PACKET_HEADER header;
					header.byCode = 0x89;
					header.wMsgType = df_REQ_ECHO;
					header.wPayloadSize = sizeof((*startCursor)->sendEchoMsg);

					// 항상 localBuf를 통해 주고받으므로 여기서도 일단 그렇게 하고 속도를 확인
					LocalBuf localBuf;
					localBuf.SetHeader((void*)&header, sizeof(header));
					localBuf.Enqueue((*startCursor)->sendEchoMsg, 128 * sizeof(WCHAR));
					localBuf >> (*startCursor)->sendQ;

					localBuf << (*startCursor)->sendQ;
					int sendDataSize = send((*startCursor)->sock, (char*)localBuf.GetReadBufferPtr(), localBuf.GetUseSize(), 0);
					if (sendDataSize == SOCKET_ERROR)
					{
						if (WSAGetLastError() != WSAEWOULDBLOCK)
						{
							// 원래는 지워야 하는데... 힘들다..
						}
					}
					else
					{
						(*startCursor)->bRecvWait = true;
						(*startCursor)->sendQ.RemoveData(sendDataSize);
						//wprintf_s(L"<%5d>SEND %d, %s\n", ++g_cnt, (*startCursor)->sock, (*startCursor)->sendEchoMsg);
						++m_sendPacketCnt;
					}
				}
			}

			if (FD_ISSET((*startCursor)->sock, &rSet) == true)
			{
				--selectedCnt;
				LocalBuf localBuf;
				int recvDataSize = recv((*startCursor)->sock, (char*)localBuf.GetWriteBufferPtr(), localBuf.GetFreeSize(), 0);
				localBuf.MoveWritePos(recvDataSize);
				if (recvDataSize == SOCKET_ERROR)
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK)
					{
						// 마찬가지로 오류 처리 필요..
					}
				}
				else
				{
					st_PACKET_HEADER header;
					localBuf << &header;
					WCHAR* payload = new WCHAR[128];
					localBuf.Dequeue((void*)payload, header.wPayloadSize);
					if (wmemcmp(payload, (*startCursor)->sendEchoMsg, 128) != 0)
					{
						++m_echoMsgDamaged;
					}
					++m_recvPacketCnt;
					//wprintf_s(L"<%5d>RECV %d, %s\n", ++g_cnt, (*startCursor)->sock, payload);
					(*startCursor)->bRecvWait = false;
					delete[] payload;
				}
			}
			++startCursor;
		}
	}
}

void StressTest::Disconnect(float percentage)
{
	int disconnCnt = m_connectedClients.size() * percentage;
	for (int i = 0; i < disconnCnt; ++i)
	{
		int key = rand() % m_connClientSearchMap.size();
		auto client = m_connClientSearchMap.find(key);
		if (client != m_connClientSearchMap.end())
		{
			closesocket(client->second->sock);
			m_connectedClients.remove(client->second);
			m_disconnectedClients.push_back(client->second);
			m_connClientSearchMap.erase(client);
		}
	}
}

bool StressTest::CheckOneSec(void)
{
	if (m_bCheck == false)
	{
		m_bCheck = true;
		m_timeChecker = timeGetTime();
	}
	else
	{
		DWORD curTime = timeGetTime();
		if (curTime - m_timeChecker >= CLOCKS_PER_SEC)
		{
			m_timeChecker = curTime - (curTime - m_timeChecker - CLOCKS_PER_SEC);
			return true;
		}
	}
	return false;
}

void StressTest::PrintPacketCnt(void)
{
	wprintf_s(L"RECV : %d   //   SEND : %d   //   MSG MISS : %d\n", m_recvPacketCnt, m_sendPacketCnt, m_echoMsgDamaged);
}

void StressTest::ResetPacketCnt(void)
{
	//g_cnt = 0;
	m_echoMsgDamaged = m_sendPacketCnt = m_recvPacketCnt = 0;
}
