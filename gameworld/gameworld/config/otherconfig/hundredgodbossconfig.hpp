#ifndef __HUNDRED_GOD_BOSS_CONFIG_HPP__
#define __HUNDRED_GOD_BOSS_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "gamedef.h"

static const int MAX_HUNDRED_GOD_BOSS_TARGET_NUM = 16;
UNSTD_STATIC_CHECK(MAX_HUNDRED_GOD_BOSS_TARGET_NUM == sizeof(short) * 8);
//目前存储用的标识是short

enum HUNDRED_GOD_BOSS_COND_TYPE
{
	HUNDRED_GOD_BOSS_COND_TYPE_INVALID = 0,
	HUNDRED_GOD_BOSS_COND_TYPE_LEVEL = 1,			//达到对应等级		param:等级
	HUNDRED_GOD_BOSS_COND_TYPE_TASK = 2,			//完成任务			param:任务ID
	HUNDRED_GOD_BOSS_COND_TYPE_BOUNTY_TASK = 3,		//完成十环门派任务	param:次数

	HUNDRED_GOD_BOSS_COND_TYPE_MAX
};


struct HundredGodBossTargetCfg
{
	HundredGodBossTargetCfg() : cond_type(0), param(0)
	{}

	int cond_type;
	int param;
	ItemConfigData reward;
};

struct HundredGodBossOtherCfg
{
	HundredGodBossOtherCfg() : pre_task(0), during_time(0), partner_id(0)
	{}

	int pre_task;				//开启前置任务
	int during_time;			//开启时持续时间(hour)
	int partner_id;				//完成解锁的伙伴ID
};

class HundredGodBossConfig : public ILogicConfig
{
public:
	HundredGodBossConfig();
	virtual ~HundredGodBossConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const HundredGodBossTargetCfg * GetTargetCfg(int target_id);
	void GetAllTargetCfg(std::vector<HundredGodBossTargetCfg> & list);
	const HundredGodBossOtherCfg & GetOtherCfg() { return m_other_cfg; }

private:
	int InitTargetCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg);

	
	int m_target_count;
	HundredGodBossTargetCfg m_target_list[MAX_HUNDRED_GOD_BOSS_TARGET_NUM];
	HundredGodBossOtherCfg m_other_cfg;
};


#endif