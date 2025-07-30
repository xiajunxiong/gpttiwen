#ifndef __RAND_ACTIVITY_JIAN_DAN_QIN_XIN_CONFIG_HPP__
#define __RAND_ACTIVITY_JIAN_DAN_QIN_XIN_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rajiandanqinxinparam.hpp"
#include "item/itembase.h"

struct RAJianDanQinXinOtherCfg
{
	RAJianDanQinXinOtherCfg() :need_member_count(0)
	{}

	int need_member_count;			//组队人数不少于	(应策划要求,单人与组队都可以,该字段暂时不读)
};

struct RAJianDanQinXinRewardCfg
{
	RAJianDanQinXinRewardCfg() :seq(0), checkpoint_layer(0), type(0), monster_group_id(0), reward_count(0)
	{}

	int seq;
	int checkpoint_layer;			//哪一关卡
	int type;						// JIAN_DAN_QIN_XIN_TYPE
	int monster_group_id;			//本关卡怪物组ID

	int reward_count;
	ItemConfigData item_reward_list[MAX_RA_JIANDANQINXIN_REWARD_NUM];	//道具奖励列表
};

struct RAJianDanQinXinSectionCfg
{
	RAJianDanQinXinSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::vector<RAJianDanQinXinRewardCfg> layer_cfg;
};

class RandActivityManager;

class RandActivityJianDanQinXinConfig : public RandActivityConfig
{
public:
	RandActivityJianDanQinXinConfig();
	virtual ~RandActivityJianDanQinXinConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAJianDanQinXinOtherCfg & GetOtherCfg() const  { return m_other_cfg; }
	const RAJianDanQinXinRewardCfg* GetLayerCfgByMonsterId(RandActivityManager* ramgr, int monster_group_id) const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);

	RAJianDanQinXinOtherCfg m_other_cfg;
	std::vector<RAJianDanQinXinSectionCfg> m_section_cfg;
};

#endif	//__RAND_ACTIVITY_JIAN_DAN_QIN_XIN_CONFIG_HPP__