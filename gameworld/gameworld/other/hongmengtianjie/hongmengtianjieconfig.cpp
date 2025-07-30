#include "hongmengtianjieconfig.hpp"

#include "npc/npcpool.h"
#include "item/itempool.h"
#include "monster/monster_group_pool.hpp"

HongMengTianJieConfig::HongMengTianJieConfig()
{
}

HongMengTianJieConfig::~HongMengTianJieConfig()
{
	for (std::map<int, std::vector<HongMengTianJieFbCfg> >::iterator it = m_fb_cfg.begin(); it != m_fb_cfg.end(); it++)
	{
		std::vector<HongMengTianJieFbCfg>().swap(it->second);
	}
	std::vector<HongMengTianJieNpcCfg>().swap(m_npc_cfg);
	std::vector<HongMengTianJieRewardCfg>().swap(m_reward_cfg);
	std::vector<HongMengTianJieStoreCfg>().swap(m_store_cfg);
}

bool HongMengTianJieConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("other", InitOtherCfg);
	LOAD_CONFIG_2("reward_group", InitGroupCfg);
	LOAD_CONFIG_2("boss_fb", InitFbCfg);
	LOAD_CONFIG_2("task_npc", InitNpcCfg);
	LOAD_CONFIG_2("fb_group", InitRewardCfg);
	LOAD_CONFIG_2("exchange_store", InitStoreCfg);

	return true;
}

const HongMengTianJieFbCfg * HongMengTianJieConfig::GetFBCfg(int fb_type, int fb_seq)
{
	std::map<int, std::vector<HongMengTianJieFbCfg> >::iterator it = m_fb_cfg.find(fb_type);
	if (it == m_fb_cfg.end())
		return NULL;

	if (fb_seq < 0 || fb_seq >= (int)it->second.size())
		return NULL;

	return &it->second[fb_seq];
}

const HongMengTianJieNpcCfg * HongMengTianJieConfig::GetNpcCfg(int fb_id)
{
	if (fb_id < 0 || fb_id >= (int)m_npc_cfg.size())
		return NULL;
	return &m_npc_cfg[fb_id];
}

const HongMengTianJieRewardCfg * HongMengTianJieConfig::GetRewardCfg(int fb_id)
{
	if (fb_id < 0 || fb_id >= (int)m_reward_cfg.size())
		return NULL;
	return &m_reward_cfg[fb_id];
}

const HongMengTianJieItemCfg * HongMengTianJieConfig::GetGroupReward(int group_id)
{
	std::map<int, HongMengTianJieGroupCfg>::iterator group_it = m_group_cfg.find(group_id);
	if (group_it == m_group_cfg.end())
		return NULL;

//	int rate = RandomNum(group_it->second.max_rate);
	// 改成固定概率值10000，允许出现 抽不到该奖励组抽不到任何道具的情况
	int rate = RandomNum(10000);
	std::map<int, HongMengTianJieItemCfg>::iterator it = group_it->second.reward_list.upper_bound(rate);
	if (it == group_it->second.reward_list.end())
		return NULL;

	return &it->second;
}

const HongMengTianJieStoreCfg * HongMengTianJieConfig::GetStoreCfg(int seq)
{
	if (seq < 0 || seq >= (int)m_store_cfg.size())
		return NULL;

	return &m_store_cfg[seq];
}

