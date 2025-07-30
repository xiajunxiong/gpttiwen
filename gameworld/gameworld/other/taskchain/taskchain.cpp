#include "taskchain.hpp"
#include "config/logicconfigmanager.hpp"
#include "taskchainconfig.hpp"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "scene/scenemanager.hpp"
#include "scene/scene.h"
#include "scene/map.h"
#include "world.h"
#include "protocol/msgtask.h"
#include "item/knapsack.h"
#include "servercommon/errornum.h"
#include "servercommon/string/gameworldstr.h"
#include "npc/npcmanager.h"
#include "other/event/eventhandler.hpp"
#include "other/pet/pet.hpp"
#include "other/route/mailroute.hpp"
#include "other/prestige/prestige.hpp"
#include "gameworld/gamelog.h"
#include "other/funopen/funopen.hpp"
#include "other/funopen/funopendef.hpp"
#include "other/levelcomplement/levelcomplement.h"
#include "global/worldstatus/worldstatus.hpp"

TaskChain::TaskChain()
{
	task_type = 0;
	memset(task_param, 0, sizeof(task_param));
}

TaskChain::~TaskChain()
{
}

void TaskChain::Init(RoleModuleManager * mgr, const TaskChainParam & param)
{
	m_role_module_mgr = mgr;
	m_param = param;

	//处理老玩家数据
	if (0 == m_param.is_version_change_process_flag)
	{
		if (0 == m_param.task_count && 0 == m_param.pass_count && 0 == m_param.task_type)	//新号或者本周尚未领取任务
		{
			m_param.is_version_change_week_reset_flag = 1;
		}
		else if (0 != m_param.task_count)					//老号本周已完成
		{
			m_param.next_completion_times = m_param.pass_count = LOGIC_CONFIG->GetTaskChainConfig()->GetMaxCompletionTimes();
			m_param.task_count = LOGIC_CONFIG->GetTaskChainConfig()->GetStageByCompetionTimes(m_param.pass_count).index + 1;
			if (0 == m_param.has_fetch_turn_table_reward)  //玩家尚未领取转盘奖励
			{
				this->RandTurnTable(false);		//重新随机一个转盘
			}
		}
		else						//老号本周任务进行中
		{
			m_param.task_count = LOGIC_CONFIG->GetTaskChainConfig()->GetStageByCompetionTimes(m_param.pass_count).index + 1;
			//刚好完成阶段
			bool is_finish_cur_stage = false;
			if (LOGIC_CONFIG->GetTaskChainConfig()->HasCompletionTimes(m_param.pass_count))
			{
				m_param.ClearTask();
				is_finish_cur_stage = true;
			}
			int max_completion_times_by_level = LOGIC_CONFIG->GetTaskChainConfig()->GetMaxCompletionTimesByLevel(m_role_module_mgr->GetRole()->GetLevel());
			if (m_param.pass_count <= max_completion_times_by_level)
			{
				m_param.next_completion_times = max_completion_times_by_level;
				m_param.is_version_change_week_reset_flag = 1;
			}
			else
			{
				//刚好完成当前阶段,则来到此结束,否则让玩家跑完该阶段
				if (is_finish_cur_stage)
				{
					m_param.next_completion_times = m_param.pass_count;
				}
				else
				{
					int max_completion_times = LOGIC_CONFIG->GetTaskChainConfig()->GetMaxCompletionTimes();
					m_param.next_completion_times = LOGIC_CONFIG->GetTaskChainConfig()->GetNextCompletionTimes(m_param.pass_count, max_completion_times);
				}
			}
		}
		m_param.is_version_change_process_flag = 1;
	}

	if(2 != m_param.is_version_change_process_flag)
	{
		TaskChainLevelTimes info = LOGIC_CONFIG->GetTaskChainConfig()->GetStageByCompetionTimes(m_param.pass_count);
		m_param.task_pass_count = m_param.pass_count - info.completion_times;
	    m_param.cur_task_max_count = m_param.next_completion_times - info.completion_times;
		m_param.is_version_change_process_flag = 2;
	}

	if (m_param.accept_task_level <= 0)	m_param.accept_task_level = m_role_module_mgr->GetRole()->GetLevel();
}

void TaskChain::GetInitParam(TaskChainParam * param)
{
	*param = m_param;
}

void TaskChain::OnLogin()
{
	if (TASK_CHAIN_TYPE_FIGHT == m_param.task_type && m_role_module_mgr->GetRole()->GetScene()->GetSceneID() == m_param.task_param[1])
	{
		this->SendTaskChainMonsterInfo();
	}
	if ((long long)EngineAdapter::Instance().Time() < m_param.next_refresh_timestamp)
	{
		this->SendTurnTableInfo();
	}
}

void TaskChain::OnRoleLevelUp()
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_TASK_CHAIN))
	{
		return;
	}
	this->SendTaskInfo();
}

void TaskChain::TaskChainUpdate(time_t now)
{
	if (now < m_param.next_refresh_timestamp) return;

	this->RefreshData();

	int tmp_interval = EngineAdapter::Instance().NextWeekInterval(MONDAY, 0, 0, 0);		// 每周一上午0点刷新
	m_param.next_refresh_timestamp = now + tmp_interval;
}

