#include <memory.h>

#include "npcpool.h"
#include "servercommon/configcommon.h"
#include "gameworld/gamelog.h"
#include "item/itempool.h"

NpcPool::NpcPool()
{
}

NpcPool::~NpcPool()
{
}

NpcPool * gs_npc_pool_cfg = NULL;
NpcPool * NpcPool::Instance()
{
	if (NULL == gs_npc_pool_cfg)
	{
		gs_npc_pool_cfg = new NpcPool();
	}

	return gs_npc_pool_cfg;
}

bool NpcPool::Init(const std::string & dir, std::string * err)
{
	PRE_LOAD_MULTI_LIST;

	{
		READ_NEW_LIST("npc_dynamic_list.xml");		// 动态列表

		iRet = this->InitNpcInformaCfg(RootElement);	// NPC 信息
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitNpcInformaCfg failed %d", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	{
		READ_NEW_LIST("npc.xml");		// 服务端动态NPC列表（用于由服务端创建的NPC实例）

		LOAD_CONFIG("npc_list", InitServerNPCCfg);
	}

	{
		READ_NEW_LIST("behavior_prize_cfg.xml");	// 交互奖励表（用于与服务器创建的实例交互后所得奖励，必须在 npc.xml 读取的后面）

		LOAD_CONFIG("behavior_prize_list", InitRewardPoolCfg);
		LOAD_CONFIG("gather_list", InitGatherListCfg);	// 会检测奖池是否存在，必须后于奖池的读取
	}

	{
		READ_NEW_LIST("npc_behavior.xml");	

		LOAD_CONFIG("operation_cfg", InitOperationCfg);//选项配置
		LOAD_CONFIG("npc_behavior_cfg", InitChosedNPCCfg);	//NPC对应选项操作
		LOAD_CONFIG("box_cfg", InitTreasureChestCfg);//宝箱消耗
	}

	return true;
}

bool NpcPool::Reload(const std::string& dir, std::string* err)
{
	NpcPool *temp = new NpcPool();

	if (!temp->Init(dir, err))
	{
		delete temp; temp = NULL;
		return false;
	}

	if (NULL != gs_npc_pool_cfg) delete gs_npc_pool_cfg;

	gs_npc_pool_cfg = temp;

	return true;
}

const NPCDynamicInformaCfg * NpcPool::GetNPCInformaCfg(int seq_id)
{
	std::map<int, NPCDynamicInformaCfg>::iterator f_it = m_npc_info_cfg.find(seq_id);
	if (m_npc_info_cfg.end() != f_it) return & f_it->second;

	gamelog::g_log_world.printf(LL_ERROR, "%s ERROR with Lacking of NPC - [%d]", __FUNCTION__, seq_id);
	return NULL;
}

const ServerNPCInfoCfg* NpcPool::GetServerNPCInfo(int npc_id)
{
	ServerNpcCfgMap::iterator it = m_server_npc_map.find(npc_id);
	if (it == m_server_npc_map.end()) return NULL;

	return &it->second;
}

const ItemConfigData * NpcPool::RandGatherAwardItem(int award_pool_id)
{
	std::map<int, std::vector<NPCItemGetCfg> >::iterator f_it = m_gather_item_cfg.find(award_pool_id);
	if (m_gather_item_cfg.end() == f_it) return NULL;

	int rand_prob = rand() % RAND_UNIVERSAL_RATIO_BASE_NUM;
	for (std::vector<NPCItemGetCfg>::iterator loop_it = f_it->second.begin(); loop_it != f_it->second.end(); ++loop_it)
	{
		if (rand_prob < loop_it->prob) return & loop_it->award_item;
		rand_prob -= loop_it->prob;
	}

	return NULL;
}

const std::vector<Posi>* NpcPool::GetSceneClientNpcList(int scene_id)
{
	std::map<int, std::vector<Posi> >::iterator it = m_scene_npc_posi_map.find(scene_id);
	if (it != m_scene_npc_posi_map.end())
	{
		return &it->second;
	}

	return NULL;
}

const NPCChosedTalkInfoCfg * NpcPool::GetChosedNpcCfg(int npc_id)
{
	std::map<int, NPCChosedTalkInfoCfg>::iterator it = m_chosed_npc_cfg_map.find(npc_id);
	if (it != m_chosed_npc_cfg_map.end())
	{
		return &it->second;
	}
	return NULL;
}

const NPCTreasureChestInfoCfg * NpcPool::GetTreasureChestCfg(int treasurechest_id)
{
	std::map<int, NPCTreasureChestInfoCfg>::iterator it = m_treasure_chest_cfg_map.find(treasurechest_id);
	if (it != m_treasure_chest_cfg_map.end())
	{
		return &it->second;
	}
	return NULL;
}

