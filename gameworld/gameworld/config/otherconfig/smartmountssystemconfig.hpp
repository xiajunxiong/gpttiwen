#ifndef __SMART_MOUNTS_SYSTEM_CONFIG_HPP__
#define __SMART_MOUNTS_SYSTEM_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/smartmountssystemdef.hpp"
#include "servercommon/struct/battle/battle_def.hpp"
#include <set>

static const int BICYCLE_ID_BASE_NUM = 1000;
static const int SKILL_INDEX_BASE_NUM = 100;

enum SMART_MOUNTS_SKILL_TYPE
{
	SMART_MOUNTS_SKILL_TYPE_BEGIN = 0,
	SMART_MOUNTS_SKILL_TYPE_BASE = 1,			// ��������
	SMART_MOUNTS_SKILL_TYPE_EXCLUSIVE = 2,		// ר������

	SMART_MOUNTS_SKILL_TYPE_MAX
};

struct SmartMountsSystemOtherCfg
{
	SmartMountsSystemOtherCfg() : energy_limit_base(0), energy_limit_skill(0), energy_up_coin(0), energy_up(0), energy_up_number(0), grow_reset_use(0),  skill_reset_use(0),
		skill_reset_number(0), lock_skill_1(0), lock_skill_2(0), init_energy(0)
	{}

	int energy_limit_base;		//����������Ч��;���
	int energy_limit_skill;		//������Ч��;���
	int energy_up_coin;			//��������ͭ������
	int energy_up;				//�����������ID
	int energy_up_number;		//ʹ�ò��侫����ֵ
	int grow_reset_use;			//ϴ�����ĵ���ID
	int skill_reset_use;		//ϴ���������ĵ���ID
	int skill_reset_number;		//ϴ���������ĵ�������
	int lock_skill_1;			//����1���������ĵ�Ԫ��
	int lock_skill_2;			//����2���������ĵ�Ԫ��
	int init_energy;
};

struct SmartMountsSystemBaseCfg
{
	SmartMountsSystemBaseCfg() : bicycle_id(0), active_use_item_id(0), active_use_item_num(0), jewelry_id(0), wear_nornaments_day_give(0), bicycle_grow_begin(0),
		grow_min_proportion(0), bicycle_energy_max(0), energy_use(0), control_num(0), bicycle_holy(0)
	{
		memset(equip_id_list, 0 ,sizeof(equip_id_list));
	}

	int bicycle_id;
	int active_use_item_id;			
	int active_use_item_num;		
	int jewelry_id;												//����ID
	int wear_nornaments_day_give;								//��Ʒ�����ÿ�����͵ľ���ֵ
	int bicycle_grow_begin;										//��ʼ����ֵ	�ٷֱ�
	int grow_min_proportion;									//����ռ��(Ͷ�ű���)(�ٷֱ�)
	int bicycle_energy_max;										//����ֵ����
	int energy_use;												//��������
	int control_num;											//ͳ������
	std::vector<int> gain_way_list;								//��ȡ;���б�(seq)
	int equip_id_list[MAX_SMART_MOUNTS_SYSTEM_EQUIP_CUR_NUM];	//װ��ID�б�
	int bicycle_holy;											//����ʥ��
};

struct SmartMountsSystemGainWayCfg
{
	SmartMountsSystemGainWayCfg() : way_type(0), param1(0), param2(0)
	{}
	static const int MAX_SMART_MOUNTS_SYSTEM_GAIN_WAY_TYPE_NUM = 6;
	enum GAIN_WAY_TYPE
	{
		GAIN_WAY_TYPE_BEGIN = 0,
		GAIN_WAY_TYPE_SMART_MOUNTS_LEVEL = 1,					//�ȼ��ﵽparam2������   
		GAIN_WAY_TYPE_HAS_NUM_LEVEL_SMART_MOUNTS = 2,			//ӵ��param1ֻparam2������
		GAIN_WAY_TYPE_HAS_SMART_MOUNTS_SKILL_LEVEL = 3,			//ӵ��param1���ȼ����ڻ����param2�������＼��
		GAIN_WAY_TYPE_HAS_MAX_SMART_MOUNTS_SKILL_LEVEL = 4,		//ӵ��param1�����ȼ����＼��                           
		GAIN_WAY_TYPE_RAND_ACTIVITY = 5,						//��Ӫ����

		GAIN_WAY_TYPE_MAX
	};

	int way_type;
	int param1;
	int param2;
};

struct SmartMountsSystemBreachCfg
{
	static const int MAX_BREACH_RATE_COUNT = 10000;
	SmartMountsSystemBreachCfg() :breach_consume_item_id(0), breach_consume_item_num(0), breach_rate(0), consume_return(0)
	{
		memset(attr_type, 0, sizeof(attr_type));
		memset(attr_value, 0, sizeof(attr_value));
	}

