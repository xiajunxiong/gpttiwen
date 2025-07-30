#ifndef __PET_SOUL_EQUIPMENT_CONFIG_HPP__
#define __PET_SOUL_EQUIPMENT_CONFIG_HPP__

#include "gamedef.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "equipment/petequipment.h"
#include "servercommon/workshopdef.h"

#include <map>
#include <set>
#include <vector>

struct PetSoulEquipOtherCfg
{
	PetSoulEquipOtherCfg() : exp_attenuation_100(0), cost_coin(0) {}
	int exp_attenuation_100;
	int cost_coin;
};

struct PetSoulEquipBaseCfg
{
	PetSoulEquipBaseCfg() :seq(0), type(0), quality(0), level_max(0), intensify_group(0), steps_group(0),
		refine_group(0), attribute_num(0), attribute_group(0), pet_weapon_skill(0), pet_weapon_item(0), levels_1_pet_weapon_exp(0),
		pet_weapon_type(0), pet_weapon_pet_id(0), change_item(0), change_item_num(0) {}

	int seq;							//���
	int	type;							//����
	int	quality;						//Ʒ��
	int	level_max;						//�������ȼ�
	int	intensify_group;				//����ǿ����
	int	steps_group;					//����ͻ����
	int	refine_group;					//����������
	int	attribute_num;					//����������
	int	attribute_group;				//������
	int	pet_weapon_skill;				//����������Ч������
	int	pet_weapon_item;				//����Ӧ�ĵ���ID
	int	levels_1_pet_weapon_exp;		//1������ǿ�����ṩ�ĳ�ʼ����
	int	pet_weapon_type;				//�������ͣ���ͨ����ר����
	int	pet_weapon_pet_id;				//ר������Ӧ�ĳ���
	int change_item;
	int	change_item_num;
};

struct AttributeNumGroup
{
	AttributeNumGroup() :attribute_num_group(0), num_min(0), num_max(0) {}
	int attribute_num_group;
	int num_min;
	int num_max;
};

struct AttributeTypeValueDetailGroup
{
	AttributeTypeValueDetailGroup() :att_type(0), att_min(0), att_max(0), rate_10000(0) {};
	int att_type;
	int att_min;
	int att_max;
	int rate_10000;
};

struct AttributeTypeValueSeqGroup
{
	AttributeTypeValueSeqGroup() :attribute_seq(0), weight(0) {}
	int attribute_seq;
	int weight;
	std::vector<AttributeTypeValueDetailGroup> m_attribute_vec;
};

struct AttributeTypeValueGroup
{
	AttributeTypeValueGroup() : attribute_group(0) {}
	int attribute_group;
	std::map<int, AttributeTypeValueSeqGroup> attribute_list;
};

struct AttributeIntensifyValueCfg
{
	AttributeIntensifyValueCfg() :att_type(0), intensify_att_min(0), intensify_att_max(0) {}
	int att_type;
	int intensify_att_min;
	int intensify_att_max;
};

struct AttributeIntensifyValueTypeCfg
{
	AttributeIntensifyValueTypeCfg() : quality(0) {};
	
	int quality;
	std::vector<AttributeIntensifyValueCfg> int_vec;
};

struct AttributeUplevelCfg
{
	AttributeUplevelCfg() :quality(0), levels(0), need_steps(0), exp(0) {}
	int quality;
	int levels;
	int need_steps;
	int exp;
};

struct PetEquipSoulStepsCfg
{
	PetEquipSoulStepsCfg() : quality(0), steps(0), min_level(0), max_level(0) {}
	int quality;
	int	steps;
	int	min_level;
	int	max_level;
	std::vector<ItemConfigData> m_cost_vec;
};

struct PetSoulEquipStepsAttrCfg
{
	PetSoulEquipStepsAttrCfg() : att_type(0), intensify_att_min(0), intensify_att_max(0) {}
	int	att_type;
	int	intensify_att_min;
	int	intensify_att_max;
};

struct PetSoulEquipStepsTypeCfg
{
	PetSoulEquipStepsTypeCfg() : quality(0) {}

	int quality;
	std::vector<PetSoulEquipStepsAttrCfg> int_vec;
};

struct PetSoulEquipSpidCfg
{
	PetSoulEquipSpidCfg() : sp_id(0),rate(0) {}
	int sp_id;
	int rate;
};

struct PetSoulEquipSpidGroupCfg
{
	PetSoulEquipSpidGroupCfg() : weight(0) {}
	
	int weight;
	std::vector<PetSoulEquipSpidCfg> m_spid_cfg;
};

struct PetEquipSoulRefineCfg
{
	PetEquipSoulRefineCfg() :refine(0), new_spid(0), attr_type_1(0), attr_value_1(0), attr_type_2(0), attr_value_2(0) {}
	int refine;
	int	new_spid;
	int attr_type_1;
	int attr_value_1;
	int attr_type_2;
	int attr_value_2;
};

