#include "roleactivitymazesummer.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "scene/fbmanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "item/knapsack.h"
#include "servercommon/errornum.h"
#include "global/randactivity/randactivitymanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "global/randactivity/randactivityimpl/randactivitymazesummer.hpp"
#include "config/randactivityconfig/impl/randactivitymazesummerconfig.hpp"
#include "battle/battle_manager_local.hpp"
#include "other/wst_question/wstquestionconfig.hpp"
#include "other/roleattribute/roleexpconfig.hpp"

RoleActivityMazeSummer::RoleActivityMazeSummer(int type)
	:RoleActivity(type)
{

}

RoleActivityMazeSummer::~RoleActivityMazeSummer()
{
}

void RoleActivityMazeSummer::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAMazeSummerParam));
}

void RoleActivityMazeSummer::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RAMazeSummerParam));
}

void RoleActivityMazeSummer::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	if (NULL == raor)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_MAZE_SUMMER))
	{
		return;
	}
	RandActivityMazeSummer * rand_activity = (RandActivityMazeSummer *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_MAZE_SUMMER);
	if (rand_activity == NULL)return;

	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_MAZE_SUMMER);

	switch (raor->opera_type)
	{
	case 0:	//下发信息
		this->SendMapInfo();
		break;
	case 1://开启游戏
		this->StartPlay();
		break;
	case 2://移动
		this->MovePos(Posi(raor->param_1,raor->param_2));
		break;
	case 3://进行特殊玩法
		this->StartSpecialPlay();
		break;
	case 4://任务下发
		this->SendTaskInfo();
		break;
	case 5://领取任务奖励
		this->GetTaskReward(raor->param_1);
		break;
	case 6://答题
		this->QuestionAnswer(raor->param_1);
		break;
	case 7://放弃本轮
		this->EndPlay();
		break;
	case 8:
		this->BuyItem(raor->param_1);
		break;
	default:
		break;
	}

}

void RoleActivityMazeSummer::OnBattleFinish(bool is_win, int battle_mode)
{
	if (is_win && battle_mode == BATTLE_MODE_MAZE_SUMMER)
	{
		const RandActivityMazeSummerConfig * config = static_cast<const RandActivityMazeSummerConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_MAZE_SUMMER));
		if (config == NULL)return;
		RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
		if (NULL == ramgr) return;

		if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_MAZE_SUMMER))
		{
			return;
		}

		int open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_MAZE_SUMMER);

		RAMazeSummerMap *node_map = this->GetMyPosMapInfo();
		if (node_map == NULL)return;
		int grid = this->PosiToGrid(m_param.m_pos);
		const RAMazeSummerMapCfg * map_cfg = config->GetMazeSummerMapCfgByDayAndGrid(open_day, grid);
		if (map_cfg == NULL)return;

		//发奖励
		const RAMazeSummerRewardGroupCfg * reward_cfg = config->GetMazeSummerRewardCfg(map_cfg->reward_group);
		if (NULL != reward_cfg)
		{
			if (reward_cfg->m_reward_vec.size() > 0)
			{
				if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)reward_cfg->m_reward_vec.size(), &reward_cfg->m_reward_vec[0], false))
				{
					//发邮件
					m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->m_reward_vec[0], (short)reward_cfg->m_reward_vec.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_MAZE_SUMMER);
				}
				else
				{
					m_role_module_mgr->GetKnapsack()->PutList((short)reward_cfg->m_reward_vec.size(), &reward_cfg->m_reward_vec[0], PUT_REASON_MAZE_SUMMER);
				}
			}

			if (reward_cfg->exp > 0)
			{
				const RoleLevelCfg* cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetLevelCfg(m_role_module_mgr->GetRole()->GetLevel());
				if (NULL != cfg)
				{
					long long add_exp = cfg->exp * reward_cfg->exp / 10000;
					m_role_module_mgr->GetRole()->AddExp(add_exp, ADD_EXP_TYPE_MAZE_SUMMER, __FUNCTION__);
				}
			}

			if (reward_cfg->coin > 0)
			{
				m_role_module_mgr->GetMoney()->AddCoinBind(reward_cfg->coin, __FUNCTION__);
			}
		}
		this->AddTaskParam(RAMAZE_SUMMER_TASK_TYPE_2);
		this->AddTaskParam(RAMAZE_SUMMER_TASK_TYPE_3);
		if (node_map->map_type == RA_MAZE_SUMMER_MAP_TYPE_BOSS)
		{
			this->OnFinishMap(true);
			this->AddTaskParam(RAMAZE_SUMMER_TASK_TYPE_1);
			return;
		}
		node_map->map_lock = RA_MAZE_SUMMER_MAP_HAS_UNLOCK;
		node_map->map_status = RA_MAZE_SUMMER_MAP_STATUS_FINISH;
		this->CheckFinishUnlockMap();
		this->SendSingleMapChange(m_param.m_pos);
		this->SendMyPos();
	}
}

