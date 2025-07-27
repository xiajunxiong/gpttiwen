#ifndef __TELNET_GAMEWORLD_PROTOCOL_H__
#define __TELNET_GAMEWORLD_PROTOCOL_H__

#include "servercommon/serverdef.h"

#pragma pack(push)
#pragma pack(4)

namespace telnetgameprotocol
{
	// GAME_TELNET  -> 游戏服发到TELNET
	// TELNET_GAME  -> TELNET发到游戏服
	enum _MsgType
	{
		MT_INVALID,

		MT_TELNET_GAME_PROTOCOL_BEGIN = MT_INVALID,
		// Telnet -> 游戏服  发送CMD指令
		MT_TELNET_GAME_CMD_REQ,
		// 游戏服 - > Telnet 
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

		int data_length;				// 发送的字符串长度 , 所有CMD都以字符串的形势发送
		// 紧接着CMD字符串的内容
	};

	struct GameTelnetCmdAck
	{
		GameTelnetCmdAck() : header(MT_GAME_TELNET_CMD_ACK) {}
		MessageHeader header;

		int data_length;				// 发送的字符串长度 , 所有CMD都以字符串的形势发送
		// 紧接着Game返回的字符串的内容
	};
}

#pragma pack(pop)

#endif	// __TELNET_GAMEWORLD_PROTOCOL_H__