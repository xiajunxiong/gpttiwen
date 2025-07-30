#include "config/logicconfigmanager.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "item/knapsack.h"
#include "item/treasuremap/treasureitem.hpp"
#include "engineadapter.h"
#include "battle/battle_manager_local.hpp"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "servercommon/commondata.hpp"
#include "servercommon/string/gameworldstr.h"

#include "world.h"
#include "scene/scene.h"

#include "gameworld/monster/monster_group_pool.hpp"
#include "gameworld/monster/monsterpool.hpp"

#include "other/treasure_map/treasuremapconfig.h"
#include "protocol/msgtreasuremap.h"
#include "treasuremap.h"
#include "other/pet/petconfig.hpp"
#include "other/funopen/funopen.hpp"
#include "other/event/eventhandler.hpp"
#include "other/dayreward/dayrewardconfig.hpp"
#include "gamelog.h"
#include "other/roleactivity/roleactivitymanager.hpp"

TreasureMap::TreasureMap() :m_role_module_mgr(NULL)
{
}

TreasureMap::~TreasureMap()
{
}

void TreasureMap::OnRoleEnterScene()
{
	if (m_role_module_mgr->GetRole()->GetScene()->GetSceneID() == m_param.scene_id)
	{
		this->SendInfo();
	}
}

void TreasureMap::Init(RoleModuleManager * mgr, const TreasureMapParam & param)
{
	m_role_module_mgr = mgr;
	m_param = param;	
}

void TreasureMap::GetInitParam(TreasureMapParam * param)
{
	*param = m_param;
}

void TreasureMap::OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid)
{
	
}

void TreasureMap::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;
	this->OnFindDataReset();

	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_TREASURE_MAP_TASK)) return;
	m_param.play_times = 0;
	m_param.get_reward_times = 0;

	{	// 清除身上的任务；
		m_param.monster_pos_x = 0;
		m_param.monster_pos_y = 0;
		m_param.monster_id = 0;
		m_param.scene_id = 0;
		m_param.task_id = 0;
	}

	this->SendInfo();
}

void TreasureMap::OnFindDataReset()
{
	const DayRewardFindCfg *cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(DAY_ACTIVITY_TYPE_TREASURE_MAP_TASK);
	if (NULL == cfg || m_role_module_mgr->GetRole()->GetLevel() < cfg->find_level) return;

	const TreasureMapOtherCfg & other_cfg = LOGIC_CONFIG->GetTreasureMapConfig()->GetOtherCfg();
	if (m_role_module_mgr->GetCommonData().offine_day < 2)
	{
		if (other_cfg.limit_get_reward_times >= m_param.get_reward_times)
		{
			m_param.find_back_times = other_cfg.limit_get_reward_times - m_param.get_reward_times;
		}
		else
		{
			m_param.find_back_times = 0;
		}
	}
	else
	{
		m_param.find_back_times = other_cfg.limit_get_reward_times;
	}
}

bool TreasureMap::RewardFindBack(int find_type, int count)
{
	if (count > m_param.find_back_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FIND_COUNT_NOT_ENOUGH);
		return false;
	}

	const TreasureMapAwardBackCon * reward_cfg = LOGIC_CONFIG->GetTreasureMapConfig()->GetRewardBack(m_role_module_mgr->GetRole()->GetLevel(), find_type);
	if (NULL == reward_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FIND_REWARD_ERROR);
		return false;
	}


	// 摇道具
	std::vector<ItemConfigData> item_list;
	for (int i = 0; i < count; ++i)		// 在这里写循环是为了实现『每次找回获取的道具都有可能不同』的逻辑；
	{
		const ItemConfigData * item = LOGIC_CONFIG->GetTreasureMapConfig()->RandBackItem(reward_cfg->back_item_group_id);
		if (NULL != item)
		{
			item_list.push_back(*item);
		}
	}
	if (!item_list.empty() && reward_cfg->back_item_group_id != -1)
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)item_list.size(), &item_list[0]))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return false;
		}
		if (!m_role_module_mgr->GetKnapsack()->PutList((short)item_list.size(), &item_list[0], PUT_REASON_FIND_REWARD))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return false;
		}
	}
	if (NULL != m_role_module_mgr->GetRole()->GetGuild())
	{
		m_role_module_mgr->GetMoney()->AddOtherCoin(count * reward_cfg->family_contribution, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__);
	}
	m_role_module_mgr->GetMoney()->AddCoinBind(count * reward_cfg->coin_bind, __FUNCTION__);
	m_role_module_mgr->GetRole()->AddExp(count * reward_cfg->experience, ADD_EXP_TYPE_FIND_REWARD, __FUNCTION__);

	m_param.find_back_times -= count;
	m_role_module_mgr->GetCommonData().today_activity_type_find_count[DAY_ACTIVITY_TYPE_TREASURE_MAP_TASK] += count;
	m_role_module_mgr->GetCommonData().today_find_reward_flag.SetBit(DAY_ACTIVITY_TYPE_TREASURE_MAP_TASK);
	return true;
}