void RoleActivityMazeSummer::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (RAND_ACTIVITY_TYPE_MAZE_SUMMER != rand_act_type)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity *activity = ramgr->ForceGetRandActivity(rand_act_type);
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.begin_tamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.begin_tamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		int rand_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_MAZE_SUMMER);

		const RandActivityMazeSummerConfig * config = static_cast<const RandActivityMazeSummerConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_MAZE_SUMMER));
		if (config == NULL)return;
		RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
		if (NULL == ramgr) return;

		const RAMazeSummerTaskListCfg * task_list_cfg = config->GetMazeSummerTaskListCfg(rand_open_day);
		if (task_list_cfg != NULL)
		{
			for (std::vector<RAMazeSummerTaskCfg>::const_iterator it = task_list_cfg->m_task_list.begin(); it != task_list_cfg->m_task_list.end() && m_param.task_num < MAX_MAZE_SUMMER_TASK_NUM; it++)
			{
				m_param.task_info[m_param.task_num++].task_id = (unsigned char)it->type;
			}
		}

		this->SetRoleDataChange();
	}
}

void RoleActivityMazeSummer::OnLoginSendInfo()
{
	this->SendTaskInfo();
	this->SendPlayStatus();
}

void RoleActivityMazeSummer::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid != now_dayid)
	{
		m_param.play_times = 0;
	}
}

void RoleActivityMazeSummer::AddTaskParam(int task_type, int num)
{
	if (task_type <= RAMAZE_SUMMER_TASK_TYPE_0 || task_type >= RAMAZE_SUMMER_TASK_TYPE_MAX || task_type >= MAX_MAZE_SUMMER_TASK_NUM)
	{
		return;
	}
	m_param.play_info[task_type] += num;
	this->SendTaskInfo();
}

void RoleActivityMazeSummer::SendTaskInfo()
{
	Protocol::SCMazeSummerTaskInfo info;
	
	for (int i = 0; i < MAX_MAZE_SUMMER_TASK_NUM; i++)
	{
		info.m_task[i].is_get_reward = m_param.task_info[i].is_get_reward;
		info.m_task[i].task_id = m_param.task_info[i].task_id;
		info.m_task[i].reserve_ch = m_param.task_info[i].reserve_ch;
		
		info.play_info[i] = m_param.play_info[i];
	}
	m_role_module_mgr->NetSend((const char *)&info, sizeof(info));
}

void RoleActivityMazeSummer::SendMapInfo()
{
	//检测是否已经开始游戏了
	if (m_param.m_map[0][0].map_lock == RA_MAZE_SUMMER_MAP_HAS_LOCK)
	{
		return;
	}
	Protocol::SCMazeSummerMapInfo info;
	info.m_pos = m_param.m_pos;
	for (int i = 0; i < MAX_MAZE_SUMMER_POS_X; i++)
	{
		for (int j = 0; j < MAX_MAZE_SUMMER_POS_Y; j++)
		{
			info.m_map[i][j] = m_param.m_map[i][j];
		}
	}
	m_role_module_mgr->NetSend((const char *)&info, sizeof(info));
}

void RoleActivityMazeSummer::SendShopInfo()
{
	Protocol::SCMazeSummerShopInfo info;
	info.m_shop_num = 0;
	for (int i = 0; i < MAX_MAZE_SUMMER_SHOP_NUM; i++)
	{
		if (m_param.m_shop[i].seq != 0)
		{
			info.m_shop[info.m_shop_num++] = m_param.m_shop[i];
		}
	}
	m_role_module_mgr->NetSend((const char *)&info, sizeof(info) - ((MAX_MAZE_SUMMER_SHOP_NUM - info.m_shop_num) * sizeof(RAMazeSummerShop)));
}

