#ifndef __MSG_RA_LUCKY_TURN_TABLE_HPP__
#define __MSG_RA_LUCKY_TURN_TABLE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/raluckyturntableparam.hpp"

//////////////////////////////////////////// 2076  ����  ����ת��  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_LUCKY_TURNTABLE_REQ_TYPE
	{
		RA_LUCKY_TURNTABLE_REQ_TYPE_INFO = 0,			//������Ϣ�·�	
		RA_LUCKY_TURNTABLE_REQ_TYPE_START = 1,			//����ת������(��ȡ����)
		RA_LUCKY_TURNTABLE_REQ_TYPE_GIVE = 2,			//����ת�̽���(�ȴ��ͻ���ת�̽����ص�) param_1 - seq
		RA_LUCKY_TURNTABLE_REQ_TYPE_SKIP = 3,			//����ת����ȡ����ֱ�Ӹ������(��������)
	};

	struct SCRALuckyTurntableInfo			//!< 3032 ת����Ϣ�·�
	{
		SCRALuckyTurntableInfo();
		MessageHeader header;

		int last_num;											//ʣ�����
		BitMap<MAX_RA_LUCKY_TURNTABLE_TYPE_NUM> flag;			//��������������ȡ��ʶ
		char schedule[MAX_RA_LUCKY_TURNTABLE_TYPE_NUM];			//������������Ľ���
	};

	struct SCRALuckyTurntableReward			//!< 3033 ����ת�̽���
	{
		SCRALuckyTurntableReward();
		MessageHeader header;

		enum LUCKY_TURN_TABLE_TYPE
		{
			LUCKY_TURN_TABLE_TYPE_START = 0,			//�������Ŷ���ת���·�
			LUCKY_TURN_TABLE_TYPE_SKIP = 1,				//��������
		};

		short index;							//��ȡ����index
		short type;	
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_TURN_TABLE_HPP__



