#include "partnerconfig.hpp"
#include "item/itembase.h"
#include "skill/skillpool.hpp"
#include "gamelog.h"
#include "checkresourcecenter.hpp"
#include "servercommon/performancecatch.hpp"
#include "task/taskpool.h"

#define PARTNER_CONFIG_PERFORMANCE_CHECK

PartnerConfig::PartnerConfig() : m_partner_num(0), max_upgrade_level(0)
{
}

PartnerConfig::~PartnerConfig()
{
	for (std::map<PartnerCfgHead, std::vector<PartnerBreakThCfg> >::iterator it = m_v_breakth.begin(); it != m_v_breakth.end(); it++)
	{
		std::vector<PartnerBreakThCfg>().swap(it->second);
	}

	for (std::map<int, std::vector<FettersGroupsCfg> >::iterator it = m_v_f_group.begin();it != m_v_f_group.end();it++)
	{
		std::vector<FettersGroupsCfg>().swap(it->second);
	}

	for (std::map<int, std::vector<PartnerIntensifyCfg> >::iterator it = m_partner_intensify_cfg.begin(); it != m_partner_intensify_cfg.end(); it++)
	{
		std::vector<PartnerIntensifyCfg>().swap(it->second);
	}
}

bool PartnerConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitPartnerOtherCfg);

	LOAD_CONFIG_2("soar_arms", InitPartnerSoarArmsCfg);			// 飞升武器配置必须放在基础信息读取前面
	//LOAD_CONFIG("qingyuan_group", InitFettersGroupsCfg);		// 必须放在基本信息读取前面
	LOAD_CONFIG("gift_list", InitPartnerTalentCfg);				// 必须放在升阶信息读取前面

	LOAD_CONFIG("partner_info", InitPartnerBasicCfg);			// 必须放在情缘组读取的后面，因为会检查伙伴情缘组是否存在
	
	LOAD_CONFIG("skill_list", InitSkillsUnlockCfg);				// 必须放在基本信息读取后面
	
	LOAD_CONFIG("partner_upgrade", InitPartnerUpGradeCfg);		// 必须放在基本信息读取和天赋信息读取后面，因为会检查伙伴信息和天赋技能是否存在
	
	LOAD_CONFIG("partner_levelup", InitPartnerAttribute);		// 必须放在基本信息读取后面，因为会检查伙伴信息是否存在

	LOAD_CONFIG("partner_star_point", InitPartnerBreakThCfg);	// 必须放在基本信息读取后面，因为会检查伙伴信息是否存在
																
	//LOAD_CONFIG("qingyuan_list", InitPartnerFetterCfg);			// 必须放在基本信息读取后面，因为会检查伙伴信息是否存在
	
	LOAD_CONFIG("parnter_quality_upgrade", InitPartnerQualityUpCfg);		//必须放在基本信息读取之后
	LOAD_CONFIG("partner_friendly", InitPartnerUpGradeRoleAttrCfg);		//必须放在基本信息读取之后
	LOAD_CONFIG("partner_biography", InitPartnerBiographyCfg);		//必须放在基本信息读取和进阶信息读取后面

	LOAD_CONFIG("compose", InitPartnerComposeGroupCfg);		
	LOAD_CONFIG("parnter_page_compose", InitPartnerComposePageCfg);		//必须放合成组后面读取
	LOAD_CONFIG("star_deed", InitPactSaveIndexCfg);
	LOAD_CONFIG_2("soar", InitPartnerSoarCfg);					// 飞升配置必须放在飞升武器配置后面读取
	LOAD_CONFIG_2("parnter_intensify", InitPartnerIntensifyCfg);		// 伙伴强化

	/*
	{
		iRet = this->CheckFetterGroupsCfg();	// 检查情缘组里是否有不存在的的情缘
		if (iRet)
		{
			sprintf(errinfo, "PartnerConfig::CheckFetterGroupsCfg failed %d", iRet);
			*err = errinfo;
			return false;
		}
	}
	*/

	this->InitLevelSkillCfgMap();

	return true;
}

/*int PartnerConfig::GetPartnerIDBySeq(int seq)
{
	return PARTNER_ID_START + seq;
}*/

int PartnerConfig::GainARandFetter(int partner_id, int f_group_id, int exclude_f_id)		// TODO: 考虑如何保证必定摇出情缘
{
	std::map<int, std::vector<FettersGroupsCfg> >::iterator m_it = m_v_f_group.find(f_group_id);
	if (m_v_f_group.end() == m_it) return 0;

	int exclude_mark = 0;
	std::map<int, PartnerFetterCfg>::iterator t_it = m_s_fetters.find(exclude_f_id);
	if (t_it != m_s_fetters.end()) exclude_mark = t_it->second.fetter_mark;
	
	int rand_prob = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
	for (std::vector<FettersGroupsCfg>::iterator v_it = m_it->second.begin(); v_it != m_it->second.end(); ++v_it)
	{
		const PartnerFetterCfg & t_pfc_c = m_s_fetters.find(v_it->fetter_id)->second;
		if (rand_prob < v_it->rand_rate && partner_id != t_pfc_c.need_partner && exclude_mark != t_pfc_c.fetter_mark)
		{ 
			return v_it->fetter_id;
		}
		
		rand_prob -= v_it->rand_rate;
	}

	return 0;
}

int PartnerConfig::PartnerActiveCheck(int check_type, int param)
{
	if (0 == param) return PARTNER_ERROR_SEQ;

	for (int partner_id = PARTNER_ID_START; partner_id < PARTNER_ID_START + m_partner_num; ++partner_id)
	{
		if (m_basic_cfg_list[partner_id].Invalid()) continue;

		PartnerBasicCfg* cfg = &m_basic_cfg_list[partner_id];
		int compared = -1;
		switch (check_type)
		{
		case UNLOCK_TYPE_TASK:
			compared = cfg->unlock_task;
			break;
		case UNLOCK_TYPE_ITEM:
			compared = cfg->unlock_item;
			break;
		case UNLOCK_TYPE_ITEM_NOREX:
			compared = cfg->unlock_item_norex;
			break;
		default:
			break;
		}
		
		if (param == compared)
		{
			return partner_id;
		}
	}
	
	return PARTNER_ERROR_SEQ;
}

int PartnerConfig::GetPartnerQualityByItemId(int item_id)
{
	std::map<int, int>::iterator it = m_basic_cfg_map.find(item_id);
	if(it == m_basic_cfg_map.end()) return -1;

	return it->second;
}

const PartnerLevelCfg * PartnerConfig::GetPartnerLevelCfg(int partner, int level)
{
	if (partner < PARTNER_ID_START || partner >= PARTNER_NUM_MAX)
	{
		return NULL;
	}

	if (level < 0 || level > MAX_ROLE_LEVEL)
	{
		return NULL;
	}
	
	return &m_lv_attr_cfg_list[partner][level];
}

const PartnerBasicCfg * PartnerConfig::GetPartnerBasicCfg(int partner_id)
{
	if (partner_id < PARTNER_ID_START || partner_id >= PARTNER_NUM_MAX) return NULL;

	if (m_basic_cfg_list[partner_id].Invalid()) return NULL;

	return &m_basic_cfg_list[partner_id];
}

const PartnerUpGradeCfg * PartnerConfig::GetCurGradeAttrCfg(int partner_id, int grade_lv)
{
	std::map<PartnerCfgHead, PartnerUpGradeCfg>::iterator m_it = m_s_upgrade.find(PartnerCfgHead(partner_id, grade_lv));
	if (m_s_upgrade.end() != m_it) return & m_it->second;

	return NULL;
}

const PartnerBreakThCfg * PartnerConfig::GetCurAttrBreakCfg(int partner_id, int grade_lv, int break_seq)
{
	std::map<PartnerCfgHead, std::vector<PartnerBreakThCfg> >::iterator m_it = m_v_breakth.find(PartnerCfgHead(partner_id, grade_lv));
	if (m_v_breakth.end() == m_it) return NULL;		// 没找到
	
	for (std::vector<PartnerBreakThCfg>::iterator v_it = m_it->second.begin(); v_it != m_it->second.end(); ++v_it)
	{
		if (break_seq == v_it->seq) return & *v_it;
	}

	return NULL;
}

const PartnerTalentCfg * PartnerConfig::GetPTalentSkillCfg(int talent_id, int talent_lv)
{
	std::map<PartnerCfgHead, PartnerTalentCfg>::iterator m_it = m_s_talents.find(PartnerCfgHead(talent_id, talent_lv));
	if (m_s_talents.end() != m_it) return & m_it->second;

	return NULL;
}