void RoleActivityMazeSummer::SendQuestionInfo(RAMazeSummerMap * question_info)
{
	Protocol::SCMazeSummerQuestionInfo info;
	info.question_id = question_info->param2;
	m_role_module_mgr->NetSend((const char *)&info, sizeof(info));
}

void RoleActivityMazeSummer::SendSingleMapChange(Posi change_pos)
{
	if (change_pos.x < 0 || change_pos.x >= MAX_MAZE_SUMMER_POS_X || change_pos.y < 0 || change_pos.y >= MAX_MAZE_SUMMER_POS_Y)return;
	Protocol::SCMazeSummerSingleMapInfo info;
	info.change_pos = change_pos;
	info.map_info = m_param.m_map[change_pos.x][change_pos.y];
	m_role_module_mgr->NetSend((const char *)&info, sizeof(info));
}

void RoleActivityMazeSummer::SendMyPos()
{
	Protocol::SCMazeSummerMyPosInfo info;
	info.m_pos = m_param.m_pos;
	m_role_module_mgr->NetSend((const char *)&info, sizeof(info));
}

void RoleActivityMazeSummer::SendPlayStatus()
{
	Protocol::SCMazeSummerStatusInfo info;
	info.play_times = m_param.play_times;
	info.is_playing = m_param.m_map[0][0].map_lock == RA_MAZE_SUMMER_MAP_HAS_UNLOCK;
	m_role_module_mgr->NetSend((const char *)&info, sizeof(info));
}

