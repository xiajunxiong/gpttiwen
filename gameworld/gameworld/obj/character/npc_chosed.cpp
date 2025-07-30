#include "npc_chosed.h"
#include "gameworld/npc/npcpool.h"
#include "gameworld/protocol/msgnpc.h"
#include "engineadapter.h"
#include "gameworld/item/knapsack.h"
#include "servercommon/errornum.h"
#include "gameworld/obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "gameworld/global/team/team.hpp"
#include "gameworld/item/itempool.h"
#include "gameworld/battle/battle_manager_local.hpp"
#include "gameworld/scene/scene.h"
#include "gameworld/other/pet/pet.hpp"
#include "gameworld/scene/speciallogic.hpp"
#include "gamelog.h"
#include "servercommon/noticenum.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/userprotocal/msgsystem.h"
#include "scene/scenemanager.hpp"
#include "other/event/eventhandler.hpp"

NPCChosed::NPCChosed() : NPC(Obj::OBJ_TYPE_NPC_CHOSED),m_battle_mode(0), m_is_lock(false), m_has_talk(false), m_send_to_member(false), m_is_send_option(true),
	m_battle_id(0), shop_num(0)
{

}

NPCChosed::~NPCChosed()
{

}

int NPCChosed::InitNPCChosed(int battle_mode)
{
	m_battle_mode = battle_mode;
	return 0;
}

bool NPCChosed::CanConsume(Role * gatherer)
{
	return false;
}

void NPCChosed::SetRemoveAfterGatherFlag(bool remove_flag)
{
	
}

bool NPCChosed::SendTalkInfoToRole(Role * role)
{
	if (role == NULL)return false;
	const NPCChosedTalkInfoCfg * cfg = NPCPOOL->GetChosedNpcCfg(this->GetNpcId());
	if (cfg == NULL)return false;
	
	Protocol::SCNPCCommitBack msg;
	msg.main_content = cfg->main_context;
	msg.obj_id = m_obj_id;
	if (m_is_send_option)
	{
		for (int i = 0; i < NPCChosedTalkInfoCfg::MAX_OPTION_NUM; i++)
		{
			msg.option_list[i].cost_coin = cfg->option_list[i].cost_coin;
			msg.option_list[i].option_content = cfg->option_list[i].option_content;
			msg.option_list[i].cost_item = cfg->option_list[i].need_item.item_id;
			msg.option_list[i].cost_num = cfg->option_list[i].need_item.num;
		}
	}
	else
	{
		memset(msg.option_list, 0, sizeof(msg.option_list));
	}

	if (role->InTeam() && role->GetMyTeam()->GetLeaderUID() == role->GetUID() && m_send_to_member)
	{
		Team * mem_team = role->GetMyTeam();
		if (mem_team == NULL)
		{
			return false;
		}
		int mem_count = mem_team->GetMemberCount();
		for (int j = 0;j < mem_count; j++)
		{
			Role* team_role = mem_team->GetMemberRoleByIndex(j);
			if (team_role != NULL && mem_team->IsRoleMemberFollow(team_role->GetUID()))
			{
				EngineAdapter::Instance().NetSend(team_role->GetNetId(), (const char *)&msg, sizeof(msg));
			}
		}
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&msg, sizeof(msg));
	}

	if (!m_has_talk)
	{
		SpecialLogic * special_logic = m_scene->GetSpecialLogic();
		special_logic->OnNPCChosedTalk(role, this->GetId());
		m_has_talk = true;
	}

	return true;
}

