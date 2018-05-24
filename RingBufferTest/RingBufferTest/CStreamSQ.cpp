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
	int bufSize = m_bufSize;
	int readPos = m_readPos;
	int writePos = m_writePos;

	// �ȿ� ���� ���빰�� ��̴���?
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
		// �� �͵� ����. ������ ���µ� �� �ϳ�.
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
	else  // �װ� ���ϴ� ��ŭ �������ŵ� �߰��� �߷����� �˶�.
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

	// �ȿ� ���� ���빰�� ��̴���?
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
		// �� �͵� ����. ������ ���µ� �� �ϳ�.
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
	else  // �װ� ���ϴ� ��ŭ �������ŵ� �߰��� �߷����� �˶�.
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