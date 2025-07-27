#ifndef __RA_TU_JIAN_TREASURE_PARAM_HPP__
#define __RA_TU_JIAN_TREASURE_PARAM_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/roleactivitydef.hpp"

//////////////////////////////////////////// 2080 ����� - ͼ���ᱦ  //////////////////////////////////////////

enum TU_JIAN_TREASURE_TYPE						//ͼ���ᱦ����
{
	TU_JIAN_TREASURE_TYPE_BEGIN = 0,
	TU_JIAN_TREASURE_TYPE_WLID_BOSS,			//Ұ��boss
	TU_JIAN_TREASURE_TYPE_FB,					//����
	TU_JIAN_TREASURE_TYPE_MARKET,				//������

	TU_JIAN_TREASURE_TYPE_TYPE_MAX
};

static const int RA_TU_JIAN_TREASURE_MAXNUM = 36;					//ͼ���ᱦ���seq
static const int RA_TU_JIAN_TREASURE_LOGIN_MAXNUM = 32;				//ͼ���ᱦ����½����

struct TuJianTreasureItem
{
	TuJianTreasureItem() { this->Reset(); }
	void Reset()
	{
		param1 = -1;
		param2 = -1;
		count = 0;
		type = 0;
	}
	int param1;				//����1:Ұ��bossID	����2:����seq	����3:ͼ��ID(0������ͼ��)
	short param2;			//����1��2:����ͼ����ID (0������Ҫ����)	����3:�����в���������0|����1|���۹������2��
	char count;
	char type;				//TU_JIAN_TREASURE_TYPE
};

#pragma pack(push, 4)
////////////////////////////////////////////  ����������  //////////////////////////////////////////////////////


////////////////////////////////////////////  ��������  //////////////////////////////////////////////////////

struct RATuJianTreasureParam
{
	RATuJianTreasureParam() { this->Reset(); }
	void Reset()
	{
		ra_tu_jian_treasure_begin_timestamp = 0;
		ra_tu_jian_treasure_end_timestamp = 0;
		ra_tu_jian_treasure_login_day = 0;
		ra_tu_jian_treasure_fetch_flag.Reset();
		ra_tu_jian_treasure_day_fetch_flag.Reset();
		for (int i = 0; i < RA_TU_JIAN_TREASURE_MAXNUM; ++i)
		{
			ra_tu_jian_treasure_item[i].Reset();
		}
		ra_tu_jian_treasure_day_id = 0;
	}

	unsigned int ra_tu_jian_treasure_begin_timestamp;
	unsigned int ra_tu_jian_treasure_end_timestamp;												//ͼ���ᱦ����ʱ���
	int ra_tu_jian_treasure_login_day;
	BitMap<RA_TU_JIAN_TREASURE_LOGIN_MAXNUM> ra_tu_jian_treasure_day_fetch_flag;				//��½������ȡ��־λ
	BitMap<RA_TU_JIAN_TREASURE_MAXNUM> ra_tu_jian_treasure_fetch_flag;							//��ȡ��־λ
	TuJianTreasureItem	ra_tu_jian_treasure_item[RA_TU_JIAN_TREASURE_MAXNUM];

	unsigned int ra_tu_jian_treasure_day_id;
};

UNSTD_STATIC_CHECK(sizeof(RATuJianTreasureParam) <= sizeof(RoleActivityRoleData))

#pragma pack(pop)


#endif	//__RA_TU_JIAN_TREASURE_PARAM_HPP__
