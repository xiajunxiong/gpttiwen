#ifndef __RAND_ACTIVITY_ZONG_XIANG_MAN_CHENG_CONFIG_HPP__
#define __RAND_ACTIVITY_ZONG_XIANG_MAN_CHENG_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/activitydef.hpp"

struct ZongXiangManChengBaseConfig
{
	ZongXiangManChengBaseConfig() :type(0), section_start(0), section_end(0), seq(0), light_type(ZONG_XIANG_TYPE_NO), light_condition(ZONG_XIANG_CONDITION_0), parameter(0), reward_group(0) {}

	enum ZONG_XIANG_TYPE
	{
		 ZONG_XIANG_TYPE_NO = 0,
		ZONG_XIANG_TYPE_1 = 1,//粽子
		ZONG_XIANG_TYPE_2 = 2,//盒子
	};

	enum ZONG_XIANG_CONDITION
	{
		ZONG_XIANG_CONDITION_0 = 0,//活跃度
		ZONG_XIANG_CONDITION_1 = 1,//点亮前三位置材料
	};

	int type;
	int section_start;
	int section_end;
	int	seq;
	int	light_type;
	int	light_condition;
	int	parameter;
	int	reward_group;
};

struct ZongXiangRewardConfig
{
	ZongXiangRewardConfig() : reward_group(0), rate(0) {}

	int reward_group;
	int rate;
	std::vector<ItemConfigData> m_reward_vec;
};

class RandActivityManager;
class RandActivityZongXiangManChengConfig : public RandActivityConfig
{
public:
	RandActivityZongXiangManChengConfig();
	virtual ~RandActivityZongXiangManChengConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);
	const ZongXiangManChengBaseConfig * GetBaseCfg(int open_day, int seq)const;
	const ZongXiangRewardConfig * GetRewardCfg(int reward_id)const;
private:
	int InitBaseCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	
	std::vector<ZongXiangManChengBaseConfig> m_base_vec;
	std::vector<ZongXiangRewardConfig> m_reward_vec;
};

#endif
