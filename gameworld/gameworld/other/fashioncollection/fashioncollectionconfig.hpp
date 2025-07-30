#ifndef __FASHION_COLLECTION_CONFIG_HPP__
#define __FASHION_COLLECTION_CONFIG_HPP__

#include "gamedef.h"
#include "servercommon/configcommon.h"
#include "servercommon/fashioncollectiondef.h"
#include "other/attributelist.hpp"

struct FashionCollectionLevelUpCfg
{
	FashionCollectionLevelUpCfg():fashion_level(0)
	{}

	int fashion_level;
	std::vector<AttrItemCfg> attr_list;
	std::vector<ItemConfigData> consume_list;
};

struct FashionCollectionStarAddCfg
{
	FashionCollectionStarAddCfg():level(0), star_level(0)
	{}

	int level;			// 专精等级
	int star_level;		// 所需星数
	std::vector<AttrItemCfg> attr_list;
};

struct FashionCollectionOtherCfg
{
	FashionCollectionOtherCfg(): max_level(0), max_star_level(0)
	{}

	int max_level;			// 单个时装最高等级
	int max_star_level;		// 最高星级
};


class FashionCollectionConfig : public ILogicConfig
{
	typedef std::map<int, std::vector<FashionCollectionLevelUpCfg> > LevelUpMap;	// key-外观id
	typedef std::map<int, std::vector<FashionCollectionStarAddCfg> > StarAddMap;	// key-职业

public:
	FashionCollectionConfig();
	virtual ~FashionCollectionConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const FashionCollectionLevelUpCfg* GetLevelUpCfg(int prof, int fashion_id, int level)const;
	const FashionCollectionStarAddCfg* GetStarCfg(int prof, int level)const;
	const FashionCollectionOtherCfg& GetOtherCfg()const { return m_other_cfg; }
	const std::pair<int, int>* GetDecomposeCfg(int item_id)const;

private:
	int InitOtherCfg(TiXmlElement* RootElement, std::string& err_msg);
	int InitFashionLevelUpCfg(TiXmlElement* RootElement, std::string& err_msg);
	int InitStarAddCfg(TiXmlElement* RootElement, std::string& err_msg);
	int InitDecomposeCfg(TiXmlElement* RootElement, std::string& err_msg);

	LevelUpMap m_level_up_cfg[PROFESSION_TYPE_NUM];		// 外观等级配置
	StarAddMap m_star_add_cfg;		// 总星级配置
	std::map<int, std::pair<int, int> > m_decompose_cfg;	// 外观分解 key-分解被消耗的道具id, pair<分解后获得物品, 分解后获得物品数量>
	FashionCollectionOtherCfg m_other_cfg;
};

#endif
