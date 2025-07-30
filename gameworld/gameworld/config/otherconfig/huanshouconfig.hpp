#ifndef __HUAN_SHOU_CONFIG_HPP__
#define __HUAN_SHOU_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/huanshoudef.hpp"
#include "servercommon/attributedef.hpp"
#include <set>

struct HuanShouOtherCfg
{
	HuanShouOtherCfg() : potential_item_id(0), exchange_price(0), wait_time(0), liandan_max(0), lock_item_id(0)
	{
		memset(need_skill_num_list, 0, sizeof(need_skill_num_list));
		memset(need_item_num_list, 0, sizeof(need_item_num_list));
	}

	int potential_item_id;		//潜能果道具ID
	int exchange_price;			//丹炉炼化消耗
	int wait_time;				//炼化所需等待时间
	int liandan_max;			//炼丹炉最大容量
	int lock_item_id;			//技能锁定所需消耗道具ID
	int need_skill_num_list[2];	//技能锁所需已解锁技能数
	int need_item_num_list[2];	//锁孔位所需消耗道具数
};

struct HuanShouBaseCfg
{
	HuanShouBaseCfg() : quality(0), convert_item_id(0), attr_group_id(0), potential_group_id(0), skill_lattice(0)
	{}

	int quality;
	int convert_item_id;		//已激活则自动转化为该道具
	int attr_group_id;			//属性组ID
	int potential_group_id;		//潜能组ID
	int skill_lattice;			//打书技能上限
	std::vector<ItemConfigData> fenjie_item_list;		//分解转化道具可得
};

struct HuanShouAttrGroupLevelCfg
{
	HuanShouAttrGroupLevelCfg() : level(0), skill_id(0), skill_level(0), cd_start(0), cd_perform(0), attr_count(0)
	{
		memset(attr_list, 0, sizeof(attr_list));
	}

	static const int MAX_ATTR_NUM = 2;

	int level;
	int skill_id;
	int skill_level;
	int cd_start;
	int cd_perform;
	int attr_count;
	AttrItemCfg attr_list[MAX_ATTR_NUM];
	std::vector<ItemConfigData> consume_list;
};

struct HuanShouAttrGroupCfg
{
	HuanShouAttrGroupCfg() : max_str_level(0)
	{}

	int max_str_level;
	HuanShouAttrGroupLevelCfg level_list[NAX_HUAN_SHOU_STR_LEVEL_NUM];
};

static const int MAX_HUAN_SHOU_EGG_RATE_NUM = 10000;		//幻兽蛋固定权重总和
struct HuanShouEggCfg
{
	HuanShouEggCfg() : huanshou_id(0), rate(0), is_broadcast(false)
	{}

	int huanshou_id;
	int rate;
	bool is_broadcast;
	bool is_rare;			// 是否稀有
};

struct HuanShouEggOpenCfg
{
	HuanShouEggOpenCfg() : egg_id(0), item_id(0), rare_guarantee(0)
	{}

	int egg_id;				//幻兽蛋ID
	int item_id;			//幻兽蛋对应道具ID
	int rare_guarantee;		//稀有保底次数
};

struct HuanShouPointUnlockCfg
{
	HuanShouPointUnlockCfg() : need_active_level(0)
	{}

	int need_active_level;				//激活所需相邻点位等级
	std::set<int> adjoin_list;			//相邻点位,判断是否可激活时,只需看相邻中是否存在满足等级的点位
};

enum HUAN_SHOU_POINT_TIAN_FU_TYPE
{
	HUAN_SHOU_POINT_TIAN_FU_TYPE_ATTR = 0,				//属性 p1:属性类型 p2:属性值
	HUAN_SHOU_POINT_TIAN_FU_TYPE_PASSIVE_SKILL = 1,		//被动技能 p1:技能ID p2:技能等级

	HUAN_SHOU_POINT_TIAN_FU_TYPE_MAX
};

struct HuanShouPotentialPointLevelCfg
{
	HuanShouPotentialPointLevelCfg() : level(0), tianfu_type(0), param1(0), param2(0)
	{}

