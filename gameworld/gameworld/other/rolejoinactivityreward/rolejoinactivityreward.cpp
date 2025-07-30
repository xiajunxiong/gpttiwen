#include "rolejoinactivityreward.hpp"

#include "engineadapter.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/route/mailroute.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/activityconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "gamelog.h"
#include <bitset>
#include "servercommon/errornum.h"
#include "other/treasuretask/treasuretask.hpp"
#include "other/wanlingfb/wanlingfb.hpp"
#include "other/prestige_task/prestige_task.hpp"
#include "other/activesystem/activesystemconfig.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "other/bountytask/bountytask.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "other/researchtask/researchtask.h"

RoleJoinActivityReward::RoleJoinActivityReward() : m_role_module_mgr(NULL)
{

}

RoleJoinActivityReward::~RoleJoinActivityReward()
{

}

void RoleJoinActivityReward::Init(RoleModuleManager * mgr, const RoleJoinActivityRewardParam & param)
{
	m_role_module_mgr = mgr;
	m_param = param;
}

void RoleJoinActivityReward::GetInitParam(RoleJoinActivityRewardParam * param)
{
	*param = m_param;
}

void RoleJoinActivityReward::OnLogin()
{
	this->SendInfo();
}

void RoleJoinActivityReward::OnDayChange()
{
	this->RewardSettlementByMail();
}

void RoleJoinActivityReward::OnClientReq(Protocol::CSRoleJoinActivityRewardReq * _req)
{
	if (NULL == _req) return;

	switch (_req->req_type)
	{
	case Protocol::CSRoleJoinActivityRewardReq::REQ_TYPE_INFO:
		{
			this->SendInfo();
		}
		break;
	case  Protocol::CSRoleJoinActivityRewardReq::REQ_TYPE_GET_REWARD:
		{
			this->GetJoinReward(_req->activity_type);
		}
		break;
	case  Protocol::CSRoleJoinActivityRewardReq::REQ_TYPE_ONE_KEY_FINISH:
		{
			this->OneKeyFinish(_req->activity_type);	// 参数实际上是activity_types(bit位表示活动类型)
		}
		break;
	default:
		break;
	}
}

void RoleJoinActivityReward::RecodeJoinInfo(int _activity_type)
{
	if (_activity_type <= ACTIVITY_TYPE_INVALID || _activity_type >= ACTIVITY_TYPE_MAX) return;

	int old_join_flag = m_param.join_flag;
	int old_get_flag = m_param.get_reward_flag;

	m_param.join_flag |= LL_ONE_BIT << _activity_type;
	if (m_param.get_reward_flag & LL_ONE_BIT << _activity_type)
	{
		m_param.get_reward_flag &= ~(LL_ONE_BIT << _activity_type);
	}

	m_param.last_join_time = (unsigned int)EngineAdapter::Instance().Time();

	this->SendInfo();

	gamelog::g_log_activity.printf(LL_INFO, "%s  role[%d  %s]  activity_type:%d  "
		"old_join_flag:%d  new_join_flag:%d  old_get_flag:%d  new_get_flag:%d ",
		__FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), _activity_type,
		old_join_flag, m_param.join_flag, old_get_flag , m_param.get_reward_flag);
	return;
}

void RoleJoinActivityReward::AddRewards(std::vector<ItemConfigData>& rewards, ItemConfigData item)
{
	bool is_same = false;
	for (size_t i = 0; i < rewards.size(); i++)
	{
		if (rewards[i].item_id == item.item_id && rewards[i].is_bind == item.is_bind)
		{
			is_same = true;
			rewards[i].num += item.num;
			break;
		}
	}

	if (!is_same)
	{
		rewards.push_back(item);
	}
}

