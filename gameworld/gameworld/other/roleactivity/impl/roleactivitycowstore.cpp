#include "roleactivitycowstore.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivitycowstoreconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"

RoleActivityCowStore::RoleActivityCowStore(int type)
	:RoleActivity(type)
{

}

RoleActivityCowStore::~RoleActivityCowStore()
{

}

void RoleActivityCowStore::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RACowStoreParam));
}

void RoleActivityCowStore::GetInitParam(RoleActivityRoleData *p)
{
	memcpy(p->base_data, &m_param, sizeof(RACowStoreParam));
}

void RoleActivityCowStore::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	//���ˢ��
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_COW_STORE))
	{
		return;
	}

	int rand_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_COW_STORE);
	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_COW_STORE);
	const RandActivityCowStoreConfig * config = static_cast<const RandActivityCowStoreConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COW_STORE));
	if (config != NULL)
	{
		const RACowStoreCfg * old_cfg = config->GetCowStoreCfgByActivityDay(open_day, m_param.open_day, 0);
		const RACowStoreCfg * new_cfg = config->GetCowStoreCfgByActivityDay(open_day, rand_open_day, 0);
		if (NULL != old_cfg && NULL != new_cfg)
		{
			if (old_cfg->section_start != new_cfg->section_start)
			{
				//�׶�ˢ��
				memset(m_param.activity_buy_times, 0, sizeof(m_param.activity_buy_times));
			}
		}
	}

	if (m_param.open_day == rand_open_day)
	{
		//return;//������GM������ֱ��ˢ����Ŀ
	}

	m_param.open_day = rand_open_day;
	m_param.DayReset();
	this->SendInfo();
	this->SetRoleDataChange();
}

void RoleActivityCowStore::OnLoginSendInfo()
{
	//���ˢ��
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_COW_STORE))
	{
		return;
	}

	this->SendInfo();
}

void RoleActivityCowStore::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_COW_STORE != rand_act_type)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	// ��ֹ������������ڿ�������Ļ�ᵼ������
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity *activity = ramgr->ForceGetRandActivity(rand_act_type);
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.begin_time);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.begin_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		int rand_open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActOpenDay(ramgr, RAND_ACTIVITY_TYPE_COW_STORE);
		const RandActivityCowStoreConfig * config = static_cast<const RandActivityCowStoreConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COW_STORE));
		if (NULL == config)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}
		m_param.open_day = rand_open_day;
		this->SetRoleDataChange();
		this->SendInfo();
	}
}

void RoleActivityCowStore::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
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

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_COW_STORE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_COW_STORE);

	switch (raor->opera_type)
	{
	case 0://�����·�
		{
			this->SendInfo();
		}
		break;
	case 1://������param1 = type��param2 = seq
		{
			this->BuyItem(raor->param_1, raor->param_2);
		}
		break;
	}
}

