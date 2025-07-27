#ifndef __MIRAGE_TERRITORY_DEF_H__
#define __MIRAGE_TERRITORY_DEF_H__

#include "servercommon/bitmapfunction.hpp"
#include "servercommon/activitydef.hpp"
#include "servercommon/protobuf/role_mirageterritory_data.pb.h"

#pragma pack(push, 4)
const static int MAX_MIRAGE_TERRITORY_PROGRAMME_NUM = 4;
const static int MAX_MIRAGE_TERRITORY_PARTNER_NUM = 3;//最多参战3个伙伴
const static int MAX_MIRAGE_TERRITORY_BATTLE_ROUND_NUM = 10;//战斗回合数限制
UNSTD_STATIC_CHECK(MAX_MIRAGE_TERRITORY_PROGRAMME_NUM == 4);//改了需要看看数据库存下的后台查询字段
struct MirageTerritoryPartnerBattleInfo
{
	MirageTerritoryPartnerBattleInfo() { this->Reset(); }
	void Reset()
	{
		top_seq = 0;
		now_seq = 0;
		partner_num = 0;
		memset(partner_id_list, 0, sizeof(partner_id_list));
		memset(partner_posi_list, 0, sizeof(partner_posi_list));
	}
	int top_seq;
	int now_seq;
	int partner_num;
	int partner_id_list[MAX_MIRAGE_TERRITORY_PARTNER_NUM];
	int partner_posi_list[MAX_MIRAGE_TERRITORY_PARTNER_NUM];
};

struct MirageTerritoryParam
{
	MirageTerritoryParam() { this->Reset(); }
	void Reset()
	{
		for (int i = 0; i < MAX_MIRAGE_TERRITORY_PROGRAMME_NUM; i++)
		{
			partner_list[i].Reset();
		}
		today_pass_times = 0;
		pass_area = 0;
		pass_seq = 0;
	}
	void OnDayChange()
	{
		today_pass_times = 0;
	}
	MirageTerritoryPartnerBattleInfo partner_list[MAX_MIRAGE_TERRITORY_PROGRAMME_NUM];

	int today_pass_times;
	int pass_area;
	int pass_seq;
};

typedef char MirageTerritoryParamHex[sizeof(MirageTerritoryParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MirageTerritoryParamHex) < 512);

struct MirageTerritoryPartnerInfo
{
	MirageTerritoryPartnerInfo() { this->Reset(); }
	void Reset()
	{
		top_seq = 0;
		now_seq = 0;
	}
	int top_seq;
	int now_seq;
};

struct MirageTerritoryInfoParam
{
	MirageTerritoryInfoParam() { this->Reset(); }
	void Reset()
	{
		for (int i = 0; i < MAX_MIRAGE_TERRITORY_PROGRAMME_NUM; i++)
		{
			partner_list[i].Reset();
		}
		today_pass_times = 0;
		pass_area = 0;
		pass_seq = 0;
	}

	bool GetPBData(ARG_OUT PB_RoleMirageTerritoryInfoData_MirageTerritoryAllInfo* out_data) const
	{
		out_data->set_today_pass_times(today_pass_times);
		out_data->set_pass_area(pass_area);
		out_data->set_pass_seq(pass_seq);

		for (int i = 0; i < MAX_MIRAGE_TERRITORY_PROGRAMME_NUM; i++)
		{
			PB_RoleMirageTerritoryInfoData_MirageTerritoryInfo* info = out_data->add_partner_list();
			info->set_now_seq(partner_list[i].now_seq);
			info->set_top_seq(partner_list[i].top_seq);
		}

		return true;
	}

	int today_pass_times;
	int pass_area;
	int pass_seq;
	MirageTerritoryPartnerInfo partner_list[MAX_MIRAGE_TERRITORY_PROGRAMME_NUM];
};
struct RoleMirageTerritoryPartnerInfoParam
{
	RoleMirageTerritoryPartnerInfoParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(mirage_territory_data, 0, sizeof(mirage_territory_data));
	}

	char mirage_territory_data[sizeof(MirageTerritoryInfoParam)];
};
typedef char MirageTerritoryInfoParamHex[sizeof(MirageTerritoryInfoParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MirageTerritoryInfoParamHex) < 512);
typedef char RoleMirageTerritoryPartnerInfoParamHex[sizeof(RoleMirageTerritoryPartnerInfoParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RoleMirageTerritoryPartnerInfoParamHex) < 512);

#pragma pack(pop)
#endif