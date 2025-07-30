#ifndef __MYSTERY_STORE_CONFIG_HPP__
#define __MYSTERY_STORE_CONFIG_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/partnerdef.h"
#include <vector>
#include <map>
#include <set>

struct MysteryStoreOtherCfg
{
	MysteryStoreOtherCfg() : auto_time(0), mystery_store_ver(0)
	{}

	int auto_time;
	int mystery_store_ver;		//版本号,变更则刷新
};

struct LevelLimit
{
	LevelLimit(int _level_min = 0, int _level_max = 0) : level_min(_level_min), level_max(_level_max) {}

	int level_min;
	int level_max;

	bool operator < (const LevelLimit & tmp) const
	{
		return this->level_min < tmp.level_min;
	}
};
struct MysteryStoreStoreCfg
{
	MysteryStoreStoreCfg() : item_group(0) {}

	int item_group;
};

struct MysteryStoreItemGroupCfg
{
	MysteryStoreItemGroupCfg() : seq(0), rate(0), price_type(0), price(0), limit_buy(0) {}

	int seq;
	int rate;
	int price_type;
	int price;
	int limit_buy;
	ItemConfigData item;
};

struct MysterStoreConsumeAddCfg
{
	MysterStoreConsumeAddCfg() : refresh_money_type(0), refresh_price(0), is_priority_use_item(0), manual_refresh_times(0)
	{}

	int refresh_money_type;			//货币类型	
	int refresh_price;				//货币数量
	int is_priority_use_item;		//是否道具优先
	ItemConfigData consume_item;	
	int manual_refresh_times;		//手动刷新次数
};

class MysteryStoreConfig : public ILogicConfig
{
public:
	MysteryStoreConfig();
	virtual ~MysteryStoreConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const int GetStoreCfgSize(int level);
	const std::vector<MysteryStoreStoreCfg> * GetStoreCfg(int level);
	const MysteryStoreItemGroupCfg * RandGetItemCfg(int item_group, int pass_level);
	const MysteryStoreItemGroupCfg * GetItemCfg(int group_id, int seq);
	const MysteryStoreOtherCfg * GetOtherCfg() { return &m_other_cfg; }
	const MysterStoreConsumeAddCfg * GetConsumeAddCfg(int refresh_time);

private:
	int InitMysteryStoreStoreCfg(TiXmlElement *RootElement);
	int InitMysteryStoreItemCfg(TiXmlElement *RootElement);
	int InitMysteryStoreOtherCfg(TiXmlElement *RootElement);
	int InitMysteryStoreConAddCfg(TiXmlElement *RootElement);
	int InitMysteryStoreLimitCfg(TiXmlElement *RootElement);

	std::map<LevelLimit, std::vector<MysteryStoreStoreCfg> > m_store_cfg;
	std::map<int, std::vector<MysteryStoreItemGroupCfg> > m_item_group_cfg;			//key - item_group_id
	MysteryStoreOtherCfg m_other_cfg;
	std::vector<MysterStoreConsumeAddCfg> m_consume_add_cfg;
	std::map<int, int > m_mystery_store_limit_cfg;

};

#endif // __MYSTERY_STORE_CONFIG_HPP__
