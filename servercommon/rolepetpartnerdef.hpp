#ifndef __ROLE_PET_PARTNER_DEF_HPP__
#define __ROLE_PET_PARTNER_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"
#include "gameworld/gameworld/gamedef.h"
#include "servercommon/servercommon.h"
#include "servercommon/msgserialize.hpp"
#include "servercommon/protobuf/role_pet_partner_data.pb.h"

#pragma pack(push, 4)

struct PetPrintItem
{
	unsigned short item_id;
	short level;
};

class PB_RolePetInfoData_PetData;
struct RolePetInfoData
{
	RolePetInfoData() :item_unique_id(0), item_id(0), str_level(0), soul_item_id(0), soul_level(0), capability(0)
	{
		memset(skill_id_list, 0, sizeof(skill_id_list));
		memset(zhufu_pet_item_id, 0, sizeof(zhufu_pet_item_id));
		memset(god_print_item, 0, sizeof(god_print_item));
	}

	void Reset()
	{
		item_unique_id = 0;
		item_id = 0;
		str_level = 0;
		memset(skill_id_list, 0, sizeof(skill_id_list));
		soul_item_id = 0;
		soul_level = 0;	
		memset(zhufu_pet_item_id, 0, sizeof(zhufu_pet_item_id));
		memset(god_print_item, 0, sizeof(god_print_item));
		capability = 0;
	}

	bool GetPBData(ARG_OUT PB_RolePetInfoData_PetData* out_data) const
	{
		out_data->set_item_unique_id(item_unique_id);
		out_data->set_item_id(item_id);
		out_data->set_str_level(str_level);
		
		out_data->mutable_skill_id_list()->Clear();
		for (int i = 0; i < ARRAY_ITEM_COUNT(skill_id_list); i++)
		{
			if (skill_id_list[i] > 0) out_data->add_skill_id_list(skill_id_list[i]);
		}

		if (soul_item_id > 0)
		{
			out_data->set_soul_item_id(soul_item_id);
			out_data->set_soul_level(soul_level);
		}

		for (int i = 0; i < ARRAY_ITEM_COUNT(zhufu_pet_item_id); i++)
		{
			if (zhufu_pet_item_id[i] > 0) out_data->add_zhufu_pet_item_id(zhufu_pet_item_id[i]);
		}

		for (int i = 0; i < ARRAY_ITEM_COUNT(god_print_item); i++)
		{
			if (god_print_item[i].item_id > 0)
			{
				PB_RolePetInfoData_PetPrintItem* pet_print_item = out_data->add_god_print_item();
				pet_print_item->set_item_id(god_print_item[i].item_id);
				pet_print_item->set_level(god_print_item[i].level);
			}
		}

		out_data->set_capability(capability);

		return true;
	}

	long long item_unique_id;
 	unsigned short item_id;
 	unsigned short str_level;
 	unsigned short skill_id_list[10];
 	int soul_item_id;
 	short soul_level;
 	unsigned short zhufu_pet_item_id[3];
 	PetPrintItem god_print_item[5];
	int capability;
};

struct RolePetInfoParam
{
	RolePetInfoParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(pet_data, 0, sizeof(pet_data));
	}

	char pet_data[sizeof(RolePetInfoData) * 100];
};

class PB_RolePartnerInfoData_PartnerData;
struct RolePartnerInfoData
{
	RolePartnerInfoData() :partner_id(0), level(0), upgrade_lv(0), quality(0), quality_segment(0), capability(0)
	{
		memset(pact_id_list, 0, sizeof(pact_id_list));
		memset(equip_list, 0, sizeof(equip_list));
	}

	void Reset()
	{
		partner_id = 0;
		level = 0;
		upgrade_lv = 0;
		quality = 0;
		quality_segment = 0;
		memset(pact_id_list, 0, sizeof(pact_id_list));
		memset(equip_list, 0, sizeof(equip_list));
		capability = 0;
	}

	bool GetPBData(ARG_OUT PB_RolePartnerInfoData_PartnerData* out_data) const
	{
		out_data->set_partner_id(partner_id);
		out_data->set_level(level);
		out_data->set_upgrade_lv(upgrade_lv);
		out_data->set_quality(quality);
		out_data->set_quality_segment(quality_segment);

		for (int i = 0; i < 3; i++)
		{
			if (pact_id_list[i] > 0) out_data->add_pact_id_list(pact_id_list[i]);
		}

		for (int i = 0; i < 3; i++)
		{
			if (equip_list[i] > 0) out_data->add_equip_list(equip_list[i]);
		}

		out_data->set_capability(capability);

		return true;
	}

	unsigned short partner_id;
	short level;
	short upgrade_lv;
	char quality;
	char quality_segment;
	unsigned short pact_id_list[3];
	unsigned short equip_list[3];
	int capability;
};

struct RolePartnerInfoParam
{
	RolePartnerInfoParam()
	{
		this->Reset();
	}

	void Reset()
	{
		memset(partner_data, 0, sizeof(partner_data));
	}

	char partner_data[sizeof(RolePartnerInfoData) * PARTNER_NUM_MAX];
};

typedef char RolePetInfoDataHex[sizeof(RolePetInfoParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RolePetInfoDataHex) < 15000);

typedef char RolePartnerInfoDataHex[sizeof(RolePartnerInfoParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(RolePartnerInfoDataHex) < 4000);
#pragma pack(pop)

#endif // !__ROLE_PET_PARTNER_DEF_HPP__