int HongMengTianJieConfig::InitOtherCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "start_level", m_other_cfg.start_level) || m_other_cfg.start_level < 0)
	{
		errormsg = STRING_SPRINTF("[start_level=%d] error, is start_level < 0?", m_other_cfg.start_level);
		return -__LINE__;
	}
	if (!GetSubNodeValue(root_element, "level_limit", m_other_cfg.level_limit) || m_other_cfg.level_limit <= 0)
	{
		errormsg = STRING_SPRINTF("[level_limit=%d] error, is level_limit <= 0?", m_other_cfg.level_limit);
		return -__LINE__;
	}

	{
		TiXmlElement * list_element = root_element->FirstChildElement("boss_reward_repeat");
		if (NULL == list_element)
		{
			errormsg = STRING_SPRINTF("[boss_reward_repeat] error, is boss_reward_repeat == NULL?");
			return -__LINE__;
		}
		TiXmlElement * node_element = list_element->FirstChildElement("node");
		while (node_element != NULL)
		{
			int boss_reward_repeat = 0;
			if (!GetNodeValue(node_element, boss_reward_repeat))
			{
				return -__LINE__;
			}

			m_other_cfg.boss_reward_repeat.push_back(boss_reward_repeat);
			node_element = node_element->NextSiblingElement();
		}
	}
	{
		TiXmlElement * list_element = root_element->FirstChildElement("help_reward_repeat");
		if (NULL == list_element)
		{
			errormsg = STRING_SPRINTF("[help_reward_repeat] error, is help_reward_repeat == NULL?");
			return -__LINE__;
		}
		TiXmlElement * node_element = list_element->FirstChildElement("node");
		while (node_element != NULL)
		{
			int help_reward_repeat = 0;
			if (!GetNodeValue(node_element, help_reward_repeat))
			{
				return -__LINE__;
			}

			m_other_cfg.help_reward_repeat.push_back(help_reward_repeat);
			node_element = node_element->NextSiblingElement();
		}
	}
	{
		TiXmlElement * list_element = root_element->FirstChildElement("reward_times");
		if (NULL == list_element)
		{
			errormsg = STRING_SPRINTF("[reward_times] error, is reward_times == NULL?");
			return -__LINE__;
		}
		TiXmlElement * node_element = list_element->FirstChildElement("node");
		int count = 0;
		while (node_element != NULL)
		{
			if (count >= HongMengTianJie::MAX_HONGMENG_TIANJIE_BOSS_NUM)
			{
				errormsg = STRING_SPRINTF("[count=%d] error, is count >= MAX_HONGMENG_TIANJIE_BOSS_NUM(%d)?", count, HongMengTianJie::MAX_HONGMENG_TIANJIE_BOSS_NUM);
				return -__LINE__;
			}
			int reward_times = 0;
			if (!GetNodeValue(node_element, reward_times))
			{
				return -__LINE__;
			}

			m_other_cfg.reward_times[count] = reward_times;
			count++;
			node_element = node_element->NextSiblingElement();
		}
	}

	if (!GetSubNodeValue(root_element, "help_times", m_other_cfg.help_times) || m_other_cfg.help_times < 0)
	{
		errormsg = STRING_SPRINTF("[help_times=%d] error, is help_times < 0?", m_other_cfg.help_times);
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "boss_reward_limit", m_other_cfg.boss_reward_limit) || m_other_cfg.boss_reward_limit <= 0)
	{
		errormsg = STRING_SPRINTF("[boss_reward_limit=%d] error, is boss_reward_limit <= 0?", m_other_cfg.boss_reward_limit);
		return -__LINE__;
	}

	if (!GetSubNodeValue(root_element, "help_reward_limit", m_other_cfg.help_reward_limit) || m_other_cfg.help_reward_limit <= 0)
	{
		errormsg = STRING_SPRINTF("[help_reward_limit=%d] error, is help_reward_limit <= 0?", m_other_cfg.help_reward_limit);
		return -__LINE__;
	}

	return 0;
}

int HongMengTianJieConfig::InitFbCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int fb_type = 0;
		if (!GetSubNodeValue(root_element, "fb_type", fb_type) || (fb_type != 1 && fb_type != 2))
		{
			errormsg = STRING_SPRINTF("[fb_type=%d] error, is fb_type != 1 && fb_type != 2?", fb_type);
			return -__LINE__;
		}

		HongMengTianJieFbCfg cfg;
		if (!GetSubNodeValue(root_element, "fb_id", cfg.fb_id) || cfg.fb_id < 0 || cfg.fb_id >= HongMengTianJie::MAX_FB_INDEX_NUM)
		{
			errormsg = STRING_SPRINTF("[fb_id=%d] error, is fb_id > MAX_FB_INDEX_NUM(%d)?", cfg.fb_id, HongMengTianJie::MAX_FB_INDEX_NUM);
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "equip_require", cfg.equipment_requirement) || cfg.equipment_requirement < 0)
		{
			errormsg = STRING_SPRINTF("[equipment_requirement=%d] error, is equipment_requirement < 0?", cfg.equipment_requirement);
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "scene_id", cfg.scene_id) || cfg.scene_id <= 0)
		{
			errormsg = STRING_SPRINTF("[scene_id=%d] error, is scene_id <= 0?", cfg.scene_id);
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "door_x", cfg.door_pos.x) || cfg.door_pos.x <= 0)
		{
			errormsg = STRING_SPRINTF("[pos_x=%d] error, is pos_x <= 0?", cfg.door_pos.x);
			return -__LINE__;
		}
		if (!GetSubNodeValue(root_element, "door_y", cfg.door_pos.y) || cfg.door_pos.y <= 0)
		{
			errormsg = STRING_SPRINTF("[pos_y=%d] error, is pos_y <= 0?", cfg.door_pos.y);
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "first_difficult", cfg.first_difficult) || cfg.first_difficult <= 0)
		{
			errormsg = STRING_SPRINTF("[first_difficult=%d] error, is first_difficult <= 0?", cfg.first_difficult);
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "down_percent", cfg.down_percent) || cfg.down_percent < 0)
		{
			errormsg = STRING_SPRINTF("[down_percent=%d] error, is down_percent < 0?", cfg.down_percent);
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "min_difficult", cfg.min_difficult) || cfg.min_difficult <= 0 || cfg.min_difficult > cfg.first_difficult)
		{
			errormsg = STRING_SPRINTF("[min_difficult=%d] error, is min_difficult <= 0 || min_difficult > first_difficult(%d)?", cfg.min_difficult, cfg.first_difficult);
			return -__LINE__;
		}

		m_fb_cfg[fb_type].push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}
	
	return 0;
}

