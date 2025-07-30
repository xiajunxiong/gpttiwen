#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/string/gameworldstr.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "config/logicconfigmanager.hpp"
#include "world.h"
#include "scene/scene.h"
#include "item/itemgriddata.h"
#include "config/otherconfig/sundryconfig.hpp"
#include "battle/battle_manager_local.hpp"
#include "item/knapsack.h"
#include "gamelog.h"

#include "protocol/msgtreasuremap.h"
#include "other/treasure_map/treasuremapconfig.h"
#include "other/treasure_map/treasuremap.h"
#include "treasureitem.hpp"
#include "gameworld/monster/monster_group_pool.hpp"
#include "gameworld/monster/monsterpool.hpp"
#include "other/pet/petconfig.hpp"

TreasureMapItem::TreasureMapItem() : ItemBase(I_TYPE_TREASURE_MAP), m_param1(0), m_param2(0)
{
}

void TreasureMapItem::RandomNetValueOnPutItem(Role * role, NetValueItemParam * param_data, int put_reason) const
{
	union TreasureMapItemUniqueSeq
	{
		struct
		{
			short server_id;
			unsigned short seq;
			unsigned int time;
		};

		long long unique_seq;
	};
	static unsigned short SEQ = 0;
	Posi pos;
	int scene_id = LOGIC_CONFIG->GetTreasureMapConfig()->GetRandScene(m_param2, &pos);
	Scene * scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(scene_id, COMMON_SCENE_KEY);
	if (scene != NULL)
	{
		TreasureMapItemUniqueSeq tmp;
		tmp.server_id = LocalConfig::Instance().GetMergeServerId();
		tmp.seq = SEQ++;
		tmp.time = (unsigned int)EngineAdapter::Instance().Time();

		param_data->treasure_map_param.scene_id = scene_id;
		param_data->treasure_map_param.pos_x= pos.x;
		param_data->treasure_map_param.pos_y = pos.y;
		param_data->treasure_map_param.client_need_uid = tmp.unique_seq;
		param_data->treasure_map_param.is_active_map = 0;
		const TreasureMapItemCfg::ItemCfg * cfg = LOGIC_CONFIG->GetTreasureMapConfig()->GetRandItemCfg(m_param1, role->GetLevel());
		if (cfg == NULL)
		{
			return;
		}

		param_data->treasure_map_param.param1 = cfg->seq;
		param_data->treasure_map_param.param2 = cfg->index;

		switch (cfg->item_type)
		{
		case TREASURE_MAP_ITEM_TYPE_MONSTER:
			{
				param_data->treasure_map_param.param_type = TREASURE_MAP_ITEM_TYPE_MONSTER;
				//param_data->treasure_map_param.monster_id = cfg->monster_id;
			}
			break;
		case TREASURE_MAP_ITEM_TYPE_COIN:
			{
				param_data->treasure_map_param.param_type = TREASURE_MAP_ITEM_TYPE_COIN;
			}
			break;
		case TREASURE_MAP_ITEM_TYPE_ITEM:
			{
				param_data->treasure_map_param.param_type = TREASURE_MAP_ITEM_TYPE_ITEM;
			}
			break;
		case TREASURE_MAP_ITEM_TYPE_RAND_ITEM:
			{
				const ItemConfigData * rand_reward = LOGIC_CONFIG->GetTreasureMapConfig()->GetRandReward(cfg->reward_grounp_id);
				if (rand_reward != NULL)
				{
					param_data->treasure_map_param.param_type = TREASURE_MAP_ITEM_TYPE_RAND_ITEM;
				}
			}
			break;

		default:
			return;
		}

		param_data->treasure_map_param.is_rand_map = 1;

		gamelog::g_Log_treasure_map.printf(LL_INFO, "%s role[%d, %s]scene_id[%d] pos[x:%d y:%d] param1:%d param2:%d map_type:%d", __FUNCTION__, role->GetUID(), role->GetName(), scene_id, pos.x, pos.y, 
			m_param1, m_param2, m_map_type);
	}
	else
	{
		gamelog::g_Log_treasure_map.printf(LL_INFO, "%s error role[%d, %s] scene_id[%d] pos[x:%d y:%d] param1:%d param2:%d map_type:%d", __FUNCTION__, role->GetUID(), role->GetName(), scene_id, pos.x, pos.y,
			m_param1, m_param2, m_map_type);
	}
}

