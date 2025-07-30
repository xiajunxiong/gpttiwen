#ifndef __ROLE_HEART_SKILL_CONFIG_HPP__
#define __ROLE_HEART_SKILL_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/configcommon.h"
#include "servercommon/roleheartskilldef.hpp"
#include "servercommon/struct/itemlistparam.h"
#include <set>

struct HeartSkillBaseCfg
{
	HeartSkillBaseCfg() : heart_skill_type(0), heart_skill_id(0)
	{}

	int heart_skill_type;
	int heart_skill_id;
};

struct HeartSkillHoleCfg
{
	HeartSkillHoleCfg() : unlock_level(0), heart_skill_type(0)
	{}
	
	int unlock_level;
	int heart_skill_type;
};

struct HeartSkillLevelCfg
{
	HeartSkillLevelCfg() : skill_level(0), coin_consume(0), gold_consume(0)
	{}

	int skill_level;
	int coin_consume;
	int gold_consume;	
	std::vector<ItemConfigData> item_consume_list;
};

struct HeartSkillResonanceLevelCfg
{
	HeartSkillResonanceLevelCfg()
	{
		memset(attr_list, 0, sizeof(attr_list));
	}

	struct ResonanceAttrInfo
	{
		ResonanceAttrInfo(): attr_type(0), attr_value(0)
		{}

		short attr_type;
		short attr_value;
	};
	ResonanceAttrInfo attr_list[MAX_HEART_SKILL_RESONANCE_ATTR_NUM];
};

struct HeartSkillConvertCfg
{
	HeartSkillConvertCfg() : convert_consume_item_id(0)
	{}

	int convert_consume_item_id;
	std::set<int> convert_list;	//道具ID
};

struct HeartSkillDisplaceCfg
{
	HeartSkillDisplaceCfg() : need_heart_skill_num(0), need_consume_item_id(0), displace_target_item_id(0), target_num(0)
	{}

	int need_heart_skill_num;
	int need_consume_item_id;
	int displace_target_item_id;
	int target_num;
};

struct HeartSkillAttrCfg
{
	HeartSkillAttrCfg()
	{
		memset(attr_list, 0, sizeof(attr_list));
	}

	AttrInfo attr_list[MAX_HEART_SKILL_ATTR_NUM];
};

class RoleHeartSkillConfig : public ILogicConfig
{
public:
	RoleHeartSkillConfig();
	virtual ~RoleHeartSkillConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const HeartSkillBaseCfg * GetBaseCfg(int heart_skill_seq);
	int GetBaseColorById(int item_id);
	const HeartSkillHoleCfg* GetUnlockLevel(int hole_seq);
	int GetSkillScore(int heart_skill_seq, int skill_level);
	int GetMaxLevelBySeq(int heart_skill_seq);
	const HeartSkillLevelCfg * GetLevelCfg(int heart_skill_seq, int skill_level);
	static int GetSkillTypeByStatus(int skill_status);

	bool IsMeetReasonance(int reasonance_group, int count);
	int GetReasonanceGroupBySeq(int heart_skill_seq);
	const HeartSkillResonanceLevelCfg * GetReasonanceLevelCfg(int resonance_group, int level);

	const ItemConfigData *GetComposeCfg(int compose_target);
	const HeartSkillConvertCfg * GetConvertCfg(int convert_item_id);
	const HeartSkillDisplaceCfg * GetDisplaceCfg(int heart_skill_color);
	const HeartSkillAttrCfg * GetAttrCfg(int heart_skill_seq, int skill_level, int prof);

private:

	int InitBaseCfg(TiXmlElement *RootElement);
	int InitHoleCfg(TiXmlElement *RootElement);
	int InitScoreCfg(TiXmlElement *RootElement);
	int InitLevelCfg(TiXmlElement *RootElement);
	int InitResonanceCfg(TiXmlElement *RootElement);
	int InitResonanceLevelCfg(TiXmlElement *RootElement);
	int InitComposeCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitDisplaceCfg(TiXmlElement *RootElement);
	int InitAttrCfg(TiXmlElement *RootElement);

	int GetKeyBySeqAndLevel(int heart_skill_seq, int skill_level);

	std::map<int, HeartSkillBaseCfg> m_base_cfg;			//基础配置 key:heart_skill_seq
	std::map<int, int> m_base_color_cfg;					//基础品质配置 key:item_id second:heart_skill_color
	std::map<int, HeartSkillHoleCfg> m_skill_hole_cfg;		//孔位配置 key:hole_seq(HEART_SKILL_STATUS) 
	std::map<int, int> m_skill_score_cfg;					//心法评分 key:MAX_HEART_SKILL_BASE_NUM * heart_skill_seq + skill_level  second:skill_score
	std::map<int, std::map<int, HeartSkillLevelCfg> > m_skill_level_map;	//心法技能等级配置 key1:heart_skill_seq key2:skill_level

	std::map<int, int> m_reasonance_count_by_group;			//共鸣组中数量 key:resonance_group second:count
	std::map<int, int> m_reasonance_group_by_seq;			//共鸣配置	key:heart_skill_seq second:resonance_group
	std::map<int, HeartSkillResonanceLevelCfg> m_skiLL_resonance_level_cfg;		//心法共鸣等级配置 key:resonance_group * MAX_HEART_SKILL_BASE_NUM + level

	std::map<int, ItemConfigData> m_compose_map;			//合成配置	key:compose_target
	std::vector<HeartSkillConvertCfg> m_convert_cfg;		//转换配置

	std::map<int, HeartSkillDisplaceCfg> m_displace_map;	//置换配置 key:heart_skill_color
	std::map<int, HeartSkillAttrCfg> m_attr_cfg_list[PROFESSION_TYPE_NUM];	//心法属性 key:GetKeyBySeqAndLevel
};


#endif //__ROLE_HEART_SKILL_CONFIG_HPP__