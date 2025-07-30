#include "holyequipmentconfig.hpp"

#include "gamedef.h"
#include "servercommon/configcommon.h"
#include "item/shengqi/holyitem.h"
#include "item/itembase.h"
#include "item/shengqi/runestoneitem.h"
#include "config/logicconfigmanager.hpp"
#include "servercommon/serverconfig/battleconfig/battle_hallow_gift_pool.hpp"
#include "other/rolemodulemanager.hpp"

HolyEquipMentConfig::HolyEquipMentConfig()
{
}

HolyEquipMentConfig::~HolyEquipMentConfig()
{
	std::vector<HolyEquipBaseCfg>().swap(m_shengqi_base_cfg);
	std::vector<HolyEquipTalentCfg>().swap(m_talent_cfg);
	std::vector<HolyEquipInitiativeSkillCfg>().swap(m_initiative_skill_cfg);
	std::vector<HolyEquipLevelUpCfg>().swap(m_level_up_cfg);
	std::vector<RuneStoneCfg>().swap(m_rune_stone_cfg);
}

bool HolyEquipMentConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("shengqi_type", InitBaseCfg);
	LOAD_CONFIG_2("tianfu_skill", InitTaLentCfg);
	LOAD_CONFIG_2("zhudong_skill", InitSkillCfg);
	LOAD_CONFIG_2("skill_obtain", InitSkillStudyCfg);
	LOAD_CONFIG_2("skill_hole", InitSkillHoleCfg);
	LOAD_CONFIG_2("shengqi_upgrade", InitHolyLevelUpCfg);
	LOAD_CONFIG_2("exp_mat", InitLevelUpItemCfg);
	LOAD_CONFIG_2("fushi", InitRuneStoneCfg);
	LOAD_CONFIG_2("fushi_hole", InitRuneStoneHoleCfg);
	LOAD_CONFIG_2("fushi_compose", InitRuneStoneFusingCfg);
	LOAD_CONFIG_2("fushi_chongzhu", InitRuneStoneRecastCfg);
	LOAD_CONFIG_2("other", InitOtherCfg);

	return true;
}

const HolyEquipBaseCfg * HolyEquipMentConfig::GetBaseCfgById(ItemID item_id) const
{
	for (size_t i = 0; i < m_shengqi_base_cfg.size(); i++)
	{
		if (item_id == m_shengqi_base_cfg[i].item_id)
		{
			return &m_shengqi_base_cfg[i];
		}
	}
	return NULL;
}

const HolyEquipLevelUpAttrCfg * HolyEquipMentConfig::GetHolyLevelCfg(int shengqi_quality, int level) const
{
	for (size_t i = 0; i < m_level_up_cfg.size(); i++)
	{
		if (shengqi_quality == m_level_up_cfg[i].shengqi_quality)
		{
			std::map<int, HolyEquipLevelUpAttrCfg>::const_iterator it = m_level_up_cfg[i].level_cfg.find(level);
			if (it != m_level_up_cfg[i].level_cfg.end()) return &it->second;
		}
	}
	return NULL;
}

int HolyEquipMentConfig::GetLevelBySkillId(int skill_id) const
{
	std::map<int, int>::const_iterator it = m_skill_find_level_cfg.find(skill_id);
	if (it != m_skill_find_level_cfg.end()) return it->second;

	return 0;
}

int HolyEquipMentConfig::GetShengQiIdByItemId(ItemID item_id) const
{
	for (size_t i = 0; i < m_shengqi_base_cfg.size(); i++)
	{
		if (m_shengqi_base_cfg[i].item_id == item_id)
		{
			return m_shengqi_base_cfg[i].shengqi_id;
		}
	}

	return -1;
}

int HolyEquipMentConfig::GetSkillIdByShanChangId(int shanchang_id) const
{
	std::map<int, int>::const_iterator it = m_skill_shanchang_find_cfg.find(shanchang_id);
	if (it != m_skill_shanchang_find_cfg.end()) return it->second;

	return 0;
}

int HolyEquipMentConfig::RandHolyItemTaLent(HolyEquipParam & data, int shengqi_quality, int shengqi_type)
{
	/***************************
	/  Ϊʥ��������ظ����츳����,Ҳ����������ͬ���츳������(�츳�ĸ߼��汾����ͨ��),��Ϊ�츳����ֻ�ڵ��߻��ʱ�����ֵ,����ֱ�����ⲿ�жϲ����Ϸ�����,����Ĭ���츳����Ϊ��
	/  shengqi_quality: ��ɫƷ�ʵ�ʥ���������������ͼ��츳���ܣ���ɫƷ��ʥ���������������߼��츳���ܣ���ɫƷ��ʥ�������������������츳����
	/  shengqi_type: �����츳������ʥ��������ƣ�ֻ������ڶ�Ӧ����ʥ��,0--ȫ����ͨ��
	***************************/

	std::vector<HolyEquipTalentCfg> skill_list;	// ����������츳�б�

	for (size_t i = 0; i < m_talent_cfg.size(); i++)
	{
		if (m_talent_cfg[i].appear_type == HolyItem::HOLY_ITEM_TYPE_ALL || m_talent_cfg[i].appear_type == shengqi_type)
		{
			skill_list.push_back(m_talent_cfg[i]);
		}
	}
	if(skill_list.empty()) return 0;

	RandomShuffle(&skill_list[0], (int)skill_list.size());

	int talent_quality = 0;
	switch (shengqi_quality)
	{
	case ItemBase::I_COLOR_PURPLE:
			talent_quality = HolyEquipTalentCfg::SKILL_TYPE_COMMON;
		break;
	case ItemBase::I_COLOR_ORANGE:
			talent_quality = HolyEquipTalentCfg::SKILL_TYPE_STRONGER;
		break;
	case ItemBase::I_COLOR_RED:
			talent_quality = HolyEquipTalentCfg::SKILL_TYPE_SUPER;
		break;
	}

	int talent_count = 0;
	memset(data.talent_skills, 0, sizeof(data.talent_skills));
	for (size_t i = 0; i < skill_list.size(); i++)
	{
		if (skill_list[i].skill_type == talent_quality)
		{
			data.talent_skills[talent_count++] = skill_list[i].skill_id;

			if (talent_count >= HolyEquipTalentCfg::MAX_HOLY_EQUIP_TALENT_COUNT) break;
		}
	}

	return 0;
}

