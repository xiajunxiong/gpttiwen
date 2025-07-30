#include "rolecrystalfb.h"
#include "world.h"
#include "engineadapter.h"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "other/rolecrystalfb/rolecrystalfbconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "servercommon/errornum.h"
#include "item/knapsack.h"
#include "other/prestige/prestige.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "drop/droppool.hpp"
#include "config/randactivityconfig/impl/randactivitydropdoubleconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/drawmanager/drawmanager.hpp"
#include "config/otherconfig/knapsack_config.hpp"
#include "item/expense/norexitem.hpp"
#include "other/workshop/workshop.hpp"
#include "other/roledrop/roledrop.hpp"
#include "servercommon/string/gameworldstr.h"
#include "scene/scenemanager.hpp"
#include "other/event/eventhandler.hpp"
#include "protocol/msgdraw.h"
#include "other/levelcomplement/levelcomplement.h"
#include "other/funopen/funopen.hpp"
#include "gamelog.h"
#include "servercommon/roleconvertshopparam.hpp"
#include "other/dayreward/dayreward.hpp"

RoleCrystalFb::RoleCrystalFb() :m_role_module_mgr(NULL)
{
}

RoleCrystalFb::~RoleCrystalFb()
{
}

void RoleCrystalFb::Init(RoleModuleManager * mgr, const RoleCrystalParam & param)
{
	m_role_module_mgr = mgr;
	m_param = param;
	if (m_param.change_flag < 1)
	{
		this->OnDayChange(0, 1);//帮已经今天已经登录过的玩家重新刷新一次水晶副本
		m_param.change_flag = 1;
	}
}

void RoleCrystalFb::GetInitParam(RoleCrystalParam * param)
{
	*param = m_param;
}

void RoleCrystalFb::OnLogin(unsigned int login_timestamp, unsigned int last_save_timestamp)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_CRYSTAL_FB))return;
	if (m_param.next_energy_reply_tamp <= login_timestamp)
	{
		const CrystalFbOtherCfg &  other_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg();
		const PowerUpCfg * power_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetPowerUpCfg(m_role_module_mgr->GetRole()->GetAdvanceTimes());
		if (power_cfg == NULL)return;
		if (m_param.role_enter_energy < power_cfg->power_up)
		{
			for (int i = m_param.role_enter_energy; i < power_cfg->power_up; i++)
			{
				m_param.next_energy_reply_tamp += other_cfg.recovery_time;
				if (m_param.next_energy_reply_tamp <= login_timestamp)
				{
					m_param.role_enter_energy += other_cfg.recovery_power;
					if (m_param.role_enter_energy >= power_cfg->power_up)
					{
						m_param.last_full_energy_timestamp = m_param.next_energy_reply_tamp;
						gamelog::g_log_energy.printf(LL_INFO, "%s role[%d, %s] last_full_energy_timestamp[%d] overflow start!", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
							m_param.last_full_energy_timestamp);

						m_param.next_energy_reply_tamp = 0;
						break;
					}
				}
				else
				{
					break;
				}
			}
		}
	}
	if (0 == m_param.is_syn_xi_you_play_times)
	{
		m_param.get_xi_you_play_times = GetMin(m_param.new_xi_you_play_times, m_param.use_xi_you_play_times);
		m_param.is_syn_xi_you_play_times = 1;
	}
	this->SendCrystalFbEnergy();
}

void RoleCrystalFb::Update(time_t now_second)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_CRYSTAL_FB))return;
	const CrystalFbOtherCfg &  other_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg();
	const PowerUpCfg * power_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetPowerUpCfg(m_role_module_mgr->GetRole()->GetAdvanceTimes());
	if (power_cfg == NULL)return;
	if (m_param.frist_fun_open == 0)
	{
		if (other_cfg.initial_power + m_param.role_enter_energy < power_cfg->power_up)
		{
			m_param.role_enter_energy += other_cfg.initial_power;
		}
		else
		{
			m_param.role_enter_energy = power_cfg->power_up;
		}
		m_param.frist_fun_open = 1;
		m_param.next_energy_reply_tamp = static_cast<unsigned int>(now_second + other_cfg.recovery_time);
		this->SendCrystalFbEnergy();
	}
	if (m_param.next_energy_reply_tamp <= now_second)
	{
		if (m_param.role_enter_energy < power_cfg->power_up)
		{
			if (0 != m_param.next_energy_reply_tamp)		//防止满体力时进阶后上限修改直接加
			{
				if (other_cfg.recovery_power + m_param.role_enter_energy < power_cfg->power_up)
				{
					m_param.role_enter_energy += other_cfg.recovery_power;
				}
				else
				{
					m_param.role_enter_energy = power_cfg->power_up;
				}
			}
			m_param.next_energy_reply_tamp = static_cast<unsigned int>(now_second + other_cfg.recovery_time);
			if (m_param.role_enter_energy >= power_cfg->power_up)
			{
				m_param.last_full_energy_timestamp = static_cast<unsigned int>(now_second);
				m_param.next_energy_reply_tamp = static_cast<unsigned int>(now_second);
			}
			this->SendCrystalFbEnergy();
		}
	}



