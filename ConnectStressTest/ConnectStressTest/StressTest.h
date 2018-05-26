#pragma once

class StressTest
{
public:
	StressTest(int clientCount);
	virtual ~StressTest();

public:
	bool SetServerAddr(const wchar_t* serverAddr, int port);
	void Connect(void);
	void Echo(void);
	void Disconnect(float percentage);

	bool CheckOneSec(void);
	void PrintPacketCnt(void);
	void ResetPacketCnt(void);

private:
	int m_clientCount;
	SOCKADDR_IN m_addr;

	std::list<Client*> m_connectedClients;
	std::list<Client*> m_disconnectedClients;
	std::unordered_map<int, Client*> m_connClientSearchMap;

	int m_sendPacketCnt;
	int m_recvPacketCnt;
	int m_echoMsgDamaged;
	DWORD m_timeChecker;
	bool m_bCheck;
};