int HolyEquipMentConfig::RandHolyItemSkill(const HolyEquipParam& data, ItemID item_id, int shengqi_type, int skill_quality, int skill_hole)
{
	/*************************** 
	/  Ϊʥ��������ظ�����������,��һ����λ����ʥ��Ʒ������������ܵȼ�,�����Ĭ��1��
	***************************/
	if (skill_hole < 0 || skill_hole >= MAX_ACTIVE_SKILLS_NUM) return -1;

	int skill_level = 1;	// ����ļ��ܵȼ�Ĭ����1
	if (skill_hole == 0)	// ����ǵ�һ������ʱ, ��ô����ʥ��Ʒ���������
	{
		switch (skill_quality)
		{
		case ItemBase::I_COLOR_PURPLE:
				skill_level = 1;
			break;
		case ItemBase::I_COLOR_ORANGE:
				skill_level = 2;
			break;
		case ItemBase::I_COLOR_RED:
				skill_level = 3;
			break;
		}
	}

	std::map<int, std::map<ItemID, int> >::iterator it = m_skill_group_cfg.find(shengqi_type);
	if (it == m_skill_group_cfg.end()) return -2;

	std::vector<int> skill_cfg;		// ��ѡȡ�ļ����б�

	std::map<ItemID, int>::const_iterator node_it = it->second.begin();

	int out_skill_id = 0, out_skill_lv = 0;
	for (; node_it != it->second.end(); node_it++)	// ѭ�����м����б�
	{
		if (!this->GetSkillLevel(node_it->second, &out_skill_id, &out_skill_lv))	// ��ȡ���ܵȼ�
		{
			return -3;
		}
	
		if (out_skill_lv == skill_level)
		{
			bool has_same = false;
			for (int i = 0; i < MAX_ACTIVE_SKILLS_NUM; i++)
			{
				if (this->GetSkillLevel(data.active_skills[i], &out_skill_id, &out_skill_lv))	// ����out_skill_id, out_skill_lv
				{
					if (out_skill_id == node_it->second)
					{
						has_same = true;
						break;
					}
				}
			}

			if (!has_same)
			{
				skill_cfg.push_back(node_it->second);
			}
		}
	}
	if (skill_cfg.empty()) return -4;

	RandomShuffle(&skill_cfg[0], (int)skill_cfg.size());
	int skill_id = skill_cfg[0];

	int shengqi_id = this->GetShengQiIdByItemId(item_id);	//
	if (shengqi_id < 0) return -6;

	// �����Ƿ�ƥ���ó���������
	for (size_t i = 0; i < m_initiative_skill_cfg.size(); i++)
	{
		if (skill_id == m_initiative_skill_cfg[i].skill_pair.first)
		{
			bool is_shanchang = m_initiative_skill_cfg[i].shanchang_shengqi.find(shengqi_id) == m_initiative_skill_cfg[i].shanchang_shengqi.end() ? false : true;
			skill_id = is_shanchang ? m_initiative_skill_cfg[i].skill_pair.second : m_initiative_skill_cfg[i].skill_pair.first;
		}
	}

	return skill_id;
}

bool HolyEquipMentConfig::RandHolyItemAttr(HolyEquipParam & data, ItemID item_id, int shengqi_quality, int shengqi_type, std::string& error)
{
	if (data.has_net_value != 0)
	{
		error = STRING_SPRINTF("data.has_net_value != 0 value[%d]", data.has_net_value);
		return false;
	}

	const HolyEquipBaseCfg* base_cfg = this->GetBaseCfgById(item_id);
	if (NULL == base_cfg)
	{
		error = STRING_SPRINTF("NULL == base_cfg item_id[%d]",(int)item_id);
		return false;
	}

	const HolyEquipLevelUpAttrCfg * level_cfg = this->GetHolyLevelCfg(shengqi_quality, 1);	// ��һ�����ɾ�ֵ,�ȼ��̶���1
	if (NULL == level_cfg)
	{
		error = STRING_SPRINTF("NULL == level_cfg shengqi_quality[%d]", shengqi_quality);
		return false;
	}

	int error_num = 0;

	data.Reset();
	data.has_net_value = 1;
	data.level = 1;
	data.exp = 0;

	////////////////////  ʥ����������	/////////////////////////////
	for (int i = 0; i < ARRAY_LENGTH(data.base_attr_list); ++i)
	{
		data.base_attr_list[i].attr_type = base_cfg->attrs[i].attr_type;
		data.base_attr_list[i].attr_value = RandomNum(base_cfg->attrs[i].attr_min, base_cfg->attrs[i].attr_max);

		if (base_cfg->attrs[i].attr_min == base_cfg->attrs[i].attr_max)
		{
			data.base_attr_list[i].attr_value = base_cfg->attrs[i].attr_max;
		}
	}

	////////////////////  ʥ����������  /////////////////////////////
	int skill_id = this->RandHolyItemSkill(data, item_id, shengqi_type, shengqi_quality, 0);
	if(skill_id < 0)
	{
		error = STRING_SPRINTF("RandHolyItemSkill is failed!!! item_id[%d] shengqi_type[%d] active_skill_hole[%d]", (int)item_id, shengqi_type, 0);
		return false;
	}
	data.active_skills[0] = skill_id;

	////////////////////  ʥ���츳����	/////////////////////////////
	if (shengqi_quality >= ItemBase::I_COLOR_PURPLE && shengqi_quality <= ItemBase::I_COLOR_RED)
	{
		error_num = this->RandHolyItemTaLent(data, shengqi_quality, shengqi_type);
		if (error_num < 0)
		{
			error = STRING_SPRINTF("RandHolyItemTaLent is failed!!! shengqi_quality[%d] shengqi_type[%d]", shengqi_quality, shengqi_type);
			return false;
		}
	}

	//////////////////  ʥ���ȼ�����  /////////////////////////////
	data.active_max = level_cfg->active_max;
	// ��ʼ��
	for (int i = 0; i < MAX_HOLY_EQUIP_EXTRA_VOC_NUM; ++i)
	{
		data.m_extra_attr_list[i].attr_type = -1;
		data.m_extra_attr_list[i].attr_value = 0;
	}

	data.valid_extra_attr_num = (short)base_cfg->attr_type_list.size();
	int attr_count = 0;
	for (size_t i = 0; i < base_cfg->attr_type_list.size() && attr_count < MAX_HOLY_EQUIP_EXTRA_VOC_NUM; i++)
	{
		data.m_extra_attr_list[attr_count].attr_type = base_cfg->attr_type_list[i];
		data.m_extra_attr_list[attr_count].attr_value = level_cfg->attr_list[base_cfg->attr_type_list[i]].attr_value;
		attr_count++;
	}

	//////////////////  ��ʯ  /////////////////////////////
	// �µ���û�з�ʯ,������һ����ʯ��
	data.rune_stone_hole[0] = HolyEquipParam::SKILL_HOLE_STATUS_UNLOCK;

	return true;
}