bool RoleActivityCowStore::OnCmdBuyItem(int type_id, int real_pay)
{
	//ֱ�������������أ�һ������ʧ�ܾ����ش�����
	const RandActivityCowStoreConfig * config = static_cast<const RandActivityCowStoreConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COW_STORE));
	if (NULL == config)
	{
		gamelog::g_log_serious_error.printf(LL_INFO, "RoleActivityCowStore::OnCmdBuyItem Err!user[%d %s] type_id[%d] chongzhi_gold[%d] reason[config NULL]",
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), type_id, real_pay);
		return false;
	}

	const RACowStoreCfg * buy_item_cfg = config->GetCowStoreCfgByIndex(type_id);
	if (NULL == buy_item_cfg)
	{
		gamelog::g_log_serious_error.printf(LL_INFO, "RoleActivityCowStore::OnCmdBuyItem Err!user[%d %s] type_id[%d] chongzhi_gold[%d] reason[buy_item_cfg NULL]",
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), type_id, real_pay);
		return false;
	}

	if (real_pay < buy_item_cfg->price)
	{
		gamelog::g_log_serious_error.printf(LL_INFO, "RoleActivityCowStore::OnCmdBuyItem Err!user[%d %s] type_id[%d] chongzhi_gold[%d] reason[real_pay < buy_item_cfg->price]",
			m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), type_id, real_pay);
		return false;
	}

	short & today_buy_times = m_param.today_buy_times[buy_item_cfg->seq];
	short & activity_buy_times = m_param.activity_buy_times[buy_item_cfg->seq];
	switch (buy_item_cfg->limit_type)
	{
	case RACowStoreCfg::LIMIT_TYPE_TODAY:
		{
			if (today_buy_times > 0 && today_buy_times >= buy_item_cfg->today_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
				return false;
			}
		}
		break;
	case RACowStoreCfg::LIMIT_TYPE_ACTIVITY:
		{
			if (activity_buy_times > 0 && activity_buy_times >= buy_item_cfg->buy_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
				return false;
			}
		}
		break;
	default:
		break;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPut(buy_item_cfg->reward_item.item_id, buy_item_cfg->reward_item.num))
	{
		if (!m_role_module_mgr->GetKnapsack()->Put(buy_item_cfg->reward_item, PUT_REASON_RA_COW_STORE))
		{
			gamelog::g_log_serious_error.printf(LL_INFO, "RoleActivityCowStore::OnCmdBuyItem Err!user[%d %s] type_id[%d] chongzhi_gold[%d] reason[buy_item_cfg NULL]",
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRole()->GetName(), type_id, real_pay);
			return false;
		}
	}
	else
	{
		//���뱳��ʧ�ܣ��ķ��ʼ�
		static MailContentParam contentparam; contentparam.Reset();
		const ItemBase * item_base = ITEMPOOL->GetItem(buy_item_cfg->reward_item.item_id);

		if (item_base != NULL)
		{
			contentparam.item_list[0].item_id = buy_item_cfg->reward_item.item_id;
			contentparam.item_list[0].num = buy_item_cfg->reward_item.num;
			contentparam.item_list[0].is_bind = (buy_item_cfg->reward_item.is_bind) ? 1 : 0;
			contentparam.item_list[0].invalid_time = item_base->CalInvalidTime();
		}
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_content, RAND_ACTIVITY_TYPE_COW_STORE);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	today_buy_times += 1;
	activity_buy_times += 1;

	this->SendInfo();
	this->SetRoleDataChange();
	return true;
}

int RoleActivityCowStore::OnBuyGiftItem(int seq, ARG_OUT int * price)
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return 1;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_COW_STORE))
	{
		return 1;
	}

	const RandActivityCowStoreConfig * config = static_cast<const RandActivityCowStoreConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COW_STORE));
	if (NULL == config)
	{
		return 1;
	}

	int open_day = config->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_COW_STORE);
	const RACowStoreCfg * buy_item_cfg =  config->GetCowStoreCfgByActivityDay(open_day, m_param.open_day, seq);
	if (NULL == buy_item_cfg)
	{
		return 1;
	}
	if (NULL != price)
	{
		*price = buy_item_cfg->price;
		return 0;
	}

	short & today_buy_times = m_param.today_buy_times[buy_item_cfg->seq];
	short & activity_buy_times = m_param.activity_buy_times[buy_item_cfg->seq];
	switch (buy_item_cfg->limit_type)
	{
	case RACowStoreCfg::LIMIT_TYPE_TODAY:
	{
		if (today_buy_times > 0 && today_buy_times >= buy_item_cfg->today_times)
		{
			return 2;
		}
	}
	break;
	case RACowStoreCfg::LIMIT_TYPE_ACTIVITY:
	{
		if (activity_buy_times > 0 && activity_buy_times >= buy_item_cfg->buy_times)
		{
			return 2;
		}
	}
	break;
	default:
		break;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPut(buy_item_cfg->reward_item.item_id, buy_item_cfg->reward_item.num))
	{
		m_role_module_mgr->GetKnapsack()->Put(buy_item_cfg->reward_item, PUT_REASON_RA_COW_STORE);
	}
	else
	{
		//���뱳��ʧ�ܣ��ķ��ʼ�
		static MailContentParam contentparam; contentparam.Reset();
		const ItemBase * item_base = ITEMPOOL->GetItem(buy_item_cfg->reward_item.item_id);

		if (item_base != NULL)
		{
			contentparam.item_list[0].item_id = buy_item_cfg->reward_item.item_id;
			contentparam.item_list[0].num = buy_item_cfg->reward_item.num;
			contentparam.item_list[0].is_bind = (buy_item_cfg->reward_item.is_bind) ? 1 : 0;
			contentparam.item_list[0].invalid_time = item_base->CalInvalidTime();
		}
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_content, RAND_ACTIVITY_TYPE_COW_STORE);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	today_buy_times += 1;
	activity_buy_times += 1;

	this->SendInfo();
	this->SetRoleDataChange();
	return 0;
}

