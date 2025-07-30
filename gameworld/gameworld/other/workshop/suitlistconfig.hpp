#ifndef __SUIT_LIST_CONFIG_HPP__
#define __SUIT_LIST_CONFIG_HPP__

#include "gamedef.h"
#include "servercommon/configcommon.h"

#include <map>
#include <vector>

const static int MAX_SUIT_LIST_NUM = 1000;//最大套装随机组
const static int MAX_SUIT_EFFECT_LIST_NUM = 5;//套装效果最大件数5
const static int MAX_SUIT_EFFECT_VALUE_NUM = 3;//套装效果属性最大类型3

struct SuitGroup
{
	SuitGroup() : id(0), rate(0) {}
	int id;
	int rate;
};

struct SuitListGroup
{
	SuitListGroup() : rate_weight(0) {}
	
	int rate_weight;
	std::vector<SuitGroup> m_suit_group;
};

struct SuitValue
{
	SuitValue() : attr_type(0), attr_add(0) {}
	int attr_type;
	int attr_add;
};

struct SuitEffect
{
	SuitEffect() : sp_id(0), suit_compose_1(0) {}
	int sp_id;

	int suit_compose_1;
	SuitValue m_value[MAX_SUIT_EFFECT_VALUE_NUM];
};

struct SuitEffectList
{
	SuitEffectList() :id(0), suit_num(0), color(0) {}

	int id;
	int suit_num;
	int color;
	SuitEffect m_effect_list[MAX_SUIT_EFFECT_LIST_NUM];
};

struct SuitEffectColorList
{
	SuitEffectColorList() {}
	std::map<int, SuitEffectList> m_effect;
};

class SuitListConfig : public ILogicConfig
{
public:
	SuitListConfig();
	virtual ~SuitListConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const SuitListGroup * GetSuitListGroup(int group_id);
	const SuitEffect * GetSuitEffectList(int suit_id, int color, int suit_num);
	int GetNewSuitByOldSuit(int old_suit_id);
private:
	int InitSuitGroupCfg(TiXmlElement * RootElement);
	int InitSuitValueCfg(TiXmlElement * RootElement);
	int InitOldSuitChangeNewCfg(TiXmlElement * RootElement);

	std::map<int, SuitListGroup> m_suit_list_map;//key - suit_group;
	std::map<int, SuitEffectColorList> m_suit_effect_map;//key - suit_id;
	std::map<int, int> m_suit_combine_map;
};

#endif