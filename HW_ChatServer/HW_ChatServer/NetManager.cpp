#include "stdafx.h"
#include "NetConnector.h"
#include "NetPacketProcessor.h"
#include "NetManager.h"


NetManager::NetManager()
{
	m_conn = new NetConnector(dfNETWORK_PORT);
	m_netPacketProcessor = new NetPacketProcessor(this);
}

NetManager::~NetManager()
{
	auto begin	= sessions.begin();
	auto end		= sessions.end();
	kks::Session* delTarget;
	while (begin != end)
	{
		delTarget = (*begin).second;
		if (delTarget != nullptr)
		{
			delete delTarget;
			delTarget = nullptr;
		}
		++begin;
	}
	sessions.clear();
	
	delete m_conn;
	m_conn = nullptr;

	delete m_netPacketProcessor;
	m_netPacketProcessor = nullptr;
}

bool NetManager::Connect(void)
{
	if (m_conn->Connect() == -1)
	{
		wprintf_s(L"Error : %d\n", WSAGetLastError());
		return false;
	}
	m_conn->SetNonBlock(true);

	// Listen Socket�� �ϳ��� Session �̶�� ���� ����.
	// ������ �� Sock�� ä���� �ϰų� �濡 �����ϴ� ���� ������ ���� ���� ��.
	// �ϰ����� ����� ���ʿ��� �Լ��� �бⰡ �پ�� �� ���Ƽ� �غ�. FD_ISSET �ϴ� �� �ƴϸ�
	// �� ���� �������� ���� ������ ����.

	// ���� ���ϱ��� ���������� �ʿ� ����� �Ǿ�� ������ ���� �����ϹǷ� AddSession�� ���� ���� ��ȯ
	return AddSession(CreateGUID(), m_conn->GetListenSocket(), L"LISTEN_SOCK");
}

void NetManager::StartLoop(void)
{
	IsServerRun = true;

	timeval timeOut;
	timeOut.tv_sec	= 0;
	timeOut.tv_usec	= 0;

	std::list<FD_SET> rSetList;
	std::list<FD_SET> wSetList;
	std::list<kks::Session*> rSessions;
	std::list<kks::Session*> wSessions;

	while (IsServerRun)
	{
		// 1. FD_SET rSet, wSet �ʱ�ȭ �� FD_SET list ��� (64�� �̻� ó���ؾ� �ϹǷ�)
		GetFdSetList(rSetList, wSetList);

		// 2. Select �� �����ؼ� ������ �־��� Session �� ã��
		if (GetSelectedSessions(rSessions, wSessions, rSetList, wSetList, timeOut) == true)
		{
			// 2.1 Session ��û���� ó��
			m_netPacketProcessor->ProcessSessionRequest(rSessions, wSessions);
		}

		rSetList.clear();
		wSetList.clear();
		rSessions.clear();
		wSessions.clear();
	}
}

bool NetManager::AddSession(SOCKET sock, const WCHAR nickName[dfNICK_MAX_LEN])
{
	if (IsAlreadyExistStrKeyIn(nicknameChecker, nickName) == true)
	{
		return false;
	}
	DWORD uid = CreateGUID();
	kks::Session* session = (kks::Session*)malloc(sizeof(kks::Session));
	ZeroMemory(session, sizeof(kks::Session));
	session->uid = uid;
	session->sock = sock;
	wmemcpy_s(session->nickName, dfNICK_MAX_LEN, nickName, dfNICK_MAX_LEN);

	sessions[uid]						= session;
	socketChecker[sock]			= session;
	nicknameChecker[nickName]	= uid;
	return true;
}

void NetManager::DeployFdSet(FD_SET& rSet, FD_SET& wSet, std::list<kks::Session*>& sessions)
{
	FD_ZERO(&rSet);
	FD_ZERO(&wSet);

	SOCKET sock;
	// 1. rSet�� �׳� �ʱ�ȭ �ϰ� �� �� �־��ش�.
	auto begin = sessions.begin();
	auto end = sessions.end();
	for (int i = 0; begin != end; i ++)
	{
		sock = (*begin)->sock;
		FD_SET(sock, &rSet);
		++begin;
	}

	// 2. wSet�� ������ Session�� �߿��� ���� ������ �ִ� �༮�鸸 �ֵ��� �Ѵ�.
	begin = sessions.begin();
	for (int i = 0; begin != end; i++)
	{
		if ((*begin)->sendQ.IsEmpty() == false)
		{
			sock = (*begin)->sock;
			FD_SET(sock, &wSet);
		}
		++begin;
	}
}

void NetManager::GetFdSetList(std::list<FD_SET>& out_rSetList, std::list<FD_SET>& out_wSetList)
{
	std::list<kks::Session*> tmpSessionList;
	auto begin = sessions.begin();
	auto end = sessions.end();
	size_t sessionCnt = sessions.size();
	size_t tmpSessionListSize;
	for (int i = 0; begin != end; ++i)
	{
		tmpSessionList.push_back((*begin).second);
		tmpSessionListSize = tmpSessionList.size();
		if (tmpSessionListSize >= FD_SETSIZE ||
			tmpSessionListSize == sessionCnt)
		{
			FD_SET rSet;
			FD_SET wSet;
			DeployFdSet(rSet, wSet, tmpSessionList);
			if (rSet.fd_count > 0)
			{
				out_rSetList.push_back(rSet);
			}
			if (wSet.fd_count > 0)
			{
				out_wSetList.push_back(wSet);
			}
			tmpSessionList.clear();
		}
		++begin;
	}
}

bool NetManager::GetSelectedSessions(
	std::list<kks::Session*>& out_rSessions, std::list<kks::Session*>& out_wSessions
	, std::list<FD_SET>& in_rSetList, std::list<FD_SET>& in_wSetList
	, timeval& timeOut)
{
	return MakeSelectedSessions(in_rSetList, out_rSessions, true, timeOut)
		|| MakeSelectedSessions(in_wSetList, out_wSessions, false, timeOut);
}

bool NetManager::MakeSelectedSessions(
	std::list<FD_SET>& checkList
	, std::list<kks::Session*>& out_list
	, bool bReadSet
	, timeval& timeOut)
{
	INT sockStateCheck;
	bool bHasSomeAction = false;
	for (FD_SET set : checkList)
	{
		sockStateCheck = (bReadSet == true) ? select(0, &set, nullptr, nullptr, &timeOut) : select(0, nullptr, &set, nullptr, &timeOut);
		if (sockStateCheck == SOCKET_ERROR)
		{
			// hmm..
			continue;
		}
		else if (sockStateCheck == 0)
		{
			// timeout. just skip.
			continue;
		}
		else
		{
			// somthing is selected.
			for (u_int i = 0; i < set.fd_count; i++)
			{
				out_list.push_back(socketChecker[set.fd_array[i]]);
			}
			bHasSomeAction = true;
		}
	}
	return bHasSomeAction;
}

DWORD NetManager::CreateGUID(void)
{
	return ++next_UID;
}

DWORD NetManager::CreateGRID(void)
{
	return ++next_RoomID;
}