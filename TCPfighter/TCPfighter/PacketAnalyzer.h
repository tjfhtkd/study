#pragma once
class PacketAnalyzer
{
public:
	PacketAnalyzer();
	virtual ~PacketAnalyzer();

public:
	bool Encode(OUT char** out_packet
		, OUT INT* out_len
		, IN BYTE Direction
		, IN WORD X
		, IN WORD Y
		, IN INT type);

	INT Decode(IN char* in_packet, OUT stPacket_ArgCollectionBox* out_packetIntendBox);

};