// 	if (m_param.ondaychange_flag == 0)
// 	{
// 		if (m_param.last_full_energy_timestamp != 0)
// 		{
// 			m_role_module_mgr->GetCommonData().overflow_find_count += ((GetZeroTime(m_param.last_full_energy_timestamp) + SECOND_PER_DAY) - m_param.last_full_energy_timestamp) / other_cfg.recovery_time;
// 
// 			if (GetZeroTime(m_param.last_full_energy_timestamp + SECOND_PER_DAY) != GetZeroTime(now_second))
// 			{
// 				m_role_module_mgr->GetCommonData().overflow_find_count = SECOND_PER_DAY / other_cfg.recovery_time;
// 			}
// 		}
// 
// 		m_param.last_full_energy_timestamp = 0;
// 		if (m_param.role_enter_energy >= power_cfg->power_up)
// 		{
// 			m_param.last_full_energy_timestamp = GetZeroTime(now_second);
// 		}
// 
// // 		m_role_module_mgr->GetCommonData().overflow_find_count = 0;
// // 		m_role_module_mgr->GetCommonData().overflow_find_count = m_param.overflow_energy;
// // 		m_param.overflow_energy = 0;
// 
// 		m_param.ondaychange_flag = 1;
// 	}

	if (0 != m_param.last_full_energy_timestamp && m_param.role_enter_energy < power_cfg->power_up)
	{
		m_param.overflow_energy += (now_second - m_param.last_full_energy_timestamp) / other_cfg.recovery_time;
		m_param.last_full_energy_timestamp = 0;
	}
}

void RoleCrystalFb::OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid)
{
	
}

void RoleCrystalFb::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if(old_dayid == now_dayid) return;

	m_param.DayReset();

	this->SendInfo();

	const CrystalFbOtherCfg & other_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg();
	if (CONVERT_LIMIT_TYPE_DAILY == other_cfg.limit_type)
	{
		m_param.buy_energy_times = 0;
	}
	m_param.new_xi_you_play_times = other_cfg.free_reward_time;
	this->SendCrystalFbEnergy();
	
}

void RoleCrystalFb::OnWeekChange()
{
	const CrystalFbOtherCfg & other_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg();
	if (CONVERT_LIMIT_TYPE_WEEKLY == other_cfg.limit_type)
	{
		m_param.buy_energy_times = 0;
		this->SendCrystalFbEnergy();
	}
}

void RoleCrystalFb::OnMonthChange()
{
	const CrystalFbOtherCfg & other_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg();
	if (CONVERT_LIMIT_TYPE_MONTHLY == other_cfg.limit_type)
	{
		m_param.buy_energy_times = 0;
		this->SendCrystalFbEnergy();
	}
}

void RoleCrystalFb::OnBuyEnergy()
{
	const CrystalFbOtherCfg & other_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg();
	if (m_param.buy_energy_times > other_cfg.buy_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_CRYSTAL_BUY_ENERGY_TIMES_NOT_ENOUGH);
		return;
	}
	if (m_param.role_enter_energy >= other_cfg.extra_power_limit)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_USE_CRYSTAL_FB_ENERGY_ITEM_UPPER_LIMIT);
		return;
	}
	const BuyEnergyCfg * buy_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetBuyEnergyCfg(m_param.buy_energy_times + 1);
	if (NULL == buy_cfg)
	{
		return;
	}
	if (m_param.role_enter_energy + buy_cfg->get_power > other_cfg.extra_power_limit)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_CRYSTAL_BUY_ENERGY_REACH_LIMIT);
		return;
	}

	if (buy_cfg->price > 0)
	{
		if (!m_role_module_mgr->GetMoney()->UseGold(buy_cfg->price, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
	}
	m_param.buy_energy_times++;
	this->ModifyCrystalFBEnergy(buy_cfg->get_power, __FUNCTION__, ENTER_ENERGY_CHANGE_TYPE_BUY);

	this->SendCrystalFbEnergy();
}

void RoleCrystalFb::UseFastPassFb(int fb_seq, int times)
{
	if (!this->HasFristPassCrystalFb(fb_seq) || 1 != times) //策划要求，扫荡只能单次
	{
		return;
	}

	if (!this->CanChallengeNormalFB(fb_seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_CRYSTAL_FB_NORMAL_TIMES_LIMIT);
		return;
	}

	if (!Scene::IsStaticScene(m_role_module_mgr->GetRole()->GetScene()->GetSceneType()))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_SCENE_NOT_CAN_FAST_PASS_FB);
		return;
	}

	const RoleCrystalFbLevelCfg  * cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(fb_seq);
	if (cfg == NULL|| cfg->type != ROLE_CRYSTAL_FB_TYPE_NORMAL)return;

	if (m_role_module_mgr->GetRole()->GetCapabilityWithFightPetAndPartner() < cfg->recommend_score)
	{
		m_role_module_mgr->OnSendScoreHintNotice(cfg->recommend_score);
		return;
	}

	const ServerNPCInfoCfg* npc_cfg = NPCPOOL->GetServerNPCInfo(cfg->boss_npc_id);
	if (NULL == npc_cfg) return;

	const CrystalFbOtherCfg & other_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg();
	//先消耗道具，不够就补体力

	Knapsack * m_knapsack = m_role_module_mgr->GetKnapsack();
	if (m_knapsack == NULL)return;
	int item_num = m_knapsack->Count(cfg->comsume_item_id);
	int item_can_times = item_num / cfg->comsume_item_num;
	//不够的使用体力
	int need_item_times = 0;
	int need_energy = 0;
	if (item_can_times >= times)
	{
		if (!m_role_module_mgr->GetKnapsack()->HasItem(cfg->comsume_item_id, cfg->comsume_item_num * times))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_FB_STUFF_NOT_ENOUGH);
			return;
		}
		need_item_times = times;
	}
	else
	{
		need_energy = (times - item_can_times) * other_cfg.use_power;

		if (m_param.role_enter_energy < need_energy)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ROLE_CRYSTAL_FB_ENERGY_NOT_ENOUGH);
			return;//体力也不够
		}

		if (item_can_times > 0 && !m_role_module_mgr->GetKnapsack()->HasItem(cfg->comsume_item_id, cfg->comsume_item_num * item_can_times))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_FB_STUFF_NOT_ENOUGH);
			return;
		}

		need_item_times = item_can_times;
	}

	int village_id = 0;
	int prestige_num = 0;
	int add_exp = 0;
	std::vector<ItemConfigData> must_has_reward;

	std::vector<int> drop_id_list = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetMonsterDropId(cfg->boss_npc_id, 0, &village_id, &prestige_num, &add_exp, &must_has_reward);
	std::vector<int> real_drop_list;
	//std::vector<int> workshop_reward;
	if (ROLE_CRYSTAL_FB_TYPE_NORMAL == cfg->type)
	{
		//一颗封印石头相当于一次消耗的体力数
		add_exp = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetExpCfg(m_role_module_mgr->GetRole()->GetLevel()) * (need_energy + need_item_times * other_cfg.use_power);	
	}

	if (village_id != 0 && prestige_num > 0)m_role_module_mgr->GetPrestige()->AddPrestige(village_id, prestige_num * times, __FUNCTION__);

	//活动 - 掉落翻倍
	int drop_double_num = 1;
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL != ramgr)
	{
		if (ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_DROP_DOUBLE))
		{
			const RandActivityDropDoubleConfig * config = static_cast<const RandActivityDropDoubleConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_DROP_DOUBLE));
			if (NULL != config)
			{
				const DropDoubleCfg * drop_double_cfg = config->GetDropPeomoteCfg(ramgr, cfg->seq);
				if (NULL != drop_double_cfg)
				{
					drop_double_num = drop_double_cfg->drop_promote;

					RandActivity* act = ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_SEAL_PROMOTE);
					if (NULL != act)
					{
						act->OnRoleParticipateRA(m_role_module_mgr->GetRole(), __FUNCTION__);
					}
				}
			}
		}
	}
	