void TaskChain::RefreshData()
{
	m_param.task_count = 0;
	m_param.pass_count = 0;
	m_param.pre_wait_task = 0;
	m_param.is_version_change_process_flag = 2;
	m_param.is_version_change_week_reset_flag = 1;
	m_param.next_completion_times = 0;
	m_param.task_pass_count = 0;
	m_param.cur_task_max_count = 0;
	m_param.ClearTask();
	this->ResetTurnTable();
	this->SendTaskInfo();
	this->SendTurnTableInfo();
	//检查玩家身上是否存留着商人烦恼的任务道具有就都清除掉
	{
		const std::map<int, std::vector<TaskChainItemCfg> >* item_list_map = LOGIC_CONFIG->GetTaskChainConfig()->GetWaitItemMap();
		if (item_list_map != NULL)
		{
			std::vector<ItemID> item_list;
			for (std::map<int, std::vector<TaskChainItemCfg> >::const_iterator c_it = item_list_map->begin(); c_it != item_list_map->end(); c_it++)
			{
				for (std::vector<TaskChainItemCfg>::const_iterator t_it = c_it->second.begin(); t_it != c_it->second.end(); t_it++)
				{
					item_list.push_back(t_it->item.item_id);
				}
				
			}
			
			m_role_module_mgr->GetKnapsack()->ClearItemList(ItemNamespace::ITEM_COLUMN_TYPE_TASK_MATERIAL, &item_list, __FUNCTION__);
		}
	}
	
}

void TaskChain::OnEnterScene(int scene_id)
{
	if (TASK_CHAIN_TYPE_FIGHT == m_param.task_type && scene_id == m_param.task_param[1]) 
	{
		if (!m_role_module_mgr->GetRole()->GetScene()->GetMap()->Validate(Obj::OBJ_TYPE_ROLE, m_param.task_param[2] / 10000, m_param.task_param[2] % 10000))
		{
			Posi pos = m_role_module_mgr->GetRole()->GetScene()->GetARandPosi();
			m_param.task_param[2] = pos.x * 10000 + pos.y;
		}
		this->SendTaskChainMonsterInfo();
	}
}

void TaskChain::OnTalkToNpc()
{
	if (TASK_CHAIN_TYPE_TALK != m_param.task_type)return;

	if (!NpcManager::Instance().CanTouchNpc(m_role_module_mgr->GetRole(), m_param.task_param[0])) return;

	this->OnCommitTask();
}

void TaskChain::OnKillMonster()
{
	if (TASK_CHAIN_TYPE_FIGHT != m_param.task_type)return;

	this->OnCommitTask();
}

void TaskChain::OnDelivery(int column, int index)
{
	if (TASK_CHAIN_TYPE_ITEM != m_param.task_type && TASK_CHAIN_TYPE_WAIT != m_param.task_type/* && TASK_CHAIN_TYPE_PET != m_param.task_type*/) return;

	if (!NpcManager::Instance().CanTouchNpc(m_role_module_mgr->GetRole(), m_param.task_param[0])) return;
	
	static ItemGridData item;
	item.Reset();
	if (!m_role_module_mgr->GetKnapsack()->GetItemGridData(column, index, &item))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}

	if (item.item_id != m_param.task_param[1])
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_ITEM_NOT_ELIGIBLE);
		return;
	}

	switch (m_param.task_type)
	{
	case TASK_CHAIN_TYPE_ITEM:
		{
			if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(column, index, m_param.task_param[2], __FUNCTION__))
			{
				m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
				return;
			}
		}
		break;
	case TASK_CHAIN_TYPE_PET:
		{
			if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(column, index, 1, __FUNCTION__))
			{
				m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
				return;
			}
		}
		break;
	case TASK_CHAIN_TYPE_WAIT:
		{
			if (!m_role_module_mgr->GetKnapsack()->ConsumeItemByIndex(column, index, 1, __FUNCTION__))
			{
				m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
				return;
			}
		}
		break;
	default:
		return;
	}

	this->OnCommitTask();
}

void TaskChain::OnKillMineMonster()
{
	if (m_param.task_type != TASK_CHAIN_TYPE_ITEM)return;

	time_t now_time = EngineAdapter::Instance().Time();

	if (m_param.task_limit_timestamp == 0 && m_param.task_limit_timestamp < now_time)//超时，不触发暗雷奖励掉落
	{
		return;
	}

	//检测对应条件

	int role_battle_scene = m_role_module_mgr->GetRole()->GetScene()->GetSceneID();
	if (role_battle_scene != m_param.task_param[3])return;
	if (m_param.task_param[4] == 0)return;
	int rate_num = rand() % TASK_CHAIN_RATE_MAX;
	if (rate_num >= m_param.task_param[4])return;

	//暗雷特殊奖励
	Knapsack * role_knapsack = m_role_module_mgr->GetKnapsack();
	if (role_knapsack == NULL)return;

	if (role_knapsack->Count(m_param.task_param[1]) < m_param.task_param[2] && m_role_module_mgr->GetKnapsack()->CheckForPut(m_param.task_param[1], 1))
	{
		ItemConfigData drop_item;
		drop_item.item_id = m_param.task_param[1];
		drop_item.num = 1;
		drop_item.is_bind = true;
		m_role_module_mgr->GetKnapsack()->Put(drop_item, PUT_REASON_TASKCHAIN_DROP_SCENE);
	}
	m_param.task_limit_timestamp = 0;
	this->SendTaskInfo();
}

void TaskChain::OnSkipFight()
{
	if (1 != m_param.can_skip)return;

	Money * m_money = m_role_module_mgr->GetMoney();
	if (m_money == NULL)return;

	if (m_param.price_type == 0)//元宝
	{
		if (!m_money->GoldMoreThan(m_param.price))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}
		m_money->UseGold(m_param.price,"TaskChain::OnSkipFight" );
	}
	else if (m_param.price_type == 2)//铜币
	{
		if (!m_money->CoinBindMoreThan(m_param.price))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
		m_money->UseCoinBind(m_param.price, "TaskChain::OnSkipFight");
	}
	else
	{
		return;//除元宝和铜币外不能跳过
	}

	this->OnCommitTask(m_param.reward_double == 1);
}

