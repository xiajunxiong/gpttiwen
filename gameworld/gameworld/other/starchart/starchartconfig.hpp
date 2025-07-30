#ifndef __STAR_CHART_CONFIG_HPP__
#define __STAR_CHART_CONFIG_HPP__

#include <map>
#include <set>

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/starchartdef.h"

struct BaseStarChartCfg
{
	BaseStarChartCfg():star_level(0), cost_coin(0)
	{}

	virtual ~BaseStarChartCfg() {}

	int star_level;
	int cost_coin;
	std::vector<ItemConfigData> cost_item_list;
	std::vector<ItemConfigData> reward_item_list;
};

struct RoleStarChartCfg : public BaseStarChartCfg
{
	RoleStarChartCfg()
	{}
	virtual ~RoleStarChartCfg() {}

	const AttrInfo* GetAttr(int prof) const
	{
		std::map<int, AttrInfo>::const_iterator it = prof_attrs.find(prof);
		if (it != prof_attrs.end()) return &it->second;
		return NULL;
	}

	std::map<int, AttrInfo> prof_attrs;		// key-职业
};

struct PetOrPartnerStarChartCfg : public BaseStarChartCfg
{
	PetOrPartnerStarChartCfg()
	{}

	virtual ~PetOrPartnerStarChartCfg() {}

	AttrInfo attr;
};

// 超星系点阵
struct SuperGalaxiesCfg : public BaseStarChartCfg
{
	SuperGalaxiesCfg() :target(0), star_type(0)
	{}
	virtual ~SuperGalaxiesCfg() {}

	int target;					// 作用对象, STAR_CHART_TYPE
	int star_type;				// 星位类型, SUPER_GALAXIESCFG_TYPE
	std::pair<int, int> param;	// 如果是属性星位:param1-属性类型 param2-属性值, 如果是被动技能星位:param1-技能id, param2-技能等级
};

struct StarChartConnectCfg
{
	StarChartConnectCfg():condition_level(0)
	{}

	int condition_level;		// 激活所需的相邻星位等级
	std::set<int> adjoin;
};


struct StarChartOtherCfg
{
	StarChartOtherCfg():reset_back(0), reset_cion1(0), reset_gold2(0)
	{}

	int reset_back;		// 重置返还铜币比例，千分比
	int reset_cion1;	// 重置小星图消耗铜币
	int reset_gold2;	// 重置超星图消耗铜币(策划后续修改为元宝)
};

///////////////////////////////////////////////////////////////////////////////////////
class StarChartConfig : public ILogicConfig
{
	typedef std::map<int, std::map<int, std::vector<BaseStarChartCfg*> > > StarChartTypeMap;	// key1-星图阶数 key2-星位id(星位索引), val-星图等级信息
	typedef std::map<int, std::map<int, StarChartConnectCfg> > StarChartConnectMap;				// key1-星图阶数 key2-星图索引, val-星位信息
	typedef std::map<int, StarChartConnectCfg>	SuperGalaxiesConnectMap;						// key-星图索引, val-星位信息
	typedef std::map<int, std::vector<SuperGalaxiesCfg> > SuperGalaxiesMap;						// key-星位索引,val-星位等级配置

public:
	StarChartConfig();
	virtual ~StarChartConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	bool ValidStarChartPoint(int star_chart_type, int star_chart_stage, int star_id) const;
	bool ValidSuperGalaxiesPoint(int star_id) const;

	const BaseStarChartCfg* GetStarChartCfg(int star_chart_type, int star_chart_stage, int star_id, int star_level)const;
	const SuperGalaxiesCfg* GetSuperGalaxiesCfg(int star_id, int star_level)const;
	const StarChartConnectCfg* GetStarChartConnectCfg(int star_chart_type, int star_chart_stage, int star_id) const;
	const StarChartConnectCfg* GetSuperGalaxiesConnectCfg(int star_id) const;
	const std::set<int>* GetSkillHoleOpenCfg(int star_chart_type, int skill_hole) const;	// 获取解锁技能孔所需节点

	const StarChartOtherCfg& GetOtherCfg()const { return m_other_cfg; }

private:
	int InitRoleStarChartCfg(TiXmlElement * RootElement, std::string& errormsg);		// 角色星图
	int InitPetStarChartCfg(TiXmlElement * RootElement, std::string& errormsg);			// 宠物星图
	int InitPartnerStarChartCfg(TiXmlElement * RootElement, std::string& errormsg);		// 伙伴星图
	int InitRoleStarChartConnectCfg(TiXmlElement * RootElement, std::string& errormsg);	// 角色星图连线
	int InitPartnerStarChartConnectCfg(TiXmlElement * RootElement, std::string& errormsg);	// 伙伴星图连线
	int InitPetStarChartConnectCfg(TiXmlElement * RootElement, std::string& errormsg);	// 宠物星图连线
	int InitStarChartSkillCfg(TiXmlElement * RootElement, std::string& errormsg);		// 技能孔开启
	int InitSuperGalaxiesCfg(TiXmlElement * RootElement, std::string& errormsg);		// 超星系星位
	int InitSuperGalaxiesConnectCfg(TiXmlElement * RootElement, std::string& errormsg);	// 超星系连线
	int InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg);				// 其他

	void Destroy();

	StarChartTypeMap m_star_chart_cfg[STAR_CHART_TYPE_MAX];				// 星图点阵
	StarChartConnectMap m_star_chart_adjoin_cfg[STAR_CHART_TYPE_MAX];	// 星图邻接
	std::map<int, std::set<int> > m_skill_hole_cfg[STAR_CHART_TYPE_MAX];	// 星图技能孔开启, key-技能孔位, set<int>(需要解锁的星位索引)

	SuperGalaxiesMap m_super_galaxies_cfg;		// 超星系点位
	SuperGalaxiesConnectMap m_super_galaxies_adjoin_cfg;	// 超星系邻接

	StarChartOtherCfg m_other_cfg;
};


#endif
