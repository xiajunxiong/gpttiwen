#ifndef __FANTASY_FB_CONFIG_HPP__
#define __FANTASY_FB_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "servercommon/commondata.hpp"
#include "servercommon/struct/itemlistparam.h"

static const int TEAM_MEMBER_MAX_NUM = 5;
static const int RATE_COUNT_MAX_NUM = 10000;			//概率总值

struct SpecialLogicFantasyObjInfo
{
	SpecialLogicFantasyObjInfo() :npc_id(0), obj_type(0), obj_id(0), pos_x(0), pos_y(0)
	{}

	int npc_id;
	short obj_type;
	ObjID obj_id;
	short pos_x;
	short pos_y;
};

struct FantasyFBOtherCfg
{
	FantasyFBOtherCfg():npc_id(0), min_team_num(0), min_level(0)
	{}

	int npc_id;
	int min_team_num;					//队伍最小人数
	int min_level;
};

struct FantasyFBSceneCfg
{
	FantasyFBSceneCfg():seq(0), scene_id(0), boss_id(0), monster_dir(0),
		event_num(0), event_group_id(0)
	{
		boss_pos = Posi(0, 0);
		birth_pos = Posi(0, 0);
		transport_pos = Posi(0, 0);
	}
	int seq;
	int scene_id;					//当前场景id
	int boss_id;					//该层boss_id
	Posi boss_pos;					//boss坐标
	Dir monster_dir;				//boss转向
	Posi birth_pos;					//出生点坐标
	Posi transport_pos;				//传送阵坐标
	int event_num;					//采集物数量
	int event_group_id;				//采集物随机组
};

struct FantasyFBRewardGroupCfg
{
	FantasyFBRewardGroupCfg():rate(0)
	{}
	
	int rate;
	ItemConfigData reward;
};

struct FantasyFBRewardCfg				//boss击杀奖励s
{
	FantasyFBRewardCfg(): boss_id(0), bind_coin(0), reward_exp(0)
	{}

	int boss_id;
	int bind_coin;
	int reward_exp;
	std::vector<ItemConfigData> reward;
};

struct FantasyFbKillBossRewardCfg			//boss击杀等级组奖励
{
	FantasyFbKillBossRewardCfg(): count(0)
	{}

	int count;
	FantasyFBRewardCfg node[LAYER_MAX_NUM];
};

struct Event_NPC_FB_Cfg
{
	enum FANTASYFB_NPC_TYPE
	{
		FANTASYFB_NPC_TYPE_0 = 0,		//!< 未知物品
		FANTASYFB_NPC_TYPE_1 = 1,		//!< 怪物
		FANTASYFB_NPC_TYPE_2 = 2,		//!< npc
		FANTASYFB_NPC_TYPE_3 = 3,		//!< Boss
		FANTASYFB_NPC_TYPE_4 = 4,		//!< 篝火
		FANTASYFB_NPC_TYPE_5 = 5,		//!< 铜币
		FANTASYFB_NPC_TYPE_6 = 6,		//!< 采集物 - 木材
		FANTASYFB_NPC_TYPE_7 = 7,		//!< 采集物 - 矿石
		FANTASYFB_NPC_TYPE_8 = 8,		//!< 采集物 - 草
		FANTASYFB_NPC_TYPE_9 = 9,		//!< 宝箱
		FANTASYFB_NPC_TYPE_10 = 10,		//!< 传说门
	};
	Event_NPC_FB_Cfg() : event_group_id(0), npc_id(0), rate(0), npc_type(0), box_type(0), consume_item(0), consume_num(0)
	{}

	int event_group_id;					//事件组id
	int npc_id;	
	int rate;
	int npc_type;
	int box_type;
	int consume_item;					//仅当事件是宝箱时才需要
	int consume_num;					
};

struct FantasyFBGatherRewardCfg
{
	FantasyFBGatherRewardCfg():npc_id(0), reward_type(0), reward_id(0)
	{}
	enum REWARD_TYPE
	{
		REWARD_TYPE_0 = 1,//buff
		REWARD_TYPE_1 = 2,//奖励id
		REWARD_TYPE_2 = 3,//铜币
		REWARD_TYPE_MAX,
	};

