#include "rolecrystalfbconfig.hpp"
#include "item/itempool.h"
#include "servercommon/rolecrystalfbdef.h"
#include "servercommon/roleconvertshopparam.hpp"

RoleCrystalFbConfig::RoleCrystalFbConfig()
{
}

RoleCrystalFbConfig::~RoleCrystalFbConfig()
{
	for (std::map<int, std::vector<Event_NPC_cfg> >::iterator it = m_event_npc_cfg_map.begin(); it != m_event_npc_cfg_map.end(); it++){
		std::vector<Event_NPC_cfg>().swap(it->second);
	}
	for (std::map<int, std::vector<NpcShopConfig> >::iterator it = m_npc_shop_list_map.begin(); it != m_npc_shop_list_map.end(); it++){
		std::vector<NpcShopConfig>().swap(it->second);
	}
	for (std::map<int, std::vector<WorkshopItemConfig> >::iterator it = m_workshop_item_cfg_map.begin(); it != m_workshop_item_cfg_map.end(); it++){
		std::vector<WorkshopItemConfig>().swap(it->second);
	}

	std::vector<PowerUpCfg>().swap(m_power_up_cfg);
}

bool RoleCrystalFbConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("scene_group", InitSceneCfg);		//先初始化场景组
	LOAD_CONFIG("crystal_config", InitLevelCfg);
	LOAD_CONFIG("box_group", InitBoxCfg);
	LOAD_CONFIG("box_num", InitRandNumCfg);
	LOAD_CONFIG("reward_group", InitRewardCfg);
	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("fb_expend_num", InitUseLimitCfg);
	LOAD_CONFIG("monster_group", InitMonsterCfg);
	LOAD_CONFIG("reward_proportion", InitPlayTimesToRewardColumnCfg);
	LOAD_CONFIG("monster_dorp_list", InitMonsterDropCfg);
	LOAD_CONFIG("born_group", InitPosCfg);
	LOAD_CONFIG("event_group", InitEventGroupCfg);
	LOAD_CONFIG("event_list", InitEventListCfg);
	LOAD_CONFIG("shop_list", InitShopListCfg);
	LOAD_CONFIG("buff_list", InitBuffListCfg);
	LOAD_CONFIG("gather_reward", InitGatherNpcCfg);
	LOAD_CONFIG("formula_drop_group", InitWrokShopItemCfg);
	LOAD_CONFIG("exp", InitExpCfg);
	LOAD_CONFIG("buy_power", InitBuyEnergyCfg);
	LOAD_CONFIG("power_up", InitPowerUpCfg);
	LOAD_CONFIG("exp", InitEnergyFindCfg);

	return true;
}

const RoleCrystalFbLevelCfg * RoleCrystalFbConfig::GetRoleCrystalFbLevelCfg(int seq)
{
	if (seq < 0 || seq >= (int)m_level_cfg.size())
	{
		return NULL;
	}

	return &m_level_cfg[seq];
}

int RoleCrystalFbConfig::GetLayerNum(int seq)
{
	if (seq < 0 || seq >= (int)m_level_cfg.size())
	{
		return 0;
	}

	return (int)m_level_cfg[seq].layer_cfg.size() + 1;		//随机层加上一个boss 固定层
}

bool RoleCrystalFbConfig::IsCrystalFbBoxGather(int gather_id)
{
	if (m_all_box_gather_id.find(gather_id) != m_all_box_gather_id.end())
	{
		return true;
	}

	return false;
}

const CrystalFbBoxCfg::ItemCfg * RoleCrystalFbConfig::GetBoxCfg(int group_id, int box_seq)
{
	std::map<int, CrystalFbBoxCfg>::iterator iter = m_box_cfg.find(group_id);
	if (iter == m_box_cfg.end())
	{
		return NULL;
	}

	if (box_seq < 0 || box_seq >= (int)iter->second.group_cfg.size())
	{
		return NULL;
	}

	return  &iter->second.group_cfg[box_seq];
}

//int RoleCrystalFbConfig::GetRandScene(int scene_group_id, std::set<int>  exclude_scene_id)
//{
//	std::map<int, CrystalFbSceneCfg > ::iterator iter = m_scene_cfg.find(scene_group_id);
//	if (iter == m_scene_cfg.end())
//	{
//		return 0;
//	}
//
//	std::vector<CrystalFbSceneCfg::ItemCfg> group_cfg = iter->second.group_cfg;
//
//	int rate_count = CrystalFbSceneCfg::MAX_RATE_COUNT;
//	
//	if (exclude_scene_id.size()>0)
//	{
//		for (int i = 0; i < (int)group_cfg.size(); ++i)
//		{
//			if (exclude_scene_id.find(group_cfg[i].scene_id) != exclude_scene_id.end())
//			{
//				rate_count -= group_cfg[i].rate;
//				group_cfg[i].rate = 0;
//			}
//		}
//	}
//
//	if (rate_count <= 0)
//	{
//		return 0;
//	}
//
//	int r = rand() % rate_count;
//	for (int i = 0; i < (int)group_cfg.size(); ++i)
//	{
//		if (r < group_cfg[i].rate)
//		{
//			return group_cfg[i].scene_id;
//		}
//
//		r -= group_cfg[i].rate;
//	}
//
//	return 0;
//}

const CrystalFbSceneCfg::ItemCfg *  RoleCrystalFbConfig::GetRandSceneCfg(int scene_group_id, std::set<int> exclude_scene_id)
{
	std::map<int, CrystalFbSceneCfg > ::iterator iter = m_scene_cfg.find(scene_group_id);
	if (iter == m_scene_cfg.end())
	{
		return NULL;
	}

	std::vector<CrystalFbSceneCfg::ItemCfg> group_cfg = iter->second.group_cfg;

	int rate_count = CrystalFbSceneCfg::MAX_RATE_COUNT;

	if (exclude_scene_id.size() > 0)
	{
		for (int i = 0; i < (int)group_cfg.size(); ++i)
		{
			if (exclude_scene_id.find(group_cfg[i].scene_id) != exclude_scene_id.end())
			{
				rate_count -= group_cfg[i].rate;
				group_cfg[i].rate = 0;
			}
		}
	}

	if (rate_count <= 0)
	{
		return NULL;
	}

	int r = rand() % rate_count;
	for (int i = 0; i < (int)group_cfg.size(); ++i)
	{
		if (r < group_cfg[i].rate)
		{
			this->RefreshPos(group_cfg[i]);

			static CrystalFbSceneCfg::ItemCfg  ret;
			ret = group_cfg[i];			//因为group_cfg经过排除参数（exclude_scene_id）后的局部变量配置，所以要返回指针弄成静态的
			return &ret;
		}

		r -= group_cfg[i].rate;
	}

	return NULL;
}