void RoleActivityMazeSummer::StartPlay()
{
	//初始化地图
	const RandActivityMazeSummerConfig * config = static_cast<const RandActivityMazeSummerConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_MAZE_SUMMER));
	if (config == NULL)return;
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_MAZE_SUMMER))
	{
		return;
	}

	//检测是否已经开始游戏了
	if (m_param.m_map[0][0].map_lock == RA_MAZE_SUMMER_MAP_HAS_UNLOCK)
	{
		return;
	}

	//消耗
	const RAMazeSummerOtherCfg & other_cfg = config->GetOtherCfg();
	int count = 0;
	if (m_param.play_times < 1)
	{
		count = other_cfg.first_num;
	}
	else if(m_param.play_times < 2)
	{
		count = other_cfg.sec_num;
	}
	else if (m_param.play_times < 3)
	{
		count = other_cfg.third_num;
	}
	else if (m_param.play_times >= 4)
	{
		count = other_cfg.four_more_num;
	}

	if (count > 0)
	{
		if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(other_cfg.consume_item, count, __FUNCTION__))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
			return;
		}
	}

	int open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_MAZE_SUMMER);

	const RAMazeSummerMapListCfg * map_cfg = config->GetMazeSummerMapListCfg(open_day);
	if (map_cfg == NULL)return;
	m_param.ResetGame();
	bool has_shop = false;
	for (int i = 0; i < MAX_MAZE_SUMMER_POS_X; i++)
	{
		for (int j = 0; j < MAX_MAZE_SUMMER_POS_Y; j++)
		{
			const RAMazeSummerEventGroupCfg * event_cfg = config->GetMazeSummerEventInfoCfg(map_cfg->map_info[i][j].event_group, has_shop);
			if (NULL != event_cfg)
			{
				m_param.m_map[i][j].map_type = event_cfg->event_type;
				m_param.m_map[i][j].param1 = event_cfg->param_1;
				if (event_cfg->event_type == RA_MAZE_SUMMER_MAP_TYPE_SHOP)//商店
				{
					//加入商店道具
					int shop_rand_num = 0;
					if (other_cfg.shop_item_num_up - other_cfg.shop_item_num_down == 0)
					{
						shop_rand_num = other_cfg.shop_item_num_down;
					}
					else
					{
						shop_rand_num = other_cfg.shop_item_num_down + rand() % (other_cfg.shop_item_num_up - other_cfg.shop_item_num_down);
					}
					
					if (shop_rand_num > 0)
					{
						m_param.has_shop = true;
						const RAMazeSummerShopGroupCfg * shop_cfg = config->GetMazeSummerShopGroupCfg(event_cfg->param_1);
						if (shop_cfg != NULL)
						{
							if (shop_cfg->weight != 0)
							{
								for (int shop_idx = 0; shop_idx < shop_rand_num && m_param.shop_num < MAX_MAZE_SUMMER_SHOP_NUM; shop_idx++)
								{
									int one_shop_weight = rand() % shop_cfg->weight;
									for (std::vector<RAMazeSummerShopCfg>::const_iterator it = shop_cfg->m_reward_vec.begin(); it != shop_cfg->m_reward_vec.end() && m_param.shop_num < MAX_MAZE_SUMMER_SHOP_NUM; it++)
									{
										if (it->rate >= one_shop_weight)
										{
											m_param.m_shop[m_param.shop_num].buy_times = 0;
											m_param.m_shop[m_param.shop_num].seq = it->index;
											m_param.m_shop[m_param.shop_num++].shop_id = it->shop_type;
										}
										one_shop_weight -= shop_cfg->weight;
									}
								}
							}
						}
					}

					has_shop = true;
				}
			}
		}
	}

	for (int i = 0; i < MAX_MAZE_SUMMER_POS_X; i++)
	{
		for (int j = 0; j < 2; j++)//初始化最开始的2行
		{
			m_param.m_map[i][j].map_lock = RA_MAZE_SUMMER_MAP_HAS_UNLOCK;
		}
	}
	this->SendPlayStatus();
	this->SendMapInfo();

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityMazeSummer::MovePos(Posi pos)
{
	//检查我所在位置是否允许移动
	if (!this->CheckMyPosCanMove())
	{
		return;
	}

	if (pos.x < 0 || pos.x >= MAX_MAZE_SUMMER_POS_X || pos.y < 0 || pos.y >= MAX_MAZE_SUMMER_POS_Y)return;
	RAMazeSummerMap &node_map = m_param.m_map[pos.x][pos.y];
	if (node_map.map_lock != RA_MAZE_SUMMER_MAP_HAS_UNLOCK)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_MAZE_SUMMER_NOT_UNLOCK);
		return;
	}

	m_param.m_pos = pos;

	bool is_lock_door = false;
	bool need_notice = false;
	switch (node_map.map_type)
	{
	case RA_MAZE_SUMMER_MAP_TYPE_BOSS:
		if (node_map.map_status == RA_MAZE_SUMMER_MAP_STATUS_NORMAL)
		{
			is_lock_door = true;
			need_notice = true;
		}
		break;
	case RA_MAZE_SUMMER_MAP_TYPE_MONSTER:
		if (node_map.map_status == RA_MAZE_SUMMER_MAP_STATUS_NORMAL)
		{
			is_lock_door = true;
			need_notice = true;
		}
		break;
	case RA_MAZE_SUMMER_MAP_TYPE_QUESTION:
		if (node_map.map_status == RA_MAZE_SUMMER_MAP_STATUS_NORMAL)
		{
			is_lock_door = true;
			need_notice = true;
		}
		break;
	case RA_MAZE_SUMMER_MAP_TYPE_SHOP:
		if (node_map.map_status == RA_MAZE_SUMMER_MAP_STATUS_NORMAL)
		{
			node_map.map_status = RA_MAZE_SUMMER_MAP_STATUS_FINISH;
			need_notice = true;
			//直接调用打开后面的场景
			this->CheckFinishUnlockMap();
			this->AddTaskParam(RAMAZE_SUMMER_TASK_TYPE_2);
		}
		break;
	default:
		break;
	}

	if (is_lock_door)
	{
		node_map.map_status = RA_MAZE_SUMMER_MAP_STATUS_DOING;
	}

	if (need_notice)
	{
		this->SendSingleMapChange(pos);
	}

	this->SendMyPos();
}

RAMazeSummerMap * RoleActivityMazeSummer::GetMyPosMapInfo()
{
	if (m_param.m_pos.x < 0 || m_param.m_pos.x >= MAX_MAZE_SUMMER_POS_X || m_param.m_pos.y < 0 || m_param.m_pos.y >= MAX_MAZE_SUMMER_POS_Y)return NULL;

	return &m_param.m_map[m_param.m_pos.x][m_param.m_pos.y];
}