//	int now_times = this->GetPuTongJingYingPlayTimes();
//	for (int i = 0; i < times; i++)
//	{
	if (cfg->type == ROLE_CRYSTAL_FB_TYPE_JING_YING)
	{
		drop_id_list.clear();
		drop_id_list = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetMonsterDropId(cfg->boss_npc_id, this->GetJingYingPlayTimes());
	}
	else if (cfg->type == ROLE_CRYSTAL_FB_TYPE_NORMAL)
	{
		drop_id_list.clear();
		drop_id_list = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetMonsterDropId(cfg->boss_npc_id, 0);
	}
	real_drop_list.insert(real_drop_list.end(), drop_id_list.begin(), drop_id_list.end());//所有掉落记录

	DrawGroupInfo draw_group_info;
	Protocol::SCDrawPokerInfo info;
	DrawCrystalPoker crystal_poker_info[SIDE_MAX_ROLE_NUM];
	for (int ind = 0; ind < SIDE_MAX_ROLE_NUM && ind < DRAW_GROUP_MAX_ITEM_NUM; ind++)	//空位也要随机牌
	{
		draw_group_info.draw_count++;
		DrawGroupInfo::DrawItem &reward_info = draw_group_info.draw_info_list[ind];
		GameName &role_name = crystal_poker_info[ind].role_name;
		int prof = 0;
		if (0 == ind)
		{
			reward_info.role_uid = m_role_module_mgr->GetRole()->GetUID();
			m_role_module_mgr->GetRole()->GetName(role_name);
			prof = m_role_module_mgr->GetRole()->GetAppearanceProf();
		}
		else
		{
			reward_info.role_uid = 0;
			memset(role_name, 0, sizeof(role_name));
		}
		int index = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRewardColumn(this->GetPuTongJingYingPlayTimes()) - 1;
		if(ROLE_CRYSTAL_FB_TYPE_NORMAL == cfg->type)index = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRewardColumn(0) - 1;
		if (ROLE_CRYSTAL_FB_TYPE_JING_YING == cfg->type)index = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRewardColumn(this->GetJingYingPlayTimes()) - 1;
		if (index < 0 || index >= (int)cfg->poker_reward_group_vec.size())
		{
			continue;
		}
		const ItemConfigData * item_data = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRandNpcReward(cfg->poker_reward_group_vec[index]);
		std::vector<ItemConfigData> reward_list;
		if (NULL != item_data)
		{
			reward_list.push_back(*item_data);
			if (drop_double_num > 1)
			{
				reward_list[(int)reward_list.size() - 1].num *= drop_double_num;
			}
			DrawManager::Instance().PutRewardItem(reward_info, (int)reward_list.size(), &reward_list[0], prof);
		}
		if (!reward_info.item_list.empty())
		{
			DrawManager::Instance().GetItemInfoType(crystal_poker_info[ind].reward_info, reward_info.item_list[0]);
		}
		crystal_poker_info[ind].index = reward_info.draw_index;
		crystal_poker_info[ind].role_uid = reward_info.role_uid;
	}

