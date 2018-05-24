#include "stdafx.h"
#include "CStreamSQ.h"


CStreamSQ::CStreamSQ()
{
	Initial(1000);
}

CStreamSQ::CStreamSQ(int iBufferSize)
{
	Initial(iBufferSize);
}


CStreamSQ::~CStreamSQ()
{
	Release();
}

////////////////////////////////////////////////// PUBLIC //////////////////////////////////////////////////
void CStreamSQ::ReCreateQueue(int size)
{
	Release();
	Initial(size);
}

int CStreamSQ::GetBufferSize(void)
{
	return m_bufSize - 1;
}

int CStreamSQ::GetUseSize(void)
{
	return (m_bufSize - 1) - GetFreeSize();
}

int CStreamSQ::GetFreeSize(void)
{
	return ((m_bufSize - (m_writePos + 1)) + m_readPos) % m_bufSize;
}

int CStreamSQ::GetWriteableSizeAtOneTime(void)
{
	if (m_writePos >= m_readPos)
	{
		return m_bufSize - (m_writePos + 1);
	}
	return m_readPos - (m_writePos + 1);
}

int CStreamSQ::GetReadableSizeAtOneTime(void)
{
	if (m_readPos >= m_writePos)
	{
		return m_bufSize - (m_readPos + 1);
	}
	return  m_writePos - m_readPos;
}

int CStreamSQ::Enqueue(char *chpData, int iByteOfData)
{
	// 이 이상으로 빨라지려면
	// 1. 소스 라인을 줄인다. (더 나은 로직 필요)
	// 2. memcpy 성능을 올린다.
	int writePos = m_writePos;
	int readPos = m_readPos;
	int bufSize = m_bufSize;

	// 라인 부하량에서 삼항 연산보다 if문이 우세함. 조건적으로 if문이 더 빠름. 분기 예측과 관련 있는 것으로 보임.
	// 삼항은 조건-참문-거짓문 모두 들어가서 그런 것으로 추측됨.
	// https://code.i-harness.com/ko/q/1086a01  < 추측한 것과 비슷한 내용에 대해 실험된 글
	int writableSize = 0;
	if (writePos >= readPos)
	{
		writableSize = bufSize - (writePos + 1);
	}
	else
	{
		writableSize = readPos - (writePos + 1);
	}

	if (writableSize >= iByteOfData)	// 쓰려는 용량보다 여유공간이 많다면
	{
		memcpy_s(&m_queue[writePos + 1], iByteOfData, chpData, iByteOfData);
		writePos += iByteOfData;
		if (writePos >= bufSize - 1)
		{
			writePos -= bufSize;
		}
		m_writePos = writePos;
		return iByteOfData;
	}
	else  // 오른쪽으로 한 번에 쓸 수 있는 여유 공간보다 더 많은 용량이 요구됨
	{
		// 일단 오른쪽에는 있는데로 다 씀
		memcpy_s(&m_queue[writePos + 1], writableSize, chpData, writableSize);
		writePos += writableSize;
		if (writePos + 1 >= bufSize)
		{
			writePos -= bufSize;
		}

		int remainByte	= iByteOfData - writableSize;
		int freeSize = 0;			// 남은 용량 재계산
		if (writePos < readPos)
		{
			freeSize = readPos - (writePos + 1);
		}
		else
		{
			freeSize = bufSize - (writePos + 1);
		}

		if (freeSize == 0)	// 남은 용량이 없다면 종료
		{
			m_writePos = writePos;
			return writableSize;
		}

		// 남은 용량이 있는데
		if (remainByte > freeSize) // 남은 용량보다도 여전히 써야할 용량이 크다면
		{
			remainByte = freeSize; // 사용 가능한 용량까지만 씀
		}
		memcpy_s(&m_queue[writePos + 1], remainByte, &chpData[writableSize], remainByte);
		writePos += remainByte;

		m_writePos = writePos;
		return remainByte + writableSize;
	}
}

int CStreamSQ::Dequeue(char *chpDest, int iByteOfData)
{
	int bufSize = m_bufSize;
	int readPos = m_readPos;
	int writePos = m_writePos;

	// 안에 읽을 내용물이 몇개이느뇨?
	int readableSize;
	int contentsSize;
	if (readPos >= writePos)
	{
		readableSize = bufSize - (readPos + 1);
		contentsSize = bufSize - readPos + writePos;
	}
	else
	{
		readableSize = writePos - readPos;
		contentsSize = writePos - readPos;
	}

	if (contentsSize == 0)
	{
		// 볼 것도 없다. 읽을게 없는데 뭘 하냐.
		return 0;
	}

	if(iByteOfData <= readableSize)
	{
		memcpy_s(chpDest, iByteOfData, &m_queue[readPos + 1], iByteOfData);
		readPos += iByteOfData;
		while (readPos >= bufSize)
		{
			readPos -= bufSize;
		}
		m_readPos = readPos;
		return iByteOfData;
	}
	else  // 네가 원하는 만큼 읽으려거든 중간이 잘렸음을 알라.
	{
		memcpy_s(chpDest, readableSize, &m_queue[readPos + 1], readableSize);
		readPos += readableSize;
		while (readPos >= bufSize)
		{
			readPos -= bufSize;
		}
		
		if (readPos == writePos)
		{
			m_readPos = readPos;
			return readableSize;
		}

		int remainByte = iByteOfData - readableSize;
		int useSize = (bufSize - 1) - ((bufSize - (writePos + 1)) + readPos) % bufSize;
		if (remainByte > useSize)
		{
			remainByte = useSize;
		}
		memcpy_s(&chpDest[readableSize], remainByte, &m_queue[(readPos + 1) % bufSize], remainByte);

		readPos += remainByte;
		while (readPos >= bufSize)
		{
			readPos -= bufSize;
		}

		m_readPos = readPos;
		return readableSize + remainByte;
	}
}

