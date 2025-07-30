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
	class SCStartMatch			//!< 3230 ��ʼƥ��֪ͨ
	{
	public:
		SCStartMatch();
		MessageHeader header;
		
		short business_type;	//!< ҵ������
		short user_num;			//!< �����
		MsgMatchUserInfo user_list[MSG_MATCH_USER_NUM];	//!< �µ�ƥ���б�
	};

	class SCMatchInfoUpdate		//!< 3231 ƥ����Ϣ���£��������ѻ�����뿪ƥ��ʱ��
	{
	public:
		SCMatchInfoUpdate();
		MessageHeader header;

		short business_type;							//!< ҵ������
		short user_num;									//!< �����
		MsgMatchUserInfo user_list[MSG_MATCH_USER_NUM]; //!< �µ�ƥ���б�
	};

	class SCStopMatch			//!< 3232 ���ֹͣƥ��֪ͨ
	{
	public:
		SCStopMatch();
		MessageHeader header;

		short business_type;	//!< ҵ������
		short reserve_sh;		//!< ������
	};

	class SCMatchEnemy : public IMessageSerializer	//!< 3233 ƥ�䵽����֪ͨ
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

		MsgMatchUserInfo left_user_list[MSG_MATCH_USER_NUM]; //!< ��߶����б�
		MsgMatchUserInfo right_user_list[MSG_MATCH_USER_NUM]; //!< �ұ߶����б�
	};
}

#pragma pack(pop)

#endif