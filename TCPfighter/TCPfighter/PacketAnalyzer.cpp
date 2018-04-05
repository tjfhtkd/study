#include "stdafx.h"
#include "PacketDefine.h"
#include "PacketAnalyzer.h"


PacketAnalyzer::PacketAnalyzer()
{
}


PacketAnalyzer::~PacketAnalyzer()
{
}

bool PacketAnalyzer::Encode(OUT char** out_packet, OUT INT* out_len, IN BYTE Direction, IN WORD X, IN WORD Y, IN INT type)
{
	st_NETWORK_PACKET_HEADER header;
	header.byCode = dfNETWORK_PACKET_CODE;
	header.byType = type;
	header.byTemp = 0xcc;

	bool retVal = true;
	switch (type)
	{
	case dfPACKET_CS_MOVE_START:
	{
		int headerSize = sizeof(st_NETWORK_PACKET_HEADER);
		int packetSize = sizeof(stPACKET_CS_MOVE_START);
		int endCodeSize = sizeof(dfNETWORK_PACKET_END);
		*out_packet = (char*)malloc(headerSize + packetSize + endCodeSize);
		
		stPACKET_CS_MOVE_START packet;
		packet.Direction = Direction;
		packet.X = X;
		packet.Y = Y;

		header.bySize = packetSize;

		char* cursor = *out_packet;
		*((st_NETWORK_PACKET_HEADER*)cursor) = header;
		*((stPACKET_CS_MOVE_START*)(cursor + headerSize)) = packet;
		*((BYTE*)(cursor + headerSize + packetSize)) = dfNETWORK_PACKET_END;
		*out_len = headerSize + packetSize + endCodeSize;
		break;
	}

	case dfPACKET_CS_MOVE_STOP:
	{
		int headerSize = sizeof(st_NETWORK_PACKET_HEADER);
		int packetSize = sizeof(stPACKET_CS_MOVE_STOP);
		int endCodeSize = sizeof(dfNETWORK_PACKET_END);
		*out_packet = (char*)malloc(headerSize + packetSize + endCodeSize);

		stPACKET_CS_MOVE_STOP packet;
		packet.Direction = Direction;
		packet.X = X;
		packet.Y = Y;

		header.bySize = packetSize;

		char* cursor = *out_packet;
		*((st_NETWORK_PACKET_HEADER*)cursor) = header;
		*((stPACKET_CS_MOVE_STOP*)(cursor + headerSize)) = packet;
		*((BYTE*)(cursor + headerSize + packetSize)) = dfNETWORK_PACKET_END;
		*out_len = headerSize + packetSize + endCodeSize;
		break;
	}

	case dfPACKET_CS_ATTACK1:
	{
		int headerSize = sizeof(st_NETWORK_PACKET_HEADER);
		int packetSize = sizeof(stPACKET_CS_ATTACK1);
		int endCodeSize = sizeof(dfNETWORK_PACKET_END);
		*out_packet = (char*)malloc(headerSize + packetSize + endCodeSize);

		stPACKET_CS_ATTACK1 packet;
		packet.Direction = Direction;
		packet.X = X;
		packet.Y = Y;

		header.bySize = packetSize;

		char* cursor = *out_packet;
		*((st_NETWORK_PACKET_HEADER*)cursor) = header;
		*((stPACKET_CS_ATTACK1*)(cursor + headerSize)) = packet;
		*((BYTE*)(cursor + headerSize + packetSize)) = dfNETWORK_PACKET_END;
		*out_len = headerSize + packetSize + endCodeSize;
		break;
	}

	case dfPACKET_CS_ATTACK2:
	{
		int headerSize = sizeof(st_NETWORK_PACKET_HEADER);
		int packetSize = sizeof(stPACKET_CS_ATTACK2);
		int endCodeSize = sizeof(dfNETWORK_PACKET_END);
		*out_packet = (char*)malloc(headerSize + packetSize + endCodeSize);

		stPACKET_CS_ATTACK2 packet;
		packet.Direction = Direction;
		packet.X = X;
		packet.Y = Y;

		header.bySize = packetSize;

		char* cursor = *out_packet;
		*((st_NETWORK_PACKET_HEADER*)cursor) = header;
		*((stPACKET_CS_ATTACK2*)(cursor + headerSize)) = packet;
		*((BYTE*)(cursor + headerSize + packetSize)) = dfNETWORK_PACKET_END;
		*out_len = headerSize + packetSize + endCodeSize;
		break;
	}

	case dfPACKET_CS_ATTACK3:
	{
		int headerSize = sizeof(st_NETWORK_PACKET_HEADER);
		int packetSize = sizeof(stPACKET_CS_ATTACK3);
		int endCodeSize = sizeof(dfNETWORK_PACKET_END);
		*out_packet = (char*)malloc(headerSize + packetSize + endCodeSize);

		stPACKET_CS_ATTACK3 packet;
		packet.Direction = Direction;
		packet.X = X;
		packet.Y = Y;

		header.bySize = packetSize;

		char* cursor = *out_packet;
		*((st_NETWORK_PACKET_HEADER*)cursor) = header;
		*((stPACKET_CS_ATTACK3*)(cursor + headerSize)) = packet;
		*((BYTE*)(cursor + headerSize + packetSize)) = dfNETWORK_PACKET_END;
		*out_len = headerSize + packetSize + endCodeSize;
		break;
	}
	default:
		retVal = false;
		break;
	}
	return retVal;
}

