#include "randactivityjiandanqinxinconfig.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"

RandActivityJianDanQinXinConfig::RandActivityJianDanQinXinConfig()
{

}

RandActivityJianDanQinXinConfig::~RandActivityJianDanQinXinConfig()
{
	std::vector<RAJianDanQinXinSectionCfg>().swap(m_section_cfg);
}

bool RandActivityJianDanQinXinConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("copy_copyreward", InitRewardCfg);
	LOAD_RA_CONFIG("organize", InitOtherCfg);

	return true;
}

const RAJianDanQinXinRewardCfg * RandActivityJianDanQinXinConfig::GetLayerCfgByMonsterId(RandActivityManager * ramgr, int monster_group_id) const
{
	if (monster_group_id < 0) return NULL;

	int act_real_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN);
	for (int i = 0; i < (int)m_section_cfg.size(); i++)
	{
		if (m_section_cfg[i].section_start <= act_real_open_day && m_section_cfg[i].section_end >= act_real_open_day)
		{
			for (int idx = 0; idx < (int)m_section_cfg[i].layer_cfg.size(); ++idx)
			{
				if (m_section_cfg[i].layer_cfg[idx].monster_group_id == monster_group_id)
				{
					return &m_section_cfg[i].layer_cfg[idx];
				}
			}
		}
	}

	return NULL;
}

int RandActivityJianDanQinXinConfig::InitOtherCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	//if (!GetSubNodeValue(data_element, "no_less_than", m_other_cfg.need_member_count) || m_other_cfg.need_member_count <= 0 || m_other_cfg.need_member_count > MAX_TEAM_MEMBER_NUM) return -1;

	return 0;
}

int RandActivityJianDanQinXinConfig::InitRewardCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	while (NULL != data_element)
	{
		bool has_section_cfg = true;

		int section_start = 0;
		if (!GetSubNodeValue(data_element, "section_start", section_start))
		{
			has_section_cfg = false;
		}

		int section_end = 0;
		if (!GetSubNodeValue(data_element, "section_end", section_end))
		{
			has_section_cfg = false;
		}

		if (!has_section_cfg) // 没配置，默认给一个无限区间
		{
			section_start = 1;
			section_end = INT_MAX;
		}

		if (0 == section_end) // 0 代表无穷
		{
			section_end = INT_MAX;
		}

		if (m_section_cfg.size() > 0)
		{
			int pre_index = m_section_cfg.size() - 1;
			if (section_start != m_section_cfg[pre_index].section_start ||
				section_end != m_section_cfg[pre_index].section_end)
			{
				if (section_start < m_section_cfg[pre_index].section_end)
				{
					return -888;
				}

				m_section_cfg.push_back(RAJianDanQinXinSectionCfg());
			}
		}
		if (m_section_cfg.size() == 0)
		{
			if (section_start != 1)
			{
				return -11111;
			}
			m_section_cfg.push_back(RAJianDanQinXinSectionCfg());
		}

		RAJianDanQinXinSectionCfg & node_cfg = m_section_cfg[m_section_cfg.size() - 1];
		node_cfg.section_start = section_start;
		node_cfg.section_end = section_end;

		RAJianDanQinXinRewardCfg node;
		if (!GetSubNodeValue(data_element, "seq1", node.seq) || node.seq < 0) return -1;
		if (!GetSubNodeValue(data_element, "checkpoint_layer", node.checkpoint_layer) || node.checkpoint_layer <= 0 || node.checkpoint_layer > MAX_RA_JIANDNAQINXIN_DAY_NUM) return -2;
		if (!GetSubNodeValue(data_element, "type", node.type) || node.type < 0 || node.type >= JIAN_DAN_QIN_XIN_TYPE_MAX) return -3;
		if (!GetSubNodeValue(data_element, "monster_group_id", node.monster_group_id) || node.monster_group_id < 0) return -4;

		TiXmlElement *list_element = data_element->FirstChildElement("reward_item_list");
		if (NULL != list_element)
		{
			TiXmlElement *item_element = list_element->FirstChildElement("reward_item");
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element)) return -100;

				if (node.reward_count >= MAX_RA_JIANDANQINXIN_REWARD_NUM || node.reward_count < 0) return -111;

				node.item_reward_list[node.reward_count++] = item;
				item_element = item_element->NextSiblingElement();
			}
		}

		node_cfg.layer_cfg.push_back(node);
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
