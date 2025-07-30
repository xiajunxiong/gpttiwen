#ifndef __MSG_FIRST_KILL_H__
#define __MSG_FIRST_KILL_H__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/struct/global/serverfirstkilldef.h"

#pragma pack(push, 4)

namespace Protocol
{
	struct CSServerFirstKillReq    // 2127 全服首杀  信息请求
	{
	public:
		CSServerFirstKillReq();
		MessageHeader header;

		int type;					//玩法类型
		int level_group;			//等级组
	};

	struct SCServerFirstKillInfo : public IMessageSerializer   // 2128 全服首杀 信息下发
	{
	public:
		SCServerFirstKillInfo();
		virtual ~SCServerFirstKillInfo() {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		struct FirstKillData
		{
			short member_count;							//参与击杀玩家的人数
			short type;									//玩法类型
			short level_group;							//等级组
			short seq;									//序号
			short round_num;							//回合数
			int boss_id;								//boss_id
			long long kill_timestamp;					//时间戳
			GameName member_name[MEMBER_NAME_MAX_NUM];	//参与击杀的玩家的名字  MEMBER_NAME_MAX_NUM---5
		};
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WriteBegin(header.msg_type, buffer, buffer_size, out_length);

			result = result && WriteInt(count, buffer, buffer_size, out_length);
			
			for (int i = 0; i < TYPE_LEVEL_GROUP_SEQ_MAX_NUM && i < count; i++)
			{
				result = result && WriteShort(data[i].member_count, buffer, buffer_size, out_length);
				result = result && WriteShort(data[i].type, buffer, buffer_size, out_length);
				result = result && WriteShort(data[i].level_group, buffer, buffer_size, out_length);
				result = result && WriteShort(data[i].seq, buffer, buffer_size, out_length);
				result = result && WriteShort(data[i].round_num, buffer, buffer_size, out_length);
				result = result && WriteInt(data[i].boss_id, buffer, buffer_size, out_length);
				result = result && WriteLL(data[i].kill_timestamp, buffer, buffer_size, out_length);
				for (int j = 0; j < data[i].member_count; j++)
				{
					result = result && WriteStrN((const char *)data[i].member_name[j], sizeof(GameName), buffer, buffer_size, out_length);
				}
			}
			return result;
		}


		int count;
		FirstKillData data[TYPE_LEVEL_GROUP_SEQ_MAX_NUM];			//TYPE_LEVEL_GROUP_SEQ_MAX_NUM --30 seq最高30
	};


}

#pragma pack(pop)

#endif