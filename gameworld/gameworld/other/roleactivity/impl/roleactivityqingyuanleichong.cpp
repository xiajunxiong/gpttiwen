#include "roleactivityqingyuanleichong.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityqingyuanleichongconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "gamelog.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"
#include "global/qingyuan/qingyuanmanager.h"
#include "other/roleactivity/roleactivitymanager.hpp"


RoleActivityQingYuanLeiChong::RoleActivityQingYuanLeiChong(int type) :RoleActivity(type)
{
}

RoleActivityQingYuanLeiChong::~RoleActivityQingYuanLeiChong()
{
}

void RoleActivityQingYuanLeiChong::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAQingYuanLeiChongParam));
}

void RoleActivityQingYuanLeiChong::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RAQingYuanLeiChongParam));
}

void RoleActivityQingYuanLeiChong::OnLogin()
{
	if (m_param.qing_yuan_id <= 0)
		return;

	QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(m_param.qing_yuan_id);
	if (NULL == qing_yuan)
	{
		m_param.qing_yuan_id = 0;
		return;
	}

	unsigned int begin_times = 0, end_timse = 0;
	qing_yuan->GetRAQingYuanLeiChongParam(&begin_times, &end_timse, NULL);

	if (begin_times != m_param.join_begin_timestamp || end_timse != m_param.join_end_timestamp)
	{
		if (begin_times < end_timse)
		{
			m_param.fetch_mark = 0;
		}
		else
		{
			this->OnRewardMail();
		}

		m_param.join_begin_timestamp = begin_times;
		m_param.join_end_timestamp = end_timse;

		this->SetRoleDataChange();
	}
}

void RoleActivityQingYuanLeiChong::Update(time_t now_second)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
		return;

	if (m_param.qing_yuan_id <= 0 || m_param.join_begin_timestamp <= 0 || m_param.join_end_timestamp <= 0)
		return;

	if (m_param.join_begin_timestamp < m_param.join_end_timestamp && now_second > m_param.join_end_timestamp)
	{
		this->OnRewardMail();

		QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(m_param.qing_yuan_id);
		if (NULL == qing_yuan)
			return;

		const RandActivityQingYuanLeiChongConfig * config = static_cast<const RandActivityQingYuanLeiChongConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
		if (NULL == config)
			return;

		long long times = m_param.join_end_timestamp + ((long long)(config->GetOtherCfg().reset_times) * 60 * 60);
		if (times > MAX_UINT32) times = MAX_UINT32;
		m_param.join_begin_timestamp = times;

		this->RASendInfo();
		this->SetRoleDataChange();
		qing_yuan->SetRAQingYuanLeiChongParam(&m_param.join_begin_timestamp, &m_param.join_end_timestamp, NULL);
	}
	if (m_param.join_end_timestamp < m_param.join_begin_timestamp && now_second > m_param.join_begin_timestamp)
	{
		QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(m_param.qing_yuan_id);
		if (NULL == qing_yuan)
			return;

		const RandActivityQingYuanLeiChongConfig * config = static_cast<const RandActivityQingYuanLeiChongConfig *>
			(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
		if (NULL == config)
			return;

		long long times = m_param.join_begin_timestamp + ((long long)(config->GetOtherCfg().continue_times) * 60 * 60);
		if (times > MAX_UINT32) times = MAX_UINT32;
		m_param.join_end_timestamp = times;
		int chongzhi_gold = 0;
		m_param.fetch_mark = 0;

		qing_yuan->SetRAQingYuanLeiChongParam(&m_param.join_begin_timestamp, &m_param.join_end_timestamp, &chongzhi_gold);
		this->RASendInfo();
		this->SetRoleDataChange();
		this->OnNotice();
	}
}

void RoleActivityQingYuanLeiChong::OnLoginSendInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
		return;

	this->RASendInfo();
}