Posi RoleCrystalFbConfig::GetRandPosi(int pos_group_id)
{
	std::map<int, CrystalFbPosCfg > ::iterator iter = m_pos_cfg.find(pos_group_id);
	if (iter == m_pos_cfg.end())
	{
		return  Posi(0, 0);
	}

	std::vector<CrystalFbPosCfg::ItemCfg> group_cfg = iter->second.group_cfg;

	int r = rand() % CrystalFbPosCfg::MAX_RATE_COUNT;
	for (int i = 0; i < (int)group_cfg.size(); ++i)
	{
		if (r < group_cfg[i].rate)
		{
			return group_cfg[i].birth_pos;
		}

		r -= group_cfg[i].rate;
	}

	return Posi(0, 0);
}

const CrystalFbBoxCfg::ItemCfg * RoleCrystalFbConfig::GetRandBoxCfg(int rand_group_id)
{
	std::map<int, CrystalFbBoxCfg > ::iterator iter = m_box_cfg.find(rand_group_id);
	if (iter != m_box_cfg.end())
	{
		int r = rand() % CrystalFbBoxCfg::MAX_RATE_COUNT;

		for (int i = 0; i < (int)iter->second.group_cfg.size(); ++i)
		{
			if (r < iter->second.group_cfg[i].rate)
			{
				return &iter->second.group_cfg[i];
			}

			r -= iter->second.group_cfg[i].rate;
		}
	}

	return NULL;
}

std::vector<ItemConfigData>  RoleCrystalFbConfig::GetRandBoxReward(std::vector<int> reward_group_ids)
{
	std::vector<ItemConfigData> ret;

	for (int k = 0; k < (int)reward_group_ids.size(); ++k)
	{
		std::map<int, CrystalFbRewardCfg > ::iterator iter = m_rewards_cfg.find(reward_group_ids[k]);
		if (iter == m_rewards_cfg.end())
		{
			return ret;
		}

		int r = rand() % CrystalFbRewardCfg::MAX_RATE_COUNT;

		for (int i = 0; i < (int)iter->second.group_cfg.size(); ++i)
		{
			if (r < iter->second.group_cfg[i].rate)
			{
				ret.push_back(iter->second.group_cfg[i].reward);
				break;
			}

			r -= iter->second.group_cfg[i].rate;
		}
	}

	return ret;
}

const ItemConfigData * RoleCrystalFbConfig::GetRandNpcReward(int reward_group_ids)
{
	std::map<int, CrystalFbRewardCfg > ::iterator iter = m_rewards_cfg.find(reward_group_ids);
	if (iter == m_rewards_cfg.end())
	{
		return NULL;
	}
	int weight_count = 0;
	for (std::vector<CrystalFbRewardCfg::ItemCfg>::iterator it2 = iter->second.group_cfg.begin();it2 != iter->second.group_cfg.end(); it2++)
	{
		weight_count += it2->rate;
	}

	if (weight_count <= 0)return NULL;

	int r = rand() % weight_count;
	int cumulative_num = 0;
	for (int i = 0; i < (int)iter->second.group_cfg.size(); ++i)
	{
		cumulative_num += iter->second.group_cfg[i].rate;
		if (r < cumulative_num)
		{
			return &(iter->second.group_cfg[i].reward);
		}
	}

	return NULL;
}

int RoleCrystalFbConfig::GetMaxRewardNum(int reward_group_id)
{
	std::map<int, CrystalFbRewardCfg > ::iterator iter = m_rewards_cfg.find(reward_group_id);
	if (iter == m_rewards_cfg.end())
	{
		return 0;
	}

	return  (int)iter->second.group_cfg.size();
}

int RoleCrystalFbConfig::GetRandNum(int num_group_id)
{
	std::map<int, CrystalFbRandNumCfg > ::iterator iter = m_rand_num_cfg.find(num_group_id);
	if (iter != m_rand_num_cfg.end())
	{
		int r = rand() % CrystalFbSceneCfg::MAX_RATE_COUNT;

		for (int i = 0; i < (int)iter->second.group_cfg.size(); ++i)
		{
			if (r < iter->second.group_cfg[i].rate)
			{
				return  iter->second.group_cfg[i].box_num;
			}

			r -= iter->second.group_cfg[i].rate;
		}
	}

	return 0;
}

const CrystalFbOtherCfg & RoleCrystalFbConfig::GetCrystalFbOtherCfg()
{
	return m_other_cfg;
}

int RoleCrystalFbConfig::GetUseItemLimitTimes(int use_limit_group_id, int use_type)
{
	std::map<int, CrystalFbUseItemLimitCfg>::iterator iter = m_use_item_limit_cfg.find(use_limit_group_id);
	if (iter == m_use_item_limit_cfg.end())
	{
		return MAX_INT;
	}

	std::map<int, int>::iterator use_limit_iter = iter->second.use_type_to_limit_times.find(use_type);
	if (use_limit_iter == iter->second.use_type_to_limit_times.end())
	{
		return MAX_INT;
	}

	return use_limit_iter->second;
}

int RoleCrystalFbConfig::IsLimitTimesUseType(int use_limit_group_id, int use_type)
{
	std::map<int, CrystalFbUseItemLimitCfg>::iterator iter = m_use_item_limit_cfg.find(use_limit_group_id);
	if (iter == m_use_item_limit_cfg.end())
	{
		return false;
	}

	std::map<int, int>::iterator use_limit_iter = iter->second.use_type_to_limit_times.find(use_type);
	if (use_limit_iter == iter->second.use_type_to_limit_times.end())
	{
		return false;
	}

	return true;
}

const CrystalFbMonsterCfg::ItemCfg * RoleCrystalFbConfig::GetCrystalFbMonsterCfg(int monster_group_id)
{
	std::map<int, CrystalFbMonsterCfg > ::iterator iter = m_monster_cfg.find(monster_group_id);
	if (iter != m_monster_cfg.end())
	{
		int r = rand() % CrystalFbMonsterCfg::MAX_RATE_COUNT;

		for (int i = 0; i < (int)iter->second.group_cfg.size(); ++i)
		{
			if (r < iter->second.group_cfg[i].rate)
			{
				return  &iter->second.group_cfg[i];
			}

			r -= iter->second.group_cfg[i].rate;
		}
	}

	return NULL;
}

std::vector<int> RoleCrystalFbConfig::GetMonsterDropId(int monster_npc_id, int play_times, int * village_id/* = NULL*/, int * prestige_num/* = NULL*/, int *add_exp/* = NULL*/, std::vector<ItemConfigData> * reward_list/* = NULL*/)
{
	std::vector<int> ret;
	std::map<int, CrystalFbMonsterDropCfg>::iterator iter = m_monster_drop_cfg.find(monster_npc_id);
	if (iter == m_monster_drop_cfg.end())
	{
		return ret;
	}

	int index = this->GetRewardColumn(play_times) - 1;
	if (index < 0 || index >= (int)iter->second.drop_id_list_group.size())
	{
		return ret;
	}

	if (village_id != NULL) *village_id = iter->second.village_id;
	if (village_id != NULL) *prestige_num = iter->second.prestige_num;
	if (add_exp != NULL) *add_exp = iter->second.exp;
	if (reward_list != NULL)
	{
		reward_list->clear();
		reward_list->assign(iter->second.reward_item.begin(), iter->second.reward_item.end());
	}

	return iter->second.drop_id_list_group[index];
}

