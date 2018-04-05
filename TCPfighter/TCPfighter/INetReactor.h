#pragma once

#include "stdafx.h"

class NetMessenger;

class INetReactor
{
public:
	virtual int ReactWhenConnected(NetMessenger* messenger) = 0;
	virtual int ReactWhenReadable(NetMessenger* messenger) = 0;
	virtual int ReactWhenWritable(NetMessenger* messenger) = 0;
	virtual int ReactWhenClose(SOCKET socket) = 0;
};