Posi RoleActivityMazeSummer::GridToPosi(int grid)
{
	int pos_x = grid % MAX_MAZE_SUMMER_POS_X;
	int pos_y = grid / MAX_MAZE_SUMMER_POS_X;
	return Posi(pos_x, pos_y);
}

int RoleActivityMazeSummer::PosiToGrid(Posi pos)
{
	return (int)(pos.x + pos.y * MAX_MAZE_SUMMER_POS_X);
}

bool RoleActivityMazeSummer::IsHouseMap(int grid)
{
	for (int i = 0; i < (MAX_MAZE_SUMMER_POS_X * MAX_MAZE_SUMMER_POS_Y / 2); i++)
	{
		if (MAX_MAZE_SUMMER_HOUSE_LIST[i] == grid)
		{
			return true;
		}
	}
	return false;
}

void RoleActivityMazeSummer::StartSpecialPlay()
{
	if (m_role_module_mgr->GetRole()->InTeam())return;
	RAMazeSummerMap *node_map = this->GetMyPosMapInfo();
	if (node_map == NULL)return;
	switch (node_map->map_type)
	{
	case RA_MAZE_SUMMER_MAP_TYPE_BOSS:
		if (node_map->map_status == RA_MAZE_SUMMER_MAP_STATUS_DOING)
		{
			BattleManagerLocal::GetInstance().StartBattleModeFightReq(m_role_module_mgr->GetRole(), BATTLE_MODE_MAZE_SUMMER,
				node_map->param1, 0, 0, true);
		}
		break;
	case RA_MAZE_SUMMER_MAP_TYPE_MONSTER:
		if (node_map->map_status == RA_MAZE_SUMMER_MAP_STATUS_DOING)
		{
			BattleManagerLocal::GetInstance().StartBattleModeFightReq(m_role_module_mgr->GetRole(), BATTLE_MODE_MAZE_SUMMER,
				node_map->param1, 0, 0, true);
		}
		break;
	case RA_MAZE_SUMMER_MAP_TYPE_SHOP:
		{
			this->SendShopInfo();
		}
		break;
	case RA_MAZE_SUMMER_MAP_TYPE_QUESTION:
		if (node_map->map_status == RA_MAZE_SUMMER_MAP_STATUS_DOING)
		{
			//下发题目
			const WSTQuestionBankCfg* question_group_cfg = LOGIC_CONFIG->GetWstQuestionConfig()->GetQuestionBankByGroupCfg(node_map->param1);
			if (NULL != question_group_cfg)
			{
				if (node_map->param2 == 0)
				{
					node_map->param2 = question_group_cfg->question_id;
				}
			}
			this->SendQuestionInfo(node_map);
		}
		break;
	default:
		break;
	}
}