const PartnerFetterCfg * PartnerConfig::GetPartnerFetterCfg(int fetter_id)
{
	std::map<int, PartnerFetterCfg>::iterator it = m_s_fetters.find(fetter_id);
	if (m_s_fetters.end() != it) return & it->second;

	return NULL;
}

const PartnerUpGradeRoleAttrCfg * PartnerConfig::GetPartnerUpGradeRoleAttrCfg(int ji_ban_seq)
{
	std::map<int, PartnerUpGradeRoleAttrCfg>::iterator it = m_upgrade_role_attr_map.find(ji_ban_seq);
	if (m_upgrade_role_attr_map.end() != it) return &it->second; 

	return NULL;
}

std::vector<int> PartnerConfig::GetAllPartnerId()
{
	std::vector<int> ret;
	ret.reserve(m_partner_num);

	for (int partner_id = PARTNER_ID_START; partner_id < PARTNER_ID_START + m_partner_num; ++partner_id)
	{
		ret.push_back(partner_id);
	}

	return ret;
}

const PartnerBiographyCfg * PartnerConfig::GetPartnerBiographyCfg(int paratner_id, int biography)
{
	std::map<PartnerCfgHead, PartnerBiographyCfg>::iterator it = m_biography_map.find(PartnerCfgHead(paratner_id, biography));
	if(it != m_biography_map.end()) return &it->second;

	return NULL;
}

int PartnerConfig::___QuaKey__(int _partner_id, int _quality_up_type, int quality_segment)
{
	return _partner_id * QualityUpgradeCfg::KEY_BASE_NUM + _quality_up_type * QualityUpgradeCfg::KEY_QUALITY_NUM + quality_segment;
}

const QualityUpgradeCfg * PartnerConfig::GetParnterQualityCfg(int _getcfg_key)
{
	std::map<int, QualityUpgradeCfg>::iterator it = m_idqua_to_upg.find(_getcfg_key);
	if (m_idqua_to_upg.end() != it) return &it->second;

	return NULL;
}

int PartnerConfig::GetCanQualityUpType(int _partner_quality)
{
	/*std::map<int, int>::iterator it = m_qua_to_canup.find(_parttner_quality);
	if (m_qua_to_canup.end() != it) return it->second;*/

	int quality_up_type = 0;
	switch (_partner_quality)
	{
		case PARTNERQUALITYT_BLUE:
		{
			quality_up_type = QUALITY_TYPE_BLUE_TO_PURPLE;
		}
		break;
		case PARTNERQUALITYT_PURPLE:
		{
			quality_up_type = QUALITY_TYPE_PURPLE_TO_ORANGE;
		}
		break;
		case PARTNERQUALITYT_ORANGE:
		{
			quality_up_type = QUALITY_TYPE_ORANGE_TO_RED;
		}
		break;
		case PARTNERQUALITYT_RED:
		{
			quality_up_type = QUALITY_TYPE_RED_TO_GOLD;
		}
		break;
		case PARTNERQUALITYT_GOLD:
		{
			quality_up_type = QUALITY_TYPE_GOLD;
		}
		break;
		default:
		break;
	}

	return quality_up_type;
}

const PartnerLevelSkillCfg* PartnerConfig::GetLevelSkillCfg(int partner_id, int parner_level)
{
	static PartnerCfgHead key;
	key.partner_id = partner_id;
	key.partner_lv = parner_level;

	LevelSkillCfgMap::iterator it = m_level_skill_cfg_map.find(key);
	if (it != m_level_skill_cfg_map.end())
	{
		return &it->second;
	}

	return NULL;
}

const PartnerComposePageCfg * PartnerConfig::GetComposePageCfg(int item_id)
{
	std::map<int, PartnerComposePageCfg>::const_iterator it = m_partner_compose_page_cfg.find(item_id);
	if(it != m_partner_compose_page_cfg.end()) return &it->second;

	return NULL;
}

const PartnerComposeCfg * PartnerConfig::GetRandComposeRewardCfg(int reward_group_id)
{
	std::map<int, PartnerComposeGroupCfg>::const_iterator it = m_partner_compose_cfg.find(reward_group_id);
	if(it == m_partner_compose_cfg.end() || it->second.node_list.empty()) return NULL;

	int rate_num = RandomNum(it->second.rate_count);
	for (int i = 0; i < (int)it->second.node_list.size(); i++)
	{
		if (rate_num < it->second.node_list[i].rate)
		{
			return &it->second.node_list[i];
		}
		rate_num -= it->second.node_list[i].rate;
	}

	return NULL;
}

int PartnerConfig::GetPactSaveIndex(ItemID item_id) const
{
	PactItemIDMap::const_iterator id_it = m_pact_item_id_map.find(item_id);
	if (id_it == m_pact_item_id_map.end())
	{
		return -1;
	}

	return id_it->second;
}

ItemID PartnerConfig::GetPactItemID(int save_index) const
{
	PactSaveIndexMap::const_iterator save_index_it = m_pact_save_index_map.find(save_index);
	if (save_index_it == m_pact_save_index_map.end())
	{
		return ItemID(0);
	}
	return save_index_it->second;
}

const PartnerSoarCfg * PartnerConfig::GetPartnerSoarCfg(int soar_frequency)
{
	std::map<int, PartnerSoarCfg>::const_iterator it = m_partner_soar_cfg.find(soar_frequency);
	if (it == m_partner_soar_cfg.end())
	{
		return NULL;
	}

	return &it->second;
}

const PartnerSoarArmsCfg * PartnerConfig::GetPartnerSoarArmsCfg(int arms_id, int arms_level)
{
	std::map<int, PartnerSoarArmsCfg>::const_iterator it = m_partner_soar_arms_cfg.find(this->GetSoarArmsKey(arms_id, arms_level));
	if (it == m_partner_soar_arms_cfg.end())
	{
		return NULL;
	}

	return &it->second;
}

const PartnerIntensifyCfg * PartnerConfig::GetPartnerIntensifyCfg(int partner_id, int star_level)
{
	std::map<int, std::vector<PartnerIntensifyCfg> >::iterator it = m_partner_intensify_cfg.find(partner_id);
	if (it == m_partner_intensify_cfg.end())
		return NULL;

	if (star_level < 0 || star_level >= (int)it->second.size())
		return NULL;

	return &it->second[star_level];
}

int PartnerConfig::InitPartnerOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "refresh", p_other.fetter_wash_price) || p_other.fetter_wash_price < 0) return -1;
	if (!GetSubNodeValue(root_element, "max_partner_on", p_other.synfight_numb_max) || p_other.synfight_numb_max < 0 || p_other.synfight_numb_max > PARTNER_SYNFIGHT_NUM_MAX) return -2;
	if (!GetSubNodeValue(root_element, "rate_return_exp", p_other.reset_level_ret_rate) || p_other.reset_level_ret_rate < 0) return -3;
	if (!GetSubNodeValue(root_element, "reset_price", p_other.reset_level_need_coint) || p_other.reset_level_need_coint < 0) return -4;
	if (!GetSubNodeValue(root_element, "upgrade_limit", p_other.quality_upgrade_limit) || p_other.quality_upgrade_limit < QUALITY_TYPE_MIN || 
		p_other.quality_upgrade_limit >= QUALITY_TYPE_MAX) return -5;
	if (!GetSubNodeValue(root_element, "segment_limit", p_other.quality_segment_limit) || p_other.quality_segment_limit <= 0 || 
		p_other.quality_segment_limit > PARTNER_QUALITY_SEGMENT_MAX_NUM) return -6;
	if (!GetSubNodeValue(root_element, "upgrade_level_limit", p_other.upgrade_level_limit) || p_other.upgrade_level_limit <= 0) return -7;
	if (!GetSubNodeValue(root_element, "soar_arms_limit", p_other.soar_arms_limit) || p_other.soar_arms_limit <= 0) return -8;

	return 0;
}