int CStreamSQ::Peek(char *chpDest, int iSize)
{
	int bufSize = m_bufSize;
	int readPos = m_readPos;
	int writePos = m_writePos;

	// 안에 읽을 내용물이 몇개이느뇨?
	int readableSize;
	int contentsSize;
	if (readPos >= writePos)
	{
		readableSize = bufSize - (readPos + 1);
		contentsSize = bufSize - readPos + writePos;
	}
	else
	{
		readableSize = writePos - readPos;
		contentsSize = writePos - readPos;
	}

	if (contentsSize == 0)
	{
		// 볼 것도 없다. 읽을게 없는데 뭘 하냐.
		return 0;
	}

	if (iSize <= readableSize)
	{
		memcpy_s(chpDest, iSize, &m_queue[readPos + 1], iSize);
		readPos += iSize;
		while (readPos >= bufSize)
		{
			readPos -= bufSize;
		}
		return iSize;
	}
	else  // 네가 원하는 만큼 읽으려거든 중간이 잘렸음을 알라.
	{
		memcpy_s(chpDest, readableSize, &m_queue[readPos + 1], readableSize);
		readPos += readableSize;
		while (readPos >= bufSize)
		{
			readPos -= bufSize;
		}

		if (readPos == writePos)
		{
			return readableSize;
		}

		int remainByte = iSize - readableSize;
		int useSize = (bufSize - 1) - ((bufSize - (writePos + 1)) + readPos) % bufSize;
		if (remainByte > useSize)
		{
			remainByte = useSize;
		}
		memcpy_s(&chpDest[readableSize], remainByte, &m_queue[(readPos + 1) % bufSize], remainByte);

		readPos += remainByte;
		while (readPos >= bufSize)
		{
			readPos -= bufSize;
		}

		return readableSize + remainByte;
	}
}

void CStreamSQ::RemoveData(int iByteOfData)
{
	int bufSize = m_bufSize;
	for (int i = 0; i < iByteOfData; i++)
	{
		if (IsEmpty() == true)
		{
			return;
		}
		m_readPos = (m_readPos + 1) % bufSize;
	}
}

int CStreamSQ::MoveWritePos(int iByteOfData)
{
	int bufSize	= m_bufSize;
	int wPos		= m_writePos;
	int rPos		= m_readPos;
	int i			= 0;
	int t;

	for (; i < iByteOfData; i++)
	{
		if ((wPos + 1) >= bufSize)
		{
			wPos = 0;
			t = bufSize - 1;
		}
		else
		{
			wPos++;
			t = wPos;
		}
		if ((rPos == t) ? true : false)
		{
			break;
		}
	}
	m_writePos = wPos;
	return i;
}

void CStreamSQ::ClearBuffer(void)
{
	m_writePos = m_readPos = 0;
}

char* CStreamSQ::GetBufferPtr(void)
{
	return (char*)&m_queue[0];
}

char* CStreamSQ::GetReadBufferPtr(void)
{
	int front = (m_readPos + 1) % m_bufSize;
	return (char*)&m_queue[front];
}

char* CStreamSQ::GetWriteBufferPtr(void)
{
	int rear = (m_writePos + 1) % m_bufSize;
	return (char*)&m_queue[rear];
}

bool CStreamSQ::IsFull(INT wPos, INT rPos, INT bufSize)
{
	int t = wPos + 1;
	if ((wPos + 1) >= bufSize)
	{
		t = bufSize - 1;
	}

	if (rPos == t)
	{
		return true;
	}
	return false;
}

bool CStreamSQ::IsEmpty(void)
{
	return (m_readPos == m_writePos)? true : false;
}

CStreamSQ& CStreamSQ::operator<<(CStreamSQ& obj)
{
	int readSize;
	int dataSize = obj.GetUseSize();
	char buf[1024];
	while (dataSize > 0)
	{
		readSize = obj.Dequeue(buf, 1024);
		dataSize -= readSize;
		this->Enqueue(buf, readSize);
	}
	return obj;
}

CStreamSQ& CStreamSQ::operator>>(CStreamLocalQ& obj)
{
	int deqSize = this->Dequeue((char*)obj.GetWriteBufferPtr(), (int)obj.GetFreeSize());
	obj.MoveWritePos(deqSize);
	return *this;
}

////////////////////////////////////////////////// PRIVATE //////////////////////////////////////////////////
void CStreamSQ::Initial(int iBufferSize)
{
	if (iBufferSize > 1)
	{
		m_bufSize = iBufferSize;
	}
	else
	{
		m_bufSize = 100;
	}

	m_queue = new BYTE[m_bufSize];
	m_writePos = m_readPos = 0;
}

void CStreamSQ::Release(void)
{
	if (m_queue != nullptr)
	{
		delete[] m_queue;
		m_queue = nullptr;
	}
}