	int breach_consume_item_id;
	int breach_consume_item_num;
	int breach_rate;				//ͻ�Ƴɹ�����
	int consume_return;				//ͻ��ʧ�ܷ���
	int attr_type[MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM];
	int attr_value[MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM];
};

//���Ŀ���ǰ�ȼ���Ӧ����,�ȼ����ƿ���һ������
struct SmartMountsSystemLevelCfg
{
	SmartMountsSystemLevelCfg() : level(0), level_up_consume_item_id(0), level_up_consume_item_num(0), is_need_breach(0), role_level_limit(0), skill_point_gain(0)
	{
		memset(attr_type, 0, sizeof(attr_type));
		memset(attr_value, 0, sizeof(attr_value));
	}

	int level;
	int level_up_consume_item_id;
	int level_up_consume_item_num;
	int is_need_breach;
	int role_level_limit;
	int attr_type[MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM];
	int attr_value[MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM];
	int skill_point_gain;
};

struct SmartMountsSkillUnlockCfg
{
	SmartMountsSkillUnlockCfg()
	{
		memset(base_skill_unlock_level, 0, sizeof(base_skill_unlock_level));
		memset(base_skill_group, 0, sizeof(base_skill_group));
		/*	memset(exclusive_skill_unlock_level, 0, sizeof(exclusive_skill_unlock_level));
			memset(exclusive_skill_id, 0, sizeof(exclusive_skill_id));*/
	}

	int base_skill_unlock_level[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM];
	int base_skill_group[MAX_SMART_MOUNTS_SYSYEM_BASE_SKILL_CUR_NUM];

	//int exclusive_skill_unlock_level[MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_CUR_NUM];
	//int exclusive_skill_id[MAX_SMART_MOUNTS_SYSTEM_EXCLUSIVE_SKILL_CUR_NUM];			//ר�����ܲ��������,�̶�
};

struct SmartMountsSkillUpCfg
{
	SmartMountsSkillUpCfg() : bicycle_level_limit(0), need_consume_skill_point(0)
	{}

	int bicycle_level_limit;
	int need_consume_skill_point;
	std::vector<ItemConfigData> consume_list;
};

struct SmartMountsSystemSkillGroupCfg
{
	SmartMountsSystemSkillGroupCfg() : rate_count(0)
	{}

	struct ItemCfg
	{
		ItemCfg() : skill_id(0), rate(0), quality(0)
		{}

		int skill_id;
		int rate;
		int quality;
	};

	int rate_count;
	std::vector<ItemCfg> skill_list;
};

struct SmartMountsSystemEquipCfg
{
	SmartMountsSystemEquipCfg() : equip_sp_id(0), role_level_limit(0), equip_score(0)
	{}

	int equip_sp_id;
	int role_level_limit;
	int equip_score;
	std::vector<ItemConfigData> consume_list;
};

struct SmartMountsSystemJewelryCfg
{
	SmartMountsSystemJewelryCfg() : bicycle_grow(0)
	{}

	int bicycle_grow;			//�ٷֱ�
	std::vector<ItemConfigData> consume_list; 
};

struct SmartMountsSystemHolyCfg
{
	SmartMountsSystemHolyCfg() : attr_type(0), attr_value(0), role_level_limit(0)
	{}

	int attr_type;
	int attr_value;
	int role_level_limit;
	std::vector<ItemConfigData> consume_list;
};

struct SmartMountsSystemRevealCfg
{
	SmartMountsSystemRevealCfg() : consume_item_id(0), consume_item_num(0), role_level_limit(0)
	{}

	int consume_item_id;
	int consume_item_num;
	int role_level_limit;
	BattleAttrPair attr_list[MAX_SMART_MOUNTS_SYSTEM_ATTR_NUM];			
};

struct SmartMountsItemConvertCfg
{
	SmartMountsItemConvertCfg() : consume_item_id(0)
	{}

	int consume_item_id;
	std::set<int> convert_item_list;	
};

struct SmartMountsActiveConvertCfg
{
	SmartMountsActiveConvertCfg() : smart_mounts_index(0), target_item_id(0)
	{}

	int smart_mounts_index;			//�������
	int target_item_id;
};

