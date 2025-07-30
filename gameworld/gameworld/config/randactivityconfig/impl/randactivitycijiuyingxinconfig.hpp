#ifndef __RAND_ACTIVITY_CI_JIU_YING_XIN_CONFIG_HPP__
#define __RAND_ACTIVITY_CI_JIU_YING_XIN_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itemextern.hpp"
#include "item/itembase.h"


struct RACiJiuYingXinRewardListCfg
{
	RACiJiuYingXinRewardListCfg() :item_count(0) {}

	int item_count;
	ItemConfigData item_list[5];
};

struct RACiJiuYingXinSubjectCfg
{
	RACiJiuYingXinSubjectCfg() :id(0), answer_id(0), weight(0) {}

	int id;
	int answer_id;
	int weight;
};

class RandActivityManager;

class RandActivityCiJiuYingXinConfig : public RandActivityConfig
{
public:
	RandActivityCiJiuYingXinConfig();
	virtual ~RandActivityCiJiuYingXinConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	int RandSubject() const;
	const RACiJiuYingXinSubjectCfg * GetCiJiuYingXinSubjectCfg(int id) const;
	const RACiJiuYingXinRewardListCfg * GetCiJiuYingXinRewardListCfg(int id) const;

private:
	int InitCiJiuYingXinCfg(TiXmlElement *RootElement);
	int InitCiJiuYingXinRewardCfg(TiXmlElement *RootElement);

	int  weight_count;
	std::map<int, RACiJiuYingXinSubjectCfg> m_subject_map;
	std::map<int, RACiJiuYingXinRewardListCfg> m_reward_map;
};

#endif	//__RAND_ACTIVITY_GIVE_PET_DRAW_CONFIG_HPP__