//	}

	Int64 add_coin = 0;
	Int64 add_gold = 0;
	Int64 add_immortal_coin = 0;
	std::vector<ItemConfigData> rand_item_list;
	DropPool::Instance()->RandOutDropItemList(m_role_module_mgr->GetRole(), real_drop_list, npc_cfg->server_npc_param.monster_param.monster_group_id,
		&rand_item_list, drop_double_num, &add_coin, &add_gold, &add_immortal_coin);
	EventHandler::Instance().OnPassCrystalEctype(m_role_module_mgr->GetRole(), cfg->type, cfg->boss_npc_id, times);
	EventHandler::Instance().OnRolePassCrystalFB(m_role_module_mgr->GetRole(), cfg->seq, times);
	EventHandler::Instance().OnRolePassCrystalFB2(m_role_module_mgr->GetRole(), cfg->seq, cfg->type, cfg->fb_index);
	std::map<int, ItemConfigData> real_item_map;

	if (must_has_reward.size() > 0)
	{
		for (std::vector<ItemConfigData>::iterator it = must_has_reward.begin();it != must_has_reward.end();it++)
		{
			it->num *= times;
		}
		rand_item_list.insert(rand_item_list.end(), must_has_reward.begin(), must_has_reward.end());
	}

	for (std::vector<ItemConfigData>::iterator it = rand_item_list.begin(); it != rand_item_list.end(); it++)//掉落累计
	{
		ItemConfigData &node = real_item_map[it->item_id];
		node.item_id = it->item_id;
		node.is_bind = it->is_bind;
		node.num += it->num;
	}

	rand_item_list.clear();
	for (std::map<int, ItemConfigData>::iterator it = real_item_map.begin(); it != real_item_map.end(); it++)
	{
		rand_item_list.push_back(it->second);
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)rand_item_list.size(), &rand_item_list[0]))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	if (need_item_times > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg->comsume_item_id, cfg->comsume_item_num * need_item_times, "RoleCrystalFb::UseFastPassFb"))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_FB_STUFF_NOT_ENOUGH);
		return;
	}

	if (add_exp > 0)
	{
		m_role_module_mgr->GetRole()->AddExp(add_exp * times, ADD_EXP_TYPE_CRYSTAL_FB, __FUNCTION__);
		m_role_module_mgr->GetLevelComplement()->OnAddExp(add_exp * times);
		EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_role_module_mgr->GetRole(), add_exp * times, __FUNCTION__);
	}
	if (need_energy > 0)
	{
		m_param.role_enter_energy -= need_energy;
		EventHandler::Instance().OnUseCrystalEnergy(m_role_module_mgr->GetRole(), need_energy, ENTER_ENERGY_CHANGE_TYPE_CRYSTAL_FAST);
	}

	if (ROLE_CRYSTAL_FB_TYPE_NORMAL == cfg->type)
	{
		this->AddNormalFBRecord(cfg->seq);
		this->SendInfo();
	}
	this->OnRoleChangeProfOrEneryChange(true);

	{
		//real_drop_list.clear();//掉落已经随机过了，传空的就行
		DropPool::Instance()->DropListToRoleAddReveal(m_role_module_mgr->GetRole(), draw_group_info.draw_info_list[0].item_list, GET_REASON_FAST_PASS_CRYSTAL_FB, &rand_item_list,
			PUT_REASON_CRYSTAL_FB_FAST_PASS, add_exp * times, times);
		m_role_module_mgr->GetMoney()->Add(add_gold, add_immortal_coin, add_coin, __FUNCTION__);
		if (ROLE_CRYSTAL_FB_TYPE_NORMAL == cfg->type)this->AddPuTongJingYingPlayTimes(times);
		if (ROLE_CRYSTAL_FB_TYPE_JING_YING == cfg->type)this->AddJingYingPlayTimes(times);
		DrawManager::Instance().AddDrawGroup(draw_group_info, DRAW_GROUP_TYPE_CRYSTAL_FAST_PASS);
		info.draw_group_id = draw_group_info.draw_group_id;
		info.timeout = draw_group_info.out_time;
		info.draw_group_type = DRAW_GROUP_TYPE_CRYSTAL_FAST_PASS;
		for (int k = 0; k < SIDE_MAX_ROLE_NUM; k++)
		{
			memcpy(info.draw_poker[k].data, &crystal_poker_info[k], sizeof(DrawCrystalPoker));
		}

		EngineAdapter::Instance().NetSendSerializeMsg2(m_role_module_mgr->GetRole()->GetNetId(), &info);
	}

	//发放配方奖励--这个道具只会出现一次，不进背包
	const std::vector<WorkshopItemConfig> * workshop_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetWorkshopItemCfg(cfg->formula_drop_group);
	if (NULL != workshop_cfg)
	{
		int rand_weight = rand() % WorkshopItemConfig::MAX_RAND_WEIGHT_NUM;
		int weight_count = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetWorkshopItemWeightCfg(cfg->formula_drop_group);
		if (rand_weight <= weight_count)
		{
			for (std::vector<WorkshopItemConfig>::const_iterator it = workshop_cfg->begin(); it != workshop_cfg->end(); it++)
			{
				if (it->rate >= rand_weight)//得到奖励
				{
					for (int prof_index = 0; prof_index < PROFESSION_TYPE_NUM; prof_index++)
					{
						if (PROF_TYPE_INVALID == prof_index) continue;

						int new_item = LOGIC_CONFIG->GetKnapsackConfig()->GetItemIdConvertByJob(it->item_id, prof_index);
						const ItemBase * gift_itembase = ITEMPOOL->GetItem(new_item);
						if (gift_itembase != NULL)
						{
							if (gift_itembase->GetItemType() == I_TYPE_EXPENSE)
							{
								const NorexItem * norex_item = (NorexItem *)gift_itembase;
								if (norex_item->GetUseType() == NorexItem::I_NOREX_ITEM_WORKSHOP_RECIPE)
								{
									//检查是否已经获得这个奖励了
									if (!m_role_module_mgr->GetWorkshop()->RecipeIsActive(norex_item->GetParam1()))
									{
										if (!m_role_module_mgr->GetRoleDrop()->IsLockReceipeDrop(gift_itembase->GetItemId()))
										{	
											bool is_notice = m_role_module_mgr->GetRole()->GetAppearanceProf() == prof_index ? true : false;
											//自己职业的才提示
											m_role_module_mgr->GetKnapsack()->Put(ItemConfigData(gift_itembase->GetItemId(), 1, 1), PUT_REASON_CRYSTAL_FB, 0, 0, 0, 0, is_notice);
											if(is_notice)
											{
												Protocol::SCSystemMsg sm;
												int sendlen = 0;
												int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_crytals_reward2_content, new_item);
												if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT)))
												{
													EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*)&sm, sendlen);
												}
											}
										}
									}
								}
							}
						}
					}
					break;
				}
				rand_weight -= it->rate;
			}
		}
	}

}