void TaskChain::OnCommitPet(int pet_index)
{
	if (m_param.task_type != TASK_CHAIN_TYPE_PET)
	{
		return;
	}

	const PetParam * pet_param = m_role_module_mgr->GetPet()->GetPetParam(pet_index);
	if (NULL == pet_param)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (pet_param->level != 1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TASK_CHAIN_COMMIT_PET_ERROR);
		return;
	}

	if (!m_role_module_mgr->GetPet()->SystemTakeBackPet(pet_index, m_param.task_param[2]))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TASK_CHAIN_PET_ERR);
		return;
	}
	this->OnCommitTask(true);
}

void TaskChain::OnGetItemByWait()
{
	if (m_param.task_type != TASK_CHAIN_TYPE_WAIT)return;

	if (!NpcManager::Instance().CanTouchNpc(m_role_module_mgr->GetRole(), m_param.task_param[3])) return;
	
	time_t now_second = EngineAdapter::Instance().Time();
	if (now_second < (unsigned int)m_param.task_param[2])
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TASK_CHAIN_WAIT_LIMIT);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->Put(ItemConfigData(m_param.task_param[1], 1, 1), PUT_REASON_TASK_CHAIN))
	{
		gamelog::g_log_task.printf(LL_INFO, "TaskChain::OnGetItemByWait put fail! user[%s %d] task_type[%d] task_param[0]{%d} task_param[1]{%d} task_param[2]{%d}",
			m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetUid(), m_param.task_type, m_param.task_param[0], m_param.task_param[1], m_param.task_param[2]);
	}

	this->SendTaskInfo(false);
}

void TaskChain::OnAcceptTask()
{
	int role_level = m_role_module_mgr->GetRole()->GetLevel();
	int max_compoletion_times = LOGIC_CONFIG->GetTaskChainConfig()->GetMaxCompletionTimesByLevel(role_level);
	if (0 == max_compoletion_times) return;

	if (0 != m_param.pass_count)
	{
		if (max_compoletion_times == m_param.pass_count)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_TASK_TIME_ALREADY_MAX);
			return;
		}

		if (!LOGIC_CONFIG->GetTaskChainConfig()->HasCompletionTimes(m_param.pass_count))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_TASK_ALREADY_ACCEPTED);
			return;
		}
	}

/*	if (m_param.task_count >= LOGIC_CONFIG->GetTaskChainConfig()->GetOtherCfg().max_times)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TASK_TIME_ALREADY_MAX);
		return;
	}
*/
	//传入上一次任务信息
	const TaskChainCfg * task_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandTaskType(role_level, task_type);
	if (NULL == task_cfg) return;
	if (task_cfg->coin > 0)//检测铜币
	{
		if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(task_cfg->coin))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
	}

	if (this->RandTask(false))
	{
		int next_completion_times = LOGIC_CONFIG->GetTaskChainConfig()->GetNextCompletionTimes(m_param.pass_count, max_compoletion_times);
		m_param.cur_task_max_count = next_completion_times - m_param.next_completion_times;
		m_param.next_completion_times = next_completion_times;
		m_param.task_pass_count = 0;
		m_param.accept_task_level = role_level;

		if (0 == m_param.pass_count)
		{
			m_role_module_mgr->GetMoney()->UseCoinBind(task_cfg->coin, __FUNCTION__);
		}
		this->SendTaskInfo(true);
		//随机到的怪物与自身处于同一场景时,需先下发任务,再下发创建怪物
		if (TASK_CHAIN_TYPE_FIGHT == m_param.task_type && m_role_module_mgr->GetRole()->GetScene()->GetSceneID() == m_param.task_param[1])
		{
			this->SendTaskChainMonsterInfo();
		}
	}
	WorldStatus::Instance().LogFunctionStats(m_role_module_mgr->GetRole(), "TaskChain");
	m_role_module_mgr->GetRole()->LogActive(LOG_ACTIVE_TYPE_TASK_CHAIN);
}

