#ifndef __RAND_ACTIVITY_XIN_FU_QIANG_HUA_CONFIG_HPP__
#define __RAND_ACTIVITY_XIN_FU_QIANG_HUA_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <map>

struct RAXinFuQiangHuaOtherCfg
{
	RAXinFuQiangHuaOtherCfg()
	{}

	std::vector<ItemConfigData> free_reward_list;
};

class RandActivityManager;
class RandActivityXinFuQiangHuaConfig : public RandActivityConfig
{
	typedef std::map<int, std::pair<int, std::vector<ItemConfigData> > > STLVRewardMap;	// key--宠物强化等级, pair.first--seq1
public:
	RandActivityXinFuQiangHuaConfig();
	virtual ~RandActivityXinFuQiangHuaConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const std::vector<ItemConfigData>* GetRewardCfg(int pet_quality, int st_level, int* out_index = NULL)const;
	int GetStLevelBySeq(int pet_quality, int seq) const;
	bool IsOpen(int pet_quality, int role_level)const;
	const RAXinFuQiangHuaOtherCfg& GetOtherCfg()const { return m_other_cfg; }

private:
	int InitRewardCfg(TiXmlElement * RootElement);
	int InitOpenCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	std::map<int, STLVRewardMap> m_reward_cfg;	// key--宠物品质
	std::map<int, int> m_open_cfg;				// key--宠物品质, val--角色等级
	RAXinFuQiangHuaOtherCfg m_other_cfg;
};

#endif	