bool HolyEquipMentConfig::RandRuneStoneAttr(Role* role, RuneStoneParam& data, int runestone_type, ItemID item_id)
{
	if (NULL == role) return false;

	RuneStoneBaseCfg base_cfg; bool is_find = false;
	for (size_t i = 0; i < m_rune_stone_cfg.size(); i++)
	{
		if (runestone_type == m_rune_stone_cfg[i].fushi_type)
		{
			for (size_t j = 0; j < m_rune_stone_cfg[i].fushi_attr.size(); j++)
			{
				if (m_rune_stone_cfg[i].fushi_attr[j].item_id == item_id)
				{
					base_cfg = m_rune_stone_cfg[i].fushi_attr[j];
					is_find = true;
				}
			}
		}
	}
	if (!is_find) return false;

	data.Reset();
	data.has_net_value = 1;
	data.item_id = item_id;
	data.attr_num = RandomNum(MAX_RUNE_STONE_VOC_NUM) + 1;

	RandomShuffle(&base_cfg.attrs[0], MAX_HOLY_ATTR_NUM);

	for (int i = 0; i < data.attr_num && i < MAX_RUNE_STONE_VOC_NUM; ++i)
	{
		data.voc_list[i].attr_type = base_cfg.attrs[i].attr_type;
		data.voc_list[i].attr_value = RandomNum(base_cfg.attrs[i].attr_max - base_cfg.attrs[i].attr_min) + (base_cfg.attrs[i].attr_min + 1);
	}

	// ��������
	std::map<int, AttrInfo>::const_iterator it = base_cfg.role_attr_list.find(role->GetBaseProfession());
	if (it == base_cfg.role_attr_list.end()) return false;

	data.role_attr = it->second;

	return true;
}

const HolyEquipInitiativeSkillHoleCfg * HolyEquipMentConfig::GetSkillHoleCfg(int hole) const
{
	std::map<int, HolyEquipInitiativeSkillHoleCfg>::const_iterator it = m_skill_hole_cfg.find(hole);
	if (it != m_skill_hole_cfg.end()) return &it->second;

	return NULL;
}

const RuneStoneHoleCfg * HolyEquipMentConfig::GetFuShiHoleCfg(int fushi_hole) const
{
	std::map<int, RuneStoneHoleCfg>::const_iterator it = m_runestone_hole_cfg.find(fushi_hole);
	if (it != m_runestone_hole_cfg.end()) return &it->second;

	return NULL;
}

const RuneStoneFusingCfg * HolyEquipMentConfig::GetFuShiFusingCfg(int fushi_level)
{
	std::map<int, RuneStoneFusingCfg>::const_iterator it = m_fushi_fusing_cfg.find(fushi_level);
	if (it != m_fushi_fusing_cfg.end()) return &it->second;

	return NULL;
}

const RuneStoneRecastCfg * HolyEquipMentConfig::GetFuShiRecastCfg(int fushi_level)
{
	std::map<int, RuneStoneRecastCfg>::const_iterator it = m_fushi_recast_cfg.find(fushi_level);
	if (it != m_fushi_recast_cfg.end()) return &it->second;

	return NULL;
}

const ItemID * HolyEquipMentConfig::GetFuShiIdByTypeLevelCfg(int fushi_type, int fushi_level)
{
	if (fushi_type <= RuneStoneItem::RUNE_STONE_TYPE_INVALID || fushi_type >= RuneStoneItem::RUNE_STONE_TYPE_MAX) return NULL;
	if (fushi_level <= 0 || fushi_level > RuneStoneItem::MAX_RUNE_STONE_LEVEL) return NULL;

	for (size_t i = 0; i < m_rune_stone_cfg.size(); i++)
	{
		if (m_rune_stone_cfg[i].fushi_type == fushi_type)
		{
			for (size_t j = 0; j < m_rune_stone_cfg[i].fushi_attr.size(); j++)
			{
				if (m_rune_stone_cfg[i].fushi_attr[j].fushi_level == fushi_level)
				{
					return &m_rune_stone_cfg[i].fushi_attr[j].item_id;
				}
			}
		}
	}
	return NULL;
}

int HolyEquipMentConfig::GetItemExpCfg(ItemID item_id) const
{
	std::map<ItemID, int>::const_iterator it = m_upgrade_consume_cfg.find(item_id);
	if (it != m_upgrade_consume_cfg.end()) return it->second;

	return -1;
}

int HolyEquipMentConfig::GetHolyEquipExp(ItemID item_id) const
{
	for (size_t i = 0; i < m_shengqi_base_cfg.size(); i++)
	{
		if (m_shengqi_base_cfg[i].item_id == item_id)
		{
			return m_shengqi_base_cfg[i].fenjie_exp;
		}
	}

	return -1;
}

int HolyEquipMentConfig::GetSkillScoreCfg(int skill_id) const
{
	std::map<int, int>::const_iterator it = m_skill_score_map_cfg.find(skill_id);
	if (it != m_skill_score_map_cfg.end()) return it->second;

	return 0;
}

int HolyEquipMentConfig::GetSkillIdByTypeAndItem(int shengqi_type, int lingwu_item) const
{
	std::map<int, std::map<ItemID, int> >::const_iterator it = m_skill_group_cfg.find(shengqi_type);
	if (it != m_skill_group_cfg.end())
	{
		std::map<ItemID, int>::const_iterator node_it = it->second.find(lingwu_item);
		if (node_it != it->second.end())
		{
			return node_it->second;
		}
	}
	return -1;
}

