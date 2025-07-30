#ifndef __RAND_ACTIVITY_MOZUZAINALI_CONFIG_HPP__
#define __RAND_ACTIVITY_MOZUZAINALI_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itembase.h"


struct RAMoZuZaiNaLiOtherCfg
{
	RAMoZuZaiNaLiOtherCfg() : frequency(0) {}

	int frequency;			// 每日驱魔次数
};

struct RAMoZuZaiNaLiRewardCfg
{
	std::vector<ItemConfigData> reward_item;
};

struct RAMoZuZaiNaLiPosiCfg
{
	RAMoZuZaiNaLiPosiCfg() : scene_id(0), npc_id(0) {}

	int scene_id;
	int npc_id;
	Posi pos;
};

class RandActivityManager;

class RandActivityMoZuZaiNaLiConfig : public RandActivityConfig
{
public:
	RandActivityMoZuZaiNaLiConfig();
	virtual ~RandActivityMoZuZaiNaLiConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RAMoZuZaiNaLiRewardCfg * GetRewardCfg(RandActivityManager * ramgr) const;

	const std::vector<RAMoZuZaiNaLiPosiCfg> & GetPosiMapCfg() const { return m_posi_cfg; }
	const RAMoZuZaiNaLiOtherCfg & GetOtherCfg() const { return m_other_cfg; }

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitPosiCfg(TiXmlElement *RootElement);

	RAMoZuZaiNaLiOtherCfg m_other_cfg;
	std::map<int, RAMoZuZaiNaLiRewardCfg> m_reward_cfg;
	std::vector<RAMoZuZaiNaLiPosiCfg> m_posi_cfg;
};

#endif	//__RAND_ACTIVITY_ZHAOCAIJINBAO_CONFIG_HPP__