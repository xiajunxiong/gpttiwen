#include "battle_special_effect_pool.hpp"
#include "servercommon/configcommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/struct/battle/battle_special_effect_def.hpp"

BattleSpecialEffectPool* g_bse_pool = NULL;

BattleSpecialEffectPool& BattleSpecialEffectPool::GetInstance()
{
	if (NULL == g_bse_pool)
	{
		g_bse_pool = new BattleSpecialEffectPool();
	}
	return *g_bse_pool;
}

BattleSpecialEffectPool::BattleSpecialEffectPool()
{
	memset(m_special_effect_list, 0, sizeof(m_special_effect_list));
}

BattleSpecialEffectPool::~BattleSpecialEffectPool()
{
	this->Release();
}

bool BattleSpecialEffectPool::Init(const std::string &configname, std::string *err)
{
	char errinfo[1024] = { 0 };
	int iRet = 0;

	TiXmlDocument document;

	if (configname == "" || !document.LoadFile(configname))
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: Load BattleSpecialEffectPool Error,\n %s.", configname.c_str(), document.ErrorDesc());
		*err = errinfo;
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();

	if (NULL == RootElement)
	{
		*err = configname + ": xml root node error.";
		return false;
	}

	{
		// spe_list
		TiXmlElement *element = RootElement->FirstChildElement("spe_list");
		if (NULL == element)
		{
			*err = configname + ": no [spe_list].";
			return false;
		}

		iRet = InitSpecialEffectList(element);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitSpecialEffectList failed %d", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	{
		// spe_timing
		TiXmlElement *element = RootElement->FirstChildElement("spe_timing");
		if (NULL == element)
		{
			*err = configname + ": no [spe_timing].";
			return false;
		}

		iRet = InitSpecialEffectDetail(element, SPECIAL_EFFECT_TYPE_TIMING);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitSpecialEffectDetail SPECIAL_EFFECT_TYPE_TIMING failed %d", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	{
		// spe_damage_control
		TiXmlElement *element = RootElement->FirstChildElement("spe_damage_control");
		if (NULL == element)
		{
			*err = configname + ": no [spe_damage_control].";
			return false;
		}

		iRet = InitSpecialEffectDetail(element, SPECIAL_EFFECT_TYPE_DAMAGE_CONTROL);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitSpecialEffectDetail SPECIAL_EFFECT_TYPE_DAMAGE_CONTROL failed %d", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	{
		// spe_add_attr
		TiXmlElement *element = RootElement->FirstChildElement("spe_add_attr");
		if (NULL == element)
		{
			*err = configname + ": no [spe_add_attr].";
			return false;
		}

		iRet = InitSpecialEffectDetail(element, SPECIAL_EFFECT_TYPE_ADD_ATTR);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitSpecialEffectDetail SPECIAL_EFFECT_TYPE_ADD_ATTR failed %d", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	{
		// spe_skill_hit
		TiXmlElement *element = RootElement->FirstChildElement("spe_skill_hit");
		if (NULL == element)
		{
			*err = configname + ": no [spe_skill_hit].";
			return false;
		}

		iRet = InitSpecialEffectDetail(element, SPECIAL_EFFECT_TYPE_SKILL_HIT);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitSpecialEffectDetail SPECIAL_EFFECT_TYPE_SKILL_HIT failed %d", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	{
		// spe_other
		TiXmlElement *element = RootElement->FirstChildElement("spe_other");
		if (NULL == element)
		{
			*err = configname + ": no [spe_other].";
			return false;
		}
	
		iRet = InitSpecialEffectDetail(element, SPECIAL_EFFECT_TYPE_OTHER);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitSpecialEffectDetail SPECIAL_EFFECT_TYPE_OTHER failed %d", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	{
		// spe_group
		TiXmlElement *element = RootElement->FirstChildElement("spe_group");
		if (NULL == element)
		{
			*err = configname + ": no [spe_group].";
			return false;
		}

		iRet = InitSpecialEffectGroup(element);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitSpecialEffectGroup failed, ret[%d]", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	{
		// spe_halo
		TiXmlElement *element = RootElement->FirstChildElement("spe_halo");
		if (NULL == element)
		{
			*err = configname + ": no [spe_halo].";
			return false;
		}

		iRet = InitHaloList(element);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitHaloList failed %d", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	{
		// deep_friendship_sp
		TiXmlElement *element = RootElement->FirstChildElement("deep_friendship_sp");
		if (NULL == element)
		{
			*err = configname + ": no [deep_friendship_sp].";
			return false;
		}

		iRet = InitJieyiSpidHash(element);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitJieyiSpidHash failed %d", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	{
		// pet_weapon_sp
		TiXmlElement *element = RootElement->FirstChildElement("pet_weapon_sp");
		if (NULL == element)
		{
			*err = configname + ": no [pet_weapon_sp].";
			return false;
		}

		iRet = InitOnlyEffectOnPerformSkillMap(element);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitOnleyEffectOnPerformSkillMap failed %d", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	if (!this->CheckEffectDetail())
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: CheckEffectDetail failed", configname.c_str());
		*err = errinfo;
		return false;
	}

	if (!this->CheckEffectGroup())
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: CheckEffectGroup failed", configname.c_str());
		*err = errinfo;
		return false;
	}

	return true;
}

