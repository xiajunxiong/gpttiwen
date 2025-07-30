#include "kuafuyingxiongluconfig.hpp"
#include "servercommon/rankdef.hpp"
#include "servercommon/medaldef.h"
#include "global/xunbao/xunbaoconfig.hpp"
#include "other/pet/petconfig.hpp"
#include "skill/skillpool.hpp"
#include "servercommon/partnerdef.h"

KuaFuYingXiongLuConfig::KuaFuYingXiongLuConfig()
{
}

KuaFuYingXiongLuConfig::~KuaFuYingXiongLuConfig()
{
	for (int i = 0; i < KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX; i++)
	{
		for (std::map<long long, std::vector<KuaFuYingXiongLuRankCfg> >::iterator it = m_rank_cfg_list[i].begin(); it != m_rank_cfg_list[i].end(); it++)
		{
			std::vector<KuaFuYingXiongLuRankCfg>().swap(it->second);
		}
		for (std::map<long long, std::vector<KuaFuYingXiongLuTaskCfg> >::iterator it = m_task_cfg_list[i].begin(); it != m_task_cfg_list[i].end(); it++)
		{
			std::vector<KuaFuYingXiongLuTaskCfg>().swap(it->second);
		}
	}
}

bool KuaFuYingXiongLuConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("other", InitOtherCfg);
	LOAD_CONFIG_2("time_configuration", InitTimeCfg);
	LOAD_CONFIG_2("basis_configuration", InitRankCfg);
	LOAD_CONFIG_2("task_configuration", InitTaskCfg);

	return true;
}

const KuaFuYingXiongLuTimeCfg * KuaFuYingXiongLuConfig::GetTimeCfg(int rank_type) const
{
	if (rank_type < 0 || rank_type >= ARRAY_ITEM_COUNT(m_time_cfg_list))
	{
		return NULL;
	}

	return &m_time_cfg_list[rank_type];
}

void KuaFuYingXiongLuConfig::GetRankVec(int section_day, int rank_type, std::vector<KuaFuYingXiongLuRankCfg>& rank_vec) const
{
	if (rank_type < 0 || rank_type >= ARRAY_ITEM_COUNT(m_rank_cfg_list) || section_day <= 0)
	{
		return;
	}

	const std::map<long long, std::vector<KuaFuYingXiongLuRankCfg> > & rank_cfg_map = m_rank_cfg_list[rank_type];
	for (std::map<long long, std::vector<KuaFuYingXiongLuRankCfg> >::const_iterator it = rank_cfg_map.begin(); it != rank_cfg_map.end(); ++it)
	{
		int section_start = 0, section_end = 0;
		this->GetInfoByKey(it->first, &section_start, &section_end);
		if (section_start <= section_day && section_end >= section_day)
		{
			rank_vec = it->second;
			return;
		}
	}

	return;
}

const KuaFuYingXiongLuTaskCfg * KuaFuYingXiongLuConfig::GetTaskCfg(int section_day, int rank_type, int seq) const
{
	const std::vector<KuaFuYingXiongLuTaskCfg>* task_vec_cfg = this->GetTaskVec(section_day, rank_type);
	if (NULL == task_vec_cfg || seq < 0 || seq >= (int)task_vec_cfg->size())
	{
		return NULL;
	}

	return &(*task_vec_cfg)[seq];
}

const std::vector<KuaFuYingXiongLuTaskCfg>* KuaFuYingXiongLuConfig::GetTaskVec(int section_day, int rank_type) const
{
	if (rank_type < 0 || rank_type >= ARRAY_ITEM_COUNT(m_task_cfg_list) || section_day <= 0)
	{
		return NULL;
	}

	const std::map<long long, std::vector<KuaFuYingXiongLuTaskCfg> > & task_cfg_map = m_task_cfg_list[rank_type];
	for (std::map<long long, std::vector<KuaFuYingXiongLuTaskCfg> >::const_iterator it = task_cfg_map.begin(); it != task_cfg_map.end(); ++it)
	{
		int section_start = 0, section_end = 0;
		this->GetInfoByKey(it->first, &section_start, &section_end);
		if (section_start <= section_day && section_end >= section_day)
		{
			return &it->second;
		}
	}

	return NULL;
}