bool HolyEquipMentConfig::GetSkillLevel(int skill_id, ARG_OUT int* out_skill_id, ARG_OUT int* out_skill_lv) const
{
	for (size_t i = 0; i < m_initiative_skill_cfg.size(); ++i)
	{
		if (m_initiative_skill_cfg[i].skill_pair.first == skill_id ||
			m_initiative_skill_cfg[i].skill_pair.second == skill_id)
		{
			*out_skill_id = m_initiative_skill_cfg[i].skill_pair.first;
			*out_skill_lv = m_initiative_skill_cfg[i].combo_level;
			return true;
		}
	}
	
	return false;
}

const AttrInfo * HolyEquipMentConfig::GetAttrBySkill(int skill_id) const
{
	int out_skill_id = 0, out_skill_lv = 0;
	if (!this->GetSkillLevel(skill_id, &out_skill_id, &out_skill_lv)) return NULL;

	std::map<int, AttrInfo>::const_iterator it = m_skill_attr_map_cfg.find(out_skill_id);
	if (it == m_skill_attr_map_cfg.end()) return NULL;

	return &it->second;
}

const AttrInfo * HolyEquipMentConfig::GetRoleAttr(Role * role, ItemID fushi_id, int fushi_type) const
{
	for (size_t i = 0; i < m_rune_stone_cfg.size(); i++)
	{
		if (fushi_type == m_rune_stone_cfg[i].fushi_type)
		{
			for (size_t j = 0; j < m_rune_stone_cfg[i].fushi_attr.size(); j++)
			{
				if (m_rune_stone_cfg[i].fushi_attr[j].item_id == fushi_id)
				{
					std::map<int, AttrInfo>::const_iterator it = m_rune_stone_cfg[i].fushi_attr[j].role_attr_list.find(role->GetBaseProfession());
					if (it != m_rune_stone_cfg[i].fushi_attr[j].role_attr_list.end())
					{
						return &it->second;
					}
				}
			}
		}
	}

	return NULL;
}