void TaskChain::OnCommitTask(bool is_double)
{
	if (0 == m_param.task_type)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_TASK_NOT_ACCEPTEND);
		return;
	}

	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role) return;

	const TaskChainExpItemCfg *  cfg = LOGIC_CONFIG->GetTaskChainConfig()->GetExp(role->GetLevel(), m_param.pass_count + 1);
	if (cfg == NULL)return;

	int exp = cfg->exp;
	int coin = cfg->coin;
	if (is_double)
	{
		exp = exp * 2;
		coin = coin * 2;
	}

	int city_id = -1;
	int city_num = 0;
	switch (m_param.task_type)
	{
	case TASK_CHAIN_TYPE_TALK:
		{
			city_id = m_param.task_param[1];
			city_num = m_param.task_param[2];
			if (is_double)city_num = city_num * 2;
		}
		break;
	case TASK_CHAIN_TYPE_ITEM:
		{
			city_id = m_param.task_param[6];
			city_num = m_param.task_param[7];
			if (is_double)city_num = city_num * 2;
		}
		break;
	case TASK_CHAIN_TYPE_FIGHT:
		{
			city_id = m_param.task_param[6];
			city_num = m_param.task_param[7];
			if (is_double)city_num = city_num * 2;
		}
		break;
	case TASK_CHAIN_TYPE_PET:
		{
			city_id = m_param.task_param[3];
			city_num = m_param.task_param[4];
			if (is_double)city_num = city_num * 2;
		}
		break;
	case TASK_CHAIN_TYPE_WAIT:
		{
			city_id = m_param.task_param[4];
			city_num = m_param.task_param[5];
			if (is_double)city_num = city_num * 2;
		}
		break;
	default:
		break;
	}
	m_role_module_mgr->GetPrestige()->AddPrestige(city_id, city_num, __FUNCTION__);
	long long addexp_real = role->AddExp(exp, ADD_EXP_TYPE_TASK_CHAIN, __FUNCTION__);
	m_role_module_mgr->GetLevelComplement()->OnAddExp(addexp_real);
	EventHandler::Instance().OnFinishLevelComplementRelatedFun(m_role_module_mgr->GetRole(), exp, __FUNCTION__);
	role->GetRoleModuleManager()->GetMoney()->AddCoinBind(coin, "TaskChain::OnCommitTask");

	if (cfg->item_rate != 0)
	{
		int rand_num = rand() % 10000;
		if (rand_num < cfg->coin_rate)//给物品
		{
			m_role_module_mgr->GetKnapsack()->Put(cfg->item, PUT_REASON_TASK_CHAIN);
		}
	}

	if (m_param.pre_wait_task != 0)m_param.pre_wait_task += 1;//记录代送间隔
	std::vector<ItemConfigData> reward_item_list;
	bool is_notice = LOGIC_CONFIG->GetTaskChainConfig()->GetTaskReward(role->GetLevel(), m_param.pass_count + 1, reward_item_list);
	if (!reward_item_list.empty()) 
	{
		if (!m_role_module_mgr->GetKnapsack()->PutList((short)reward_item_list.size(), &reward_item_list[0], PUT_REASON_TASK_CHAIN))
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&reward_item_list[0], (short)reward_item_list.size());
		}
		else
		{
			//背包满转为发邮件时不弹窗
			if (is_notice)
			{
				m_role_module_mgr->GetKnapsack()->NotifyGetItemList((short)reward_item_list.size(), &reward_item_list[0], GET_REASON_TASK_CHAIN_REWARD);
			}
		}
	}
	//记录本次为前置随机任务信息
	task_type = m_param.task_type;
	memcpy(task_param, m_param.task_param, sizeof(m_param.task_param));

	m_param.ClearTask();
	if (++m_param.pass_count < m_param.next_completion_times)
	{
		m_param.task_pass_count++;
		this->RandTask();
	}
	else
	{
		++m_param.task_count;
		m_param.task_pass_count = 0;
		m_param.cur_task_max_count = 0;
		this->SendTaskInfo();
		//完成任务链送一个转盘奖励
		this->RandTurnTable();
	}

	EventHandler::Instance().OnCommitTaskChain(role);
}

void TaskChain::OnGiveupTask()
{
	//屏蔽放弃任务
	/*if (0 == m_param.task_type) return;

	m_param.pass_count = 0;
	//记录本次为前置随机任务信息
	task_type = m_param.task_type;
	memcpy(task_param, m_param.task_param, sizeof(m_param.task_param));
	//检查任务类型，判断是否为等待任务，如果是检查一下玩家身上是否已经获得了这个任务道具

	if (m_param.task_type == TASK_CHAIN_TYPE_WAIT)
	{
		if (m_role_module_mgr->GetKnapsack()->HasItem(m_param.task_param[1], 1))//只允许一个，配置进入时候会检查
		{
			m_role_module_mgr->GetKnapsack()->ConsumeItem(m_param.task_param[1], 1, "TaskChain::OnGiveupTask");//把道具消耗掉
		}
	}

	m_param.ClearTask();
	this->SendTaskInfo();*/
}

void TaskChain::OnGiveTurnTableReward()
{
	const TaskChainLevelTimesCfg * level_times_cfg = LOGIC_CONFIG->GetTaskChainConfig()->GetCompletionTimesByIndex(m_param.turn_table_completion_times_index);
	if(NULL == level_times_cfg|| m_param.turn_table_completion_times_index >= sizeof(m_param.has_fetch_turn_table_reward) * BIT_COUNT_OF_BYTE) return;

	if ((m_param.has_fetch_turn_table_reward & (1 << m_param.turn_table_completion_times_index)) == 1)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}
//	if(m_param.turn_table_reward_index < 0) return;

	const ItemConfigData * reward_item = LOGIC_CONFIG->GetTaskChainConfig()->GetTurnTableReward(level_times_cfg->completion_times, m_param.accept_task_level);
	if (NULL == reward_item)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(reward_item->item_id, reward_item->num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}
	if (!m_role_module_mgr->GetKnapsack()->Put(*reward_item, PUT_REASON_TASK_CHAIN))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_role_module_mgr->GetKnapsack()->NotifyGetItemList(1, reward_item, GET_REASON_TASK_CHAIN_COMPLETION);

	m_param.has_fetch_turn_table_reward |= (1 << m_param.turn_table_completion_times_index);
	m_param.turn_table_completion_times_index = -1;
	m_param.turn_table_group_id = 0;
	this->SendTurnTableInfo();
}

int TaskChain::GetMonsterGroupId()
{
	if (TASK_CHAIN_TYPE_FIGHT != m_param.task_type) return 0;

	return m_param.task_param[3];
}

