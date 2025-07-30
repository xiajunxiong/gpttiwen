#include "hundredgodbossconfig.hpp"
#include "checkresourcecenter.hpp"

#include "task/taskpool.h"

HundredGodBossConfig::HundredGodBossConfig() : m_target_count(0)
{
}

HundredGodBossConfig::~HundredGodBossConfig()
{
}

bool HundredGodBossConfig::Init(const std::string & configname, std::string * err)
{

	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("target", InitTargetCfg);
	LOAD_CONFIG_2("other", InitOtherCfg);

	return true;
}

const HundredGodBossTargetCfg * HundredGodBossConfig::GetTargetCfg(int target_id)
{
	if(target_id <= 0 || target_id > m_target_count || target_id > ARRAY_ITEM_COUNT(m_target_list)) return NULL;

	return &m_target_list[target_id - 1];
}

void HundredGodBossConfig::GetAllTargetCfg(std::vector<HundredGodBossTargetCfg>& list)
{
	for (int i = 0; i < m_target_count && i < ARRAY_ITEM_COUNT(m_target_list); ++i)
	{
		list.push_back(m_target_list[i]);
	}
}

int HundredGodBossConfig::InitTargetCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	m_target_count = 0;
	while (NULL != data_element)
	{
		int id = 0;
		if (!GetSubNodeValue(data_element, "id", id) || id != m_target_count + 1 || id > MAX_HUNDRED_GOD_BOSS_TARGET_NUM)
		{
			errormsg = STRING_SPRINTF("[id=%d] error, is not find data or != [%d] or > %d?", id, m_target_count + 1, MAX_HUNDRED_GOD_BOSS_TARGET_NUM);
			return -1;
		}

		HundredGodBossTargetCfg node_cfg;
		if (!GetSubNodeValue(data_element, "cond_type", node_cfg.cond_type) || node_cfg.cond_type <= HUNDRED_GOD_BOSS_COND_TYPE_INVALID ||
			node_cfg.cond_type >= HUNDRED_GOD_BOSS_COND_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[id=%d] error, cond_type[%d] is not find data or <= [%d] or >= %d?", id, node_cfg.cond_type, 
				HUNDRED_GOD_BOSS_COND_TYPE_INVALID, HUNDRED_GOD_BOSS_COND_TYPE_MAX);
			return -2;
		}
		if (!GetSubNodeValue(data_element, "param", node_cfg.param))
		{
			errormsg = STRING_SPRINTF("[id=%d] error, param[%d] not find data?", id, node_cfg.param);
			return -3;
		}

		switch (node_cfg.cond_type)
		{
			case HUNDRED_GOD_BOSS_COND_TYPE_LEVEL:
			case HUNDRED_GOD_BOSS_COND_TYPE_BOUNTY_TASK:
			{
				if (node_cfg.param <= 0)
				{
					errormsg = STRING_SPRINTF("[id=%d] error, param[%d] <= 0?", id, node_cfg.param);
					return -4;
				}
			}
			break;
			case HUNDRED_GOD_BOSS_COND_TYPE_TASK:
			{
				if (NULL == TASKPOOL->GetTaskCfgByID(node_cfg.param))
				{
					errormsg = STRING_SPRINTF("[id=%d] error, param[%d] not find task?", id, node_cfg.param);
					return -5;
				}
			}
			break;
			default:
			break;
		}

		TiXmlElement* reward_Element = data_element->FirstChildElement("finish_reward");
		if(reward_Element == NULL)
		{
			errormsg = STRING_SPRINTF("[id=%d] error, not find data[finish_reward]?", id);
			return -6;
		}

		if (!node_cfg.reward.ReadConfig(reward_Element))
		{
			errormsg = STRING_SPRINTF("[id=%d] error, data[finish_reward] error?", id);
			return -7;
		}

		m_target_list[m_target_count++] = node_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int HundredGodBossConfig::InitOtherCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(data_element, "prepose_task", m_other_cfg.pre_task) || NULL == TASKPOOL->GetTaskCfgByID(m_other_cfg.pre_task))
	{
		errormsg = STRING_SPRINTF("[prepose_task=%d] error, is not find data or not has task?", m_other_cfg.pre_task);
		return -1;
	}

	if (!GetSubNodeValue(data_element, "during_time", m_other_cfg.during_time) || m_other_cfg.during_time <= 0)
	{
		errormsg = STRING_SPRINTF("[during_time=%d] error, is not find data or <= 0?", m_other_cfg.during_time);
		return -2;
	}

	if (!GetSubNodeValue(data_element, "partner_id", m_other_cfg.partner_id))
	{
		errormsg = STRING_SPRINTF("[partner_id=%d] error, is not find data?", m_other_cfg.partner_id);
		return -3;
	}
	CheckResourceCenter::Instance().GetPartnerCheck()->Add(m_other_cfg.partner_id, __FUNCTION__);

	return 0;
}
