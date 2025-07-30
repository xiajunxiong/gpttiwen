#include "randactivitymijingqiwenconfig.hpp"
#include "item/itempool.h"
#include "servercommon/rolecrystalfbdef.h"

RandActivityMiJingQiWenConfig::RandActivityMiJingQiWenConfig()
{
}

RandActivityMiJingQiWenConfig::~RandActivityMiJingQiWenConfig()
{
	std::vector<RoleCrystalFbLevelCfg>().swap(m_level_cfg);
	for (std::map<int, std::vector<new_Event_NPC_cfg> >::iterator it = m_event_npc_cfg_map.begin(); it != m_event_npc_cfg_map.end(); it++)
	{
		std::vector<new_Event_NPC_cfg>().swap(it->second);
	}
	for (std::map<int, std::vector<BuffListConfig> >::iterator it = m_buff_list_group.begin(); it != m_buff_list_group.end(); it++)
	{
		std::vector<BuffListConfig>().swap(it->second);
	}
	for (std::map<int, std::vector<NpcShopConfig> >::iterator it = m_npc_shop_list_map.begin(); it != m_npc_shop_list_map.end(); it++)
	{
		std::vector<NpcShopConfig>().swap(it->second);
	}
	for (std::map<int, std::vector<WorkshopItemConfig> >::iterator it = m_workshop_item_cfg_map.begin(); it != m_workshop_item_cfg_map.end(); it++)
	{
		std::vector<WorkshopItemConfig>().swap(it->second);
	}
	std::vector<MiJingQiWenFbLevelTypeCfg>().swap(m_level_to_seq_vec);
}

bool RandActivityMiJingQiWenConfig::Init(TiXmlElement *RootElement, std::string *err)
{
	PRE_LOAD_RA_CONFIG;

	LOAD_RA_CONFIG("scene_group", InitSceneCfg);		//先初始化场景组
	LOAD_RA_CONFIG("anecdote_configure", InitLevelCfg);
	LOAD_RA_CONFIG("box_group", InitBoxCfg);
	LOAD_RA_CONFIG("box_num", InitRandNumCfg);
	LOAD_RA_CONFIG("anecdote_gift_group", InitRewardCfg);
	LOAD_RA_CONFIG("other", InitOtherCfg);
	LOAD_RA_CONFIG("fb_expend_num", InitUseLimitCfg);
	LOAD_RA_CONFIG("monster_group", InitMonsterCfg);
	LOAD_RA_CONFIG("reward_proportion", InitPlayTimesToRewardColumnCfg);
	LOAD_RA_CONFIG("monster_dorp_list", InitMonsterDropCfg);
	LOAD_RA_CONFIG("born_group", InitPosCfg);
	LOAD_RA_CONFIG("anecdote_random_event", InitEventGroupCfg);
	LOAD_RA_CONFIG("anecdote_event_list", InitEventListCfg);
	LOAD_RA_CONFIG("anecdote_mall", InitShopListCfg);
	LOAD_RA_CONFIG("anecdote_buff_group", InitBuffListCfg);
	LOAD_RA_CONFIG("gather_reward", InitGatherNpcCfg);
	LOAD_RA_CONFIG("formula_drop_group", InitWrokShopItemCfg);
	LOAD_RA_CONFIG("level_group", InitMiJingQiWenLevelTypeCfg);

	return true;
}

const RoleCrystalFbLevelCfg * RandActivityMiJingQiWenConfig::GetRoleCrystalFbLevelCfg(int seq) const
{
	if (seq < 0 || seq >= (int)m_level_cfg.size())
	{
		return NULL;
	}

	return &m_level_cfg[seq];
}

const int RandActivityMiJingQiWenConfig::GetLayerNum(int seq)const
{
	if (seq < 0 || seq >= (int)m_level_cfg.size())
	{
		return 0;
	}

	return (int)m_level_cfg[seq].layer_cfg.size() + 1;		//随机层加上一个boss 固定层
}

const bool RandActivityMiJingQiWenConfig::IsCrystalFbBoxGather(int gather_id)const
{
	if (m_all_box_gather_id.find(gather_id) != m_all_box_gather_id.end())
	{
		return true;
	}

	return false;
}

