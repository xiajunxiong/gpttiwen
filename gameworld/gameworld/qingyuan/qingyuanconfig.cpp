#include "qingyuanconfig.hpp"
#include "servercommon/struct/moneyotherdef.h"
#include "npc/npcpool.h"
#include "monster/monster_group_pool.hpp"

QingYuanConfig::QingYuanConfig()
{
	m_fb_total_time_s = 0;
	m_fb_pre_pare_time_s = 0;

	memset(m_sub_act_finish_flag, 0, sizeof(m_sub_act_finish_flag));
}

QingYuanConfig::~QingYuanConfig()
{	
	std::vector<WeddingFB::LinkCfg>().swap(m_fb_phase_vec);
	std::vector<WeddingFB::BlessShopItemCfg>().swap(m_bless_shop_vec);
	std::vector<WeddingFB::HeatRewardCfg>().swap(m_heat_reward_vec);
	std::vector<int>().swap(m_sub_act_bao_wei_xiu_qiu_npc_id);
	std::vector<WeddingFB::SubActZaoShengGuiZi>().swap(m_zao_sheng_gui_zi_vec);
	std::vector<WeddingFB::XunYouCfg>().swap(m_xun_you_cfg_vec);
	std::vector<WeddingFB::XunYouWayPoint>().swap(m_xun_you_way_point_vec);
	std::vector<QingYuanTitleCfg>().swap(m_qing_yuan_title_cfg);
	std::vector<QingYuanDurationRewardCfg>().swap(m_qing_yuan_duration_reward_cfg);
	std::vector<QingYuanSkillCfg>().swap(m_qing_yuan_skill_cfg);
}

bool QingYuanConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("reward_group", InitGroupCfg);

	LOAD_CONFIG("other", InitFBBaseCfg);
	LOAD_CONFIG("wedding_party", InitFBLinkCfg);
	LOAD_CONFIG("heat_item", InitFBBlessShopCfg);
	LOAD_CONFIG("heat_reward", InitFBHeatRewardCfg);
	LOAD_CONFIG("wedding_party_order", InitWeddingRegistCfg);

	LOAD_CONFIG("float_parade", InitXunYouCfg);
	LOAD_CONFIG("parade_way", InitXunYouWayPointCfg);


	LOAD_CONFIG("find_gift", InitXubQingMiAiCfg);
	LOAD_CONFIG("heart_heart", InitXinXinXiangYinCfg);
	LOAD_CONFIG("guard_flowerball", InitBaoWeiXiuQiuCfg);
	LOAD_CONFIG("answer_questions", IniZaoShengGuiZiCfg);

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("marry_base", InitQingYuanCfg);
	LOAD_CONFIG("marry_title", InitQingYuanTitleCfg);
	LOAD_CONFIG("marry_day", InitQingYuanDurationRewardCfg);
	LOAD_CONFIG("marry_gift", InitQingYuanGiftCfg);
	LOAD_CONFIG("marry_skill", InitQingYuanSkillCfg);
	
	return true;
}

bool QingYuanConfig::CheckWeddingBeginTime(time_t begin_time)
{
	time_t zero_time = GetZeroTime(begin_time);
	time_t day_time = begin_time - zero_time;

	WeddingFB::WeddingRegistCfgMap::iterator cfg_it = m_wedding_regist_cfg_map.find(day_time);
	if (cfg_it == m_wedding_regist_cfg_map.end())
	{
		return false;
	}

	return true;
}

const WeddingFB::LinkCfg * QingYuanConfig::GetFBLinkCfg(int link_seq) const
{
	if (0 >= link_seq
		|| (int)m_fb_phase_vec.size() <= link_seq)
	{
		return NULL;
	}
	return &m_fb_phase_vec[link_seq];
}

int QingYuanConfig::GetFBCommonOperParam(int link_seq, int common_oper) const
{
	if (0 >= link_seq
		|| (int)m_fb_phase_vec.size() < link_seq)
	{
		return 0;
	}

	if (0 > common_oper
		|| WeddingFB::COMMON_OPER_TYPE_MAX <= common_oper)
	{
		return 0;
	}

	return m_fb_phase_vec[link_seq].common_oper_cfg_[common_oper];
}

