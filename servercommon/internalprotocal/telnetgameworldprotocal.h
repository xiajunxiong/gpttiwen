#ifndef __TELNET_GAMEWORLD_PROTOCOL_H__
#define __TELNET_GAMEWORLD_PROTOCOL_H__

#include "servercommon/serverdef.h"

#pragma pack(push)
#pragma pack(4)

namespace telnetgameprotocol
{
	// GAME_TELNET  -> ��Ϸ������TELNET
	// TELNET_GAME  -> TELNET������Ϸ��
	enum _MsgType
	{
		MT_INVALID,

		MT_TELNET_GAME_PROTOCOL_BEGIN = MT_INVALID,
		// Telnet -> ��Ϸ��  ����CMDָ��
		MT_TELNET_GAME_CMD_REQ,
		// ��Ϸ�� - > Telnet 
		MT_GAME_TELNET_CMD_ACK,

		MT_TELNET_GAME_MAX,
	};

	struct MessageHeader
	{
		explicit MessageHeader() : msg_type(MT_INVALID) {}
		explicit MessageHeader(int _msg_type) : msg_type(_msg_type) {}

		int msg_type;
	};

	struct TelnetGameCmdReq
	{
		TelnetGameCmdReq() : header(MT_TELNET_GAME_CMD_REQ) {}
		MessageHeader header;

		int data_length;				// ���͵��ַ������� , ����CMD�����ַ��������Ʒ���
		// ������CMD�ַ���������
	};

	struct GameTelnetCmdAck
	{
		GameTelnetCmdAck() : header(MT_GAME_TELNET_CMD_ACK) {}
		MessageHeader header;

		int data_length;				// ���͵��ַ������� , ����CMD�����ַ��������Ʒ���
		// ������Game���ص��ַ���������
	};
}

#pragma pack(pop)

#endif	// __TELNET_GAMEWORLD_PROTOCOL_H__