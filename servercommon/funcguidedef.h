#ifndef __FUNC_GUIDE_DEF_H__
#define __FUNC_GUIDE_DEF_H__

#include "servercommon/bitmapfunction.hpp"

static const int FUNC_GUIDE_FLAG_MAX = 64;		// ���������¼������ֵ
static const int PET_DEAD_SUMMON_GUIDE_RECORD_DAYS = 8;

enum FuncGuideFlagType
{
	FUNC_GUIDE_NEWBIE_PET_CHOICE = 0,			// Ѱ�����ֳ���2
	FUNC_GUIDE_CHALLENGE_COLOSSEUM = 1,			// ������ս���񱦿�
	FUNC_GUIDE_NEWBIE_CATCH_PET = 2,			// ����ץ��ָ��
	FUNC_GUIDE_NEWBIE_DRAW_PET_2 = 3,			// Ѱ�����ֳ���2
	FUNC_GUIDE_CLOUD_ARENA = 4,					// �ƶ˾���
	FUNC_GUIDE_LUNHUI_WANGCHUAN = 5,			// �ֻ�����
	FUNC_GUIDE_NEWBIE_DRAW_PARTNER = 6,			// Ѱ�����ָ��
	FUNC_GUIDE_NEWBIE_DRAW_PARTNER_2 = 7,		// Ѱ�����ָ��2
	FUNC_GUIDE_FAST_DREAM_NOTES_CHALLENGE = 8,	// ɨ����Ԩ��¼ָ������(��������)
	FUNC_GUIDE_PASS_LUN_HUI_WANG_CHUAN = 9,		// ͨ���ֻ�����ָ������(��������)

	FUNC_GUILD_TYPE_MAX
};
UNSTD_STATIC_CHECK(FUNC_GUILD_TYPE_MAX <= FUNC_GUIDE_FLAG_MAX);

#pragma pack(push, 4)

struct FuncGuideParam
{
	FuncGuideParam() { this->Reset(); }

	void Reset()
	{
		guide_flag.Reset();
		choose_pet_seq = 0;
		reserve_sh = 0;
		memset(pet_dead_summon_guide_times_list, 0, sizeof(pet_dead_summon_guide_times_list));
	}

	BitMap<FUNC_GUIDE_FLAG_MAX> guide_flag;		// �����¼���������б�

	short choose_pet_seq;
	short reserve_sh;
	short pet_dead_summon_guide_times_list[PET_DEAD_SUMMON_GUIDE_RECORD_DAYS];
};
typedef char FuncGuideParamHex[sizeof(FuncGuideParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(FuncGuideParamHex) < 256);

#pragma pack(pop)
#endif