const WeddingFB::BlessShopItemCfg * QingYuanConfig::GetFBBlessShopItemCfg(int shop_index) const
{
	if (0 > shop_index
		|| (int)m_bless_shop_vec.size() <= shop_index)
	{
		return NULL;
	}
	return &m_bless_shop_vec[shop_index];
}

const WeddingFB::HeatRewardCfg * QingYuanConfig::GetFBHeatRewardCfg(int reward_seq) const
{
	if (0 >= reward_seq
		|| (int)m_heat_reward_vec.size() < reward_seq)
	{
		return NULL;
	}
	return &m_heat_reward_vec[reward_seq];
}

const WeddingFB::SubActXubQingMiAi & QingYuanConfig::GetSubActXubQingMiAi() const
{
	return m_xun_qing_mi_ai_cfg;
}

const WeddingFB::SubActXinXinXiangYin & QingYuanConfig::GetSubActXinXinXiangYin() const
{
	return m_xin_xin_xiang_yin_cfg;
}

const WeddingFB::SubActBaoWeiXiuQiu * QingYuanConfig::GetSubActBaoWeiXiuQiu(int npc_id) const
{
	std::map<int, WeddingFB::SubActBaoWeiXiuQiu>::const_iterator cfg_it 
		= m_sub_act_bao_wei_xiu_qiu_map.find(npc_id);

	if (cfg_it == m_sub_act_bao_wei_xiu_qiu_map.end())
	{
		return NULL;
	}
	return &cfg_it->second;
}

const std::vector<int>& QingYuanConfig::GetSubActBaoWeiXiuQiuNpcIDVec() const
{
	return m_sub_act_bao_wei_xiu_qiu_npc_id;
}

const WeddingFB::SubActZaoShengGuiZi * QingYuanConfig::GetSubActZaoShengGuiZi(int index) const
{
	if (0 > index
		|| (int)m_zao_sheng_gui_zi_vec.size() <= index)
	{
		return NULL;
	}

	return &m_zao_sheng_gui_zi_vec[index];
}

bool QingYuanConfig::IsSubActFinish(int sub_act_type, int act_param)
{
	if (0 >= sub_act_type
		|| WeddingFB::SUB_ACT_TYPE_MAX <= sub_act_type)
	{
		return false;
	}

	return m_sub_act_finish_flag[sub_act_type] == act_param;
}

const WeddingFB::XunYouCfg * QingYuanConfig::GeXunYouCfg(int xun_you_type) const
{
	if (0 >= xun_you_type
		|| (int)m_xun_you_cfg_vec.size()<= xun_you_type)
	{
		return NULL;
	}

	return &m_xun_you_cfg_vec[xun_you_type];
}

const WeddingFB::XunYouWayPoint * QingYuanConfig::GeXunYouWayPoint(int way_point_index) const
{
	if (0 > way_point_index
		|| (int)m_xun_you_way_point_vec.size() <= way_point_index)
	{
		return NULL;
	}

	return &m_xun_you_way_point_vec[way_point_index];
}

const WeddingFB::GroupItemCfg * QingYuanConfig::GetGroupRandCfg(int group_id) const
{
	WeddingFB::GroupIDMap::const_iterator group_it = m_group_id_map.find(group_id);
	if (m_group_id_map.end() == group_it)
	{
		return NULL;
	}

	int rand_num = RandomNum(group_it->second.total_power);

	WeddingFB::GroupItemPool::const_iterator rand_it
		= group_it->second.item_pool.upper_bound(rand_num);
	if (group_it->second.item_pool.begin() == rand_it)
	{
		return NULL;
	}
	rand_it--;
	return &rand_it->second;
}

const QingYuanMarryCfg * QingYuanConfig::GetQingYuanMarryCfg(int marry_type)
{
	return MapValuesConstPtr(m_marry_cfg, marry_type);
}

const QingYuanOtherCfg & QingYuanConfig::GetOtherCfg()
{
	return m_other_cfg;
}

const QingYuanGiftCfg * QingYuanConfig::GetQingYuanGiftCfg(int gift_type)
{
	return MapValuesConstPtr(m_qing_yuan_gift_cfg, gift_type);
}