int RoleCrystalFbConfig::GetRewardColumn(int play_times)
{
	for (std::map<int, int> ::iterator iter = m_play_times_to_reward_column.begin(); iter != m_play_times_to_reward_column.end(); ++iter)
	{
		if (play_times <= iter->first)
		{
			return iter->second;
		}
	}

	return 0;
}

int RoleCrystalFbConfig::GetReawrdAttColumn(int ARG_OUT * out_play_times)
{
	int index = 1;
	for (std::map<int, int> ::iterator iter = m_play_times_to_reward_column.begin(); iter != m_play_times_to_reward_column.end(); ++iter)
	{
		if (index++ > 1)
		{
			if (NULL != out_play_times)
			{
				*out_play_times = iter->first;
			}
			return iter->second;
		}
	}

	return 0;
}

const CrystalFbNPCRewardCfg * RoleCrystalFbConfig::GetNpcRewardCfg(int npc_id, int option_index)
{
	if (option_index < 0 || option_index >= 2)return NULL;
	std::map<int, CrystalFbNPCRewardListCfg>::iterator it = m_npc_reward_cfg_map.find(npc_id);
	if (it != m_npc_reward_cfg_map.end())
	{
		return &it->second.reward_list[option_index];
	}

	return NULL;
}

const CrystalFbNPCRewardListCfg * RoleCrystalFbConfig::GetNpcEventCfg(int npc_id)
{
	std::map<int, CrystalFbNPCRewardListCfg>::iterator it =  m_npc_reward_cfg_map.find(npc_id);
	if (it != m_npc_reward_cfg_map.end())
	{
		return &it->second;
	}
	return NULL;
}

int RoleCrystalFbConfig::GetEventNpcCfg(int group_id,int rand_num, std::vector<Event_NPC_cfg>* get_npc_list, std::vector<Event_NPC_cfg>*has_create_npc_vec)
{
	if (get_npc_list == NULL)return 0;
	std::map<int, std::vector<Event_NPC_cfg> >::iterator it = m_event_npc_cfg_map.find(group_id);
	std::map<int, int>::iterator it2 = m_event_npc_cfg_weight_map.find(group_id);
	int real_count = 0;
	if (it != m_event_npc_cfg_map.end() && it2 != m_event_npc_cfg_weight_map.end())
	{
		std::vector<Event_NPC_cfg> node_vec;
		node_vec.assign(it->second.begin(),it->second.end());
		int weight = it2->second;//记录总权重
		//处理重复NPC
		if (has_create_npc_vec != NULL)
		{
			int size_num = (int)has_create_npc_vec->size();
			std::vector<Event_NPC_cfg>::iterator same_it2 = has_create_npc_vec->begin();//把需要取出的一个一个删除
			for(int size_vec = 0; size_vec < size_num && size_vec < 100;size_vec++)
			{
				for (std::vector<Event_NPC_cfg>::iterator same_it = node_vec.begin(); same_it != node_vec.end(); same_it++)
				{
					if (same_it->npc_id == same_it2->npc_id)
					{
						weight -= same_it->rate;
						node_vec.erase(same_it);
						same_it2++;
						//扣去对应权重
						break;
					}
				}
			}

		}
		
		for (int i = 0; i < rand_num && i < 100;i++)
		{
			if ((int)node_vec.size() <= 0 || real_count >= rand_num)return real_count;
			if (weight <= 0)return real_count;
			int rand_dis = rand() % weight;
			int cum_count = 0;
			for (std::vector<Event_NPC_cfg>::iterator itor = node_vec.begin();itor != node_vec.end();itor++)
			{
				cum_count += itor->rate;
				if (cum_count > rand_dis)
				{
					(*get_npc_list).push_back(*itor);
					weight -= itor->rate;
					node_vec.erase(itor);
					real_count++;
					break;
				}
			}
		}
	}
	return real_count;
}

const BuffListConfig * RoleCrystalFbConfig::GetBuffId(int buff_id)
{
	std::map<int, BuffListConfig>::iterator it = m_buff_id_list_map.find(buff_id);
	if (it != m_buff_id_list_map.end())
	{
		return &it->second;
	}
	return NULL;
}

int RoleCrystalFbConfig::RandNPCShopItemList(int npc_id,NpcShopItem but_list[MAX_ROLE_CRYSTALFB_BUY_ITEM_COUNT])
{
	std::map<int, std::vector<NpcShopConfig> >::iterator it = m_npc_shop_list_map.find(npc_id);
	int real_item_count = 0;
	if (it != m_npc_shop_list_map.end())
	{
		for (std::vector<NpcShopConfig>::iterator it2 = it->second.begin(); it2 != it->second.end() && real_item_count < MAX_ROLE_CRYSTALFB_BUY_ITEM_COUNT; it2++)
		{
			int dis_rate = rand() % 10000;
			if (dis_rate < it2->rate)
			{
				but_list[real_item_count].item.item_id = it2->commodity.item_id;
				but_list[real_item_count].item.num = it2->commodity.num;
				but_list[real_item_count].item.is_bind = it2->commodity.is_bind ? 1 : 0;
				but_list[real_item_count].item.money_type = it2->money_type;
				but_list[real_item_count].item.price = it2->price;
				but_list[real_item_count].num = it2->buy_times;
				but_list[real_item_count].item.discount = it2->discount;
				real_item_count += 1;
			}
		}
	}
	return real_item_count;
}

