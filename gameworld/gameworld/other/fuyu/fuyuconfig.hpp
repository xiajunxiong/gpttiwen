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

	int attri_num;		// ��������
	std::vector<AttrInfo> attrs;	// ��������&��ֵ
};

struct FuYuSPCfg
{
	FuYuSPCfg():sp_level(0), limit_type(0), param1(0), sp_id(0)
	{}

	int sp_level;		// ��Ч�Ƚ�
	int limit_type;		// �������� // FU_YU_SP_LIMIT_TYPE(fuyudef.h)
	int param1;			// ��Ч����
	int sp_id;			// ��Чid
};

struct FuYuDecomposeCfg
{
	FuYuDecomposeCfg():common_fuyu_decompose(0), hunyuan_fuyu_decompose(0), huashen_fuyu_decompose(0)
	{}

	int common_fuyu_decompose;		// ������Ч����ֽ�������
	int hunyuan_fuyu_decompose;		// ��Ԫ����ֽ�������
	int huashen_fuyu_decompose;		// �������ֽ�������
};

struct FuYuOtherCfg
{
	FuYuOtherCfg(): switch_consume_item(0)
	{}

	ItemID switch_consume_item;
};

class FuYuConfig : public ILogicConfig
{
	typedef std::map<int, int> LvProbMap;	//key-Ҫ��ǿ���ķ���Ƚ�, value-ǿ���ɹ�����
	typedef std::vector<FuYuSPCfg> SPList;
public:
	FuYuConfig();
	virtual ~FuYuConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const FuYuUpgradeCfg* GetUpgradeCfg(int fuyu_rank)const;		
	bool IsStrengthenSucc(int materials_id, int target_fuyu_level) const;	//�ж��Ƿ�ǿ���ɹ�
	int RandSpGroup()const;		// �����Ч��
	const FuYuSPCfg* GetSpByGroup(int sp_group, int fuyu_level)const;
	const FuYuOtherCfg& GetOtherCfg() { return m_other_cfg; }
	bool IsvalidSpId(int sp_id)const;		// �ж���Чid�Ƿ���Ч
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
	int InitBelongCfg(TiXmlElement* RootElement, std::string& err);	// ר������
	int InitProtectCfg(TiXmlElement* RootElement, std::string& err);
	int InitOtherCfg(TiXmlElement* RootElement, std::string& err);
	int InitDecomposeCfg(TiXmlElement* RootElement, std::string& err);


	std::map<int, FuYuUpgradeCfg> m_fuyu_upgrade_cfg;	// key-�������(rank), �����������
	std::map<int, LvProbMap> m_strengthen_cfg;	// key-�������id, ǿ������
	std::map<int, int> m_materialsId_materials_level_map;	// key-�������id, value-������ϵȽ�
	std::map<int, int> m_rank_itemid_cfg;	// key-����, value-����id
	std::map<int, SPList> m_sp_cfg;			// key-��Ч��, ����Ч��

	std::map<int, std::pair<int, int> > m_belong_fuyu_cfg;		// ר���������� <����id, <����Ƚ�, ��Чid> >

	std::vector<int> m_sp_group_list;	// ��Ч���б�
	std::set<int> m_sp_id_list;			// ��Чid�б�

	std::map<int, std::pair<int, int> > m_protect_cfg;	// ǿ���������ĵ���, key-����Ƚ�, pair<����id, ����>

	std::map<int, FuYuDecomposeCfg> m_decompose_cfg;	// ����ֽ�, key-����id
	
	FuYuOtherCfg m_other_cfg;
};


#endif