void RoleActivityMazeSummer::QuestionAnswer(int answer_id)
{
	const RandActivityMazeSummerConfig * config = static_cast<const RandActivityMazeSummerConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_MAZE_SUMMER));
	if (config == NULL)return;
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;
	RAMazeSummerMap * map_info = this->GetMyPosMapInfo();
	if (map_info != NULL)
	{
		if (map_info->map_type != RA_MAZE_SUMMER_MAP_TYPE_QUESTION)return;
		if (map_info->map_status != RA_MAZE_SUMMER_MAP_STATUS_DOING)return;

		const WSTQuestionBankCfg* question_group_cfg = LOGIC_CONFIG->GetWstQuestionConfig()->GetQuestionBankCfg(map_info->param2);
		if (question_group_cfg != NULL && question_group_cfg->answer_id == answer_id)
		{
			map_info->map_status = RA_MAZE_SUMMER_MAP_STATUS_FINISH;
			map_info->map_lock = RA_MAZE_SUMMER_MAP_HAS_UNLOCK;

			int open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_MAZE_SUMMER);
			int grid = this->PosiToGrid(m_param.m_pos);
			const RAMazeSummerMapCfg * map_cfg = config->GetMazeSummerMapCfgByDayAndGrid(open_day, grid);
			if (map_cfg == NULL)return;

			//发奖励
			const RAMazeSummerRewardGroupCfg * reward_cfg = config->GetMazeSummerRewardCfg(map_cfg->reward_group);
			if (NULL != reward_cfg)
			{
				if (reward_cfg->m_reward_vec.size() > 0)
				{
					if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)reward_cfg->m_reward_vec.size(), &reward_cfg->m_reward_vec[0], false))
					{
						//发邮件
						m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->m_reward_vec[0], (short)reward_cfg->m_reward_vec.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, RAND_ACTIVITY_TYPE_MAZE_SUMMER);
					}
					else
					{
						m_role_module_mgr->GetKnapsack()->PutList((short)reward_cfg->m_reward_vec.size(), &reward_cfg->m_reward_vec[0], PUT_REASON_MAZE_SUMMER);
					}
				}

				if (reward_cfg->exp > 0)
				{
					const RoleLevelCfg* cfg = LOGIC_CONFIG->GetRoleExpConfig()->GetLevelCfg(m_role_module_mgr->GetRole()->GetLevel());
					if (NULL != cfg)
					{
						long long add_exp = cfg->exp * reward_cfg->exp / 10000;
						m_role_module_mgr->GetRole()->AddExp(add_exp, ADD_EXP_TYPE_MAZE_SUMMER, __FUNCTION__);
					}
				}

				if (reward_cfg->coin > 0)
				{
					m_role_module_mgr->GetMoney()->AddCoinBind(reward_cfg->coin, __FUNCTION__);
				}
			}
			this->AddTaskParam(RAMAZE_SUMMER_TASK_TYPE_2);
			this->AddTaskParam(RAMAZE_SUMMER_TASK_TYPE_4);
			this->SendSingleMapChange(m_param.m_pos);
			this->CheckFinishUnlockMap();
		}
		else if(question_group_cfg != NULL && question_group_cfg->answer_id != answer_id)
		{
			map_info->param2 = 0;
			const WSTQuestionBankCfg* question_group_cfg = LOGIC_CONFIG->GetWstQuestionConfig()->GetQuestionBankByGroupCfg(map_info->param1);
			if (NULL != question_group_cfg)
			{
				if (map_info->param2 == 0)
				{
					map_info->param2 = question_group_cfg->question_id;
				}
			}
			this->SendQuestionInfo(map_info);
		}
	}
}

void RoleActivityMazeSummer::EndPlay()
{
	this->OnFinishMap(false);
}

void RoleActivityMazeSummer::BuyItem(int buy_idx)
{
	RAMazeSummerMap * m_pos = this->GetMyPosMapInfo();
	if (m_pos != NULL)
	{
		if (m_pos->map_type != RA_MAZE_SUMMER_MAP_TYPE_SHOP)return;
		if (buy_idx < 0 || buy_idx >= MAX_MAZE_SUMMER_SHOP_NUM)return;
		RAMazeSummerShop &shop_info = m_param.m_shop[buy_idx];

		const RandActivityMazeSummerConfig * config = static_cast<const RandActivityMazeSummerConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_MAZE_SUMMER));
		if (config == NULL)return;
		const RAMazeSummerShopCfg *shop_item_cfg = config->GetShopItemDetailCfg(shop_info.shop_id, shop_info.seq);
		if (shop_item_cfg != NULL)
		{
			if (shop_item_cfg->buy_times <= shop_info.buy_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_CRYSTAL_BUY_ENERGY_TIMES_NOT_ENOUGH);
				return;
			}

			if (!m_role_module_mgr->GetKnapsack()->CheckForPut(shop_item_cfg->shop_info.item_id, shop_item_cfg->shop_info.num))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
				return;
			}
			switch (shop_item_cfg->money_type)
			{
			case MONEY_TYPE_GOLD:
				if (!m_role_module_mgr->GetMoney()->GoldMoreThan(shop_item_cfg->price))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
					return;
				}
				m_role_module_mgr->GetMoney()->UseGold(shop_item_cfg->price, __FUNCTION__);
				break;
			case MONEY_TYPE_COIN_BIND:
				if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(shop_item_cfg->price))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
					return;
				}
				m_role_module_mgr->GetMoney()->UseCoinBind(shop_item_cfg->price, __FUNCTION__);
				break;
			default:
				break;
			}

			m_role_module_mgr->GetKnapsack()->Put(shop_item_cfg->shop_info, PUT_REASON_MAZE_SUMMER);
			shop_info.buy_times += 1;
			this->SendShopInfo();
		}
	}
}