bool NPCChosed::ChosedOption(Role * role, int option_index)
{
	if (role == NULL)return false;
	const NPCChosedTalkInfoCfg * cfg = NPCPOOL->GetChosedNpcCfg(this->GetNpcId());
	if (cfg == NULL)return false;
	if (option_index < 0 || option_index >= NPCChosedTalkInfoCfg::MAX_OPTION_NUM)
	{
		return false;
	}

	const option_cfg &option_config = cfg->option_list[option_index];
	if (option_config.option_effect_type == OPTION_EFFECT_TYPE_SHOP)
	{
		this->SendShopInfo(role);
		return true;
	}

	if (role->InTeam() && role->GetMyTeam()->GetLeaderUID() != role->GetUID())
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
		return false;
	}

	switch (option_config.option_effect_type)
	{
	case OPTION_EFFECT_TYPE_NO:
		{
			SpecialLogic * special_logic = m_scene->GetSpecialLogic();
			special_logic->OnNPCChosedReward(role, this->GetId(), option_index);
		}
		break;
	case OPTION_EFFECT_TYPE_BATTLE:
		{
			BattleManagerLocal::GetInstance().StartBattleModeFightReq(role, this->GetBattleMode(), option_config.monster_group_id,
				 this->GetNpcSeq(), option_index,true);
		}
		break;
	case OPTION_EFFECT_TYPE_CONSUME:
		{
			if (option_config.cost_coin > 0)
			{
				if (!role->GetRoleModuleManager()->GetMoney()->CoinBindMoreThan(option_config.cost_coin))
				{
					role->GetRoleModuleManager()->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
					return false;
				}
			}

			if (option_config.need_item.num > 0 && NULL != ITEMPOOL->GetItem(option_config.need_item.item_id))
			{
				if (!role->GetRoleModuleManager()->GetKnapsack()->HasItem(option_config.need_item.item_id, option_config.need_item.num))
				{
					role->GetRoleModuleManager()->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
					return false;
				}
			}

			if (option_config.cost_coin > 0)
			{
				if (!role->GetRoleModuleManager()->GetMoney()->UseCoinBind(option_config.cost_coin, "NPCChosed::ChosedOption"))
				{
					role->GetRoleModuleManager()->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
					return false;
				}

				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_money_on_consume_money_message, MONEY_TYPE_COIN_BIND, option_config.cost_coin);
				if (length > 0)
				{
					Protocol::SCSystemMsg sm;
					int sendlen = 0;
					if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT)))
					{
						EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
					}
				}
			}

			if (option_config.need_item.num > 0 && NULL != ITEMPOOL->GetItem(option_config.need_item.item_id))
			{
				if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(option_config.need_item.item_id, option_config.need_item.num, "NPCChosed::ChosedOption", KNAPSACK_CONSUME_TYPE_CANNOT_BUY,true))
				{
					role->GetRoleModuleManager()->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
					return false;
				}

				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_knapsack_on_consume_message, option_config.need_item.item_id, option_config.need_item.num);
				if (length > 0)
				{
					Protocol::SCSystemMsg sm;
					int sendlen = 0;
					if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT, SYS_MSG_SYSTEM_CHAT)))
					{
						EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
					}
				}
			}
			//消耗道具跳过战斗（仅消耗队长）
			SpecialLogic * special_logic = m_scene->GetSpecialLogic();
			special_logic->OnNPCChosedReward(role, this->GetId(), option_index);
			//实际掉落奖励
			EventHandler::Instance().OnFightMonsterGroup(role, option_config.monster_group_id, true, false);
		}
		break;
	default://没有这个选项
		return false;
	}

	return true;
}

void NPCChosed::SendShopInfo(Role * role)
{
	Protocol::SCNPCShopInfo info;
	info.item_count = 0;
	info.obj_id = this->GetId();
	int real_item_count = 0;
	for (std::map<int, NpcShopItem>::iterator it = shop_list.begin(); it != shop_list.end() && real_item_count < Protocol::SCNPCShopInfo::MAX_SALE_ITEM_COUNT;it++)
	{
		info.item_list[real_item_count].num = it->second.num;
		info.item_list[real_item_count].max_num = it->second.max_num;
		info.item_list[real_item_count].item.item_id = it->second.item.item_id;
		info.item_list[real_item_count].item.is_bind = it->second.item.is_bind;
		info.item_list[real_item_count].item.num = it->second.item.num;
		info.item_list[real_item_count].item.money_type = it->second.item.money_type;
		info.item_list[real_item_count].item.price = it->second.item.price;
		info.item_list[real_item_count].item.discount = it->second.item.discount;

		real_item_count += 1;
	}
	info.item_count = real_item_count;
	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info) - ((Protocol::SCNPCShopInfo::MAX_SALE_ITEM_COUNT - real_item_count)*sizeof(info.item_list[0])));
}

