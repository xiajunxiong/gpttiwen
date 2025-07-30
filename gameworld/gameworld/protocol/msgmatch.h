#ifndef __MSG_MATCH_H__
#define __MSG_MATCH_H__

#include "servercommon/userprotocal/msgheader.h"
#include "gamedef.h"

#pragma pack(push) 
#pragma pack(4)

struct MsgMatchUserInfo
{
	MsgMatchUserInfo():uid(0), profession(0), level(0), avatar_type(0), headshot_id(0), reserve_sh(0)
	{
		memset(name, 0, sizeof(GameName));
	}

	int uid;
	int profession;
	short level;
	short avatar_type;
	short headshot_id;
	short reserve_sh;
	GameName name;
};

static const int MSG_MATCH_USER_NUM = 5;

namespace Protocol
{
	class SCStartMatch			//!< 3230 开始匹配通知
	{
	public:
		SCStartMatch();
		MessageHeader header;
		
		short business_type;	//!< 业务类型
		short user_num;			//!< 玩家数
		MsgMatchUserInfo user_list[MSG_MATCH_USER_NUM];	//!< 新的匹配列表
	};

	class SCMatchInfoUpdate		//!< 3231 匹配信息更新（新增队友或队友离开匹配时）
	{
	public:
		SCMatchInfoUpdate();
		MessageHeader header;

		short business_type;							//!< 业务类型
		short user_num;									//!< 玩家数
		MsgMatchUserInfo user_list[MSG_MATCH_USER_NUM]; //!< 新的匹配列表
	};

	class SCStopMatch			//!< 3232 玩家停止匹配通知
	{
	public:
		SCStopMatch();
		MessageHeader header;

		short business_type;	//!< 业务类型
		short reserve_sh;		//!< 保留字
	};

	class SCMatchEnemy : public IMessageSerializer	//!< 3233 匹配到对手通知
	{
	public:
		SCMatchEnemy();
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_SHORT(business_type)
				&& WRITE_SHORT(reserve_sh)
				&& WRITE_SHORT(left_user_num)
				&& WRITE_SHORT(right_user_num);
			if (!ret) return false;

			for (int i = 0; i < left_user_num; ++i)
			{
				ret = WRITE_INT(left_user_list[i].uid)
					&& WRITE_INT(left_user_list[i].profession)
					&& WRITE_SHORT(left_user_list[i].level)
					&& WRITE_SHORT(left_user_list[i].avatar_type)
					&& WRITE_SHORT(left_user_list[i].headshot_id)
					&& WRITE_SHORT(left_user_list[i].reserve_sh)
					&& WRITE_STRN(left_user_list[i].name, sizeof(GameName));
				if (!ret) return false;
			}

			for (int i = 0; i < right_user_num; ++i)
			{
				ret = WRITE_INT(right_user_list[i].uid)
					&& WRITE_INT(right_user_list[i].profession)
					&& WRITE_SHORT(right_user_list[i].level)
					&& WRITE_SHORT(right_user_list[i].avatar_type)
					&& WRITE_SHORT(right_user_list[i].headshot_id)
					&& WRITE_SHORT(right_user_list[i].reserve_sh)
					&& WRITE_STRN(right_user_list[i].name, sizeof(GameName));
				if (!ret) return false;
			}

			return true;
		}

		short business_type;
		short reserve_sh;
		short left_user_num;
		short right_user_num;

		MsgMatchUserInfo left_user_list[MSG_MATCH_USER_NUM]; //!< 左边队伍列表
		MsgMatchUserInfo right_user_list[MSG_MATCH_USER_NUM]; //!< 右边队伍列表
	};
}

#pragma pack(pop)

#endif