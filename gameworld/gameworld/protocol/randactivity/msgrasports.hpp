#ifndef __MSG_RA_SPORTS_HPP__
#define __MSG_RA_SPORTS_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/rasportsparam.hpp"

//////////////////////////////////////////// 2079  ����  ��������������  //////////////////////////////////////////

#pragma pack(push, 4)

namespace Protocol
{
	// ��Э��  3000
	enum RA_SPORTS_OPERA_TYPE
	{
		RA_SPORTS_OPERA_TYPE_INFO = 0,				//��ȡ��Ϣ	
		RA_SPORTS_OPERA_TYPE_GET_REWARD = 1,		//��ȡ���� param_1 : �������
	};


	class SCSportsHistroyInfo			// �·�������·� 
	{
	public:
		SCSportsHistroyInfo();
		MessageHeader header;

		short ra_sports_histroy_top;				//-1��δ�ϰ�,����
		unsigned short ra_sports_histroy_reward;	//�����ƣ��Ƿ��콱
	};
}

#pragma pack(pop)

#endif  // __MSG_RA_HAPPY_UP_HPP__