bool RoleCrystalFb::UseCrystalFbEnergy(int energy_value, const char* reason, int enter_energy_change_type)
{
	if (energy_value > m_param.role_enter_energy)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_CRYSTAL_FB_ENERGY_NOT_ENOUGH);
		return false;
	}

	this->ModifyCrystalFBEnergy(-energy_value, __FUNCTION__, enter_energy_change_type);
	EventHandler::Instance().OnUseCrystalEnergy(m_role_module_mgr->GetRole(), energy_value, enter_energy_change_type);

	this->OnRoleChangeProfOrEneryChange();
	this->SendCrystalFbEnergy();
	return true;
}

void RoleCrystalFb::UseCrystalFbEnergyItem(ItemID item_id, short num)
{
	if(num <= 0) return;

	const ItemBase* item_base = ITEMPOOL->GetItem(item_id);
	if(NULL == item_base || item_base->GetItemType() != I_TYPE_EXPENSE) return;

	const NorexItem * norex_item = (NorexItem *)item_base;
	if(NorexItem::I_NOREX_ITEM_RECOVERY_POWER != norex_item->GetUseType()) return;

	int add_energy_value = norex_item->GetParam1();
	if(add_energy_value <= 0)  return;

	int has_count = m_role_module_mgr->GetKnapsack()->Count(item_id);
	if (has_count < num)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}
	const CrystalFbOtherCfg & other_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg();
	if (m_param.role_enter_energy >= other_cfg.extra_power_limit)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_USE_CRYSTAL_FB_ENERGY_ITEM_UPPER_LIMIT);
		return;
	}

	int consume_num = 0;
	for (int i = 0; i < num && i < has_count; i++)
	{
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(item_id, 1, __FUNCTION__))
		{
			break;
		}
		
		this->ModifyCrystalFBEnergy(add_energy_value, __FUNCTION__, ENTER_ENERGY_CHANGE_TYPE_USE_ITEM);
		consume_num++;

		if (m_param.role_enter_energy > other_cfg.extra_power_limit)
		{
			m_param.role_enter_energy = other_cfg.extra_power_limit;
			break;
		}
		if (m_param.role_enter_energy < 0)
		{
			m_param.role_enter_energy = 0;
			break;
		}	
	}
	if(consume_num <= 0) return;

	if (m_param.role_enter_energy >= other_cfg.max_power)
	{
		m_param.last_full_energy_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_param.next_energy_reply_tamp = 0;
	}

	this->SendCrystalFbEnergy();
}

bool RoleCrystalFb::AddCrystalFbEnergyByItem(int energy_value, const char* reason)
{
	const CrystalFbOtherCfg & other_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg();
	if (m_param.role_enter_energy >= other_cfg.extra_power_limit)
	{
		return false;
	}

	this->ModifyCrystalFBEnergy(energy_value, reason, ENTER_ENERGY_CHANGE_TYPE_USE_ITEM);
	this->SendCrystalFbEnergy();
	return true;
}

void RoleCrystalFb::AddCrystalFbEnergy(int energy_value, const char* reason, int enter_energy_change_type, bool is_notice /* = true */)
{
	if (energy_value <= 0)
	{
		gamelog::g_log_energy.printf(LL_WARNING, "RoleCrystalFb::AddCrystalFbEnergy value<=0 ERROR? role[%d,%s] energy_value[%d] reason[%s]",
			m_role_module_mgr->GetRoleUID(), m_role_module_mgr->GetRoleName(), energy_value, reason ? reason : "NULL");
		return;
	}

	this->ModifyCrystalFBEnergy(energy_value, reason, enter_energy_change_type, is_notice);
	this->SendCrystalFbEnergy();
}

void RoleCrystalFb::ModifyCrystalFBEnergy(int energy_value, const char* reason, int enter_energy_change_type, bool is_notice /* = true */)
{
	const CrystalFbOtherCfg & other_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg();

	m_param.role_enter_energy += energy_value;
	if (m_param.role_enter_energy > other_cfg.extra_power_limit)
	{
		m_param.role_enter_energy = other_cfg.extra_power_limit;
	}

	if (m_param.role_enter_energy < 0)
	{
		m_param.role_enter_energy = 0;
	}

	const PowerUpCfg * power_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetPowerUpCfg(m_role_module_mgr->GetRole()->GetAdvanceTimes());
	if (power_cfg != NULL)
	{
		if (m_param.role_enter_energy >= power_cfg->power_up)
		{
			m_param.last_full_energy_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
			m_param.next_energy_reply_tamp = 0;
		}
	}

	gamelog::g_log_energy.printf(LL_INFO, "RoleCrystalFb::ModifyCrystalFBEnergy role[%d,%s] add_energy[%d] cur_energy[%d] enter_energy_change_type[%d] reason[%s]",
		m_role_module_mgr->GetRoleUID(), m_role_module_mgr->GetRoleName(), energy_value, m_param.role_enter_energy, enter_energy_change_type, reason ? reason : "NULL");

	if (energy_value > 0 && is_notice)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_energy_add_notice, energy_value);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(m_role_module_mgr->GetRole(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
		}
	}
}

