#include "stdafx.h"
#include "CStreamSQ.h"
#include "CStreamLocalQ.h"

CStreamLocalQ::CStreamLocalQ(size_t defaultQSize, size_t defaultHeaderSize)
	: m_size(defaultQSize), m_headerSpaceSiz(defaultHeaderSize)
{
	if (defaultQSize < DEFAULT_QSIZ)
	{
		defaultQSize = DEFAULT_QSIZ;
	}
	m_buf = (unsigned char*)malloc(sizeof(unsigned char*) * defaultQSize);
	Clear();
}

CStreamLocalQ::~CStreamLocalQ()
{
	free(m_buf);
}

size_t CStreamLocalQ::SetHeader(void* header, size_t headerSize)
{
	if (headerSize >= DEFAULT_HEADER_SPACE_SIZ)
	{
		return -1;
	}
	//*m_pHeaderBack = *((unsigned char*)header);
	memcpy_s(m_pHeaderBack, headerSize, header, headerSize);
	m_pHeaderBack += headerSize;
	return headerSize;
}

size_t CStreamLocalQ::Enqueue(void* data, size_t byteLen)
{
	if (byteLen >= m_size)
	{
		byteLen -= m_size;
	}

	memcpy_s(m_pBodyBack, byteLen, data, byteLen);
	m_pBodyBack += byteLen;
	return byteLen;
}

size_t CStreamLocalQ::Dequeue(void* out_data, size_t size)
{
	if (size <= 0)
	{
		return 0;
	}

	size_t headerCpySiz = (m_pHeaderBack - m_pHeaderFront);
	size_t bodyCpySiz = (m_pBodyBack - m_pBodyFront);
	unsigned char* outDataCursor = (unsigned char*)out_data;

	if (size < headerCpySiz)
	{
		headerCpySiz = size;
	}
	// header copy
	memcpy_s(outDataCursor, size, m_pHeaderFront, headerCpySiz);
	outDataCursor += headerCpySiz;
	m_pHeaderFront += headerCpySiz;

	if (size - headerCpySiz == 0)
	{
		return headerCpySiz;
	}

	if (size - headerCpySiz < bodyCpySiz)
	{
		bodyCpySiz = size - headerCpySiz;
	}
	// body copy
	memcpy_s(outDataCursor, size - headerCpySiz, m_pBodyFront, bodyCpySiz);
	m_pBodyFront += bodyCpySiz;

	return headerCpySiz + bodyCpySiz;
}

size_t CStreamLocalQ::Peek(void* out_data, size_t size)
{
	if (size <= 0)
	{
		return 0;
	}

	size_t headerCpySiz = (m_pHeaderBack - m_pHeaderFront);
	size_t bodyCpySiz = (m_pBodyBack - m_pBodyFront);
	unsigned char* outDataCursor = (unsigned char*)out_data;

	if (size < headerCpySiz)
	{
		headerCpySiz = size;
	}
	// header copy
	memcpy_s(outDataCursor, size, m_pHeaderFront, headerCpySiz);
	outDataCursor += headerCpySiz;

	if (size - headerCpySiz == 0)
	{
		return headerCpySiz;
	}

	if (size - headerCpySiz < bodyCpySiz)
	{
		bodyCpySiz = size - headerCpySiz;
	}
	// body copy
	memcpy_s(outDataCursor, size - headerCpySiz, m_pBodyFront, bodyCpySiz);

	return headerCpySiz + bodyCpySiz;
}

void CStreamLocalQ::MoveWritePos(size_t moveCnt)
{
	m_pBodyBack += moveCnt;
}

unsigned char* CStreamLocalQ::GetReadBufferPtr(void)
{
	return m_pBodyFront;
}

unsigned char* CStreamLocalQ::GetWriteBufferPtr(void)
{
	return m_pBodyBack;
}

size_t CStreamLocalQ::GetUseSize(void)
{
	size_t headerCpySiz = (m_pHeaderBack - m_pHeaderFront);
	size_t bodyCpySiz = (m_pBodyBack - m_pBodyFront);
	return headerCpySiz + bodyCpySiz;
}

size_t CStreamLocalQ::GetFreeSize(void)
{
	size_t headerCpySiz = (m_pHeaderBack - m_pHeaderFront);
	size_t bodyCpySiz = (m_pBodyBack - m_pBodyFront);
	return m_size - (headerCpySiz + bodyCpySiz);
}

bool CStreamLocalQ::IsEmpty()
{
	size_t headerCpySiz = (m_pHeaderBack - m_pHeaderFront);
	size_t bodyCpySiz = (m_pBodyBack - m_pBodyFront);
	if (headerCpySiz == 0 && bodyCpySiz == 0)
	{
		return true;
	}
	return false;
}

void CStreamLocalQ::Clear(void)
{
	m_pHeaderFront = m_pHeaderBack = m_buf;

	m_pBodyBack = m_pBodyFront = m_buf;
	m_pBodyFront += DEFAULT_HEADER_SPACE_SIZ;
	m_pBodyBack += DEFAULT_HEADER_SPACE_SIZ;
}

CStreamLocalQ& CStreamLocalQ::operator<<(CStreamSQ& obj)
{
	size_t dataSize = obj.GetUseSize();
	if (dataSize > m_size)
	{
		dataSize -= m_size;
	}
	char* buf = new char[dataSize];
	obj.Dequeue(buf, (int)dataSize);
	this->Enqueue(buf, (int)dataSize);
	delete buf;
	return *this;
}

CStreamLocalQ& CStreamLocalQ::operator>>(CStreamSQ& obj)
{
	size_t headerCpySiz = (m_pHeaderBack - m_pHeaderFront);
	size_t bodyCpySiz = (m_pBodyBack - m_pBodyFront);
	size_t dataSize = headerCpySiz + bodyCpySiz;
	char* buf = new char[dataSize];

	this->Dequeue(buf, dataSize);
	obj.Enqueue(buf, (int)dataSize);
	delete buf;

	return *this;
}