void TaskChain::SendTaskInfo(bool is_new)
{
	Protocol::SCTaskChainInfo info;
	info.pass_count = m_param.pass_count;
	info.task_type = m_param.task_type;
	memcpy(info.task_param, m_param.task_param, sizeof(info.task_param));
	info.can_skip = m_param.can_skip;
	info.price = m_param.price;
	info.price_type = m_param.price_type;
	info.task_count = m_param.task_count;
	info.is_new = is_new ? 1 : 0;
	int completion_times = LOGIC_CONFIG->GetTaskChainConfig()->GetMaxCompletionTimesByLevel(m_role_module_mgr->GetRole()->GetLevel());
	if (0 == m_param.is_version_change_week_reset_flag)
	{	
		completion_times = GetMax(completion_times, (int)m_param.next_completion_times);
	}
	info.completion_times = completion_times;
	info.task_limit_timestamp = m_param.task_limit_timestamp;
	info.task_pass_count = m_param.task_pass_count;
	info.cur_task_max_count = m_param.cur_task_max_count;
	info.accept_task_level = m_param.accept_task_level;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void TaskChain::SendTurnTableInfo()
{
	Protocol::SCTaskChainTurnTableInfo info;
	info.turn_table_index = -1; // m_param.turn_table_reward_index;
	info.turn_table_group_id = m_param.turn_table_group_id;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void TaskChain::GmTaskChainOrder(int op_type, int param)
{
	enum TaskChainOrderType
	{
		TASK_CHAIN_REFERSH_NOW = 0,		// 立刻执行周刷新逻辑
		TASK_CHAIN_ACCEPT_TASK = 1,		// 领取任务
		TASK_CHAIN_SETUP_TIMES = 2,		// 设置今日任务次数
	};

	switch (op_type)
	{
	case TASK_CHAIN_REFERSH_NOW:
		m_param.next_refresh_timestamp = 0;
		break;
	case TASK_CHAIN_ACCEPT_TASK:
		{
			m_param.task_count = 0;
			m_param.pass_count = 0;
			m_param.task_pass_count = 0;
			m_param.cur_task_max_count = 0;
			m_param.next_completion_times = 0;
			this->OnAcceptTask();
		}
		break;
	case TASK_CHAIN_SETUP_TIMES:
		m_param.pass_count = 0;
		m_param.task_count = 0;
		m_param.task_pass_count = 0;
		m_param.cur_task_max_count = 0;
		m_param.next_completion_times = 0;
		int competion_times = LOGIC_CONFIG->GetTaskChainConfig()->GetMaxCompletionTimesByLevel(m_role_module_mgr->GetRole()->GetLevel());
		if (0 == competion_times || param > competion_times)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_FUN_OPEN_ROLE_LEVEL_LIMIT);
			return;
		}
		if (!LOGIC_CONFIG->GetTaskChainConfig()->HasCompletionTimes(param))
		{
			this->OnAcceptTask();
		}
		else
		{
			m_param.ClearTask();
		}
		TaskChainLevelTimes level_times = LOGIC_CONFIG->GetTaskChainConfig()->GetStageByCompetionTimes(param);

		m_param.task_count = level_times.index + 1;
		m_param.pass_count = param;
		m_param.next_completion_times = LOGIC_CONFIG->GetTaskChainConfig()->GetNextCompletionTimes(level_times.completion_times, competion_times);
		m_param.task_pass_count = m_param.pass_count - level_times.completion_times;
		m_param.cur_task_max_count = m_param.next_completion_times - level_times.completion_times;

		this->SendTaskInfo();
		break;
	}
}

void TaskChain::ClearTemporaryPreTask()
{
	task_type = 0;
	memset(task_param, 0, sizeof(task_param));
}

