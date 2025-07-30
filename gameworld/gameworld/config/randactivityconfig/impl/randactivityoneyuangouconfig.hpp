#ifndef __RAND_ACTIVITY_ONE_YUAN_GOUI_HPP__
#define __RAND_ACTIVITY_ONE_YUAN_GOUI_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "item/itembase.h"

#include <vector>
#include <set>
#include <map>

class RandActivityManager;


struct RAOneYuanGouItemCfg
{
	RAOneYuanGouItemCfg()
	{
		type = 0;
		seq = 0;
		level_down = 0;
		level_up = 0;
	}

	int type;
	int seq;
	int level_down;
	int level_up;

	std::vector<ItemConfigData> rewards;
};

struct RAOneYuanGouSectionCfg
{
	RAOneYuanGouSectionCfg() :section_start(0), section_end(0)
	{
	}

	int section_start;
	int section_end;
	std::vector<RAOneYuanGouItemCfg> item;
};

struct RAOneYuanGouOtherCfg
{
	RAOneYuanGouOtherCfg() 
	{
		cfg_ver = 0;
		need_chong_zhi_gold = 0;
	}

	int cfg_ver;
	int need_chong_zhi_gold;
};

class RandActivityManager;
class RandActivityOneYuanGouConfig : public RandActivityConfig
{
public:
	RandActivityOneYuanGouConfig();
	virtual ~RandActivityOneYuanGouConfig();
	virtual bool Init(TiXmlElement * RootElement, std::string * err);

	const RAOneYuanGouItemCfg * GetRAOneYuanGouItemCfg(RandActivityManager * ramgr, int role_level) const;
	const RAOneYuanGouOtherCfg & GetOtherCfg()const;
	virtual int GetConfigVer() const;
private:
	int InitBuyCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	std::vector<RAOneYuanGouSectionCfg> m_section_cfg;
	RAOneYuanGouOtherCfg m_other_cfg;
};

#endif	