void TreasureMap::OnActiveStarTreasureMap(short column, short index)
{
	const ItemGridData * item_grid = m_role_module_mgr->GetKnapsack()->GetItemGridData2(column, index);
	if (NULL == item_grid)
	{
		return;
	}
	const ItemBase * item_base = ITEMPOOL->GetItem(item_grid->item_id);
	if (NULL == item_base || I_TYPE_TREASURE_MAP != item_base->GetItemType() || NULL == item_grid->param) return;

	TreasureMapItemParam & map_item_param = item_grid->param->treasure_map_param;
	if(0 != map_item_param.is_active_map) return;
	const TreasureMapItem * treasure_map_item = (const TreasureMapItem *)item_base;
	if (TREASURE_MAP_ITEM_RANK_TYPE_STAR != treasure_map_item->GetMapType() || TREASURE_MAP_ITEM_RANK_TYPE_HALLOW != treasure_map_item->GetMapType()) return;

	map_item_param.is_active_map = 1;
	m_role_module_mgr->GetKnapsack()->SetItemDirty(column, index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
}

void TreasureMap::OnResetTreasureMap(short column, short index)
{
	const ItemGridData * item_grid = m_role_module_mgr->GetKnapsack()->GetItemGridData2(column, index);
	if (NULL == item_grid)
	{
		return;
	}
	const ItemBase * item_base = ITEMPOOL->GetItem(item_grid->item_id);
	if (NULL == item_base || I_TYPE_TREASURE_MAP != item_base->GetItemType() || NULL == item_grid->param) return;

	//保存之前状态,重新随机坐标
	char is_active_map = item_grid->param->treasure_map_param.is_active_map;
	const TreasureMapItem * treasure_map_item = (const TreasureMapItem *)item_base;
	treasure_map_item->RandomNetValueOnPutItem(m_role_module_mgr->GetRole(), item_grid->param, treasure_map_item->GetPutReasonType());
	item_grid->param->treasure_map_param.is_active_map = is_active_map;

	m_role_module_mgr->GetKnapsack()->SetItemDirty(column, index, true, Knapsack::ITEM_CHANGE_NOTICE_REASON_DATA_CHANGE);
}

void TreasureMap::OnRoleUpLevel(int from_level, int to_level)
{

}

void TreasureMap::ReFresh(bool is_force, bool _is_onekeyfinish)
{
	if (!is_force && m_param.task_id != 0)
	{
		return;
	}

	Posi pos;
	LOGIC_CONFIG->GetTreasureMapConfig()->GetRandMonsterAndScene(m_role_module_mgr->GetRole()->GetLevel(), &m_param.task_id, &m_param.scene_id, &m_param.monster_id, &pos);
	Scene * scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(m_param.scene_id, COMMON_SCENE_KEY);
	if (scene != NULL)
	{
		m_param.monster_pos_x = pos.x;
		m_param.monster_pos_y = pos.y;
	}
	if (!_is_onekeyfinish)
	{
		this->SendInfo(true);
	}
	
}

void TreasureMap::Clear()
{
	m_param.monster_pos_x = 0;
	m_param.monster_pos_y = 0;
	m_param.monster_id = 0;
	m_param.scene_id = 0;
	m_param.task_id = 0;

	this->SendInfo();
}

void TreasureMap::SendInfo(bool is_new)
{
	Protocol::SCTreasureMapInfo info_protocol;

	info_protocol.play_times = m_param.play_times;
	info_protocol.get_reward_times = m_param.get_reward_times;
	info_protocol.x = m_param.monster_pos_x;
	info_protocol.y = m_param.monster_pos_y;
	info_protocol.monster_id = m_param.monster_id;
	info_protocol.scene_id = m_param.scene_id;
	info_protocol.task_id = m_param.task_id;
	info_protocol.is_new = is_new;

	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), &info_protocol, sizeof(info_protocol));
}