int TreasureMapItem::Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type /*= ITEM_PARAM_TYPE_INVAILD*/, void * param /*= NULL*/) const
{
	if (NULL == role || NULL == role->GetScene() || num <= 0 || item_data.param == NULL) return -1;
	Posi target_posi(item_data.param->treasure_map_param.pos_x, item_data.param->treasure_map_param.pos_y);

	if (role->GetScene()->GetSceneID() != item_data.param->treasure_map_param.scene_id)
	{
		return -__LINE__;
	}

	//if ((role->GetPos() - target_posi).IsLongerThan(15))
	//{
	//	return -__LINE__;
	//}

	if (num > 1)
	{
		role->NoticeNum(errornum::EN_ITEM_CANNOT_MULTI_USE);
		return -__LINE__;
	}

	if (role->InTeam())
	{
		role->NoticeNum(errornum::EN_CANNOT_USE_ITEM_IN_TEAM);
		return -__LINE__;
	}

	if (item_data.param->treasure_map_param.monster_id != 0)
	{
		return BattleManagerLocal::GetInstance().StartBattleModeFightReq(role, BATTLE_MODE_TREASURE_ITEM, item_data.param->treasure_map_param.monster_id, 0, 0, true) ? 0 : -__LINE__;
	}
	
	if (!this->RandReward(role, item_data, out_need_set_dirty))                                                                      
	{
		return -__LINE__;
	}

	return 0;
}