	int level;
	int tianfu_type;
	int param1;
	int param2;
	std::vector<ItemConfigData> consume_list;
};

struct HuanShouPotentialPointGroupCfg
{
	HuanShouPotentialPointGroupCfg()
	{
		memset(max_level_list, 0, sizeof(max_level_list));
	}

	int max_level_list[MAX_HUAN_SHOU_POTENTIAL_POINT];		//潜能点位最大等级列表
	HuanShouPotentialPointLevelCfg point_list[MAX_HUAN_SHOU_POTENTIAL_POINT][MAX_HUAN_SHOU_POINT_LEVEL_NUM];
};

class HuanShouConfig : public ILogicConfig
{
public:
	HuanShouConfig();
	virtual ~HuanShouConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const HuanShouOtherCfg & GetOtherCfg() { return m_other_cfg; }
	int GetBaseCount() const { return m_base_count; }
	const HuanShouBaseCfg * GetBaseCfg(int huanshou_id) const;
	const std::vector<ItemConfigData> * GetDecomposeCfg(int item_id) const;
	const HuanShouAttrGroupCfg * GetAttrGroupCfg(int attr_group_id) const;
	const HuanShouAttrGroupLevelCfg * GetAttrGroupLevelCfg(int attr_group_id, int str_level) const;
	const HuanShouAttrGroupLevelCfg * GetAttrGroupLevelCfgByLevel(int attr_group_id, int str_level) const;
	int GetEggCount() const { return m_huan_shou_egg_count; }
	const HuanShouEggOpenCfg * GetHuanShouEggOpenCfg(int item_id) const;
	const HuanShouEggCfg * GetRandHuanShouEggCfgByEgg(int egg_id) const;
	const HuanShouEggCfg * GetRandRareHuanShouEggCfgByEgg(int egg_id) const;
	bool IsCanAddNewSkill(int has_hole_count)const;

	int GetPointCount() const { return m_point_count; }
	const HuanShouPointUnlockCfg * GetPointUnlockCfg(int point_index) const;
	const HuanShouPotentialPointLevelCfg * GetPointLevelCfg(int point_group_id, int point_index, int level)const;

private:
	int InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitBaseCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitAttrGroupCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitEggItemCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitEggCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitSkillHoleCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitPointUnlcokCfg(TiXmlElement * RootElement, std::string& errormsg);
	int InitPointGroupCfg(TiXmlElement * RootElement, std::string& errormsg);


	HuanShouOtherCfg m_other_cfg;
	int m_base_count;											//幻兽基础配置列表数量
	HuanShouBaseCfg m_base_cfg_list[MAX_HUAN_SHOU_NUM];			//幻兽基础配置
	std::map<int, std::vector<ItemConfigData> > m_decompose_map;//幻兽分解配置 key:item_id
	std::map<int, HuanShouAttrGroupCfg> m_attr_group_map;		//幻兽属性配置(强化) key:attr_group_id

	int m_huan_shou_egg_count;									//列表数量
	HuanShouEggOpenCfg m_huan_shou_egg_item_list[MAX_HUAN_SHOU_EGG_ID_NUM];		//幻兽蛋道具开启配置
	std::vector<HuanShouEggCfg> m_huan_shou_egg_list[MAX_HUAN_SHOU_EGG_ID_NUM];	//幻兽蛋配置

	std::map<int, int> m_skill_hole_map;						//技能孔位配置

	int m_point_count;	//该潜能点位数量同时也是潜能点位组中点位数量
	HuanShouPointUnlockCfg m_point_unlock_list[MAX_HUAN_SHOU_POTENTIAL_POINT];	//幻兽潜能点位解锁配置

	int m_ppg_count;
	HuanShouPotentialPointGroupCfg m_ppg_list[MAX_HUAN_SHOU_POTENTIAL_POINT_GROUP_NUM];	//潜能点位组配置
};

#endif	//__HUAN_SHOU_CONFIG_HPP__