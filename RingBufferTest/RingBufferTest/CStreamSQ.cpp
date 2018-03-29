#include "stdafx.h"
#include "CStreamSQ.h"


CStreamSQ::CStreamSQ()
{
	Initial(100);
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
	int writableSize = (m_writePos >= m_readPos) ? m_bufSize - (m_writePos + 1) : m_readPos - (m_writePos + 1);
	if (writableSize >= iByteOfData)
	{
		memcpy_s(&m_queue[(m_writePos + 1) % m_bufSize], iByteOfData, chpData, iByteOfData);
		return MoveWritePos(iByteOfData);
	}
	else
	{
		memcpy_s(&m_queue[(m_writePos + 1) % m_bufSize], writableSize, chpData, writableSize);
		int writedByte	= MoveWritePos(writableSize);
		int remainByte	= iByteOfData - writedByte;
		int freeSize		= GetFreeSize();
		if (freeSize == 0)
		{
			return writedByte;
		}

		if (remainByte > freeSize)
		{
			remainByte = freeSize;
		}
		memcpy_s(&m_queue[(m_writePos + 1) % m_bufSize], remainByte, &chpData[writedByte], remainByte);
		return MoveWritePos(remainByte) + writedByte;
	}
	return 0;
}

int CStreamSQ::Dequeue(char *chpDest, int iByteOfData)
{
	int readableSize = GetReadableSizeAtOneTime();
	if (readableSize >= iByteOfData)
	{
		memcpy_s(chpDest, iByteOfData, &m_queue[(m_readPos + 1) % m_bufSize], iByteOfData);
		RemoveData(iByteOfData);
		return iByteOfData;
	}
	else
	{
		memcpy_s(chpDest, readableSize, &m_queue[(m_readPos + 1) % m_bufSize], readableSize);
		RemoveData(readableSize);
		int remainByte = iByteOfData - readableSize;
		if (IsEmpty())
		{
			return readableSize;
		}

		int useSize = GetUseSize();
		if (remainByte > useSize)
		{
			remainByte = useSize;
		}
		memcpy_s(&chpDest[readableSize], remainByte, &m_queue[(m_readPos + 1) % m_bufSize], remainByte);
		RemoveData(remainByte);
		return remainByte + readableSize;
	}
	return 0;
}

int CStreamSQ::Peek(char *chpDest, int iSize)
{
	int i			= 0;
	int bufSize	= m_bufSize;
	int front		= m_readPos;
	for (int i = 0; i < iSize; i++)
	{
		if (front == m_writePos)
		{
			return i;
		}
		front = (front + 1) % bufSize;
		chpDest[i] = m_queue[front];
	}
	return i;
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
	return (char*)&m_queue[m_readPos];
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