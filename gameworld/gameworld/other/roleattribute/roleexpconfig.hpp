#ifndef __ROLE_EXP_CONFIG_HPP__
#define __ROLE_EXP_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/servercommon.h"
#include "other/attributelist.hpp"
#include "servercommon/attributedef.hpp"
#include "servercommon/struct/itemlistparam.h"

static const int BASE_EXP_PERCENTAGE_NUM = 100;			//��������ٷֱ�

struct RoleLevelCfg
{
	RoleLevelCfg() : exp(0), total_attr_point(0), add_attr_point(0) {}

	long long exp;
	int total_attr_point;
	int add_attr_point;
};

struct RoleBaseAttrCfg
{
	RoleBaseAttrCfg() : maxhp(0), maxmp(0), attack(0), defence(0), agility(0), mentality(0), recovery(0) {}

	Attribute maxhp;
	Attribute maxmp;
	Attribute attack;
	Attribute defence;
	Attribute agility;
	Attribute mentality;
	Attribute recovery;
};

struct AttrPointCfg
{
	AttrPointCfg() : prof(0), con(0), str(0), def(0), agi(0), mag(0) {}

	short prof;
	short con;
	short str;
	short def;
	short agi;
	short mag;
};

struct ConvertAddPointCfg	// ת������
{
	ConvertAddPointCfg() : maxhp_rate(0), maxmp_rate(0), attack_rate(0), defense_rate(0), agility_rate(0), mentality_rate(0), recovery_rate(0) {}

	float maxhp_rate;		// Ѫ��
	float maxmp_rate;		// ħ��
	float attack_rate;		// ����
	float defense_rate;		// ����
	float agility_rate;		// �ٶ�
	float mentality_rate;	// ����
	float recovery_rate;	// �ظ�
};

struct RoleExpOtherCfg
{
	RoleExpOtherCfg():wash_point_item(0), wash_point_num(0), auto_add_attr_level_upbound(0), world_level_effect(0), world_level_open_day(0), 
		world_level_overflow(0), top_level_reset_day(0), exp_coefficient(0.0)
	{}

	int wash_point_item;
	int	wash_point_num;
	int auto_add_attr_level_upbound;	// �Զ��ӵ�ȼ�����
	int world_level_effect;				// ���羭��ӳ�����ȼ�
	int world_level_open_day;			// ����ȼ�����ʱ��(������������)
	int world_level_overflow;			// ����ȼ����
	int top_level_reset_day;			// �۷�֮·��������
	double exp_coefficient;				// ����ϵ��
};

struct RoleTopLevelCfg
{
	RoleTopLevelCfg() : up_exp(0)
	{}

	enum REWARD_GROUP_TYPE
	{
		REWARD_GROUP_TYPE_BEGIN = 0,
		REWARD_GROUP_TYPE_NORMAL = 1,		//��������
		REWARD_GROUP_TYPE_LOOP = 2,			//ѭ������

		REWARD_GROUP_TYPE_MAX
	};

	int up_exp;
	std::vector<ItemConfigData> reward_list;
};

struct RoleTopLevelAchieveCfg
{
	RoleTopLevelAchieveCfg() : achieve_type(0), param(0)
	{}

	enum ACHIEVE_TYPE
	{
		ACHIEVE_TYPE_INVALID = 0,	
		ACHIEVE_TYPE_LEVEL = 1,			//�۷�ȼ�

		ACHIEVE_TYPE_MAX
	};

	int achieve_type;
	int param;
	std::vector<ItemConfigData> item_list;
};

class RoleExpConfig : public ILogicConfig
{
public:
	RoleExpConfig();
	virtual ~RoleExpConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const RoleLevelCfg* GetLevelCfg(int level);
	const AttrPointCfg* GetBasePointDistributeCfg(int profession);
	const AttrPointCfg* GetRecommendAddPointCfg(int profession);
	const RoleBaseAttrCfg& GetRoleBaseAttrCfg() const { return m_base_attr_cfg; }
	const ConvertAddPointCfg* GetConvertCfg(int add_point_type);
	const RoleExpOtherCfg& GetOtherCfg() { return m_other_cfg; }
	void ConvertPointToAttribute(const AttributePlan& ap, AttributeList& out_attr_list);
	void ConvertPointToAttribute2( AttributeList & ARG_IN_AND_OUT out_attr_list);
	int GetWorldLevel(int open_server_day);
	int GetWorldLevelUp(int role_level, int world_level);
	int GetWorldLevelUpLimit(int role_level) const;
	int GetExpDecayScaleByPassLevel(int pass_level) const;

	long long GetNeedExpByLevel(int begin_level, int end_level);

	const RoleTopLevelCfg * GetTopLevelCfg(int top_level);
	const RoleTopLevelAchieveCfg * GetToplevelAchieveCfg(int seq);
private:
	int InitExpInfoCfg(TiXmlElement *RootElement); 
	int InitBaseAttrCfg(TiXmlElement* RootElement);
	int InitBaseAttrPointDistributeCfg(TiXmlElement* RootElement);
	int InitRecommendAddPointCfg(TiXmlElement* RootElement);
	int InitConvertAddPointCfg(TiXmlElement* RootElement);
	int InitOtherCfg(TiXmlElement* RootElement);
	int InitWorldLevelCfg(TiXmlElement* RootElement);
	int InitWorldLevelUpCfg(TiXmlElement* RootElement);
	int InitWorldLevelUpDivideCfg(TiXmlElement* RootElement);
	int InitTopLevelCfg(TiXmlElement* RootElement);
	int InitTopLevelAchieveCfg(TiXmlElement* RootElement);
	int InitLevelExpDecayCfg(TiXmlElement* RootElement);


	RoleLevelCfg m_level_cfg_list[MAX_ROLE_LEVEL + 1];
	RoleBaseAttrCfg m_base_attr_cfg;

	int m_base_point_distribute_cfg_num;
	AttrPointCfg m_base_point_distribute_cfg[PROF_TYPE_MAX / PROFESSION_BASE];

	int m_recommend_add_point_cfg_num;
	AttrPointCfg m_recommend_add_point_cfg[PROF_TYPE_MAX / PROFESSION_BASE];

	ConvertAddPointCfg m_convert_list[ADD_POINT_TYPE_MAX];
	RoleExpOtherCfg m_other_cfg;

	std::map<int, int> m_world_level_map;				//����ȼ� key:�������� second:����ȼ�
	std::map<int, int> m_world_level_up_map;			//����ȼ��ӳ� key:min_level * 1000 + max_level second:level_addition
	std::map<int, int> m_world_level_divide_map;		//����ȼ�����ֶ� key:min_level * 1000 + max_level second:level_addition_limit
	std::map<int, int> m_level_exp_decay_map;			//����˥�� key:pass_level second:exp_decay_scale

	std::vector<RoleTopLevelCfg> m_top_level_reward_vec;		//�۷�ȼ�
	std::vector<RoleTopLevelCfg> m_loop_top_level_reward_vec;	//�۷�ȼ�(ѭ������) ����������������

	std::vector<RoleTopLevelAchieveCfg> m_top_level_achieve_vec;	//�۷�ȼ��ɾͽ���
};

#endif