#pragma once

class NetConnector;
class NetPacketProcessor;

class NetManager
{
public:
	NetManager();
	virtual ~NetManager();

public:
	bool Connect(void);
	void StartLoop(void);
	
	// 로그인 시도할 때 이거 호출. true이면 성공적으로 로긴, 실패하면 false 반환
	bool AddSession(SOCKET sock, const WCHAR nickName[dfNICK_MAX_LEN]);

public:
	bool IsServerRun;

private:
	// sessions 맵을 기준으로 fd_set rset, fd_set wset을 세팅한다.
	void DeployFdSet(FD_SET& rSet, FD_SET& wSet, std::list<kks::Session*>& sessions);
	void GetFdSetList(std::list<FD_SET>& out_rSetList, std::list<FD_SET>& out_wSetList);
	bool GetSelectedSessions(
		std::list<kks::Session*>& out_rSessions
		, std::list<kks::Session*>& out_wSessions
		, std::list<FD_SET>& in_rSetList
		, std::list<FD_SET>& in_wSetList
		, timeval& timeOut
	);
	bool MakeSelectedSessions(
		std::list<FD_SET>& checkList
		, std::list<kks::Session*>& out_list
		, bool bReadSet
		, timeval& timeOut);

	// Global User ID
	DWORD CreateGUID(void);
	// Global Room ID
	DWORD CreateGRID(void);

	template <class V>
	bool IsAlreadyExistStrKeyIn(std::unordered_map<std::wstring, V>& map, std::wstring key);

private:
	DWORD next_UID;			// 일단 ID 리셋 주기가 빠르다 가정하고 그냥 DWORD++ 하도록 함.
	DWORD next_RoomID;		// 나중에 GUID_32BIT 개념 넣어서 바꿀거임.

	NetConnector*			m_conn;
	NetPacketProcessor*	m_netPacketProcessor;

	std::unordered_map<DWORD, kks::Session*>	sessions;
	std::unordered_map<DWORD, kks::Room*>	rooms;
	std::unordered_map<std::wstring, DWORD>	nicknameChecker;
	std::unordered_map<SOCKET, kks::Session*>	socketChecker;
};


template<class V>
inline bool NetManager::IsAlreadyExistStrKeyIn(std::unordered_map<std::wstring, V>& map, std::wstring key)
{
	size_t checker = map.size();
	map[key.c_str()] = 0;
	if (checker == map.size()) {
		return true;
	}
	else {
		map.erase(key);
		return false;
	}
}