bool BattleSpecialEffectPool::Reload(const std::string &configname, std::string *err)
{
	BattleSpecialEffectPool *temp = new BattleSpecialEffectPool();

	if (!temp->Init(configname, err))
	{
		delete temp; temp = NULL;
		return false;
	}

	if (NULL != g_bse_pool) delete g_bse_pool;

	g_bse_pool = temp;

	return true;
}

const SpecialEffectCfg* BattleSpecialEffectPool::GetSpecialEffectCfg(int sp_id)
{
	if (sp_id < 0 || sp_id >= SPECIAL_EFFECT_MAX_NUM) return NULL;

	if (NULL == m_special_effect_list[sp_id]) return NULL;

	return m_special_effect_list[sp_id];
}

const EffectDetailCfg* BattleSpecialEffectPool::GetEffectDetailCfg(const SpecialEffectKey& key)
{
	EffectDetailMap::iterator it = m_effect_detail_map.find(key);
	if (it == m_effect_detail_map.end()) return NULL;

	return &it->second;
}

const EffectGroupCfg* BattleSpecialEffectPool::GetEffectGroupCfg(int group_id)
{
	EffectGroupMap::iterator it = m_effect_group_map.find(group_id);
	if (it == m_effect_group_map.end()) return NULL;

	return &it->second;
}

const BattleHaloCfg* BattleSpecialEffectPool::GetHaloCfg(int halo_id)
{
	HaloCfgMap::iterator it = m_halo_cfg_map.find(halo_id);
	if (it == m_halo_cfg_map.end()) return NULL;

	return &it->second;
}

int BattleSpecialEffectPool::GetMedalSpid(int sp_id, int grade)
{
	const SpecialEffectCfg * cfg=this->GetSpecialEffectCfg(sp_id);
	if (cfg == NULL)
	{
		return 0;
	}

	for (int i = 0; i < ARRAY_LENGTH(m_special_effect_list); ++i)
	{
		if (NULL != m_special_effect_list[i] && m_special_effect_list[i]->sp_type == cfg->sp_type && m_special_effect_list[i]->medal_grade == grade)
		{
			return m_special_effect_list[i]->sp_id;
		}
	}

	return 0;
}

int BattleSpecialEffectPool::RandomSpecialEffect(int group_id)
{
	const EffectGroupCfg* cfg = this->GetEffectGroupCfg(group_id);
	if (NULL != cfg && cfg->effect_num > 0 && cfg->total_rate > 0)
	{
		static const int TOTAL_RATE = 10000;
		int r = RandomNum(TOTAL_RATE);
		for (int i = 0; i < cfg->effect_num; ++i)
		{
			if (r < cfg->effect_list[i].rate)
			{
				return cfg->effect_list[i].sp_id;
			}
		}
	}

	return 0;
}

bool BattleSpecialEffectPool::CheckSpidMatchGroupid(int sp_id, int group_id)
{
	const EffectGroupCfg* cfg = this->GetEffectGroupCfg(group_id);
	if (NULL != cfg && cfg->effect_num > 0)
	{
		for (int i = 0; i < cfg->effect_num; ++i)
		{
			if (cfg->effect_list[i].sp_id == sp_id)
			{
				return true;
			}
		}
	}

	return false;
}

