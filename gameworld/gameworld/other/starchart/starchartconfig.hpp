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

	std::map<int, AttrInfo> prof_attrs;		// key-ְҵ
};

struct PetOrPartnerStarChartCfg : public BaseStarChartCfg
{
	PetOrPartnerStarChartCfg()
	{}

	virtual ~PetOrPartnerStarChartCfg() {}

	AttrInfo attr;
};

// ����ϵ����
struct SuperGalaxiesCfg : public BaseStarChartCfg
{
	SuperGalaxiesCfg() :target(0), star_type(0)
	{}
	virtual ~SuperGalaxiesCfg() {}

	int target;					// ���ö���, STAR_CHART_TYPE
	int star_type;				// ��λ����, SUPER_GALAXIESCFG_TYPE
	std::pair<int, int> param;	// �����������λ:param1-�������� param2-����ֵ, ����Ǳ���������λ:param1-����id, param2-���ܵȼ�
};

struct StarChartConnectCfg
{
	StarChartConnectCfg():condition_level(0)
	{}

	int condition_level;		// ���������������λ�ȼ�
	std::set<int> adjoin;
};


struct StarChartOtherCfg
{
	StarChartOtherCfg():reset_back(0), reset_cion1(0), reset_gold2(0)
	{}

	int reset_back;		// ���÷���ͭ�ұ�����ǧ�ֱ�
	int reset_cion1;	// ����С��ͼ����ͭ��
	int reset_gold2;	// ���ó���ͼ����ͭ��(�߻������޸�ΪԪ��)
};

///////////////////////////////////////////////////////////////////////////////////////
class StarChartConfig : public ILogicConfig
{
	typedef std::map<int, std::map<int, std::vector<BaseStarChartCfg*> > > StarChartTypeMap;	// key1-��ͼ���� key2-��λid(��λ����), val-��ͼ�ȼ���Ϣ
	typedef std::map<int, std::map<int, StarChartConnectCfg> > StarChartConnectMap;				// key1-��ͼ���� key2-��ͼ����, val-��λ��Ϣ
	typedef std::map<int, StarChartConnectCfg>	SuperGalaxiesConnectMap;						// key-��ͼ����, val-��λ��Ϣ
	typedef std::map<int, std::vector<SuperGalaxiesCfg> > SuperGalaxiesMap;						// key-��λ����,val-��λ�ȼ�����

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
	const std::set<int>* GetSkillHoleOpenCfg(int star_chart_type, int skill_hole) const;	// ��ȡ�������ܿ�����ڵ�

	const StarChartOtherCfg& GetOtherCfg()const { return m_other_cfg; }

private:
	int InitRoleStarChartCfg(TiXmlElement * RootElement, std::string& errormsg);		// ��ɫ��ͼ
	int InitPetStarChartCfg(TiXmlElement * RootElement, std::string& errormsg);			// ������ͼ
	int InitPartnerStarChartCfg(TiXmlElement * RootElement, std::string& errormsg);		// �����ͼ
	int InitRoleStarChartConnectCfg(TiXmlElement * RootElement, std::string& errormsg);	// ��ɫ��ͼ����
	int InitPartnerStarChartConnectCfg(TiXmlElement * RootElement, std::string& errormsg);	// �����ͼ����
	int InitPetStarChartConnectCfg(TiXmlElement * RootElement, std::string& errormsg);	// ������ͼ����
	int InitStarChartSkillCfg(TiXmlElement * RootElement, std::string& errormsg);		// ���ܿ׿���
	int InitSuperGalaxiesCfg(TiXmlElement * RootElement, std::string& errormsg);		// ����ϵ��λ
	int InitSuperGalaxiesConnectCfg(TiXmlElement * RootElement, std::string& errormsg);	// ����ϵ����
	int InitOtherCfg(TiXmlElement * RootElement, std::string& errormsg);				// ����

	void Destroy();

	StarChartTypeMap m_star_chart_cfg[STAR_CHART_TYPE_MAX];				// ��ͼ����
	StarChartConnectMap m_star_chart_adjoin_cfg[STAR_CHART_TYPE_MAX];	// ��ͼ�ڽ�
	std::map<int, std::set<int> > m_skill_hole_cfg[STAR_CHART_TYPE_MAX];	// ��ͼ���ܿ׿���, key-���ܿ�λ, set<int>(��Ҫ��������λ����)

	SuperGalaxiesMap m_super_galaxies_cfg;		// ����ϵ��λ
	SuperGalaxiesConnectMap m_super_galaxies_adjoin_cfg;	// ����ϵ�ڽ�

	StarChartOtherCfg m_other_cfg;
};


#endif