const GatherNpcConfig * RoleCrystalFbConfig::GetGatherNpcCfg(int npc_id)
{
	std::map<int, GatherNpcConfig>::iterator it = m_gather_npc_cfg_map.find(npc_id);
	if (it != m_gather_npc_cfg_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const int RoleCrystalFbConfig::GetWorkshopItemWeightCfg(int group_id)
{
	std::map<int, int>::iterator it = m_workshop_wight_map.find(group_id);
	if (it != m_workshop_wight_map.end())
	{
		return it->second;
	}
	return 0;
}

const std::vector<WorkshopItemConfig> * RoleCrystalFbConfig::GetWorkshopItemCfg(int group_id)
{
	std::map<int, std::vector<WorkshopItemConfig> >::iterator it = m_workshop_item_cfg_map.find(group_id);
	if (it != m_workshop_item_cfg_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const BuyEnergyCfg * RoleCrystalFbConfig::GetBuyEnergyCfg(int buy_times)
{
	std::map<int, BuyEnergyCfg>::iterator it = m_buy_energy_cfg.find(buy_times);
	if(it == m_buy_energy_cfg.end()) return NULL;

	return &it->second;
}

int RoleCrystalFbConfig::GetExpCfg(int role_level)
{
	if (role_level <= 0 || role_level > MAX_ROLE_LEVEL) return 0;

	return m_exp_cfg[role_level - 1];
}

const bool RoleCrystalFbConfig::IsItemNotice(int item_id)
{
	for (std::vector<int>::const_iterator it = m_other_cfg.item_notice_vec.begin(); it != m_other_cfg.item_notice_vec.end(); ++it)
	{
		if (item_id == *it)
		{
			return true;
		}
	}
	return false;
}

const PowerUpCfg * RoleCrystalFbConfig::GetPowerUpCfg(int adv_level)
{
	if (adv_level < 0 || adv_level >= PROFESSION_BASE)return NULL;
	return power_up_list[adv_level];
}

int RoleCrystalFbConfig::GetFindExpCfg(int role_level)
{
	if (role_level <= 0 || role_level > MAX_ROLE_LEVEL) return 0;

	std::map<int, int>::iterator it = m_find_energy.find(role_level);
	if (it != m_find_energy.end())
	{
		return it->second;
	}

	return 0;
}


int RoleCrystalFbConfig::InitLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int normal_fb_num = 0;
	std::map<int, int> fb_type_to_next_fb_index;
	int next_seq = 0;
	while (NULL != root_element)
	{
		RoleCrystalFbLevelCfg cfg;
		if (!GetSubNodeValue(root_element, "seq", cfg.seq) || cfg.seq != next_seq)
		{
			return -1;
		}
		next_seq += 1;

		if (!GetSubNodeValue(root_element, "consume_item", cfg.comsume_item_id) || ITEMPOOL->GetItem(cfg.comsume_item_id) == NULL)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "type", cfg.type) || cfg.type < ROLE_CRYSTAL_FB_TYPE_NORMAL || cfg.type >= ROLE_CRYSTAL_FB_TYPE_MAX)
		{
			return -3;
		}

		int & next_fb_index = fb_type_to_next_fb_index[cfg.type];
		if (!GetSubNodeValue(root_element, "fb_index", cfg.fb_index) || cfg.fb_index != next_fb_index)
		{
			return -4;
		}
		next_fb_index += 1;


		if (ROLE_CRYSTAL_FB_TYPE_XI_YOU == cfg.type)
		{
			if (!GetSubNodeValue(root_element, "limit", cfg.limit_times))
			{
				return -5;
			}

			if (next_fb_index > CRYSTAL_FB_XI_YOU_MAX_LEVEL)
			{
				return -501;
			}

			TiXmlElement * first_kill_reward_list_element = root_element->FirstChildElement("first_kill_reward_list");
			if (NULL != first_kill_reward_list_element)
			{
				TiXmlElement * first_kill_reward_element = first_kill_reward_list_element->FirstChildElement("first_kill_reward");
				while (NULL != first_kill_reward_element)
				{
					ItemConfigData first_reward;
					if (!first_reward.ReadConfig(first_kill_reward_element))
					{
						return -502;
		}
					cfg.first_kill_reward_vec.push_back(first_reward);
					first_kill_reward_element = first_kill_reward_element->NextSiblingElement();
				}
			}
		}
		else if (ROLE_CRYSTAL_FB_TYPE_NORMAL == cfg.type)
		{
			normal_fb_num += 1;
		}

		if (!GetSubNodeValue(root_element, "min_level", cfg.need_role_level))
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "scene_id", cfg.last_layer_scene_id))
		{
			return -7;
		}


		if (!GetSubNodeValue(root_element, "boss_id", cfg.boss_npc_id))
		{
			return -8;
		}

		if (!GetSubNodeValue(root_element, "boss_layer_sceneborn_group_id", cfg.boss_layer_sceneborn_group_id))
		{
			return -__LINE__;
		}

		if (!GetSubNodeValue(root_element, "x", cfg.boss_pos.x) || cfg.boss_pos.x < 0)
		{
			return -9;
		}

		if (!GetSubNodeValue(root_element, "y", cfg.boss_pos.y) || cfg.boss_pos.y < 0)
		{
			return -10;
		}
		int _dir = 0;
		if (!GetSubNodeValue(root_element, "dir_y", _dir))
		{
			return -101;
		}
		cfg.monster_dir = (Dir)_dir;
		if (!GetSubNodeValue(root_element, "box_group_id", cfg.box_group_id))
		{
			return -11;
		}

		if (!GetSubNodeValue(root_element, "box_num", cfg.box_num_group_id))
		{
			return -12;
		}

		if (!GetSubNodeValue(root_element, "monster_group_id", cfg.monster_id_group_id))
		{
			return -13;
		}

		if (!GetSubNodeValue(root_element, "monster_num", cfg.monster_num_group_id))
		{
			return -14;
		}

		if (!GetSubNodeValue(root_element, "item_num", cfg.use_item_limit_group_id))
		{
			return -15;
		}

		if (!GetSubNodeValue(root_element, "event_num", cfg.event_num))
		{
			return -16;
		}

		if (!GetSubNodeValue(root_element, "event_group_id", cfg.event_group_id))
		{
			return -17;
		}

		if (!GetSubNodeValue(root_element, "recover_npc_id", cfg.recover_npc_id))
		{
			return -18;
		}

		if (!GetSubNodeValue(root_element, "recover_npc_dir", cfg.recover_npc_dir))
		{
			return -19;
		}

		if (!GetSubNodeValue(root_element, "recover_npc_x", cfg.recover_npc_pos.x))
		{
			return -20;
		}

		if (!GetSubNodeValue(root_element, "recover_npc_y", cfg.recover_npc_pos.y))
		{
			return -21;
		}

		if (!GetSubNodeValue(root_element, "recommend_score", cfg.recommend_score) || cfg.recommend_score < 0)
		{
			return -30;
		}

		int is_fly_up_fb = 0;
		if (!GetSubNodeValue(root_element, "is_feisheng", is_fly_up_fb) || is_fly_up_fb < 0)
		{
			return -31;
		}
		cfg.is_fly_up_fb = (0 != is_fly_up_fb) ? true : false;

		TiXmlElement *formula_drop_group_element = root_element->FirstChildElement("formula_drop_group");
		if(formula_drop_group_element && formula_drop_group_element->FirstChild())
		{
			int formula_drop_group = 0;
			if (!GetNodeValue(formula_drop_group_element, formula_drop_group))
			{
				return -22;
			}

			cfg.formula_drop_group = formula_drop_group;
		}

		if (!GetSubNodeValue(root_element, "first_reward_id", cfg.first_poker_reward_group_id) || cfg.first_poker_reward_group_id <= 0)
		{
			return -123;
		}
		int poker_reward_group_id = 0;
		if (!GetSubNodeValue(root_element, "reward_group_id_0", poker_reward_group_id) || poker_reward_group_id <= 0)
		{
			return -124;
		}
		cfg.poker_reward_group_vec.push_back(poker_reward_group_id);
		if (!GetSubNodeValue(root_element, "reward_group_id_1", poker_reward_group_id) || poker_reward_group_id <= 0)
		{
			return -125;
		}
		cfg.poker_reward_group_vec.push_back(poker_reward_group_id);
		if (!GetSubNodeValue(root_element, "reward_group_id_2", poker_reward_group_id) || poker_reward_group_id <= 0)
		{
			return -126;
		}
		cfg.poker_reward_group_vec.push_back(poker_reward_group_id);


		TiXmlElement *  drop_list_ele = root_element->FirstChildElement("drop_list_4");
		if (NULL != drop_list_ele)
		{
			TiXmlElement *node_element = drop_list_ele->FirstChildElement("dropid");
			while (node_element && node_element->FirstChild())
			{
				int drop_id = 0;
				if (!GetNodeValue(node_element, drop_id))
				{
					return -23;
				}

				cfg.drop_list_vec.push_back(drop_id);
				node_element = node_element->NextSiblingElement();
			}
		}

		std::set<int> check_repeat_scene_id;
		for (int i = 0; i < CRYSTAL_FB_MAX_LAYER - 1; ++i)
		{
			RoleCrystalFbLevelCfg::LayerCfg item_cfg;

			char scene_group[64] = { 0 };

			sprintf(scene_group, "scene_group_%d", i + 1);
			if (!GetSubNodeValue(root_element, scene_group, item_cfg.scene_id_group) || item_cfg.scene_id_group == 0)
			{
				break;
			}

			cfg.layer_cfg.push_back(item_cfg);

			std::map<int, CrystalFbSceneCfg>::iterator scene_iter = m_scene_cfg.find(item_cfg.scene_id_group);
			if (scene_iter == m_scene_cfg.end())
			{
				return -104;		//没有这个场景组
			}

			for (std::vector<CrystalFbSceneCfg::ItemCfg>::iterator scene_item_iter = scene_iter->second.group_cfg.begin(); scene_item_iter != scene_iter->second.group_cfg.end(); ++scene_item_iter)
			{
				check_repeat_scene_id.insert(scene_item_iter->scene_id);
			}
		}

		if (check_repeat_scene_id.size() < cfg.layer_cfg.size())
		{
			//这关 用到场景加起来scene_id 不足
			return -105;
		}

		if (check_repeat_scene_id.count(cfg.last_layer_scene_id) != 0)
		{			
			//boss 层scene_id 不能在场景组里。不然前面有可能被用了
			return -106;
		}

		m_level_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (normal_fb_num >= CRYSTAL_FB_NORMAL_NUM)
	{
		// 普通秘境数量超过上限，需要改代码
		return -999;
	}

	return 0;
}