void RoleActivityQingYuanLeiChong::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
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
	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByRoleId(m_role_module_mgr->GetRole()->GetUID());
		if (NULL != qing_yuan)
		{
			m_param.qing_yuan_id = qing_yuan->GetQingYuanId();
			qing_yuan->GetRAQingYuanLeiChongParam(&m_param.join_begin_timestamp, &m_param.join_end_timestamp, NULL);

			if (m_param.join_begin_timestamp == 0)
			{
				const RandActivityQingYuanLeiChongConfig * config = static_cast<const RandActivityQingYuanLeiChongConfig *>
					(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
				if (NULL == config)
					return;

				m_param.join_begin_timestamp = static_cast<unsigned int>(GetZeroTime(EngineAdapter::Instance().Time()));
				m_param.join_end_timestamp = m_param.join_begin_timestamp + config->GetOtherCfg().continue_times * 60 * 60;
				int congzhi_gold = 0;

				qing_yuan->SetRAQingYuanLeiChongParam(&m_param.join_begin_timestamp, &m_param.join_end_timestamp, &congzhi_gold);
			}
		}

		this->SetRoleDataChange();
	}
}

void RoleActivityQingYuanLeiChong::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
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

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}
	this->CheckInitRandActivity(m_role_activity_type);

	switch (raor->opera_type)
	{
	case 0:
		{
			this->RASendInfo();
		}
		break;
	case 1:
		{
			this->RAFetchReward(raor->param_1);
		}
		break;
	}
}

void RoleActivityQingYuanLeiChong::OnAddChongZhi(long long chongzhi)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		gamelog::g_log_role_activity.printf(LL_INFO, "%s error[NULL == ramgr] user[%d, %s], level[%d], activity_type[%d]  ", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type);
		return;
	}

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		gamelog::g_log_role_activity.printf(LL_INFO, "%s error[activity not open] user[%d, %s], level[%d], activity_type[%d]  ", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type);
		return;
	}

	QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(m_param.qing_yuan_id);
	if (NULL == qing_yuan)
	{
		gamelog::g_log_role_activity.printf(LL_INFO, "%s error[not qingyuan] user[%d, %s], level[%d], activity_type[%d] qingyuan_id[%lld] ", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
			m_param.qing_yuan_id);
		return;
	}

	unsigned int begin_times = 0, end_timse = 0;
	qing_yuan->GetRAQingYuanLeiChongParam(&begin_times, &end_timse, NULL);
	unsigned int now_timsetamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] qingyuan_id[%lld] begin_times[%d] end_times[%d] now_times[%d] chongzhi_gold[%lld]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		m_param.qing_yuan_id, begin_times, end_timse, now_timsetamp, chongzhi);
	
	if (begin_times <= now_timsetamp && now_timsetamp <end_timse)
	{
// 		long long gold = chongzhi_gold + chongzhi;
// 		if (gold > MAX_INT) gold = MAX_INT;
// 
// 		chongzhi_gold = gold;
// 		qing_yuan->SetRAQingYuanLeiChongParam(NULL, NULL, &chongzhi_gold);

		qing_yuan->RAQingYuanLeiChongAddChongZhi(chongzhi);

		this->RASendInfo();
		this->OnNotice();

		gamelog::g_log_role_activity.printf(LL_INFO, "%s succ user[%d, %s], level[%d], activity_type[%d] qingyuan_id[%lld], add_gold[%lld]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
			m_param.qing_yuan_id, chongzhi);
		return;
	}
}