bool TreasureMap::CanStart()
{
	const TreasureMapOtherCfg & other_cfg = LOGIC_CONFIG->GetTreasureMapConfig()->GetOtherCfg();

	if (m_param.get_reward_times >= other_cfg.limit_get_reward_times)
	{
		return false;
	}

	return true;
}

void TreasureMap::OnFightWin(bool _is_one_key_finish)
{
	const TreasureMapOtherCfg & other_cfg = LOGIC_CONFIG->GetTreasureMapConfig()->GetOtherCfg();

	const TreasureMapTaskCfg  * cfg = LOGIC_CONFIG->GetTreasureMapConfig()->GetTreasureMapTaskCfg(m_param.task_id);
	if (cfg != NULL)
	{
		bool is_put = true;

		if (!_is_one_key_finish)
		{
			if (other_cfg.max_play_times - m_param.play_times > other_cfg.limit_get_reward_times - m_param.get_reward_times)
			{
				int r = rand() % TreasureMapTaskCfg::MAX_RATE_COUNT;
				if (r >= cfg->reward_rate)
				{
					is_put = false;
				}
			}
		}

		if (is_put)
		{
			m_param.get_reward_times += 1;
			
			if (!_is_one_key_finish)
			{
				m_role_module_mgr->GetMoney()->AddCoinBind(cfg->coin, __FUNCTION__);
				if (!cfg->reward_list.empty() && (m_role_module_mgr->GetRole()->GetLevel() >= cfg->min_level && m_role_module_mgr->GetRole()->GetLevel() <= cfg->max_level))
				{
					m_role_module_mgr->GetRole()->GetRoleModuleManager()->GetKnapsack()->NotifyGetItemList((short)cfg->reward_list.size(), &cfg->reward_list[0], GET_REASON_TREASURE_MAP_TASK);
					m_role_module_mgr->GetRole()->GetRoleModuleManager()->GetKnapsack()->PutList((short)cfg->reward_list.size(), &cfg->reward_list[0], PUT_REASON_TREASURE_MAP);
				}
			}
			m_role_module_mgr->GetRole()->AddExp(cfg->exp, ADD_EXP_TYPE_NORMAL);

			//奖励铜币和经验
			gamelog::g_log_debug.printf(LL_INFO, "should get coin = %d,should get exp = %d ,max_pay_time = %d,has been play time = %d\n", cfg->coin, cfg->exp, other_cfg.max_play_times, m_param.get_reward_times);

			EventHandler::Instance().OnGetTreasureMap(m_role_module_mgr->GetRole());

		}
		else
		{
			m_role_module_mgr->NoticeNum(noticenum::NT_TREASURE_MAP_NOT_ITEM_DROP);
		}
	}

	m_param.play_times += 1;

	this->Clear();

	if (m_param.get_reward_times < other_cfg.limit_get_reward_times)
	{
		this->ReFresh(true, _is_one_key_finish);
	}	
}

int TreasureMap::GetMonsterId()
{
	return m_param.monster_id;
}