void RoleJoinActivityReward::GetJoinReward(int _activity_type)
{
	if (_activity_type <= ACTIVITY_TYPE_INVALID || _activity_type >= ACTIVITY_TYPE_MAX) return;

	if (!(m_param.join_flag & LL_ONE_BIT << _activity_type))
		return;

	if ( m_param.get_reward_flag & LL_ONE_BIT << _activity_type)
		return;

	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role) 
		return;

	//发放奖励
	const ActivityJoinRewardCfg * cfg = LOGIC_CONFIG->GetActivityConfig()->GetJoinRewardCfg(_activity_type , role->GetLevel());
	if (NULL == cfg)
	{
		gamelog::g_log_activity.printf(LL_INFO, "%s[WARRING_1]  role[%d  %s]  activity_type:%d  level:%d",
			__FUNCTION__, role->GetUID(), role->GetName(), _activity_type , role->GetLevel());
		return;
	}

	if (cfg->exp > 0)
	{
		role->AddExp(cfg->exp, ADD_EXP_TYPE_ROLE_JOIN_ACTIVITY_REWARD, __FUNCTION__);
	}
	
	if (cfg->coin > 0)
	{
		m_role_module_mgr->GetMoney()->AddCoinBind(cfg->coin, __FUNCTION__);
	}

	if (cfg->reward_vec.size() > 0)
	{
		for (unsigned int reward_i = 0; reward_i < cfg->reward_vec.size(); ++reward_i)
		{
			m_role_module_mgr->GetKnapsack()->Put(cfg->reward_vec[reward_i], PUT_REASON_ROLE_JOIN_ACTIVITY_REWARD);
		}
	}

	int old_flag = m_param.get_reward_flag;
	m_param.get_reward_flag |= LL_ONE_BIT << _activity_type;

	this->SendInfo();

	// 记录日志
	gamelog::g_log_activity.buff_printf("%s[SUCC] role[%d %s] activity_type[%d]  level[%d]  "
		"join_flag[%d]  old_get_flag[%d]  new_get_flag[%d] exp[%d] coin[%d] ",
		__FUNCTION__, role->GetUID(), role->GetName(), _activity_type , role->GetLevel(),  
		m_param.join_flag , old_flag , m_param.get_reward_flag , cfg->exp, cfg->coin);
	for (unsigned int reward_i = 0; reward_i < cfg->reward_vec.size(); ++reward_i)
	{
		gamelog::g_log_activity.buff_printf("item[index:%d  item_id:%d  num:%d] ",
			reward_i, cfg->reward_vec[reward_i].item_id, cfg->reward_vec[reward_i].num);
	}
	gamelog::g_log_activity.commit_buff(LL_INFO);
}


void RoleJoinActivityReward::SendInfo()
{
	Protocol::SCRoleJoinActivityRewardInfo info;

	info.join_flag = m_param.join_flag;
	info.get_reward_flag = m_param.get_reward_flag;

	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char *)&info, sizeof(info));
}

void RoleJoinActivityReward::RewardSettlementByMail()
{
	Role * role = m_role_module_mgr->GetRole();
	if (NULL == role)
		return;

	// 先记录一下忘记信息 , 免得玩家找歪
	gamelog::g_log_activity.printf(LL_INFO, "%s[Recode]  role[%d  %s]  join_flag:%d  get_reward_flag:%d  last_times:%u",
		__FUNCTION__, role->GetUID(), role->GetName(),
		m_param.join_flag , m_param.get_reward_flag , m_param.last_join_time);


	if (0 == m_param.last_join_time)
		return;

	if (m_param.join_flag == m_param.get_reward_flag)
	{
		//相等说明已经都领取过了

		m_param.join_flag = 0;
		m_param.get_reward_flag = 0;
		m_param.last_join_time = 0;

		this->SendInfo();
		return;
	}

	for (int i = ACTIVITY_TYPE_INVALID + 1; i < MAX_JOIN_ACTIVITY_REWARD_COUNT && i < ACTIVITY_TYPE_MAX; ++i)
	{
		if (!(m_param.join_flag & LL_ONE_BIT << i) )
			continue;


		if ( m_param.get_reward_flag & LL_ONE_BIT << i)
			continue;

		const ActivityJoinRewardCfg * cfg = LOGIC_CONFIG->GetActivityConfig()->GetJoinRewardCfg(i, role->GetLevel());
		if (NULL == cfg)
		{
			gamelog::g_log_activity.printf(LL_INFO, "%s[WARRING_1]  role[%d  %s]  activity_type:%d  level:%d" , 
				__FUNCTION__, role->GetUID() , role->GetName() , i , role->GetLevel());
			continue;
		}

		// 发送奖励
		static MailContentParam content_param;
		content_param.Reset();
		int lensub = SNPRINTF(content_param.subject, sizeof(content_param.subject), gamestring::g_role_join_activity_reward_subject, i);
		int length = SNPRINTF(content_param.contenttxt, sizeof(content_param.contenttxt), gamestring::g_role_join_activity_reward_content, i);
		if (lensub <= 0 || length <= 0)
		{
			gamelog::g_log_activity.printf(LL_INFO, "%s[WARRING_2] role[%d %s] activity_type[%d] ",
				__FUNCTION__ , role->GetUID() , role->GetName() , i);
			continue;
		}

		content_param.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = cfg->exp;
		content_param.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = cfg->coin;

		for (unsigned int reward_i = 0; reward_i < cfg->reward_vec.size(); ++reward_i)
		{
			content_param.item_list[reward_i].item_id = cfg->reward_vec[reward_i].item_id;
			content_param.item_list[reward_i].num = cfg->reward_vec[reward_i].num;
			content_param.item_list[reward_i].is_bind = cfg->reward_vec[reward_i].is_bind;
		}

		MailRoute::MailToUser(IntToUid(role->GetUID()), MAIL_REASON_DEFAULT, content_param);

		// 记录日志
		gamelog::g_log_activity.buff_printf("%s[SUCC] role[%d %s] activity_type[%d]  level[%d] exp[%d] coin[%d] ",
			__FUNCTION__, role->GetUID(), role->GetName(), i , role->GetLevel() , cfg->exp , cfg->coin);
		for (unsigned int reward_i = 0; reward_i < cfg->reward_vec.size(); ++reward_i)
		{
			gamelog::g_log_activity.buff_printf("item[index:%d  item_id:%d  num:%d] ",
				reward_i , cfg->reward_vec[reward_i].item_id , cfg->reward_vec[reward_i].num);
		}
		gamelog::g_log_activity.commit_buff(LL_INFO);
	}

	m_param.join_flag = 0;
	m_param.get_reward_flag = 0;
	m_param.last_join_time = 0;

	this->SendInfo();
}

