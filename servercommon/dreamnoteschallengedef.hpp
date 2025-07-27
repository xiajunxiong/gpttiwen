#ifndef __DREAM_NOTES_CHALLENGE_DEF_HPP__
#define __DREAM_NOTES_CHALLENGE_DEF_HPP__

#include "servercommon/servercommon.h"

#pragma pack(push, 4)

static const int MAX_DREAM_NOTES_REISSUE_MAIL_NUM = 7;			// ��Ԩ��¼����δ��½�����ʼ������ֵ

struct DreamNotesChallengeTicketData
{
	DreamNotesChallengeTicketData() { this->Reset(); }

	void Reset()
	{
		has_give_ticket = 0;
		has_get_boss_reward = 0;
		consume_energy = 0;
		reserve_int = 0;
	}

	void ResetTicketInfo()
	{
		has_give_ticket = 0;
		has_get_boss_reward = 0;
		consume_energy = 0;
		reserve_int = 0;
	}

	char has_give_ticket;		//!< �Ƿ������Ʊ
	char has_get_boss_reward;	//!< �Ƿ�����Boss����
	short consume_energy;		//!< ���ĵ�������
	int reserve_int;	
};


struct DreamNotesChallengeParam
{
	DreamNotesChallengeParam() { this->Reset(); }

	void Reset()
	{
		pass_level = 0;
		remaining_times = 0;
		pass_new_level_timestamp = 0;
		is_set_times = 0;
		reserve_ch = 0;
		buy_times = 0;
		use_item_add_times = 0;
		reserve_sh = 0;
		ticket_data.Reset();
	}

	short pass_level;							// ��ǰͨ�ع�����߹ؿ�
	short remaining_times;						// ����ʣ�����
	unsigned int pass_new_level_timestamp;		// ͨ���¹ؿ���ʱ���
	char is_set_times;							// 0:δ���� 1:������
	char reserve_ch;
	short buy_times;							// �����ɨ������
	short use_item_add_times;					// ʹ�õ������ӵĴ���
	short reserve_sh;
	DreamNotesChallengeTicketData ticket_data;	
};

typedef char DreamNotesChallengeParamHex[sizeof(DreamNotesChallengeParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(DreamNotesChallengeParamHex) < 256);

#pragma pack(pop)

#endif