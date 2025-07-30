#ifndef __MSG_RA_TU_JIAN_TREASURE_HPP__
#define __MSG_RA_TU_JIAN_TREASURE_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ratujiantreasureparam.hpp"

//////////////////////////////////////////// 2080  ����  ͼ���ᱦ  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_TU_JIAN_TREASURE_OPERA_TYPE
	{
		RA_TU_JIAN_TREASURE_OPERA_TYPE_INFO = 0,		//��Ϣ�·�	
		RA_TU_JIAN_TREASURE_OPERA_TYPE_FETCH_DAY = 1,	//��ȡ��������	param1:seq
		RA_TU_JIAN_TREASURE_OPERA_TYPE_FETCH = 2,		//��ȡ����	param1:seq
	};


	struct RATuJianTreasureInfo
	{
		RATuJianTreasureInfo() { this->Reset(); }
		void Reset()
		{
			seq = 0;
			is_fetch = 0;
			count = 0;
		}

		char seq;
		char is_fetch;						// 1:����ȡ 0:δ��ȡ
		short count;
	};

	struct SCRATuJianTreasureInfo			//!<��3038 ͼ���ᱦ�����·�
	{
	public:
		SCRATuJianTreasureInfo();
		MessageHeader header;

		unsigned int end_timestamp;										//��һ����ʱ��
		int login_day;
		BitMap<RA_TU_JIAN_TREASURE_LOGIN_MAXNUM> day_fetch_flag;		//������ȡ��־ RA_TU_JIAN_TREASURE_LOGIN_MAXNUM 32
		int count;
		RATuJianTreasureInfo info[RA_TU_JIAN_TREASURE_MAXNUM];
	};

}

#pragma pack(pop)

#endif  // __MSG_RA_TU_JIAN_TREASURE_HPP__