void RoleActivityQingYuanLeiChong::OnJieHun(long long qingyuan_id)
{
	m_param.qing_yuan_id = qingyuan_id;

	QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(qingyuan_id);
	if (NULL != qing_yuan)
	{
		unsigned int begin_time = 0, end_time = 0;
		qing_yuan->GetRAQingYuanLeiChongParam(&begin_time, &end_time, NULL);
		if (begin_time != 0 || end_time != 0)
		{
			m_param.join_begin_timestamp = begin_time;
			m_param.join_end_timestamp = end_time;
		}
		else
		{
			unsigned int qingyuan_refresh_times = 0;
			CommonSaveQingYuanRoleInfoItem * qingyuan_info = qing_yuan->GetOtherSideMemberData(m_role_module_mgr->GetUid(), false);
			if (qingyuan_info != NULL)
			{
				Role * qingyuan_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(qingyuan_info->role_id));
				if (NULL != qingyuan_role)
				{
					RoleActivityQingYuanLeiChong * role_activity = static_cast<RoleActivityQingYuanLeiChong *>
						(qingyuan_role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(m_role_activity_type));
					if (NULL != role_activity)
					{
						qingyuan_refresh_times = role_activity->GetLiHunTimes();
					}
				}
			}
			
			const RandActivityQingYuanLeiChongConfig * config = static_cast<const RandActivityQingYuanLeiChongConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
			if (NULL == config)
				return;

			m_param.join_begin_timestamp = m_param.lihun_refresh_timestamp > qingyuan_refresh_times ?
				m_param.lihun_refresh_timestamp : qingyuan_refresh_times;

			unsigned int zero_time = static_cast<unsigned int>(GetZeroTime(EngineAdapter::Instance().Time()));
			if (m_param.join_begin_timestamp < zero_time) m_param.join_begin_timestamp = zero_time;

			m_param.join_end_timestamp = m_param.join_begin_timestamp + config->GetOtherCfg().continue_times * 60 * 60;
			int congzhi_gold = 0;

			qing_yuan->SetRAQingYuanLeiChongParam(&m_param.join_begin_timestamp, &m_param.join_end_timestamp, &congzhi_gold);
		}
	}

	m_param.fetch_mark = 0;
	m_param.lihun_refresh_timestamp = 0;
	this->SetRoleDataChange();
	this->RASendInfo();

	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] qingyuan_id[%lld] begin_time[%d] end_time[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		qingyuan_id, m_param.join_begin_timestamp, m_param.join_end_timestamp);
}

void RoleActivityQingYuanLeiChong::OnRewardMail()
{
	QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(m_param.qing_yuan_id);
	if (NULL == qing_yuan)
		return;

	int chongzhi_gold = 0;
	qing_yuan->GetRAQingYuanLeiChongParam(NULL, NULL, &chongzhi_gold);

	if (chongzhi_gold <= 0) return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityQingYuanLeiChongConfig * config = static_cast<const RandActivityQingYuanLeiChongConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	int gift_size = config->GetGiftSize(ramgr, m_param.join_begin_timestamp);
	for (int i = 0; i < gift_size; i++)
	{
		if(IsSetBit(m_param.fetch_mark, i))
			continue;

		const RAQingYuanLeiChongCfg * cfg = config->GetCfg(ramgr, i, m_param.join_begin_timestamp);
		if (NULL == cfg)
			continue;
		
		if(chongzhi_gold < cfg->acc_price)
			continue;

		m_role_module_mgr->GetKnapsack()->SendMail(cfg->reward_item, SEND_MAIL_TYPE_ACTIVITY_END, false, m_role_activity_type);

		SetBit(m_param.fetch_mark, i);

		gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] fetch_index[%d]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
			i);
	}
}