int RoleCrystalFbConfig::InitExpCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -999;

	int next_level = 1;
	while (NULL != root_element)
	{
		int level = 0;
		if (!GetSubNodeValue(root_element, "level", level) || level != next_level || level > MAX_ROLE_LEVEL) return -1;
		next_level++;

		int exp = 0;
		if (!GetSubNodeValue(root_element, "exp", exp) || exp < 0) return -2;

		m_exp_cfg[level - 1] = exp;
		root_element = root_element->NextSiblingElement();
	}
	if (next_level != MAX_ROLE_LEVEL + 1)
	{
		return -11;
	}

	return 0;
}

int RoleCrystalFbConfig::InitBoxCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "box_group_id", group_id))
		{
			return -1;
		}

		CrystalFbBoxCfg & cfg = m_box_cfg[group_id];
		CrystalFbBoxCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "gather_id", item_cfg.treasure_chest_id))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "consume_item", item_cfg.open_need_item_id) || ITEMPOOL->GetItem(item_cfg.open_need_item_id) == NULL)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "consume_num", item_cfg.open_need_item_num))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "encounter_monster_group_id", item_cfg.encounter_monster_group_id))
		{
			return -6;
		}

		GetSubNodeValue(root_element, "box_x", item_cfg.treasure_chest_posi.x);
		GetSubNodeValue(root_element, "box_y", item_cfg.treasure_chest_posi.y);

		if (item_cfg.encounter_monster_group_id == 0)
		{
			for (int i = 0; i < MAX_ROLE_CRYSTAL_FB_REWARD_COLUMN; ++i)
			{
				char add_coin_str[128] = { 0 };
				SNPRINTF(add_coin_str, sizeof(add_coin_str) - 1, "coin_%d", i + 1);
				int add_coin = 0;
				if (!GetSubNodeValue(root_element, add_coin_str, add_coin) || add_coin < 0)
				{
					return -7;
				}
				item_cfg.add_coin.push_back(add_coin);

				memset(add_coin_str, 0, sizeof(add_coin_str));
				SNPRINTF(add_coin_str, sizeof(add_coin_str) - 1, "captain_coin_%d", i + 1);
				int captain_coin = 0;
				if (!GetSubNodeValue(root_element, add_coin_str, captain_coin) || captain_coin < 0)
				{
					return -71;
				}
				item_cfg.captain_coin.push_back(captain_coin);

				char reward_group_str[128] = { 0 };
				SNPRINTF(reward_group_str, sizeof(reward_group_str) - 1, "reward_group_%d", i + 1);
				TiXmlElement * open_box_reward = root_element->FirstChildElement(reward_group_str);
				std::vector<int> reward_column;
				if (NULL != open_box_reward)
				{
					TiXmlElement *node_element = open_box_reward->FirstChildElement("node");
					while (node_element)
					{
						int reward_id = 0;
						if (!GetNodeValue(node_element, reward_id))
						{
							return -20001;
						}

						reward_column.push_back(reward_id);
						node_element = node_element->NextSiblingElement();
					}
				}

				item_cfg.reward_column.push_back(reward_column);
				memset(reward_group_str, 0, sizeof(reward_group_str));
				SNPRINTF(reward_group_str, sizeof(reward_group_str) - 1, "captainreward_group_%d", i + 1);
				TiXmlElement * open_box_captain_reward = root_element->FirstChildElement(reward_group_str);
				reward_column.clear();
				if (NULL != open_box_captain_reward)
				{
					TiXmlElement *node_element = open_box_captain_reward->FirstChildElement("node");
					while (node_element)
					{
						int reward_id = 0;
						if (!GetNodeValue(node_element, reward_id))
						{
							return -20001;
						}

						reward_column.push_back(reward_id);
						node_element = node_element->NextSiblingElement();
					}
				}

				item_cfg.captain_reward_column.push_back(reward_column);
			}

		}

		item_cfg.seq = (int)cfg.group_cfg.size();

		cfg.group_cfg.push_back(item_cfg);
		m_all_box_gather_id.insert(item_cfg.treasure_chest_id);
		root_element = root_element->NextSiblingElement();
	}

	if (m_box_cfg.size() == 0)
	{
		return -10085;
	}

	for (std::map<int, CrystalFbBoxCfg >::iterator iter = m_box_cfg.begin(); iter != m_box_cfg.end(); ++iter)
	{
		int count = 0;
		for (int i = 0; i < (int)iter->second.group_cfg.size(); ++i)
		{
			count += iter->second.group_cfg[i].rate;
		}

		if (count > CrystalFbBoxCfg::MAX_RATE_COUNT)
		{
			return -10086;
		}
	}

	return 0;
}

