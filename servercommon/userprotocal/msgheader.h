#ifndef __MSGHEADER_H__
#define __MSGHEADER_H__

#include "servercommon/basedef.h"
#include "servercommon/servercommon.h"
#include "servercommon/msgserialize.hpp"

#pragma pack(push, 4)

namespace Protocol
{
	enum
	{
		MT_INVALID = 0,
	};

	struct PtcItemInfo		//!< Э���������1 - ��������Ϣ - ��С 4 �ֽ�
	{
		PtcItemInfo(ItemID _item_id = 0, UInt16 _item_num = 0) : item_id(_item_id), item_num(_item_num) {}

		ItemID item_id;		//!< ����ID		- unsigned short
		UInt16 item_num;	//!< ��������	- unsigned short
	};

	struct PtcItemCon		//!< Э���������2 - Я������Ϣ - ��С 8 �ֽ�
	{
		PtcItemCon(ItemID _item_id = 0, short _is_bind = 0, int _item_num = 0) : item_id(_item_id), is_bind(_is_bind), item_num(_item_num) {}

		ItemID item_id;
		short is_bind;
		int item_num;
	};

	class MessageHeader
	{
	public:
		MessageHeader() : msg_type(MT_INVALID) {}
		MessageHeader(Int32 type) : msg_type(type), server_id(-1) {}
		bool Serialize(TLVSerializer2 * outstream) const { return outstream->Push(msg_type) && outstream->Push(server_id); }
		bool Unserialize(TLVUnserializer2 & instream) { return  instream.Pop(&msg_type) && instream.Pop(&server_id); }

		unsigned short msg_type;
		unsigned short server_id;
	};

	bool WriteBegin(unsigned short msg_type, char* buffer, int max_len, int* out_len);
	bool ReadBegin(unsigned short& msg_type, const char* buffer, int max_len, int* out_len);
}

#pragma pack(pop)
#endif // __MSGHEADER_H__