void RoleActivityQingYuanLeiChong::OnLiHunBuFa(int chongzhi_gold, bool is_offline)
{
	if (is_offline)	// 离线数据来补发的 不会有情缘还存在的情况, 除非一直没下过线，再跨服又结了一次婚
	{
		QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByRoleId(m_role_module_mgr->GetRole()->GetUID());
		if (NULL != qing_yuan)
			return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	const RandActivityQingYuanLeiChongConfig * config = static_cast<const RandActivityQingYuanLeiChongConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		return;
	}

	int gift_size = config->GetGiftSize(ramgr, m_param.join_begin_timestamp);
	for (int i = 0; i <= gift_size; i++)
	{
		if (IsSetBit(m_param.fetch_mark, i))
			continue;

		const RAQingYuanLeiChongCfg * cfg = config->GetCfg(ramgr, i, m_param.join_begin_timestamp);
		if (NULL == cfg)
			continue;

		if (chongzhi_gold < cfg->acc_price)
			continue;

		m_role_module_mgr->GetKnapsack()->SendMail(cfg->reward_item, SEND_MAIL_TYPE_ACTIVITY_END, false, m_role_activity_type);

		SetBit(m_param.fetch_mark, i);

		gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] fetch_index[%d]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
			i);
	}

	if (m_param.lihun_refresh_timestamp == 0)
	{
		if (m_param.join_begin_timestamp < m_param.join_end_timestamp)
		{
			long long times = m_param.join_end_timestamp + ((long long)(config->GetOtherCfg().reset_times) * 60 * 60);
			if (times > MAX_UINT32) times = MAX_UINT32;
			m_param.lihun_refresh_timestamp = times;
		}
		else
		{
			m_param.lihun_refresh_timestamp = m_param.join_begin_timestamp;
		}
	}

	this->SetRoleDataChange();
}

void RoleActivityQingYuanLeiChong::RASendInfo()
{
	Protocol::SCRAQingYuanLeiChongInfo info;
	QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(m_param.qing_yuan_id);
	if (NULL == qing_yuan)
		return;

	qing_yuan->GetRAQingYuanLeiChongParam(&info.join_begin_timestamp, &info.join_end_timestamp, &info.chongzhi_gold);
	info.fetch_mark = m_param.fetch_mark;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityQingYuanLeiChong::RAFetchReward(int seq_index)
{
	QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(m_param.qing_yuan_id);
	if (NULL == qing_yuan)
		return;

	if (seq_index < 0 || seq_index >= 32)
		return;

	if (IsSetBit(m_param.fetch_mark, seq_index))
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RandActivityQingYuanLeiChongConfig * config = static_cast<const RandActivityQingYuanLeiChongConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAQingYuanLeiChongCfg * cfg = config->GetCfg(ramgr, seq_index, m_param.join_begin_timestamp);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	int chongzhi_gold = 0;
	qing_yuan->GetRAQingYuanLeiChongParam(NULL, NULL, &chongzhi_gold);

	if (chongzhi_gold < cfg->acc_price)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_CUMULATIVE_RECHARGE_ACC_PRICE_ERR);
		return;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutList2(cfg->reward_item, false))
	{
		m_role_module_mgr->GetKnapsack()->PutVec(cfg->reward_item, PUT_REASON_RA_QINGYUAN_LEICHONG);
	}
	else
	{
		m_role_module_mgr->GetKnapsack()->SendMail(cfg->reward_item, SEND_MAIL_TYPE_ACTIVITY_REWARD, true, m_role_activity_type);
	}

	SetBit(m_param.fetch_mark, seq_index);

	this->RASendInfo();
	this->SetRoleDataChange();

	//
	gamelog::g_log_role_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], activity_type[%d] fetch_index[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetLevel(), m_role_activity_type,
		seq_index);
}

void RoleActivityQingYuanLeiChong::OnNotice()
{
	QingYuan * qing_yuan = QingYuanManager::Instance().GetQingYuanByQingYuanId(m_param.qing_yuan_id);
	if (NULL == qing_yuan)
	{
		return;
	}

	CommonSaveQingYuanRoleInfoItem * qingyuan_data = qing_yuan->GetOtherSideMemberData(m_role_module_mgr->GetRole()->GetUID());
	if (NULL != qingyuan_data)
	{
		Role * qingyuan_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(qingyuan_data->role_id));
		if (NULL != qingyuan_role)
		{
			RoleActivityQingYuanLeiChong * qingyuan_leichong = static_cast<RoleActivityQingYuanLeiChong *>
				(qingyuan_role->GetRoleModuleManager()->GetRoleActivityManager()->GetRoleActivity(m_role_activity_type));
			if (NULL != qingyuan_leichong)
			{
				qingyuan_leichong->RASendInfo();
			}
		}
	}
}