int PartnerConfig::InitPartnerBasicCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	m_partner_num = 0;
	int last_partner_id = PARTNER_ID_START - 1;
	while (NULL != root_element)
	{
		int partner_id = 0;

		// 做了伙伴模块的小优化，因此必须控制partner_id在[1, PARTNER_NUM_MAX)之间
		if (!GetSubNodeValue(root_element, "partner_id", partner_id)
			|| partner_id < 0 || partner_id >= PARTNER_NUM_MAX ||
			last_partner_id + 1 != partner_id)
		{
			// partner_id 必须每次+1
			return -1;
		}
		last_partner_id = partner_id;

		PartnerBasicCfg& cfg = m_basic_cfg_list[partner_id];
		if (!cfg.Invalid())
		{
			return -111; // 有重复partner_id
		}
		cfg.is_valid = true;
		m_partner_num += 1;

		if (!GetSubNodeValue(root_element, "name", cfg.name))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "job", cfg.profession)
			|| cfg.profession < PARTNER_PROF_NEWCOMER
			|| cfg.profession >= PARTNER_PROF_MAX)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "race", cfg.race_type)
			|| cfg.race_type < RACE_TYPE_MIN
			|| cfg.race_type >= RACE_TYPE_MAX)
		{
			return -5;
		}

		int sex = -1;

		if (!GetSubNodeValue(root_element, "sex", sex)
			|| sex < FEMALE
			|| sex >= SEX_MAX)
		{
			return -6;
		}

		cfg.sex = sex;

		//if (!GetSubNodeValue(root_element, "qingyuan_group", cfg.fetter_group)
		//	|| m_v_f_group.find(cfg.fetter_group) == m_v_f_group.end())
		//{
		//	return -6;
		//}

		if (!GetSubNodeValue(root_element, "activation_task", cfg.unlock_task)/* || TODO: 检测任务池是否有对应任务*/)
		{
			return -7;
		}
		if (cfg.unlock_task > 0)
		{
			CheckResourceCenter::Instance().GetTaskCheck()->Add(cfg.unlock_task, __FUNCTION__);
		}

		if (!GetSubNodeValue(root_element, "activation_level", cfg.activation_level) || cfg.activation_level <= 0 || cfg.activation_level > MAX_ROLE_LEVEL)
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "activation_item", cfg.unlock_item))
		{
			return -8;
		}
		if (cfg.unlock_item > 0)
		{
			if (NULL == ITEMPOOL->GetItem(cfg.unlock_item))
			{
				return -88;
			}
		}

		if (!GetSubNodeValue(root_element, "activation_item_norex", cfg.unlock_item_norex))
		{
			return -9;
		}
		if (cfg.unlock_item_norex > 0)
		{
			if (NULL == ITEMPOOL->GetItem(cfg.unlock_item_norex))
			{
				return -77;
			}
		}


		if (!GetSubNodeValue(root_element, "activation_num", cfg.need_num) || cfg.need_num < 0)
		{
			return -20;
		}

		if (!GetSubNodeValue(root_element, "is_long_range", cfg.is_long_range_unit)
			|| cfg.is_long_range_unit < 0 || cfg.is_long_range_unit > 1)
		{
			return -21;
		}

		if (!GetSubNodeValue(root_element, "partner_quality", cfg.quality)
			|| cfg.quality <= PQT_MIN || cfg.quality >= PQT_MAX)
		{
			return -22;
		}
		if (!GetSubNodeValue(root_element, "crystal_item", cfg.crystal_item)
			|| cfg.crystal_item < PARTNER_CRYSTAL_ID_START
			|| cfg.crystal_item >= PARTNER_CRYSTAL_ID_END)
		{
			return -23;
		}
		const ItemBase* item = ITEMPOOL->GetItem(cfg.crystal_item);
		if (NULL == item || item->GetItemType() != I_TYPE_ELEMENT_CRYSTAL)
		{
			return -24;
		}

		int ele_count = 0;
		const char * ele_str[PARTNER_ELE_MAX] = { "element_water", "element_fire", "element_wind", "element_land" };
		for (int t_cur = PARTNER_ELE_WATER; t_cur < PARTNER_ELE_MAX; ++t_cur)
		{
			if (!GetSubNodeValue(root_element, ele_str[t_cur], cfg.element[t_cur]) || cfg.element[t_cur] < 0)
			{
				return -(9 + t_cur);
			}
			ele_count += cfg.element[t_cur];
		}

		if (PARTNER_ELEMET_NUMBER_TOTAL != ele_count) return -(9 + PARTNER_ELE_MAX);
		m_basic_cfg_map[cfg.unlock_item] = cfg.quality;


		if (!GetSubNodeValue(root_element, "get_star_deed", cfg.unlock_item_xun_bao))
		{
			return -25;
		}
		if (cfg.unlock_item_xun_bao > 0)
		{
			if (NULL == ITEMPOOL->GetItem(cfg.unlock_item_xun_bao))
			{
				return -26;
			}
		}

		if (!GetSubNodeValue(root_element, "arms_id", cfg.soar_arms_id) || m_partner_soar_arms_cfg.end() == m_partner_soar_arms_cfg.find(this->GetSoarArmsKey(cfg.soar_arms_id, 1)))
		{
			return -27;
		}

		if (!GetSubNodeValue(root_element, "soar_fin_task", cfg.soar_task_id) || NULL == TASKPOOL->GetTaskCfgByID(cfg.soar_task_id))
		{
			return -28;
		}

		root_element = root_element->NextSiblingElement();
	}

	if (m_partner_num >= PARTNER_NUM_MAX)
	{
		return -7777;
	}

	return 0;
}

int PartnerConfig::InitSkillsUnlockCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	int last_partner_id = PARTNER_ID_START - 1;
	int last_skill_index = 0;
	while (NULL != root_element)
	{
		int t_partner_id = 0, t_skill_id = 0;
		if (!GetSubNodeValue(root_element, "partner_id", t_partner_id) 
			|| NULL == this->GetPartnerBasicCfg(t_partner_id)
			|| (last_partner_id != t_partner_id && last_partner_id + 1 != t_partner_id)
			)
		{
			return -1;
		}
		if (t_partner_id != last_partner_id)
		{
			last_partner_id = t_partner_id;
			last_skill_index = 0;
		}

		PartnerBasicCfg* partner_cfg = &m_basic_cfg_list[t_partner_id];

		if (!GetSubNodeValue(root_element, "skill_id", t_skill_id))
		{
			return -4;
		}

		int skill_index = 0;
		if (!GetSubNodeValue(root_element, "skill_index", skill_index)
			|| last_skill_index + 1 != skill_index
			|| skill_index <= 0 
			|| skill_index > PARTNER_COMMON_SKILL_MAX)
		{
			return -5;
		}
		last_skill_index = skill_index;

		partner_cfg->skill_list[skill_index - 1] = t_skill_id;

		root_element = root_element->NextSiblingElement();
	}
	
	return 0;
}

int PartnerConfig::InitPartnerUpGradeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;
	
	while (NULL != root_element)
	{
		PartnerCfgHead t_head; PartnerUpGradeCfg t_ug;
		if (!GetSubNodeValue(root_element, "partner_id", t_head.partner_id))
		{
			return -1;
		}

		const PartnerBasicCfg* partner_cfg = this->GetPartnerBasicCfg(t_head.partner_id);
		if (NULL == partner_cfg)
		{
			return -111;
		}

		if (!GetSubNodeValue(root_element, "level", t_head.partner_lv) || t_head.partner_lv < 0 || t_head.partner_lv > UCHAR_MAX)
		{
			return -2;
		}

		if (m_s_upgrade.find(t_head) != m_s_upgrade.end())
		{
			return -3;
		}

		if (!m_s_upgrade.empty())	// 不连续，报错
		{
			PartnerCfgHead t_check = m_s_upgrade.rbegin()->first;
			if (t_check.partner_id == t_head.partner_id && t_check.partner_lv + 1 != t_head.partner_lv)
			{
				return -4;
			}

			if (t_check.partner_id != t_head.partner_id && (t_check.partner_id + 1 != t_head.partner_id || 0 != t_head.partner_lv))
			{
				return -5;
			}
		}
		
		if (!GetSubNodeValue(root_element, "need_rolelevel", t_ug.player_lv_need) || t_ug.player_lv_need < 0)
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "skill_gift", t_ug.talent_skill_id))
		{
			return -7;
		}

		if (!GetSubNodeValue(root_element, "skill_gift_level", t_ug.talent_skill_lv) || t_ug.talent_skill_lv < 0)
		{
			return -8;
		}

		if (!GetSubNodeValue(root_element, "skill_friendly", t_ug.ji_ban_seq))
		{
			return -9;
		}

		m_s_upgrade.insert(std::pair<PartnerCfgHead, PartnerUpGradeCfg>(t_head, t_ug));
		max_upgrade_level = GetMax(max_upgrade_level, t_head.partner_lv);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PartnerConfig::InitPartnerBreakThCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	char * cfg_reader = new char[15];
	while (NULL != root_element)
	{
		PartnerCfgHead t_head; PartnerBreakThCfg t_pbtc;
		if (!GetSubNodeValue(root_element, "partner_id", t_head.partner_id) || NULL == this->GetPartnerBasicCfg(t_head.partner_id)) return -1;
		if (!GetSubNodeValue(root_element, "level", t_head.partner_lv) || t_head.partner_lv < 0) return -2;

		if (!GetSubNodeValue(root_element, "seq", t_pbtc.seq) || t_pbtc.seq < 0) return -3;
		if (!GetSubNodeValue(root_element, "att_type", t_pbtc.attr_type) || t_pbtc.attr_type < 0 || t_pbtc.attr_type >= BATTLE_ATTR_MAX) return -4;
		if (!GetSubNodeValue(root_element, "att_value", t_pbtc.add_value) || t_pbtc.add_value < 0) return -5;
		
		for (int stuff_type = 0; stuff_type < PARTNER_ATTR_BKTH_ITEM_TYPE; ++stuff_type)
		{
			ItemPairCon & read_con = t_pbtc.need_item[stuff_type];

			sprintf(cfg_reader, "stuff_need_%d", stuff_type + 1);
			if (!GetSubNodeValue(root_element, cfg_reader, read_con.item_id))
			{
				return -(100 + stuff_type);
			}

			if (read_con.item_id == 0)
			{
				continue;
			}

			if (NULL == ITEMPOOL->GetItem(read_con.item_id))
			{
				return -(1000 + stuff_type);
			}

			sprintf(cfg_reader, "stuff_num_%d", stuff_type + 1);
			if (!GetSubNodeValue(root_element, cfg_reader, read_con.item_num) || read_con.item_num < 0)
			{
				return -(200 + stuff_type);
			}
		}

		std::map<PartnerCfgHead, std::vector<PartnerBreakThCfg> >::iterator it = m_v_breakth.find(t_head);
		if (it != m_v_breakth.end())
		{
			if (it->second.back().seq + 1 != t_pbtc.seq) return -8;
			it->second.push_back(t_pbtc);
		}
		else
		{
			if (!m_v_breakth.empty() && m_v_breakth.begin()->second.size() != m_v_breakth.rbegin()->second.size()) return -9;
			std::vector<PartnerBreakThCfg> t_v_pbtc; t_v_pbtc.push_back(t_pbtc);
			m_v_breakth.insert(std::pair<PartnerCfgHead, std::vector<PartnerBreakThCfg> >(t_head, t_v_pbtc));

			m_v_breakth[t_head].reserve(10);
		}

		root_element = root_element->NextSiblingElement();
	}

	delete[] cfg_reader;
	return 0;
}

