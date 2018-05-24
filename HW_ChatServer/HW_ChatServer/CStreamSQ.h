#pragma once

class CStreamLocalQ;

class CStreamSQ
{
public:
	CStreamSQ();
	CStreamSQ(int iBufferSize);

	virtual ~CStreamSQ();

public:
	//void	Resize(int size);
	// Resize�� ũ�⸸ �ø��� �����Ϳ� ���� ó���� ��� �ؾ����� ���� �̸� �ٲ�.
	void	ReCreateQueue(int size);
	int		GetBufferSize(void);

	/////////////////////////////////////////////////////////////////////////
	// ���� ������� �뷮 ���.
	//
	// Parameters: ����.
	// Return: (int)������� �뷮.
	/////////////////////////////////////////////////////////////////////////
	int	GetUseSize(void);

	/////////////////////////////////////////////////////////////////////////
	// ���� ���ۿ� ���� �뷮 ���.
	//
	// Parameters: ����.
	// Return: (int)�����뷮.
	/////////////////////////////////////////////////////////////////////////
	int	GetFreeSize(void);

	/////////////////////////////////////////////////////////////////////////
	// ���� �����ͷ� �ܺο��� �ѹ濡 �а�, �� �� �ִ� ����.
	// (������ ���� ����)
	//
	// Parameters: ����.
	// Return: (int)��밡�� �뷮.
	////////////////////////////////////////////////////////////////////////
	//int	GetNotBrokenGetSize(void);
	//int	GetNotBrokenPutSize(void);
	int		GetWriteableSizeAtOneTime(void);
	int		GetReadableSizeAtOneTime(void);

	/////////////////////////////////////////////////////////////////////////
	// WritePos �� ����Ÿ ����.
	//
	// Parameters: (char *)����Ÿ ������. (int)ũ��. 
	// Return: (int)���� ũ��.
	/////////////////////////////////////////////////////////////////////////
	int	Enqueue(char *chpData, int iByteOfData);

	/////////////////////////////////////////////////////////////////////////
	// ReadPos ���� ����Ÿ ������. ReadPos �̵�.
	//
	// Parameters: (char *)����Ÿ ������. (int)ũ��.
	// Return: (int)������ ũ��.
	/////////////////////////////////////////////////////////////////////////
	int	Dequeue(char *chpDest, int iByteOfData);

	/////////////////////////////////////////////////////////////////////////
	// ReadPos ���� ����Ÿ �о��. ReadPos ����.
	//
	// Parameters: (char *)����Ÿ ������. (int)ũ��.
	// Return: (int)������ ũ��.
	/////////////////////////////////////////////////////////////////////////
	int	Peek(char *chpDest, int iSize);


	/////////////////////////////////////////////////////////////////////////
	// ���ϴ� ���̸�ŭ �б���ġ ���� ���� / ���� ��ġ �̵�
	//
	// Parameters: ����.
	// Return: ����.
	/////////////////////////////////////////////////////////////////////////
	void	RemoveData(int iByteOfData);
	int		MoveWritePos(int iByteOfData);

	/////////////////////////////////////////////////////////////////////////
	// ������ ��� ����Ÿ ����.
	//
	// Parameters: ����.
	// Return: ����.
	/////////////////////////////////////////////////////////////////////////
	void	ClearBuffer(void);

	/////////////////////////////////////////////////////////////////////////
	// ������ ������ ����.
	//
	// Parameters: ����.
	// Return: (char *) ���� ������.
	/////////////////////////////////////////////////////////////////////////
	char	*GetBufferPtr(void);

	/////////////////////////////////////////////////////////////////////////
	// ������ ReadPos ������ ����.
	//
	// Parameters: ����.
	// Return: (char *) ���� ������.
	/////////////////////////////////////////////////////////////////////////
	char	*GetReadBufferPtr(void);

	/////////////////////////////////////////////////////////////////////////
	// ������ WritePos ������ ����.
	//
	// Parameters: ����.
	// Return: (char *) ���� ������.
	/////////////////////////////////////////////////////////////////////////
	char	*GetWriteBufferPtr(void);

	bool IsFull(INT wPos, INT rPos, INT bufSize);
	bool IsEmpty(void);

	CStreamSQ& operator<< (CStreamSQ& obj);
	CStreamSQ& operator>> (CStreamLocalQ& obj);

	template<class T>
	friend CStreamSQ& operator>> (CStreamSQ& obj, T& data);

	template<class T>
	friend CStreamSQ& operator<< (CStreamSQ& obj, T* out_data);


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
inline CStreamSQ& operator>>(CStreamSQ& obj, T& data)
{
	obj.Enqueue((char*)&data, sizeof(T));
	return obj;
}

template<class T>
inline CStreamSQ& operator<<(CStreamSQ& obj, T* out_data)
{
	obj.Dequeue((char*)out_data, sizeof(T));
	return obj;
}