int KuaFuYingXiongLuConfig::InitOtherCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	int is_can_open_activity = 0;
	if (!GetSubNodeValue(data_element, "is_can_open_activity", is_can_open_activity))
	{
		errormsg = STRING_SPRINTF("[is_can_open_activity=%d] error, is not find data?", is_can_open_activity);
		return -1;
	}
	m_other_cfg.is_can_open_activity = (0 != is_can_open_activity);

	return 0;
}

int KuaFuYingXiongLuConfig::InitTimeCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int rank_type = 0;
		if (!GetSubNodeValue(data_element, "rank_type", rank_type) || rank_type < KUA_FU_YING_XIONG_LU_RANK_TYPE_BEGIN || rank_type >= KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[rank_type=%d] error, is not find data?", rank_type);
			return -1;
		}

		KuaFuYingXiongLuTimeCfg node_cfg;
		if (!GetSubNodeValue(data_element, "start_time", node_cfg.start_time) || node_cfg.start_time <= 0)
		{
			errormsg = STRING_SPRINTF("[rank_type=%d] error, start_time[%d] <= 0?", rank_type, node_cfg.start_time);
			return -2;
		}

		if (!GetSubNodeValue(data_element, "continuou_times", node_cfg.continuou_times) || node_cfg.continuou_times <= 0)
		{
			errormsg = STRING_SPRINTF("[rank_type=%d] error, continuou_times[%d] <= 0?", rank_type, node_cfg.continuou_times);
			return -3;
		}

		m_time_cfg_list[rank_type] = node_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int KuaFuYingXiongLuConfig::InitRankCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start <= 0) 
		{
			errormsg = STRING_SPRINTF("[section_start=%d]error, <= 0 or not find data?", section_start);
			return -1;
		}
		if (!GetSubNodeValue(data_element, "section_end", section_end) || section_end < 0) 
		{
			errormsg = STRING_SPRINTF("[section_start=%d] [section_end=%d] error, < 0 or not find data?", section_start, section_end);
			return -2;
		}
		if (0 == section_end)
		{
			section_end = MAX_INT;
		}
		if (section_start > section_end) 
		{
			errormsg = STRING_SPRINTF("[section_start=%d] > [section_end=%d]?", section_start, section_end);
			return -3;
		}
		long long section_key = this->GetSectionKey(section_start, section_end);

		KuaFuYingXiongLuRankCfg node_cfg;
		if (!GetSubNodeValue(data_element, "rank_type", node_cfg.rank_type) || node_cfg.rank_type < KUA_FU_YING_XIONG_LU_RANK_TYPE_BEGIN || 
			node_cfg.rank_type >= KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX) 
		{
			errormsg = STRING_SPRINTF("[section_start=%d] [section_end=%d] error, rank_type[%d] < %d or >= %d?", section_start, section_end, 
				node_cfg.rank_type, KUA_FU_YING_XIONG_LU_RANK_TYPE_BEGIN, KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX);
			return -4;
		}

		std::vector<KuaFuYingXiongLuRankCfg> & cfg_list = m_rank_cfg_list[node_cfg.rank_type][section_key];
		if (!GetSubNodeValue(data_element, "seq", node_cfg.seq) || node_cfg.seq != (int)cfg_list.size() || node_cfg.seq >= MAX_KUA_FU_YING_XIONG_LU_RANK_TYPE_SEQ_NUM) 
		{
			errormsg = STRING_SPRINTF("[section_start=%d] [section_end=%d] error, seq[%d] != %d or >= %d?", section_start, section_end,
				node_cfg.seq, (int)cfg_list.size(), MAX_KUA_FU_YING_XIONG_LU_RANK_TYPE_SEQ_NUM);
			return -5;
		}

		TiXmlElement * list_node = data_element->FirstChildElement("reward1_item_list");
		if (NULL == list_node)
		{
			return -16;
		}

		TiXmlElement * item_node = list_node->FirstChildElement("reward1_item");
		while (NULL != item_node)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_node)) return -100;

			node_cfg.reward_list.push_back(item);
			item_node = item_node->NextSiblingElement();
		}

		cfg_list.push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int KuaFuYingXiongLuConfig::InitTaskCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -10000;
	}

	while (NULL != data_element)
	{
		int section_start = 0, section_end = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start) || section_start <= 0)
		{
			errormsg = STRING_SPRINTF("[section_start=%d]error, <= 0 or not find data?", section_start);
			return -1;
		}
		if (!GetSubNodeValue(data_element, "section_end", section_end) || section_end < 0)
		{
			errormsg = STRING_SPRINTF("[section_start=%d] [section_end=%d] error, < 0 or not find data?", section_start, section_end);
			return -2;
		}
		if (0 == section_end)
		{
			section_end = MAX_INT;
		}
		if (section_start > section_end)
		{
			errormsg = STRING_SPRINTF("[section_start=%d] > [section_end=%d]?", section_start, section_end);
			return -3;
		}
		long long section_key = this->GetSectionKey(section_start, section_end);

		KuaFuYingXiongLuTaskCfg node_cfg;
		if (!GetSubNodeValue(data_element, "rank_type", node_cfg.rank_type) || node_cfg.rank_type < KUA_FU_YING_XIONG_LU_RANK_TYPE_BEGIN ||
			node_cfg.rank_type >= KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[section_start=%d] [section_end=%d] error, rank_type[%d] < %d or >= %d?", section_start, section_end,
				node_cfg.rank_type, KUA_FU_YING_XIONG_LU_RANK_TYPE_BEGIN, KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX);
			return -4;
		}

		std::vector<KuaFuYingXiongLuTaskCfg> & cfg_list = m_task_cfg_list[node_cfg.rank_type][section_key];
		if (!GetSubNodeValue(data_element, "seq", node_cfg.seq) || node_cfg.seq != (int)cfg_list.size() || node_cfg.seq >= MAX_KUA_FU_YING_XIONG_LU_RANK_TASK_SEQ_NUM)
		{
			errormsg = STRING_SPRINTF("[section_start=%d] [section_end=%d] error, seq[%d] != %d or >= %d?", section_start, section_end,
				node_cfg.seq, (int)cfg_list.size(), MAX_KUA_FU_YING_XIONG_LU_RANK_TASK_SEQ_NUM);
			return -5;
		}

		if (!GetSubNodeValue(data_element, "type", node_cfg.task_type) || node_cfg.task_type <= KUA_FU_YING_XIONG_LU_TASK_TYPE_INVALID ||
			node_cfg.task_type >= KUA_FU_YING_XIONG_LU_TASK_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[section_start=%d] [section_end=%d] error, type[%d] <= %d or >= %d?", section_start, section_end,
				node_cfg.task_type, KUA_FU_YING_XIONG_LU_TASK_TYPE_INVALID, KUA_FU_YING_XIONG_LU_TASK_TYPE_MAX);
			return -6;
		}
		if (!GetSubNodeValue(data_element, "parameter1", node_cfg.param1))
		{
			errormsg = STRING_SPRINTF("[section_start=%d] [section_end=%d] error, [parameter1] not find data?", section_start, section_end);
			return -7;
		}
		if (!GetSubNodeValue(data_element, "parameter2", node_cfg.param2) || node_cfg.param2 <= 0)
		{
			errormsg = STRING_SPRINTF("[section_start=%d] [section_end=%d] error, parameter2[%d] <= 0?", section_start, section_end, node_cfg.param2);
			return -8;
		}
		

		UNSTD_STATIC_CHECK(KUA_FU_YING_XIONG_LU_TASK_TYPE_MAX == 10);
		switch (node_cfg.task_type)
		{
			case KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_MEDAL:
			{
				if (node_cfg.param1 < 0 || node_cfg.param1 >= MAX_MEDAL_MAX_GRADE)
				{
					errormsg = STRING_SPRINTF("[section_start=%d] [section_end=%d] error, parameter1[%d] < 0 or >= %d?", section_start, section_end, 
						node_cfg.param1, MAX_MEDAL_MAX_GRADE);
					return -10;
				}
			}
			break;
			case KUA_FU_YING_XIONG_LU_TASK_TYPE_INSERT_TONE:
			{
				if (node_cfg.param1 <= 0)
				{
					errormsg = STRING_SPRINTF("[section_start=%d] [section_end=%d] error, parameter1[%d] <= 0 ?", section_start, section_end, node_cfg.param1);
					return -11;
				}
			}
			break;
			case KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_EQUIP:
			case KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_PET_EQUIP:
			case KUA_FU_YING_XIONG_LU_TASK_TYPE_WEAR_SOUL_EQUIP:
			{
				if (node_cfg.param1 < ItemBase::I_COLOR_INVALID || node_cfg.param1 >= ItemBase::I_COLOR_MAX)
				{
					errormsg = STRING_SPRINTF("[section_start=%d] [section_end=%d] error, parameter1[%d] < %d or >= %d?", section_start, section_end,
						node_cfg.param1, ItemBase::I_COLOR_INVALID, ItemBase::I_COLOR_MAX);
					return -12;
				}
			}
			break;
			case KUA_FU_YING_XIONG_LU_TASK_TYPE_XUN_BAO:
			{
				if (node_cfg.param1 < XUN_BAO_TREASURE_TYPE_INVALID || node_cfg.param1 >= XUN_BAO_TREASURE_TYPE_MAX)
				{
					errormsg = STRING_SPRINTF("[section_start=%d] [section_end=%d] error, parameter1[%d] < %d or >= %d?", section_start, section_end,
						node_cfg.param1, XUN_BAO_TREASURE_TYPE_INVALID, XUN_BAO_TREASURE_TYPE_MAX);
					return -13;
				}
			}
			break;
			case KUA_FU_YING_XIONG_LU_TASK_TYPE_HAS_PET:
			{
				if (node_cfg.param1 < PET_QUALITY_INVALID || node_cfg.param1 >= PET_QUALITY_MAX)
				{
					errormsg = STRING_SPRINTF("[section_start=%d] [section_end=%d] error, parameter1[%d] < %d or >= %d?", section_start, section_end,
						node_cfg.param1, PET_QUALITY_INVALID, PET_QUALITY_MAX);
					return -14;
				}
			}
			break;
			case KUA_FU_YING_XIONG_LU_TASK_TYPE_LEARN_PET_SKILL:
			{
				if (node_cfg.param1 < GAME_PASSIVE_SKILL_PRIORITY_TYPE_INVALID || node_cfg.param1 >= GAME_PASSIVE_SKILL_PRIORITY_TYPE_MAX)
				{
					errormsg = STRING_SPRINTF("[section_start=%d] [section_end=%d] error, parameter1[%d] < %d or >= %d?", section_start, section_end,
						node_cfg.param1, GAME_PASSIVE_SKILL_PRIORITY_TYPE_INVALID, GAME_PASSIVE_SKILL_PRIORITY_TYPE_MAX);
					return -15;
				}
			}
			break;
			case KUA_FU_YING_XIONG_LU_TASK_TYPE_PARTNER_STR:
			{
				if (node_cfg.param1 < PQT_MIN || node_cfg.param1 >= PQT_MAX)
				{
					errormsg = STRING_SPRINTF("[section_start=%d] [section_end=%d] error, parameter1[%d] < %d or >= %d?", section_start, section_end,
						node_cfg.param1, PQT_MIN, PQT_MAX);
					return -16;
				}
			}
			break;
			default:
				return -222;
		}

		TiXmlElement * list_node = data_element->FirstChildElement("reward1_item_list");
		if (NULL == list_node)
		{
			return -16;
		}

		TiXmlElement * item_node = list_node->FirstChildElement("reward1_item");
		while (NULL != item_node)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_node)) return -100;

			node_cfg.reward_list.push_back(item);
			item_node = item_node->NextSiblingElement();
		}

		cfg_list.push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

long long KuaFuYingXiongLuConfig::GetSectionKey(int section_start, int section_end)
{
	KuaFuYingXiongLuKey info_key;
	info_key.param[0] = section_start;
	info_key.param[1] = section_end;

	return info_key.unique_key;
}

void KuaFuYingXiongLuConfig::GetInfoByKey(long long _info_key, int * info_param1, int * info_param2) const
{
	if (NULL == info_param1 || NULL == info_param2) return;

	KuaFuYingXiongLuKey info_key;
	info_key.unique_key = _info_key;
	*info_param1 = info_key.param[0];
	*info_param2 = info_key.param[1];
}
