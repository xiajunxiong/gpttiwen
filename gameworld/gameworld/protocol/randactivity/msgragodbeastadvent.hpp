#ifndef __MSG_RA_GOD_BEAST_ADVENT_HPP__
#define __MSG_RA_GOD_BEAST_ADVENT_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/ragodbeastadventparam.hpp"

#pragma pack(push, 4)

namespace Protocol
{

	class SCGodBeastAdventInfo				//!<3160 ����-��������������Ϣ
	{
	public:
		SCGodBeastAdventInfo();
		MessageHeader header;

		short zhigou_flag;			// �Ƿ����˻��Ʊ
		short reserve_sh;

		unsigned int cur_like;			// ��ǰ�øж�

		///////// ������� //////////
		BitMap<MAX_BITMAP_GOD_BEAST_ADVENT> today_task_finish;		// ÿ�������Ƿ����
		BitMap<MAX_BITMAP_GOD_BEAST_ADVENT> is_receive_reward;		// ÿ���������Ƿ�����ȡ
		int today_task_count[MAX_TASK_NUM_GOD_BEAST_ADVENT];		// ÿ���������

		///////// �һ���� /////////
		short buy_day_limit[MAX_REWARD_GROUP_GOD_BEAST_ADVENT];			// �޹����߶һ�����
	};

	class SCGodBeastAdventOpenInfo			//!<3162 ����-������������״̬��Ϣ
	{
	public:
		SCGodBeastAdventOpenInfo();
		MessageHeader header;

		int open_flag;		// �������
		int seq;			// ��������
	};

	class SCGodBeastAdventBuyInfo		//!<3163 ����-��������ȫ���޹�����������Ϣ
	{
	public:
		SCGodBeastAdventBuyInfo();
		MessageHeader header;

		int last_buy_limit;		// ʣ��ɶһ�����
	};

	class SCGodBeastAdventNotifyInfo			//3169 ����-���������øжȱ䶯֪ͨ
	{
	public:
		SCGodBeastAdventNotifyInfo();
		MessageHeader header;

		int add_like;
	};

}

#pragma pack(pop)

#endif