// 패킷 프로토콜 입력을 외부에서 받고 처리하도록 하는 방법은 없나..?
INT PacketAnalyzer::Decode(IN char* in_packet, OUT stPacket_ArgCollectionBox* out_packetIntendBox)
{
	int retVal = -1;
	char* cursor = in_packet;
	st_NETWORK_PACKET_HEADER* header = (st_NETWORK_PACKET_HEADER*)cursor;

	// 제대로 된 패킷인가?
	if (header->byCode != dfNETWORK_PACKET_CODE)
	{
		return retVal;
	}

	cursor = &in_packet[sizeof(st_NETWORK_PACKET_HEADER)];
	switch (header->byType)
	{
	case dfPACKET_SC_CREATE_MY_CHARACTER:
	{
		stPACKET_SC_CREATE_MY_CHARACTER* packet = (stPACKET_SC_CREATE_MY_CHARACTER*)cursor;
		out_packetIntendBox->PacketType = dfPACKET_SC_CREATE_MY_CHARACTER;
		out_packetIntendBox->ID			= packet->ID;
		out_packetIntendBox->Direction	= packet->Direction;
		out_packetIntendBox->HP			= packet->HP;
		out_packetIntendBox->X			= packet->X;
		out_packetIntendBox->Y			= packet->Y;
		retVal = sizeof(stPACKET_SC_CREATE_MY_CHARACTER);
		break;
	}

	case dfPACKET_SC_CREATE_OTHER_CHARACTER:
	{
		stPACKET_SC_CREATE_OTHER_CHARACTER* packet = (stPACKET_SC_CREATE_OTHER_CHARACTER*)cursor;
		out_packetIntendBox->PacketType = dfPACKET_SC_CREATE_OTHER_CHARACTER;
		out_packetIntendBox->ID			= packet->ID;
		out_packetIntendBox->Direction	= packet->Direction;
		out_packetIntendBox->HP			= packet->HP;
		out_packetIntendBox->X			= packet->X;
		out_packetIntendBox->Y			= packet->Y;
		retVal = sizeof(stPACKET_SC_CREATE_OTHER_CHARACTER);
		break;
	}

	case dfPACKET_SC_DELETE_CHARACTER:
	{
		stPACKET_SC_DELETE_CHARACTER* packet = (stPACKET_SC_DELETE_CHARACTER*)cursor;
		out_packetIntendBox->PacketType = dfPACKET_SC_DELETE_CHARACTER;
		out_packetIntendBox->ID = packet->ID;
		retVal = sizeof(stPACKET_SC_DELETE_CHARACTER);
		break;
	}

	case dfPACKET_SC_MOVE_START:
	{
		stPACKET_SC_MOVE_START* packet	= (stPACKET_SC_MOVE_START*)cursor;
		out_packetIntendBox->PacketType = dfPACKET_SC_MOVE_START;
		out_packetIntendBox->ID				= packet->ID;
		out_packetIntendBox->Direction		= packet->Direction;
		out_packetIntendBox->X				= packet->X;
		out_packetIntendBox->Y				= packet->Y;
		retVal = sizeof(stPACKET_SC_MOVE_START);
		break;
	}

	case dfPACKET_SC_MOVE_STOP:
	{
		stPACKET_SC_MOVE_STOP* packet	= (stPACKET_SC_MOVE_STOP*)cursor;
		out_packetIntendBox->PacketType = dfPACKET_SC_MOVE_STOP;
		out_packetIntendBox->ID				= packet->ID;
		out_packetIntendBox->Direction		= packet->Direction;
		out_packetIntendBox->X				= packet->X;
		out_packetIntendBox->Y				= packet->Y;
		retVal = sizeof(stPACKET_SC_MOVE_STOP);
		break;
	}

	case dfPACKET_SC_ATTACK1:
	{
		stPACKET_SC_ATTACK1* packet	= (stPACKET_SC_ATTACK1*)cursor;
		out_packetIntendBox->PacketType = dfPACKET_SC_ATTACK1;
		out_packetIntendBox->ID			= packet->ID;
		out_packetIntendBox->Direction	= packet->Direction;
		out_packetIntendBox->X			= packet->X;
		out_packetIntendBox->Y			= packet->Y;
		retVal = sizeof(stPACKET_SC_ATTACK1);
		break;
	}

	case dfPACKET_SC_ATTACK2:
	{
		stPACKET_SC_ATTACK2* packet	= (stPACKET_SC_ATTACK2*)cursor;
		out_packetIntendBox->PacketType = dfPACKET_SC_ATTACK2;
		out_packetIntendBox->ID			= packet->ID;
		out_packetIntendBox->Direction	= packet->Direction;
		out_packetIntendBox->X			= packet->X;
		out_packetIntendBox->Y			= packet->Y;
		retVal = sizeof(stPACKET_SC_ATTACK2);
		break;
	}

	case dfPACKET_SC_ATTACK3:
	{
		stPACKET_SC_ATTACK3* packet	= (stPACKET_SC_ATTACK3*)cursor;
		out_packetIntendBox->PacketType = dfPACKET_SC_ATTACK3;
		out_packetIntendBox->ID			= packet->ID;
		out_packetIntendBox->Direction	= packet->Direction;
		out_packetIntendBox->X			= packet->X;
		out_packetIntendBox->Y			= packet->Y;
		retVal = sizeof(stPACKET_SC_ATTACK3);
		break;
	}

	case dfPACKET_SC_DAMAGE:
	{
		stPACKET_SC_DAMAGE* packet		= (stPACKET_SC_DAMAGE*)cursor;
		out_packetIntendBox->PacketType	= dfPACKET_SC_DAMAGE;
		out_packetIntendBox->DamageID	= packet->DamageID;
		out_packetIntendBox->ID				= packet->AttackID;
		out_packetIntendBox->HP				= packet->DamageHP;
		retVal = sizeof(stPACKET_SC_DAMAGE);
		break;
	}

	default:
		retVal = -1;
		break;
	}
	return retVal;
}