#ifndef __PRESTIGE_DEF_HPP__
#define __PRESTIGE_DEF_HPP__

#include <string.h>
#include "servercommon/servercommon.h"

#pragma pack(push, 4)
static const int PRESTIGE_VILLAGE_MAX_NUM = 20;
static const int OLD_PRESTIGE_VILLAGE_GOODS_MAX_NUM = 1000;		//�����ɵ�
static const int NEW_PRESTIGE_VILLAGE_GOODS_MAX_NUM = 2000;		//�����鳤��

struct PrestigeVillageNode
{
	PrestigeVillageNode() { this->Reset(); }

	void Reset()
	{
		prestige = 0;
		level = 0;
		use_limit = 0;
		reserve_sh = 0;
		reserve_i = 0;
	}

	int prestige;
	unsigned char level;
	unsigned char use_limit;
	unsigned short reserve_sh;
	int reserve_i;
};
struct PrestigeParam
{
	PrestigeParam() { this->Reset(); }

	void Reset()
	{
		for (int i = 0; i < PRESTIGE_VILLAGE_MAX_NUM; i++)
		{
			prestige_village_list[i].Reset();
		}

		today_donate_count = 0;
		today_add_value = 0;

		memset(new_buy_count, 0, sizeof(new_buy_count));
		transfer_mark = 0;
		buy_count_version = 0;
		reserver_sh = 0;
	}
	// ������Ϣ
	PrestigeVillageNode prestige_village_list[20];

	// ��������
	int today_donate_count;			//!< ���վ����˶��ټ�
	int today_add_value;			//!< ���ջ���˶�������

	// �¡��޹�����
	unsigned short new_buy_count[OLD_PRESTIGE_VILLAGE_GOODS_MAX_NUM];
	char transfer_mark;
	char buy_count_version;		// ��¼new_buy_count�İ汾�ţ�������������ָ��index��buy_count
	short reserver_sh;
};
UNSTD_STATIC_CHECK(sizeof(PrestigeParam) == (unsigned int)2252);

struct NewPrestigeParam
{
	NewPrestigeParam() { this->Reset(); }

	void Reset()
	{
		transfer_mark = 0;
		buy_count_version = 0;
		reserver_sh = 0;
		memset(reserve_ll, 0, sizeof(reserve_ll));

		for (int i = 0; i < PRESTIGE_VILLAGE_MAX_NUM; i++)
		{
			prestige_village_list[i].Reset();
		}

		today_donate_count = 0;
		today_add_value = 0;

		memset(new_buy_count, 0, sizeof(new_buy_count));
	}

	char transfer_mark;
	char buy_count_version;		// ��¼new_buy_count�İ汾�ţ�������������ָ��index��buy_count
	short reserver_sh;
	long long reserve_ll[3];

	// ������Ϣ
	PrestigeVillageNode prestige_village_list[PRESTIGE_VILLAGE_MAX_NUM];

	// ��������
	int today_donate_count;			//!< ���վ����˶��ټ�
	int today_add_value;			//!< ���ջ���˶�������
	// �¡��޹�����
	unsigned short new_buy_count[NEW_PRESTIGE_VILLAGE_GOODS_MAX_NUM];
};

typedef char PrestigeParamHex[sizeof(NewPrestigeParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(PrestigeParamHex) < 10240);

#pragma pack(pop)

#endif  // __PRESTIGE_DEF_HPP__

