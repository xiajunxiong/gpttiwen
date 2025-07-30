#ifndef __NEW_LIFE_SKILL_CONFIG_HPP__
#define __NEW_LIFE_SKILL_CONFIG_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include <set>

struct NewLifeSkillOtherCfg
{
	NewLifeSkillOtherCfg() : occupation_double_times(0), food_time(0), shift_card_time(0)
	{}

	int occupation_double_times;			//ҩʦ�ͳ�ʦÿ������˫������
	int food_time;							//��ʳ����ʱ��
	int shift_card_time;					//��������ʱ��
};

struct NewLifeSkillLevelCfg
{
	static const int MAX_DOUBLE_RATE_COUNT_NUM = 1000;
	NewLifeSkillLevelCfg() : up_expend(0), double_rate(0), prof_double_rate(0), up_expend_medicine(0), up_expend_cook(0)
	{}

	int up_expend;				//������������
	int double_rate;			//�ɼ�˫������(ǧ�ֱ�)
	int prof_double_rate;		//ְҵ����˫������(ǧ�ֱ�)
	int up_expend_medicine;		//ҩʦְҵ��������
	int up_expend_cook;			//��ʦְҵ��������
};

struct NewLifeSkillPropsCfg
{
	static const int MAX_ATTR_NUM = 3;
	NewLifeSkillPropsCfg() : type(0), expend_point(0), increase_point(0), skill_id(0), use_skill_need_point(0), attr_count(0)
	{
		for (int i = 0; i < ARRAY_LENGTH(attr_list); i++)
		{
			attr_list[i].attr_type = 0;
			attr_list[i].attr_value = 0;
		}
	}

	int type;							//NEW_LIFE_SKILL_PROPS_TYPE_BEGIN_MAX
	int expend_point;					//����ս�����Ļû���
	int increase_point;					//�������ӻû���
	int skill_id;						//����ID
	int use_skill_need_point;			//ʹ�ü������Ļû���
	int attr_count;						
	AttrInfo attr_list[MAX_ATTR_NUM];	//�����б�
};

struct NewLifeSkillRecipeCfg
{
	static const int MAX_NEED_ITEM_COUNT = 2;
	NewLifeSkillRecipeCfg() : new_life_skill_type_limit(0), new_life_skill_level_limit(0), need_item_count(0), 
		expend_active(0), compose_item_id(0)
	{}

	int new_life_skill_type_limit;
	int new_life_skill_level_limit;
	std::set<int> role_prof_set;
	int need_item_count;
	ItemConfigData need_item_list[MAX_NEED_ITEM_COUNT];
	int expend_active;
	int compose_item_id;			//�ϳɵ���
};

struct NewLifeSkillRecoveryCfg
{
	NewLifeSkillRecoveryCfg() : mail_virtual_type(0), price(0)
	{}

	int mail_virtual_type;
	int price;			//һ��û��㷵�ظ���ֵ X(�û���) *price
};

class NewLifeSkillConfig : public ILogicConfig
{
public:
	NewLifeSkillConfig();
	virtual ~NewLifeSkillConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const NewLifeSkillOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const NewLifeSkillLevelCfg * GetNewLifeSkillLevelCfg(int skill_level);
	const NewLifeSkillRecipeCfg * GetRecipeCfg(int sn);
	const NewLifeSkillPropsCfg * GetPropsCfg(int seq);

	bool IsNewLifeProf(int new_life_skill_type);
	const std::vector<int>& GetSystemFoodList() const { return m_system_food_list; }
	
	const NewLifeSkillRecoveryCfg * GetRecoveryCfg(int change_card_seq) const;

private:
	
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitLevelCfg(TiXmlElement *RootElement);
	int InitRecipeCfg(TiXmlElement *RootElement);
	int InitPropsCfg(TiXmlElement *RootElement);
	int InitSystemFoodCfg(TiXmlElement* RootElement);
	int InitRecoveryCfg(TiXmlElement* RootElement);

	NewLifeSkillOtherCfg m_other_cfg;
	std::map<int, NewLifeSkillLevelCfg> m_new_life_skill_level_cfg;		//����ܵȼ����� key:skill_level
	std::map<int, NewLifeSkillRecipeCfg> m_recipe_cfg;					//�䷽���� key:sn
	std::map<int, NewLifeSkillPropsCfg> m_props_cfg;					//�������� key:seq
	std::vector<int> m_system_food_list;								//������ʳ���б�
	std::map<int, NewLifeSkillRecoveryCfg> m_recovery_map;				//��������(�û��㷵��)
};


#endif