std::vector<QingYuanDurationRewardCfg> QingYuanConfig::GetQingYuanDurationCfg(int last_day, int now_day)
{
	std::vector<QingYuanDurationRewardCfg> ret;
	for (int i = 0; i < (int)m_qing_yuan_duration_reward_cfg.size(); ++i)
	{
		const QingYuanDurationRewardCfg & curr = m_qing_yuan_duration_reward_cfg[i];
		if (last_day < curr.need_marry_day && now_day >= curr.need_marry_day)  //已经领取的天数奖励要小于配置的，当前结婚天数要大于
		{
			ret.push_back(curr);
		}
	}

	return ret;
}

std::set<int> QingYuanConfig::GetQingYuanTitle(int intimacy, int day)
{
	std::set<int> ret;

	for (int i = 0; i < (int)m_qing_yuan_title_cfg.size(); ++i)
	{
		const QingYuanTitleCfg & curr = m_qing_yuan_title_cfg[i];
		if (day >= curr.need_day && intimacy >= curr.need_intimacy)
		{
			ret.insert(curr.title_id);
		}
	}

	return ret;
}

std::vector<QingYuanSkillCfg> QingYuanConfig::GetQingYuanSkillCfg(int intimacy)
{
	std::vector<QingYuanSkillCfg> ret;
	for (int i = 0; i < (int)m_qing_yuan_skill_cfg.size(); ++i)
	{
		const QingYuanSkillCfg & curr = m_qing_yuan_skill_cfg[i];
		if (intimacy >= curr.need_intimacy)
		{
			ret.push_back(curr);
		}
	}

	return ret;
}

int QingYuanConfig::InitFBBaseCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	if (!GetSubNodeValue(root_element, "scene_id", m_fb_base_cfg.scene_id)
		|| 0 > m_fb_base_cfg.scene_id)
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "born_x", m_fb_base_cfg.role_born.x))
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "born_y", m_fb_base_cfg.role_born.y))
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "how_many_barrage", m_fb_base_cfg.barrage_heat_add_times)
		|| 0 > m_fb_base_cfg.barrage_heat_add_times)
	{
		return -4;
	}

	if (!GetSubNodeValue(root_element, "barrage_add_heat", m_fb_base_cfg.barrage_heat_add_num)
		|| 0 > m_fb_base_cfg.barrage_heat_add_num)
	{
		return -5;
	}

	{
		TiXmlElement * node_element = root_element->FirstChildElement("candy_reward");
		if (NULL != node_element)
		{
			if (!m_fb_base_cfg.candies.ReadConfig(node_element))
			{
				return -100;
			}
		}
	}

	{
		TiXmlElement * reward_group_list = root_element->FirstChildElement("eatreward_group");
		if (NULL == reward_group_list)
		{
			return -200;
		}
		TiXmlElement * node_element = reward_group_list->FirstChildElement("node");
		while (node_element != NULL)
		{
			int reward_group_id = 0;
			if (!GetNodeValue(node_element, reward_group_id)
				||NULL == this->GetGroupRandCfg(reward_group_id))
			{
				return -201;
			}

			m_fb_base_cfg.meal__group_id_list.push_back(reward_group_id);
			node_element = node_element->NextSiblingElement();
		}
	}

	if (!GetSubNodeValue(root_element, "invite_number", m_fb_base_cfg.base_invite_max)
		|| 0 > m_fb_base_cfg.base_invite_max)
	{
		return -6;
	}

	if (!GetSubNodeValue(root_element, "add_number", m_fb_base_cfg.buy_invite_max_add)
		|| 0 > m_fb_base_cfg.buy_invite_max_add)
	{
		return -7;
	}

	if (!GetSubNodeValue(root_element, "add_times", m_fb_base_cfg.buy_invite_max_times)
		|| 0 > m_fb_base_cfg.buy_invite_max_times)
	{
		return -8;
	}



	if (!GetSubNodeValue(root_element, "money_type", m_fb_base_cfg.buy_invite_max_cost_type)
		|| 0 > m_fb_base_cfg.buy_invite_max_cost_type)
	{
		return -9;
	}

	if (!GetSubNodeValue(root_element, "add_consume", m_fb_base_cfg.buy_invite_max_cost_num)
		|| 0 > m_fb_base_cfg.buy_invite_max_cost_num)
	{
		return -10;
	}

	if (!GetSubNodeValue(root_element, "suit_id", m_fb_base_cfg.weddding_suit_id)
		|| 0 > m_fb_base_cfg.weddding_suit_id)
	{
		return -11;
	}

	if (!GetSubNodeValue(root_element, "eat_time_space", m_fb_base_cfg.meal__interval_time_s)
		|| 0 > m_fb_base_cfg.meal__interval_time_s)
	{
		return -12;
	}

	if (!GetSubNodeValue(root_element, "eat_times_max", m_fb_base_cfg.meal__max_num)
		|| 0 > m_fb_base_cfg.meal__max_num)
	{
		return -13;
	}
	return 0;
}