int PartnerConfig::InitPartnerAttribute(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	int last_partner_id = PARTNER_ID_START - 1;
	int last_level = 0;
	while (NULL != root_element)
	{
		int partner_id = 0;
		if (!GetSubNodeValue(root_element, "partner_id", partner_id)
			|| (partner_id != last_partner_id && partner_id != last_partner_id + 1)
			|| NULL == this->GetPartnerBasicCfg(partner_id)
			)
		{
			return -1;
		}
		if (last_partner_id != partner_id)
		{
			last_level = 0;
			last_partner_id = partner_id;
		}
		
		int partner_lv = 0;
		if (!GetSubNodeValue(root_element, "level", partner_lv)
			|| partner_lv <= 0 || partner_lv > MAX_ROLE_LEVEL
			|| last_level + 1 != partner_lv
			)
		{
			return -2;
		}
		last_level = partner_lv;

		PartnerLevelCfg& t_pac = m_lv_attr_cfg_list[partner_id][partner_lv];

		if (!GetSubNodeValue(root_element, "partner_id", partner_id)
		    || (partner_id != last_partner_id && partner_id != last_partner_id + 1)
		    || NULL == this->GetPartnerBasicCfg(partner_id)
		    )
		{
			return -2001;
		}


		if (!GetSubNodeValue(root_element, "partner_id", partner_id)
		    || (partner_id != last_partner_id && partner_id != last_partner_id + 1)
		    || NULL == this->GetPartnerBasicCfg(partner_id)
		    )
		{
			return -2002;
		}

		if (!GetSubNodeValue(root_element, "price_type", t_pac.comsume_money_type))
		{
			return -2003;
		}

		if (!GetSubNodeValue(root_element, "price", t_pac.comsume_money_value))
		{
			return -2004;
		}

		// 攻、防、敏、回、精 ---- 血、魔 另行处理
		const char * attr_str[PARTNER_ATTR_MAX] = { "maxhp", "maxmp", "gongji", "fangyu", "minjie", "huifu", "jingshen" };
		for (int t_cur = PARTNER_ATTR_MHP; t_cur < PARTNER_ATTR_MAX; ++t_cur)	// 允许属性是负数
		{
			if (!GetSubNodeValue(root_element, attr_str[t_cur], t_pac.attr_list[t_cur])/* || t_pac.attr_list[t_cur] < 0*/)
			{
				return -(100 + t_cur);
			}
		}
		//(抗性) 毒、睡、石、乱、醉
		const char * resi_str[PARTNER_RESI_MAX] = { "def_zhongdu", "def_hunshui", "def_shihua", "def_hunluan", "def_jiuzui" };
		for (int t_cur = PARTNER_RESI_TOXIC; t_cur < PARTNER_RESI_MAX; ++t_cur)	// 允许属性是负数
		{
			if (!GetSubNodeValue(root_element, resi_str[t_cur], t_pac.resi_list[t_cur])/* || t_pac.resi_list[t_cur] < 0*/)
			{
				return -(200 + t_cur);
			}
		}
		// 必杀、命中、闪避、反击、魔攻、魔抗 ---- 坚韧 另行处理
		const char * amen_str[PARTNER_AMEN_MAX] = { "bisha", "mingzhong", "shanduo", "fanji", "mogong", "mokang", "jianren" ,"bisha_dmg" };
		for (int t_cur = PARTNER_AMEN_CRI; t_cur < PARTNER_AMEN_MAX; ++t_cur)	// 允许属性是负数
		{
			if (!GetSubNodeValue(root_element, amen_str[t_cur], t_pac.amen_list[t_cur])/* || t_pac.amen_list[t_cur] < 0*/)
			{
				return -(300 + t_cur);
			}
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PartnerConfig::InitPartnerTalentCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		PartnerCfgHead t_pch; PartnerTalentCfg t_ptc;
		if (!GetSubNodeValue(root_element, "gift_id", t_pch.partner_id) || t_pch.partner_id < 0) return -1;
		if (!GetSubNodeValue(root_element, "gift_level", t_pch.partner_lv) || t_pch.partner_lv < 0) return -2;

		if (m_s_talents.end() != m_s_talents.find(t_pch)) return -3;	// 如果已经有同技能同等级的配置，报错
		if (!m_s_talents.empty())
		{
			const PartnerCfgHead & t_pch_c = m_s_talents.rbegin()->first;	// 如果配置不连续，报错
			if (t_pch_c.partner_lv + 1 != t_pch.partner_lv && t_pch_c.partner_id + 1 != t_pch.partner_id) return -4;
		}

		if (!GetSubNodeValue(root_element, "type", t_ptc.effect_type) || t_ptc.effect_type <= TALENT_SKILL_TYPE_INVALID || t_ptc.effect_type >= TALENT_SKILL_TYPE_MAX) return -5;
		if (!GetSubNodeValue(root_element, "target", t_ptc.effect_target) || t_ptc.effect_target < 0 || t_ptc.effect_target >= TALENT_TARGET_MAX) return -6;
		if (!GetSubNodeValue(root_element, "param_1", t_ptc.param1)) return -7;
		if (!GetSubNodeValue(root_element, "param_2", t_ptc.param2)) return -8;

		switch (t_ptc.effect_type)
		{
		case TALENT_PLAYER_ATTR_VALUE_INCR:
			if (t_ptc.param1 < 0 || t_ptc.param1 >= BATTLE_ATTR_MAX) return -100;
			break;

		case TALENT_PLAYER_WITHOUT_ANY_EFF:		// 不检测
			break;

		case TALENT_PLAYER_PASSIVE_SKILL:
			if (NULL == SkillPool::Instance().GetPassiveSkillCfg(t_ptc.param1)) return -300;
			break;

		default:
			return -200;
			break;
		}

		m_s_talents.insert(std::pair<PartnerCfgHead, PartnerTalentCfg>(t_pch, t_ptc));
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PartnerConfig::InitPartnerFetterCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int t_fetter_id; PartnerFetterCfg t_pfc;
		if (!GetSubNodeValue(root_element, "qingyuan_id", t_fetter_id) || t_fetter_id <= 0 || t_fetter_id > 255 || m_s_fetters.find(t_fetter_id) != m_s_fetters.end()) return -1;
		if (!GetSubNodeValue(root_element, "need_partner", t_pfc.need_partner) || NULL == this->GetPartnerBasicCfg(t_pfc.need_partner)) return -2;
		if (!GetSubNodeValue(root_element, "qingyuan_type", t_pfc.fetter_mark) || t_pfc.fetter_mark <= 0) return -3;
		// 攻、防、敏、回、精 ---- 血、魔 另行处理
		const char * attr_str[PARTNER_ATTR_MAX] = { "maxhp", "maxmp", "gongji", "fangyu", "minjie", "huifu", "jingshen" };
		for (int t_cur = PARTNER_ATTR_MHP; t_cur < PARTNER_ATTR_MAX; ++t_cur)
		{
			if (!GetSubNodeValue(root_element, attr_str[t_cur], t_pfc.attr_add[t_cur]) || t_pfc.attr_add[t_cur] < 0) return -(4 + t_cur);
		}
		// 毒、睡、石、乱、醉
		const char * resi_str[PARTNER_RESI_MAX] = { "def_zhongdu", "def_hunshui", "def_shihua", "def_hunluan", "def_jiuzui" };
		for (int t_cur = PARTNER_RESI_TOXIC; t_cur < PARTNER_RESI_MAX; ++t_cur)
		{
			if (!GetSubNodeValue(root_element, resi_str[t_cur], t_pfc.resi_add[t_cur]) || t_pfc.resi_add[t_cur] < 0) return -(4 + PARTNER_ATTR_MAX + t_cur);
		}
		// 必杀、命中、闪避、反击、魔攻、魔抗 ---- 坚韧 另行处理
		const char * amen_str[PARTNER_AMEN_MAX] = { "bisha", "mingzhong", "shanduo", "fanji", "mogong", "mokang", "jianren" };
		for (int t_cur = PARTNER_AMEN_CRI; t_cur < PARTNER_AMEN_MAX; ++t_cur)
		{
			if (!GetSubNodeValue(root_element, amen_str[t_cur], t_pfc.amen_add[t_cur]) || t_pfc.resi_add[t_cur] < 0) return -(4 + PARTNER_ATTR_MAX + PARTNER_RESI_MAX + t_cur);
		}

		m_s_fetters.insert(std::pair<int, PartnerFetterCfg>(t_fetter_id, t_pfc));
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PartnerConfig::InitFettersGroupsCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int t_f_group; FettersGroupsCfg t_fgc;
		if (!GetSubNodeValue(root_element, "qingyuan_group", t_f_group) || (!m_v_f_group.empty() && t_f_group < m_v_f_group.rbegin()->first)) return -1;
		
		if (!GetSubNodeValue(root_element, "qingyuan_id", t_fgc.fetter_id) || t_fgc.fetter_id < 0) return -2;
		if (!GetSubNodeValue(root_element, "rate", t_fgc.rand_rate) || t_fgc.rand_rate > RAND_UNIVERSAL_RATIO_BASE_NUM) return -3;
		
		std::map<int, std::vector<FettersGroupsCfg> >::iterator it = m_v_f_group.find(t_f_group);
		if (it != m_v_f_group.end())
		{
			it->second.push_back(t_fgc);
		}
		else
		{
			if (!m_v_f_group.empty())
			{
				int t_weight_total = 0;
				for (std::vector<FettersGroupsCfg>::iterator v_it = m_v_f_group.rbegin()->second.begin(); v_it != m_v_f_group.rbegin()->second.end(); ++v_it)
				{
					t_weight_total += v_it->rand_rate;
				}

				if (RAND_UNIVERSAL_RATIO_BASE_NUM != t_weight_total) return -4;
			}
			
			std::vector<FettersGroupsCfg> t_v_fgc; t_v_fgc.push_back(t_fgc);
			m_v_f_group.insert(std::pair<int, std::vector<FettersGroupsCfg> >(t_f_group, t_v_fgc));
		}

		root_element = root_element->NextSiblingElement();
	}
	
	return 0;
}

int PartnerConfig::InitPartnerQualityUpCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		static QualityUpgradeCfg q_cfg;

		if (!GetSubNodeValue(root_element, "partner_id", q_cfg.partner_id) || q_cfg.partner_id < 0 || q_cfg.partner_id > 65535) return -10;
		const PartnerBasicCfg* partner_cfg = this->GetPartnerBasicCfg(q_cfg.partner_id);
		if (NULL == partner_cfg)
		{
			return -111;
		}

		if (!GetSubNodeValue(root_element, "quality_upgrade", q_cfg.quality_upgrade_type) || q_cfg.quality_upgrade_type < QUALITY_TYPE_MIN || q_cfg.quality_upgrade_type >= QUALITY_TYPE_MAX) return -20;
		if (!GetSubNodeValue(root_element, "need_item", q_cfg.need_item) || q_cfg.need_item < 0) return -30;
		if (!GetSubNodeValue(root_element, "need_num", q_cfg.need_num) || q_cfg.need_num < 0) return -40;
		if (!GetSubNodeValue(root_element, "maxhp", q_cfg.add_maxhp) || q_cfg.add_maxhp < 0) return -50;
		if (!GetSubNodeValue(root_element, "maxmp", q_cfg.add_maxmp) || q_cfg.add_maxmp < 0) return -60;
		if (!GetSubNodeValue(root_element, "gongji", q_cfg.add_gongji) || q_cfg.add_gongji < 0) return -70;
		if (!GetSubNodeValue(root_element, "fangyu", q_cfg.add_fangyu) || q_cfg.add_fangyu < 0) return -80;
		if (!GetSubNodeValue(root_element, "minjie", q_cfg.add_minjie) || q_cfg.add_minjie < 0) return -90;
		if (!GetSubNodeValue(root_element, "jingshen", q_cfg.add_jingshen) || q_cfg.add_jingshen < 0) return -100;
		if (!GetSubNodeValue(root_element, "huifu", q_cfg.add_huifu) || q_cfg.add_huifu < 0) return -110;
		if (!GetSubNodeValue(root_element, "level_limit", q_cfg.level_limit) || q_cfg.level_limit < 0) return -120;
		if (!GetSubNodeValue(root_element, "quality_segment", q_cfg.quality_segment) || q_cfg.quality_segment <= 0 || q_cfg.quality_segment > PARTNER_QUALITY_SEGMENT_MAX_NUM) return -130;

		for (int skill_idx = 0; skill_idx < PARTNER_COMMON_SKILL_MAX; ++skill_idx)
		{
			char skill_lv_reader[128] = { 0 };
			sprintf(skill_lv_reader, "skill_level_%d", skill_idx + 1);
			int skill_level = 0;
			if (!GetSubNodeValue(root_element, skill_lv_reader, skill_level) || skill_level < 0)
			{
				return -(200 + skill_idx);
			}

			q_cfg.common_skill_id_list[skill_idx] = partner_cfg->skill_list[skill_idx];
			q_cfg.common_skill_lv_list[skill_idx] = skill_level;
		}

		q_cfg.add_attr[PARTNER_ATTR_MHP] = q_cfg.add_maxhp;
		q_cfg.add_attr[PARTNER_ATTR_MMP] = q_cfg.add_maxmp;
		q_cfg.add_attr[PARTNER_ATTR_ATK] = q_cfg.add_gongji;
		q_cfg.add_attr[PARTNER_ATTR_DEF] = q_cfg.add_fangyu;
		q_cfg.add_attr[PARTNER_ATTR_AGI] = q_cfg.add_minjie;
		q_cfg.add_attr[PARTNER_ATTR_REC] = q_cfg.add_huifu;
		q_cfg.add_attr[PARTNER_ATTR_SPI] = q_cfg.add_jingshen;

		int key = ___QuaKey__(q_cfg.partner_id, q_cfg.quality_upgrade_type, q_cfg.quality_segment);

		if (m_idqua_to_upg.find(key) != m_idqua_to_upg.end())
		{
			return -140; // 有重复
		}
		m_idqua_to_upg.insert(std::map<int, QualityUpgradeCfg>::value_type(key, q_cfg));

		root_element = root_element->NextSiblingElement();
	}

	return 0;

}