int HolyEquipMentConfig::InitBaseCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;
	
	while (NULL != data_element)
	{
		HolyEquipBaseCfg base_cfg;
		if (!GetSubNodeValue(data_element, "shengqi_id", base_cfg.shengqi_id) || base_cfg.shengqi_id < 0) return -1;
		if (!GetSubNodeValue(data_element, "item_id", base_cfg.item_id) || NULL == ITEMPOOL->GetItem(base_cfg.item_id))
		{
			errormsg = STRING_SPRINTF("can not find item_id[%d]", base_cfg.item_id);
			return -2;
		}
		if (!GetSubNodeValue(data_element, "shengqi_type", base_cfg.shengqi_type) || base_cfg.shengqi_type <= HolyItem::HOLY_ITEM_TYPE_ALL || base_cfg.shengqi_type >= HolyItem::HOLY_ITEM_TYPE_MAX) return -3;
		if (!GetSubNodeValue(data_element, "shengqi_quality", base_cfg.shengqi_quality) || base_cfg.shengqi_quality <= ItemBase::I_COLOR_INVALID ||  base_cfg.shengqi_quality >= ItemBase::I_COLOR_GOD_COLOR_BEGAIN) return -4;
		if (!GetSubNodeValue(data_element, "fenjie_exp", base_cfg.fenjie_exp) || base_cfg.fenjie_exp < 0) return -5;

		TiXmlElement* attr_type_element = data_element->FirstChildElement("att_type");
		if (NULL == attr_type_element) return -888;

		TiXmlElement * node_element = attr_type_element->FirstChildElement("node");
		if (NULL == node_element) return -889;

		while (node_element)
		{
			int attr_type = -1;
			if (!GetNodeValue(node_element, attr_type)) return -6;

			base_cfg.attr_type_list.push_back(attr_type);

			node_element = node_element->NextSiblingElement();
		}

		{
			base_cfg.attrs[0].attr_type = HolyItem::HOLY_ATTR_TYPE_ACTIVENESS;
			if (!GetSubNodeValue(data_element, "huoyue_min", base_cfg.attrs[0].attr_min) || base_cfg.attrs[0].attr_min < 0) return -10;
			if (!GetSubNodeValue(data_element, "huoyue_max", base_cfg.attrs[0].attr_max) || base_cfg.attrs[0].attr_max < base_cfg.attrs[0].attr_min) return -11;

			base_cfg.attrs[1].attr_type = HolyItem::HOLY_ATTR_TYPE_ADD_ATTACK;
			if (!GetSubNodeValue(data_element, "damage_min", base_cfg.attrs[1].attr_min) || base_cfg.attrs[1].attr_min < 0) return -20;
			if (!GetSubNodeValue(data_element, "damage_max", base_cfg.attrs[1].attr_max) || base_cfg.attrs[1].attr_max < base_cfg.attrs[1].attr_min) return -21;

			base_cfg.attrs[2].attr_type = HolyItem::HOLY_ATTR_TYPE_SPEED;
			if (!GetSubNodeValue(data_element, "speed_min", base_cfg.attrs[2].attr_min) || base_cfg.attrs[2].attr_min < 0) return -30;
			if (!GetSubNodeValue(data_element, "speed_max", base_cfg.attrs[2].attr_max) || base_cfg.attrs[2].attr_max < base_cfg.attrs[2].attr_min) return -31;

			base_cfg.attrs[3].attr_type = HolyItem::HOLY_ATTR_TYPE_RECOVER;
			if (!GetSubNodeValue(data_element, "huifu_min", base_cfg.attrs[3].attr_min) || base_cfg.attrs[3].attr_min < 0) return -40;
			if (!GetSubNodeValue(data_element, "huifu_max", base_cfg.attrs[3].attr_max) || base_cfg.attrs[3].attr_max < base_cfg.attrs[3].attr_min) return -41;

			base_cfg.attrs[4].attr_type = HolyItem::HOLY_ATTR_TYPE_HELP;
			if (!GetSubNodeValue(data_element, "yuanzhu_min", base_cfg.attrs[4].attr_min) || base_cfg.attrs[4].attr_min < 0) return -50;
			if (!GetSubNodeValue(data_element, "yuanzhu_max", base_cfg.attrs[4].attr_max) || base_cfg.attrs[4].attr_max < base_cfg.attrs[4].attr_min) return -51;

			base_cfg.attrs[5].attr_type = HolyItem::HOLY_ATTR_TYPE_FALL;
			if (!GetSubNodeValue(data_element, "luobao_min", base_cfg.attrs[5].attr_min) || base_cfg.attrs[5].attr_min < 0) return -60;
			if (!GetSubNodeValue(data_element, "luobao_max", base_cfg.attrs[5].attr_max) || base_cfg.attrs[5].attr_max < base_cfg.attrs[5].attr_min) return -61;

			base_cfg.attrs[6].attr_type = HolyItem::HOLY_ATTR_TYPE_COPE_FALL;
			if (!GetSubNodeValue(data_element, "kangluobao_min", base_cfg.attrs[6].attr_min) || base_cfg.attrs[6].attr_min < 0) return -70;
			if (!GetSubNodeValue(data_element, "kangluobao_max", base_cfg.attrs[6].attr_max) || base_cfg.attrs[6].attr_max < base_cfg.attrs[6].attr_min) return -71;
		}

		m_shengqi_base_cfg.push_back(base_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int HolyEquipMentConfig::InitTaLentCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		HolyEquipTalentCfg talent_cfg;

		if (!GetSubNodeValue(data_element, "skill_id", talent_cfg.skill_id) || talent_cfg.skill_id < 0) return -1;
		if (!GetSubNodeValue(data_element, "skill_class", talent_cfg.skill_class) || talent_cfg.skill_class < 0) return -2;
		if (!GetSubNodeValue(data_element, "skill_type", talent_cfg.skill_type) || (talent_cfg.skill_type != HolyEquipTalentCfg::SKILL_TYPE_COMMON &&
			talent_cfg.skill_type != HolyEquipTalentCfg::SKILL_TYPE_STRONGER && talent_cfg.skill_type != HolyEquipTalentCfg::SKILL_TYPE_SUPER)) return -3;

		if (!GetSubNodeValue(data_element, "appear_type", talent_cfg.appear_type) || talent_cfg.appear_type < HolyItem::HOLY_ITEM_TYPE_ALL || talent_cfg.appear_type >= HolyItem::HOLY_ITEM_TYPE_MAX) return -4;

		int talent_score = 0;
		if (!GetSubNodeValue(data_element, "skill_score", talent_score) || talent_score < 0) return -5;

		m_skill_score_map_cfg[talent_cfg.skill_id] = talent_score;

		m_talent_cfg.push_back(talent_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int HolyEquipMentConfig::InitSkillCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (NULL != data_element)
	{
		HolyEquipInitiativeSkillCfg skill_cfg;

		if (!GetSubNodeValue(data_element, "combo_level", skill_cfg.combo_level) || skill_cfg.combo_level <= 0) return -1;
		if (!GetSubNodeValue(data_element, "skill_type", skill_cfg.skill_type) || skill_cfg.skill_type <= HolyItem::HOLY_ITEM_TYPE_ALL || skill_cfg.skill_type >= HolyItem::HOLY_ITEM_TYPE_MAX) return -2;

		int skill_id = 0, shanchang_id = 0;
		if (!GetSubNodeValue(data_element, "skill_id", skill_id) || skill_id < 0) return -3;
		if (!GetSubNodeValue(data_element, "shanchang_id", shanchang_id) || shanchang_id < 0) return -4;

		int skill_score = 0, shanchang_skill_score = 0;
		if (!GetSubNodeValue(data_element, "skill_score", skill_score) || skill_score < 0) return -21;
		if (!GetSubNodeValue(data_element, "shanchang_skill_score", shanchang_skill_score) || shanchang_skill_score < 0) return -22;

		AttrInfo attr;
		if (!GetSubNodeValue(data_element, "att_type", attr.attr_type) || attr.attr_type < HolyItem::HOLY_ATTR_TYPE_MIN || attr.attr_type >= HolyItem::HOLY_ATTR_TYPE_MAX) return -23;
		if (!GetSubNodeValue(data_element, "att", attr.attr_value) || attr.attr_value < 0) return -24;

		m_skill_score_map_cfg[skill_id] = skill_score;
		m_skill_score_map_cfg[shanchang_id] = shanchang_skill_score;

		if (skill_id == shanchang_id) return -5;
		skill_cfg.skill_pair = std::make_pair(skill_id, shanchang_id);
		m_skill_attr_map_cfg[skill_id] = attr;
		m_skill_shanchang_find_cfg.insert(std::make_pair(shanchang_id, skill_id));

		TiXmlElement * shanchang_shengqi_list = data_element->FirstChildElement("shanchang_shengqi");
		if (NULL == shanchang_shengqi_list) return -__LINE__;
		TiXmlElement *node_element = shanchang_shengqi_list->FirstChildElement("node");
		while (node_element)
		{
			int shengqi_id = -1;
			if (!GetNodeValue(node_element, shengqi_id) || shengqi_id < 0)
			{
				return -6;
			}

			skill_cfg.shanchang_shengqi.insert(shengqi_id);

			node_element = node_element->NextSiblingElement();
		}

		m_initiative_skill_cfg.push_back(skill_cfg);
		m_skill_find_level_cfg[skill_id] = skill_cfg.combo_level;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int HolyEquipMentConfig::InitSkillStudyCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int shengqi_type = 0;
		if (!GetSubNodeValue(data_element, "shengqi_type", shengqi_type) || shengqi_type <= HolyItem::HOLY_ITEM_TYPE_ALL || shengqi_type >= HolyItem::HOLY_ITEM_TYPE_MAX) return -1;

		if (m_skill_group_cfg.find(shengqi_type) == m_skill_group_cfg.end())
		{
			m_skill_group_cfg.insert(std::make_pair(shengqi_type, std::map<ItemID, int>()));
		}

		std::map<int, std::map<ItemID, int> >::iterator type_it = m_skill_group_cfg.find(shengqi_type);
		if (type_it == m_skill_group_cfg.end()) return -__LINE__;
		std::map<ItemID, int>& group_cfg = type_it->second;

		std::pair<ItemID, int> study_cfg;

		if (!GetSubNodeValue(data_element, "lingwu_item", study_cfg.first) || NULL == ITEMPOOL->GetItem(study_cfg.first)) return -3;
		if (!GetSubNodeValue(data_element, "skill_id", study_cfg.second) || study_cfg.second < 0) return -4;

		if (group_cfg.find(study_cfg.first) != group_cfg.end())
		{
			errormsg = STRING_SPRINTF("same lingwu_item:%d", study_cfg.first);
			return -5;
		}

		group_cfg.insert(study_cfg);

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int HolyEquipMentConfig::InitSkillHoleCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int hole = 0;
		HolyEquipInitiativeSkillHoleCfg skill_hole_cfg;

		if (!GetSubNodeValue(data_element, "hole", hole) || hole < 0) return -1;
		if (!GetSubNodeValue(data_element, "unlock_cost", skill_hole_cfg.unlock_cost) || NULL == ITEMPOOL->GetItem(skill_hole_cfg.unlock_cost)) return -2;
		if (!GetSubNodeValue(data_element, "num", skill_hole_cfg.num) || skill_hole_cfg.num < 0) return -3;
		if (!GetSubNodeValue(data_element, "shengqi_quality", skill_hole_cfg.shengqi_quality) || skill_hole_cfg.shengqi_quality <= ItemBase::I_COLOR_INVALID || skill_hole_cfg.shengqi_quality >= ItemBase::I_COLOR_GOD_COLOR_BEGAIN) return -4;

		m_skill_hole_cfg[hole] = skill_hole_cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int HolyEquipMentConfig::InitHolyLevelUpCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int shengqi_quality = 0;
		if (!GetSubNodeValue(data_element, "shengqi_quality", shengqi_quality) || shengqi_quality <= ItemBase::I_COLOR_INVALID || shengqi_quality >= ItemBase::I_COLOR_GOD_COLOR_BEGAIN) return -1;

		if (!m_level_up_cfg.empty())
		{
			if (m_level_up_cfg[m_level_up_cfg.size() - 1].shengqi_quality != shengqi_quality)
			{
				m_level_up_cfg.push_back(HolyEquipLevelUpCfg());
			}
		}
		else
		{
			m_level_up_cfg.push_back(HolyEquipLevelUpCfg());
		}

		HolyEquipLevelUpCfg& level_cfg = m_level_up_cfg[m_level_up_cfg.size() - 1];
		level_cfg.shengqi_quality = shengqi_quality;

		int level = 0;
		if (!GetSubNodeValue(data_element, "level", level) || level <= 0 || level > HolyItem::MAX_HOLY_ITEM_LEVEL) return -2;

		HolyEquipLevelUpAttrCfg attr_cfg;

		if (!GetSubNodeValue(data_element, "upgrade_exp", attr_cfg.upgrade_exp) || attr_cfg.upgrade_exp < 0) return -3;
		if (!GetSubNodeValue(data_element, "coin_cost", attr_cfg.coin_cost) || attr_cfg.coin_cost < 0) return -4;
		if (!GetSubNodeValue(data_element, "active_max", attr_cfg.active_max) || attr_cfg.active_max < 0) return -5;

		{
			attr_cfg.attr_list[BATTLE_ATTR_ATTACK].attr_type = BATTLE_ATTR_ATTACK;
			if (!GetSubNodeValue(data_element, "attack", attr_cfg.attr_list[BATTLE_ATTR_ATTACK].attr_value) || attr_cfg.attr_list[BATTLE_ATTR_ATTACK].attr_value < 0) return -10;

			attr_cfg.attr_list[BATTLE_ATTR_DEFENSE].attr_type = BATTLE_ATTR_DEFENSE;
			if (!GetSubNodeValue(data_element, "defense", attr_cfg.attr_list[BATTLE_ATTR_DEFENSE].attr_value) || attr_cfg.attr_list[BATTLE_ATTR_DEFENSE].attr_value < 0) return -11;

			attr_cfg.attr_list[BATTLE_ATTR_MENTAL].attr_type = BATTLE_ATTR_MENTAL;
			if (!GetSubNodeValue(data_element, "spirit", attr_cfg.attr_list[BATTLE_ATTR_MENTAL].attr_value) || attr_cfg.attr_list[BATTLE_ATTR_MENTAL].attr_value < 0) return -12;

			attr_cfg.attr_list[BATTLE_ATTR_RECOVERY].attr_type = BATTLE_ATTR_RECOVERY;
			if (!GetSubNodeValue(data_element, "restore", attr_cfg.attr_list[BATTLE_ATTR_RECOVERY].attr_value) || attr_cfg.attr_list[BATTLE_ATTR_RECOVERY].attr_value < 0) return -13;

			attr_cfg.attr_list[BATTLE_ATTR_MAX_HP].attr_type = BATTLE_ATTR_MAX_HP;
			if (!GetSubNodeValue(data_element, "hp", attr_cfg.attr_list[BATTLE_ATTR_MAX_HP].attr_value) || attr_cfg.attr_list[BATTLE_ATTR_MAX_HP].attr_value < 0) return -14;

			attr_cfg.attr_list[BATTLE_ATTR_MAGIC_ATTACK].attr_type = BATTLE_ATTR_MAGIC_ATTACK;
			if (!GetSubNodeValue(data_element, "magic_attack", attr_cfg.attr_list[BATTLE_ATTR_MAGIC_ATTACK].attr_value) || attr_cfg.attr_list[BATTLE_ATTR_MAGIC_ATTACK].attr_value < 0) return -15;

			attr_cfg.attr_list[BATTLE_ATTR_REAL_DAMAGE].attr_type = BATTLE_ATTR_REAL_DAMAGE;
			if (!GetSubNodeValue(data_element, "real_injury", attr_cfg.attr_list[BATTLE_ATTR_REAL_DAMAGE].attr_value) || attr_cfg.attr_list[BATTLE_ATTR_REAL_DAMAGE].attr_value < 0) return -16;

			attr_cfg.attr_list[BATTLE_ATTR_ANTI_POISON].attr_type = BATTLE_ATTR_ANTI_POISON;
			if (!GetSubNodeValue(data_element, "de_zhongdu", attr_cfg.attr_list[BATTLE_ATTR_ANTI_POISON].attr_value) || attr_cfg.attr_list[BATTLE_ATTR_ANTI_POISON].attr_value < 0) return -17;

			attr_cfg.attr_list[BATTLE_ATTR_ANTI_FREEZE].attr_type = BATTLE_ATTR_ANTI_FREEZE;
			if (!GetSubNodeValue(data_element, "de_bingfeng", attr_cfg.attr_list[BATTLE_ATTR_ANTI_FREEZE].attr_value) || attr_cfg.attr_list[BATTLE_ATTR_ANTI_FREEZE].attr_value < 0) return -18;

			attr_cfg.attr_list[BATTLE_ATTR_ANTI_PETRIFY].attr_type = BATTLE_ATTR_ANTI_PETRIFY;
			if (!GetSubNodeValue(data_element, "de_shihua", attr_cfg.attr_list[BATTLE_ATTR_ANTI_PETRIFY].attr_value) || attr_cfg.attr_list[BATTLE_ATTR_ANTI_PETRIFY].attr_value < 0) return -19;

			attr_cfg.attr_list[BATTLE_ATTR_ANTI_CHAOS].attr_type = BATTLE_ATTR_ANTI_CHAOS;
			if (!GetSubNodeValue(data_element, "de_hunluan", attr_cfg.attr_list[BATTLE_ATTR_ANTI_CHAOS].attr_value) || attr_cfg.attr_list[BATTLE_ATTR_ANTI_CHAOS].attr_value < 0) return -20;

			attr_cfg.attr_list[BATTLE_ATTR_ANTI_DRUNK].attr_type = BATTLE_ATTR_ANTI_DRUNK;
			if (!GetSubNodeValue(data_element, "de_jiuzui", attr_cfg.attr_list[BATTLE_ATTR_ANTI_DRUNK].attr_value) || attr_cfg.attr_list[BATTLE_ATTR_ANTI_DRUNK].attr_value < 0) return -21;

			attr_cfg.attr_list[BATTLE_ATTR_ANTI_SILENCE].attr_type = BATTLE_ATTR_ANTI_SILENCE;
			if (!GetSubNodeValue(data_element, "de_chenmo", attr_cfg.attr_list[BATTLE_ATTR_ANTI_SILENCE].attr_value) || attr_cfg.attr_list[BATTLE_ATTR_ANTI_SILENCE].attr_value < 0) return -22;

			attr_cfg.attr_list[BATTLE_ATTR_ANTI_SEVERE_INJURE].attr_type = BATTLE_ATTR_ANTI_SEVERE_INJURE;
			if (!GetSubNodeValue(data_element, "de_zhongshang", attr_cfg.attr_list[BATTLE_ATTR_ANTI_SEVERE_INJURE].attr_value) || attr_cfg.attr_list[BATTLE_ATTR_ANTI_SEVERE_INJURE].attr_value < 0) return -23;
		}

		level_cfg.level_cfg[level] = attr_cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int HolyEquipMentConfig::InitLevelUpItemCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while(data_element)
	{
		ItemID item_id = 0;
		int item_exp = 0;

		if (!GetSubNodeValue(data_element, "item_id", item_id) || NULL == ITEMPOOL->GetItem(item_id))
		{
			errormsg = STRING_SPRINTF("can not find item_id[%d]", (int)item_id);
			return -1;
		}
		if (!GetSubNodeValue(data_element, "item_exp", item_exp) || item_exp < 0) return -2;

		m_upgrade_consume_cfg.insert(std::make_pair(item_id, item_exp));

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int HolyEquipMentConfig::InitRuneStoneCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int fushi_type = 0;
		if (!GetSubNodeValue(data_element, "fushi_type", fushi_type) || fushi_type >= RuneStoneItem::RUNE_STONE_TYPE_MAX || fushi_type <= RuneStoneItem::RUNE_STONE_TYPE_INVALID)
		{
			errormsg = STRING_SPRINTF("[fushi_type] <= %d or >= %d", RuneStoneItem::RUNE_STONE_TYPE_INVALID, RuneStoneItem::RUNE_STONE_TYPE_MAX);
			return -1;
		}

		if (!m_rune_stone_cfg.empty())
		{
			if (m_rune_stone_cfg[m_rune_stone_cfg.size() - 1].fushi_type != fushi_type)
			{
				m_rune_stone_cfg.push_back(RuneStoneCfg());
			}
		}
		else
		{
			m_rune_stone_cfg.push_back(RuneStoneCfg());
		}

		RuneStoneCfg& fushi_cfg = m_rune_stone_cfg[m_rune_stone_cfg.size() - 1];
		fushi_cfg.fushi_type = fushi_type;

		RuneStoneBaseCfg base_cfg;

		if (!GetSubNodeValue(data_element, "item_id", base_cfg.item_id) || NULL == ITEMPOOL->GetItem(base_cfg.item_id))
		{
			errormsg = STRING_SPRINTF("can not find item_id[%d]", (int)base_cfg.item_id);
			return -1;
		}
		if (!GetSubNodeValue(data_element, "fushi_level", base_cfg.fushi_level) || base_cfg.fushi_level <= 0 || base_cfg.fushi_level > RuneStoneItem::MAX_RUNE_STONE_LEVEL)
		{
			errormsg = STRING_SPRINTF("[fushi_level] >= %d or <= 0", RuneStoneItem::MAX_RUNE_STONE_LEVEL);
			return -2;
		}

		{
			base_cfg.attrs[0].attr_type = HolyItem::HOLY_ATTR_TYPE_ACTIVENESS;
			if (!GetSubNodeValue(data_element, "huoyue_min", base_cfg.attrs[0].attr_min) || base_cfg.attrs[0].attr_min < 0) return -10;
			if (!GetSubNodeValue(data_element, "huoyue_max", base_cfg.attrs[0].attr_max) || base_cfg.attrs[0].attr_max < base_cfg.attrs[0].attr_min) return -11;

			base_cfg.attrs[1].attr_type = HolyItem::HOLY_ATTR_TYPE_ADD_ATTACK;
			if (!GetSubNodeValue(data_element, "damage_min", base_cfg.attrs[1].attr_min) || base_cfg.attrs[1].attr_min < 0) return -20;
			if (!GetSubNodeValue(data_element, "damage_max", base_cfg.attrs[1].attr_max) || base_cfg.attrs[1].attr_max < base_cfg.attrs[1].attr_min) return -21;

			base_cfg.attrs[2].attr_type = HolyItem::HOLY_ATTR_TYPE_SPEED;
			if (!GetSubNodeValue(data_element, "speed_min", base_cfg.attrs[2].attr_min) || base_cfg.attrs[2].attr_min < 0) return -30;
			if (!GetSubNodeValue(data_element, "speed_max", base_cfg.attrs[2].attr_max) || base_cfg.attrs[2].attr_max < base_cfg.attrs[2].attr_min) return -31;

			base_cfg.attrs[3].attr_type = HolyItem::HOLY_ATTR_TYPE_RECOVER;
			if (!GetSubNodeValue(data_element, "huifu_min", base_cfg.attrs[3].attr_min) || base_cfg.attrs[3].attr_min < 0) return -40;
			if (!GetSubNodeValue(data_element, "huifu_max", base_cfg.attrs[3].attr_max) || base_cfg.attrs[3].attr_max < base_cfg.attrs[3].attr_min) return -41;

			base_cfg.attrs[4].attr_type = HolyItem::HOLY_ATTR_TYPE_HELP;
			if (!GetSubNodeValue(data_element, "yuanzhu_min", base_cfg.attrs[4].attr_min) || base_cfg.attrs[4].attr_min < 0) return -50;
			if (!GetSubNodeValue(data_element, "yuanzhu_max", base_cfg.attrs[4].attr_max) || base_cfg.attrs[4].attr_max < base_cfg.attrs[4].attr_min) return -51;

			base_cfg.attrs[5].attr_type = HolyItem::HOLY_ATTR_TYPE_FALL;
			if (!GetSubNodeValue(data_element, "luobao_min", base_cfg.attrs[5].attr_min) || base_cfg.attrs[5].attr_min < 0) return -60;
			if (!GetSubNodeValue(data_element, "luobao_max", base_cfg.attrs[5].attr_max) || base_cfg.attrs[5].attr_max < base_cfg.attrs[5].attr_min) return -61;

			base_cfg.attrs[6].attr_type = HolyItem::HOLY_ATTR_TYPE_COPE_FALL;
			if (!GetSubNodeValue(data_element, "kangluobao_min", base_cfg.attrs[6].attr_min) || base_cfg.attrs[6].attr_min < 0) return -70;
			if (!GetSubNodeValue(data_element, "kangluobao_max", base_cfg.attrs[6].attr_max) || base_cfg.attrs[6].attr_max < base_cfg.attrs[6].attr_min) return -71;
		}

		{
			std::string att_type = "att_type", att = "att";
			for (int i = 1; i < PROFESSION_TYPE_NUM; i++)
			{
				std::pair<int, AttrInfo> temp_info;

				temp_info.first = i * PROFESSION_BASE;
				
				if (!GetSubNodeValue(data_element, STRING_SPRINTF("%s%d", att_type.c_str(), i).c_str(), temp_info.second.attr_type) || temp_info.second.attr_type < 0) return -101;
				if (!GetSubNodeValue(data_element, STRING_SPRINTF("%s%d", att.c_str(), i).c_str(), temp_info.second.attr_value) || temp_info.second.attr_value < 0) return -102;

				base_cfg.role_attr_list.insert(temp_info);
			}
		}

		fushi_cfg.fushi_attr.push_back(base_cfg);

		data_element = data_element->NextSiblingElement();
	}


	return 0;
}

int HolyEquipMentConfig::InitRuneStoneHoleCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int hole = 0;
		if (!GetSubNodeValue(data_element, "hole", hole) || hole < 0) return -1;
		if (m_runestone_hole_cfg.find(hole) != m_runestone_hole_cfg.end())
		{
			errormsg = STRING_SPRINTF("hole is repeat![%d]", hole);
			return -__LINE__;
		}

		RuneStoneHoleCfg hole_cfg;
		if (!GetSubNodeValue(data_element, "shengqi_level", hole_cfg.shengqi_level) || hole_cfg.shengqi_level < 0) return -2;
		if (!GetSubNodeValue(data_element, "unlock_cost", hole_cfg.unlock_cost) || NULL == ITEMPOOL->GetItem(hole_cfg.unlock_cost))
		{
			errormsg = STRING_SPRINTF("can not find item_id[%d]", (int)hole_cfg.unlock_cost);
			return -3;
		}
		if (!GetSubNodeValue(data_element, "num", hole_cfg.consume_num) || hole_cfg.consume_num < 0) return -4;

		m_runestone_hole_cfg.insert(std::make_pair(hole, hole_cfg));

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int HolyEquipMentConfig::InitRuneStoneFusingCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int fushi_level = 0;
		if (!GetSubNodeValue(data_element, "fushi_level", fushi_level) || fushi_level <= 0 || fushi_level >= RuneStoneItem::MAX_RUNE_STONE_LEVEL) return -1;

		RuneStoneFusingCfg fusing_cfg;
		if (!GetSubNodeValue(data_element, "need_num", fusing_cfg.need_item_num) || fusing_cfg.need_item_num <= 0) return -2;
		if (!GetSubNodeValue(data_element, "cost", fusing_cfg.cost_coin) || fusing_cfg.cost_coin < 0) return -3;

		m_fushi_fusing_cfg[fushi_level] = fusing_cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int HolyEquipMentConfig::InitRuneStoneRecastCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	while (data_element)
	{
		int fushi_level = 0;
		if (!GetSubNodeValue(data_element, "fushi_level", fushi_level) || fushi_level <= 0 || fushi_level > RuneStoneItem::MAX_RUNE_STONE_LEVEL) return -1;

		RuneStoneRecastCfg recast_cfg;
		if (!GetSubNodeValue(data_element, "need_num", recast_cfg.need_item_num) || recast_cfg.need_item_num <= 0) return -2;
		if (!GetSubNodeValue(data_element, "cost", recast_cfg.cost_coin) || recast_cfg.cost_coin < 0) return -3;

		m_fushi_recast_cfg[fushi_level] = recast_cfg;

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int HolyEquipMentConfig::InitOtherCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element) return -544;

	if (!GetSubNodeValue(data_element, "feisheng_type", m_other_cfg.feisheng_type) || m_other_cfg.feisheng_type < 0) return -1;
	if (!GetSubNodeValue(data_element, "exp_back", m_other_cfg.exp_back) || m_other_cfg.exp_back < 0) return -2;

	TiXmlElement* convert_item1_element = data_element->FirstChildElement("baoxiang1");
	if (NULL == convert_item1_element) return -99;
	if (!m_other_cfg.baoxiang1.ReadConfig(convert_item1_element)) return -3;

	TiXmlElement* convert_item2_element = data_element->FirstChildElement("baoxiang2");
	if (NULL == convert_item2_element) return -199;
	if (!m_other_cfg.baoxiang2.ReadConfig(convert_item2_element)) return -4;

	return 0;
}
