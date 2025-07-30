#ifndef __YIN_YANG_EYE_CONFIG_HPP__
#define __YIN_YANG_EYE_CONFIG_HPP__

#include <vector>
#include <map>
#include <set>
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/yinyangeyedef.h"

struct YinYangEyeBoxCfg
{
	YinYangEyeBoxCfg()
	{
		seq = 0;
		scene_id = 0;
		reward_group = 0;
		npc_id = 0;
		npc_type = 0;
		monster_group_id = 0;
		is_always_appear = 0;
	}

	int seq;
	int scene_id;
	Posi pos;
	int reward_group;
	int npc_id;
	int npc_type;
	int monster_group_id;
	int is_always_appear;			//0:开眼才能看的到
};

struct YinYangEyeNpcCfg
{
	YinYangEyeNpcCfg()
	{
		seq = 0;
		scene_id = 0;
		npc_id = 0;
		npc_type = 0;
		is_anecdotes = 0;
		reward_group = 0;
		monster_group_id = 0;
		is_always_appear = 0;
	}

	int seq;
	int scene_id;
	Posi pos;
	int npc_id;
	int npc_type;					//0：善NPC 1：恶NPC
	int is_anecdotes;				//是否奇闻
	int reward_group;
	int monster_group_id;
	int is_always_appear;			// 0:普通状态下隐藏阴阳眼状态下出现
};

struct YinYangEyeRewardCfg
{
	YinYangEyeRewardCfg() {}

	std::vector<ItemConfigData> reward_list;
};

struct YinYangEyeOtherCfg
{
	YinYangEyeOtherCfg()
	{
		box_radius = 0;
	}

	int box_radius;				//宝箱交互半径坐标
};

// -----------------------------------------------------------------------------------
class YinYangEyeConfig : public ILogicConfig
{
public:
	YinYangEyeConfig();
	virtual ~YinYangEyeConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const YinYangEyeOtherCfg& GetOtherCfg()const { return m_other_cfg; }
	const YinYangEyeBoxCfg* GetBoxCfg(int seq)const;
	const YinYangEyeNpcCfg* GetNpcCfg(int seq)const;
	const YinYangEyeRewardCfg* GetRewardCfg(int reward_group)const;
	const std::vector<YinYangEyeBoxCfg> & GetBoxVecCfg()const { return m_box_cfg; }
	const std::vector<YinYangEyeNpcCfg> & GetNpcVecCfg()const { return m_npc_cfg; }

private:
	int InitBoxCfg(TiXmlElement * RootElement);
	int InitNpcCfg(TiXmlElement * RootElement);
	int InitRewardCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	YinYangEyeOtherCfg m_other_cfg;
	std::vector<YinYangEyeBoxCfg> m_box_cfg;
	std::vector<YinYangEyeNpcCfg> m_npc_cfg;
	std::map<int, YinYangEyeRewardCfg> m_reward_cfg;		//key-reward_groud
};

#endif