	int npc_id;
	int reward_type;
	int reward_id;				//奖励组ID
};

struct FantasyFBGatherRewardlevelCfg
{
	FantasyFBGatherRewardlevelCfg()
	{}

	std::vector<FantasyFBGatherRewardCfg> node_cfg;
};

struct FantasyFBFindRewardCfg				//找回奖励
{
	FantasyFBFindRewardCfg():find_type(0), exp(0), coin(0), family_gongxian(0), group_id(0), min_level(0), max_level(0)
	{}

	int find_type;
	int exp;
	int coin;
	int family_gongxian;
	int group_id;
	int min_level;
	int max_level;
	int seq;
};

struct FantasyFbFindLevelGroupCfg			//找回等级奖励组
{
	FantasyFbFindLevelGroupCfg(): count(0)
	{}

	int count;
	FantasyFBFindRewardCfg node[LAYER_MAX_NUM];
};


struct FantasyFBFindRewardGroupCfg
{
	FantasyFBFindRewardGroupCfg():rate(0)
	{}

	int rate;
	ItemConfigData reward;
};

class FantasyFbConfig : public ILogicConfig
{
public:
	FantasyFbConfig();
	virtual ~FantasyFbConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const FantasyFBOtherCfg* GetOtherCfg() { return &m_other_cfg; }
	const FantasyFBSceneCfg* GetSceneCfgBySeq(int seq);
	const FantasyFBSceneCfg* GetFBSceneCfg(int level, std::map<int, int>* seq_by_layer);
	const int GetSeqByBossid(int _boss_id);

	const FantasyFBRewardGroupCfg* GetRewardGroupCfg(int reward_id);
	const FantasyFbKillBossRewardCfg* GetRewardCfg(int level);
	int GetEventNPCFBCfg(int event_group_id, int need_npc_num, std::vector<Event_NPC_FB_Cfg>* node_cfg, std::map<int, std::vector<SpecialLogicFantasyObjInfo> >* obj_info_list);
	const FantasyFBGatherRewardCfg* GetGatherRewardCfg(int level, int npc_id);
	const FantasyFbFindLevelGroupCfg* GetFindCfg(int level, int find_type);
	const FantasyFBFindRewardGroupCfg* GetFindRewardCfg(int group_id);
	const int GetFindType(int _role_level);

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitSceneCfg(TiXmlElement *RootElement);
	int InitRewardGroupCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitEventNPCCfg(TiXmlElement *RootElement);
	int InitGatherRewardCfg(TiXmlElement *RootElement);
	int InitFindRewardGroupCfg(TiXmlElement *RootElement);
	int InitFindCfg(TiXmlElement *RootElement);
	
	FantasyFBOtherCfg m_other_cfg;			
	std::map<int, FantasyFBSceneCfg> m_scene_cfg;								//以seq为key
	std::map<int, std::vector<FantasyFBSceneCfg> > m_scene_by_level;			//以1000 * min_level + max_level为key

	std::map<int, std::vector<FantasyFBRewardGroupCfg> > m_reward_group_cfg;	//以reward_id为key
	std::map<int, FantasyFbKillBossRewardCfg> m_reward_cfg;						//以1000 * min_level + max_level为key
	std::map<int, std::vector<Event_NPC_FB_Cfg> > m_event_npc_cfg;				//以event_group_id 为key
	std::map<int, FantasyFBGatherRewardlevelCfg> m_gather_reward_cfg;			//以1000 * min_level + max_level为key

	// 奖励找回
	std::map<int, std::vector<FantasyFBFindRewardGroupCfg> > m_find_reward_group_map;			// 找回道具组：键 - 组ID
	std::map<int, std::map<int, FantasyFbFindLevelGroupCfg> > m_fantasy_find_cfg;	// 奖励找回表：键 - 等级区间、找回类型

	std::map<int, int> m_bossid_to_seq;				//boss_id  -->  seq
	std::map<int ,FantasyFBFindRewardCfg> m_seq_to_fanrwcfg;
	std::vector<FantasyFBFindRewardCfg> m_fanrwcfg_v;
};


#endif
