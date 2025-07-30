#ifndef __MSG_RA_CHONG_JI_FAN_LI_HPP__
#define __MSG_RA_CHONG_JI_FAN_LI_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rachongjifanliparam.hpp"

//////////////////////////////////////////// 2209  ����  �輼����  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	//3000Э��ת��
	enum RA_CHONG_JI_FAN_LI_REQ_TYPE
	{
		RA_CHONG_JI_FAN_LI_REQ_TYPE_INFO = 0,		// ������Ϣ
		RA_CHONG_JI_FAN_LI_REQ_TYPE_FETCH = 1,		// ��ȡ���� P1:seq
	};

	class SCRAChongJiFanLiInfo						//<! 3187 ����-�輼������Ϣ�·�
	{
	public:
		SCRAChongJiFanLiInfo();
		MessageHeader header;

		char is_day_meet_flag;			//�����Ƿ��Ѿ����������� 0:��δ 1:������
		char reserve_ch;
		short reserve_sh;
		int meet_cond_day_num;				//���������������
		int day_chongzhi_num;				//�����ֵ��Ԫ������
		BitMap<MAX_RA_CHONG_JI_FAN_LI_REWARD_NUM> reward_fetch_flag;  //������ȡ ��ʶ
	};
	
}

#pragma pack(pop)

#endif  // __MSG_RA_LUCKY_BAG_HPP__
