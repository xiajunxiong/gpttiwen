#include "roleactivityxianshileichong.hpp"

#include "engineadapter.h"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "protocol/randactivity/msgraxianshileichong.hpp"
#include "global/randactivity/randactivityimpl/randactivityxianshileichong.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityxianshileichongconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "servercommon/errornum.h"
#include "gamelog.h"

RoleActivityXianShiLeiChong::RoleActivityXianShiLeiChong(int type)
	:RoleActivity(type)
{

}

RoleActivityXianShiLeiChong::~RoleActivityXianShiLeiChong()
{

}

void RoleActivityXianShiLeiChong::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAXianShiLeiChongParam));
}

void RoleActivityXianShiLeiChong::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RAXianShiLeiChongParam));
}

void RoleActivityXianShiLeiChong::OnLogin()
{
	// 放在这里是为了做一个容错
	this->CalcChongZhiInfo();

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG))
	{
		if (0 != m_param.can_fetch_flag)
		{
			//没有领取的奖励发邮件
			this->OnRAClose();
		}
	}

	this->SendInfo();
}

void RoleActivityXianShiLeiChong::OnAddChongZhi(long long chongzhi)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG))
	{
		return;
	}

	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG);

	int old_total_gold = m_param.total_gold;
	m_param.total_gold += chongzhi;
	// 计算一下可领取标记, 因为最后要补发而每日充值都会清0 , 所以需要记录一下标记位
	this->SetRoleDataChange();
	this->CalcChongZhiInfo();

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d %s] chongzhi:%lld  old_total_chongzhi:%d new_total_chongzhi:%d ",
		__FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		chongzhi , old_total_gold , m_param.total_gold);
}

void RoleActivityXianShiLeiChong::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG != rand_act_type)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.last_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.last_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		this->SendInfo();
		this->SetRoleDataChange();
	}
}

void RoleActivityXianShiLeiChong::OnRAClose()
{
	this->ReplacementReward();
	this->SendInfo();
	this->SetRoleDataChange();
}

void RoleActivityXianShiLeiChong::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
{
	if (NULL == raor)
		return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG);

	switch (raor->opera_type)
	{
	case Protocol::RA_XIAN_SHI_LEI_CHONG_REQ_TYPE_INFO:
		{
			this->SendInfo();
		}
		break;
	case Protocol::RA_XIAN_SHI_LEI_CHONG_REQ_TYPE_GET_REWARD:
		{
			this->GetReward(raor->param_1);
		}
		break;
	}
}

void RoleActivityXianShiLeiChong::CalcChongZhiInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
		return;
	RandActivityXianShiLeiChong * ra = static_cast<RandActivityXianShiLeiChong *>(ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG));
	if (NULL == ra)
		return;
	const RandActivityXianShiLeiChongConfig * cfg = static_cast<const RandActivityXianShiLeiChongConfig *>(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG));
	if (NULL == cfg)
		return;
	const std::vector<RAXianShiLeiChongCfg> * cfg_vec = cfg->GetRewardCfg(ra->GetOpenDay());
	if (NULL == cfg_vec)
		return;

	int old_flag = m_param.can_fetch_flag;
	for (int i = 0; i < (int)cfg_vec->size(); ++i)
	{
		if(m_param.total_gold < cfg_vec->at(i).acc_price)
			break;

		m_param.can_fetch_flag |= (1 << i);
	}

	if (old_flag != m_param.can_fetch_flag)
	{
		this->SetRoleDataChange();
	}

	this->SendInfo();
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d %s] total_chongzhi:%d open_day:%d can_fetch_flag:%d",
		__FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		m_param.total_gold, ra->GetOpenDay(), m_param.can_fetch_flag);
}

