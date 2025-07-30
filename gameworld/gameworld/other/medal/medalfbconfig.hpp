#ifndef __MEDAL_FB_CONFIG_HPP__
#define __MEDAL_FB_CONFIG_HPP__

#include <vector>
#include <map>

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/medaldef.h"
#include "item/medalitem/medalitem.hpp"
#include "gamedef.h"

const int static MEDAL_FB_LAYER_MAX_MING_MONSTER_NUM = 2;			//勋章副本每层最多的明怪

struct  MedalFbLayerCfg
{
	MedalFbLayerCfg()
	{
		scene_id = 0;
		transport_pos = Posi(0, 0);
		birth_pos = Posi(0, 0);
		npc_id = 0;
		npc_pos = Posi(0, 0);
		npc_dir = 0;
		recover_num = 0;
	}

	struct MonsterInfoItem
	{
		MonsterInfoItem()
		{
			boss_npc_id = 0;
			boss_pos = Posi(0, 0);
			boss_dir = 0;
		}


		int boss_npc_id;
		Posi boss_pos;
		Dir boss_dir;
	};

	int scene_id;
	Posi transport_pos;
	Posi birth_pos;
	int npc_id;
	Posi npc_pos;
	Dir npc_dir;
	int recover_num;
	std::vector<MonsterInfoItem> monster_item;
};


struct  MedalFbLevelCfg
{
	MedalFbLevelCfg()
	{
		need_item_id = 0;
		need_item_num = 1;
		limit_role_level = 0;
		min_enter_role_num = 0;
		max_enter_role_num = 0;
	}

	int need_item_id;
	int need_item_num;
	int limit_role_level;
	int min_enter_role_num;
	int max_enter_role_num;
	std::vector<MedalFbLayerCfg> layer_cfg;
};

struct MedalMaterialItemCfg
{
	const int static NEED_COMSUME_ITEM_MAX_COUNT = 5;

	MedalMaterialItemCfg()
	{
	}

	struct ComsumeItem
	{
		ComsumeItem()
		{
			item_id = 0;
			num = 0;
		}
		int item_id;
		int num;
	};

	ItemConfigData target_item;
	std::vector<ComsumeItem> need_item_list;
};

// -----------------------------------------------------------------------------------
class MedalFbConfig : public ILogicConfig
{
public:
	MedalFbConfig();
	virtual ~MedalFbConfig();

	virtual bool Init(const std::string &configname, std::string *err);


	const MedalFbLevelCfg * GetMedalFbLevelCfg(int fb_seq);
	const MedalFbLayerCfg * GetMedalFbLayerCfg(int fb_seq,int fb_layer);
	bool IsSingleFb(int fb_seq);
	const MedalMaterialItemCfg * GetMedalMaterialItemCfg(int seq);
private:
	int InitFbLevelCfg(TiXmlElement *RootElement);
	int InitMaterialCfg(TiXmlElement * RootElement);


	std::map<int, MedalFbLevelCfg> m_fb_level_cfg;
	std::vector<MedalMaterialItemCfg> m_fb_material_cfg;
};

#endif