#ifndef __RAND_ACTIVITY_XUNMENG_HUDIE_CONFIG_HPP__
#define __RAND_ACTIVITY_XUNMENG_HUDIE_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"


struct RAXunMengHuDieSceneCfg
{
	RAXunMengHuDieSceneCfg() :scene_id(0), npc_id(0) {}

	int scene_id;
	int npc_id;
	Posi pos;
};

struct RAXunMengHuDieRewardCfg
{
	RAXunMengHuDieRewardCfg() {}

	std::vector<ItemConfigData> reward;
};

struct RAXunMengHuDieOtherCfg
{
	RAXunMengHuDieOtherCfg() : frequency(0) {}

	int frequency;
};


class RandActivityManager;
class RandActivityXunMengHuDieConfig : public RandActivityConfig
{
public:
	RandActivityXunMengHuDieConfig();
	virtual ~RandActivityXunMengHuDieConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const std::vector<RAXunMengHuDieSceneCfg> & GetSceneMapCfg() const { return m_scene_cfg; }
	const RAXunMengHuDieRewardCfg * GetRewardCfg(RandActivityManager * ramgr) const;
	const RAXunMengHuDieOtherCfg * GetOtherCfg() const { return &m_other_cfg; }

private:
	int InitSceneCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::vector<RAXunMengHuDieSceneCfg> m_scene_cfg;
	std::map<short, RAXunMengHuDieRewardCfg> m_reward_cfg;
	RAXunMengHuDieOtherCfg m_other_cfg;
};


#endif	// __RAND_ACTIVITY_XUNMENG_HUDIE_CONFIG_HPP__