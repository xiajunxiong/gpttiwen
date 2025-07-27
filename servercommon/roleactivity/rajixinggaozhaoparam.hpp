#ifndef __RA_JIXING_GAOZHAO_PARAM_HPP__
#define __RA_JIXING_GAOZHAO_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"


//////////////////////////////////////////// ���Ǹ��� //////////////////////////////////////////
static const int MAX_RA_JIXING_GAOZHAO_SHOP_INDEX = 16;				// �̳���������

struct JiXingGaoZhaoRecordInfo			// �齱��¼��Ϣ
{
	JiXingGaoZhaoRecordInfo() { this->Reset(); }
	void Reset()
	{
		memset(role_name, 0, sizeof(role_name));
		reward_type = -1;
		item_id = 0;
		num = 0;
	}

	GameName role_name;
	short reward_type;
	ItemID item_id;			//��ƷID
	int num;
};

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////
struct RandActivityJiXingGaoZhaoParam
{
	RandActivityJiXingGaoZhaoParam() { this->Reset(); }

	void Reset()
	{
		draw_times = 0;
		item_id = 0;
		jackpot_num = 0;

		for (int i = 0; i < 10; i++)
		{
			queue_list[i].Reset();
		}
	}
	
	short draw_times;
	unsigned short item_id;
	unsigned int jackpot_num;

	JiXingGaoZhaoRecordInfo queue_list[10];
};


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

const static int MAX_JI_XING_GAO_ZHAO_REWARD_GROUP_NUM = 64;//���Ǹ��յ�ʱ�����������������

struct RAJiXingGaoZhaoParam
{
	RAJiXingGaoZhaoParam() { this->Reset(); }

	void Reset()
	{
		ra_begin_timestamp = 0;
		draw_times = 0;
		draw_reward_fetch_mark = 0;
		memset(shop_list, 0, sizeof(shop_list));

		rand_times = 0;
		floor_bitmap.Reset();
	}

	unsigned int ra_begin_timestamp;
	int draw_times;
	int draw_reward_fetch_mark;

	short shop_list[MAX_RA_JIXING_GAOZHAO_SHOP_INDEX];

	int rand_times;
	BitMap<MAX_JI_XING_GAO_ZHAO_REWARD_GROUP_NUM> floor_bitmap;
};

UNSTD_STATIC_CHECK(sizeof(RAJiXingGaoZhaoParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)
#endif	//__RA_JIXING_GAOZHAO_PARAM_HPP__
