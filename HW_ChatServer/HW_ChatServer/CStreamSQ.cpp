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
	// �� �̻����� ����������
	// 1. �ҽ� ������ ���δ�. (�� ���� ���� �ʿ�)
	// 2. memcpy ������ �ø���.
	int writePos = m_writePos;
	int readPos = m_readPos;
	int bufSize = m_bufSize;

	// ���� ���Ϸ����� ���� ���꺸�� if���� �켼��. ���������� if���� �� ����. �б� ������ ���� �ִ� ������ ����.
	// ������ ����-����-������ ��� ���� �׷� ������ ������.
	// https://code.i-harness.com/ko/q/1086a01  < ������ �Ͱ� ����� ���뿡 ���� ����� ��
	int writableSize = 0;
	if (writePos >= readPos)
	{
		writableSize = bufSize - (writePos + 1);
	}
	else
	{
		writableSize = readPos - (writePos + 1);
	}

	if (writableSize >= iByteOfData)	// ������ �뷮���� ���������� ���ٸ�
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
	else  // ���������� �� ���� �� �� �ִ� ���� �������� �� ���� �뷮�� �䱸��
	{
		// �ϴ� �����ʿ��� �ִµ��� �� ��
		memcpy_s(&m_queue[writePos + 1], writableSize, chpData, writableSize);
		writePos += writableSize;
		if (writePos + 1 >= bufSize)
		{
			writePos -= bufSize;
		}

		int remainByte	= iByteOfData - writableSize;
		int freeSize = 0;			// ���� �뷮 ����
		if (writePos < readPos)
		{
			freeSize = readPos - (writePos + 1);
		}
		else
		{
			freeSize = bufSize - (writePos + 1);
		}

		if (freeSize == 0)	// ���� �뷮�� ���ٸ� ����
		{
			m_writePos = writePos;
			return writableSize;
		}

		// ���� �뷮�� �ִµ�
		if (remainByte > freeSize) // ���� �뷮���ٵ� ������ ����� �뷮�� ũ�ٸ�
		{
			remainByte = freeSize; // ��� ������ �뷮������ ��
		}
		memcpy_s(&m_queue[writePos + 1], remainByte, &chpData[writableSize], remainByte);
		writePos += remainByte;

		m_writePos = writePos;
		return remainByte + writableSize;
	}
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
	int i;
	int bufSize	= m_bufSize;
	int front		= m_readPos;
	for (i = 0; i < iSize; i++)
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