int RoleCrystalFbConfig::InitRandNumCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "id", group_id))
		{
			return -1;
		}

		CrystalFbRandNumCfg & cfg = m_rand_num_cfg[group_id];
		CrystalFbRandNumCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "box_num_group", item_cfg.box_num))
		{
			return -2;
		}

		cfg.group_cfg.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_rand_num_cfg.size() == 0)
	{
		return -10085;
	}

	for (std::map<int, CrystalFbRandNumCfg >::iterator iter = m_rand_num_cfg.begin(); iter != m_rand_num_cfg.end(); ++iter)
	{
		int count = 0;
		for (int i = 0; i < (int)iter->second.group_cfg.size(); ++i)
		{
			count += iter->second.group_cfg[i].rate;
		}

		if (count != CrystalFbRandNumCfg::MAX_RATE_COUNT)
		{
			return -10086;
		}
	}

	return 0;
}

int RoleCrystalFbConfig::InitRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "reward_id", group_id))
		{
			return -1;
		}

		CrystalFbRewardCfg & cfg = m_rewards_cfg[group_id];
		CrystalFbRewardCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}

		TiXmlElement * box_reward = root_element->FirstChildElement("reward_item");
		if (!item_cfg.reward.ReadConfig(box_reward))
		{
			return -104;
		}

		cfg.group_cfg.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RoleCrystalFbConfig::InitPosCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	std::map<int, std::set<int> >check_repeat;
	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "born_group_id", group_id))
		{
			return -1;
		}

		CrystalFbPosCfg& cfg = m_pos_cfg[group_id];
		CrystalFbPosCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "born_x", item_cfg.birth_pos.x))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "born_y", item_cfg.birth_pos.y))
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "door_x", item_cfg.transport_pos.x))
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "door_y", item_cfg.transport_pos.y))
		{
			return -6;
		}

		cfg.group_cfg.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_pos_cfg.size() == 0)
	{
		return -10085;
	}

	for (std::map<int, CrystalFbPosCfg >::iterator iter = m_pos_cfg.begin(); iter != m_pos_cfg.end(); ++iter)
	{
		int count = 0;
		for (int i = 0; i < (int)iter->second.group_cfg.size(); ++i)
		{
			count += iter->second.group_cfg[i].rate;
		}

		if (count != CrystalFbPosCfg::MAX_RATE_COUNT)
		{
			return -10086;
		}
	}

	return 0;
}

int RoleCrystalFbConfig::InitSceneCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	std::map<int, std::set<int> >check_repeat;
	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "scene_group_id", group_id))
		{
			return -1;
		}

		CrystalFbSceneCfg & cfg = m_scene_cfg[group_id];
		CrystalFbSceneCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}


		if (!GetSubNodeValue(root_element, "scene_id", item_cfg.scene_id))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "sceneborn_group_id", item_cfg.sceneborn_group_id))
		{
			return -4;
		}

		if (check_repeat[group_id].find(item_cfg.scene_id) != check_repeat[group_id].end())
		{
			return -89;
		}

		cfg.group_cfg.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_scene_cfg.size() == 0)
	{
		return -10085;
	}

	for (std::map<int, CrystalFbSceneCfg >::iterator iter = m_scene_cfg.begin(); iter != m_scene_cfg.end(); ++iter)
	{
		int count = 0;
		for (int i = 0; i < (int)iter->second.group_cfg.size(); ++i)
		{
			count += iter->second.group_cfg[i].rate;
		}

		if (count != CrystalFbSceneCfg::MAX_RATE_COUNT)
		{
			return -10086;
		}
	}

	return 0;
}

int RoleCrystalFbConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "daily_limit", m_other_cfg.all_level_limit_times))
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "initial_power", m_other_cfg.initial_power))
	{
		return -21;
	}

	if (!GetSubNodeValue(root_element, "use_power", m_other_cfg.use_power))
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "max_power", m_other_cfg.max_power))
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "recovery_power", m_other_cfg.recovery_power))
	{
		return -4;
	}

	if (!GetSubNodeValue(root_element, "recovery_time", m_other_cfg.recovery_time))
	{
		return -5;
	}

	if (!GetSubNodeValue(root_element, "extra_power_limit", m_other_cfg.extra_power_limit))
	{
		return -6;
	}

	if (!GetSubNodeValue(root_element, "limit_type", m_other_cfg.limit_type) || m_other_cfg.limit_type <= CONVERT_LIMIT_TYPE_NONE || m_other_cfg.limit_type > CONVERT_LIMIT_TYPE_MONTHLY)
	{
		return -7;
	}

	if (!GetSubNodeValue(root_element, "buy_times", m_other_cfg.buy_times) || m_other_cfg.buy_times <= 0)
	{
		return -8;
	}

	if (!GetSubNodeValue(root_element, "pass_time_max", m_other_cfg.normal_fb_day_pass_time_max) || m_other_cfg.normal_fb_day_pass_time_max <= 0)
	{
		return -9;
	}

	if (!GetSubNodeValue(root_element, "extra_buy_times", m_other_cfg.extra_buy_times) || m_other_cfg.extra_buy_times < 0)
	{
		return -10;
	}

	if (!GetSubNodeValue(root_element, "extra_buy_consume", m_other_cfg.extra_buy_consume) || m_other_cfg.extra_buy_consume <= 0)
	{
		return -11;
	}

	if (!GetSubNodeValue(root_element, "pass_time_max", m_other_cfg.pass_time_max) || m_other_cfg.pass_time_max <= 0)
	{
		return -12;
	}

	if (!GetSubNodeValue(root_element, "normal_reward_time", m_other_cfg.normal_reward_time) || m_other_cfg.normal_reward_time <= 0)
	{
		return -13;
	}

	if (!GetSubNodeValue(root_element, "free_reward_time", m_other_cfg.free_reward_time) || m_other_cfg.free_reward_time <= 0)
	{
		return -14;
	}

	TiXmlElement * notice_list = root_element->FirstChildElement("notice");
	if (NULL != notice_list)
	{
		TiXmlElement *node_element = notice_list->FirstChildElement("node");
		while (node_element)
		{
			int notice_id = 0;
			if (!GetNodeValue(node_element, notice_id))
			{
				return -100;
			}

			m_other_cfg.item_notice_vec.push_back(notice_id);
			node_element = node_element->NextSiblingElement();
		}
	}

	return 0;
}