int HongMengTianJieConfig::InitNpcCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		HongMengTianJieNpcCfg cfg;

		for (int i = 0; i < HongMengTianJie::MAX_HONGMENG_TIANJIE_BOSS_NUM; i++)
		{
			std::string npc_str = STRING_SPRINTF("npc_id_%d", i + 1);

			if (!GetSubNodeValue(root_element, npc_str, cfg.npc_param[i].npc_id) || NULL == NPCPOOL->GetServerNPCInfo(cfg.npc_param[i].npc_id))
			{
				errormsg = STRING_SPRINTF("[npc_id_%d=%d] error, is %d == NULL?", i + 1, cfg.npc_param[i].npc_id, cfg.npc_param[i].npc_id);
				return -__LINE__;
			}

			std::string pos_x_str = STRING_SPRINTF("npc_x_%d", i + 1);
			if (!GetSubNodeValue(root_element, pos_x_str, cfg.npc_param[i].pos.x) || cfg.npc_param[i].pos.x <= 0)
			{
				errormsg = STRING_SPRINTF("[npc_x_%d=%d] error, is pos.x <= 0?", i + 1, cfg.npc_param[i].pos.x);
				return -__LINE__;
			}

			std::string pos_y_str = STRING_SPRINTF("npc_y_%d", i + 1);
			if (!GetSubNodeValue(root_element, pos_y_str, cfg.npc_param[i].pos.y) || cfg.npc_param[i].pos.y <= 0)
			{
				errormsg = STRING_SPRINTF("[npc_y_%d=%d] error, is pos.y <= 0?", i + 1, cfg.npc_param[i].pos.y);
				return -__LINE__;
			}

			std::string monster_group_str = STRING_SPRINTF("monster_group_%d", i + 1);

			if (!GetSubNodeValue(root_element, monster_group_str, cfg.npc_param[i].monster_group) || 
				NULL == MonsterGroupPool::GetInstance().GetMonsterGroupCfg(cfg.npc_param[i].monster_group))
			{
				errormsg = STRING_SPRINTF("[monster_group%d=%d] error, is %d == NULL?", i + 1, cfg.npc_param[i].monster_group, cfg.npc_param[i].monster_group);
				return -__LINE__;
			}
		}	

		m_npc_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int HongMengTianJieConfig::InitRewardCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		HongMengTianJieRewardCfg cfg;

		{
			TiXmlElement * list_element = root_element->FirstChildElement("person_reward_list");
			if (NULL == list_element)
			{
				errormsg = STRING_SPRINTF("[person_reward_list] error, is person_reward_list == NULL?");
				return -__LINE__;
			}

			TiXmlElement * item_element = list_element->FirstChildElement("person_reward");
			while (NULL != item_element)
			{
				ItemConfigData item;	
				if (!item.ReadConfig(item_element))
					return -__LINE__;

				cfg.person_reward.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}

		{
			TiXmlElement * list_element = root_element->FirstChildElement("world_reward_list");
			if (NULL == list_element)
			{
				errormsg = STRING_SPRINTF("[world_reward_list] error, is world_reward_list == NULL?");
				return -__LINE__;
			}

			TiXmlElement * item_element = list_element->FirstChildElement("world_reward");
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
					return -__LINE__;

				cfg.world_reward.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}

		{
			for (int i = 0; i < HongMengTianJie::MAX_HONGMENG_TIANJIE_BOSS_NUM; i++)
			{
				std::string reward_str = STRING_SPRINTF("success_reward_%d", i + 1);

				TiXmlElement * list_element = root_element->FirstChildElement(reward_str.c_str());
				if (NULL == list_element)
				{
					errormsg = STRING_SPRINTF("[success_reward_%d] error, is success_reward_%d == NULL?", i + 1, i + 1);
					return -__LINE__;
				}
				TiXmlElement * node_element = list_element->FirstChildElement("node");
				while (node_element != NULL)
				{
					int reward_group_id = 0;
					if (!GetNodeValue(node_element, reward_group_id) || m_group_cfg.end() == m_group_cfg.find(reward_group_id))
					{
						errormsg = STRING_SPRINTF("[reward_group_id=%d] error, is success_reward_%d?", reward_group_id, i + 1);
						return -__LINE__;
					}

					cfg.success_reward[i].push_back(reward_group_id);
					node_element = node_element->NextSiblingElement();
				}
			}
		}

		{
			TiXmlElement * list_element = root_element->FirstChildElement("help_reward_list");
			if (NULL == list_element)
			{
				errormsg = STRING_SPRINTF("[help_reward_list] error, is help_reward_list == NULL?");
				return -__LINE__;
			}

			TiXmlElement * item_element = list_element->FirstChildElement("help_reward");
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
					return -__LINE__;

				cfg.help_reward.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}

		m_reward_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int HongMengTianJieConfig::InitGroupCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int fb_index = 0;
		if (!GetSubNodeValue(root_element, "fb_index", fb_index) || fb_index <= 0)
		{
			errormsg = STRING_SPRINTF("[fb_index=%d] error, is fb_index <= 0?", fb_index);
			return -__LINE__;
		}

		HongMengTianJieGroupCfg &map = m_group_cfg[fb_index];

		HongMengTianJieItemCfg cfg;
		TiXmlElement * item_element = root_element->FirstChildElement("item");
		if (NULL == item_element)
		{
			errormsg = STRING_SPRINTF("[item] error, is item == NULL?");
			return -__LINE__;
		}

		if (!cfg.item.ReadConfig(item_element))
			return -__LINE__;

		int rate = 0;
		if (!GetSubNodeValue(root_element, "rate", rate) || rate <= 0)
		{
			errormsg = STRING_SPRINTF("[rate=%d] error, is rate <= 0?", rate);
			return -__LINE__;
		}

		int is_broadcast = 0;
		if (!GetSubNodeValue(root_element, "is_broadcast", is_broadcast))
		{
			return -__LINE__;
		}

		cfg.is_broadcast = 0 != is_broadcast;

		map.max_rate += rate;
		map.reward_list[map.max_rate] = cfg;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int HongMengTianJieConfig::InitStoreCfg(TiXmlElement * RootElement, std::string & errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		HongMengTianJieStoreCfg cfg;
		if (!GetSubNodeValue(root_element, "item_id", cfg.item_id) || NULL == ITEMPOOL->GetItem(cfg.item_id))
		{
			errormsg = STRING_SPRINTF("[item_id=%d] error, is item_id == NULL?", cfg.item_id);
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "is_bind", cfg.is_bind))
			return -__LINE__;

		if (!GetSubNodeValue(root_element, "num", cfg.num) || cfg.num <= 0)
		{
			errormsg = STRING_SPRINTF("[num=%d] error, is num <= 0?", cfg.num);
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "price", cfg.price) || cfg.price <= 0)
		{
			errormsg = STRING_SPRINTF("[price=%d] error, is price <= 0?", cfg.price);
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "conver_item_id", cfg.conver_item_id) || NULL == ITEMPOOL->GetItem(cfg.conver_item_id))
		{
			errormsg = STRING_SPRINTF("[conver_item_id=%d] error, is conver_item_id == NULL?", cfg.conver_item_id);
			return -__LINE__;
		}
		if (!GetSubNodeValue(root_element, "limit_seq", cfg.limit_seq) || cfg.limit_seq >= HongMengTianJie::MAX_LIMIT_SEQ_NUM)
		{
			errormsg = STRING_SPRINTF("[limit_seq=%d] error, is limit_seq >= HongMengTianJie::MAX_LIMIT_SEQ_NUM(%d)?", cfg.limit_seq, HongMengTianJie::MAX_LIMIT_SEQ_NUM);
			return -__LINE__;
		}
		if (!GetSubNodeValue(root_element, "limit_convert_count", cfg.limit_convert_count) || 
			(cfg.limit_seq >= 0 && (cfg.limit_convert_count <= 0 || cfg.limit_convert_count > HongMengTianJie::MAX_LIMIT_ITEM_NUM)))
		{
			errormsg = STRING_SPRINTF("[limit_convert_count=%d] error, is limit_convert_count <= 0 || limit_convert_count > MAX_LIMIT_ITEM_NUM(%d)?", cfg.limit_convert_count, HongMengTianJie::MAX_LIMIT_ITEM_NUM);
			return -__LINE__;
		}

		m_store_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
