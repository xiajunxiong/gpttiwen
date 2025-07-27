#ifndef __ROLE_CRYSTAL_FB_DEF_H__
#define __ROLE_CRYSTAL_FB_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/struct/global/drawdef.hpp"

#pragma pack(push, 4)

const static int CRYSTAL_FB_MAX_LAYER = 6;

const static int CRYSTAL_FB_XI_YOU_MAX_LEVEL = 52;		//ϡ�и���Ҫ��¼����������Ҫ�����ݿ�
const static int CRYSTAL_FB_XI_LIMIT_ITEM_MAX = 100;		//���������Ʒ����
const static int CRYSTAL_FB_FIRST_FLAG = 5;				// ����¼ 32*5 - 1��������ͨ
UNSTD_STATIC_CHECK(CRYSTAL_FB_FIRST_FLAG >= (CRYSTAL_FB_XI_YOU_MAX_LEVEL / 32) + 1);//�������޸���Ҫ�Ķ����ݿ�

static const int CRYSTAL_FB_NORMAL_NUM = 30;
static const int CRYSTAL_FB_XI_YOU_NUM = 30;//ϡ�и���¼����

struct CrystalFBTicketData
{
	enum TICKET_TYPE
	{
		TICKET_TYPE_ENERGY = 1,
		TICKET_TYPE_ITEM = 2,
	};

	CrystalFBTicketData()
	{
		this->Reset();
	}

	void Reset()
	{
		has_give_ticket = 0;
		has_get_boss_reward = 0;
		ticket_type = 0;
		reserve_sh = 0;
		consume_energy = 0;
		consume_item_id = 0;
		consume_item_num = 0;
		consume_item_is_bind = 0;
	}

	void ResetTicketInfo()
	{
		has_give_ticket = 0;
		has_get_boss_reward = 0;
		ticket_type = 0;
		reserve_sh = 0;
		consume_energy = 0;
		consume_item_id = 0;
		consume_item_num = 0;
		consume_item_is_bind = 0;
	}

	char has_give_ticket;		//!< �Ƿ������Ʊ
	char has_get_boss_reward;	//!< �Ƿ�����Boss����
	char ticket_type;			//!< ��Ʊ���� 1:���� 2:��Ʒ
	char reserve_sh;			//!< ������
	short consume_energy;		//!< ���ĵ�������
	ItemID consume_item_id;		//!< ���ĵ���ƷID
	short consume_item_num;		//!< ���ĵ���Ʒ����
	short consume_item_is_bind;	//!< ���ĵ���Ʒ�Ƿ��
};

struct CrystalFBNormalRecord
{
	CrystalFBNormalRecord()
	{
		this->Reset();
	}

	void Reset()
	{
		fb_seq = -1;
		times = 0;
	}

	short fb_seq;
	short times;
};

struct RoleCrystalParam
{
	RoleCrystalParam()
	{
		this->Reset();
	}

	void Reset()
	{
		pu_tong_jing_ying_play_times = 0;
		memset(xi_you_fb_play_times, 0, sizeof(xi_you_fb_play_times));
		memset(frist_pass_flag, 0, sizeof(frist_pass_flag));

		role_enter_energy = 0;
		next_energy_reply_tamp = 0;
		frist_fun_open = 0;
		change_flag = 0;
		buy_energy_times = 0;

		for (int i = 0; i < CRYSTAL_FB_NORMAL_NUM; ++i)
		{
			normal_fb_record_list[i].Reset();
			normal_fb_buy_times_list[i].Reset();
		}

		new_jing_ying_play_times = 0;
		new_xi_you_play_times = 0;
		use_xi_you_play_times = 0;
		memset(frist_pass_reward, 0, sizeof(frist_pass_reward));
		memset(reserve_ll, 0, sizeof(reserve_ll));

		is_syn_xi_you_play_times = 0;
		ondaychange_flag = 0;
		reserve_sh = 0;
		get_xi_you_play_times = 0;
		last_full_energy_timestamp = 0;
		overflow_energy = 0;
	}

	void DayReset()
	{
		pu_tong_jing_ying_play_times = 0;

		for (int i = 0; i < ARRAY_ITEM_COUNT(xi_you_fb_play_times); ++i)
		{
			xi_you_fb_play_times[i] = 0;
		}

		for (int i = 0; i < CRYSTAL_FB_NORMAL_NUM; ++i)
		{
			normal_fb_record_list[i].Reset();
			normal_fb_buy_times_list[i].Reset();
		}

		new_jing_ying_play_times = 0;
		new_xi_you_play_times = 0;
		use_xi_you_play_times = 0;
		get_xi_you_play_times = 0;
		ondaychange_flag = 0;
	}

	int pu_tong_jing_ying_play_times;
	unsigned char xi_you_fb_play_times[CRYSTAL_FB_XI_YOU_MAX_LEVEL];
	unsigned int frist_pass_flag[CRYSTAL_FB_FIRST_FLAG];

	int role_enter_energy;
	unsigned int next_energy_reply_tamp;
	int frist_fun_open;
	short change_flag;
	short buy_energy_times;

	CrystalFBTicketData fb_ticket_data;

	CrystalFBNormalRecord normal_fb_record_list[CRYSTAL_FB_NORMAL_NUM];			//��ս����
	CrystalFBNormalRecord normal_fb_buy_times_list[CRYSTAL_FB_NORMAL_NUM];		//���⹺�����

	int new_jing_ying_play_times;
	int new_xi_you_play_times;				 //�ж��ٴ�
	int use_xi_you_play_times;				 //ʹ���˶��ٴ�

	unsigned int frist_pass_reward[CRYSTAL_FB_FIRST_FLAG];
	long long reserve_ll[2];

	unsigned int last_full_energy_timestamp;	// ������һ����������ʱ���
	int overflow_energy;						// �������

	char is_syn_xi_you_play_times;			//�Ϻ�ϡ�д���ͬ����ʶ 0:δͬ�� 1:��ͬ��
	char ondaychange_flag;
	short reserve_sh;
	int get_xi_you_play_times;				//ϡ�и��������ý�������(��Ӣ����)
};

struct DrawCrystalPoker
{
	DrawCrystalPoker() { this->Reset(); }

	bool Serialize(TLVSerializer2 & outstream) const ;
	void Reset()
	{
		index = -1;
		role_uid = 0;
		memset(role_name, 0, sizeof(role_name));
		reward_info.Reset();
	}

	int index;
	int role_uid;
	GameName role_name;
	ItemDrawInfo reward_info;
};


typedef char RoleCrystalParamHex[sizeof(RoleCrystalParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleCrystalParamHex) < 2048);

#pragma pack(pop)

#endif // !__SCHOOL_TASK_DEF_H__
