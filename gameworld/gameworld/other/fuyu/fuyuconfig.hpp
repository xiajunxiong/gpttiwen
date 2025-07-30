#ifndef __FUYU_CONFIG_HPP__
#define __FUYU_CONFIG_HPP__

#include <vector>
#include <map>
#include <set>
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/configcommon.h"
#include "servercommon/fuyudef.h"
#include "other/attributelist.hpp"
#include "item/itembase.h"

struct FuYuUpgradeCfg
{
	FuYuUpgradeCfg():attri_num(0)
	{}

	int attri_num;		// 属性条数
	std::vector<AttrInfo> attrs;	// 属性类型&数值
};

struct FuYuSPCfg
{
	FuYuSPCfg():sp_level(0), limit_type(0), param1(0), sp_id(0)
	{}

	int sp_level;		// 特效等阶
	int limit_type;		// 限制类型 // FU_YU_SP_LIMIT_TYPE(fuyudef.h)
	int param1;			// 生效条件
	int sp_id;			// 特效id
};

struct FuYuDecomposeCfg
{
	FuYuDecomposeCfg():common_fuyu_decompose(0), hunyuan_fuyu_decompose(0), huashen_fuyu_decompose(0)
	{}

	int common_fuyu_decompose;		// 单个特效符玉分解获得灵玉
	int hunyuan_fuyu_decompose;		// 混元符玉分解获得灵玉
	int huashen_fuyu_decompose;		// 化神符玉分解获得灵玉
};

struct FuYuOtherCfg
{
	FuYuOtherCfg(): switch_consume_item(0)
	{}

	ItemID switch_consume_item;
};

class FuYuConfig : public ILogicConfig
{
	typedef std::map<int, int> LvProbMap;	//key-要被强化的符玉等阶, value-强化成功概率
	typedef std::vector<FuYuSPCfg> SPList;
public:
	FuYuConfig();
	virtual ~FuYuConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const FuYuUpgradeCfg* GetUpgradeCfg(int fuyu_rank)const;		
	bool IsStrengthenSucc(int materials_id, int target_fuyu_level) const;	//判断是否强化成功
	int RandSpGroup()const;		// 随机特效组
	const FuYuSPCfg* GetSpByGroup(int sp_group, int fuyu_level)const;
	const FuYuOtherCfg& GetOtherCfg() { return m_other_cfg; }
	bool IsvalidSpId(int sp_id)const;		// 判断特效id是否有效
	ItemID GetNextIdByRank(int fuyu_level) const;
	int GetLevelByMaterials(int materials_id)const;
	int GetSpGroupBySpId(int sp_id)const;
	const FuYuSPCfg* GetLowSpIdByGroup(int sp_group) const;
	const std::pair<int, int>* GetProtectCfg(int fuyu_level) const;

	bool IsBelongFuYu(ItemID fuyu_id)const;
	int GetSpLevelById(int sp_id)const;
	bool IsBelongSpid(int sp_id)const;
	const FuYuDecomposeCfg* GetDecomposeCfg(int item_id)const;

	bool RandFuYuParam(NetValueItemParam * param_data, ItemID item_id, int sp_group = 0);

private:
	int InitTargetCfg(TiXmlElement* RootElement, std::string& err);
	int InitStrengthenCfg(TiXmlElement* RootElement, std::string& err);
	int InitSpGroupCfg(TiXmlElement* RootElement, std::string& err);
	int InitBelongCfg(TiXmlElement* RootElement, std::string& err);	// 专属符玉
	int InitProtectCfg(TiXmlElement* RootElement, std::string& err);
	int InitOtherCfg(TiXmlElement* RootElement, std::string& err);
	int InitDecomposeCfg(TiXmlElement* RootElement, std::string& err);


	std::map<int, FuYuUpgradeCfg> m_fuyu_upgrade_cfg;	// key-符玉阶数(rank), 符玉基础属性
	std::map<int, LvProbMap> m_strengthen_cfg;	// key-符玉材料id, 强化概率
	std::map<int, int> m_materialsId_materials_level_map;	// key-符玉材料id, value-符玉材料等阶
	std::map<int, int> m_rank_itemid_cfg;	// key-阶数, value-符玉id
	std::map<int, SPList> m_sp_cfg;			// key-特效组, 特殊效果

	std::map<int, std::pair<int, int> > m_belong_fuyu_cfg;		// 专属符玉配置 <符玉id, <符玉等阶, 特效id> >

	std::vector<int> m_sp_group_list;	// 特效组列表
	std::set<int> m_sp_id_list;			// 特效id列表

	std::map<int, std::pair<int, int> > m_protect_cfg;	// 强化保护消耗道具, key-符玉等阶, pair<道具id, 数量>

	std::map<int, FuYuDecomposeCfg> m_decompose_cfg;	// 符玉分解, key-符玉id
	
	FuYuOtherCfg m_other_cfg;
};


#endif