void RoleActivityCowStore::SendInfo()
{
	Protocol::SCRACowStoreInfo info;
	
	info.open_day = m_param.open_day;
	
	memcpy(info.today_buy_times, m_param.today_buy_times, sizeof(info.today_buy_times));
	memcpy(info.activity_buy_times, m_param.activity_buy_times, sizeof(info.activity_buy_times));

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityCowStore::BuyItem(int answer_id, int seq_id_check)
{
	if (seq_id_check < 0 || seq_id_check >= MAX_LOG_TIMES)
	{
		return;
	}
	const RandActivityCowStoreConfig * config = static_cast<const RandActivityCowStoreConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_COW_STORE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	if (0 == m_param.open_day)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	int open_day = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, RAND_ACTIVITY_TYPE_COW_STORE);
	const RACowStoreCfg *check_item_cfg = config->GetCowStoreCfgByActivityDay(open_day, m_param.open_day, seq_id_check);
	if (NULL == check_item_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RACowStoreCfg * buy_item_cfg = config->GetCowStoreCfgByIndex(answer_id);
	if (NULL == buy_item_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	//��鹺����ߵĶ�Ӧ�Ƿ���ȷ
	if (buy_item_cfg->seq != seq_id_check || buy_item_cfg->seq < 0 || buy_item_cfg->seq >= MAX_LOG_TIMES)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	//�������
	short has_buy_times_today = m_param.today_buy_times[buy_item_cfg->seq];
	short has_buy_times_activity = m_param.activity_buy_times[buy_item_cfg->seq];
	switch (buy_item_cfg->limit_type)
	{
	case RACowStoreCfg::LIMIT_TYPE_TODAY:
		{
			if (has_buy_times_today > 0 && has_buy_times_today >= buy_item_cfg->today_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
				return;
			}
		}
		break;
	case RACowStoreCfg::LIMIT_TYPE_ACTIVITY:
		{
			if (has_buy_times_activity > 0 && has_buy_times_activity >= buy_item_cfg->buy_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RA_SPRING_FESTIVAL_STORE_TIMES_LIMIT);
				return;
			}
		}
		break;
	default:
		break;
	}

	//�������
	switch (buy_item_cfg->price_type)
	{
	case RACowStoreCfg::PRICE_TYPE_COIN:
		{
			if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(buy_item_cfg->price))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
				return;
			}
			m_role_module_mgr->GetMoney()->UseCoinBind(buy_item_cfg->price, __FUNCTION__);
		}
		break;
	case RACowStoreCfg::PRICE_TYPE_GOLD:
		{
			if (!m_role_module_mgr->GetMoney()->GoldMoreThan(buy_item_cfg->price))
			{
				m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
				return;
			}
			m_role_module_mgr->GetMoney()->UseGold(buy_item_cfg->price, __FUNCTION__);
		}
		break;
	case RACowStoreCfg::PRICE_TYPE_MONEY:
		{
			return;
		}
		break;
	default:
		break;
	}

	//����
	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(buy_item_cfg->reward_item.item_id, buy_item_cfg->reward_item.num))
	{
		//���뱳��ʧ�ܣ��ķ��ʼ�
		static MailContentParam contentparam; contentparam.Reset();
		const ItemBase * item_base = ITEMPOOL->GetItem(buy_item_cfg->reward_item.item_id);

		if (item_base != NULL)
		{
			contentparam.item_list[0].item_id = buy_item_cfg->reward_item.item_id;
			contentparam.item_list[0].num = buy_item_cfg->reward_item.num;
			contentparam.item_list[0].is_bind = (buy_item_cfg->reward_item.is_bind) ? 1 : 0;
			contentparam.item_list[0].invalid_time = item_base->CalInvalidTime();
		}
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_randactivity_reward_content, RAND_ACTIVITY_TYPE_COW_STORE);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
	else
	{
		if (!m_role_module_mgr->GetKnapsack()->Put(buy_item_cfg->reward_item, PUT_REASON_RA_COW_STORE))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		}
	}
	

	m_param.activity_buy_times[seq_id_check] += 1;
	m_param.today_buy_times[seq_id_check] += 1;
	
	this->SendInfo();
	this->SetRoleDataChange();
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_rand_activity.printf(LL_INFO, "RoleActivityCowStore::Answer user[%d, %s], level[%d], type[%d], reward_item[%d] num[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(),
		RAND_ACTIVITY_TYPE_COW_STORE, buy_item_cfg->reward_item.item_id, buy_item_cfg->reward_item.num);
}