void RoleCrystalFb::GetNormalFBRecord(ARG_OUT int* record_num, ARG_OUT CrystalFBNormalRecord* record_list, int max_num)
{
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.normal_fb_record_list) && *record_num < max_num; ++i)
	{
		if (-1 == m_param.normal_fb_record_list[i].fb_seq)
		{
			break;
		}

		record_list[(*record_num)++] = m_param.normal_fb_record_list[i];
	}
}

void RoleCrystalFb::GetNormalFBRecordPro(ARG_OUT int * record_num, ARG_OUT Protocol::SCCrystalFbInfo::CrystalFBNormalRecordPro * record_list, int max_num)
{
	UNSTD_STATIC_CHECK(ARRAY_ITEM_COUNT(m_param.normal_fb_record_list) == ARRAY_ITEM_COUNT(m_param.normal_fb_buy_times_list));

	std::map<int, Protocol::SCCrystalFbInfo::CrystalFBNormalRecordPro> crystal_fb_normal_record_list;
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.normal_fb_record_list) && i < ARRAY_ITEM_COUNT(m_param.normal_fb_buy_times_list); i++)
	{
		if (-1 == m_param.normal_fb_record_list[i].fb_seq && -1 == m_param.normal_fb_buy_times_list[i].fb_seq)
		{
			break;
		}
		crystal_fb_normal_record_list[m_param.normal_fb_record_list[i].fb_seq].challenge_times = m_param.normal_fb_record_list[i].times;
		crystal_fb_normal_record_list[m_param.normal_fb_buy_times_list[i].fb_seq].buy_times = m_param.normal_fb_buy_times_list[i].times;
	}
	
	if (crystal_fb_normal_record_list.empty())
	{
		return;
	}
	std::map<int, Protocol::SCCrystalFbInfo::CrystalFBNormalRecordPro>::iterator it = crystal_fb_normal_record_list.begin();
	for (; it != crystal_fb_normal_record_list.end() && *record_num < max_num; ++it)
	{
		if (-1 == it->first)
		{
			continue;
		}
		it->second.fb_seq = it->first;
		record_list[(*record_num)++] = it->second;
	}
}

void RoleCrystalFb::AddNormalFBRecord(int seq)
{
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.normal_fb_record_list); ++i)
	{
		if (m_param.normal_fb_record_list[i].fb_seq == seq)
		{
			m_param.normal_fb_record_list[i].times += 1;
			return;
		}

		if (-1 == m_param.normal_fb_record_list[i].fb_seq)
		{
			m_param.normal_fb_record_list[i].fb_seq = static_cast<short>(seq);
			m_param.normal_fb_record_list[i].times = 1;
			return;
		}
	}

#ifdef _DEBUG
	assert(0);	// 不应出现满了的情况
#else
	gamelog::g_log_serious_error.printf(LL_ERROR, "RoleCrystalFb::AddNormalFBRecord RecordList FULL, role[%d,%s] seq[%d]",
		m_role_module_mgr->GetRoleUID(), m_role_module_mgr->GetRoleName(), seq);
#endif

}

bool RoleCrystalFb::CanChallengeNormalFB(int seq)
{
	int buy_times = 0;
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.normal_fb_buy_times_list); i++)
	{
		if (-1 == m_param.normal_fb_buy_times_list[i].fb_seq)
		{
			break;
		}
		if (seq == m_param.normal_fb_buy_times_list[i].fb_seq)
		{
			buy_times = m_param.normal_fb_buy_times_list[i].times;
			break;
		}
	}
	int limit_times = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg().normal_fb_day_pass_time_max + buy_times;
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.normal_fb_record_list); ++i)
	{
		if (-1 == m_param.normal_fb_record_list[i].fb_seq)
		{
			return true;
		}

		if (seq == m_param.normal_fb_record_list[i].fb_seq)
		{
			return m_param.normal_fb_record_list[i].times < limit_times;
		}
	}

	return true;
}

void RoleCrystalFb::OverFlowEnergyCal()
{
	const CrystalFbOtherCfg &  other_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg();
	const PowerUpCfg * power_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetPowerUpCfg(m_role_module_mgr->GetRole()->GetAdvanceTimes());
	if (power_cfg == NULL)return;

	if (m_param.ondaychange_flag == 0)
	{
		if (m_param.last_full_energy_timestamp != 0)
		{
			if (m_param.overflow_energy == 0)
			{
				m_param.overflow_energy = (EngineAdapter::Instance().Time() - m_param.last_full_energy_timestamp) / other_cfg.recovery_time;
			}
			else
			{
				m_param.overflow_energy += (EngineAdapter::Instance().Time() - m_param.last_full_energy_timestamp) / other_cfg.recovery_time;
			}

//			m_param.overflow_energy += ((GetZeroTime(m_param.last_full_energy_timestamp) + SECOND_PER_DAY) - m_param.last_full_energy_timestamp) / other_cfg.recovery_time;

// 			if (GetZeroTime(m_param.last_full_energy_timestamp + SECOND_PER_DAY) != GetZeroTime(EngineAdapter::Instance().Time()))
// 			{
// 				m_param.overflow_energy = SECOND_PER_DAY / other_cfg.recovery_time;
// 			}
		}

		m_param.last_full_energy_timestamp = 0;
		if (m_param.role_enter_energy >= power_cfg->power_up)
		{
			m_param.last_full_energy_timestamp = GetZeroTime(EngineAdapter::Instance().Time());
		}

		if (m_param.overflow_energy > (SECOND_PER_DAY / other_cfg.recovery_time))
		{
			m_param.overflow_energy = SECOND_PER_DAY / other_cfg.recovery_time;
		}

		m_role_module_mgr->GetCommonData().overflow_find_count = 0;
		m_role_module_mgr->GetCommonData().overflow_find_count = m_param.overflow_energy;
		m_param.overflow_energy = 0;

		m_param.ondaychange_flag = 1;
	}
}


