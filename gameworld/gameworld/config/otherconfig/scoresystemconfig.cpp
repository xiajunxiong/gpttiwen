#include "scoresystemconfig.hpp"
#include "servercommon/struct/battle/battle_def.hpp"
#include "gameworld/item/itempool.h"
#include "servercommon/traindef.hpp"
#include "other/rolemodulemanager.hpp"
#include "gameworld/item/shengqi/holyitem.h"

ScoreSystemConfig::ScoreSystemConfig()
{
}

ScoreSystemConfig::~ScoreSystemConfig()
{
	std::vector<SkillScoreCfg>().swap(m_skill_score_vec);
	std::vector<ScoreTaskCfg>().swap(m_score_task);
}

bool ScoreSystemConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("spe_score", InitMedalSpeScoreCfg);
	LOAD_CONFIG("partner_score", InitPartnerScoreCfg);
	LOAD_CONFIG("role_score", InitRoleScoreCfg);
	LOAD_CONFIG("pet_score", InitPetScoreCfg);
	LOAD_CONFIG("equipment_score", InitEquipmentScoreCfg);
	LOAD_CONFIG("scale_score", InitCapbilityBaseCfg);
	LOAD_CONFIG("skill_score", InitProfSkillScoreCfg);
	LOAD_CONFIG("train_score", InitTrainSkillScoreCfg);
	LOAD_CONFIG("passive_skill_score", InitSkillScoreCfg);
	LOAD_CONFIG("role_score_log", InitScoreTaskCfg);
	LOAD_CONFIG("shengqi_score_count", InitHolyEquipScoreCfg);

	return true;
}

const MedalSpeScoreCfg * ScoreSystemConfig::GetMedalSpeScroeCfg(int level)
{
	if(level <= 0 || level > MAX_ROLE_LEVEL) return NULL;

	return &m_medal_spe_score_cfg[level - 1];
}

const CharacterScoreCfg * ScoreSystemConfig::GetPartnerScoreCfg(int job, int level)
{
	if (job < 0 || job >= PROFESSION_TYPE_NUM)return NULL;
	if (level <= 0 || level > MAX_ROLE_LEVEL)return NULL;

	return &m_partner_score_cfg[job][level];
}

const CharacterScoreCfg * ScoreSystemConfig::GetRoleScoreCfg(int job, int level)
{
	if (job < 0 || job >= PROFESSION_TYPE_NUM)return NULL;
	if (level <= 0 || level > MAX_ROLE_LEVEL)return NULL;

	return &m_role_score_cfg[job][level];
}

const PetScoreCfg * ScoreSystemConfig::GetPetScoreCfg(int level)
{
	if (level <= 0 || level > MAX_ROLE_LEVEL)return NULL;

	return &m_pet_score_cfg[level];
}

const EquipmentScoreCfg * ScoreSystemConfig::GetEquipmentScoreCfg(ItemID item_id)
{
	std::map<ItemID, EquipmentScoreCfg>::iterator it = m_equipment_score_cfg.find(item_id);
	if (it != m_equipment_score_cfg.end())
	{
		return &it->second;
	}
	return NULL;
}

const CapabilityCalCfg * ScoreSystemConfig::GetProfCapabilityCalCfg(int app_prof)
{
	if(app_prof < 0 || app_prof >= PROFESSION_TYPE_NUM) return NULL;

	return &capability_base_cfg[app_prof];
}

bool ScoreSystemConfig::IsSpecialAttrType(int attr_type)
{
	bool ret = false;
	switch (attr_type)
	{
		case BATTLE_ATTR_REAL_DAMAGE:
		{
			ret = true;
		}
		break;
		default:
		break;
	}

	return ret;
}

int ScoreSystemConfig::GetProfSkillScore(ProfSkillScoreKey job_key)
{
	std::map<ProfSkillScoreKey, int>::iterator it = m_prof_skill_score_cfg.find(job_key);
	if(it != m_prof_skill_score_cfg.end()) return it->second;

	return -1;
}

int ScoreSystemConfig::GetTrainSkillScore(int train_skill_type, int train_skill_level)
{
	TrainSkillScoreKey key(train_skill_type, train_skill_level);
	std::map<TrainSkillScoreKey, int>::iterator it = m_train_skill_score_cfg.find(key);
	if (it != m_train_skill_score_cfg.end())
	{
		return it->second;
	}

	return 0;
}