bool TreasureMapItem::RandReward(Role * role, const ItemGridData & item_data, bool ARG_OUT & out_need_set_dirty) const
{
	TreasureMapItemParam & node = item_data.param->treasure_map_param;
	
	if (TREASURE_MAP_ITEM_RANK_TYPE_STAR != m_map_type && TREASURE_MAP_ITEM_RANK_TYPE_HALLOW != m_map_type && node.param_type != TREASURE_MAP_ITEM_TYPE_MONSTER)
	{
		role->NoticeNum(noticenum::NT_USE_TREASURE_MAP_GET_ITEM);
	}

	if (item_data.param->treasure_map_param.is_rand_map == 0)//未随机图,帮玩家替换一下旧图
	{
		this->RandomNetValueOnPutItem(role, item_data.param, PUT_REASON_NO_NOTICE);
	}

	const TreasureMapItemCfg::ItemCfg * cfg = LOGIC_CONFIG->GetTreasureMapConfig()->GetItemDetailCfg(node.param1, role->GetLevel(),node.param2);
	if (cfg == NULL)
	{
		return false;
	}
	
	Protocol::SCTreasureMapItemOpenResult open_result;
	open_result.seq = node.param1;
	open_result.index = node.param2;
	open_result.item_id = this->GetItemId();
	EngineAdapter::Instance().NetSend(role->GetNetId(), &open_result, sizeof(open_result));
	bool ret = true;
	
	switch (node.param_type)
	{
	case TREASURE_MAP_ITEM_TYPE_MONSTER:
		{
			item_data.param->treasure_map_param.monster_id = cfg->monster_id;
			if (role->GetRoleModuleManager()->GetKnapsack()->HasItem(LOGIC_CONFIG->GetSundryConfig()->GetOtherCfg().treasure_map_notice_item, 1))
			{
				ret = BattleManagerLocal::GetInstance().StartBattleModeFightReq(role, BATTLE_MODE_TREASURE_ITEM, item_data.param->treasure_map_param.monster_id, 0, 0, true);
				if (ret)
				{
					role->GetRoleModuleManager()->GetTreasureMap()->SendNotice(cfg->monster_id, m_item_id, cfg->is_notice);
				}
				return ret;
			}
			else
			{
				out_need_set_dirty = true;
				ret = false;
			}
		}
		break;
	case TREASURE_MAP_ITEM_TYPE_COIN:
		{
			role->GetRoleModuleManager()->GetMoney()->AddCoinBind(cfg->coin, __FUNCTION__, false, true);
		}
		break;
	case TREASURE_MAP_ITEM_TYPE_ITEM:
		{
			bool is_send_mail = true;
			if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPut2(cfg->item, false))
			{
				if (role->GetRoleModuleManager()->GetKnapsack()->Put(cfg->item, this->GetPutReasonType()))
				{
					is_send_mail = false;
				}
			}
			if(is_send_mail)
			{
				role->GetRoleModuleManager()->GetKnapsack()->SendMail(&cfg->item, 1, SNED_MAIL_TYPE_DEFAULT, true);
			}
			gamelog::g_Log_treasure_map.printf(LL_INFO, "%s role[%d, %s] map[%d, %d] reward[%d, %d] param1:%d param2:%d map_type:%d", __FUNCTION__, role->GetUID(), role->GetName(), node.param1, node.param2,
				cfg->item.item_id, cfg->item.num, m_param1, m_param2, m_map_type);
		}
		break;
	case TREASURE_MAP_ITEM_TYPE_RAND_ITEM:
		{
			const ItemConfigData * rand_reward = LOGIC_CONFIG->GetTreasureMapConfig()->GetRandReward(cfg->reward_grounp_id);
			if (rand_reward != NULL)
			{
				bool is_send_mail = true;
				if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPut2(*rand_reward, false))
				{
					if (role->GetRoleModuleManager()->GetKnapsack()->Put(*rand_reward, this->GetPutReasonType()))
					{
						is_send_mail = false;
					}
				}
				if (is_send_mail)
				{
					role->GetRoleModuleManager()->GetKnapsack()->SendMail(rand_reward, 1, SNED_MAIL_TYPE_DEFAULT, true);
				}
				gamelog::g_Log_treasure_map.printf(LL_INFO, "%s role[%d, %s] map[%d, %d] reward[%d, %d] param1:%d param2:%d map_type:%d", __FUNCTION__, role->GetUID(), role->GetName(), node.param1, node.param2,
					rand_reward->item_id, rand_reward->num, m_param1, m_param2, m_map_type);
			}
		}
		break;

	default:
		return false;
	}

	return ret;
}
/*
void TreasureMapItem::SendNotice(Role * role, int param, int is_notice) const 
{
	if(NULL == role || param < 0) return;

	static Protocol::SCSystemMsg sm;
	switch (is_notice)		// 0: 不公告 1:发布公告1 2:发布公告2
	{
		case 0:
		{
			return;
		}
		break;
	case 1:
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_treasure_item_notice1, role->GetName(), m_item_id, param);
		int sendlen = 0;
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
		{
			World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
		}
	}
	break;
	case 2:
	{
		const MonsterGroupCfg * monst_group_cfg = MonsterGroupPool::GetInstance().GetMonsterGroupCfg(param);
		if(NULL == monst_group_cfg) return;
		
		int pet_id = 0;
		for (int i = 0; i < SIDE_CHARACTER_NUM; i++)
		{
			const MonsterCfg*  monst_cfg = MonsterPool::GetInstance().GetMonsterCfg(monst_group_cfg->monster_id_list[i]);
			if (NULL == monst_cfg) continue;

			if (monst_cfg->m_catch_pet_id != 0)
			{
				pet_id = monst_cfg->m_catch_pet_id;
				break;
			}
		}
		if (pet_id == 0)
		{
			return;
		}

		const PetCfg *pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
		if (NULL == pet_cfg)
		{
			return;
		}

		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_treasure_item_notice2, role->GetName(), m_item_id, pet_cfg->quality, pet_id);
		int sendlen = 0;
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
		{
			World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
		}
	}
	break;
	default:
		break;
	}
}
*/
int TreasureMapItem::GetPutReasonType() const
{
	int put_reason_type = 0;
	switch (m_map_type)
	{
		case TREASURE_MAP_ITEM_RANK_TYPE_HIGH:
		{
			put_reason_type = PUT_REASON_HIGH_TREASURE_MAP;
		}
		break;
		case TREASURE_MAP_ITEM_RANK_TYPE_LOW:
		{
			put_reason_type = PUT_REASON_LOW_TREASURE_MAP;
		}
		break;
		case TREASURE_MAP_ITEM_RANK_TYPE_STAR:
		{
			put_reason_type = PUT_REASON_STAR_TREASURE_MAP;
		}
		break;
		case TREASURE_MAP_ITEM_RANK_TYPE_HALLOW:
		{
			put_reason_type = PUT_REASON_SHENG_QI_TREASURE_MAP;
		}
		break;
	default:
		break;
	}
	return put_reason_type;
}