void RoleCrystalFb::SendCrystalFbEnergy()
{
	Protocol::SCCrystalFbEnergyInfo protocol;

	protocol.role_enter_energy = m_param.role_enter_energy;
	protocol.buy_times = m_param.buy_energy_times;
	protocol.reserve_sh = 0;
	protocol.next_energy_reply_tamp = m_param.next_energy_reply_tamp;
	memcpy(protocol.frist_pass_flag, m_param.frist_pass_flag, sizeof(protocol.frist_pass_flag));
	memcpy(protocol.frist_pass_reward, m_param.frist_pass_reward, sizeof(protocol.frist_pass_reward));
	protocol.new_jing_ying_play_times = m_param.new_jing_ying_play_times;
	protocol.use_xi_you_reward_play_times = m_param.get_xi_you_play_times;
	protocol.new_xi_you_play_times = m_param.new_xi_you_play_times;

	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char *)&protocol, sizeof(protocol));
}

void RoleCrystalFb::OnRoleChangeProfOrEneryChange(bool is_send)
{
	const CrystalFbOtherCfg & other_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg();
	const PowerUpCfg * power_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetPowerUpCfg(m_role_module_mgr->GetRole()->GetAdvanceTimes());
	if (power_cfg != NULL)
	{
		if (m_param.next_energy_reply_tamp == 0 && m_param.role_enter_energy < other_cfg.max_power)
		{
			m_param.next_energy_reply_tamp = static_cast<unsigned int>(EngineAdapter::Instance().Time() + other_cfg.recovery_time);
		}
	}
	if (is_send)
	{
		this->SendCrystalFbEnergy();
	}
}

int RoleCrystalFb::GetPuTongJingYingPlayTimes()
{
	return m_param.pu_tong_jing_ying_play_times;
}

int RoleCrystalFb::GetJingYingPlayTimes()
{
	return m_param.new_jing_ying_play_times;
}

void RoleCrystalFb::AddJingYingPlayTimes(int times)
{
	m_param.new_jing_ying_play_times += times;
	this->SendCrystalFbEnergy();
}

void RoleCrystalFb::AddPuTongJingYingPlayTimes(int times)
{
	m_param.pu_tong_jing_ying_play_times += times;
}

int RoleCrystalFb::GetXiYouPlayTimes()
{
	return m_param.new_xi_you_play_times;
}

int RoleCrystalFb::GetXiYourRewardPlayTimes()
{
	return m_param.get_xi_you_play_times;
}

void RoleCrystalFb::UseXiYouRewardPlayTimes(int times)
{
	m_param.get_xi_you_play_times += times;
	this->SendCrystalFbEnergy();
}

bool RoleCrystalFb::HasUseTimes()
{
	return m_param.get_xi_you_play_times < m_param.new_xi_you_play_times;
}

int RoleCrystalFb::GetXiYouUseTimes()
{
	return m_param.use_xi_you_play_times;
}

void RoleCrystalFb::UseXiYouPlayTimes(int times)
{
	m_param.use_xi_you_play_times += times;
	this->SendCrystalFbEnergy();
}

bool RoleCrystalFb::AddXiYouPlayTimesByNorexitem(int times)
{
	m_param.new_xi_you_play_times += times;
	this->SendCrystalFbEnergy();
	return true;
}

int RoleCrystalFb::GetPuTongTimesByFbSeq(int fb_seq)
{
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.normal_fb_record_list); ++i)
	{
		if (m_param.normal_fb_record_list[i].fb_seq == fb_seq)
		{
			return m_param.normal_fb_record_list[i].times;
		}
	}

	return 0;
}

void RoleCrystalFb::SetFristPassCrystalFb(int fb_seq)
{
	int index = fb_seq / 32;
	int seq = fb_seq % 32;
	m_param.frist_pass_flag[index] |= 1 << seq;
	this->SendCrystalFbEnergy();
}

bool RoleCrystalFb::HasFristPassCrystalFb(int fb_seq)
{
	int index = fb_seq / 32;
	int seq = fb_seq % 32;
	if ((m_param.frist_pass_flag[index] & (1 << seq)) > 0)
	{
		return true;
	}
	return false;
}

void RoleCrystalFb::GetRewardPassCrystalFb(int fb_seq)
{
	int index = fb_seq / 32;
	int seq = fb_seq % 32;

	if ((m_param.frist_pass_flag[index] & (1 << seq)) == 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_NOT_PASS_CRYSTAL_FB);
		return;
	}

	if ((m_param.frist_pass_reward[index] & (1 << seq)) > 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_PASS_CRYSTAL_FB_REWARD_HAS_GET);
		return;
	}

	const RoleCrystalFbLevelCfg * level_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(fb_seq);
	if (level_cfg == NULL)return;
	
	if ((short)level_cfg->first_kill_reward_vec.size() > 0)
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)level_cfg->first_kill_reward_vec.size(), &level_cfg->first_kill_reward_vec[0]))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
			return;
		}
		m_role_module_mgr->GetKnapsack()->PutList((short)level_cfg->first_kill_reward_vec.size(), &level_cfg->first_kill_reward_vec[0], PUT_REASON_CRYSTAL_FB);
	}

	m_param.frist_pass_reward[index] |= 1 << seq;
	this->SendCrystalFbEnergy();
}

