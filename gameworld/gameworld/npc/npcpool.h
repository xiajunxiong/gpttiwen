#ifndef __NPCPOOL_HPP__
#define __NPCPOOL_HPP__

#include "gamedef.h"
#include "servercommon/struct/itemlistparam.h"

#include <map>
#include <vector>

static const int GATHER_ITEM_TYPE_PER_GROUP = 50;
static const int NPC_MINE_MONSTER_NPC_ID = 65534;		// 以暗怪逻辑做明怪，写死NPC_ID  

struct NPCInformaCfg;		// NPC 配置信息键值对 - 值 // 键 是 配置的 seq - ID

enum SERVER_NPC_TYPE
{
	SERVER_NPC_TYPE_INVALID = 0,//无
	SERVER_NPC_TYPE_MONSTER = 1,//明怪
	SERVER_NPC_TYPE_GATHERS = 2,//采集物
	SERVER_NPC_TYPE_CHOSED = 3,//交互npc

	SERVER_NPC_TYPE_MAX
};

struct ServerNPCMonsterParam
{
	int monster_group_id;
};

struct ServerNPCGatherParam		// 采集物
{
	void Reset()
	{
		gather_id = 0;
		gather_time = 0;
		gather_type = SERVER_NPC_TYPE_INVALID;

		award_item_count = 0;
		memset(gather_item_group_list, 0, sizeof(gather_item_group_list));
	}

	int	gather_id;				// 采集物ID
	int gather_time;			// 采集所需时间
	int gather_type;			// 采集物类型

	int award_item_count;		// 奖品总数
	int gather_item_group_list[GATHER_ITEM_TYPE_PER_GROUP];	// 奖励表
};

struct ServerNPCConsumeParam		// 消耗列表
{
	int	consume_id;				// 消耗品ID
	int consume_num;			// 消耗品数量
	int consume_coin;			// 消耗铜币
	int consume_gold;			// 消耗元宝
};

union ServerNPCParam
{
	ServerNPCMonsterParam monster_param;
	ServerNPCGatherParam gather_param;
	ServerNPCConsumeParam consume_param;
};

struct NPCDynamicInformaCfg		// NPC 配置信息键值对 - 值 // 键 是 配置的 seq - ID
{
	NPCDynamicInformaCfg(int p_npc_id = 0) : npc_id(p_npc_id), scene_id(0), npc_posi(0, 0) {}

	int npc_id;				// NPC ID

	int scene_id;			// 场景
	Posi npc_posi;			// 位置
};

struct NPCItemGetCfg		// 采集物奖励配置
{
	NPCItemGetCfg() : prob(0) {};

	int prob;
	ItemConfigData award_item;
};

struct ServerNPCInfoCfg
{
	ServerNPCInfoCfg() : npc_id(0), server_npc_type(0) {}

	int npc_id;
	int server_npc_type;	// 服务器NPC类型  \see SERVER_NPC_TYPE

	ServerNPCParam server_npc_param;
};

enum OPTION_EFFECT_TYPE//选项效果
{
	OPTION_EFFECT_TYPE_NO = 0,		//!< 对话
	OPTION_EFFECT_TYPE_BATTLE = 1,	//!< 战斗
	OPTION_EFFECT_TYPE_CONSUME = 2,	//!< 花费
	OPTION_EFFECT_TYPE_SHOP = 3		//!< 商店
};

struct option_cfg
{
	option_cfg() :operation_id(0),option_content(0), option_effect_type(OPTION_EFFECT_TYPE_NO), cost_coin(0), monster_group_id(0){}
	int operation_id;					//!< 选项id
	int option_content;					//!< 选项内容语言seq
	int option_effect_type;				//!< 选项效果
	int cost_coin;						//!< 消耗铜币
	int monster_group_id;
	ItemConfigData need_item;
};

struct NPCChosedTalkInfoCfg
{
	NPCChosedTalkInfoCfg(){}
	const static int MAX_OPTION_NUM = 2;// 最大选项数
	int npc_id;
	int main_context;
	option_cfg option_list[MAX_OPTION_NUM];
};

struct NPCTreasureChestInfoCfg
{
	NPCTreasureChestInfoCfg() {}
	int box_id;
	int consume_id;
	int consume_num;
};

// ————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————————— //

class Role;

#define NPCPOOL NpcPool::Instance()

class NpcPool
{
public:
	typedef std::map<int, ServerNPCInfoCfg> ServerNpcCfgMap;

public:
	NpcPool();
	~NpcPool();

	static NpcPool * Instance();

	bool Init(const std::string & dir, std::string * err);
	static bool Reload(const std::string& dir, std::string* err);

	const NPCDynamicInformaCfg * GetNPCInformaCfg(int seq_id);
	const ServerNPCInfoCfg * GetServerNPCInfo(int npc_id);
	const ItemConfigData * RandGatherAwardItem(int award_pool_id);
	const std::vector<Posi>* GetSceneClientNpcList(int scene_id);
	const NPCChosedTalkInfoCfg * GetChosedNpcCfg(int npc_id);
	const NPCTreasureChestInfoCfg * GetTreasureChestCfg(int treasurechest_id);
private:
	int InitNpcInformaCfg(TiXmlElement * RootElement);
	int InitGatherListCfg(TiXmlElement * RootElement);
	int InitRewardPoolCfg(TiXmlElement * RootElement);
	int InitServerNPCCfg(TiXmlElement * RootElement);
	int InitOperationCfg(TiXmlElement *RootElement);//先加载选项
	int InitChosedNPCCfg(TiXmlElement *RootElement);//写入配置的时候把选项加载进ChosedNPC里面
	int InitTreasureChestCfg(TiXmlElement *RootElement);

	std::map<int, NPCDynamicInformaCfg> m_npc_info_cfg;
	ServerNpcCfgMap m_server_npc_map;

	std::map<int, std::vector<NPCItemGetCfg> > m_gather_item_cfg;	// 采集奖池配置

	std::map<int, std::vector<Posi> > m_scene_npc_posi_map;

	std::map<int, NPCChosedTalkInfoCfg> m_chosed_npc_cfg_map;//选项NPC配置

	std::map<int, option_cfg> m_option_cfg_map;//选项操作配置

	std::map<int, NPCTreasureChestInfoCfg> m_treasure_chest_cfg_map;//选宝箱消耗配置
};

#endif