int PartnerConfig::InitPartnerUpGradeRoleAttrCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;


	std::string attr_str[] = { "maxhp","maxmp","gongji","fangyu","minjie","jingshen","huifu","bisha","bisha_dmg","jianren","fanji",
		"mingzhong","shanduo","def_zhongdu","def_shihua","def_hunshui","def_jiuzui","def_hunluan","mogong","mokang" };
	while (NULL != root_element)
	{
		PartnerUpGradeRoleAttrCfg cfg;

		if (!GetSubNodeValue(root_element, "seq", cfg.seq)) return -__LINE__;
		if (!GetSubNodeValue(root_element, "skill_id", cfg.ji_ban_skill_id)) return -__LINE__;
		if (!GetSubNodeValue(root_element, "level", cfg.ji_ban_skill_level)) return -__LINE__;


		for (int i = 0; i < ARRAY_LENGTH(attr_str); ++i)
		{
			int attr_index = GetAttrIndexByName(attr_str[i]);

			if (!GetSubNodeValue(root_element, attr_str[i], cfg.attrs[attr_index]))
			{
				return -__LINE__;
			}
		}

		m_upgrade_role_attr_map[cfg.seq] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PartnerConfig::InitPartnerBiographyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		PartnerCfgHead p_head;
		if (!GetSubNodeValue(root_element, "partner_id", p_head.partner_id) || p_head.partner_id < 0 || p_head.partner_id >= PARTNER_NUM_MAX) return -1;
		const PartnerBasicCfg* partner_cfg = this->GetPartnerBasicCfg(p_head.partner_id);
		if (NULL == partner_cfg)
		{
			return -111;
		}
		
		if (!GetSubNodeValue(root_element, "biography", p_head.partner_lv) || p_head.partner_lv < 0 || p_head.partner_lv >= BIT_COUNT_OF_BYTE) return -2;

		if (m_biography_map.find(p_head) != m_biography_map.end())
		{
			return -222;
		}

		PartnerBiographyCfg item_cfg;
		if (!GetSubNodeValue(root_element, "upgrade", item_cfg.need_upgrade) || item_cfg.need_upgrade < 0) return -3;
		PartnerCfgHead t_head(p_head.partner_id, item_cfg.need_upgrade);
		if (m_s_upgrade.find(t_head) == m_s_upgrade.end())
		{
			return -333;
		}
		if (!GetSubNodeValue(root_element, "coin", item_cfg.coin) || item_cfg.coin  < 0) return -4;

		TiXmlElement * list_node = root_element->FirstChildElement("reward_item_list");
		if (NULL != list_node)
		{
			TiXmlElement * item_node = list_node->FirstChildElement("reward_item");
			while (NULL != item_node)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_node)) return -444;

				item_cfg.reward_list.push_back(item);
				item_node = item_node->NextSiblingElement();
			}
		}
		m_biography_map[p_head] = item_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PartnerConfig::InitPartnerComposeGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int reward_group_id = 0;
		if (!GetSubNodeValue(root_element, "reward_group", reward_group_id) || reward_group_id <= 0) return -1;

		PartnerComposeCfg node_cfg;
		if (!GetSubNodeValue(root_element, "rate", node_cfg.rate) || node_cfg.rate <= 0) return -2;
		
		TiXmlElement * item_node = root_element->FirstChildElement("reward");
		if (NULL == item_node)
		{
			return -100;
		}
		if (!node_cfg.item.ReadConfig(item_node)) return -101;

		m_partner_compose_cfg[reward_group_id].rate_count += node_cfg.rate;
		m_partner_compose_cfg[reward_group_id].node_list.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PartnerConfig::InitPartnerComposePageCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int item_id = 0;
		if (!GetSubNodeValue(root_element, "item_id", item_id) || NULL == ITEMPOOL->GetItem(item_id)) return -1;
		
		if (m_partner_compose_page_cfg.end() != m_partner_compose_page_cfg.find(item_id))
		{
			return -2;
		}
		PartnerComposePageCfg node_cfg;
		if (!GetSubNodeValue(root_element, "num", node_cfg.num) || node_cfg.num <= 0) return -3;
		if (!GetSubNodeValue(root_element, "reward_group", node_cfg.reward_group_id) || m_partner_compose_cfg.end() == m_partner_compose_cfg.find(node_cfg.reward_group_id)) return -4;

		m_partner_compose_page_cfg[item_id] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PartnerConfig::InitPactSaveIndexCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	int save_index_num = 0;
	while (NULL != root_element)
	{
		int save_index = 0;
		if (!GetSubNodeValue(root_element, "save_index", save_index)
			|| 0 > save_index
			|| PARTNER_SPIRIT_PACT_MAX_GRID <= save_index
			|| save_index_num != save_index)
		{
			return -1;
		}
		save_index_num++;

		ItemID item_id = 0;
		if (!GetSubNodeValue(root_element, "item_id", item_id))
		{
			return -2;
		}

		const ItemBase *item_base = ITEMPOOL->GetItem(item_id);
		if (NULL == item_base)
		{
			return -3;
		}

		if (I_TYPE_PARTNER_PACT != item_base->GetItemType())
		{
			return -4;
		}
		if (ItemNamespace::ITEM_SUB_TYPE_SPIRIT_PACT != item_base->GetItemSubType())
		{
			return -5;
		}
		if (B_BELONG_PARTNER_SPIRIT_PACT != item_base->GetBagsBelong())
		{
			return -6;
		}

		{
			PactItemIDMap::iterator id_it = m_pact_item_id_map.find(item_id);
			if (id_it != m_pact_item_id_map.end())
			{
				return -7;
			}
			m_pact_item_id_map[item_id] = save_index;
		}
		{
			PactSaveIndexMap::iterator save_index_it = m_pact_save_index_map.find(save_index);
			if (save_index_it != m_pact_save_index_map.end())
			{
				return -8;
			}
			m_pact_save_index_map[save_index] = item_id;
		}

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PartnerConfig::InitPartnerSoarCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	int next_soar_frequency = 1;
	PartnerSoarCfg last_node_cfg;
	while (NULL != root_element)
	{
		int soar_frequency = 0;
		if (!GetSubNodeValue(root_element, "soar_frequency", soar_frequency) || soar_frequency != next_soar_frequency)
		{
			errormsg = STRING_SPRINTF("[soar_frequency=%d] error, is soar_frequency != next_soar_frequency(%d)?", soar_frequency, next_soar_frequency);
			return -1;
		}
		next_soar_frequency += 1;

		PartnerSoarCfg node_cfg;
		//飞升所需
		if (!GetSubNodeValue(root_element, "soar_level", node_cfg.soar_need_level) || node_cfg.soar_need_level <= 0 || node_cfg.soar_need_level > MAX_ROLE_LEVEL) 
		{
			errormsg = STRING_SPRINTF("[soar_frequency=%d] error, is soar_level[%d] <= 0 || > %d?", soar_frequency, node_cfg.soar_need_level, MAX_ROLE_LEVEL);
			return -2;
		}
		if (!GetSubNodeValue(root_element, "pre_score", node_cfg.soar_need_score) || node_cfg.soar_need_score < 0) 
		{
			errormsg = STRING_SPRINTF("[soar_frequency=%d] error, is pre_score[%d] < 0?", soar_frequency, node_cfg.soar_need_score);
			return -3;
		}
		if (!GetSubNodeValue(root_element, "upgrade_condition", node_cfg.soar_need_quality_upgrade) || node_cfg.soar_need_quality_upgrade < QUALITY_TYPE_MIN || 
			node_cfg.soar_need_quality_upgrade >= QUALITY_TYPE_MAX) 
		{
			errormsg = STRING_SPRINTF("[soar_frequency=%d] error, is upgrade_condition[%d] < %d || >= %d?", soar_frequency, node_cfg.soar_need_quality_upgrade, QUALITY_TYPE_MIN, QUALITY_TYPE_MAX);
			return -4;
		}
		if (!GetSubNodeValue(root_element, "segment_condition", node_cfg.soar_need_quality_segment) || node_cfg.soar_need_quality_segment <= 0 || 
			node_cfg.soar_need_quality_segment > PARTNER_QUALITY_SEGMENT_MAX_NUM) 
		{
			errormsg = STRING_SPRINTF("[soar_frequency=%d] error, is segment_condition[%d] <= 0 || >= %d?", soar_frequency, node_cfg.soar_need_quality_segment, PARTNER_QUALITY_SEGMENT_MAX_NUM);
			return -5;
		}

		//飞升解锁
		if (!GetSubNodeValue(root_element, "upgrade_level", node_cfg.upgrade_level) || node_cfg.upgrade_level < p_other.upgrade_level_limit || node_cfg.upgrade_level > max_upgrade_level) 
		{
			errormsg = STRING_SPRINTF("[soar_frequency=%d] error, is upgrade_level[%d] < %d or > %d?", soar_frequency, node_cfg.upgrade_level, p_other.upgrade_level_limit, max_upgrade_level);
			return -6;
		}
		if (!GetSubNodeValue(root_element, "quality_upgrade", node_cfg.soar_quality_upgrade) || node_cfg.soar_quality_upgrade >= QUALITY_TYPE_MAX || 
			node_cfg.soar_quality_upgrade < node_cfg.soar_need_quality_upgrade) 
		{
			errormsg = STRING_SPRINTF("[soar_frequency=%d] error, is quality_upgrade[%d] < upgrade_condition[%d] || >= %d?", soar_frequency, node_cfg.soar_quality_upgrade, 
				node_cfg.soar_need_quality_upgrade, QUALITY_TYPE_MAX);
			return -7;
		}
		if (!GetSubNodeValue(root_element, "quality_segment", node_cfg.soar_quality_segment) || node_cfg.soar_quality_segment <= 0 ||
			node_cfg.soar_quality_segment > PARTNER_QUALITY_SEGMENT_MAX_NUM) 
		{
			errormsg = STRING_SPRINTF("[soar_frequency=%d] error, is quality_segment[%d] <= 0 || > %d?", soar_frequency, node_cfg.soar_quality_segment, PARTNER_QUALITY_SEGMENT_MAX_NUM);
			return -8;
		}
		if (node_cfg.soar_quality_upgrade == node_cfg.soar_need_quality_upgrade && node_cfg.soar_quality_segment < node_cfg.soar_need_quality_segment)
		{
			errormsg = STRING_SPRINTF("[soar_frequency=%d] error, is upgrade_condition[%d] == quality_upgrade[%d], segment_condition[%d] > quality_segment[%d]?", soar_frequency, 
				node_cfg.soar_need_quality_upgrade, node_cfg.soar_quality_upgrade, node_cfg.soar_need_quality_segment, node_cfg.soar_quality_segment);
			return -9;
		}

		if (!GetSubNodeValue(root_element, "attribute_pellet_limit", node_cfg.attr_pellet_limit) || node_cfg.attr_pellet_limit <= 0) 
		{
			errormsg = STRING_SPRINTF("[soar_frequency=%d] error, is attribute_pellet_limit[%d] <= 0?", soar_frequency, node_cfg.attr_pellet_limit);
			return -10;
		}
		if (!GetSubNodeValue(root_element, "arms_level", node_cfg.arms_level) || node_cfg.arms_level <= 0) 
		{
			errormsg = STRING_SPRINTF("[soar_frequency=%d] error, is arms_level[%d] <= 0?", soar_frequency, node_cfg.arms_level);
			return -11;
		}
		if (last_node_cfg.soar_need_level > node_cfg.soar_need_level)
		{
			errormsg = STRING_SPRINTF("cur[soar_frequency=%d] last[soar_frequency=%d] error, is last soar_level[%d] > cur soar_level[%d]?", soar_frequency, soar_frequency - 1, 
				last_node_cfg.soar_need_level, node_cfg.soar_need_level);
			return -12;
		}
		if (last_node_cfg.soar_need_score > node_cfg.soar_need_score)
		{
			errormsg = STRING_SPRINTF("cur[soar_frequency=%d] last[soar_frequency=%d] error, is last pre_score[%d] > cur pre_score[%d]?", soar_frequency, soar_frequency - 1,
				last_node_cfg.soar_need_score, node_cfg.soar_need_score);
			return -13;
		}
		if (last_node_cfg.soar_need_quality_upgrade > node_cfg.soar_need_quality_upgrade)
		{
			errormsg = STRING_SPRINTF("cur[soar_frequency=%d] last[soar_frequency=%d] error, is last upgrade_condition[%d] > cur upgrade_condition[%d]?", soar_frequency, soar_frequency - 1,
				last_node_cfg.soar_need_quality_upgrade, node_cfg.soar_need_quality_upgrade);
			return -14;
		}
		if (last_node_cfg.soar_need_quality_segment > node_cfg.soar_need_quality_segment)
		{
			errormsg = STRING_SPRINTF("cur[soar_frequency=%d] last[soar_frequency=%d] error, is last segment_condition[%d] > cur segment_condition[%d]?", soar_frequency, soar_frequency - 1,
				last_node_cfg.soar_need_quality_segment, node_cfg.soar_need_quality_segment);
			return -15;
		}
		if (last_node_cfg.upgrade_level > node_cfg.upgrade_level)
		{
			errormsg = STRING_SPRINTF("cur[soar_frequency=%d] last[soar_frequency=%d] error, is last upgrade_level[%d] > cur upgrade_level[%d]?", soar_frequency, soar_frequency - 1,
				last_node_cfg.upgrade_level, node_cfg.upgrade_level);
			return -16;
		}
		if (last_node_cfg.soar_quality_upgrade > node_cfg.soar_quality_upgrade)
		{
			errormsg = STRING_SPRINTF("cur[soar_frequency=%d] last[soar_frequency=%d] error, is last quality_upgrade[%d] > cur quality_upgrade[%d]?", soar_frequency, soar_frequency - 1,
				last_node_cfg.soar_quality_upgrade, node_cfg.soar_quality_upgrade);
			return -17;
		}
		if (last_node_cfg.soar_quality_segment > node_cfg.soar_quality_segment)
		{
			errormsg = STRING_SPRINTF("cur[soar_frequency=%d] last[soar_frequency=%d] error, is last quality_segment[%d] > cur quality_segment[%d]?", soar_frequency, soar_frequency - 1,
				last_node_cfg.soar_quality_segment, node_cfg.soar_quality_segment);
			return -18;
		}
		if (last_node_cfg.attr_pellet_limit > node_cfg.attr_pellet_limit)
		{
			errormsg = STRING_SPRINTF("cur[soar_frequency=%d] last[soar_frequency=%d] error, is last attribute_pellet_limit[%d] > cur attribute_pellet_limit[%d]?", soar_frequency, soar_frequency - 1,
				last_node_cfg.attr_pellet_limit, node_cfg.attr_pellet_limit);
			return -19;
		}
		if (last_node_cfg.arms_level > node_cfg.arms_level)
		{
			errormsg = STRING_SPRINTF("cur[soar_frequency=%d] last[soar_frequency=%d] error, is last arms_level[%d] > cur arms_level[%d]?", soar_frequency, soar_frequency - 1,
				last_node_cfg.arms_level, node_cfg.arms_level);
			return -20;
		}

		last_node_cfg = node_cfg;
		m_partner_soar_cfg[soar_frequency] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int PartnerConfig::InitPartnerSoarArmsCfg(TiXmlElement * RootElement, std::string& errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	std::string attr_str[] = { "maxhp","maxmp","gongji","fangyu","minjie","jingshen","huifu","bisha", "shanduo", "mingzhong", "fanji", "mokang", "mogong", "jianren","bisha_dmg",
		"def_zhongdu","def_shihua","def_hunshui","def_jiuzui","def_hunluan"};

	while (NULL != root_element)
	{
		int arms_id = 0, arms_level = 0;
		if (!GetSubNodeValue(root_element, "arms_id", arms_id) || arms_id <= 0) 
		{
			errormsg = STRING_SPRINTF("[arms_id=%d] error, is arms_id <= 0?", arms_id);
			return -1;
		}
		if (!GetSubNodeValue(root_element, "arms_level", arms_level) || arms_level < 0) 
		{
			errormsg = STRING_SPRINTF("[arms_id=%d][arms_level=%d] error, is arms_level < 0?", arms_id, arms_level);
			return -2;
		}
		int soar_arms_key = this->GetSoarArmsKey(arms_id, arms_level);
		if (m_partner_soar_arms_cfg.end() != m_partner_soar_arms_cfg.find(soar_arms_key))
		{
			errormsg = STRING_SPRINTF("[arms_id=%d][arms_level=%d] error, id and level is repeat?", arms_id, arms_level);
			return -3;
		}

		PartnerSoarArmsCfg node_cfg;
		if (!GetSubNodeValue(root_element, "skill_id", node_cfg.arms_skill_id) || NULL == SkillPool::Instance().GetPassiveSkillCfg(node_cfg.arms_skill_id)) 
		{
			errormsg = STRING_SPRINTF("[arms_id=%d][arms_level=%d] error, is skill_id[%d] not find?", arms_id, arms_level, node_cfg.arms_skill_id);
			return -4;
		}
		if (!GetSubNodeValue(root_element, "skill_level", node_cfg.arms_skill_level) || node_cfg.arms_skill_level < 0 || node_cfg.arms_skill_level > MAX_BATTLE_SKILL_LEVEL) 
		{
			errormsg = STRING_SPRINTF("[arms_id=%d][arms_level=%d] error, is skill_level[%d] < 0 || > %d?", arms_id, arms_level, node_cfg.arms_skill_level, MAX_BATTLE_SKILL_LEVEL);
			return -5;
		}
		for (int i = 0; i < ARRAY_LENGTH(attr_str); ++i)
		{
			int attr_index = GetAttrIndexByName(attr_str[i]);

			if (!GetSubNodeValue(root_element, attr_str[i], node_cfg.attr_list[attr_index]))
			{
				errormsg = STRING_SPRINTF("[arms_id=%d][arms_level=%d] error, is [%s] not find config info?", arms_id, arms_level, attr_str[i].c_str());
				return -(200 + i);
			}
		}
		
		//强化需要消耗材料
		int steps_num = 0;
		if (!GetSubNodeValue(root_element, "steps_num", steps_num) || steps_num < 0) 
		{
			errormsg = STRING_SPRINTF("[arms_id=%d][arms_level=%d] error, is steps_num[%d] < 0?", arms_id, arms_level, steps_num);
			return -5;	
		}
		
		TiXmlElement* item_list_element = root_element->FirstChildElement("item_list_list");
		if (NULL == item_list_element)
		{
			errormsg = STRING_SPRINTF("[arms_id=%d][arms_level=%d] error, is [item_list_list] not find config info?", arms_id, arms_level);
			return -100;
		}
		TiXmlElement* item_element = item_list_element->FirstChildElement("item_list");
		if (NULL == item_element)
		{
			errormsg = STRING_SPRINTF("[arms_id=%d][arms_level=%d] error, is [item_list] not find config info?", arms_id, arms_level);
			return -101;
		}
		std::set<int> consume_set;
		while (NULL != item_element)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_element))
			{
				errormsg = STRING_SPRINTF("[arms_id=%d][arms_level=%d] error, is item[id:%d num:%d]?", arms_id, arms_level, item.item_id, item.num);
				return -102;
			}
			if (consume_set.end() != consume_set.find(item.item_id))
			{
				errormsg = STRING_SPRINTF("[arms_id=%d][arms_level=%d] error, is item[id:%d] repeat?", arms_id, arms_level, item.item_id);
				return -103;
			}
			node_cfg.break_consume_list.push_back(item);
			consume_set.insert(item.item_id);
			item_element = item_element->NextSiblingElement();
		}
		if (steps_num != (int)node_cfg.break_consume_list.size())
		{
			errormsg = STRING_SPRINTF("[arms_id=%d][arms_level=%d] error, is steps_num[%d] != item_list_count[%d]?", arms_id, arms_level, steps_num, (int)node_cfg.break_consume_list.size());
			return -104;
		}

		m_partner_soar_arms_cfg[soar_arms_key] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PartnerConfig::InitPartnerIntensifyCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int partner_id = 0;
		if (!GetSubNodeValue(root_element, "partner_id", partner_id) || partner_id < 0 || partner_id >= PARTNER_NUM_MAX)
		{
			errormsg = STRING_SPRINTF("[partner_id=%d] error, is partner_id < 0 || partner_id >= PARTNER_NUM_MAX(%d) ?", partner_id, PARTNER_NUM_MAX);
			return -1;
		}
		const PartnerBasicCfg* partner_cfg = this->GetPartnerBasicCfg(partner_id);
		if (NULL == partner_cfg)
		{
			errormsg = STRING_SPRINTF("[partner_id=%d] error, NULL == this->GetPartnerBasicCfg(partner_id) ?", partner_id);
			return -2;
		}

		PartnerIntensifyCfg cfg;
		if (!GetSubNodeValue(root_element, "levels", cfg.levels) || cfg.levels < 0 || cfg.levels > MAX_ROLE_LEVEL)
		{
			errormsg = STRING_SPRINTF("[levels=%d] error, cfg.levels < 0 || cfg.levels > %d ?", cfg.levels, MAX_ROLE_LEVEL);
			return -8;
		}

		if (!GetSubNodeValue(root_element, "feisheng_num", cfg.feisheng_num) || cfg.feisheng_num < 0)
		{
			errormsg = STRING_SPRINTF("[feisheng_num=%d] error, cfg.feisheng_num < 0 ?", cfg.feisheng_num);
			return -9;
		}

		if (!GetSubNodeValue(root_element, "expend_item_1", cfg.expend_item_1) ||(cfg.expend_item_1 != 0 && NULL == ITEMPOOL->GetItem(cfg.expend_item_1)))
		{
			errormsg = STRING_SPRINTF("[expend_item_1=%d] error, NULL == ITEMPOOL->GetItem(cfg.expend_item_1) ?", cfg.expend_item_1);
			return -3;
		}
		if (!GetSubNodeValue(root_element, "expend_num_1", cfg.expend_num_1) || cfg.expend_num_1 < 0)
		{
			errormsg = STRING_SPRINTF("[expend_num_1=%d] error, cfg.expend_num_1 <= 0 ?", cfg.expend_num_1);
			return -4;
		}
		if (!GetSubNodeValue(root_element, "expend_num_2", cfg.expend_num_2) || cfg.expend_num_2 < 0)
		{
			errormsg = STRING_SPRINTF("[expend_num_2=%d] error,  cfg.expend_num_2 < 0 ?", cfg.expend_num_2);
			return -5;
		}
		if (!GetSubNodeValue(root_element, "expend_item_3", cfg.expend_item_3) || (cfg.expend_item_3 != 0 &&NULL == ITEMPOOL->GetItem(cfg.expend_item_3)))
		{
			errormsg = STRING_SPRINTF("[expend_item_3=%d] error, NULL == ITEMPOOL->GetItem(cfg.expend_item_3) ?", cfg.expend_item_3);
			return -6;
		}
		if (!GetSubNodeValue(root_element, "expend_num_3", cfg.expend_num_3) || cfg.expend_num_3 < 0)
		{
			errormsg = STRING_SPRINTF("[expend_num_3=%d] error,  cfg.expend_num_3 < 0 ?", cfg.expend_num_3);
			return -7;
		}

		if (!GetSubNodeValue(root_element, "coin", cfg.use_coin) || cfg.use_coin < 0)
		{
			errormsg = STRING_SPRINTF("[coin=%d] error,  cfg.use_coin < 0 ?", cfg.use_coin);
			return -8;
		}

		int broadcast = 0;
		if (!GetSubNodeValue(root_element, "is_broadcast", broadcast))
		{
			errormsg = STRING_SPRINTF("error, not is_broadcast ?");
			return -9;
		}
		cfg.is_broadcast = broadcast != 0;

		std::string att_up_str;
		GetSubNodeValue(root_element, "att_up", att_up_str);

		if(att_up_str.size() > 0)
		{
			std::vector<char *> str_list;
			char * p_tmp = NULL;
			p_tmp = strtok(const_cast<char *>(att_up_str.c_str()), "|");

			while (p_tmp)
			{
				str_list.push_back(p_tmp);
				p_tmp = strtok(NULL, "|");
			}

			for (int i = 0; i < str_list.size(); i++)
			{
				char * p_str = strtok(str_list[i], ":");

				PartnerIntensifyCfg::AttrData attr_data;
				attr_data.type = atoi(p_str);
				p_str = strtok(NULL, ":");
				attr_data.param = atoi(p_str);
				
				cfg.attr_list.push_back(attr_data);
			}
		}		

		m_partner_intensify_cfg[partner_id].push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int PartnerConfig::GetSoarArmsKey(int arms_id, int arms_level)
{
	return arms_id * MAX_PARTNER_SOAR_ARMS_BASE_NUM + arms_level;
}

int PartnerConfig::CheckFetterGroupsCfg()
{
	for (std::map<int, std::vector<FettersGroupsCfg> >::iterator m_it = m_v_f_group.begin(); m_it != m_v_f_group.end(); ++m_it)
	{
		for (std::vector<FettersGroupsCfg>::iterator v_it = m_it->second.begin(); v_it != m_it->second.end(); ++v_it)
		{
			if (m_s_fetters.find(v_it->fetter_id) == m_s_fetters.end()) return -1;
		}
	}

	return 0;
}

int PartnerConfig::InitLevelSkillCfgMap()
{
	std::map<PartnerCfgHead, PartnerUpGradeCfg>::iterator u_it = m_s_upgrade.begin();
	for (; u_it != m_s_upgrade.end(); ++u_it)
	{
		PartnerBasicCfg* partner_cfg = this->MutablePartnerBasicCfg(u_it->first.partner_id);
		if (NULL == partner_cfg) continue;

		PartnerLevelSkillCfg cfg;
		cfg.gift_skill_id = u_it->second.talent_skill_id;
		cfg.gift_skill_level = u_it->second.talent_skill_lv;

	/*	for (size_t i = 0; i < PARTNER_COMMON_SKILL_MAX; ++i)
		{
			cfg.skill_id_list[i] = u_it->second.common_skill_id_list[i];
			cfg.skill_level_list[i] = u_it->second.common_skill_lv_list[i];
		}
	*/	
		m_level_skill_cfg_map.insert(LevelSkillCfgMap::value_type(u_it->first, cfg));
	}

	return 0;
}

PartnerBasicCfg * PartnerConfig::MutablePartnerBasicCfg(int partner_id)
{
	if (partner_id < PARTNER_ID_START || partner_id >= PARTNER_NUM_MAX) return NULL;

	if (m_basic_cfg_list[partner_id].Invalid()) return NULL;

	return &m_basic_cfg_list[partner_id];
}
