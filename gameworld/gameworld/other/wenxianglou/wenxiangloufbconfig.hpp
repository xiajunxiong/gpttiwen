#ifndef __WEN_XIANG_LOU_FB_CONFIG_HPP__
#define __WEN_XIANG_LOU_FB_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "gamedef.h"
#include "npc/npcpool.h"

#include <map>
#include <vector>

struct RoleClueConfig
{
	RoleClueConfig():seq(0), npc_id(0), suspicion(0), identity(0), add_suspicion(0){}
	int seq;
	int npc_id;
	int suspicion;
	int identity;
	int add_suspicion;
	std::vector<int> m_directed_list;
};

struct ItemClueConfig
{
	ItemClueConfig() :seq(0), x(0), y(0), directed(0), add_suspicion(0) {}
	int seq;
	int x;
	int y;
	int directed;
	int add_suspicion;
};

struct EventConfig
{
	EventConfig() :stage(0), x(0), y(0), talk_id(0){}
	int stage;
	int x;
	int y;
	int talk_id;
};

struct OtherConfig
{
	OtherConfig() : min_suspicion(0), prove_error(0), prove_correct(0), radius(0), item_radius(0), battle_1(0), battle_2(0), scene_id(0){}
	int min_suspicion;
	int prove_error;
	int prove_correct;
	int radius;
	int item_radius;
	int battle_1;
	int battle_2;
	int scene_id;
};

class WenXiangLouFbConfig : public ILogicConfig
{
public:
	WenXiangLouFbConfig();
	virtual ~WenXiangLouFbConfig();
	
	enum WEN_XIANG_LOU_IDENTITY
	{
		WEN_XIANG_LOU_IDENTITY_0 = 0,		// 死者
		WEN_XIANG_LOU_IDENTITY_1,			// 指认NPC
		WEN_XIANG_LOU_IDENTITY_2,			// 嫌疑人
		WEN_XIANG_LOU_IDENTITY_3,			// 凶手
	};

	virtual bool Init(const std::string &configname, std::string *err);
	const RoleClueConfig * GetRoleClueCfg(int seq);
	const ItemClueConfig * GetItemClueCfg(int seq);
	const EventConfig * GetEventCfg(int seq);
	const OtherConfig * GetOtherCfg() { return &m_other_cfg; }
private:
	int InitRoleClueCfg(TiXmlElement * RootElement);
	int InitItemClueCfg(TiXmlElement * RootElement);
	int InitEventCfg(TiXmlElement * RootElement);
	int InitOhterCfg(TiXmlElement *RootElement);

	std::map<int, RoleClueConfig> m_role_clue_cfg;
	std::map<int, ItemClueConfig> m_item_clue_cfg;
	std::map<int, EventConfig> m_event_cfg;
	OtherConfig m_other_cfg;
};

#endif