void TreasureMap::OneKeyFinishTreasureMap()
{

	const DayRewardFindCfg *dr_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindReward(DAY_ACTIVITY_TYPE_TREASURE_MAP_TASK); if (!dr_cfg) return;
	const DayRewardFindPriceCfg * price_cfg = LOGIC_CONFIG->GetDayRewardConfig()->GetFindRewardPriceCfg(DAY_ACTIVITY_TYPE_TREASURE_MAP_TASK, m_role_module_mgr->GetRole()->GetLevel());
	if (NULL == price_cfg) return;
	const TreasureMapOtherCfg & other_cfg = LOGIC_CONFIG->GetTreasureMapConfig()->GetOtherCfg();
	const TreasureMapAwardBackCon *tmb_cfg = LOGIC_CONFIG->GetTreasureMapConfig()->GetFindType(m_role_module_mgr->GetRole()->GetLevel()); if (!tmb_cfg) return;

	int shengyu = other_cfg.limit_get_reward_times - this->GetTodayFinish();
	
	int consume_type = tmb_cfg->find_type;
	if (consume_type != RB_TYPE_COIN) return;
	int consume_money = 0;

	switch (consume_type)
	{
	case RB_TYPE_COIN:
		{
			consume_money = (price_cfg->coin_price * shengyu * price_cfg->coin_scale) / 100;
			if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(consume_money))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
			m_role_module_mgr->GetMoney()->UseCoinBind(consume_money, __FUNCTION__);
		}
		break;
/*	case RB_TYPE_GOLD:
		{
			consume_money = shengyu * dr_cfg->gold_price;
			if (!m_role_module_mgr->GetMoney()->GoldMoreThan(consume_money))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
			m_role_module_mgr->GetMoney()->UseGold(consume_money, __FUNCTION__);
		}
		break;*/
	default:
		return;
		break;
	}

	int count_x = 0;				// 限定次数，防止死循环,100次
	while ((this->GetTodayFinish() != other_cfg.limit_get_reward_times) && (count_x < 100))
	{
		OnFightWin(true);
		++count_x;
	}
}

void TreasureMap::OneClickUsedTreasureMap(unsigned int num)
{
	if (!m_role_module_mgr->GetRoleActivityManager()->HoldMonthCard())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_MONTH_CARD_CAN_USE_FUNCTION);
		return;
	}

	if (num <= 0) return;

	const ItemConfigData *item = LOGIC_CONFIG->GetTreasureMapConfig()->GetMapItem(TREASURE_MAP_ITEM_RANK_TYPE_HIGH, m_role_module_mgr->GetRole()->GetLevel());
	if (NULL == item) return;

	const ItemBase * new_item = ITEMPOOL->GetItem(item->item_id);
	if (NULL == new_item)return;
	if (new_item->GetItemType() != I_TYPE_TREASURE_MAP)return;
	const TreasureMapItem * ib = (const TreasureMapItem *)new_item;

	ItemID consume_item_id = LOGIC_CONFIG->GetTreasureMapConfig()->GetOtherCfg().high_map_id;

	if (!m_role_module_mgr->GetKnapsack()->HasItem(consume_item_id, num, false))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}

	int add_coin = 0;
	std::vector<ItemConfigData> rewards;
	int index = 0, count = 0;
	while (index < num && count < num * 2)	// count为了防止死循环,最多允许循环2n次
	{
		count++;
		static NetValueItemParam p_nvip; p_nvip.Reset();
		ib->RandomNetValueOnPutItem(m_role_module_mgr->GetRole(), &p_nvip, 0);

		TreasureMapItemParam & node = p_nvip.treasure_map_param;

		const TreasureMapItemCfg::ItemCfg * cfg = LOGIC_CONFIG->GetTreasureMapConfig()->GetItemDetailCfg(node.param1, m_role_module_mgr->GetRole()->GetLevel(), node.param2);
		if (cfg == NULL) continue;
		
		switch (node.param_type)
		{
		case TREASURE_MAP_ITEM_TYPE_MONSTER:
			{
				continue;		// 不随机到怪物事件, 重新随机净值
			}
			break;
		case TREASURE_MAP_ITEM_TYPE_COIN:
			{
				add_coin += cfg->coin;
			}
			break;
		case TREASURE_MAP_ITEM_TYPE_ITEM:
			{
				this->AddRewards(rewards, cfg->item);
			}
			break;
		case TREASURE_MAP_ITEM_TYPE_RAND_ITEM:
			{
				const ItemConfigData * rand_reward = LOGIC_CONFIG->GetTreasureMapConfig()->GetRandReward(cfg->reward_grounp_id);
				if (rand_reward != NULL)
				{
					this->AddRewards(rewards, *rand_reward);
				}
			}
			break;
		}
		index++;
	}


	if (!m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(rewards))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(consume_item_id, num, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (add_coin > 0)
	{
		m_role_module_mgr->GetMoney()->AddCoinBind(add_coin, __FUNCTION__, false, true);
	}

	m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_NO_NOTICE);
	
	std::string log_str;
	{
		Protocol::SCTreasureMapBatchUsedResult msg;
		msg.coin = add_coin;
		msg.count = (int)rewards.size();
		for (size_t i = 0; i < rewards.size() && i < MAX_TREASURE_MAP_REWARD_NOTICE; i++)
		{
			msg.rewards[i].item_id = rewards[i].item_id;
			msg.rewards[i].is_bind = rewards[i].is_bind ? 1 : 0;
			msg.rewards[i].num = rewards[i].num;

			log_str += STRING_SPRINTF("<%d, %d, %d> ", rewards[i].item_id, rewards[i].is_bind ? 1 : 0, rewards[i].num);
		}

		m_role_module_mgr->NetSend(&msg, sizeof(msg));
	}

	gamelog::g_Log_treasure_map.printf(LL_INFO, "%s role[%d, %s] used_num[%d] add_coin[%d] rewards[%s]", __FUNCTION__,
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), num, add_coin, log_str.c_str());
}

