#include "dreamnoteschallengeconfig.hpp"
#include "checkresourcecenter.hpp"
#include "servercommon/struct/moneyotherdef.h"

DreamNotesChallengeConfig::DreamNotesChallengeConfig() : max_level(0)
{
}

DreamNotesChallengeConfig::~DreamNotesChallengeConfig()
{
	std::vector<DreamNotesChallengeCfg>().swap(m_challenge_fb_cfg);
}

bool DreamNotesChallengeConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("dream_config", InitChallengeFbCfg);
	LOAD_CONFIG("exp", InitChallengeExpCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("buy_extra_times", InitExtraBuyCfg);

	return true;
}

const DreamNotesChallengeCfg * DreamNotesChallengeConfig::GetChallengeFbCfg(int level)
{
	if(level <= 0 || level > (int)m_challenge_fb_cfg.size()) return NULL;

	return &m_challenge_fb_cfg[level - 1];
}

int DreamNotesChallengeConfig::GetChallengeExpCfg(int level)
{
	std::map<int, int>::iterator it = m_challenge_exp_cfg.find(level);
	if(it != m_challenge_exp_cfg.end()) return it->second;

	return 0;
}

const DreamNotesChallengeBuyCfg * DreamNotesChallengeConfig::GetPriceByBuyTimes(int buy_times)
{
	std::map<int, DreamNotesChallengeBuyCfg>::iterator it = m_extra_buy_cfg.find(buy_times);
	if(it == m_extra_buy_cfg.end()) return NULL;

	return &it->second;
}

int DreamNotesChallengeConfig::GetMaxBuyTimes()
{
	if(m_extra_buy_cfg.empty()) return 0;

 	return m_extra_buy_cfg.rbegin()->first;
}

int DreamNotesChallengeConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(data_element, "pass_time_limit", m_other_cfg.pass_time_limit) || m_other_cfg.pass_time_limit < 0) return -1;
	if (!GetSubNodeValue(data_element, "extra_purchase_time", m_other_cfg.extra_purchase_time) || m_other_cfg.extra_purchase_time < 0) return -2;

	return 0;
}