const SkillScoreCfg * ScoreSystemConfig::GetSkillScpreCfg(int skill_id, int skill_level)
{
	for (std::vector<SkillScoreCfg>::iterator it = m_skill_score_vec.begin(); it != m_skill_score_vec.end(); it++)
	{
		if (it->skill_id == skill_id && it->skill_level == skill_level)
		{
			return &(*it);
		}
	}
	return NULL;
}

int ScoreSystemConfig::GetHolyEquipScoreCfg(int attr_type) const
{
	std::map<int, int>::const_iterator it = m_holy_equip_score_cfg.find(attr_type);
	if (it != m_holy_equip_score_cfg.end()) return it->second;

	return 0;
}

int ScoreSystemConfig::InitMedalSpeScoreCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	int last_level = 0;
	while (NULL != data_element)
	{
		int level = 0;
		if (!GetSubNodeValue(data_element, "level", level) || level <= 0 || level > MAX_ROLE_LEVEL) return -1;

		if(last_level + 1 != level) return -2;

		MedalSpeScoreCfg node_cfg;
		char score_name[MAX_MEDAL_MAX_GRADE][64];
		memset(score_name, 0, sizeof(score_name));

		for (int i = 5; i < MAX_MEDAL_MAX_GRADE; i++)
		{
			SNPRINTF(score_name[i], sizeof(score_name[i]),"score_%d", i);
		}
		for (int i = 5; i < MAX_MEDAL_MAX_GRADE; i++)
		{
			if (!GetSubNodeValue(data_element, score_name[i], node_cfg.score_priority[i])) return -(100 + i);
		}

		m_medal_spe_score_cfg[last_level] = node_cfg;
		last_level = level;

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int ScoreSystemConfig::InitPartnerScoreCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	//int last_level = 0;
	while (NULL != data_element)
	{
		int job = 0;
		if (!GetSubNodeValue(data_element, "job", job) || job < 0 || job > PROFESSION_TYPE_NUM) return -1;

		int level = 0;
		if (!GetSubNodeValue(data_element, "level", level) || level < 0 || level > MAX_ROLE_LEVEL) return -2;

		CharacterScoreCfg &node = m_partner_score_cfg[job][level];

		//if (last_level + 1 != level) return -2;

		if (!GetSubNodeValue(data_element, "score_s", node.score_s) || node.score_s < 0) return -3;
		if (!GetSubNodeValue(data_element, "score_a", node.score_a) || node.score_a < 0) return -4;
		if (!GetSubNodeValue(data_element, "score_b", node.score_b) || node.score_b < 0) return -5;

		//last_level = level;

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int ScoreSystemConfig::InitRoleScoreCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	//int last_level = 0;
	while (NULL != data_element)
	{
		int job = 0;
		if (!GetSubNodeValue(data_element, "job", job) || job < 0 || job > PROFESSION_TYPE_NUM) return -1;

		int level = 0;
		if (!GetSubNodeValue(data_element, "level", level) || level < 0 || level > MAX_ROLE_LEVEL) return -2;

		CharacterScoreCfg &node = m_role_score_cfg[job][level];

		//if (last_level + 1 != level) return -2;

		if (!GetSubNodeValue(data_element, "score_s", node.score_s) || node.score_s < 0) return -3;
		if (!GetSubNodeValue(data_element, "score_a", node.score_a) || node.score_a < 0) return -4;
		if (!GetSubNodeValue(data_element, "score_b", node.score_b) || node.score_b < 0) return -5;

		//last_level = level;

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int ScoreSystemConfig::InitPetScoreCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	//int last_level = 0;
	while (NULL != data_element)
	{
		int level = 0;
		if (!GetSubNodeValue(data_element, "level", level) || level < 0 || level > MAX_ROLE_LEVEL) return -1;

		PetScoreCfg &node = m_pet_score_cfg[level];

		//if (last_level + 1 != level) return -2;

		if (!GetSubNodeValue(data_element, "score_s", node.score_s) || node.score_s < 0) return -3;
		if (!GetSubNodeValue(data_element, "score_a", node.score_a) || node.score_a < 0) return -4;
		if (!GetSubNodeValue(data_element, "score_b", node.score_b) || node.score_b < 0) return -5;

		//last_level = level;

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int ScoreSystemConfig::InitEquipmentScoreCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	//int last_level = 0;
	while (NULL != data_element)
	{
		int level = 0;
		if (!GetSubNodeValue(data_element, "level", level) || level < 0 || NULL == ITEMPOOL->GetItem(level)) return -1;

		EquipmentScoreCfg &node = m_equipment_score_cfg[level];
		node.item_id = level;
		//if (last_level + 1 != level) return -2;

		if (!GetSubNodeValue(data_element, "attr_type", node.attr_type) || node.attr_type < 0 || node.attr_type >= BATTLE_ATTR_MAX) return -3;
		if (!GetSubNodeValue(data_element, "score_s", node.score_s) || node.score_s < 0) return -4;
		if (!GetSubNodeValue(data_element, "score_a", node.score_a) || node.score_a < 0) return -5;
		if (!GetSubNodeValue(data_element, "score_b", node.score_b) || node.score_b < 0) return -6;

		//last_level = level;

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int ScoreSystemConfig::InitCapbilityBaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	while (NULL != data_element)
	{
		int prof_base = 0;
		if (!GetSubNodeValue(data_element, "prof", prof_base) || prof_base <= PROF_TYPE_INVALID || prof_base >= PROF_TYPE_MAX) return -1;

		CapabilityCalCfg node_cfg;
		char str[36][32] = { "maxhp", "maxmp", "gongji", "fangyu", "minjie", "huifu", "jingshen", "bisha", "shanduo", "mingzhong",
			"fanji", "mokang", "mogong", "bisha_dmg", "jianren", "def_zhongdu", "def_hunshui", "def_shihua", "def_hunluan", "def_jiuzui",
			"vit", "str", "pow", "agi", "mag", "phy_inc", "mag_inc", "phy_red", "mag_red",
			"xiaozhong", "xiaodi", "zhenshang", "def_zhongshang", "def_chenmo", "fabao", "fabao_dmg" };
		int attr_index[36] = { BATTLE_ATTR_MAX_HP, BATTLE_ATTR_MAX_MP, BATTLE_ATTR_ATTACK, BATTLE_ATTR_DEFENSE, BATTLE_ATTR_AGILITY,
			BATTLE_ATTR_RECOVERY, BATTLE_ATTR_MENTAL, BATTLE_ATTR_CRITICAL, BATTLE_ATTR_DODGE, BATTLE_ATTR_HIT, BATTLE_ATTR_COUNTER_ATTACK,
			BATTLE_ATTR_MAGIC_DEFENSE, BATTLE_ATTR_MAGIC_ATTACK, BATTLE_ATTR_CRITICAL_INC_VALUE, BATTLE_ATTR_CRITICAL_DEC_VALUE,
			BATTLE_ATTR_ANTI_POISON, BATTLE_ATTR_ANTI_FREEZE, BATTLE_ATTR_ANTI_PETRIFY, BATTLE_ATTR_ANTI_CHAOS, BATTLE_ATTR_ANTI_DRUNK,
			BATTLE_ADD_POINT_TYPE_VITALITY ,BATTLE_ADD_POINT_TYPE_STRENGTH ,BATTLE_ADD_POINT_TYPE_POWER ,BATTLE_ADD_POINT_TYPE_AGILITY ,
			BATTLE_ADD_POINT_TYPE_MAGIC, BATTLE_ATTR_PHYSICAL_INC_DAMAGE, BATTLE_ATTR_MAGIC_INC_DAMAGE, BATTLE_ATTR_PHYSICAL_DEC_DAMAGE, 
			BATTLE_ATTR_MAGIC_DEC_DAMAGE, BATTLE_ATTR_CONTROL_HIT, BATTLE_ATTR_ANTI_CONTROL_RATE, BATTLE_ATTR_REAL_DAMAGE,
			BATTLE_ATTR_ANTI_SEVERE_INJURE, BATTLE_ATTR_ANTI_SILENCE, BATTLE_ATTR_MAGIC_CRIRICAL, BATTLE_ATTR_MAGIC_CRIRICAL_INC_DAMAGE  };

		for (int i = 0; i < ARRAY_LENGTH(str) && i < ARRAY_LENGTH(attr_index); i++)
		{
			if (!GetSubNodeValue(data_element, str[i], node_cfg.cap_base[attr_index[i]]) || node_cfg.cap_base[attr_index[i]] < 0)
			{
				return -100 - i;
			}
		}
		int prof_index = prof_base / PROFESSION_BASE;
		if(prof_index < 0 || prof_index >= PROFESSION_TYPE_NUM) return -2;

		capability_base_cfg[prof_index] = node_cfg;
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int ScoreSystemConfig::InitProfSkillScoreCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	while (NULL != data_element)
	{
		ProfSkillScoreKey job_key;
		if (!GetSubNodeValue(data_element, "job_type", job_key.job_type) || job_key.job_type <= 0 || job_key.job_type >= PROFESSION_TYPE_NUM) return -1;
		if (!GetSubNodeValue(data_element, "skill_level", job_key.skill_level) || job_key.skill_level <= 0 || job_key.skill_level > MAX_BATTLE_SKILL_LEVEL) return -2;

		int skill_score = 0;
		if (!GetSubNodeValue(data_element, "skill_score", skill_score) || skill_score < 0) return -3;

		m_prof_skill_score_cfg[job_key] = skill_score;
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int ScoreSystemConfig::InitTrainSkillScoreCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	while (NULL != data_element)
	{
		TrainSkillScoreKey key;
		if (!GetSubNodeValue(data_element, "train_type", key.train_skill_type) 
			|| key.train_skill_type < TRAIN_BEGIN 
			|| key.train_skill_type >= TRAIN_ENG)
		{
			return -1;
		}

		if (!GetSubNodeValue(data_element, "train_level", key.train_skill_level)
			|| key.train_skill_level < 0)
		{
			return -2;
		}

		int train_score = 0;
		if (!GetSubNodeValue(data_element, "train_score", train_score) 
			|| train_score < 0)
		{
			return -3;
		}

		m_train_skill_score_cfg[key] = train_score;

		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int ScoreSystemConfig::InitSkillScoreCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	while (NULL != data_element)
	{
		SkillScoreCfg skill_cfg;
		
		if (!GetSubNodeValue(data_element, "skill_id", skill_cfg.skill_id)) return -1;

		if (!GetSubNodeValue(data_element, "skill_level", skill_cfg.skill_level) || skill_cfg.skill_level <= 0 || skill_cfg.skill_level > MAX_BATTLE_SKILL_LEVEL) return -2;

		if (!GetSubNodeValue(data_element, "skill_score", skill_cfg.skill_score) || skill_cfg.skill_score < 0) return -3;

		m_skill_score_vec.push_back(skill_cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int ScoreSystemConfig::InitScoreTaskCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -544;
	}

	while (NULL != data_element)
	{
		int level = 0, role_score = 0, money = 0;
		if (!GetSubNodeValue(data_element, "level", level) || level < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "role_score", role_score) || role_score < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "money", money) || money < 0) return -__LINE__;

		if (level == 0 && role_score == 0)
		{
			level = 1;
		}

		ScoreTaskCfg st_cfg;
		st_cfg.level = level;
		st_cfg.role_score = role_score;
		st_cfg.money = money;
		m_score_task.push_back(st_cfg);

		if ((int)m_score_task.size() > MAX_SCORE_TASK_NUM) return -__LINE__;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int ScoreSystemConfig::InitHolyEquipScoreCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	int huoyue = 0;
	if (!GetSubNodeValue(data_element, "huoyue", huoyue) || huoyue < 0) return -1;
	m_holy_equip_score_cfg[HolyItem::HOLY_ATTR_TYPE_ACTIVENESS] = huoyue;

	int damage = 0;
	if (!GetSubNodeValue(data_element, "damage", damage) || damage < 0) return -2;
	m_holy_equip_score_cfg[HolyItem::HOLY_ATTR_TYPE_ADD_ATTACK] = damage;

	int speed = 0;
	if (!GetSubNodeValue(data_element, "speed", speed) || speed < 0) return -3;
	m_holy_equip_score_cfg[HolyItem::HOLY_ATTR_TYPE_SPEED] = speed;

	int huifu = 0;
	if (!GetSubNodeValue(data_element, "huifu", huifu) || huifu < 0) return -4;
	m_holy_equip_score_cfg[HolyItem::HOLY_ATTR_TYPE_RECOVER] = huifu;

	int yuanzhu = 0;
	if (!GetSubNodeValue(data_element, "yuanzhu", yuanzhu) || yuanzhu < 0) return -5;
	m_holy_equip_score_cfg[HolyItem::HOLY_ATTR_TYPE_HELP] = yuanzhu;

	int luobao = 0;
	if (!GetSubNodeValue(data_element, "luobao", luobao) || luobao < 0) return -6;
	m_holy_equip_score_cfg[HolyItem::HOLY_ATTR_TYPE_FALL] = luobao;

	int kangluobao = 0;
	if (!GetSubNodeValue(data_element, "kangluobao", kangluobao) || kangluobao < 0) return -7;
	m_holy_equip_score_cfg[HolyItem::HOLY_ATTR_TYPE_COPE_FALL] = kangluobao;

	return 0;
}
