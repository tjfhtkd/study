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
	
	/**
	@param	SOCKET	$sock ���ǿ� ����� socket
	@param	const WCHAR[]	$nickName	nullptr�� �г����� ���� ���·� �߰�, �ִٸ� �г����� ����ϸ鼭 �߰�
	@return	bool	true�� ���������� �߰��� ���, false�� �г����� �̹� ������̶� �߰��� �� ���� ����.
	*/
	bool AddSession(SOCKET sock, const WCHAR nickName[dfNICK_MAX_LEN]);

public:
	bool IsServerRun;

private:
	// sessions ���� �������� fd_set rset, fd_set wset�� �����Ѵ�.
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

	inline kks::Session* CreateSession(SOCKET sock);

private:
	DWORD next_UID;			// �ϴ� ID ���� �ֱⰡ ������ �����ϰ� �׳� DWORD++ �ϵ��� ��.
	DWORD next_RoomID;		// ���߿� GUID_32BIT ���� �־ �ٲܰ���.

	NetConnector*			m_conn;
	NetPacketProcessor*	m_netPacketProcessor;

	std::unordered_map<DWORD, kks::Session*>	sessions;
	std::unordered_map<SOCKET, kks::Session*>	socketChecker;
	std::unordered_map<std::wstring, DWORD>	nicknameChecker;

	std::unordered_map<DWORD, kks::Room*>	rooms;
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
