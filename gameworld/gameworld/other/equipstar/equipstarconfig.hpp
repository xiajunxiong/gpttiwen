#ifndef _EQUIP_STAR_CONFIG_HPP_
#define _EQUIP_STAR_CONFIG_HPP_

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "item/itembase.h"
#include <map>
#include <vector>

struct EquipStarAttr
{
	EquipStarAttr() :attr_type(0), attr_min(0), attr_max(0) {}
	int attr_type;
	int attr_min;
	int attr_max;
};

struct EquipStarCfg
{
	EquipStarCfg() :id(0), item_star(0), return_times(0) {}
	static const int ATTR_NUM_MAX = 6;
	static const int PRICE_RATE = 1000;//千分比
	UNSTD_STATIC_CHECK(JewelryParam::ATTR_NUM_MAX == ATTR_NUM_MAX);
	UNSTD_STATIC_CHECK(EquipmentParam::ATTR_NUM_MAX == ATTR_NUM_MAX);

	int id;
	int	item_star;
	
	std::vector<int> trade_min_gold_price;
	std::vector<int> trade_max_gold_price;
	int	return_times;

	EquipStarAttr attr_voc[ATTR_NUM_MAX];
};

struct EquipStarGroupCfg
{
	EquipStarGroupCfg() {}
	int group_id;
	int	item_star;
	int	rate;
};

struct EquipStarUpCfg
{
	EquipStarUpCfg() : star_limit(0), level_limit(0), color_limit(0), compose_id(0), item_star(0) {}
	int	star_limit;
	int	level_limit;
	int	color_limit;
	int	compose_id;
	int	item_star;
};

class EquipStarConfig : public ILogicConfig
{
public:
	EquipStarConfig();
	virtual ~EquipStarConfig();

	virtual bool Init(const std::string & configname, std::string * err);
	const EquipStarGroupCfg * RandStarLevelCfg(int item_id);
	const EquipStarCfg * GetEquipStarAttrCfg(int item_id, int level);
	const EquipStarUpCfg * GetEquipStarUpCfg(int seq);
private:
	int InitEquipStarCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitEquipStarGroupCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitEquipStarUpCfg(TiXmlElement * RootElement, std::string& errormsg);

	std::vector<EquipStarCfg> equip_star_vec;

	std::map<int, int> equip_star_weight_map;// key 对应group_id 值对应group_id总权重
	std::map<int, std::vector<EquipStarGroupCfg *> > equip_star_group_map;//key 对应group_id 值是对应list
	std::vector<EquipStarGroupCfg> equip_star_group_vec;

	std::map<int, EquipStarUpCfg> m_star_up_map;	// key 合成seq
};



#endif // _WUSHUANGDIANCONFIG_HPP_