bool NPCChosed::OnBuyItem(Role* role, int item_index,int num)
{
	if (role == NULL)return false;
	std::map<int, NpcShopItem>::iterator it = shop_list.find(item_index);
	if (it == shop_list.end())
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_ITEM_IS_INVALID);
		return false;
	}

	//检查消耗
	NpcShopItem &comsue_item = it->second;
	if (comsue_item.num < num)
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return false;
	}
	Money *  role_money = role->GetRoleModuleManager()->GetMoney();
	//检查给道具
	if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(comsue_item.item.item_id, comsue_item.item.num, false))
	{
		role->GetRoleModuleManager()->GetKnapsack()->SendErrorNotice();
		//role->GetRoleModuleManager()->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}
	int cost_num = comsue_item.item.price * num;
	switch (comsue_item.item.money_type)
	{
	case 0://元宝
		if (!role_money->UseGold(cost_num, "NPCChosed::OnBuyItem"))
		{
			role->GetRoleModuleManager()->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return false;
		}
		break;
	case 1://仙币
		if (!role_money->UseImmortalCoin(cost_num, "NPCChosed::OnBuyItem"))
		{
			role->GetRoleModuleManager()->NoticeNum(errornum::EN_IMMORTAL_COIN_NOT_ENOUGH);
			return false;
		}
		break;
	case 2://铜币
		if (!role_money->UseCoinBind(cost_num, "NPCChosed::OnBuyItem"))
		{
			role->GetRoleModuleManager()->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return false;
		}
		break;
	default:
		break;
	}
	
	if (!role->GetRoleModuleManager()->GetKnapsack()->Put(ItemConfigData(comsue_item.item.item_id, comsue_item.item.is_bind == 1, comsue_item.item.num * num), PUT_REASON_NPC_SHOP))
	{
		role->GetRoleModuleManager()->GetKnapsack()->SendErrorNotice();
		return false;
	}
	

	if(comsue_item.num > 0)comsue_item.num -= num;

	role->GetRoleModuleManager()->NoticeNum(noticenum::NT_NPC_SHOP_BUY_SUCC);

	//下发物品列表--刷新周围玩家的npc商店
	this->SendShopInfo(role);

	gamelog::g_log_npc.printf(LL_INFO, "NPCChosed::OnBuyItem role_name[%s] role_id[%d] npc_id[%d] npc_scene[%d] item_index[%d] buyitem[id:%d num:%d isbind:%d,can_buy:%d money_type:%d price:%d discount:%d]",
		role->GetName(),role->GetUID(),this->GetNpcId(), this->GetScene()->GetSceneID(),
		item_index, comsue_item.item.item_id, comsue_item.item.num, comsue_item.item.is_bind, 
		comsue_item.num, comsue_item.item.money_type, comsue_item.item.price,comsue_item.item.discount);
	return true;
}

void NPCChosed::AddBuyItem(NpcShopItem* buy_item)
{
	if (buy_item == NULL)return;
	NpcShopItem cfg;
	cfg.item.item_id = buy_item->item.item_id;
	cfg.item.num = buy_item->item.num;
	cfg.item.is_bind = buy_item->item.is_bind ? 1 : 0;
	cfg.item.money_type = buy_item->item.money_type;
	cfg.item.price = buy_item->item.price;
	cfg.num = buy_item->num;
	cfg.max_num = buy_item->num;//加入物品时，限购数量为可购买数量
	cfg.item.discount = buy_item->item.discount;
	shop_list.insert(std::pair<int, NpcShopItem>(shop_num++, cfg));
	gamelog::g_log_npc.printf(LL_INFO, "NPCChosed::AddBuyItem npc_id[%d] npc_scene[%d] item_count[%d] additem[id:%d num:%d isbind:%d can_buy:%d money_type:%d price:%d]",
		this->GetNpcId(), this->GetScene()->GetSceneID(), shop_num, cfg.item.item_id, cfg.item.num, cfg.item.is_bind, cfg.num,cfg.item.money_type,cfg.item.price);
}

void NPCChosed::RemoveAfterBuy()
{
	m_scene->DeleteObj(m_obj_id);
}
