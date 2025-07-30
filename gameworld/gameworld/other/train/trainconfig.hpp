#ifndef __TRAIN_CONFIG_HPP__
#define __TRAIN_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/traindef.hpp"
#include "other/attributelist.hpp"
#include <map>
#include <vector>
#include <set>

static const int MAX_PET_TRAIN_SKILL_LEVEL = 20;

struct TrainCfg
{
	TrainCfg()
	{
		type = 0;
		level = 0;
		max_exp = 0;
		attribute_value = 0;

		bind_coin_consume = 0;
		get_exp = 0;

		train_ball_exp = 0;
		train_ball_consume = 0;
		item_id = 0;
		consume_type = 0;
		level_limit = 0;
	}
	int type;
	int level;
	int max_exp;
	int attribute_value;

	int bind_coin_consume;
	int get_exp;

	int train_ball_exp;
	int train_ball_consume;
	int item_id;
	int consume_type;
	int level_limit;		//属性上限
};

struct TrainAttrCfg
{
	TrainAttrCfg()
	{
		type = 0;
		level = 0;
		prof = 0;
		attribute_type = 0;
		attribute_value = 0;
	}

	int type;
	int level;
	int prof;
	int attribute_type;
	int attribute_value;
};

struct PetTrainCfg		//宠物修炼技能，御兽法决 加成长值和属性点加成
{
	PetTrainCfg()
	{
		skill_level = 0;
		need_role_level = 0;
		need_exp = 0;
		need_coin = 0;
		add_exp = 0;
	}


	int skill_level;
	int need_role_level;
	int need_exp;
	int need_coin;
	int add_exp;
};

struct PetTrainCfg2		//宠物修炼技能，直接加属性的
{
	PetTrainCfg2()
	{
		type = 0;
		level = 0;
		max_exp = 0;
		attribute_value = 0;

		bind_coin_consume = 0;
		get_exp = 0;

		train_ball_exp = 0;
		train_ball_consume = 0;
		item_id = 0;
		consume_type = 0;
		need_role_level = 0;
	}
	int type;
	int level;
	int max_exp;
	int attribute_value;

	int bind_coin_consume;
	int get_exp;

	int train_ball_exp;
	int train_ball_consume;
	int item_id;
	int consume_type;
	int need_role_level;

	std::set<int> attr_type_list;
};

struct PartnerTrainCfg		//伙伴修炼技能，直接加属性的
{
	PartnerTrainCfg()
	{
		type = 0;
		level = 0;
		max_exp = 0;
		attribute_value = 0;

		bind_coin_consume = 0;
		get_exp = 0;

		train_ball_exp = 0;
		train_ball_consume = 0;
		item_id = 0;
		consume_type = 0;
		need_role_level = 0;
	}
	int type;
	int level;
	int max_exp;
	int attribute_value;

	int bind_coin_consume;
	int get_exp;

	int train_ball_exp;
	int train_ball_consume;
	int item_id;
	int consume_type;

	int need_role_level;
	std::set<int> attr_type_list;
};

struct TrainItemConvertCfg
{
	const static int MAX_COMSUME_SIZE = 6;

	TrainItemConvertCfg()
	{
		seq = 0;
		target_item_id = 0;
		target_item_num = 0;
		comsume_size = 0;
		for (int i = 0; i < MAX_COMSUME_SIZE; ++i)
		{
			comsume_item_id[i] = 0;
			comsume_num[i] = 0;
		}
	}

	int seq;
	int target_item_id;
	int target_item_num;
	int comsume_size;
	int comsume_item_id[MAX_COMSUME_SIZE];
	int comsume_num[MAX_COMSUME_SIZE];
};

struct TrainOtherCfg
{
	TrainOtherCfg()
	{
		item_ret = 0;
	}

	int item_ret;
};

enum TRAIN_SKILL_TYPE
{
	TRAIN_SKILL_TYPE_FU_ZHU,
	TRAIN_SKILL_TYPE_XIU_LIAN,
};

class TrainConfig : public ILogicConfig
{
public:
	TrainConfig();
	virtual ~TrainConfig();

	virtual bool Init(const std::string & configname, std::string * err);

	const TrainCfg * GetTrainCfg(int type, int level);
	const TrainAttrCfg * GetTrainAttrCfg(int prof, int type, int level);
	int GetMaxTrainLevel(int type, int role_level);
	int GetMainAttrType(int prof);
	const PetTrainCfg* GetPetTrainCfg(int skill_level);
	int GetPetTrainMaxLevel();
	int GetPetTrainMaxLevelByRoleLevel(int role_level);
	const TrainItemConvertCfg * GetTrainItemConvertCfg(int seq);
	const TrainOtherCfg & GetOtherCfg();
	std::set<int> GetTrainTypeBySkillType(int train_skill_type);

	int GetPetTrainCfgMaxLevel2(int type);
	const PetTrainCfg2 * GetPetTrainCfg2(int type, int level);
	int GetPetTrain2MaxLevelByRoleLevel(int type, int role_level);
	int GetPartnerTrainCfgMaxLevel(int type);
	const PartnerTrainCfg * GetPartnerTrainCfg(int type, int level);
	std::set<int> GetAllPetTrainCfg2Type();
	std::set<int> GetAllPartnerTrainCfgType();
	bool IsPetTrainCfg2Type(int type);
	bool IsPartnerTrainCfgType(int type);
private:
	int InitTrainCfg(TiXmlElement *RootElement);
	int InitTrainAttrCfg(TiXmlElement * RootElement);
	int InitTrainLevelCfg(TiXmlElement *RootElement);
	int InitProfReflectMap(TiXmlElement *RootElement);
	int InitPetTrainCfg(TiXmlElement* RootElement);
	int InitItemConvertCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitSkillTypeCfg(TiXmlElement * RootElement);
	int InitPetTrain2Cfg(TiXmlElement * RootElement);
	int InitPartnerTrainCfg(TiXmlElement * RootElement);

	std::map<int, std::map<int, TrainCfg> > m_train_map;
	std::map<int, std::map<int, int> > m_train_level_map;
	std::map<int, int> m_prof_attr_map;						// 职业 <--> 主属性 映射表

	int m_pet_train_max_level;
	PetTrainCfg m_pet_train_cfg_list[MAX_PET_TRAIN_SKILL_LEVEL];

	std::map<int, TrainItemConvertCfg> m_item_convert_cfg;						//琉璃鼎


	std::map<int, std::map<int, std::map<int, TrainAttrCfg> > > m_train_attr_map;
	TrainOtherCfg m_other_cfg;

	std::map<int, int> m_skill_type_cfg;

	std::map<int, std::map<int, PetTrainCfg2> > m_pet_train_2_map;
	std::map<int, std::map<int, PartnerTrainCfg> > m_partner_train_map;
};

#endif // !__TRAIN_CONFIG_HPP__