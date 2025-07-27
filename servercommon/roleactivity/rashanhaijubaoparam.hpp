#ifndef __RA_SHAN_HAI_JU_BAO_PARAM_HPP__
#define __RA_SHAN_HAI_JU_BAO_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//ɽ���۱�
//RAND_ACTIVITY_TYPE_SHAN_HAI_JU_BAO
//RAShanHaiJuBaoParam

//���⽱�����׶���
static const int SHAN_HAI_JU_BAO_EXTRA_PHASE_MAX = 3;

//���⽱�������
static const int SHAN_HAI_JU_BAO_EXTRA_REWARD_MAX = 4;


#pragma pack(push, 4)

//////////////////  ��������  /////////////////////////////////
struct RAShanHaiJuBaoParam
{
	RAShanHaiJuBaoParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		hold_ll = 0;

		total_draw_times = 0;

		memset(phase_info_, 0, sizeof(phase_info_));
	}
	unsigned int ra_begin_timestamp;

	long long hold_ll;

	int total_draw_times;	//�ܳ�ȡ����

	//��׶���Ϣ
	struct PhaseInfo
	{
		PhaseInfo()
		{
			extra_reward_flag = 0;
			draw_times = 0;

			hold_int = 0;
		}
		int extra_reward_flag;	//���⽱����ȡ���
		int draw_times;	//��ȡ����

		int hold_int;
	};

	//�ֽ׶���Ϣ[��׶� - 1]
	PhaseInfo phase_info_[SHAN_HAI_JU_BAO_EXTRA_PHASE_MAX];
};

UNSTD_STATIC_CHECK(sizeof(RAShanHaiJuBaoParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_SHAN_HAI_JU_BAO_PARAM_HPP__