void RoleActivityMazeSummer::CheckFinishUnlockMap()
{
	RAMazeSummerMap * m_pos = this->GetMyPosMapInfo();
	if (NULL == m_pos)return;
	if (m_pos->map_status != RA_MAZE_SUMMER_MAP_STATUS_FINISH)return;
	if (m_pos->map_lock != RA_MAZE_SUMMER_MAP_HAS_UNLOCK)return;
	
	int grid = this->PosiToGrid(m_param.m_pos);
	bool is_house = this->IsHouseMap(grid);
	bool has_change = false;
	int change_count = 0;
	Posi change_pos_list[2 * MAX_MAZE_SUMMER_POS_X] = { Posi(0,0),Posi(0,0),Posi(0,0),Posi(0,0),Posi(0,0),Posi(0,0) };
	if (is_house)//是房间移动或变更
	{
		//检测后面两行
		for (int i = 1 + m_param.m_pos.y; i <= 2 + m_param.m_pos.y && i < MAX_MAZE_SUMMER_POS_Y; i++)
		{
			for (int j = 0; j < MAX_MAZE_SUMMER_POS_X; j++)
			{
				RAMazeSummerMap &node_map = m_param.m_map[j][i];
				if (node_map.map_lock == RA_MAZE_SUMMER_MAP_HAS_UNLOCK)
				{
					continue;
				}
				//解锁
				node_map.map_lock = RA_MAZE_SUMMER_MAP_HAS_UNLOCK;
				has_change = true;
				change_pos_list[change_count++] = Posi(j, i);
			}
		}
	}
	if (has_change)
	{
		for (int change_idx = 0; change_idx < change_count && change_idx < 2 * MAX_MAZE_SUMMER_POS_X; change_idx++)
		{
			this->SendSingleMapChange(change_pos_list[change_idx]);
		}
		this->SendMyPos();
	}
}

bool RoleActivityMazeSummer::CheckMyPosCanMove()
{
	bool ret = true;

	RAMazeSummerMap * node_map = this->GetMyPosMapInfo();
	if (node_map == NULL)return false;

	if (node_map->map_status == RA_MAZE_SUMMER_MAP_STATUS_DOING)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_MAZE_SUMMER_DOING_MAP);
		ret = false;
	}

	return ret;
}

void RoleActivityMazeSummer::OnFinishMap(bool is_pass)
{
	m_param.ResetGame();
	m_param.play_times += 1;
	//下发完成结果
	Protocol::SCMazeSummerPlayFinishInfo info;
	info.is_pass = is_pass;
	m_role_module_mgr->NetSend((const char *)&info, sizeof(info));
	this->SendPlayStatus();
}

void RoleActivityMazeSummer::GetTaskReward(int seq)
{
	const RandActivityMazeSummerConfig * config = static_cast<const RandActivityMazeSummerConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_MAZE_SUMMER));
	if (config == NULL)return;
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_MAZE_SUMMER))
	{
		return;
	}

	int open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_MAZE_SUMMER);
	if (seq < 0 || seq >= MAX_MAZE_SUMMER_TASK_NUM || seq >= m_param.task_num)return;
	RAMazeSummerTaskInfo & task_info = m_param.task_info[seq];

	const RAMazeSummerTaskCfg * task_cfg = config->GetMazeSummerTaskCfgByType(task_info.task_id);
	if (task_cfg != NULL)
	{
		if (task_cfg->task < 0 || task_cfg->task >= RAMAZE_SUMMER_TASK_TYPE_MAX)
		{
			return;
		}

		int & task_param = m_param.play_info[task_cfg->task];

		if (task_param < task_cfg->param_1)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_TASK_IS_NOT_COMPLETED);
			return;
		}

		if (task_info.is_get_reward == 1)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_MAZE_SUMMER_REWARD_HAS_GET);
			return;
		}

		if (!m_role_module_mgr->GetKnapsack()->CheckForPut(task_cfg->reward.item_id, task_cfg->reward.num))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_ITEM_COLUMN_NO_SPACE);
			return;
		}

		m_role_module_mgr->GetKnapsack()->Put(task_cfg->reward, PUT_REASON_MAZE_SUMMER);
		
		task_info.is_get_reward = 1;

		this->SendTaskInfo();
	}
}

void RoleActivityMazeSummer::CheckMoveUnlockMap(Posi pos)
{

}
