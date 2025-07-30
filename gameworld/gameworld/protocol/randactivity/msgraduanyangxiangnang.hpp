#ifndef __MSG_RA_DUAN_YANG_XIANG_NANG_HPP__
#define __MSG_RA_DUAN_YANG_XIANG_NANG_HPP__

#include "servercommon/userprotocal/msgheader.h"
#include "servercommon/roleactivity/randactivityroleparam.hpp"

#include "servercommon/roleactivity/raduanyangxiangnangparam.hpp"

#pragma  pack(push, 4)

namespace Protocol
{

	////////////////////////////////  ��������  /////////////////////////////////////
	struct SCRADuanYangXiangNang		//<!3183 ����-��������
	{
	public:
		SCRADuanYangXiangNang();
		MessageHeader header;

		BitMap<DUAN_WU_ACTIVITY_MAX_BITMAP_NUM> today_task_finish;		// ÿ�������Ƿ����
		BitMap<DUAN_WU_ACTIVITY_MAX_BITMAP_NUM> is_receive_reward;		// ÿ���������Ƿ�����ȡ
		int today_task_count[MAX_TASK_NUM_DUAN_YANG_XIANG_NANG];		// ÿ���������
	};


	////////////////////////////////  �����͸�  /////////////////////////////////////
	struct SCRACangLongCiFuInfo		//<!3184 ����-�����͸�
	{
	public:
		SCRACangLongCiFuInfo();
		MessageHeader header;

		int all_light_reward_flag;		// ȫ������������ʶ, 0--������ȡ, 1--����ȡ, 2--����ȡ
		BitMap<CLCF_BITMAP_MAX_VALUE> light_up_flag;
	};

	////////////////////////////////  ��������  /////////////////////////////////////
	struct SCRALanTangChuMuInfo		//<!3186 ����-��������
	{
	public:
		SCRALanTangChuMuInfo();
		MessageHeader header;

		BitMap<DUAN_WU_ACTIVITY_MAX_BITMAP_NUM> is_receive_reward;	// ÿ���������Ƿ�����ȡ
		int today_task_count[MAX_TASK_NUM_LAN_TANG_CHU_MU];			// ÿ���������

		int last_reward_flag;		// ���ս���, 0--������ȡ, 1--����ȡ, 2--����ȡ
		short add_herbs_num[MAX_HERBS_TYPE_LAN_TANG_CHU_MU];	// �Ѽ���ҩ������
	};

}

#pragma  pack(pop)

#endif
