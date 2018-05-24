// RingBufferTest.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "CStreamSQ.h"
#include "CStreamLocalQ.h"

int main()
{
	char testParse[500] = "qwertEWQ4567984958$!1234!!oi;hajwoei&#$%^ 1234567890 abcdefghijnopqrstuvwxyz 1234567890 abcdefghijklmnopqrstuvwxyz 12345";
	char buf[1000];
	char peekBuf[1000];
	CStreamSQ ringBuf(10);

	int start = 0;
	int	end = 0;
	int tmpLenEnQ;
	int enQRemain;

	int dq_start = 0;
	int	dq_end = 0;
	int tmpLenDeQ;
	int deQRemain;

	int len = strlen(testParse);
	//int testCnt = 100000;
	while (1)
	{
		// Random Enqueue
		tmpLenEnQ = strlen(&testParse[start]);
		if (tmpLenEnQ > 5)
		{
			end = (rand() % (tmpLenEnQ - 1)) + 1;
		}
		else
		{
			end = tmpLenEnQ;
		}
		enQRemain = ringBuf.Enqueue(&testParse[start], end);
		start += enQRemain;
		if (start >= len)
		{
			start = 0;
		}

		// Random Dequeue
		tmpLenDeQ = strlen(&testParse[dq_start]);
		if (tmpLenDeQ > 5)
		{
			dq_end = (rand() % (tmpLenDeQ - 1)) + 1;
		}
		else
		{
			dq_end = tmpLenDeQ;
		}
		// Peek check
		//int peekSize = ringBuf.Peek(&peekBuf[dq_start], dq_end);
		// Deqeue
		deQRemain = ringBuf.Dequeue(&buf[dq_start], dq_end);
		//if (memcmp(&peekBuf[dq_start], &buf[dq_start], deQRemain) != 0)
		//{
		//	printf("\nSomething is wrong!! %s %d ~ %d\n", peekBuf, dq_start, dq_end);
		//	break;
		//}

		dq_start += deQRemain;
		if (dq_start >= len)
		{
			buf[len] = '\0';
			printf("%s", buf);
			buf[0] = '\0';
			dq_start = 0;
		}
	}
    return 0;
}