class SmartMountsSystemConfig : public ILogicConfig
{
public:
	SmartMountsSystemConfig();
	virtual ~SmartMountsSystemConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const SmartMountsSystemOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const SmartMountsSystemGainWayCfg* GetGainWayCfg(int seq);
	const SmartMountsSystemBaseCfg * GetBaseCfg(int index);
	const SmartMountsSystemBreachCfg * GetBreachCfg(int bicycle_id, int level);
	const std::map<int, SmartMountsSystemBreachCfg> * GetBreachCfg(int bicycle_id);
	int GetMaxLevelById(int bicycle_id);
	const SmartMountsSystemLevelCfg * GetLevelCfg(int bicycle_id, int level);
	const SmartMountsSkillUnlockCfg * GetUnlockCfg(int bicycle_id);
	int GetMaxSkillLevel(int bicycle_id, int skill_index, bool is_base_skill);
	const SmartMountsSkillUpCfg *  GetSkillUpCfg(int bicycle_id, int skill_index, bool is_base_skill, int skill_level);
	const std::map<int, SmartMountsSkillUpCfg> * GetSkillUpCfg(int bicycle_id, int skill_index, bool is_base_skill);
	int GetUnlockBaseSkill(int skill_group_id, UInt16 *now_has_skill_list, int count);
	bool GetRefineBaseSkill(int bicycle_id, UInt16 *lock_skill_id_list, int count, UInt16 *new_base_skill_list);
	const SmartMountsSystemEquipCfg * GetEquipCfg(int equip_id, int equip_level);
	int GetEquipMaxLevelById(int equip_id);
	const SmartMountsSystemJewelryCfg * GetJewelryCfg(int jewelry_id, int jewelry_level);
	int GetJewelryMaxLevelById(int jewelry_id);
	const SmartMountsSystemHolyCfg * GetHolyCfg(int holy_id, int holy_level);
	int GetHolyMaxLevelById(int holy_id);
	const SmartMountsSystemRevealCfg * GetSmartMountsSystemRevealCfg(int bicycle_id, int reveal_level);

	int GetQualityById(int skill_id);
	int GetSkillScoreCfg(int quality, int skill_level);
	int GetSkillScoreCfgByIdAndLevel(int skill_id, int skill_level);

	const SmartMountsItemConvertCfg * GetItemConvertCfg(int convert_item_id) const;
	const SmartMountsActiveConvertCfg * GetActiveConvertCfg(int active_item_id) const;

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitGainWayCfg(TiXmlElement *RootElement);
	int InitbaseCfg(TiXmlElement *RootElement);
	int InitBreachCfg(TiXmlElement *RootElement);
	int InitLevelCfg(TiXmlElement *RootElement);
	int InitSkillUnlockCfg(TiXmlElement *RootElement);
	int InitSkillUpCfg(TiXmlElement *RootElement);
	int InitSkillGroupCfg(TiXmlElement *RootElement);
	int InitSkillScoreCfg(TiXmlElement *RootElement);
	int InitEquipCfg(TiXmlElement *RootElement);
	int InitJewelryCfg(TiXmlElement *RootElement);
	int InitHolyCfg(TiXmlElement *RootElement);
	int InitRevealCfg(TiXmlElement *RootElement);
	int InitItemConvertCfg(TiXmlElement *RootElement);

	int GetSkillUpKeyCfg(int bicycle_id, int skill_index, int skill_type);
	int GetSkillQualityKey(int quality, int skill_level);

	SmartMountsSystemOtherCfg m_other_cfg;
	SmartMountsSystemBaseCfg m_smart_mounts_cfg_list[MAX_SMART_MOUNTS_SYSTEM_NUM];			//�����������
	std::map<int, SmartMountsActiveConvertCfg> m_active_convert_map;						//���Ｄ�����ת������ key:�������ID
	std::set<int> m_bicycle_set;
	std::map<int, SmartMountsSystemGainWayCfg> m_gain_way_map;								//��ȡ;�� 

	std::map<int, std::map<int, SmartMountsSystemBreachCfg> > breach_map;					//ͻ������	key:bicycle_id level
	std::map<int, std::map<int, SmartMountsSystemLevelCfg> > level_map;						//�ȼ�����	key:bicycle_id level
	std::map<int, SmartMountsSkillUnlockCfg> skill_unlock_map;								//���ܽ���	key:bicycle_id
	std::map<int, std::map<int, SmartMountsSkillUpCfg> > skill_up_map;						//��������	key:GetSkillUpKeyCfg����ֵ skill_level
	std::map<int, SmartMountsSystemSkillGroupCfg> skill_map;								//�������������� key:skill_group_id
	std::map<int, int> m_skill_quality_map;													//key:skill_id second:quality
	std::map<int, int> m_skill_score_map;													//key:quality * 1000 + skill_level  second:skill_scroe
	std::set<int> m_skill_quality_set;	

	std::map<int, std::map<int, SmartMountsSystemEquipCfg> > m_equip_cfg;					//װ������ key1:equip_id key2:equip_level  
	std::map<int, std::map<int, SmartMountsSystemJewelryCfg> > m_jewelry_cfg;				//�������� key1:jewelry_id key2:jewelry_level
	std::map<int, std::map<int, SmartMountsSystemHolyCfg> > m_holy_cfg;						//ʥ������ key1:holy_id  key2:holy_level

	SmartMountsSystemRevealCfg * m_reveal_list[MAX_SMART_MOUNTS_SYSTEM_NUM][MAX_SMART_MOUNTS_SYSTEM_REVEAL_LEVEL_NUM];		//�㻯���� 

	std::vector<SmartMountsItemConvertCfg> m_item_convert_list;								//����ת������
};


#endif