int DreamNotesChallengeConfig::InitChallengeFbCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	int last_level = 0;
	while (NULL != data_element)
	{
		int level = 0;
		if (!GetSubNodeValue(data_element, "level", level) || level <= 0) return -1;
		if (last_level + 1 != level)
		{
			return -(700 + level);
		}
		last_level = level;
		DreamNotesChallengeCfg challenge_fb_node;
		challenge_fb_node.level = level;

		if (!GetSubNodeValue(data_element, "recommend_level", challenge_fb_node.recommend_level) || challenge_fb_node.recommend_level < 0 || challenge_fb_node.recommend_level > MAX_ROLE_LEVEL) return -555;
		if (!GetSubNodeValue(data_element, "scene_id", challenge_fb_node.scene_id)) return -2;
		CheckResourceCenter::Instance().GetSceneCheck()->Add(challenge_fb_node.scene_id, __FUNCTION__);

		if (!GetSubNodeValue(data_element, "boss_id", challenge_fb_node.boss_id)|| challenge_fb_node.boss_id <= 0) return -3;
		if (!GetSubNodeValue(data_element, "boss_x", challenge_fb_node.boss_pos.x) || challenge_fb_node.boss_pos.x < 0) return -4;
		if (!GetSubNodeValue(data_element, "boss_y", challenge_fb_node.boss_pos.y) || challenge_fb_node.boss_pos.y < 0) return -5;
		if (!GetSubNodeValue(data_element, "boss_dir", challenge_fb_node.boss_dir) || challenge_fb_node.boss_dir < 0) return -6;

		TiXmlElement * first_reward_list = data_element->FirstChildElement("first_reward_list");
		if (NULL == first_reward_list)
		{
			return -100;
		}
		TiXmlElement * first_reward = first_reward_list->FirstChildElement("first_reward");
		while (first_reward != NULL)
		{
			ItemConfigData reward;
			if (!reward.ReadConfig(first_reward))
			{
				return -101;
			}
			challenge_fb_node.first_pass_reward.push_back(reward);
			first_reward = first_reward->NextSiblingElement();
		}

		TiXmlElement * common_reward_list = data_element->FirstChildElement("reward_list");
		if (NULL == common_reward_list)
		{
			return -200;
		}
		TiXmlElement * common_reward = common_reward_list->FirstChildElement("reward");
		while (common_reward != NULL)
		{
			ItemConfigData reward;
			if (!reward.ReadConfig(common_reward))
			{
				return -201;
			}
			challenge_fb_node.common_reward.push_back(reward);
			common_reward = common_reward->NextSiblingElement();
		}

		if (!GetSubNodeValue(data_element, "coin", challenge_fb_node.coin) || challenge_fb_node.coin < 0) return -7;
		if (!GetSubNodeValue(data_element, "luck", challenge_fb_node.lucky) || challenge_fb_node.lucky < 0) return -8;

		if (!GetSubNodeValue(data_element, "npc_1_id", challenge_fb_node.npc_1_id) || challenge_fb_node.npc_1_id < 0) return -9;
		if (!GetSubNodeValue(data_element, "npc_1_x", challenge_fb_node.npc_1_pos.x) || challenge_fb_node.npc_1_pos.x < 0) return -10;
		if (!GetSubNodeValue(data_element, "npc_1_y", challenge_fb_node.npc_1_pos.y) || challenge_fb_node.npc_1_pos.y < 0) return -11;
		if (!GetSubNodeValue(data_element, "npc_1_dir", challenge_fb_node.npc_1_dir) || challenge_fb_node.npc_1_dir < 0) return -12;
		if (!GetSubNodeValue(data_element, "is_need_talk1", challenge_fb_node.is_need_talk_1)) return -__LINE__;
		challenge_fb_node.is_need_talk_1 = (0 != challenge_fb_node.is_need_talk_1) ? 1 : 0;

		//npc_2可以为0
		if (!GetSubNodeValue(data_element, "npc_2_id", challenge_fb_node.npc_2_id) || challenge_fb_node.npc_2_id < 0) return -13;
		if (!GetSubNodeValue(data_element, "npc_2_x", challenge_fb_node.npc_2_pos.x) || challenge_fb_node.npc_2_pos.x < 0) return -14;
		if (!GetSubNodeValue(data_element, "npc_2_y", challenge_fb_node.npc_2_pos.y) || challenge_fb_node.npc_2_pos.y < 0) return -15;
		if (!GetSubNodeValue(data_element, "npc_2_dir", challenge_fb_node.npc_2_dir) || challenge_fb_node.npc_2_dir < 0) return -16;
		if (!GetSubNodeValue(data_element, "is_need_talk2", challenge_fb_node.is_need_talk_2)) return -__LINE__;
		challenge_fb_node.is_need_talk_2 = (0 != challenge_fb_node.is_need_talk_2) ? 1 : 0;

		//npc_3可以为0
		if (!GetSubNodeValue(data_element, "npc_3_id", challenge_fb_node.npc_3_id) || challenge_fb_node.npc_3_id < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "npc_3_x", challenge_fb_node.npc_3_pos.x) || challenge_fb_node.npc_3_pos.x < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "npc_3_y", challenge_fb_node.npc_3_pos.y) || challenge_fb_node.npc_3_pos.y < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "npc_3_dir", challenge_fb_node.npc_3_dir) || challenge_fb_node.npc_3_dir < 0) return -__LINE__;
		if (!GetSubNodeValue(data_element, "is_need_talk3", challenge_fb_node.is_need_talk_3)) return -__LINE__;
		challenge_fb_node.is_need_talk_3 = (0 != challenge_fb_node.is_need_talk_3) ? 1 : 0;

		if (!GetSubNodeValue(data_element, "first_yuanling", challenge_fb_node.first_yuanling) || challenge_fb_node.first_yuanling < 0) return -17;
		if (!GetSubNodeValue(data_element, "yuanling", challenge_fb_node.common_yuanling) || challenge_fb_node.common_yuanling < 0) return -18;
		if (!GetSubNodeValue(data_element, "first_luck", challenge_fb_node.first_lucky) || challenge_fb_node.first_lucky < 0) return -19;

		if (!GetSubNodeValue(data_element, "partner_fragment_rate", challenge_fb_node.partner_fragment_rate) || challenge_fb_node.partner_fragment_rate < 0) return -20;
		if (!GetSubNodeValue(data_element, "power_expend", challenge_fb_node.power_expend) || challenge_fb_node.power_expend <= 0) return -21;
		if (!GetSubNodeValue(data_element, "recommend_score", challenge_fb_node.recommend_score) || challenge_fb_node.recommend_score < 0) return -22;
		
		TiXmlElement * partner_fragment_reward_list = data_element->FirstChildElement("partner_fragment_reward_list");
		if (NULL != partner_fragment_reward_list)
		{
			TiXmlElement * partner_fragment_reward = partner_fragment_reward_list->FirstChildElement("partner_fragment_reward");
			while (partner_fragment_reward != NULL)
			{
				ItemConfigData reward;
				if (!reward.ReadConfig(partner_fragment_reward))
				{
					return -201;
				}
				challenge_fb_node.partner_fragment_reward.push_back(reward);
				partner_fragment_reward = partner_fragment_reward->NextSiblingElement();
			}
		}

		max_level = max_level >= challenge_fb_node.level ? max_level : challenge_fb_node.level;
		m_challenge_fb_cfg.push_back(challenge_fb_node);
		data_element = data_element->NextSiblingElement();
	}


	return 0;
}

int DreamNotesChallengeConfig::InitChallengeExpCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	while (NULL != data_element)
	{
		int level = 0, exp = 0;
		if (!GetSubNodeValue(data_element, "level", level) || level <= 0 || level > MAX_ROLE_LEVEL) return -1;
		if (!GetSubNodeValue(data_element, "exp", exp) || exp < 0) return -2;

		m_challenge_exp_cfg[level] = exp;
		data_element = data_element->NextSiblingElement();
	}
	if (m_challenge_exp_cfg.empty() || m_challenge_exp_cfg.size() != MAX_ROLE_LEVEL)
	{
		return -33;
	}

	return 0;
}

int DreamNotesChallengeConfig::InitExtraBuyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}

	int next_buy_times = 1;
	while (NULL != data_element)
	{
		int buy_times = 0;
		if (!GetSubNodeValue(data_element, "buy_times", buy_times) || buy_times != next_buy_times) return -1;

		DreamNotesChallengeBuyCfg node_cfg;
		if (!GetSubNodeValue(data_element, "price_type", node_cfg.price_type) || node_cfg.price_type < 0 || node_cfg.price_type >= MONEY_TYPE_MAX) return -2;
		if (!GetSubNodeValue(data_element, "price", node_cfg.price) || node_cfg.price <= 0) return -3;
	
		next_buy_times++;
		m_extra_buy_cfg[buy_times] = node_cfg;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}
