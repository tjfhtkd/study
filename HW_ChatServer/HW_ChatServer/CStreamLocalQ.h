#pragma once

/*
 지역변수처럼 사용할 Q이며, 1회 소모성이다.
 모든 로직도 1회 소모성이라는 가정으로 짜여져있다.
 목적 자체도 하나의 완성된 패킷을 뽑기 위함이다.

  현재는 기본 자료형에 대해서만 template
*/
class CStreamLocalQ
{
public:
	const static int DEFAULT_QSIZ = 2048;
	const static int DEFAULT_HEADER_SPACE_SIZ = 256;

public:
	CStreamLocalQ(size_t defaultQSize = DEFAULT_QSIZ, size_t defaultHeaderSize = DEFAULT_HEADER_SPACE_SIZ);
	virtual ~CStreamLocalQ();

public:
	size_t SetHeader(void* header, size_t headerSize);
	size_t Enqueue(void* data, size_t byteLen);
	size_t Dequeue(void* out_data, size_t size);
	size_t Peek(void* out_data, size_t size);

	void MoveWritePos(size_t moveCnt);

	unsigned char* GetReadBufferPtr(void);
	unsigned char* GetWriteBufferPtr(void);

	size_t GetUseSize(void);
	size_t GetFreeSize(void);

	bool IsEmpty();

	void Clear(void);

	CStreamLocalQ& operator<< (CStreamSQ& obj);
	CStreamLocalQ& operator>> (CStreamSQ& obj);

	template<class T>
	friend CStreamLocalQ& operator>> (CStreamLocalQ& obj, T* data);

	//friend CStreamLocalQ& operator>> (CStreamLocalQ& obj, std::string* data);
	//friend CStreamLocalQ& operator>> (CStreamLocalQ& obj, std::wstring* data);

	template<class T>
	friend CStreamLocalQ& operator<< (CStreamLocalQ& obj, T* out_data);

private:
	unsigned char* m_pHeaderBack;
	unsigned char* m_pHeaderFront;
	unsigned char* m_pBodyFront;
	unsigned char* m_pBodyBack;
	unsigned char* m_buf;
	size_t	m_size;
	size_t m_headerSpaceSiz;
};



template<class T>
inline CStreamLocalQ& operator>>(CStreamLocalQ& obj, T* data)
{
	obj.Enqueue((char*)data, sizeof(T));
	return obj;
}

//template<>
//inline CStreamLocalQ& operator>>(CStreamLocalQ& obj, std::string* data)
//{
//	obj.Enqueue((char*)data->c_str(), sizeof(data->length()));
//	return obj;
//}

//template<>
//inline CStreamLocalQ& operator>>(CStreamLocalQ& obj, std::wstring* data)
//{
//	obj.Enqueue((char*)data->c_str(), sizeof(data->length()));
//	return obj;
//}

template<class T>
inline CStreamLocalQ& operator<<(CStreamLocalQ& obj, T* out_data)
{
	obj.Dequeue((char*)out_data, sizeof(T));
	return obj;
}