int NpcPool::InitNpcInformaCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;
	
	while (NULL != dataElement)
	{
		int t_seq_id = 0; NPCDynamicInformaCfg t_nic;
		if (!GetSubNodeValue(dataElement, "seq", t_seq_id) || t_seq_id < 0 || m_npc_info_cfg.end() != m_npc_info_cfg.find(t_seq_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "id", t_nic.npc_id) || t_nic.npc_id < 0)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "scene_id", t_nic.scene_id))
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "x", t_nic.npc_posi.x) || t_nic.npc_posi.x < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "y", t_nic.npc_posi.y) || t_nic.npc_posi.y < 0)
		{
			return -5;
		}
		
		m_npc_info_cfg.insert(std::pair<int, NPCDynamicInformaCfg>(t_seq_id, t_nic));
		dataElement = dataElement->NextSiblingElement();

		{
			m_scene_npc_posi_map[t_nic.scene_id].push_back(t_nic.npc_posi);
		}
	}
	
	return 0;
}

int NpcPool::InitGatherListCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int t_npc_id = 0;
		if (!GetSubNodeValue(dataElement, "npc_id", t_npc_id))
		{
			return -1;
		}

		ServerNpcCfgMap::iterator f_it = m_server_npc_map.find(t_npc_id);
		if (m_server_npc_map.end() == f_it)
		{
			printf("InitGatherListCfg npc_id[%d] not exist\n", t_npc_id);
			return -10000;		// 如果找不到，报错
		}	
		
		ServerNPCGatherParam & t_sngp = f_it->second.server_npc_param.gather_param;
		if (!GetSubNodeValue(dataElement, "gather_time", t_sngp.gather_time) || t_sngp.gather_time < 0)
		{
			return -2;
		}

		t_sngp.award_item_count = 0;
		TiXmlElement * award_list = dataElement->FirstChildElement("behavior_prize");
		if (NULL == award_list) return -200;

		TiXmlElement * data_node = award_list->FirstChildElement("node");
		while (NULL != data_node)
		{
			if (t_sngp.award_item_count >= GATHER_ITEM_TYPE_PER_GROUP) return -3;

			int t_award_group = 0;
			if (!GetNodeValue(data_node, t_award_group))
			{
				data_node = data_node->NextSiblingElement();
				continue;		// 如果读取不到，不返回错误，继续
			}

			if (m_gather_item_cfg.end() == m_gather_item_cfg.find(t_award_group))
			{
				return -3;
			}

			t_sngp.gather_item_group_list[t_sngp.award_item_count++] = t_award_group;
			data_node = data_node->NextSiblingElement();
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int NpcPool::InitRewardPoolCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	int rand_total = 0;
	while (NULL != dataElement)
	{
		int t_group_id = 0; NPCItemGetCfg t_nigc;
		if (!GetSubNodeValue(dataElement, "prize_id", t_group_id) || t_group_id < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "odds_ten_thousand", t_nigc.prob) || t_nigc.prob < 0 || t_nigc.prob > RAND_UNIVERSAL_RATIO_BASE_NUM)
		{
			return -2;
		}

		TiXmlElement * award_obj = dataElement->FirstChildElement("prize");
		if (NULL == award_obj)
		{
			return -3;
		}

		if (!t_nigc.award_item.ReadConfig(award_obj))
		{
			return -7;
		}

		std::map<int, std::vector<NPCItemGetCfg> >::iterator f_it = m_gather_item_cfg.find(t_group_id);
		if (m_gather_item_cfg.end() != f_it)
		{
			if ((int)f_it->second.size() >= GATHER_ITEM_TYPE_PER_GROUP) return -4;
			rand_total += t_nigc.prob;
			f_it->second.push_back(t_nigc);
		}
		else
		{
			if (!m_gather_item_cfg.empty() && RAND_UNIVERSAL_RATIO_BASE_NUM != rand_total) return -5;
			rand_total = t_nigc.prob;

			std::vector<NPCItemGetCfg> t_vec_igc; t_vec_igc.push_back(t_nigc);
			m_gather_item_cfg.insert(std::pair<int, std::vector<NPCItemGetCfg> >(t_group_id, t_vec_igc));
		}

		dataElement = dataElement->NextSiblingElement();
	}

	if (RAND_UNIVERSAL_RATIO_BASE_NUM != rand_total) return -6;

	return 0;
}

