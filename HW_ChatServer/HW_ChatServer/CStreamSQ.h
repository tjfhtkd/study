#pragma once
class CStreamSQ
{
public:
	CStreamSQ();
	CStreamSQ(int iBufferSize);

	virtual ~CStreamSQ();

public:
	//void	Resize(int size);
	// Resize는 크기만 늘리고 데이터에 대한 처리를 어떻게 해야할지 몰라서 이름 바꿈.
	void	ReCreateQueue(int size);
	int		GetBufferSize(void);

	/////////////////////////////////////////////////////////////////////////
	// 현재 사용중인 용량 얻기.
	//
	// Parameters: 없음.
	// Return: (int)사용중인 용량.
	/////////////////////////////////////////////////////////////////////////
	int	GetUseSize(void);

	/////////////////////////////////////////////////////////////////////////
	// 현재 버퍼에 남은 용량 얻기.
	//
	// Parameters: 없음.
	// Return: (int)남은용량.
	/////////////////////////////////////////////////////////////////////////
	int	GetFreeSize(void);

	/////////////////////////////////////////////////////////////////////////
	// 버퍼 포인터로 외부에서 한방에 읽고, 쓸 수 있는 길이.
	// (끊기지 않은 길이)
	//
	// Parameters: 없음.
	// Return: (int)사용가능 용량.
	////////////////////////////////////////////////////////////////////////
	//int	GetNotBrokenGetSize(void);
	//int	GetNotBrokenPutSize(void);
	int		GetWriteableSizeAtOneTime(void);
	int		GetReadableSizeAtOneTime(void);

	/////////////////////////////////////////////////////////////////////////
	// WritePos 에 데이타 넣음.
	//
	// Parameters: (char *)데이타 포인터. (int)크기. 
	// Return: (int)넣은 크기.
	/////////////////////////////////////////////////////////////////////////
	int	Enqueue(char *chpData, int iByteOfData);

	/////////////////////////////////////////////////////////////////////////
	// ReadPos 에서 데이타 가져옴. ReadPos 이동.
	//
	// Parameters: (char *)데이타 포인터. (int)크기.
	// Return: (int)가져온 크기.
	/////////////////////////////////////////////////////////////////////////
	int	Dequeue(char *chpDest, int iByteOfData);

	/////////////////////////////////////////////////////////////////////////
	// ReadPos 에서 데이타 읽어옴. ReadPos 고정.
	//
	// Parameters: (char *)데이타 포인터. (int)크기.
	// Return: (int)가져온 크기.
	/////////////////////////////////////////////////////////////////////////
	int	Peek(char *chpDest, int iSize);


	/////////////////////////////////////////////////////////////////////////
	// 원하는 길이만큼 읽기위치 에서 삭제 / 쓰기 위치 이동
	//
	// Parameters: 없음.
	// Return: 없음.
	/////////////////////////////////////////////////////////////////////////
	void	RemoveData(int iByteOfData);
	int		MoveWritePos(int iByteOfData);

	/////////////////////////////////////////////////////////////////////////
	// 버퍼의 모든 데이타 삭제.
	//
	// Parameters: 없음.
	// Return: 없음.
	/////////////////////////////////////////////////////////////////////////
	void	ClearBuffer(void);

	/////////////////////////////////////////////////////////////////////////
	// 버퍼의 포인터 얻음.
	//
	// Parameters: 없음.
	// Return: (char *) 버퍼 포인터.
	/////////////////////////////////////////////////////////////////////////
	char	*GetBufferPtr(void);

	/////////////////////////////////////////////////////////////////////////
	// 버퍼의 ReadPos 포인터 얻음.
	//
	// Parameters: 없음.
	// Return: (char *) 버퍼 포인터.
	/////////////////////////////////////////////////////////////////////////
	char	*GetReadBufferPtr(void);

	/////////////////////////////////////////////////////////////////////////
	// 버퍼의 WritePos 포인터 얻음.
	//
	// Parameters: 없음.
	// Return: (char *) 버퍼 포인터.
	/////////////////////////////////////////////////////////////////////////
	char	*GetWriteBufferPtr(void);

	bool IsFull(INT wPos, INT rPos, INT bufSize);
	bool IsEmpty(void);

	CStreamSQ& operator<< (CStreamSQ& obj);

	template<class T>
	friend CStreamSQ& operator>> (CStreamSQ& obj, T data);

	friend CStreamSQ& operator>> (CStreamSQ& obj, std::string* data);

	template<class T>
	friend CStreamSQ& operator<< (CStreamSQ& obj, T& out_data);


private:
	void	Initial(int iBufferSize);
	void	Release(void);

private:
	INT		m_bufSize;
	INT		m_readPos;	// front
	INT		m_writePos;	// rear
	BYTE*		m_queue;
};



template<class T>
inline CStreamSQ& operator>>(CStreamSQ& obj, T data)
{
	obj.Enqueue(data, sizeof(T));
	return obj;
}

template<>
inline CStreamSQ & operator>>(CStreamSQ & obj, std::string* data)
{
	obj.Enqueue((char*)data->c_str(), sizeof(data->length()));
	return obj;
}

template<class T>
inline CStreamSQ& operator<<(CStreamSQ& obj, T& out_data)
{
	obj.Dequeue(out_data, sizeof(T));
	return obj;
}