int RoleCrystalFbConfig::InitUseLimitCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "limit_item_group_id", group_id))
		{
			return -1;
		}

		CrystalFbUseItemLimitCfg & cfg = m_use_item_limit_cfg[group_id];

		int use_type = 0;
		if (!GetSubNodeValue(root_element, "use_type", use_type) )
		{
			return -2;
		}

		int limit_times = 0;
		if (!GetSubNodeValue(root_element, "xz_num", limit_times) || limit_times <= 0)
		{
			return -3;
		}

		cfg.use_type_to_limit_times[use_type] = limit_times;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RoleCrystalFbConfig::InitMonsterCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -10000;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "id", group_id))
		{
			return -1;
		}

		CrystalFbMonsterCfg & cfg = m_monster_cfg[group_id];
		CrystalFbMonsterCfg::ItemCfg item_cfg;

		if (!GetSubNodeValue(root_element, "rate", item_cfg.rate) || item_cfg.rate <= 0)
		{
			return -2;
		}


		if (!GetSubNodeValue(root_element, "box_num_group", item_cfg.monster_npc_id))
		{
			return -3;
		}


		cfg.group_cfg.push_back(item_cfg);
		root_element = root_element->NextSiblingElement();
	}

	if (m_monster_cfg.size() == 0)
	{
		return -10085;
	}

	for (std::map<int, CrystalFbMonsterCfg >::iterator iter = m_monster_cfg.begin(); iter != m_monster_cfg.end(); ++iter)
	{
		int count = 0;
		for (int i = 0; i < (int)iter->second.group_cfg.size(); ++i)
		{
			count += iter->second.group_cfg[i].rate;
		}

		if (count != CrystalFbMonsterCfg::MAX_RATE_COUNT)
		{
			return -10086;
		}
	}

	return 0;
}

int RoleCrystalFbConfig::InitMonsterDropCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		CrystalFbMonsterDropCfg cfg;
		if (!GetSubNodeValue(root_element, "box_num_group", cfg.monster_npc_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "village_id", cfg.village_id))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "prestige_num", cfg.prestige_num))
		{
			return -3;
		}

		for (int i = 0; i < MAX_ROLE_CRYSTAL_FB_REWARD_COLUMN; ++i)
		{
			char drop_list_str[128] = { 0 };
			SNPRINTF(drop_list_str, sizeof(drop_list_str) - 1, "drop_list_%d", i + 1);

			TiXmlElement *  drop_list_ele = root_element->FirstChildElement(drop_list_str);
			std::vector<int> drop_id_column;
			if (NULL != drop_list_ele)
			{
				TiXmlElement *node_element = drop_list_ele->FirstChildElement("dropid");
				while (node_element)
				{
					int drop_id = 0;
					if (!GetNodeValue(node_element, drop_id))
					{
						return -20001;
					}

					drop_id_column.push_back(drop_id);
					node_element = node_element->NextSiblingElement();
				}
			}

			cfg.drop_id_list_group.push_back(drop_id_column);
		}

		if (!GetSubNodeValue(root_element, "exp", cfg.exp))
		{
			return -6;
		}

		TiXmlElement * reward_list_element = root_element->FirstChildElement("reward_list");
		if (reward_list_element != NULL)
		{
			TiXmlElement * reward_element = reward_list_element->FirstChildElement("reward");
			while (reward_element != NULL)
			{
				ItemConfigData node_reward;
				if (!node_reward.ReadConfig(reward_element))
				{
					return -7;
				}
				cfg.reward_item.push_back(node_reward);
				reward_element = reward_element->NextSiblingElement();
			}
		}

		m_monster_drop_cfg[cfg.monster_npc_id] = cfg;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RoleCrystalFbConfig::InitPlayTimesToRewardColumnCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int last_play_times = 0;
	while (NULL != root_element)
	{
		int play_times = 0;
		if (!GetSubNodeValue(root_element, "time", play_times) || play_times <= last_play_times)
		{
			return -1;
		}
		last_play_times = play_times;

		int reward_column = 0;
		if (!GetSubNodeValue(root_element, "reward_index", reward_column))
		{
			return -2;
		}

		m_play_times_to_reward_column[play_times] = reward_column;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RoleCrystalFbConfig::InitEventGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	int last_event_group_id = 0;
	while (NULL != root_element)
	{
		Event_NPC_cfg node_cfg;
		if (!GetSubNodeValue(root_element, "event_group_id", node_cfg.event_group_id) || node_cfg.event_group_id <= 0 || last_event_group_id > node_cfg.event_group_id)
		{
			return -1;
		}
		
		if (last_event_group_id != node_cfg.event_group_id)
		{
			last_event_group_id = node_cfg.event_group_id;
			m_event_npc_cfg_weight_map[node_cfg.event_group_id] = 0;
		}
		if (!GetSubNodeValue(root_element, "npc_id", node_cfg.npc_id) || node_cfg.npc_id <= 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "rate", node_cfg.rate) || node_cfg.rate <= 0)
		{
			return -3;
		}
		
		if (!GetSubNodeValue(root_element, "npc_type", node_cfg.npc_type) ||
			node_cfg.npc_type < Event_NPC_cfg::CRYSTALFB_NPC_TYPE_0 || 
			node_cfg.npc_type >  Event_NPC_cfg::CRYSTALFB_NPC_TYPE_10)
		{
			return -4;
		}

		m_event_npc_cfg_map[node_cfg.event_group_id].push_back(node_cfg);
		m_event_npc_cfg_weight_map[node_cfg.event_group_id] += node_cfg.rate;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RoleCrystalFbConfig::InitEventListCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int npc_id = 0;
		if (!GetSubNodeValue(root_element, "npc_id", npc_id) || npc_id <= 0)
		{
			return -1;
		}
		CrystalFbNPCRewardListCfg &node_cfg = m_npc_reward_cfg_map[npc_id];
		
		int operation_id = 0;
		if (!GetSubNodeValue(root_element, "operation_id", operation_id) || operation_id <= 0 || operation_id > CrystalFbNPCRewardListCfg::MAX_OPERATION_INDEX)
		{
			return -2;
		}

		int reward_type = 0;
		if (!GetSubNodeValue(root_element, "reward_type", reward_type) || reward_type <= 0 || reward_type >= CrystalFbNPCRewardCfg::REWARD_TYPE_MAX)
		{
			return -3;
		}

		int reward_1 = 0;
		if (!GetSubNodeValue(root_element, "reward_1", reward_1) || reward_1 < 0)
		{
			return -4;
		}

		int reward_2 = 0;
		if (!GetSubNodeValue(root_element, "reward_2", reward_2) || reward_2 < 0)
		{
			return -5;
		}

		int reward_3 = 0;
		if (!GetSubNodeValue(root_element, "reward_3", reward_3) || reward_3 < 0)
		{
			return -6;
		}

		int is_delete = 0;
		if (!GetSubNodeValue(root_element, "is_delete", is_delete) || is_delete < 0)
		{
			return -7;
		}

		node_cfg.reward_list[operation_id - 1].npc_id = npc_id;
		node_cfg.reward_list[operation_id - 1].option_index = operation_id;
		node_cfg.reward_list[operation_id - 1].reward_type = reward_type;
		node_cfg.reward_list[operation_id - 1].reward_list[0] = reward_1;
		node_cfg.reward_list[operation_id - 1].reward_list[1] = reward_2;
		node_cfg.reward_list[operation_id - 1].reward_list[2] = reward_3;
		node_cfg.reward_list[operation_id - 1].is_delete = is_delete;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RoleCrystalFbConfig::InitShopListCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		NpcShopConfig node_cfg;

		if (!GetSubNodeValue(root_element, "shop_type", node_cfg.shop_type) || node_cfg.shop_type <= 0)
		{
			return -1;
		}
		
		if (!GetSubNodeValue(root_element, "index", node_cfg.index) || node_cfg.index < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "buy_times", node_cfg.buy_times) || node_cfg.buy_times < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "money_type", node_cfg.money_type) || node_cfg.money_type < 0 || node_cfg.money_type > NpcShopConfig::MONEY_TYPE_2)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "price", node_cfg.price) || node_cfg.price <= 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "rate", node_cfg.rate) || node_cfg.rate <= 0 || node_cfg.rate > 10000)
		{
			return -6;
		}

		if (!GetSubNodeValue(root_element, "discount", node_cfg.discount) || node_cfg.discount < 0 || node_cfg.discount > 100)
		{
			return -7;
		}

		TiXmlElement * shop_reward = root_element->FirstChildElement("reward_item");
		if (!node_cfg.commodity.ReadConfig(shop_reward))
		{
			return -8;
		}

		m_npc_shop_list_map[node_cfg.shop_type].push_back(node_cfg);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RoleCrystalFbConfig::InitBuffListCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		
		int buff_id = 0;
		if (!GetSubNodeValue(root_element, "buff_id", buff_id) || buff_id <= 0)
		{
			return -1;
		}
		BuffListConfig &node_cfg = m_buff_id_list_map[buff_id];
		node_cfg.buff_id = buff_id;
		if (!GetSubNodeValue(root_element, "type", node_cfg.type) || node_cfg.type < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "param_1", node_cfg.param_1) || node_cfg.param_1 < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "param_2", node_cfg.param_2) || node_cfg.param_2 < 0)
		{
			return -4;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RoleCrystalFbConfig::InitGatherNpcCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int npc_id = 0;
		if (!GetSubNodeValue(root_element, "npc_id", npc_id) || npc_id <= 0)
		{
			return -1;
		}
		GatherNpcConfig &node_cfg = m_gather_npc_cfg_map[npc_id];
		node_cfg.npc_id = npc_id;
		int reward_type = 0;
		if (!GetSubNodeValue(root_element, "reward_type", reward_type) || reward_type < 0 || reward_type >= GatherNpcConfig::REWARD_TYPE_MAX)
		{
			return -2;
		}

		int reward_1 = 0;
		if (!GetSubNodeValue(root_element, "reward_1", reward_1) || reward_1 < 0)
		{
			return -4;
		}

		int reward_2 = 0;
		if (!GetSubNodeValue(root_element, "reward_2", reward_2) || reward_2 < 0)
		{
			return -5;
		}

		int reward_3 = 0;
		if (!GetSubNodeValue(root_element, "reward_3", reward_3) || reward_3 < 0)
		{
			return -6;
		}


		node_cfg.npc_id = npc_id;
		node_cfg.reward_type = reward_type;
		node_cfg.reward_list[0] = reward_1;
		node_cfg.reward_list[1] = reward_2;
		node_cfg.reward_list[2] = reward_3;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RoleCrystalFbConfig::InitWrokShopItemCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int group_id = 0;
		if (!GetSubNodeValue(root_element, "group_id", group_id) || group_id <= 0)
		{
			return -1;
		}

		std::vector<WorkshopItemConfig> &node_cfg = m_workshop_item_cfg_map[group_id];
		int &weight = m_workshop_wight_map[group_id];

		WorkshopItemConfig node;
		node.group_id = group_id;
		
		if (!GetSubNodeValue(root_element, "item_id", node.item_id) || node.item_id < 0 || NULL == ITEMPOOL->GetItem(node.item_id))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "rate", node.rate) || node.rate < 0)
		{
			return -3;
		}

		weight += node.rate;

		if (weight > WorkshopItemConfig::MAX_RAND_WEIGHT_NUM)
		{
			return -4;
		}

		node_cfg.push_back(node);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RoleCrystalFbConfig::InitBuyEnergyCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	int next_buy_times = 1;
	while (NULL != root_element)
	{
		int buy_times = 0;
		if (!GetSubNodeValue(root_element, "buy_times", buy_times) || buy_times != next_buy_times || buy_times > m_other_cfg.buy_times)
		{
			return -1;
		}
		next_buy_times++;
		BuyEnergyCfg node_cfg;
		if (!GetSubNodeValue(root_element, "price", node_cfg.price) || node_cfg.price < 0)
		{
			return -2;
		}
		if (!GetSubNodeValue(root_element, "get_power", node_cfg.get_power) || node_cfg.get_power <= 0)
		{
			return -3;
		}

		m_buy_energy_cfg[buy_times] = node_cfg;
		root_element = root_element->NextSiblingElement();
	}
	if (m_buy_energy_cfg.size() != m_other_cfg.buy_times)
	{
		return -4;
	}

	return 0;
}