bool BattleSpecialEffectPool::CheckOnlyForSkillPerformBeforeAdd(int sp_id, std::vector<int>* out_skill_list)
{
	OnlyEffectOnPerformSkillMap::iterator it = m_only_effect_on_perform_skill_map.find(sp_id);
	if (it != m_only_effect_on_perform_skill_map.end())
	{
		if (NULL != out_skill_list) *out_skill_list = it->second;

		return true;
	}

	return false;
}

void BattleSpecialEffectPool::Release()
{
	for (int i = 0; i < SPECIAL_EFFECT_MAX_NUM; ++i)
	{
		if (NULL != m_special_effect_list[i])
		{
			delete m_special_effect_list[i];
			m_special_effect_list[i] = NULL;
		}
	}

	m_effect_detail_map.clear();
	m_effect_group_map.clear();
}

int BattleSpecialEffectPool::InitSpecialEffectList(TiXmlElement *RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	while (dataElement)
	{
		int sp_id = 0;
		if (!GetSubNodeValue(dataElement, "sp_id", sp_id) || sp_id <= 0 || sp_id >= SPECIAL_EFFECT_MAX_NUM)
		{
			return -1;
		}
		if (NULL != m_special_effect_list[sp_id])
		{
			printf("BattleSpecialEffectPool::InitSpecialEffectList spid[%d] already exist!!!\n", sp_id);
			return -2;
		}

		SpecialEffectCfg* cfg = new SpecialEffectCfg();
		cfg->sp_id = sp_id;

		if (!GetSubNodeValue(dataElement, "effect_num", cfg->effect_num) || cfg->effect_num <= 0 || cfg->effect_num > EFFECT_DETAIL_NUM)
		{
			delete cfg;
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "trigger_num", cfg->max_trigger_times) || cfg->max_trigger_times < -1)
		{
			delete cfg;
			return -4;
		}

		int is_heart_skill = 0;
		if (!GetSubNodeValue(dataElement, "is_heart_skill", is_heart_skill))
		{
			delete cfg;
			return -5;
		}
		cfg->is_heart_skill = (0 != is_heart_skill) ? true : false;

		int is_jieyi = 0;
		if (!GetSubNodeValue(dataElement, "is_friendship_skill", is_jieyi))
		{
			delete cfg;
			return -55;
		}
		cfg->is_jieyi = (is_jieyi > 0);

		int jieyi_effect_pve = 0;
		if (!GetSubNodeValue(dataElement, "friendship_pve_effect", jieyi_effect_pve))
		{
			delete cfg;
			return -56;
		}
		cfg->jieyi_only_pve_active = (jieyi_effect_pve > 0);

		int is_qingyuan = 0;
		if (!GetSubNodeValue(dataElement, "is_marriage", is_qingyuan))
		{
			delete cfg;
			return -57;
		}
		cfg->is_qingyuan = (0 != is_qingyuan) ? true : false;

		if (!GetSubNodeValue(dataElement, "sp_type", cfg->sp_type) || cfg->sp_type < 0)
		{
			delete cfg;
			return -41;
		}

		if (cfg->sp_type >= BATTLE_SPECIAL_EFFECT_TYPE_NUM)
		{
			printf("InitSpecialEffectList sp_type[%d] Too Many sp_type, Contact Us To Enlarge The Limit\n", cfg->sp_type);
			delete cfg;
			return -411;
		}

		if (!GetSubNodeValue(dataElement, "sp_priority", cfg->sp_priority))
		{
			delete cfg;
			return -42;
		}

		if (!GetSubNodeValue(dataElement, "role_id", cfg->prof_type) || cfg->prof_type < 0 || cfg->prof_type >= PROFESSION_TYPE_NUM)
		{
			delete cfg;
			return -43;
		}

		if (!GetSubNodeValue(dataElement, "sp_rank", cfg->medal_grade))
		{
			delete cfg;
			return -44;
		}

		int invalid_on_silent = 0;
		if (!GetSubNodeValue(dataElement, "invalid_on_silent", invalid_on_silent))
		{
			delete cfg;
			return -45;
		}
		cfg->invalid_on_silent = invalid_on_silent > 0;

		int is_hide = 0;
		if (!GetSubNodeValue(dataElement, "is_hide", is_hide))
		{
			delete cfg;
			return -45;
		}
		cfg->is_show_on_list = (is_hide == 0);

		for (int i = 0; i < cfg->effect_num; ++i)
		{
			char tmp1[20] = { 0 };
			char tmp2[20] = { 0 };

			SNPRINTF(tmp1, sizeof(tmp1), "effect%d_type", i + 1);
			SNPRINTF(tmp2, sizeof(tmp2), "effect%d_id", i + 1);

			if (!GetSubNodeValue(dataElement, tmp1, cfg->effect_detail_list[i].effect_type))
			{
				delete cfg;
				return -300 - i;
			}

			if (!GetSubNodeValue(dataElement, tmp2, cfg->effect_detail_list[i].effect_id))
			{
				delete cfg;
				return -3000 - i;
			}
		}
		m_special_effect_list[sp_id] = cfg;

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int BattleSpecialEffectPool::InitSpecialEffectDetail(TiXmlElement *RootElement, short effect_type)
{
	if (effect_type <= SPECIAL_EFFECT_TYPE_INVALID || effect_type >= SPECIAL_EFFECT_TYPE_MAX)
	{
		return -20000;
	}

	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	while (dataElement)
	{
		short effect_id = 0;
		if (!GetSubNodeValue(dataElement, "effect_id", effect_id) || effect_id <= 0)
		{
			return -1;
		}
		static SpecialEffectKey key;
		key.effect_id = effect_id;
		key.effect_type = effect_type;

		if (m_effect_detail_map.find(key) != m_effect_detail_map.end())
		{
			printf("BattleSpecialEffectPool::InitSpecialEffectDetail Duplicate[%d,%d]\n", key.effect_id, key.effect_type);
			return -2;
		}
		
		EffectDetailCfg cfg;
		cfg.effect_id = effect_id;
		cfg.param_num = 0;
		memset(cfg.param_list, 0, sizeof(cfg.param_list));
		
		if (SPECIAL_EFFECT_TYPE_TIMING == effect_type)
		{
			if (!GetSubNodeValue(dataElement, "timing", cfg.timing) || cfg.timing <= SPE_TIMING_INVALID || cfg.timing >= SPE_TIMING_MAX)
			{
				return -3;
			}
			if (!GetSubNodeValue(dataElement, "valid_round", cfg.valid_round) || cfg.valid_round < 0)
			{
				return -200;
			}
		}
		
		if (!GetSubNodeValue(dataElement, "type", cfg.effect_sub_type) || !CheckEffectSubType(effect_type, cfg.effect_sub_type))
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "param_num", cfg.param_num) || cfg.param_num < 0 || cfg.param_num > EFFECT_DETAIL_PARAM_NUM)
		{
			return -5;
		}

		for (int i = 0; i < cfg.param_num; ++i)
		{
			char tmp[20] = { 0 };
			SNPRINTF(tmp, sizeof(tmp), "param%d", i + 1);
			if (!GetSubNodeValue(dataElement, tmp, cfg.param_list[i]))
			{
				return -6;
			}
		}
		if (SPECIAL_EFFECT_TYPE_DAMAGE_CONTROL == effect_type && cfg.effect_sub_type == SPE_DAMAGE_UP_BY_TARGET_BUFF && cfg.param_list[0] < BATTLE_PERCENT_NUM)
		{
			return -66;
		}

		if (SPECIAL_EFFECT_TYPE_SKILL_HIT == effect_type)
		{
			if (!GetSubNodeValue(dataElement, "cond_type", cfg.cond_type) || cfg.cond_type <= SPE_SKILL_COND_INVALID || cfg.cond_type >= SPE_SKILL_COND_MAX)
			{
				return -7;
			}

			GetSubNodeValue(dataElement, "cond_param1", cfg.cond_param1);
			GetSubNodeValue(dataElement, "cond_param2", cfg.cond_param2);
			GetSubNodeValue(dataElement, "cond_param3", cfg.cond_param3);
			GetSubNodeValue(dataElement, "cond_param4", cfg.cond_param4);
		}

		m_effect_detail_map.insert(EffectDetailMap::value_type(key, cfg));

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int BattleSpecialEffectPool::InitSpecialEffectGroup(TiXmlElement *RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	while (dataElement)
	{
		int group_id = 0;
		if (!GetSubNodeValue(dataElement, "effect_group", group_id) || group_id <= 0)
		{
			return -1;
		}

		EffectGroupMap::iterator it = m_effect_group_map.find(group_id);
		if (it == m_effect_group_map.end())
		{
			static EffectGroupCfg tmp_cfg;
			tmp_cfg.group_id = group_id;
			tmp_cfg.effect_num = 0;
			tmp_cfg.total_rate = 0;

			it = m_effect_group_map.insert(EffectGroupMap::value_type(group_id, tmp_cfg)).first;
		}
		
		EffectGroupCfg* cfg = &it->second;

		if (cfg->effect_num >= EffectGroupCfg::MAX_EFFECT_NUM_IN_ONE_GROUP)
		{
			return -2;
		}
		
		int sp_id = 0;
		if (!GetSubNodeValue(dataElement, "sp_id", sp_id) || sp_id <= 0)
		{
			return -3;
		}

		int rate = 0;
		if (!GetSubNodeValue(dataElement, "rate", rate) || rate <= 0)
		{
			return -4;
		}

		cfg->AddEffect(sp_id, rate);

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int BattleSpecialEffectPool::InitHaloList(TiXmlElement *RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	while (NULL != dataElement)
	{
		int halo_id = 0;
		if (!GetSubNodeValue(dataElement, "effect_id", halo_id) || halo_id < 0)
		{
			return -1;
		}

		if (m_halo_cfg_map.find(halo_id) != m_halo_cfg_map.end())
		{
			return -111; // 重复了
		}

		BattleHaloCfg cfg;
		cfg.halo_id = halo_id;

		if (!GetSubNodeValue(dataElement, "type", cfg.effect_type) || cfg.effect_type <= HALO_EFFECT_TYPE_INVALID || cfg.effect_type >= HALO_EFFECT_TYPE_MAX)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "target_side", cfg.target_side) || cfg.target_side < 0 || cfg.target_side > 1)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "range_type", cfg.range_type) || cfg.range_type < 0 || cfg.range_type >= HALO_RANGE_PRIORITY_NUM)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "param1", cfg.param.param1))
		{
			return -5;
		}

		if (!GetSubNodeValue(dataElement, "param2", cfg.param.param2))
		{
			return -6;
		}

		if (!GetSubNodeValue(dataElement, "param3", cfg.param.param3))
		{
			return -7;
		}

		if (!GetSubNodeValue(dataElement, "param4", cfg.param.param4))
		{
			return -8;
		}

		m_halo_cfg_map.insert(HaloCfgMap::value_type(halo_id, cfg));

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int BattleSpecialEffectPool::InitJieyiSpidHash(TiXmlElement *RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return 0; // 允许没有
	}

	while (NULL != dataElement)
	{
		int origin_sp_id = 0;
		if (!GetSubNodeValue(dataElement, "origin_sp_id", origin_sp_id) || NULL == this->GetSpecialEffectCfg(origin_sp_id))
		{
			return -1;
		}

		int jieyi_num = 0;
		if (!GetSubNodeValue(dataElement, "number", jieyi_num) || jieyi_num < 2 || jieyi_num > 5)
		{
			return -2;
		}

		int upgraded_sp_id = 0;
		if (!GetSubNodeValue(dataElement, "sp_id", upgraded_sp_id) || NULL == this->GetSpecialEffectCfg(upgraded_sp_id))
		{
			return -3;
		}

		JieyiOriginSpid key;
		key.origin_spid = origin_sp_id;
		key.jieyi_num = jieyi_num;
		if (m_jieyi_spid_hash.find(key) != m_jieyi_spid_hash.end())
		{
			return -111;
		}

		m_jieyi_spid_hash.insert(JieyiSpidHash::value_type(key, upgraded_sp_id));

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int BattleSpecialEffectPool::InitOnlyEffectOnPerformSkillMap(TiXmlElement *RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return 0; // 允许没有
	}

	while (NULL != dataElement)
	{
		int skill_id = 0;
		if (!GetSubNodeValue(dataElement, "skill_id", skill_id))
		{
			return -1;
		}

		int sp_id = 0;
		if (!GetSubNodeValue(dataElement, "sp_id", sp_id))
		{
			return -2;
		}

		OnlyEffectOnPerformSkillMap::iterator it = m_only_effect_on_perform_skill_map.find(sp_id);
		if (it != m_only_effect_on_perform_skill_map.end())
		{
			it->second.push_back(skill_id);
		}
		else
		{
			std::vector<int> skill_list;
			skill_list.push_back(skill_id);
			m_only_effect_on_perform_skill_map.insert(OnlyEffectOnPerformSkillMap::value_type(sp_id, skill_list));
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

bool BattleSpecialEffectPool::CheckEffectDetail()
{
	for (int i = 0; i < SPECIAL_EFFECT_MAX_NUM; ++i)
	{
		if (NULL != m_special_effect_list[i])
		{
			for (int idx = 0; idx < m_special_effect_list[i]->effect_num; ++idx)
			{
				SpecialEffectKey& key = m_special_effect_list[i]->effect_detail_list[idx];
				if (key.effect_type != SPECIAL_EFFECT_TYPE_HALO)
				{			
					if (m_effect_detail_map.find(key) == m_effect_detail_map.end())
					{
						printf("BattleSpecialEffectPool::CheckEffectDetail type[%d] id[%d] sp_id[%d] not found\n",
							key.effect_type, key.effect_id, m_special_effect_list[i]->sp_id);
						return false; // 没有找到key
					}
				}
				else
				{
					if (m_halo_cfg_map.find(key.effect_id) == m_halo_cfg_map.end())
					{
						printf("BattleSpecialEffectPool::CheckEffectDetail type[%d] id[%d] sp_id[%d] not found\n",
							key.effect_type, key.effect_id, m_special_effect_list[i]->sp_id);
						return false; // 没有找到key
					}
				}
			}
		}
	}

	return true;
}

bool BattleSpecialEffectPool::CheckEffectGroup()
{
	EffectGroupMap::iterator it = m_effect_group_map.begin();
	for (; it != m_effect_group_map.end(); ++it)
	{
		for (int i = 0; i < it->second.effect_num; ++i)
		{
			int sp_id = it->second.effect_list[i].sp_id;
			if (NULL == this->GetSpecialEffectCfg(sp_id))
			{
				printf("BattleSpecialEffectPool::CheckEffectGroup sp_id[%d] not found", sp_id);
				return false;
			}
		}
	}

	return true;
}

bool BattleSpecialEffectPool::CheckEffectSubType(short effect_type, short effect_sub_type)
{
	if (SPECIAL_EFFECT_TYPE_TIMING == effect_type)
	{
		if (effect_sub_type <= SPE_TRIGGER_EFFECT_INVALID || effect_sub_type >= SPE_TRIGGER_EFFECT_MAX)
		{
			return false;
		}
	}
	else if (SPECIAL_EFFECT_TYPE_DAMAGE_CONTROL == effect_type)
	{
		if (effect_sub_type <= SPE_DAMAGE_INVALID || effect_sub_type >= SPE_DAMAGE_MAX)
		{
			return false;
		}
	}
	else if (SPECIAL_EFFECT_TYPE_ADD_ATTR == effect_type)
	{
		if (effect_sub_type <= SPE_ATTR_INVALID || effect_sub_type >= SPE_ATTR_MAX)
		{
			return false;
		}
	}
	else if (SPECIAL_EFFECT_TYPE_SKILL_HIT == effect_type)
	{
		if (effect_sub_type <= SPE_TRIGGER_EFFECT_INVALID || effect_sub_type >= SPE_TRIGGER_EFFECT_MAX)
		{
			return false;
		}
	}
	else if (SPECIAL_EFFECT_TYPE_OTHER == effect_type)
	{
		if (effect_sub_type <= SPE_OTHER_INVALID || effect_sub_type >= SPE_OTHER_MAX)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}