void TaskChain::GMTaskChainAccept(int task_type)
{
	int max_compoletion_times = LOGIC_CONFIG->GetTaskChainConfig()->GetMaxCompletionTimesByLevel(m_role_module_mgr->GetRole()->GetLevel());
	if (0 == max_compoletion_times) return;

	if (0 != m_param.pass_count)
	{
		if (max_compoletion_times == m_param.pass_count)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_TASK_TIME_ALREADY_MAX);
			return;
		}

		if (!LOGIC_CONFIG->GetTaskChainConfig()->HasCompletionTimes(m_param.pass_count))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_TASK_ALREADY_ACCEPTED);
			return;
		}
	}

	const TaskChainCfg * task_cfg = LOGIC_CONFIG->GetTaskChainConfig()->GetTaskByTaskID(m_role_module_mgr->GetRole()->GetLevel(), task_type);
	if (NULL == task_cfg) return;
	if (task_cfg->coin > 0)//检测铜币
	{
		if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(task_cfg->coin))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
	}
	time_t now_time = EngineAdapter::Instance().Time();
	switch (task_cfg->task_type)
	{
	case TASK_CHAIN_TYPE_TALK:
		{
			int pre_npc_id = 0;
			if (task_type == TASK_CHAIN_TYPE_ITEM || task_type == TASK_CHAIN_TYPE_PET)
			{
				pre_npc_id = task_param[0];
			}
			const TaskChainNpcCfg *npc_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandNpc(task_cfg->param1, pre_npc_id);
			if (NULL == npc_cfg) return;

			m_param.task_type = task_cfg->task_type;
			m_param.task_param[0] = npc_cfg->npc_id;
			m_param.task_param[1] = npc_cfg->reputation_city;
			m_param.task_param[2] = npc_cfg->reputation_num;
			if (task_cfg->time_limit != 0)m_param.task_limit_timestamp = now_time + task_cfg->time_limit;
		}
		break;
	case TASK_CHAIN_TYPE_ITEM:
		{
			int pre_npc_id = 0;
			if (task_type == TASK_CHAIN_TYPE_TALK || task_type == TASK_CHAIN_TYPE_PET)
			{
				pre_npc_id = task_param[0];
			}
			const TaskChainItemCfg *item_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandItem(task_cfg->param1);
			if (NULL == item_cfg) return;
			const TaskChainNpcCfg *npc_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandNpc(task_cfg->param2, pre_npc_id);
			if (NULL == npc_cfg) return;
			const TaskChainItemDropSceneCfg *drop_scene_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandItemDropScene(task_cfg->param3);
			if (NULL == drop_scene_cfg) return;

			m_param.task_type = task_cfg->task_type;
			m_param.task_param[0] = npc_cfg->npc_id;
			m_param.task_param[1] = item_cfg->item.item_id;
			m_param.task_param[2] = item_cfg->item.num;
			m_param.task_param[3] = drop_scene_cfg->scene_id;
			m_param.task_param[4] = drop_scene_cfg->item_rate;
			m_param.task_param[5] = item_cfg->is_specialway;
			m_param.task_param[6] = npc_cfg->reputation_city;
			m_param.task_param[7] = npc_cfg->reputation_num;
			if (task_cfg->time_limit != 0 && item_cfg->is_specialway == 1)m_param.task_limit_timestamp = now_time + task_cfg->time_limit;
		}
		break;
	case TASK_CHAIN_TYPE_FIGHT:
		{
			const TaskChainMonsterCfg * monster_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandMonster(task_cfg->param1);
			if (NULL == monster_cfg) return;
			const TaskChainSceneCfg * scene_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandScene(task_cfg->param2);
			if (NULL == scene_cfg) return;
			Scene * scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(scene_cfg->scene_id, COMMON_SCENE_KEY);
			if (NULL == scene) return;
			const Posi *pos = LOGIC_CONFIG->GetTaskChainConfig()->RandScenePosi(scene_cfg->pos_group_id);
			if (NULL == pos) return;
			//scene->GetARandPosi();

			m_param.task_type = task_cfg->task_type;
			m_param.task_param[0] = monster_cfg->monster_id;
			m_param.task_param[1] = scene_cfg->scene_id;
			m_param.task_param[2] = pos->x * 10000 + pos->y;
			m_param.task_param[4] = task_cfg->param3;
			m_param.task_param[5] = task_cfg->param4;
			m_param.task_param[6] = scene_cfg->reputation_city;
			m_param.task_param[7] = scene_cfg->reputation_num;
			if (task_cfg->time_limit != 0)m_param.task_limit_timestamp = now_time + task_cfg->time_limit;

			const ServerNPCInfoCfg * npc = NPCPOOL->GetServerNPCInfo(monster_cfg->monster_id);
			if (NULL != npc && SERVER_NPC_TYPE_MONSTER == npc->server_npc_type) m_param.task_param[3] = npc->server_npc_param.monster_param.monster_group_id;
		}
		break;
	case TASK_CHAIN_TYPE_PET:
		{
			int pre_npc_id = 0;
			if (task_cfg->task_type == TASK_CHAIN_TYPE_TALK || task_cfg->task_type == TASK_CHAIN_TYPE_ITEM)
			{
				pre_npc_id = task_param[0];
			}
			const TaskChainPetCfg *pet_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandPet(task_cfg->param1);
			if (NULL == pet_cfg) return;
			const TaskChainNpcCfg *npc_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandNpc(task_cfg->param2, pre_npc_id);
			if (NULL == npc_cfg) return;

			m_param.task_type = task_cfg->task_type;
			m_param.task_param[0] = npc_cfg->npc_id;
			m_param.task_param[1] = pet_cfg->item_id;
			m_param.task_param[2] = pet_cfg->pet_id;
			m_param.task_param[3] = npc_cfg->reputation_city;
			m_param.task_param[4] = npc_cfg->reputation_num;
			if (task_cfg->time_limit != 0)m_param.task_limit_timestamp = now_time + task_cfg->time_limit;
		}
		break;
	}

	m_role_module_mgr->GetMoney()->UseCoinBind(task_cfg->coin, __FUNCTION__);
	int next_completion_times = LOGIC_CONFIG->GetTaskChainConfig()->GetNextCompletionTimes(m_param.pass_count, max_compoletion_times);
	m_param.cur_task_max_count = next_completion_times - m_param.next_completion_times;
	m_param.next_completion_times = next_completion_times;
	m_param.task_pass_count = m_param.pass_count;

	this->SendTaskInfo(true);
	//随机到的怪物与自身处于同一场景时,需先下发任务,再下发创建怪物
	if (TASK_CHAIN_TYPE_FIGHT == m_param.task_type && m_role_module_mgr->GetRole()->GetScene()->GetSceneID() == m_param.task_param[1])
	{
		this->SendTaskChainMonsterInfo();
	}
}

void TaskChain::GMRefreshTaskChain()
{
	m_param.next_refresh_timestamp = 0;
}

