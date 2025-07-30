#include "spidconfig.h"
#include "config/sharedconfig/sharedconfig.h"
#include "servercommon/chatserverdef.h"
#include "gameworld/engineadapter.h"
#include "servercommon/maildef.hpp"
#include "world.h"


globalconfig::SpidConfig::SpidConfig() :  new_server_open_days(0), old_server_open_days(0), 
	talk_level(0), talk_level_xianmeng(0), talk_level_siliao(0), 
	m_is_open_chat_auto_forbid(-1), m_dev_spid(0)
{
	PlatName dev_pname;
	F_STRNCPY(dev_pname, "dev", sizeof(dev_pname));
	m_dev_spid = GetSpidByPlatName(dev_pname);
}

globalconfig::SpidConfig::~SpidConfig()
{
}

bool globalconfig::SpidConfig::Init(const std::string& configname, std::string* err)
{
	int iRet = 0;
	char err_info[1024] = { 0 };

	TiXmlDocument document;
	if (configname == "" || !document.LoadFile(configname))
	{
		*err = configname + " Load fail: " + document.ErrorDesc();
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();
	if (NULL == RootElement)
	{
		*err = configname + ": xml node error in root";
		return false;
	}

	TiXmlElement *NodeElement = RootElement->FirstChildElement("agent_adapt");
	if (NULL == NodeElement)
	{
		*err = configname + ": xml node error in root->agent_adapt";
		return false;
	}

	iRet = this->InitSpidConfig(NodeElement);
	if (0 != iRet)
	{
		SNPRINTF(err_info, sizeof(err_info), "%s:InitSpidConfig fail %d\n", configname.c_str(), iRet);
		*err = err_info;
		return false;
	}

	TiXmlElement *talk_level_element = RootElement->FirstChildElement("talk_level");
	if (NULL == NodeElement)
	{
		*err = configname + ": xml node error in root->talk_level";
		return false;
	}

	iRet = this->InitCharLimitByOpenServer(talk_level_element);
	if (0 != iRet)
	{
		SNPRINTF(err_info, sizeof(err_info), "%s:InitCharLimitByOpenServer fail %d\n", configname.c_str(), iRet);
		*err = err_info;
		return false;
	}

	return true;
}

int globalconfig::SpidConfig::GetTalkLevel()
{
	int spid = LocalConfig::Instance().GetPlatType();
	new_server_open_days = this->GetServerOpenDays();
	if (new_server_open_days != old_server_open_days)
	{
		old_server_open_days = new_server_open_days;
		for (std::map<int, ChatLimitCfg>::const_iterator it = m_chat_limit_section_cfg.begin(); it != m_chat_limit_section_cfg.end(); ++it)
		{
			if (it->second.spid == spid)
			{
				if (old_server_open_days >= it->second.day_start &&
					old_server_open_days <= it->second.day_end)
				{
					talk_level = it->second.talk_level;
					talk_level_xianmeng = it->second.talk_level_xianmeng;
					talk_level_siliao = it->second.talk_level_siliao;
					break;
				}
			}
			talk_level = 0;
			talk_level_xianmeng = 0;
			talk_level_siliao = 0;
		}
	}

	if (talk_level > 0)
	{
		return talk_level;
	}

	SpidConfigItemMap::iterator it = m_spid_config_item_map.find(spid);
	if (it != m_spid_config_item_map.end())
	{
		return it->second.talk_level;
	}
	else
	{
		it = m_spid_config_item_map.find(m_dev_spid);
		if (it != m_spid_config_item_map.end())
		{
			return it->second.talk_level;
		}
	}

	return 20;
}

int globalconfig::SpidConfig::GetTalkLevelXianMeng()
{
	int spid = LocalConfig::Instance().GetPlatType();
	new_server_open_days = this->GetServerOpenDays();
	if (new_server_open_days != old_server_open_days)
	{
		old_server_open_days = new_server_open_days;
		for (std::map<int, ChatLimitCfg>::const_iterator it = m_chat_limit_section_cfg.begin(); it != m_chat_limit_section_cfg.end(); ++it)
		{
			if (it->second.spid == spid)
			{
				if (old_server_open_days >= it->second.day_start &&
					old_server_open_days <= it->second.day_end)
				{
					talk_level = it->second.talk_level;
					talk_level_xianmeng = it->second.talk_level_xianmeng;
					talk_level_siliao = it->second.talk_level_siliao;
					break;
				}
			}
			talk_level = 0;
			talk_level_xianmeng = 0;
			talk_level_siliao = 0;
		}
	}
	if (talk_level_xianmeng > 0)
	{
		return talk_level_xianmeng;
	}

	SpidConfigItemMap::iterator it = m_spid_config_item_map.find(spid);
	if (it != m_spid_config_item_map.end())
	{
		return it->second.talk_level_xianmeng;
	}
	else
	{
		it = m_spid_config_item_map.find(m_dev_spid);
		if (it != m_spid_config_item_map.end())
		{
			return it->second.talk_level;
		}
	}

	return 35;
}

int globalconfig::SpidConfig::GetTalkLevelSiLiao()
{
	int spid = LocalConfig::Instance().GetPlatType();
	new_server_open_days = this->GetServerOpenDays();
	if (new_server_open_days != old_server_open_days)
	{
		old_server_open_days = new_server_open_days;
		for (std::map<int, ChatLimitCfg>::const_iterator it = m_chat_limit_section_cfg.begin(); it != m_chat_limit_section_cfg.end(); ++it)
		{
			if (it->second.spid == spid)
			{
				if (old_server_open_days >= it->second.day_start &&
					old_server_open_days <= it->second.day_end)
				{
					talk_level = it->second.talk_level;
					talk_level_xianmeng = it->second.talk_level_xianmeng;
					talk_level_siliao = it->second.talk_level_siliao;
					break;
				}
			}
			talk_level = 0;
			talk_level_xianmeng = 0;
			talk_level_siliao = 0;
		}
	}
	if (talk_level_siliao > 0)
	{
		return talk_level_siliao;
	}

	SpidConfigItemMap::iterator it = m_spid_config_item_map.find(spid);
	if (it != m_spid_config_item_map.end())
	{
		return it->second.talk_level_siliao;
	}
	else
	{
		it = m_spid_config_item_map.find(m_dev_spid);
		if (it != m_spid_config_item_map.end())
		{
			return it->second.talk_level;
		}
	}

	return 35;
}

int globalconfig::SpidConfig::GetTalkLevelCross()
{
	int spid = LocalConfig::Instance().GetPlatType();

	SpidConfigItemMap::iterator it = m_spid_config_item_map.find(spid);
	if (it != m_spid_config_item_map.end())
	{
		return it->second.talk_level_cross;
	}
	else
	{
		it = m_spid_config_item_map.find(m_dev_spid);
		if (it != m_spid_config_item_map.end())
		{
			return it->second.talk_level;
		}
	}

	return 35;
}

int globalconfig::SpidConfig::GetSpeakerLevel()
{
	int spid = LocalConfig::Instance().GetPlatType();

	SpidConfigItemMap::iterator it = m_spid_config_item_map.find(spid);
	if (it != m_spid_config_item_map.end())
	{
		return it->second.speaker_level;
	}
	else
	{
		it = m_spid_config_item_map.find(m_dev_spid);
		if (it != m_spid_config_item_map.end())
		{
			return it->second.talk_level;
		}
	}

	return 35;
}

int globalconfig::SpidConfig::GetCrossSpeakerLevel()
{
	int spid = LocalConfig::Instance().GetPlatType();

	SpidConfigItemMap::iterator it = m_spid_config_item_map.find(spid);
	if (it != m_spid_config_item_map.end())
	{
		return it->second.cross_speaker_level;
	}
	else
	{
		it = m_spid_config_item_map.find(m_dev_spid);
		if (it != m_spid_config_item_map.end())
		{
			return it->second.cross_speaker_level;
		}
	}

	return 35;
}


int globalconfig::SpidConfig::GetCreateChatGroupLevel()
{
	int spid = LocalConfig::Instance().GetPlatType();

	SpidConfigItemMap::iterator it = m_spid_config_item_map.find(spid);
	if (it != m_spid_config_item_map.end())
	{
		return it->second.create_group_level;
	}
	else
	{
		it = m_spid_config_item_map.find(m_dev_spid);
		if (it != m_spid_config_item_map.end())
		{
			return it->second.create_group_level;
		}
	}

	return 35;
}

int globalconfig::SpidConfig::GetSingleChatNumLevel()
{
	int spid = LocalConfig::Instance().GetPlatType();

	SpidConfigItemMap::iterator it = m_spid_config_item_map.find(spid);
	if (it != m_spid_config_item_map.end())
	{
		return it->second.single_talk_num_lv;
	}
	else
	{
		it = m_spid_config_item_map.find(m_dev_spid);
		if (it != m_spid_config_item_map.end())
		{
			return it->second.single_talk_num_lv;
		}
	}

	return 35;
}

int globalconfig::SpidConfig::GetSingleChatNumTime()
{
	int spid = LocalConfig::Instance().GetPlatType();

	SpidConfigItemMap::iterator it = m_spid_config_item_map.find(spid);
	if (it != m_spid_config_item_map.end())
	{
		return it->second.single_talk_num_time;
	}
	else
	{
		it = m_spid_config_item_map.find(m_dev_spid);
		if (it != m_spid_config_item_map.end())
		{
			return it->second.single_talk_num_time;
		}
	}

	return 6;
}

int globalconfig::SpidConfig::GetSingleChatNumNum()
{
	int spid = LocalConfig::Instance().GetPlatType();

	SpidConfigItemMap::iterator it = m_spid_config_item_map.find(spid);
	if (it != m_spid_config_item_map.end())
	{
		return it->second.single_talk_num_num;
	}
	else
	{
		it = m_spid_config_item_map.find(m_dev_spid);
		if (it != m_spid_config_item_map.end())
		{
			return it->second.single_talk_num_num;
		}
	}

	return 2;
}

int globalconfig::SpidConfig::GetBuyLimit()
{
	int spid = LocalConfig::Instance().GetPlatType();

	SpidConfigItemMap::iterator it = m_spid_config_item_map.find(spid);
	if (it != m_spid_config_item_map.end())
	{
		return it->second.day_trading_up;
	}
	else
	{
		it = m_spid_config_item_map.find(m_dev_spid);
		if (it != m_spid_config_item_map.end())
		{
			return it->second.day_trading_up;
		}
	}

	return 0;
}

int globalconfig::SpidConfig::GetForbidTalkTipsLevel()
{
	int spid = LocalConfig::Instance().GetPlatType();

	SpidConfigItemMap::iterator it = m_spid_config_item_map.find(spid);
	if (it != m_spid_config_item_map.end())
	{
		return it->second.forbidden_tips_level;
	}
	else
	{
		it = m_spid_config_item_map.find(m_dev_spid);
		if (it != m_spid_config_item_map.end())
		{
			return it->second.forbidden_tips_level;
		}
	}

	return 0;
}

bool globalconfig::SpidConfig::IsCanSell()
{
	int spid = LocalConfig::Instance().GetPlatType();

	SpidConfigItemMap::iterator it = m_spid_config_item_map.find(spid);
	if (it != m_spid_config_item_map.end())
	{
		return it->second.is_sell == 1 ? true : false;
	}
	else
	{
		it = m_spid_config_item_map.find(m_dev_spid);
		if (it != m_spid_config_item_map.end())
		{
			return it->second.is_sell == 1 ? true : false;
		}
	}

	return false;
}

const std::vector<ItemConfigData>* globalconfig::SpidConfig::GetRealNameReward()
{
	int spid = LocalConfig::Instance().GetPlatType();

	SpidConfigItemMap::iterator it = m_spid_config_item_map.find(spid);
	if (it != m_spid_config_item_map.end())
	{
		if (!it->second.real_name_rewards.empty())
			return &it->second.real_name_rewards;
	}
	else
	{
		it = m_spid_config_item_map.find(m_dev_spid);
		if (it != m_spid_config_item_map.end())
		{
			if (!it->second.real_name_rewards.empty())
				return &it->second.real_name_rewards;
		}
	}

	return NULL;
}

int globalconfig::SpidConfig::GetSpidByPlatName(PlatName plat_name)
{
	PlatTypeUnion tmp_union;
	tmp_union.spid[0] = tmp_union.spid[1] = tmp_union.spid[2] = tmp_union.spid[3] = 0;
	UNSTD_STATIC_CHECK(sizeof(PlatName) >= 4);
	for (unsigned int i = 0; i < 3; i++)
	{
		if ('\0' != plat_name[i])
		{
			tmp_union.spid[i + 1] = plat_name[i];	// 这样做是为了确保id大于0 字节序的关系
		}
	}

	return tmp_union.id;
}

int globalconfig::SpidConfig::GetSpidByStrPlatName(const std::string& str_plat_name)
{
	PlatTypeUnion tmp_union;
	tmp_union.spid[0] = tmp_union.spid[1] = tmp_union.spid[2] = tmp_union.spid[3] = 0;
	for (unsigned int i = 0; i < 3; i++)
	{
		if (i < str_plat_name.length() && '\0' != str_plat_name[i])
		{
			tmp_union.spid[i + 1] = str_plat_name[i];	// 这样做是为了确保id大于0 字节序的关系
		}
	}

	return tmp_union.id;
}


bool globalconfig::SpidConfig::GetPlatNameBySpid(int spid, ARG_OUT char platname[sizeof(int)])
{
	PlatTypeUnion tmp_union;
	tmp_union.id = spid;

	for (int i = 0; i < 3; ++i)
	{
		platname[i] = tmp_union.spid[i + 1];
	}
	platname[sizeof(int) - 1] = '\0';

	return true;
}

int globalconfig::SpidConfig::InitSpidConfig(TiXmlElement *RootElement)
{
	if (NULL == RootElement)
	{
		return -10000;
	}

	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -20000;
	}

	while (NULL != data_element)
	{
		std::string spid_name;
		if (!GetSubNodeValue(data_element, "spid", spid_name))
		{
			return -1;
		}

		int talk_level = 0;
		if (!GetSubNodeValue(data_element, "talk_level", talk_level))
		{
			return -2;
		}
		int talk_level_xianmeng = 0;
		if (!GetSubNodeValue(data_element, "talk_level_xianmeng", talk_level_xianmeng))
		{
			return -3;
		}
		int talk_level_siliao = 0;
		if (!GetSubNodeValue(data_element, "talk_level_siliao", talk_level_siliao))
		{
			return -4;
		}
		int talk_level_cross = 0;
		if (!GetSubNodeValue(data_element, "talk_level_cross", talk_level_cross))
		{
			return -5;
		}

		int speaker_level = 0;
		if (!GetSubNodeValue(data_element, "horn_grade", speaker_level))
		{
			return -6;
		}

		int cross_speaker_level = 0;
		if (!GetSubNodeValue(data_element, "horn_grade_cross", cross_speaker_level))
		{
			return -7;
		}

		int create_group_level = 0;
		if (!GetSubNodeValue(data_element, "create_group_level", create_group_level) || create_group_level < 0 || create_group_level > MAX_ROLE_LEVEL)
		{
			return -8;
		}

		int single_talk_num_lv = 0;
		if (!GetSubNodeValue(data_element, "talk_num_lv", single_talk_num_lv) || single_talk_num_lv < 0 || single_talk_num_lv > MAX_ROLE_LEVEL)
		{
			return -9;
		}

		int single_talk_num_time = 0;
		if (!GetSubNodeValue(data_element, "talk_num_time", single_talk_num_time) || single_talk_num_time < 0)
		{
			return -10;
		}

		int single_talk_num_num = 0;
		if (!GetSubNodeValue(data_element, "talk_num_num", single_talk_num_num) || single_talk_num_num < 0)
		{
			return -11;
		}

		int forbidden_tips_level = 0;
		if (!GetSubNodeValue(data_element, "forbidden_tips_level", forbidden_tips_level) || forbidden_tips_level < 0)
		{
			return -12;
		}

		int day_trading_up = 0;
		if (!GetSubNodeValue(data_element, "day_trading_up", day_trading_up) || day_trading_up < 0)
		{
			return -13;
		}

		int is_sell = 0;
		if (!GetSubNodeValue(data_element, "is_sell", is_sell) || (is_sell != 0 && is_sell != 1))
		{
			return -14;
		}

		std::vector<ItemConfigData> reward_list;
		TiXmlElement* reward_list_element = data_element->FirstChildElement("real_name_reward_list");
		if (NULL != reward_list_element)
		{
			TiXmlElement* reward_element = reward_list_element->FirstChildElement("real_name_reward");
			while (reward_element)
			{
				ItemConfigData item;
				if (!item.ReadConfigNoCheck(reward_element)) break;

				reward_list.push_back(item);

				reward_element = reward_element->NextSiblingElement();
			}
		}

		PlatTypeUnion tmp_union;
		tmp_union.spid[0] = tmp_union.spid[1] = tmp_union.spid[2] = tmp_union.spid[3] = 0;
		for (unsigned int i = 0; i < 3; i++)
		{
			if (i < spid_name.length() && '\0' != spid_name[i])
			{
				tmp_union.spid[i + 1] = spid_name[i];	// 这样做是为了确保id大于0 字节序的关系
			}
		}

		SpidConfigItemMap::iterator it = m_spid_config_item_map.find(tmp_union.id);
		if (it != m_spid_config_item_map.end())
		{
			return -9999;
		}

		static SpidConfigItem cfg_item;
		cfg_item.spid = tmp_union.id;
		cfg_item.talk_level = talk_level;
		cfg_item.plat_name = spid_name;
		cfg_item.talk_level_xianmeng = talk_level_xianmeng;
		cfg_item.talk_level_siliao = talk_level_siliao;
		cfg_item.talk_level_cross = talk_level_cross;
		cfg_item.speaker_level = speaker_level;
		cfg_item.cross_speaker_level = cross_speaker_level;
		cfg_item.create_group_level = create_group_level;
		cfg_item.single_talk_num_lv = single_talk_num_lv;
		cfg_item.single_talk_num_time = single_talk_num_time;
		cfg_item.single_talk_num_num = single_talk_num_num;
		cfg_item.forbidden_tips_level = forbidden_tips_level;
		cfg_item.day_trading_up = day_trading_up;
		cfg_item.is_sell = is_sell;
		if(!reward_list.empty()) cfg_item.real_name_rewards = reward_list;
		m_spid_config_item_map.insert(SpidConfigItemMap::value_type(tmp_union.id, cfg_item));

		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int globalconfig::SpidConfig::InitCharLimitByOpenServer(TiXmlElement * RootElement)
{
	if (NULL == RootElement)
	{
		return -10000;
	}

	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -20000;
	}
	int cfg_count = 0;
	while (NULL != data_element)
	{
		ChatLimitCfg cl_cfg;
		std::string spid_name;
		if (!GetSubNodeValue(data_element, "spid", spid_name))
		{
			return -6;
		}

		if (!GetSubNodeValue(data_element, "day_start", cl_cfg.day_start))
		{
			return -1;
		}
		if (!GetSubNodeValue(data_element, "day_end", cl_cfg.day_end))
		{
			return -1;
		}
		
		if (0 == cl_cfg.day_end)
		{
			cl_cfg.day_end = INT_MAX;
		}

		if (!GetSubNodeValue(data_element,"talk_level", cl_cfg.talk_level))
		{
			return -3;
		}
		if (!GetSubNodeValue(data_element, "talk_level_xianmeng", cl_cfg.talk_level_xianmeng))
		{
			return -4;
		}
		if (!GetSubNodeValue(data_element, "talk_level_siliao", cl_cfg.talk_level_siliao))
		{
			return -5;
		}

		for (SpidConfigItemMap::const_iterator it = m_spid_config_item_map.begin(); it != m_spid_config_item_map.end(); ++it)
		{
			if (it->second.plat_name == spid_name)
			{
				cl_cfg.spid = it->second.spid;
			}
		}
		
		m_chat_limit_section_cfg[cfg_count++] = cl_cfg;
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int globalconfig::SpidConfig::GetServerOpenDays()
{
	time_t cur_time = EngineAdapter::Instance().Time(); //取当前时间做计算
	time_t open_server_timestamp = World::GetInstWorld()->GetServerOpenZeroTime(LocalConfig::Instance().GetUniqueServerID());
	int cur_real_open_day = GetDayIndex(open_server_timestamp, cur_time) + 1; // 活动开启时的开服天数，1~N

	if (cur_real_open_day <= 0)
	{
		cur_real_open_day = 1;
	}
	return cur_real_open_day;
}