int QingYuanConfig::InitFBLinkCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	m_fb_phase_vec.push_back(WeddingFB::LinkCfg());
	m_fb_total_time_s = 0;

	while (NULL != root_element)
	{
		int link_seq = 0;
		if (!GetSubNodeValue(root_element, "wedding_list", link_seq)
			|| (int)m_fb_phase_vec.size() != link_seq)
		{
			return -1;
		}
		m_fb_phase_vec.push_back(WeddingFB::LinkCfg());
		
		WeddingFB::LinkCfg &cfg_info = m_fb_phase_vec[link_seq];
		cfg_info.link_seq = link_seq;

		if (!GetSubNodeValue(root_element, "link_type", cfg_info.phase_type)
			|| WeddingFB::PHASE_TYPE_INVALID > cfg_info.phase_type
			|| WeddingFB::PHASE_TYPE_MAX <= cfg_info.phase_type)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "link_parm", cfg_info.param_1))
		{
			return -3;
		}

		UNSTD_STATIC_CHECK(WeddingFB::PHASE_TYPE_MAX == 6);
		switch (cfg_info.phase_type)
		{
		case WeddingFB::PHASE_TYPE_SUB_ACT:
			{
				if (0 > cfg_info.param_1
					|| WeddingFB::SUB_ACT_TYPE_MAX <= cfg_info.param_1)
				{
					return -501;
				}
			}
			break;
		default:
			break;
		}

		if (!GetSubNodeValue(root_element, "last_time", cfg_info.link_time)
			|| 0 >= cfg_info.link_time)
		{
			return -5;
		}
		m_fb_total_time_s += cfg_info.link_time;
		if (WeddingFB::PHASE_TYPE_BROADCAST == cfg_info.phase_type)
		{
			m_fb_pre_pare_time_s += cfg_info.link_time;
		}
		

		UNSTD_STATIC_CHECK(WeddingFB::COMMON_OPER_TYPE_MAX == 7);
		if (!GetSubNodeValue(root_element, "is_come", cfg_info.common_oper_cfg_[WeddingFB::COMMON_OPER_TYPE_ENTER_SCENE]))
		{
			return -11;
		}

		if (!GetSubNodeValue(root_element, "wedding_candies", cfg_info.common_oper_cfg_[WeddingFB::COMMON_OPER_TYPE_CANDIES]))
		{
			return -12;
		}

		if (!GetSubNodeValue(root_element, "marry_cartoon", cfg_info.common_oper_cfg_[WeddingFB::COMMON_OPER_TYPE_RITE]))
		{
			return -13;
		}

		if (!GetSubNodeValue(root_element, "marry_eat", cfg_info.common_oper_cfg_[WeddingFB::COMMON_OPER_TYPE_MEAL]))
		{
			return -14;
		}

		if (!GetSubNodeValue(root_element, "is_blessing", cfg_info.common_oper_cfg_[WeddingFB::COMMON_OPER_TYPE_BLESS]))
		{
			return -15;
		}

		if (!GetSubNodeValue(root_element, "is_barrage", cfg_info.common_oper_cfg_[WeddingFB::COMMON_OPER_TYPE_BARRAGE]))
		{
			return -16;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int QingYuanConfig::InitFBBlessShopCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int shop_item_index = 0;
		if (!GetSubNodeValue(root_element, "shop_item_index", shop_item_index)
			|| (int)m_bless_shop_vec.size() != shop_item_index)
		{
			return -1;
		}

		m_bless_shop_vec.push_back(WeddingFB::BlessShopItemCfg());
		WeddingFB::BlessShopItemCfg &cfg_info = m_bless_shop_vec[shop_item_index];
		cfg_info.shop_item_index = shop_item_index;

		if (!GetSubNodeValue(root_element, "money_type", cfg_info.cost_type)
			|| 0 > cfg_info.cost_type
			|| MONEY_TYPE_MAX <= cfg_info.cost_type)
		{
			return -2;
		}
		if (!GetSubNodeValue(root_element, "price", cfg_info.cost_num)
			|| 0 >= cfg_info.cost_num)
		{
			return -3;
		}
		if (!GetSubNodeValue(root_element, "add_heat", cfg_info.add_heatt)
			|| 0 >= cfg_info.add_heatt)
		{
			return -4;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int QingYuanConfig::InitFBHeatRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	m_heat_reward_vec.push_back(WeddingFB::HeatRewardCfg());

	while (NULL != root_element)
	{
		int reward_seq = 0;
		if (!GetSubNodeValue(root_element, "heat_type", reward_seq)
			|| (int)m_heat_reward_vec.size() != reward_seq)
		{
			return -1;
		}

		m_heat_reward_vec.push_back(WeddingFB::HeatRewardCfg());
		WeddingFB::HeatRewardCfg &cfg_info = m_heat_reward_vec[reward_seq];
		cfg_info.reward_seq = reward_seq;

		if (!GetSubNodeValue(root_element, "heat_request", cfg_info.need_hot)
			|| 0 > cfg_info.need_hot)
		{
			return -2;
		}

		{
			TiXmlElement * reward_list_element = root_element->FirstChildElement("heatreward_list");
			if (NULL == reward_list_element)
			{
				return -101;
			}

			TiXmlElement * node_element = reward_list_element->FirstChildElement("heatreward");
			while (NULL != node_element)
			{
				ItemConfigData temp_item;
				if (!temp_item.ReadConfig(node_element))
				{
					return -102;
				}
				cfg_info.reward_vec.push_back(temp_item);
				node_element = node_element->NextSiblingElement();
			}

			if (cfg_info.reward_vec.empty())
			{
				return -103;
			}
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int QingYuanConfig::InitWeddingRegistCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int index = 0;
		if (!GetSubNodeValue(root_element, "seq", index)
			|| index != (int)m_wedding_regist_cfg_map.size())
		{
			return -1;
		}
		time_t begin_day_second = 0;
		if (!GetSubNodeValue(root_element, "open_time", begin_day_second)
			|| 0 > begin_day_second
			|| SECOND_PER_DAY <= begin_day_second)
		{
			return -2;
		}

		WeddingFB::WeddingRegistCfg & cfg_info = m_wedding_regist_cfg_map[begin_day_second];
		cfg_info.index = index;
		cfg_info.begin_day_second = begin_day_second;

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int QingYuanConfig::InitXubQingMiAiCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	{
		TiXmlElement * reward_group_list = root_element->FirstChildElement("reward_group");
		if (NULL == reward_group_list)
		{
			return -200;
		}
		TiXmlElement * node_element = reward_group_list->FirstChildElement("node");
		while (node_element != NULL)
		{
			int reward_group_id = 0;
			if (!GetNodeValue(node_element, reward_group_id)
				|| NULL == this->GetGroupRandCfg(reward_group_id))
			{
				return -201;
			}

			m_xun_qing_mi_ai_cfg.item_list.push_back(reward_group_id);
			node_element = node_element->NextSiblingElement();
		}
	}
	const int GIFT_POINT_NUM = 6;
	const int GIFT_POINT_SIZE = 2;
	for (int i = 0; i < GIFT_POINT_NUM; i++)
	{
		std::string list_str = STRING_SPRINTF("xyz%d", i + 1);

		TiXmlElement * point_list = root_element->FirstChildElement(list_str.c_str());
		if (NULL == point_list)
		{
			return -300;
		}

		TiXmlElement * node_element = point_list->FirstChildElement("node");
		Posi posi;
		for (int i = 0; i < GIFT_POINT_SIZE; i++)
		{
			if (!GetNodeValue(node_element, posi[i])
				|| 0 > posi[i])
			{
				return -301;
			}
		}
		m_xun_qing_mi_ai_cfg.gift_pos.push_back(posi);
	}

	//全完成标记
	m_sub_act_finish_flag[WeddingFB::SUB_ACT_TYPE_XUN_QING_MI_AI] = 1;

	return 0;
}

int QingYuanConfig::InitXinXinXiangYinCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	{
		TiXmlElement * reward_group_list = root_element->FirstChildElement("reward_group");
		if (NULL == reward_group_list)
		{
			return -200;
		}
		TiXmlElement * node_element = reward_group_list->FirstChildElement("node");
		while (node_element != NULL)
		{
			int reward_group_id = 0;
			if (!GetNodeValue(node_element, reward_group_id)
				|| NULL == this->GetGroupRandCfg(reward_group_id))
			{
				return -201;
			}

			m_xin_xin_xiang_yin_cfg.item_list.push_back(reward_group_id);
			node_element = node_element->NextSiblingElement();
		}
	}

	//全完成标记
	m_sub_act_finish_flag[WeddingFB::SUB_ACT_TYPE_XIN_XIN_XIANG_YIN] = 1;

	return 0;
}

int QingYuanConfig::InitBaoWeiXiuQiuCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int index = 0;
		if (!GetSubNodeValue(root_element, "index", index)
			|| index != (int)m_sub_act_bao_wei_xiu_qiu_map.size()
			|| WeddingFB::SUB_ACT_BAO_WEI_XIU_QIU_MAX_CFG_SIZE <= index)
		{
			return -1;
		}

		int npc_id = 0;
		if (!GetSubNodeValue(root_element, "npc_id", npc_id))
		{
			return -2;
		}
		//const ServerNPCInfoCfg* npc_cfg = NPCPOOL->GetServerNPCInfo(npc_id);
		//if (NULL == npc_cfg)
		//{
		//	return -3;
		//}

		int monster_group_id = 0;
		if (!GetSubNodeValue(root_element, "monster_group_id", monster_group_id))
		{
			return -4;
		}

		if (NULL == MonsterGroupPool::GetInstance().GetMonsterGroupCfg(monster_group_id))
		{
			return -4000 - index;
		}
		WeddingFB::SubActBaoWeiXiuQiu & cfg_info = m_sub_act_bao_wei_xiu_qiu_map[npc_id];
		cfg_info.reward_index = index;
		cfg_info.npc_id = npc_id;
		cfg_info.monster_group_id = monster_group_id;
		{
			TiXmlElement * reward_list_element = root_element->FirstChildElement("reward_list");
			if (NULL == reward_list_element)
			{
				return -201;
			}

			TiXmlElement * node_element = reward_list_element->FirstChildElement("reward");
			while (NULL != node_element)
			{
				ItemConfigData temp_item;
				if (!temp_item.ReadConfig(node_element))
				{
					return -202;
				}
				cfg_info.item_list.push_back(temp_item);
				node_element = node_element->NextSiblingElement();
			}
		}

		m_sub_act_bao_wei_xiu_qiu_npc_id.push_back(npc_id);

		//全完成标记
		SetBit(m_sub_act_finish_flag[WeddingFB::SUB_ACT_TYPE_BAO_WEI_XIU_QIU], cfg_info.reward_index);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int QingYuanConfig::IniZaoShengGuiZiCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int index = 0;
		if (!GetSubNodeValue(root_element, "index", index)
			|| index != (int)m_zao_sheng_gui_zi_vec.size()
			|| 30 < index)
		{
			return -1;
		}
		m_zao_sheng_gui_zi_vec.push_back(WeddingFB::SubActZaoShengGuiZi());
		WeddingFB::SubActZaoShengGuiZi & cfg_info = m_zao_sheng_gui_zi_vec[index];
		cfg_info.reward_index = index;

		{
			TiXmlElement * reward_list_element = root_element->FirstChildElement("answerreward_list");
			if (NULL == reward_list_element)
			{
				return -201;
			}

			TiXmlElement * node_element = reward_list_element->FirstChildElement("answerreward");
			while (NULL != node_element)
			{
				ItemConfigData temp_item;
				if (!temp_item.ReadConfig(node_element))
				{
					return -202;
				}
				cfg_info.item_list.push_back(temp_item);
				node_element = node_element->NextSiblingElement();
			}
		}

		SetBit(m_sub_act_finish_flag[WeddingFB::SUB_ACT_TYPE_ZAO_SHENG_GUI_ZI], cfg_info.reward_index);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int QingYuanConfig::InitXunYouCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}
	m_xun_you_cfg_vec.push_back(WeddingFB::XunYouCfg());
	while (NULL != root_element)
	{
		int xun_you_car_type = 0;
		if (!GetSubNodeValue(root_element, "float_type", xun_you_car_type)
			|| m_xun_you_cfg_vec.size() != xun_you_car_type)
		{
			return -1;
		}

		m_xun_you_cfg_vec.push_back(WeddingFB::XunYouCfg());
		WeddingFB::XunYouCfg & cfg_info = m_xun_you_cfg_vec[xun_you_car_type];
		cfg_info.xun_you_car_type = xun_you_car_type;

		if (!GetSubNodeValue(root_element, "scene_id", cfg_info.scene_id)
			|| 0 > cfg_info.scene_id)
		{
			return -2;
		}
		
		if (!GetSubNodeValue(root_element, "dir_y", cfg_info.begin_dir))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "move_speed", cfg_info.speed)
			|| 0 >= cfg_info.speed)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "money_type", cfg_info.cost_type)
			|| 0 > cfg_info.cost_type
			|| MONEY_TYPE_MAX <= cfg_info.cost_type)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "price", cfg_info.cost_num)
			|| 0 > cfg_info.cost_num)
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "box_num", cfg_info.creat_gift_num)
			|| 0 > cfg_info.creat_gift_num)
		{
			return -7;
		}

		if (!GetSubNodeValue(root_element, "box_lasttime", cfg_info.gift_valid_time)
			|| 0 >= cfg_info.gift_valid_time)
		{
			return -8;
		}

		if (!GetSubNodeValue(root_element, "boxnpc_id", cfg_info.gift_gather_id))
		{
			return -9;
		}
		const ServerNPCInfoCfg* npc_cfg = NPCPOOL->GetServerNPCInfo(cfg_info.gift_gather_id);
		if (NULL == npc_cfg)
		{
			return -10;
		}

		{
			TiXmlElement * reward_group_list = root_element->FirstChildElement("reward_group_id");
			if (NULL == reward_group_list)
			{
				return -200;
			}
			TiXmlElement * node_element = reward_group_list->FirstChildElement("node");
			while (node_element != NULL)
			{
				int reward_group_id = 0;
				if (!GetNodeValue(node_element, reward_group_id)
					|| NULL == this->GetGroupRandCfg(reward_group_id))
				{
					return -201;
				}

				cfg_info.gift_group_id_list.push_back(reward_group_id);
				node_element = node_element->NextSiblingElement();
			}
		}

		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int QingYuanConfig::InitXunYouWayPointCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}
	while (NULL != root_element)
	{
		int way_point_index = 0;
		if (!GetSubNodeValue(root_element, "way_index", way_point_index)
			|| m_xun_you_way_point_vec.size() != way_point_index)
		{
			return -1;
		}
		m_xun_you_way_point_vec.push_back(WeddingFB::XunYouWayPoint());
		WeddingFB::XunYouWayPoint &cfg_info = m_xun_you_way_point_vec[way_point_index];
		cfg_info.way_point_index = way_point_index;

		if (!GetSubNodeValue(root_element, "way_x", cfg_info.way_poi.x))
		{
			return -3;
		}
		if (!GetSubNodeValue(root_element, "way_y", cfg_info.way_poi.y))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "stop_time", cfg_info.stop_wait_time)
			|| 0 > cfg_info.stop_wait_time)
		{
			return -5;
		}
		const int XUN_YOU_GIFT_POINT_NUM = 10;
		for (int i = 0; i < XUN_YOU_GIFT_POINT_NUM; i++)
		{
			std::string box_x_str = STRING_SPRINTF("box_x%d", i + 1);
			std::string box_y_str = STRING_SPRINTF("box_y%d", i + 1);

			Posi gift_poi;
			if (!GetSubNodeValue(root_element, box_x_str, gift_poi.x))
			{
				return -101;
			}

			if (!GetSubNodeValue(root_element, box_y_str, gift_poi.y))
			{
				return -102;
			}

			cfg_info.creat_gift_vec.push_back(gift_poi);
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int QingYuanConfig::InitGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id)
			|| 0 >= group_id)
		{
			return -1;
		}

		WeddingFB::GroupCfg & group_cfg = m_group_id_map[group_id];
		group_cfg.group_id = group_id;

		WeddingFB::GroupItemCfg & item_cfg = group_cfg.item_pool[group_cfg.total_power];
		item_cfg.group_id = group_id;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.power)
			|| 0 >= item_cfg.power)
		{
			return -2;
		}
		group_cfg.total_power += item_cfg.power;

		if (!GetSubNodeValue(root_element, "coin", item_cfg.coin_bind)
			|| 0 > item_cfg.coin_bind)
		{
			return -3;
		}

		{
			TiXmlElement * node_element = root_element->FirstChildElement("reward");
			if (NULL != node_element)
			{
				if (!item_cfg.reward_item.ReadConfig(node_element))
				{
					return -102;
				}
			}
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int QingYuanConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		ret_line;
	}

	if (!GetSubNodeValue(root_element, "marry_min_intimacy", m_other_cfg.qing_yuan_need_intimacy))ret_line;
	if (!GetSubNodeValue(root_element, "calm_time", m_other_cfg.qing_yuan_leave_wait_time))ret_line;
	if (!GetSubNodeValue(root_element, "force_divorce_time", m_other_cfg.qing_yuan_logout_duration_can_leave))ret_line;
	if (!GetSubNodeValue(root_element, "divorce_use", m_other_cfg.qing_yuan_force_leave_comsume_gold))ret_line;

	return 0;
}