bool TaskChain::RandTask(bool is_notice)
{
	//传入上一次任务信息
	const TaskChainCfg * task_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandTaskType(m_role_module_mgr->GetRole()->GetLevel(), task_type, m_param.pre_wait_task, m_param.pass_count);
	if (NULL == task_cfg) return false;
	/*if (task_cfg->coin > 0)//检测铜币
	{
		if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(task_cfg->coin))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
			return;
		}
	}*/
	time_t now_time = EngineAdapter::Instance().Time();
	switch (task_cfg->task_type)
	{
	case TASK_CHAIN_TYPE_TALK:
		{
			int pre_npc_id = 0;
			if (task_type == TASK_CHAIN_TYPE_ITEM || task_type == TASK_CHAIN_TYPE_PET || task_type == TASK_CHAIN_TYPE_WAIT)
			{
				pre_npc_id = task_param[0];
			}
			const TaskChainNpcCfg *npc_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandNpc(task_cfg->param1, pre_npc_id);
			if (NULL == npc_cfg) return false;
			
			m_param.task_type = task_cfg->task_type;
			m_param.can_skip = task_cfg->can_skip ? 1 : 0;
			m_param.reward_double = task_cfg->reward_double;
			m_param.price = task_cfg->price;
			m_param.price_type = task_cfg->price_type;
			m_param.task_param[0] = npc_cfg->npc_id;
			m_param.task_param[1] = npc_cfg->reputation_city;
			m_param.task_param[2] = npc_cfg->reputation_num;
			if(task_cfg->time_limit != 0)m_param.task_limit_timestamp = now_time + task_cfg->time_limit;
		}
		break;
	case TASK_CHAIN_TYPE_ITEM:
		{
			int pre_npc_id = 0;
			if (task_type == TASK_CHAIN_TYPE_TALK || task_type == TASK_CHAIN_TYPE_PET || task_type == TASK_CHAIN_TYPE_WAIT)
			{
				pre_npc_id = task_param[0];
			}
			const TaskChainItemCfg *item_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandItem(task_cfg->param1);
			if (NULL == item_cfg) return false;
			const TaskChainNpcCfg *npc_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandNpc(task_cfg->param2, pre_npc_id);
			if (NULL == npc_cfg) return false;
			const TaskChainItemDropSceneCfg *drop_scene_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandItemDropScene(task_cfg->param3);
			if (NULL == drop_scene_cfg) return false;

			m_param.task_type = task_cfg->task_type;
			m_param.can_skip = task_cfg->can_skip ? 1 : 0;
			m_param.reward_double = task_cfg->reward_double;
			m_param.price = task_cfg->price;
			m_param.price_type = task_cfg->price_type;
			m_param.task_param[0] = npc_cfg->npc_id;
			m_param.task_param[1] = item_cfg->item.item_id;
			m_param.task_param[2] = item_cfg->item.num;
			m_param.task_param[3] = drop_scene_cfg->scene_id;
			m_param.task_param[4] = drop_scene_cfg->item_rate;
			m_param.task_param[5] = item_cfg->is_specialway;
			m_param.task_param[6] = npc_cfg->reputation_city;
			m_param.task_param[7] = npc_cfg->reputation_num;
			if (task_cfg->time_limit != 0 && item_cfg->is_specialway == 1)m_param.task_limit_timestamp = now_time + task_cfg->time_limit;
		}
		break;
	case TASK_CHAIN_TYPE_FIGHT:
		{
			const TaskChainMonsterCfg * monster_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandMonster(task_cfg->param1);
			if (NULL == monster_cfg) return false;
			const TaskChainSceneCfg * scene_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandScene(task_cfg->param2);
			if (NULL == scene_cfg) return false;
			Scene * scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(scene_cfg->scene_id, COMMON_SCENE_KEY);
			if (NULL == scene) return false;
			const Posi *pos = LOGIC_CONFIG->GetTaskChainConfig()->RandScenePosi(scene_cfg->pos_group_id);
			if (NULL == pos) return false;
			//scene->GetARandPosi();

			m_param.task_type = task_cfg->task_type;
			m_param.can_skip = task_cfg->can_skip ? 1 : 0;
			m_param.reward_double = task_cfg->reward_double;
			m_param.price = task_cfg->price;
			m_param.price_type = task_cfg->price_type;
			m_param.task_param[0] = monster_cfg->monster_id;
			m_param.task_param[1] = scene_cfg->scene_id;
			m_param.task_param[2] = pos->x * 10000 + pos->y;
			m_param.task_param[4] = task_cfg->param3;
			m_param.task_param[5] = task_cfg->param4;
			m_param.task_param[6] = scene_cfg->reputation_city;
			m_param.task_param[7] = scene_cfg->reputation_num;
			if (task_cfg->time_limit != 0)m_param.task_limit_timestamp = now_time + task_cfg->time_limit;

			const ServerNPCInfoCfg * npc = NPCPOOL->GetServerNPCInfo(monster_cfg->monster_id);
			if (NULL != npc && SERVER_NPC_TYPE_MONSTER == npc->server_npc_type) m_param.task_param[3] = npc->server_npc_param.monster_param.monster_group_id;
		}
		break;
	case TASK_CHAIN_TYPE_PET:
		{
			int pre_npc_id = 0;
			if (task_cfg->task_type == TASK_CHAIN_TYPE_TALK || task_cfg->task_type == TASK_CHAIN_TYPE_ITEM || task_type == TASK_CHAIN_TYPE_WAIT)
			{
				pre_npc_id = task_param[0];
			}
			const TaskChainPetCfg *pet_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandPet(task_cfg->param1);
			if (NULL == pet_cfg) return false;
			const TaskChainNpcCfg *npc_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandNpc(task_cfg->param2, pre_npc_id);
			if (NULL == npc_cfg) return false;

			m_param.task_type = task_cfg->task_type;
			m_param.can_skip = task_cfg->can_skip ? 1 : 0;
			m_param.reward_double = task_cfg->reward_double;
			m_param.price = task_cfg->price;
			m_param.price_type = task_cfg->price_type;
			m_param.task_param[0] = npc_cfg->npc_id;
			m_param.task_param[1] = pet_cfg->item_id;
			m_param.task_param[2] = pet_cfg->pet_id;
			m_param.task_param[3] = npc_cfg->reputation_city;
			m_param.task_param[4] = npc_cfg->reputation_num;
			m_param.task_param[5] = pre_npc_id;
			if (task_cfg->time_limit != 0)m_param.task_limit_timestamp = now_time + task_cfg->time_limit;
		}
		break;
	case TASK_CHAIN_TYPE_WAIT:
		{
			int pre_npc_id = 0;
			if (task_type == TASK_CHAIN_TYPE_ITEM || task_type == TASK_CHAIN_TYPE_TALK || task_type == TASK_CHAIN_TYPE_PET)
			{
				pre_npc_id = task_param[0];
			}

			//增加一个判断以防万一交手类型任务到了这里
			if (task_type == TASK_CHAIN_TYPE_FIGHT)
			{
				const TaskChainNpcCfg *npc0_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandNpc(task_cfg->param3, pre_npc_id);
				if (NULL == npc0_cfg) return false;
				pre_npc_id = npc0_cfg->npc_id;
			}

			const TaskChainItemCfg *item_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandWaitItem(task_cfg->param1);
			if (NULL == item_cfg) return false;
			const TaskChainNpcCfg *npc_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandNpc(task_cfg->param3, pre_npc_id);
			if (NULL == npc_cfg) return false;
			
			if (pre_npc_id == 0)//前一个NPC为空
			{
				//到NPC表随机一个给他
				const TaskChainNpcCfg *npc2_cfg = LOGIC_CONFIG->GetTaskChainConfig()->RandNpc(task_cfg->param3, npc_cfg->npc_id);
				if (NULL == npc2_cfg) return false;
				pre_npc_id = npc2_cfg->npc_id;
			}

			m_param.task_type = task_cfg->task_type;
			m_param.can_skip = task_cfg->can_skip ? 1 : 0;
			m_param.reward_double = task_cfg->reward_double;
			m_param.price = task_cfg->price;
			m_param.price_type = task_cfg->price_type;
			m_param.task_param[0] = npc_cfg->npc_id;
			m_param.task_param[1] = item_cfg->item.item_id;
			m_param.task_param[2] = now_time + task_cfg->param2;
			m_param.task_param[3] = pre_npc_id;
			m_param.task_param[4] = npc_cfg->reputation_city;
			m_param.task_param[5] = npc_cfg->reputation_num;
			if (task_cfg->time_limit != 0 && item_cfg->is_specialway == 1)m_param.task_limit_timestamp = now_time + task_cfg->time_limit;

			m_param.pre_wait_task = 1;//接到代送任务记录为1
		}
		break;
	}

	//m_role_module_mgr->GetMoney()->UseCoinBind(task_cfg->coin,__FUNCTION__);
	if (is_notice)
	{
		this->SendTaskInfo(true);
		//随机到的怪物与自身处于同一场景时,需先下发任务,再下发创建怪物
		if (TASK_CHAIN_TYPE_FIGHT == m_param.task_type && m_role_module_mgr->GetRole()->GetScene()->GetSceneID() == m_param.task_param[1]) 
		{
			this->SendTaskChainMonsterInfo();
		}
	}
	return true;
}