bool RoleJoinActivityReward::IsCanOneClick(int activite_type)
{
	switch (activite_type)
	{
	case Protocol::CSRoleJoinActivityRewardReq::ONE_KEY_FINISH_TYPE_SHIMEN:
		{
			return m_role_module_mgr->GetBountyTask()->IsCanOneClick();
		}
		break;
	case Protocol::CSRoleJoinActivityRewardReq::ONE_KEY_FINISH_TYPE_SANJIE:
		{
			return m_role_module_mgr->GetResearchTask()->IsCanOneClick();
		}
		break;
	case Protocol::CSRoleJoinActivityRewardReq::ONE_KEY_FINISH_TYPE_SHENGWANG:
		{
			return m_role_module_mgr->GetPrestigeTask()->IsCanOneClick();
		}
		break;
	case Protocol::CSRoleJoinActivityRewardReq::ONE_KEY_FINISH_TYPE_XUNXIAN:
		{
			return m_role_module_mgr->GetFlyUpSystem()->IsCanOneClick();
		}
		break;
	case Protocol::CSRoleJoinActivityRewardReq::ONE_KEY_FINISH_TYPE_JIUZHOU:
		{
			return m_role_module_mgr->GetTreasureTask()->IsCanOneClick();
		}
		break;
	case Protocol::CSRoleJoinActivityRewardReq::ONE_KEY_FINISH_TYPE_HUABEN:
		{
			return m_role_module_mgr->GetRoleWanLingFB()->IsCanOneClick();
		}
		break;
	default:
		return false;
	}
	return false;
}

