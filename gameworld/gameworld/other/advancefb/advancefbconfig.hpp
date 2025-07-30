#ifndef __ADVANCE_FB_CONFIG_HPP__
#define __ADVANCE_FB_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "gamedef.h"
#include "npc/npcpool.h"

#include <map>
#include <vector>

static const int ADV_FB_ATTR_CHANGE_FB_BOSS_NUM = 3;

enum AdvanceFbPlayType
{
	ADV_TYPE_DEFEAT = 0,					// 默认-踏上最后一关传送阵通关
	ADV_TYPE_CAN_COMMIT_TASK = 1,			// 消耗物品为使用传送阵条件
	ADV_TYPE_PASS_AGAIN = 2,				// 多次通关,
	ADV_TYPE_TASK_PASS = 3,					// 收集任务道具，传送第二副本，到达最后一层击败boss通关    特殊参数是任务收集进度
	ADV_TYPE_MAX,
};

/*
struct AdvFbBossCon
{
	AdvFbBossCon() : npc_id(0), posi(0, 0) {}

	int npc_id;
	Posi posi;
};

struct AdvFbOtherCfg
{
	AdvFbOtherCfg()
	{
		memset(min_team_num, 0, sizeof(min_team_num));
	}

	int min_team_num[ADV_TYPE_MAX];			// 每种玩法的最小组队人数需求
};

struct AdvFbMonsterListCfg
{
	AdvFbMonsterListCfg() : scene_id(0) {}

	int scene_id;
	std::vector<int> monster_list;
};

struct AdvFbAttrChangesCfg
{
	AdvFbAttrChangesCfg() : scene_id(0), attr_type(0), attr_change_rate(0) {}

	int scene_id;
	int attr_type;				// 属性类型；
	int attr_change_rate;		// 全员的属性改变率，正数为加，负数为减；（百分比）
};

struct AdvFbCloneInformCfg
{
	AdvFbCloneInformCfg() : scene_id(0), clone_attr_change_rate(0) {}

	int scene_id;
	int clone_attr_change_rate;		// 分身的属性改变率，正数为加，负数为减；（百分比）
	AdvFbBossCon boss_con;
};*/

struct AdvFbInfoCfg
{
	AdvFbInfoCfg() : adv(0), level(0), play_type(0), min_level(0),scene_id(0), transport_id(0), transport_pos(0,0), enter_pos(0,0), task_id(0), drop_id(0),
		drop_num(0), item_id(0), item_num(0), item_inv_time(0), boss_npc_id(0), boss_pos(0,0), dir_y1(0), boss_npc_id2(0), boss_pos2(0, 0), dir_y2(0),
		npc_id(0),npc_pos(0,0),npc_dir(0),recover_num(0){}

	int adv;
	int level;
	int play_type;
	int min_level;
	int scene_id;
	int transport_id;
	Posi transport_pos;
	Posi enter_pos;
	
	int task_id;
	int drop_id;
	int drop_num;
	int item_id;
	int item_num;
	int item_inv_time;

	int boss_npc_id;
	Posi boss_pos;
	Dir dir_y1;
	int boss_npc_id2;
	Posi boss_pos2;
	Dir dir_y2;

	int npc_id;
	Posi npc_pos;
	Dir npc_dir;
	int recover_num;

	std::vector<int> monster_drop_list;
};

struct AdvanceNpcOption
{
	AdvanceNpcOption() : npc_id(0), operation_id(0), is_delete(0), operation_type(ADVANCE_OPTION_TYPE_NO) {}
	enum ADVANCE_OPTION_TYPE
	{
		ADVANCE_OPTION_TYPE_NO = -1,//无
		ADVANCE_OPTION_TYPE_0,		//恢复
		ADVANCE_OPTION_TYPE_1,		//奖励
		ADVANCE_OPTION_TYPE_MAX,
	};
	int npc_id;
	int operation_id;
	int is_delete;
	int operation_type;
	std::vector<int> task_id_list;
	ItemConfigData item;
};

struct AdvanceNpcOptionList
{
	AdvanceNpcOptionList() {}
	AdvanceNpcOption info[NPCChosedTalkInfoCfg::MAX_OPTION_NUM];
};

struct AdvanceDropCfg
{
	AdvanceDropCfg():npc_id(0) {}
	int npc_id;
	std::vector<int> task_id_list;
	std::vector<ItemConfigData> item_vec;
};

class AdvanceFbConfig : public ILogicConfig
{
public:
	AdvanceFbConfig();
	virtual ~AdvanceFbConfig();
	
	virtual bool Init(const std::string &configname, std::string *err);

	/*const AdvFbOtherCfg & GetAdvFbOtherCfg() { return m_adv_fb_other; }
	const AdvFbCloneInformCfg & GetAdvCloneCfg() { return m_clone_info; }
	const std::vector<AdvFbBossCon> & GetCloneMonsterCfg() { return m_monster_clone_list; }
	const AdvFbMonsterListCfg * GetClearListByTier(int tier);
	const AdvFbAttrChangesCfg * GetAttrChangeCfg(int fb_type, int tier);
	const AdvFbBossCon * GetACFbBoss(int param);*/
	const AdvFbInfoCfg * GetAdvFbInfoCfg(int adv_idx, int level);
	const AdvanceNpcOption * GetNpcOptionCfg(int npcid,int option_index);
	const AdvanceDropCfg * GetDropByNPCidCfg(int npc_id);
	bool HasNoticeTaskID(int task_id);
private:
	int InitAdvFbInfoCfg(TiXmlElement * RootElement);
	int InitAdvFbNpcInfoCfg(TiXmlElement * RootElement);
	int InitAdvFbNpcDropCfg(TiXmlElement * RootElement);
	int InitOhterCfg(TiXmlElement *RootElement);
	/*int InitAdvFbOthersType0Cfg(TiXmlElement * RootElement);
	int InitAdvFbOthersType1Cfg(TiXmlElement * RootElement);
	int InitAdvFbMonsterListCfg(TiXmlElement * RootElement);
	int InitAdvFbAttrChangesCfg(TiXmlElement * RootElement);
	int InitAdvFbCloneMonsteCfg(TiXmlElement * RootElement);
	int InitAdvFbShadowsAttrCfg(TiXmlElement * RootElement);*/

	/*AdvFbOtherCfg m_adv_fb_other;

	AdvFbCloneInformCfg m_clone_info;
	std::vector<AdvFbBossCon> m_monster_clone_list;		// 怪物列表

	std::map<int, AdvFbMonsterListCfg> m_monster_clear_map;					// 怪物清理表：键 - 层数；

	std::map<int, std::map<int, AdvFbAttrChangesCfg> > m_attr_change_map;	// 属性改变表：键 - 副本类型、层数；
	std::map<int, AdvFbBossCon> m_ac_boss_map;								// BOSS 列表：键 - 类型参数；
*/

	std::map<int, std::map<int, AdvFbInfoCfg> > m_fb_info_map;			//进阶副本
	std::map<int, AdvanceNpcOptionList > m_npc_option_cfg;
	std::map<int, AdvanceDropCfg> m_drop_cfg;
	std::vector<int> m_notice_task_cfg;
};

#endif