int RoleCrystalFbConfig::InitPowerUpCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	memset(power_up_list, NULL, sizeof(power_up_list));
	while (NULL != root_element)
	{
		PowerUpCfg node;
		if (!GetSubNodeValue(root_element, "job_id", node.job_num) || node.job_num < 0 || node.job_num >= PROFESSION_BASE)
		{
			return -1;
		}
		if (!GetSubNodeValue(root_element, "power_up", node.power_up) || node.power_up < 0)
		{
			return -2;
		}

		m_power_up_cfg.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	for (std::vector<PowerUpCfg>::iterator it = m_power_up_cfg.begin(); it != m_power_up_cfg.end(); it++)
	{
		if (it->job_num >= 0 && it->job_num < PROFESSION_BASE)
		{
			if (power_up_list[it->job_num] != NULL)continue;
			power_up_list[it->job_num] = &(*it);
		}
	}

	return 0;
}

int RoleCrystalFbConfig::InitEnergyFindCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -999;
	
	while (NULL != root_element)
	{
		int level = 0, exp = 0;

		if (!GetSubNodeValue(root_element, "level", level) || level <= 0 || level > MAX_ROLE_LEVEL) return -__LINE__;
		if (!GetSubNodeValue(root_element, "exp", exp) || exp < 0) return -__LINE__;

		m_find_energy[level] = exp;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}


void RoleCrystalFbConfig::RefreshPos(CrystalFbSceneCfg::ItemCfg ARG_OUT & cfg)
{
	std::map<int, CrystalFbPosCfg > ::iterator iter = m_pos_cfg.find(cfg.sceneborn_group_id);
	if (iter == m_pos_cfg.end())
	{
		return;
	}

	std::vector<CrystalFbPosCfg::ItemCfg> group_cfg = iter->second.group_cfg;

	int r = rand() % CrystalFbPosCfg::MAX_RATE_COUNT;
	for (int i = 0; i < (int)group_cfg.size(); ++i)
	{
		if (r < group_cfg[i].rate)
		{
			cfg.birth_pos = group_cfg[i].birth_pos;
			cfg.transport_pos = group_cfg[i].transport_pos;
			break;
		}

		r -= group_cfg[i].rate;
	}
}