void RoleCrystalFb::OnRoleEnterScene()
{
	Scene* scene = m_role_module_mgr->GetRole()->GetScene();
	if (NULL == scene || !scene->IsInStaticScene()) return;

	if (m_param.fb_ticket_data.has_give_ticket > 0)
	{
		if (0 == m_param.fb_ticket_data.has_get_boss_reward)
		{
			// 未获奖励则退票
			if (CrystalFBTicketData::TICKET_TYPE_ENERGY == m_param.fb_ticket_data.ticket_type)
			{
				bool is_notice = false; // 不飘字
				this->AddCrystalFbEnergy(m_param.fb_ticket_data.consume_energy, "OnRoleEnterStaticSceneReturnTicket", ENTER_ENERGY_CHANGE_TYPE_ADVANCE_CRYSTAL, is_notice);
			}
			else
			{
				ItemConfigData item;
				item.item_id = m_param.fb_ticket_data.consume_item_id;
				item.num = m_param.fb_ticket_data.consume_item_num;
				item.is_bind = m_param.fb_ticket_data.consume_item_is_bind > 0;

				if (!m_role_module_mgr->GetKnapsack()->Put(item, PUT_REASON_CRYSTAL_FB_RETURN_TICKET))
				{
					m_role_module_mgr->GetKnapsack()->SendMail(&item, 1);
				}
			}
		}

		m_param.fb_ticket_data.ResetTicketInfo();
	}
}

void RoleCrystalFb::OnRoleEnterFBUseEnergy(int use_energy_num)
{
	m_param.fb_ticket_data.ResetTicketInfo();
	m_param.fb_ticket_data.has_give_ticket = 1;
	m_param.fb_ticket_data.ticket_type = CrystalFBTicketData::TICKET_TYPE_ENERGY;
	m_param.fb_ticket_data.consume_energy = use_energy_num;
}

void RoleCrystalFb::OnRoleEnterFBUseItem(const ItemConfigData& item)
{
	m_param.fb_ticket_data.ResetTicketInfo();
	m_param.fb_ticket_data.has_give_ticket = 1;
	m_param.fb_ticket_data.ticket_type = CrystalFBTicketData::TICKET_TYPE_ITEM;
	m_param.fb_ticket_data.consume_item_id = item.item_id;
	m_param.fb_ticket_data.consume_item_num = item.num;
	m_param.fb_ticket_data.consume_item_is_bind = item.item_id ? 1 : 0;
}

void RoleCrystalFb::OnRoleGetFBBossReward(int seq)
{
	const RoleCrystalFbLevelCfg* cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetRoleCrystalFbLevelCfg(seq);
	if (NULL == cfg) return;

	if (ROLE_CRYSTAL_FB_TYPE_NORMAL == cfg->type)
	{
		m_param.fb_ticket_data.has_get_boss_reward = 1;
		EventHandler::Instance().OnSpecialUseEnergy(m_role_module_mgr->GetRole(), m_param.fb_ticket_data.consume_energy, ENTER_ENERGY_CHANGE_TYPE_CRYSTAL_PASS);

		this->AddNormalFBRecord(seq);
		this->SendInfo();
	}
}

void RoleCrystalFb::OnRoleBuyNormalFBTimes(int fb_seq)
{
	const CrystalFbOtherCfg & other_cfg = LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg();
	if (0 == other_cfg.extra_buy_times)
	{
		return;
	}

	for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.normal_fb_buy_times_list); ++i)
	{
		if (m_param.normal_fb_buy_times_list[i].fb_seq == fb_seq)
		{
			if(m_param.normal_fb_buy_times_list[i].times >= other_cfg.extra_buy_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_MATERIAL_LEVEL_BUY_NOT_ENOUGH);
				return;
			}
			if (!m_role_module_mgr->GetMoney()->UseGold(other_cfg.extra_buy_consume, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
			int add_times = other_cfg.extra_buy_times - m_param.normal_fb_buy_times_list[i].times;
			m_param.normal_fb_buy_times_list[i].times = other_cfg.extra_buy_times;
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_buy_extra_times_notice, add_times);
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(m_role_module_mgr->GetRole(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
			}
			this->SendInfo();
			return;
		}
		else if (-1 == m_param.normal_fb_buy_times_list[i].fb_seq)
		{
			if (!m_role_module_mgr->GetMoney()->UseGold(other_cfg.extra_buy_consume, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
			m_param.normal_fb_buy_times_list[i].fb_seq = static_cast<short>(fb_seq);
			m_param.normal_fb_buy_times_list[i].times = other_cfg.extra_buy_times;
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_buy_extra_times_notice, other_cfg.extra_buy_times);
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgPerson2(m_role_module_mgr->GetRole(), gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT));
			}
			this->SendInfo();
			return;
		}
	}

#ifdef _DEBUG
	assert(0);	// 不应出现满了的情况
#else
	gamelog::g_log_serious_error.printf(LL_ERROR, "%s buy_times_list FULL, role[%d,%s] fb_seq[%d]", __FUNCTION__,
		m_role_module_mgr->GetRoleUID(), m_role_module_mgr->GetRoleName(), fb_seq);
#endif
		
}

void RoleCrystalFb::SendInfo()
{
	Protocol::SCCrystalFbInfo protocol;

	memcpy(protocol.xi_you_fb_play_times, m_param.xi_you_fb_play_times, sizeof(protocol.xi_you_fb_play_times));

	protocol.normal_record_num = 0;
	this->GetNormalFBRecordPro(&protocol.normal_record_num, protocol.normal_fb_record_list, ARRAY_ITEM_COUNT(protocol.normal_fb_record_list));

	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char *)&protocol, sizeof(protocol));
}