void TaskChain::SendTaskChainMonsterInfo()
{
	if (TASK_CHAIN_TYPE_FIGHT != m_param.task_type) return;

	Protocol::SCTaskChainMonsterInfo tcmi;
	tcmi.monster_id = m_param.task_param[0];
	tcmi.monster_group_id = m_param.task_param[3];
	tcmi.scene_id = m_param.task_param[1];
	tcmi.pos_x = m_param.task_param[2] / 10000;
	tcmi.pos_y = m_param.task_param[2] % 10000;

	m_role_module_mgr->NetSend((const void *)&tcmi, sizeof(tcmi));
}

void TaskChain::ResetTurnTable()
{
	const TaskChainLevelTimesCfg * level_times_cfg = LOGIC_CONFIG->GetTaskChainConfig()->GetCompletionTimesByIndex(m_param.turn_table_completion_times_index);
	if (NULL != level_times_cfg && m_param.turn_table_completion_times_index < sizeof(m_param.has_fetch_turn_table_reward) * BIT_COUNT_OF_BYTE)
	{
		int flag = (m_param.has_fetch_turn_table_reward & (1 << m_param.turn_table_completion_times_index));
		if (((m_param.has_fetch_turn_table_reward & (1 << m_param.turn_table_completion_times_index)) == 0) /*&& m_param.turn_table_reward_index >= 0*/ && m_param.turn_table_group_id != 0 )
		{
			const ItemConfigData * reward_item = LOGIC_CONFIG->GetTaskChainConfig()->GetTurnTableReward(level_times_cfg->completion_times, m_param.accept_task_level);
			if (NULL != reward_item)
			{
				static MailContentParam contentparam; contentparam.Reset();
				contentparam.item_list[0].item_id = reward_item->item_id;
				contentparam.item_list[0].num = reward_item->num;
				contentparam.item_list[0].is_bind = reward_item->is_bind;

				int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_task_chain_turn_table_reward_subject);
				int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_task_chain_turn_table_reward_content, level_times_cfg->completion_times);
				if (length1 > 0 && length2 > 0)
				{
					MailRoute::MailToUser(m_role_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
				}
			}
		}
	}
	
	m_param.has_fetch_turn_table_reward = 0;
	m_param.turn_table_group_id = 0;
	m_param.turn_table_completion_times_index = -1;
}

void TaskChain::RandTurnTable(bool is_notice)
{
	TaskChainLevelTimes info = LOGIC_CONFIG->GetTaskChainConfig()->GetStageByCompetionTimes(m_param.pass_count);
	if(info.index < 0 || info.index >= sizeof(m_param.has_fetch_turn_table_reward) * BIT_COUNT_OF_BYTE || info.completion_times <= 0) return; 

	m_param.has_fetch_turn_table_reward &= (~(1 << info.index));
	m_param.turn_table_completion_times_index = info.index;
	m_param.turn_table_group_id = 1;

	if (is_notice)
	{
		this->SendTurnTableInfo();
	}

// 	int reward_group = LOGIC_CONFIG->GetTaskChainConfig()->GetTurnTableRewardGroup(info.completion_times, m_role_module_mgr->GetRole()->GetLevel());
// 	if (reward_group != 0)
// 	{
// 		m_param.turn_table_group_id = reward_group;
// 		m_param.turn_table_reward_index = LOGIC_CONFIG->GetTaskChainConfig()->GetRankTurnTableReward(reward_group);
// 		if (m_param.turn_table_reward_index != -1)
// 		{
// 			m_param.has_fetch_turn_table_reward &= (~(1 << info.index));
// 			m_param.turn_table_completion_times_index = info.index;
// 			if (is_notice)
// 			{
// 				this->SendTurnTableInfo();
// 			}
// 		}
// 	}
}
