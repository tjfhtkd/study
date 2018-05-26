#include "stdafx.h"
#include "NetConnector.h"
#include "NetPacketProcessor.h"
#include "NetManager.h"


NetManager::NetManager()
{
	m_conn = new NetConnector(dfNETWORK_PORT);
	m_netPacketProcessor = new NetPacketProcessor(this);
	
	timeOut.tv_sec = 0;
	timeOut.tv_usec = 0;
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

	// Listen Socket도 하나의 Session 이라고 보고 수행.
	// 어차피 이 Sock이 채팅을 하거나 방에 참여하는 등의 행위를 하진 않을 것.
	// 일관성이 생기면 불필요한 함수나 분기가 줄어들 것 같아서 해봄. FD_ISSET 하는 때 아니면
	// 다 같은 로직으로 되지 않을까 싶음.

	// 리슨 소켓까지 성공적으로 맵에 등록이 되어야 유저가 유입 가능하므로 AddSession의 성공 여부 반환
	return AddSession(m_conn->GetListenSocket(), L"LISTEN_SOCK");
}

void NetManager::ProcessNetwork(void)
{
	/*std::list<FD_SET> rSetList;
	std::list<FD_SET> wSetList;
	std::list<kks::Session*> rSessions;
	std::list<kks::Session*> wSessions;*/

	if (IsServerRun)
	{
		// 1. FD_SET rSet, wSet 초기화 및 FD_SET list 얻기 (64명 이상 처리해야 하므로)
		GetFdSetList(rSetList, wSetList);

		// 2. Select 를 수행해서 반응이 있었던 Session 만 찾기
		if (GetSelectedSessions(rSessions, wSessions, rSetList, wSetList, timeOut) == true)
		{
			// 2.1 Session 요청사항 처리
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
	if (nickName == nullptr)
	{
		CreateSession(sock);
	}
	else
	{
		if (IsAlreadyExistStrKeyIn(nicknameChecker, nickName) == true)
		{
			return false;
		}
		else
		{
			kks::Session* session = CreateSession(sock);
			wmemcpy_s(session->nickName, dfNICK_MAX_LEN, nickName, dfNICK_MAX_LEN);
			nicknameChecker[nickName] = socketChecker[sock]->uid;
		}
	}
	return true;
}

void NetManager::RemoveSession(kks::Session* session)
{
	sessions.erase(session->uid);
	socketChecker.erase(session->sock);
	nicknameChecker.erase(session->nickName);
	rooms.erase(session->roomNo);
	closesocket(session->sock);
	delete session;
	session = nullptr;
}

kks::Room* NetManager::GetRoomInfo(DWORD roomID)
{
	return rooms[roomID];
}

kks::Session* NetManager::GetSession(DWORD uid)
{
	return sessions[uid];
}

int NetManager::GetUserCount(void)
{
	return sessions.size();
}


void NetManager::DeployFdSet(FD_SET& rSet, FD_SET& wSet, std::list<kks::Session*>& sessions)
{
	FD_ZERO(&rSet);
	FD_ZERO(&wSet);

	SOCKET sock;
	// 1. rSet은 그냥 초기화 하고 싹 다 넣어준다.
	auto begin = sessions.begin();
	auto end = sessions.end();
	for (int i = 0; begin != end; i ++)
	{
		sock = (*begin)->sock;
		FD_SET(sock, &rSet);
		++begin;
	}

	// 2. wSet은 보유한 Session들 중에서 뭔가 보낼게 있는 녀석들만 넣도록 한다.
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
	if (in_rSetList.size() == 0 && in_wSetList.size() == 0)
	{
		return false;
	}

	INT sockStateCheck;
	bool bHasSomeAction = false;

	INT checkListSize	= in_rSetList.size() + in_wSetList.size();
	auto rSetListBegin	= in_rSetList.begin();
	auto rSetListEnd		= in_rSetList.end();
	auto wSetListBegin	= in_wSetList.begin();
	auto wSetListEnd	= in_wSetList.end();
	for (int i = 0; i < checkListSize; i++)
	{
		sockStateCheck = select(0, &(*rSetListBegin), &(*wSetListBegin), nullptr, &timeOut);
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
			bHasSomeAction = true;

			for (u_int j = 0; j < (*rSetListBegin).fd_count; j++)
			{
				out_rSessions.push_back(socketChecker[(*rSetListBegin).fd_array[j]]);
			}

			for (u_int j = 0; j < (*wSetListBegin).fd_count; j++)
			{
				out_wSessions.push_back(socketChecker[(*wSetListBegin).fd_array[j]]);
			}
		}

		if (rSetListBegin != rSetListEnd)
		{
			rSetListBegin++;
		}

		if (wSetListBegin != wSetListEnd)
		{
			wSetListBegin++;
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

inline kks::Session* NetManager::CreateSession(SOCKET sock)
{
	kks::Session* session = socketChecker[sock];
	if (session == nullptr)
	{
		DWORD uid	= CreateGUID();
		session		= new kks::Session;

		session->roomNo	= 0;
		session->sock		= sock;
		session->uid			= uid;

		socketChecker[sock]	= session;
		sessions[uid]				= session;
	}
	return session;
}