int QingYuanConfig::InitQingYuanCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		QingYuanMarryCfg cfg;
		if (!GetSubNodeValue(root_element, "marry_type", cfg.marry_type))ret_line;
		if (!GetSubNodeValue(root_element, "money_type", cfg.money_type))ret_line;
		if (!GetSubNodeValue(root_element, "price", cfg.money_value))ret_line;

		TiXmlElement * item_list_element = root_element->FirstChildElement("bugreward_list");
		if (NULL != item_list_element)
		{
			TiXmlElement * item_element = item_list_element->FirstChildElement("bugreward");
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
				{
					return -__LINE__;
				}

				cfg.rewards.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}

		m_marry_cfg[cfg.marry_type] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int QingYuanConfig::InitQingYuanTitleCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		QingYuanTitleCfg cfg;
		if (!GetSubNodeValue(root_element, "title_id", cfg.title_id))ret_line;
		if (!GetSubNodeValue(root_element, "marry_intimacy", cfg.need_intimacy))ret_line;
		if (!GetSubNodeValue(root_element, "marry_time", cfg.need_day))ret_line;
	
		m_qing_yuan_title_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int QingYuanConfig::InitQingYuanDurationRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		QingYuanDurationRewardCfg cfg;
		if (!GetSubNodeValue(root_element, "marry_time", cfg.need_marry_day))ret_line;
		TiXmlElement * item_list_element = root_element->FirstChildElement("reward_list");
		if (NULL != item_list_element)
		{
			TiXmlElement * item_element = item_list_element->FirstChildElement("reward");
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
				{
					return -__LINE__;
				}

				cfg.rewards.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}

		m_qing_yuan_duration_reward_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int QingYuanConfig::InitQingYuanGiftCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		QingYuanGiftCfg cfg;
		if (!GetSubNodeValue(root_element, "gift_id", cfg.qing_yuan_gift_type))ret_line;
		if (!GetSubNodeValue(root_element, "gift_price", cfg.need_money_value))ret_line;
		if (!GetSubNodeValue(root_element, "money_type", cfg.need_money_type))ret_line;
		if (!GetSubNodeValue(root_element, "last_day", cfg.during_day))ret_line;
		if (!GetSubNodeValue(root_element, "bug_get", cfg.active_reward_money_value))ret_line;
		if (!GetSubNodeValue(root_element, "money_type_1", cfg.active_reward_money_type))ret_line;

		TiXmlElement * item_list_element = root_element->FirstChildElement("dayreward_list");
		if (NULL != item_list_element)
		{
			TiXmlElement * item_element = item_list_element->FirstChildElement("dayreward");
			while (NULL != item_element)
			{
				ItemConfigData item;
				if (!item.ReadConfig(item_element))
				{
					return -__LINE__;
				}

				cfg.day_rewards.push_back(item);
				item_element = item_element->NextSiblingElement();
			}
		}

		m_qing_yuan_gift_cfg[cfg.qing_yuan_gift_type] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int QingYuanConfig::InitQingYuanSkillCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		QingYuanSkillCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.seq))ret_line;
		if (!GetSubNodeValue(root_element, "sp_id", cfg.skill_id))ret_line;
		if (!GetSubNodeValue(root_element, "marry_intimacy", cfg.need_intimacy))ret_line;

		m_qing_yuan_skill_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