struct PetSoulEquipItemIdExpCfg
{
	PetSoulEquipItemIdExpCfg() : item_id(0), item_exp(0), cost_coin(0) {}
	int item_id;
	int item_exp;
	int cost_coin;
};

struct PetEquipSoulRefineGroupCfg
{
	PetEquipSoulRefineGroupCfg() {}
	int seq;
	std::vector<PetEquipSoulRefineCfg> m_refine_vec;
};

struct PetEquipSoulExchangeCfg
{
	PetEquipSoulExchangeCfg() : pet_weapon_seq(0), pet_weapon_item_id(0), expend_item_id(0), expend_num(0) {}
	int pet_weapon_seq;
	int	pet_weapon_item_id;
	int	expend_item_id;
	int	expend_num;
};

struct PetSoulSpPriority
{
	int spid;  
	int sp_type; 
	int priority;
};

class PetSoulEquipMentConfig : public ILogicConfig
{
public:
	PetSoulEquipMentConfig();
	virtual ~PetSoulEquipMentConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const PetSoulEquipOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const PetSoulEquipBaseCfg * GetPetSoulEquipBaseCfg(int seq);
	const PetSoulSpPriority * GetPetSoulSpPriority(int sp_id);
	const AttributeNumGroup * GetPetSoulEquipAttrNumCfg(int group_id);
	const AttributeTypeValueDetailGroup * RandAttrTypeValueCfg(int group_id,int attr_idx);
	const AttributeIntensifyValueCfg * GetPetSoulEquipIntensifyValueCfg(int quality, int attr_type);
	const AttributeUplevelCfg * GetPetSoulEquipUpLevelCfg(int quality, int level);
	const PetEquipSoulStepsCfg * GetPetSoulEquipStepsCfg(int quality, int steps);
	const PetSoulEquipStepsAttrCfg * GetPetSoulEquipStepsAddValueCfg(int quality, int attr_type);
	const PetSoulEquipSpidCfg * GetPetSoulEquipSpidCfg(int seq);
	const PetEquipSoulRefineCfg * GetPetSoulEquipRefineCfg(int seq, int refine);
	const PetSoulEquipItemIdExpCfg * GetPetSoulEquipItemIdExpCfg(int item_id);
	const PetEquipSoulExchangeCfg * GetEquipSoulExchangeCfg(int seq);

	void GetBackItemByExtraExp(int extra_exp, std::vector<ItemConfigData> * back_vec);//���ݾ��鷵���������
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitBaseItemGroup(TiXmlElement * RootElement);
	int InitAttributeNumGroup(TiXmlElement *RootElement);
	int InitAttributeTypeValueGroup(TiXmlElement *RootElement);
	int InitAttributeTypeIntensifyCfg(TiXmlElement *RootElement);
	int InitAttributeUplevelCfg(TiXmlElement *RootElement);
	int InitEquipSoulStepsCfg(TiXmlElement *RootElement);
	int InitEquipSoulStepsValueCfg(TiXmlElement *RootElement);
	int InitEquipSoulSpidCfg(TiXmlElement *RootElement);
	int InitEquipSoulRefineCfg(TiXmlElement *RootElement);
	int InitItemIdExpCfg(TiXmlElement *RootElement);
	int InitPetEquipSoulExchangeCfg(TiXmlElement *RootElement);
	int InitPetEquipSoulPriority(TiXmlElement * RootElement);

	PetSoulEquipOtherCfg m_other_cfg;
	std::map<int, PetSoulEquipBaseCfg> m_base_cfg;									// ������������
	std::map<int, AttributeNumGroup> m_attr_num_cfg;								// ����������������
	std::map<int, AttributeTypeValueGroup> m_attr_type_cfg;							// ����������
	std::map<int, AttributeIntensifyValueTypeCfg> m_intensify_cfg;					// ǿ��ʱ��ӦƷ��������Ӧ���Է�Χ
	std::vector<AttributeUplevelCfg> m_soul_uplevel_cfg;							// ǿ����������
	std::vector<PetEquipSoulStepsCfg> m_soul_steps_cfg;								// ����ͻ������
	std::map<int, PetSoulEquipStepsTypeCfg> m_pet_soul_equip_steps_cfg;				// ����ͻ����������
	std::map<int, PetSoulEquipSpidGroupCfg> m_pet_soul_equip_spid_cfg;				// ������Ч�� 
	std::map<int, PetEquipSoulRefineGroupCfg> m_refine_cfg;							// ����������
	std::vector<PetSoulEquipItemIdExpCfg> m_item_exp_cfg;							// ���������ṩ����ֵ
	std::map<int, PetEquipSoulExchangeCfg> m_exchange_vec;
	std::map<int, PetSoulSpPriority > m_pet_soul_sp_priority_map;
};

#endif