const CrystalFbBoxCfg::ItemCfg * RandActivityMiJingQiWenConfig::GetBoxCfg(int group_id, int box_seq)const
{
	std::map<int, CrystalFbBoxCfg>::const_iterator iter = m_box_cfg.find(group_id);
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

//int RandActivityMiJingQiWenConfig::GetRandScene(int scene_group_id, std::set<int>  exclude_scene_id)
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

const CrystalFbSceneCfg::ItemCfg *  RandActivityMiJingQiWenConfig::GetRandSceneCfg(int scene_group_id, std::set<int> exclude_scene_id) const
{
	std::map<int, CrystalFbSceneCfg > ::const_iterator iter = m_scene_cfg.find(scene_group_id);
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

const Posi RandActivityMiJingQiWenConfig::GetRandPosi(int pos_group_id)const
{
	std::map<int, CrystalFbPosCfg > ::const_iterator iter = m_pos_cfg.find(pos_group_id);
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

const CrystalFbBoxCfg::ItemCfg * RandActivityMiJingQiWenConfig::GetRandBoxCfg(int rand_group_id)const
{
	std::map<int, CrystalFbBoxCfg > ::const_iterator iter = m_box_cfg.find(rand_group_id);
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

const std::vector<ItemConfigData>  RandActivityMiJingQiWenConfig::GetRandBoxReward(std::vector<int> reward_group_ids)const
{
	std::vector<ItemConfigData> ret;

	for (int k = 0; k < (int)reward_group_ids.size(); ++k)
	{
		std::map<int, CrystalFbRewardCfg > ::const_iterator iter = m_rewards_cfg.find(reward_group_ids[k]);
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

const ItemConfigData * RandActivityMiJingQiWenConfig::GetRandNpcReward(int reward_group_ids)const
{

	std::map<int, CrystalFbRewardCfg > ::const_iterator iter = m_rewards_cfg.find(reward_group_ids);
	if (iter == m_rewards_cfg.end())
	{
		return NULL;
	}
	int weight_count = 0;
	for (std::vector<CrystalFbRewardCfg::ItemCfg>::const_iterator it2 = iter->second.group_cfg.begin(); it2 != iter->second.group_cfg.end(); it2++)
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

const int RandActivityMiJingQiWenConfig::GetMaxRewardNum(int reward_group_id)const
{
	std::map<int, CrystalFbRewardCfg > ::const_iterator iter = m_rewards_cfg.find(reward_group_id);
	if (iter == m_rewards_cfg.end())
	{
		return 0;
	}

	return  (int)iter->second.group_cfg.size();
}

const int RandActivityMiJingQiWenConfig::GetRandNum(int num_group_id)const
{
	std::map<int, CrystalFbRandNumCfg >::const_iterator iter = m_rand_num_cfg.find(num_group_id);
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

const MiJingQiWenOtherCfg & RandActivityMiJingQiWenConfig::GetCrystalFbOtherCfg()const
{
	return m_other_cfg;
}

const int RandActivityMiJingQiWenConfig::GetUseItemLimitTimes(int use_limit_group_id, int use_type)const
{
	std::map<int, CrystalFbUseItemLimitCfg>::const_iterator iter = m_use_item_limit_cfg.find(use_limit_group_id);
	if (iter == m_use_item_limit_cfg.end())
	{
		return MAX_INT;
	}

	std::map<int, int>::const_iterator use_limit_iter = iter->second.use_type_to_limit_times.find(use_type);
	if (use_limit_iter == iter->second.use_type_to_limit_times.end())
	{
		return MAX_INT;
	}

	return use_limit_iter->second;
}

const int RandActivityMiJingQiWenConfig::IsLimitTimesUseType(int use_limit_group_id, int use_type)const
{
	std::map<int, CrystalFbUseItemLimitCfg>::const_iterator iter = m_use_item_limit_cfg.find(use_limit_group_id);
	if (iter == m_use_item_limit_cfg.end())
	{
		return false;
	}

	std::map<int, int>::const_iterator use_limit_iter = iter->second.use_type_to_limit_times.find(use_type);
	if (use_limit_iter == iter->second.use_type_to_limit_times.end())
	{
		return false;
	}

	return true;
}

const CrystalFbMonsterCfg::ItemCfg * RandActivityMiJingQiWenConfig::GetCrystalFbMonsterCfg(int monster_group_id)const
{
	std::map<int, CrystalFbMonsterCfg > ::const_iterator iter = m_monster_cfg.find(monster_group_id);
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

const std::vector<int> RandActivityMiJingQiWenConfig::GetMonsterDropId(int monster_npc_id, int play_times, int * village_id/* = NULL*/, int * prestige_num/* = NULL*/, int *add_exp/* = NULL*/, std::vector<ItemConfigData> * reward_list/* = NULL*/) const
{
	std::vector<int> ret;
	std::map<int, CrystalFbMonsterDropCfg>::const_iterator iter = m_monster_drop_cfg.find(monster_npc_id);
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

const int RandActivityMiJingQiWenConfig::GetRewardColumn(int play_times)const
{
	for (std::map<int, int> ::const_iterator iter = m_play_times_to_reward_column.begin(); iter != m_play_times_to_reward_column.end(); ++iter)
	{
		if (play_times <= iter->first)
		{
			return iter->second;
		}
	}

	return 0;
}

const MiJingQiWenFbNPCRewardCfg * RandActivityMiJingQiWenConfig::GetNpcRewardCfg(int npc_id, int option_index) const
{
	if (option_index < 0 || option_index >= 2)return NULL;
	std::map<int, MiJingQiWenFbNPCRewardListCfg>::const_iterator it = m_npc_reward_cfg_map.find(npc_id);
	if (it != m_npc_reward_cfg_map.end())
	{
		return &it->second.reward_list[option_index];
	}

	return NULL;
}

const MiJingQiWenFbNPCRewardListCfg * RandActivityMiJingQiWenConfig::GetNpcEventCfg(int npc_id)const
{
	std::map<int, MiJingQiWenFbNPCRewardListCfg>::const_iterator it = m_npc_reward_cfg_map.find(npc_id);
	if (it != m_npc_reward_cfg_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const int RandActivityMiJingQiWenConfig::GetEventNpcCfg(int group_id, int rand_num, std::vector<new_Event_NPC_cfg>* get_npc_list, std::vector<new_Event_NPC_cfg>*has_create_npc_vec) const
{
	if (get_npc_list == NULL)return 0;
	std::map<int, std::vector<new_Event_NPC_cfg> >::const_iterator it = m_event_npc_cfg_map.find(group_id);
	std::map<int, int>::const_iterator it2 = m_event_npc_cfg_weight_map.find(group_id);
	int real_count = 0;
	if (it != m_event_npc_cfg_map.end() && it2 != m_event_npc_cfg_weight_map.end())
	{
		std::vector<new_Event_NPC_cfg> node_vec;
		node_vec.assign(it->second.begin(), it->second.end());
		int weight = it2->second;//记录总权重
								 //处理重复NPC
		if (has_create_npc_vec != NULL)
		{
			int size_num = (int)has_create_npc_vec->size();
			std::vector<new_Event_NPC_cfg>::iterator same_it2 = has_create_npc_vec->begin();//把需要取出的一个一个删除
			for (int size_vec = 0; size_vec < size_num && size_vec < 100; size_vec++)
			{
				for (std::vector<new_Event_NPC_cfg>::iterator same_it = node_vec.begin(); same_it != node_vec.end(); same_it++)
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

		for (int i = 0; i < rand_num && i < 100; i++)
		{
			if ((int)node_vec.size() <= 0 || real_count >= rand_num)return real_count;
			if (weight <= 0)return real_count;
			int rand_dis = rand() % weight;
			int cum_count = 0;
			for (std::vector<new_Event_NPC_cfg>::iterator itor = node_vec.begin(); itor != node_vec.end(); itor++)
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

const BuffListConfig * RandActivityMiJingQiWenConfig::GetBuffId(int buff_group_id) const
{
	std::map<int, std::vector<BuffListConfig> >::const_iterator it = m_buff_list_group.find(buff_group_id);
	if (it != m_buff_list_group.end())
	{
		if (it->second.size() > 0)
		{
			int rand_idx = rand() % (int)it->second.size();
			return &it->second[rand_idx];
		}
	}
	return NULL;
}

const int RandActivityMiJingQiWenConfig::RandNPCShopItemList(int npc_id, NpcShopItem but_list[MAX_ROLE_CRYSTALFB_BUY_ITEM_COUNT])const
{
	std::map<int, std::vector<NpcShopConfig> >::const_iterator it = m_npc_shop_list_map.find(npc_id);
	int real_item_count = 0;
	if (it != m_npc_shop_list_map.end())
	{
		for (std::vector<NpcShopConfig>::const_iterator it2 = it->second.begin(); it2 != it->second.end() && real_item_count < MAX_ROLE_CRYSTALFB_BUY_ITEM_COUNT; it2++)
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

const GatherNpcConfig * RandActivityMiJingQiWenConfig::GetGatherNpcCfg(int npc_id) const
{
	std::map<int, GatherNpcConfig>::const_iterator it = m_gather_npc_cfg_map.find(npc_id);
	if (it != m_gather_npc_cfg_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const int RandActivityMiJingQiWenConfig::GetWorkshopItemWeightCfg(int group_id)const
{
	std::map<int, int>::const_iterator it = m_workshop_wight_map.find(group_id);
	if (it != m_workshop_wight_map.end())
	{
		return it->second;
	}
	return 0;
}

const std::vector<WorkshopItemConfig> * RandActivityMiJingQiWenConfig::GetWorkshopItemCfg(int group_id)const
{
	std::map<int, std::vector<WorkshopItemConfig> >::const_iterator it = m_workshop_item_cfg_map.find(group_id);
	if (it != m_workshop_item_cfg_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const MiJingQiWenFbLevelTypeCfg * RandActivityMiJingQiWenConfig::GetLevelToCfg(int role_level) const
{
	for (std::vector<MiJingQiWenFbLevelTypeCfg>::const_iterator it = m_level_to_seq_vec.begin(); it != m_level_to_seq_vec.end(); it++)
	{
		if (it->level_down <= role_level && it->level_up >= role_level)
		{
			return &(*it);
		}
	}
	return NULL;
}

int RandActivityMiJingQiWenConfig::InitLevelCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

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


		if (cfg.type == ROLE_CRYSTAL_FB_TYPE_XI_YOU)
		{
			if (!GetSubNodeValue(root_element, "limit", cfg.limit_times))
			{
				return -5;
			}

			if (next_fb_index > CRYSTAL_FB_XI_YOU_MAX_LEVEL)
			{
				return -501;
			}
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

		TiXmlElement *formula_drop_group_element = root_element->FirstChildElement("formula_drop_group");
		if (formula_drop_group_element && formula_drop_group_element->FirstChild())
		{
			int formula_drop_group = 0;
			if (!GetNodeValue(formula_drop_group_element, formula_drop_group))
			{
				return -18;
			}

			cfg.formula_drop_group = formula_drop_group;
		}

		TiXmlElement *  drop_list_ele = root_element->FirstChildElement("drop_list_4");
		if (NULL != drop_list_ele)
		{
			TiXmlElement *node_element = drop_list_ele->FirstChildElement("dropid");
			while (node_element && node_element->FirstChild())
			{
				int drop_id = 0;
				if (!GetNodeValue(node_element, drop_id))
				{
					return -18;
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

			if (i >= 1)return -999;

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

		m_level_cfg.push_back(cfg);
		root_element = root_element->NextSiblingElement();
	}


	return 0;
}

int RandActivityMiJingQiWenConfig::InitBoxCfg(TiXmlElement * RootElement)
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

int RandActivityMiJingQiWenConfig::InitRandNumCfg(TiXmlElement * RootElement)
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

int RandActivityMiJingQiWenConfig::InitRewardCfg(TiXmlElement * RootElement)
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

int RandActivityMiJingQiWenConfig::InitPosCfg(TiXmlElement * RootElement)
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

int RandActivityMiJingQiWenConfig::InitSceneCfg(TiXmlElement * RootElement)
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

int RandActivityMiJingQiWenConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "item_num", m_other_cfg.item_num))
	{
		return -1;
	}

	if (!GetSubNodeValue(root_element, "random_event_num", m_other_cfg.random_event_num))
	{
		return -2;
	}

	if (!GetSubNodeValue(root_element, "question_num", m_other_cfg.question_num))
	{
		return -3;
	}

	if (!GetSubNodeValue(root_element, "question_times", m_other_cfg.question_times))
	{
		return -4;
	}

	if (!GetSubNodeValue(root_element, "mall_item_num", m_other_cfg.mall_item_num))
	{
		return -5;
	}

	if (!GetSubNodeValue(root_element, "scene_id", m_other_cfg.scene_id))
	{
		return -6;
	}

	if (!GetSubNodeValue(root_element, "x", m_other_cfg.x))
	{
		return -7;
	}

	if (!GetSubNodeValue(root_element, "y", m_other_cfg.y))
	{
		return -8;
	}

	if (!GetSubNodeValue(root_element, "start_time", m_other_cfg.start_time))
	{
		return -9;
	}

	if (!GetSubNodeValue(root_element, "end_time", m_other_cfg.end_time))
	{
		return -10;
	}

	if (!GetSubNodeValue(root_element, "question_id", m_other_cfg.question_id))
	{
		return -11;
	}

	return 0;
}

int RandActivityMiJingQiWenConfig::InitUseLimitCfg(TiXmlElement * RootElement)
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
		if (!GetSubNodeValue(root_element, "use_type", use_type))
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

int RandActivityMiJingQiWenConfig::InitMonsterCfg(TiXmlElement * RootElement)
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

int RandActivityMiJingQiWenConfig::InitMonsterDropCfg(TiXmlElement * RootElement)
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

int RandActivityMiJingQiWenConfig::InitPlayTimesToRewardColumnCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	int last_play_times = -1;
	while (NULL != root_element)
	{
		int play_times = -1;
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

int RandActivityMiJingQiWenConfig::InitEventGroupCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}
	int last_event_group_id = 0;
	while (NULL != root_element)
	{
		new_Event_NPC_cfg node_cfg;
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
			node_cfg.npc_type < new_Event_NPC_cfg::CRYSTALFB_NPC_TYPE_0 ||
			node_cfg.npc_type >  new_Event_NPC_cfg::CRYSTALFB_NPC_TYPE_11)
		{
			return -4;
		}

		m_event_npc_cfg_map[node_cfg.event_group_id].push_back(node_cfg);
		m_event_npc_cfg_weight_map[node_cfg.event_group_id] += node_cfg.rate;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityMiJingQiWenConfig::InitEventListCfg(TiXmlElement * RootElement)
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
		MiJingQiWenFbNPCRewardListCfg &node_cfg = m_npc_reward_cfg_map[npc_id];

		int operation_id = 0;
		if (!GetSubNodeValue(root_element, "operation_id", operation_id) || operation_id <= 0 || operation_id > MiJingQiWenFbNPCRewardListCfg::MAX_OPERATION_INDEX)
		{
			return -2;
		}

		int reward_type = 0;
		if (!GetSubNodeValue(root_element, "reward_type", reward_type) || reward_type <= 0 || reward_type >= MiJingQiWenFbNPCRewardCfg::REWARD_TYPE_MAX)
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

int RandActivityMiJingQiWenConfig::InitShopListCfg(TiXmlElement * RootElement)
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

int RandActivityMiJingQiWenConfig::InitBuffListCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int buff_group_id = 0;
		if (!GetSubNodeValue(root_element, "buff_group_id", buff_group_id) || buff_group_id <= 0)
		{
			return -99;
		}

		std::vector<BuffListConfig> &it = m_buff_list_group[buff_group_id];
		BuffListConfig node_cfg;
		if (!GetSubNodeValue(root_element, "buff_id", node_cfg.buff_id) || node_cfg.buff_id <= 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "type", node_cfg.type) || node_cfg.type < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "param_1", node_cfg.param_1) || node_cfg.param_1 < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "param_2", node_cfg.param_2))
		{
			return -4;
		}
		it.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityMiJingQiWenConfig::InitGatherNpcCfg(TiXmlElement * RootElement)
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

int RandActivityMiJingQiWenConfig::InitWrokShopItemCfg(TiXmlElement * RootElement)
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

int RandActivityMiJingQiWenConfig::InitMiJingQiWenLevelTypeCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		MiJingQiWenFbLevelTypeCfg node;
		if (!GetSubNodeValue(root_element, "type", node.type) || node.type < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "level_down", node.level_down) || node.level_down < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "level_up", node.level_up) || node.level_up < 0)
		{
			return -3;
		}

		m_level_to_seq_vec.push_back(node);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}


void RandActivityMiJingQiWenConfig::RefreshPos(CrystalFbSceneCfg::ItemCfg ARG_OUT & cfg) const
{
	std::map<int, CrystalFbPosCfg > ::const_iterator iter = m_pos_cfg.find(cfg.sceneborn_group_id);
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