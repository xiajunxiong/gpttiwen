#ifndef __SAN_XING_GUI_DONG_CONFIG_HPP__
#define __SAN_XING_GUI_DONG_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include <map>
#include <vector>

struct SanXianLevelCfg
{
	SanXianLevelCfg() :level(0), grade(0) {}
	int level;
	int grade;
	std::vector<ItemConfigData> reward_vec;
};

struct SanXianOtherCfg
{
	SanXianOtherCfg() :chose_time(0), reward_time(0), scene_id(0), title_item_id(0), title_level(0), title_id(0){}
	int chose_time;
	int	reward_time;
	int scene_id;
	int title_item_id;
	int	title_level;
	int title_id;
	std::vector<ItemConfigData> reward_vec;
};

class SanXianConfig : public ILogicConfig
{
public:
	SanXianConfig();
	virtual ~SanXianConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const SanXianOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const SanXianLevelCfg * GetLevelCfg(int level);
private:
	int InitLevelCfg(TiXmlElement *RootElement);
	//int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	SanXianOtherCfg m_other_cfg;
	std::vector<SanXianLevelCfg> m_level_vec;
};

#endif // !__COLLECTION_CONFIG_HPP__