void RoleJoinActivityReward::OneKeyFinish(int bit_type)
{
	// 判断是否开启 玩家等级and开服时间
	if (m_role_module_mgr->GetRole()->GetLevel() < LOGIC_CONFIG->GetActiveSystemConfig()->GetOtherCfg().one_click_level 
		|| World::GetInstWorld()->GetCurRealOpenDay(m_role_module_mgr->GetRole()->GetUniqueServerID()) < LOGIC_CONFIG->GetActiveSystemConfig()->GetOtherCfg().one_click_open_time)
	{
		return;
	}

	std::bitset<(sizeof(bit_type) * 8)> bits(bit_type);

	if (bits.none())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	// 判断是否是周卡月卡用户
	bool is_month_card = m_role_module_mgr->GetRoleActivityManager()->HoldMonthCard();
	int consume_gold = 0;

	uint32_t exp = 0;
	int coin = 0;
	int jade = 0;
	std::vector<ItemConfigData> rewards;
	
	std::bitset<sizeof(int) * 8> out_bits;	// 最终实际完成了的任务
	for (int activity_type = 0; activity_type < Protocol::CSRoleJoinActivityRewardReq::ONE_KEY_FINISH_TYPE_MAX; activity_type++)
	{
		if (!bits[activity_type]) continue;

		// 判断可不可以一键
		if (!this->IsCanOneClick(activity_type)) continue;		
			
		if (!is_month_card)
		{
			int gold_num = LOGIC_CONFIG->GetActiveSystemConfig()->GetOneClickConsumeGold(activity_type);
			if (gold_num < 0) continue;
			consume_gold += gold_num;
		}

		out_bits.set(activity_type);
	}

	if (!is_month_card)
	{
		if (out_bits.none()) return;

		if (!m_role_module_mgr->GetMoney()->GoldMoreThan(consume_gold))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
			return;
		}

		if (consume_gold > 0)
		{
			if (!m_role_module_mgr->GetMoney()->UseGold(consume_gold, __FUNCTION__))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
				return;
			}
		}
	}

	for (int activity_type = 0; activity_type < Protocol::CSRoleJoinActivityRewardReq::ONE_KEY_FINISH_TYPE_MAX; activity_type++)
	{
		// 如果bits[activity_type]为0,continue
		if (!out_bits[activity_type]) continue;
	
		switch (activity_type)
		{
		case Protocol::CSRoleJoinActivityRewardReq::ONE_KEY_FINISH_TYPE_SHIMEN:
			{
				m_role_module_mgr->GetBountyTask()->OneClickFinishTask(exp, coin, jade, rewards);
			}
			break;
		case Protocol::CSRoleJoinActivityRewardReq::ONE_KEY_FINISH_TYPE_SANJIE:
			{
				m_role_module_mgr->GetResearchTask()->OneClickFinishTask(exp, coin, jade, rewards);
			}
			break;
		case Protocol::CSRoleJoinActivityRewardReq::ONE_KEY_FINISH_TYPE_SHENGWANG:
			{
				m_role_module_mgr->GetPrestigeTask()->OneClickFinishTask(exp, coin, jade, rewards);
			}
			break;
		case Protocol::CSRoleJoinActivityRewardReq::ONE_KEY_FINISH_TYPE_XUNXIAN:
			{
				m_role_module_mgr->GetFlyUpSystem()->OneClickFinishTask(exp, coin, jade, rewards);
			}
			break;
		case Protocol::CSRoleJoinActivityRewardReq::ONE_KEY_FINISH_TYPE_JIUZHOU:
			{
				m_role_module_mgr->GetTreasureTask()->OneClickFinishTask(exp, coin, jade, rewards);
			}
			break;
		case Protocol::CSRoleJoinActivityRewardReq::ONE_KEY_FINISH_TYPE_HUABEN:
			{
				m_role_module_mgr->GetRoleWanLingFB()->OneClickFinishTask(exp, coin, jade, rewards);
			}
			break;
		default:
			continue;
		}	
	}

	if (exp > 0){
		m_role_module_mgr->GetRole()->AddExp((long long)exp, ADD_EXP_TYPE_ROLE_ONE_CLICK_FINISH, __FUNCTION__);
	}
	if (coin > 0){
		m_role_module_mgr->GetMoney()->AddCoinBind(coin, __FUNCTION__);
	}
	if (jade > 0){
		m_role_module_mgr->GetMoney()->AddOtherCoin(jade, MONEY_TYPE_SILVER_COIN, __FUNCTION__);
	}

	if (!rewards.empty())
	{
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporaryVec(rewards))
		{
			m_role_module_mgr->GetKnapsack()->PutVec(rewards, PUT_REASON_ONE_CLICK_FINISH);
		}
		else
		{
			m_role_module_mgr->GetKnapsack()->SendMail(rewards);
		}
	}

	{
		Protocol::SCRoleOneKeyFinishRewardInfo msg;
		msg.activity_types = (int)out_bits.to_ulong();
		msg.exp = exp;
		msg.money[0] = coin;
		msg.money[1] = jade;
		msg.count = (int)rewards.size() <= ONE_KEY_FINISH_MAX_REWARDS_TYPE_NUM ? (int)rewards.size() : ONE_KEY_FINISH_MAX_REWARDS_TYPE_NUM;
		for (int i = 0; i < msg.count; i++)
		{
			msg.rewards[i].item_id = rewards[i].item_id;
			msg.rewards[i].is_bind = rewards[i].is_bind ? 1 : 0;
			msg.rewards[i].num = rewards[i].num;
		}

		m_role_module_mgr->NetSend(&msg, sizeof(msg));
	}
}