void RoleActivityXianShiLeiChong::GetReward(int _seq)
{
	if (!(m_param.can_fetch_flag & (1 << _seq)))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GIVE_SECRET_KEY_PARAM_NOT_ADV);
		return;
	}

	if (m_param.flag & (1 << _seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
		return;
	RandActivityXianShiLeiChong * ra = static_cast<RandActivityXianShiLeiChong *>(ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG));
	if (NULL == ra)
		return;
	const RandActivityXianShiLeiChongConfig * cfg = static_cast<const RandActivityXianShiLeiChongConfig *>(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG));
	if (NULL == cfg)
		return;
	const RAXianShiLeiChongCfg * reward_cfg = cfg->GetRewardCfgBySeq(ra->GetOpenDay() , _seq);
	if (NULL == reward_cfg)
		return;

	bool ret = true;
	if (!m_role_module_mgr->GetKnapsack()->PutList(reward_cfg->count, reward_cfg->reward_arr, PUT_REASON_RA_XIAN_SHI_LEI_CHONG))
	{
		ret = m_role_module_mgr->GetKnapsack()->SendMail(reward_cfg->reward_arr, reward_cfg->count, SEND_MAIL_TYPE_RA_XIAN_SHI_LEI_CHONG);
	}

	if (!ret)
	{
		gamelog::g_log_role_activity.printf(LL_INFO ,"%s[ERROR] user[%d %s] open_day:%d  seq:%d can_fetch_flag:%d  flag:%d",
			__FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
			ra->GetOpenDay(), _seq, m_param.can_fetch_flag,m_param.flag);
	}
	else
	{
		int old_flag = m_param.flag;
		m_param.flag |= (1 << _seq);
		this->SetRoleDataChange();
		this->SendInfo();

		gamelog::g_log_role_activity.buff_printf("%s user[%d %s] open_day:%d  seq:%d can_fetch_flag:%d  old_flag:%d  flag:%d",
			__FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
			ra->GetOpenDay(), _seq, m_param.can_fetch_flag, old_flag , m_param.flag);
		for (int j = 0; j < reward_cfg->count; ++j)
		{
			gamelog::g_log_role_activity.buff_printf("item[index:%d  item_id:%d  num:%d] ",
				j, reward_cfg->reward_arr[j].item_id, reward_cfg->reward_arr[j].num);
		}
		gamelog::g_log_role_activity.commit_buff(LL_INFO);
	}

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityXianShiLeiChong::ReplacementReward(bool is_day_change )
{
	if (m_param.can_fetch_flag == m_param.flag)
	{
		// 相等说明都领取过了
		m_param.flag = 0;
		m_param.can_fetch_flag = 0;
		m_param.total_gold = 0;
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
		return;
	RandActivityXianShiLeiChong * ra = static_cast<RandActivityXianShiLeiChong *>(ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG));
	if (NULL == ra)
		return;
	const RandActivityXianShiLeiChongConfig * cfg = static_cast<const RandActivityXianShiLeiChongConfig *>(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG));
	if (NULL == cfg)
		return;
	const std::vector<RAXianShiLeiChongCfg> * cfg_vec = cfg->GetRewardCfg(ra->GetOpenDay());
	if (NULL == cfg_vec)
		return;

	MailContentParam contentparam; contentparam.Reset();
	int len_0 = 0, len_1 = 0;
	len_0 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_leijichongzhi_subbject);
	if (is_day_change)
	{
		len_1 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt),gamestring::g_leijichongzhi_day_content);
	}
	else
	{
		len_1 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt),gamestring::g_leijichongzhi_content);
	}

	if (len_0 <= 0 || len_1 <= 0)
		return;

	for (int i = 0; i < (int)cfg_vec->size(); ++i)
	{
		if(m_param.flag & (1 << i))
			continue; //已经领取过了
		if(!(m_param.can_fetch_flag & (1 << i)))
			continue; //不能领取

		memset(contentparam.item_list, 0, sizeof(contentparam.item_list));
		for (int j = 0; j < cfg_vec->at(i).count; ++j)
		{
			contentparam.item_list[j].item_id = cfg_vec->at(i).reward_arr[j].item_id;
			contentparam.item_list[j].num = cfg_vec->at(i).reward_arr[j].num;
			contentparam.item_list[j].is_bind = cfg_vec->at(i).reward_arr[j].is_bind;
		}

		MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);

		gamelog::g_log_role_activity.buff_printf("%s user[%d %s] is_day_change:%d open_day:%d  seq:%d can_fetch_flag:%d flag:%d",
			__FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),is_day_change,
			ra->GetOpenDay() , i , m_param.can_fetch_flag ,  m_param.flag);
		for (int j = 0; j < cfg_vec->at(i).count; ++j)
		{
			gamelog::g_log_role_activity.buff_printf("item[index:%d  item_id:%d  num:%d] ",
				j , cfg_vec->at(i).reward_arr[j].item_id , cfg_vec->at(i).reward_arr[j].num );
		}
		gamelog::g_log_role_activity.commit_buff(LL_INFO);
	}

	m_param.flag = 0;
	m_param.can_fetch_flag = 0;
	m_param.total_gold = 0; 
}

void RoleActivityXianShiLeiChong::SendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
		return;
	
	RandActivityXianShiLeiChong * ra = static_cast<RandActivityXianShiLeiChong *>(ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG));
	if (NULL == ra)
		return;

	Protocol::SCRAXianShiLeiChongInfo info;
	info.open_day = ra->GetOpenDay();
	info.total_chong_zhi = m_param.total_gold;
	info.can_fetch_flag = m_param.can_fetch_flag;
	info.flag = m_param.flag;

	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*)&info, sizeof(info));
}