void TreasureMap::AddRewards(std::vector<ItemConfigData>& rewards, ItemConfigData item)
{
	const ItemBase * item_base = ITEMPOOL->GetItem(item.item_id);
	if (NULL == item_base) return;

	for (size_t i = 0; i < rewards.size(); i++)
	{
		// 找到了就堆叠放入,否则push_back
		if (rewards[i].item_id == item.item_id && rewards[i].is_bind == item.is_bind)	
		{
			int max_pile = item_base->GetPileLimit();

			// 刚好达到堆叠上限,后面可能还有新开的堆叠
			if(rewards[i].num >= max_pile) continue;	

			// 加上之后, 溢出的部分直接break到后面去加
			if ((rewards[i].num + item.num) > max_pile)	
			{
				int add_num = max_pile - rewards[i].num;
				rewards[i].num += add_num;
				item.num -= add_num;
				break;
			}

			// 不超过堆叠上限直接加就可以了
			rewards[i].num += item.num;		
			return;
		}
	}
	
	// 上面判断全部过了,直接放进去
	rewards.push_back(item);
}

bool TreasureMap::UseTreasureMap(int map_type, int use_num, ARG_OUT int* actual_use_num)
{	
	const ItemConfigData *item = LOGIC_CONFIG->GetTreasureMapConfig()->GetMapItem(map_type, m_role_module_mgr->GetRole()->GetLevel());
	if(NULL == item) return false;

	int need_grid_num = item->num * use_num;
	if (!m_role_module_mgr->GetKnapsack()->CheckEmptyGridNoLessThanForItem(item->item_id, need_grid_num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}

	*actual_use_num = 0;
	for (int i = 0; i < use_num; ++i)
	{
		if (!m_role_module_mgr->GetKnapsack()->Put(*item, PUT_REASON_TREASURE_MAP))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return *actual_use_num > 0;
		}

		++*actual_use_num;
	}
		
	return true;
}

void TreasureMap::SendNotice(int param1, int use_item_id, int is_notice) const
{
	if (param1 < 0) return;

	const ItemBase * item_base = ITEMPOOL->GetItem(use_item_id);
	if (NULL == item_base || I_TYPE_TREASURE_MAP != item_base->GetItemType()) return;

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
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_treasure_item_notice1, m_role_module_mgr->GetRole()->GetName(), use_item_id, param1);
		int sendlen = 0;
		if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
		{
			World::GetInstWorld()->SendToAllGateway((const char*)&sm, sendlen);
		}
	}
	break;
	case 2:
	{
		const MonsterGroupCfg * monst_group_cfg = MonsterGroupPool::GetInstance().GetMonsterGroupCfg(param1);
		if (NULL == monst_group_cfg) return;

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

		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_treasure_item_notice2, m_role_module_mgr->GetRole()->GetName(), use_item_id, pet_cfg->quality, pet_id);
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

	gamelog::g_Log_treasure_map.printf(LL_INFO, "%s role[%d, %s] is_notice[%d] param1[%d] use_item[%d]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), 
		is_notice, param1, use_item_id);
}