int NpcPool::InitServerNPCCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int npc_id = 0;
		if (!GetSubNodeValue(dataElement, "id", npc_id) || m_server_npc_map.find(npc_id) != m_server_npc_map.end())
		{
			return -1;
		}

		int server_npc_type = 0;
		if (!GetSubNodeValue(dataElement, "server_npc_cfg", server_npc_type))
		{
			return -2;
		}
		if (server_npc_type <= SERVER_NPC_TYPE_INVALID || server_npc_type >= SERVER_NPC_TYPE_MAX)
		{
			dataElement = dataElement->NextSiblingElement();
			continue;
		}

		int server_param1 = 0;
		if (!GetSubNodeValue(dataElement, "server_param1", server_param1))
		{
			return -4;
		}

		static ServerNPCInfoCfg cfg;
		cfg.npc_id = npc_id;
		cfg.server_npc_type = server_npc_type;
		if (SERVER_NPC_TYPE_MONSTER == cfg.server_npc_type)
		{
			cfg.server_npc_param.monster_param.monster_group_id = server_param1;
		}
		else if (SERVER_NPC_TYPE_GATHERS == cfg.server_npc_type)
		{
			cfg.server_npc_param.gather_param.gather_id = server_param1;
		}
		m_server_npc_map.insert(ServerNpcCfgMap::value_type(npc_id, cfg));

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int NpcPool::InitOperationCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int operation_id = 0; 
		
		if (!GetSubNodeValue(dataElement, "operation_id", operation_id) || operation_id < 0)
		{
			return -1;
		}
		if (m_option_cfg_map.find(operation_id) != m_option_cfg_map.end())
		{
			return -999;
		}
		option_cfg &opt_cfg = m_option_cfg_map[operation_id];
		opt_cfg.operation_id = operation_id;
		if (!GetSubNodeValue(dataElement, "effect_type", opt_cfg.option_effect_type) || 
			opt_cfg.option_effect_type < OPTION_EFFECT_TYPE_NO ||
			opt_cfg.option_effect_type > OPTION_EFFECT_TYPE_SHOP)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "desc", opt_cfg.option_content))
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "need_coin", opt_cfg.cost_coin) || opt_cfg.cost_coin < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "monster_group_id", opt_cfg.monster_group_id) || opt_cfg.monster_group_id < 0)
		{
			return -5;
		}


		TiXmlElement * need_item = dataElement->FirstChildElement("need_item");
		if (NULL != need_item)
		{
			TiXmlElement * item_node = need_item->FirstChildElement("item_id");
			TiXmlElement * is_bind_node = need_item->FirstChildElement("is_bind");
			TiXmlElement * num_node = need_item->FirstChildElement("num");
			if (NULL != item_node && item_node->FirstChild()&&
				NULL != is_bind_node && is_bind_node->FirstChild()&&
				NULL != num_node && num_node->FirstChild())
			{
				if(!opt_cfg.need_item.ReadConfigNoCheck(need_item))return -6;
			}
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int NpcPool::InitChosedNPCCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int npc_id = 0;

		if (!GetSubNodeValue(dataElement, "npc_id", npc_id) || npc_id < 0)
		{
			return -1;
		}

		if (m_chosed_npc_cfg_map.find(npc_id) != m_chosed_npc_cfg_map.end())
		{
			return -2;
		}

		NPCChosedTalkInfoCfg &node_cfg = m_chosed_npc_cfg_map[npc_id];
		node_cfg.npc_id = npc_id;

		if (!GetSubNodeValue(dataElement, "talk_id", node_cfg.main_context))
		{
			return -3;
		}

		int operation_1 = 0;
		if (!GetSubNodeValue(dataElement, "operation_1", operation_1) || operation_1 < 0)
		{
			return -4;
		}

		std::map<int, option_cfg>::iterator it = m_option_cfg_map.find(operation_1);
		if (it != m_option_cfg_map.end())
		{
			memcpy(&node_cfg.option_list[0], &it->second, sizeof(option_cfg));
		}

		int operation_2 = 0;
		if (!GetSubNodeValue(dataElement, "operation_2", operation_2) || operation_2 < 0)
		{
			return -6;
		}

		it = m_option_cfg_map.find(operation_2);
		if (it != m_option_cfg_map.end())
		{
			memcpy(&node_cfg.option_list[1], &it->second, sizeof(option_cfg));
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int NpcPool::InitTreasureChestCfg(TiXmlElement * RootElement)
{
	TiXmlElement * dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -1000;

	while (NULL != dataElement)
	{
		int box_id = 0;

		if (!GetSubNodeValue(dataElement, "box_id", box_id) || box_id < 0)
		{
			return -1;
		}

		if (m_treasure_chest_cfg_map.find(box_id) != m_treasure_chest_cfg_map.end())
		{
			return -2;
		}

		NPCTreasureChestInfoCfg &node_cfg = m_treasure_chest_cfg_map[box_id];
		node_cfg.box_id = box_id;

		if (!GetSubNodeValue(dataElement, "consume_id", node_cfg.consume_id) || NULL == ITEMPOOL->GetItem(node_cfg.consume_id))
		{
			return -3;
		}

		int operation_1 = 0;
		if (!GetSubNodeValue(dataElement, "consume_num", node_cfg.consume_num) || node_cfg.consume_num <= 0)
		{
			return -4;
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}
