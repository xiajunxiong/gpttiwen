#include "kuafuyingxionglumanager.hpp"
#include "world.h"
#include "internalcomm.h"
#include "gamelog.h"

#include "servercommon/uniqueidgenerator.hpp"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "config/logicconfigmanager.hpp"
#include "config/sharedconfig/sharedconfig.h"
#include "config/otherconfig/kuafuyingxiongluconfig.hpp"

#include "protocol/msgkuafuyingxionglu.hpp"

#include "global/rank/rankmanager.hpp"
#include "global/usercache/usercache.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "global/datasynccheck/datasynccheck.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/route/mailroute.hpp"
#include "other/kuafuyingxionglu/rolekuafuyingxionglu.hpp"

KuaFuYingXiongLuManager & KuaFuYingXiongLuManager::Instance()
{
	static KuaFuYingXiongLuManager manager;
	return manager;
}

void KuaFuYingXiongLuManager::Update(unsigned long interval, time_t now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	this->OnCheckTime((unsigned int)now_second);
	this->OnCheckOpenNew((unsigned int)now_second);
}

void KuaFuYingXiongLuManager::Init(const KuaFuYingXiongLuActivityParam & param)
{
	m_param = param;
}

void KuaFuYingXiongLuManager::GetInitParam(KuaFuYingXiongLuActivityParam * param)
{
	*param = m_param;
}

void KuaFuYingXiongLuManager::OnRoleLogin(Role * role)
{
	if (NULL == role)
	{
		return;
	}

	this->SendAllInfo(role);
}

void KuaFuYingXiongLuManager::OnReloadConfig()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		std::vector<int> online_user_list;
		UserCacheManager::Instance().GetUidListByStatusMap(&online_user_list, UserStatusNode::STATUS_TYPE_ONLINE);
		for (int i = 0; i < (int)online_user_list.size(); i++)
		{
			Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(online_user_list[i]);
			if (NULL == role) continue;

			role->GetRoleModuleManager()->GetRoleKuaFuYingXiongLu()->OnReloadConfig();
		}
	}
	else
	{
		std::vector<int> online_user_list;
		UserCacheManager::Instance().GetUidListByStatusMap(&online_user_list, UserStatusNode::STATUS_TYPE_CROSS);
		for (int i = 0; i < (int)online_user_list.size(); i++)
		{
			Role *role = World::GetInstWorld()->GetSceneManager()->GetRole(online_user_list[i]);
			if (NULL == role) continue;

			role->GetRoleModuleManager()->GetRoleKuaFuYingXiongLu()->OnReloadConfig();
		}
	}
}

void KuaFuYingXiongLuManager::OnConnectCrossSucc()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenReqSynKuaFuYingXiongLuData ghrskfyxld;
		ghrskfyxld.server_id = LocalConfig::Instance().GetMergeServerId();

		InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghrskfyxld, sizeof(ghrskfyxld));
		DataSyncCheck::Instance().AddCheck(crossgameprotocal::DATA_SYNC_CHECK_TYPE_KUA_FU_YING_XIONG_LU);
	}
	else
	{
		this->OnHiddenSynDataToGameworld();
	}
}

void KuaFuYingXiongLuManager::OnHiddenSynDataToGameworld(int server_id)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::HiddenGameRetSynKuaFuYingXiongLuData hgskfyxlfd;
	hgskfyxlfd.data = m_param;

	if (0 == server_id)
	{
		InternalComm::Instance().SendToAllGameThroughCross(&hgskfyxlfd, sizeof(hgskfyxlfd));
	}
	else
	{
		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), server_id, (const char *)&hgskfyxlfd, sizeof(hgskfyxlfd));
	}
}

void KuaFuYingXiongLuManager::OnGameRecvHiddenSynData(crossgameprotocal::HiddenGameRetSynKuaFuYingXiongLuData * msg)
{
	if (CrossConfig::Instance().IsHiddenServer() || NULL == msg) return;

	m_param = msg->data;
	this->SendAllInfo();
}

void KuaFuYingXiongLuManager::OnHiddenRecvCrossActivityRankEnd(crossgameprotocal::CrossHiddenRetKuaFuYingXiongLuRankEnd * msg, std::vector<int>& rank_uid_list)
{
	if (NULL == msg || msg->kfyxl_rank_type < 0 || msg->kfyxl_rank_type >= ARRAY_ITEM_COUNT(m_param.rank_list) || msg->activity_unique_id != m_param.activity_unique_id)
	{
		return;
	}

	//已经发过了
	if (0 != m_param.rank_list[msg->kfyxl_rank_type].is_give_rand_activity_reward_flag)
	{
		return;
	}

	std::vector<KuaFuYingXiongLuRankCfg> rank_vec;
	LOGIC_CONFIG->GetKuaFuYingXiongLuConfig()->GetRankVec(m_param.activity_open_cross_day, msg->kfyxl_rank_type, rank_vec);
	if (rank_vec.empty()) return;

	for (int i = 0; i < (int)rank_vec.size() && i < (int)rank_uid_list.size(); i++)
	{
		if (rank_uid_list[i] <= 0) continue;

		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, rank_vec[i].reward_list);
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_kua_fu_ying_xiong_lu_rank_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_kua_fu_ying_xiong_lu_rank_content, msg->kfyxl_rank_type,
			i + 1);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(rank_uid_list[i], MAIL_REASON_DEFAULT, contentparam);
			gamelog::g_log_kua_fu_ying_xiong_lu.printf(LL_INFO, "%s uid[%d] rank[%d, %d] succ", __FUNCTION__, rank_uid_list[i], msg->kfyxl_rank_type, i + 1);
		}
	}
	m_param.rank_list[msg->kfyxl_rank_type].is_give_rand_activity_reward_flag = 1;
	gamelog::g_log_kua_fu_ying_xiong_lu.printf(LL_INFO, "%s rank_type[%d] unique_id[%lld]", __FUNCTION__, msg->kfyxl_rank_type, msg->activity_unique_id);
	this->OnCheckOpenNew((unsigned int)EngineAdapter::Instance().Time());
}

void KuaFuYingXiongLuManager::SendAllInfo(Role * role)
{
	Protocol::SCKuaFuYingXiongLuActivityInfo info;
	info.cross_open_day = m_param.activity_open_cross_day;
	info.activity_begin_timestamp = m_param.activity_begin_timestamp;
	info.activty_end_timestamp = m_param.activty_end_timestamp;
	info.count = 0;

	for (int i = 0; i < ARRAY_ITEM_COUNT(info.rank_end_timestamp_list) && i < ARRAY_ITEM_COUNT(m_param.rank_list) && i < KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX; ++i)
	{
		info.rank_end_timestamp_list[info.count++] = m_param.rank_list[i].rank_end_timestamp;
	}

	int length = sizeof(info) - (ARRAY_ITEM_COUNT(info.rank_end_timestamp_list) - info.count) * sizeof(info.rank_end_timestamp_list[0]);
	if (NULL == role)
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&info, length);
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &info, length);
	}
}

void KuaFuYingXiongLuManager::OnReopen()
{	
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	//1.上锁则无法重开活动 2.配置不开
	if (0 != m_param.is_lock_flag || !LOGIC_CONFIG->GetKuaFuYingXiongLuConfig()->GetOtherCfg().is_can_open_activity)
	{
		return;
	}
	//设置一下标识
	m_param.is_has_ready_open = 1;

	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	if (m_param.activity_begin_timestamp > 0 && m_param.activity_open_cross_day > 0)
	{
		for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.rank_list) && i < KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX; ++i)
		{
			m_param.rank_list[i].rank_end_timestamp = now_second;
			//榜单未开启就提前结束,则不发放榜单奖励
			const KuaFuYingXiongLuTimeCfg * time_cfg = LOGIC_CONFIG->GetKuaFuYingXiongLuConfig()->GetTimeCfg(i);
			if (NULL != time_cfg)
			{
				int open_day = World::GetInstWorld()->GetDayByTimestamp(m_param.activity_begin_timestamp, now_second);
				if (open_day < time_cfg->start_time)
				{
					m_param.rank_list[i].is_give_rand_activity_reward_flag = true;
				}
			}
		}
		m_param.activty_end_timestamp = now_second;
	}
	//检测一下上次活动是否还未结束,如存在还进行则直接结算
	this->OnCheckTime(now_second);
	//检测是否开启新一轮
	this->OnCheckOpenNew(now_second);
}

void KuaFuYingXiongLuManager::OnClose()
{
	if (!CrossConfig::Instance().IsHiddenServer() || !this->IsOpen())
	{
		return;
	}

	unsigned int now_second = (unsigned int)(EngineAdapter::Instance().Time());
	if (m_param.activity_begin_timestamp > 0 && m_param.activity_open_cross_day > 0)
	{
		for (int i = 0; i < ARRAY_ITEM_COUNT(m_param.rank_list) && i < KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX; ++i)
		{
			m_param.rank_list[i].rank_end_timestamp = now_second;
			//榜单未开启就提前结束,则不发放榜单奖励
			const KuaFuYingXiongLuTimeCfg * time_cfg = LOGIC_CONFIG->GetKuaFuYingXiongLuConfig()->GetTimeCfg(i);
			if (NULL != time_cfg)
			{
				int open_day = World::GetInstWorld()->GetDayByTimestamp(m_param.activity_begin_timestamp, now_second);
				if (open_day < time_cfg->start_time)
				{
					m_param.rank_list[i].is_give_rand_activity_reward_flag = true;
				}
			}
		}
		m_param.activty_end_timestamp = now_second;
	}
	this->OnHiddenSynDataToGameworld();
	this->SendAllInfo();
}

void KuaFuYingXiongLuManager::OnSetLockFlag(bool is_lock)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	m_param.is_lock_flag = is_lock ? 1 : 0;
}

int KuaFuYingXiongLuManager::GetPersonRankType(int kfyxlr_rank_type) const
{
	int rank_type = -1;
	UNSTD_STATIC_CHECK(5 == KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX);
	switch (kfyxlr_rank_type)
	{
		case KUA_FU_YING_XIONG_LU_RANK_TYPE_ROLE_CAP:
		{
			rank_type = PERSON_RANK_TYPE_ROLE_CAPABILITY;
		}
		break;
		case KUA_FU_YING_XIONG_LU_RANK_TYPE_EQUIP_CAP:
		{
			rank_type = PERSON_RANK_TYPE_EQUIPMENT_CAP;
		}
		break;
		case KUA_FU_YING_XIONG_LU_RANK_TYPE_PARTNER_CAP:
		{
			rank_type = PERSON_RANK_TYPE_FOUR_MAX_PARTNER;
		}
		break;
		case KUA_FU_YING_XIONG_LU_RANK_TYPE_PET_CAP:
		{
			rank_type = PERSON_RANK_TYPE_PET_MAX_CAPABILITY;
		}
		break;
		case KUA_FU_YING_XIONG_LU_RANK_TYPE_COMPLEX_CAP:
		{
			rank_type = PERSON_RANK_TYPE_CAPABILITY;
		}
		break;
		default:
		break;
	}

	return rank_type;
}

bool KuaFuYingXiongLuManager::IsOpen() const
{
	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	if (m_param.activity_begin_timestamp <= now_second && now_second <= m_param.activty_end_timestamp)
	{
		return true;
	}

	return false;
}

bool KuaFuYingXiongLuManager::IsRankInTime(int rank_type) const
{
	if (rank_type < KUA_FU_YING_XIONG_LU_RANK_TYPE_BEGIN || rank_type >= KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX)
	{
		return false;
	}
	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	if (m_param.rank_list[rank_type].IsInvalid() || now_second < m_param.activity_begin_timestamp || now_second > m_param.activty_end_timestamp)
	{
		return false;
	}

	if (now_second <= m_param.rank_list[rank_type].rank_end_timestamp)
	{
		return true;
	}

	return false;
}

KuaFuYingXiongLuManager::KuaFuYingXiongLuManager()
{
}

KuaFuYingXiongLuManager::~KuaFuYingXiongLuManager()
{
}

void KuaFuYingXiongLuManager::OnCheckTime(unsigned int now_second)
{
	//原服不做该操作
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	for (int rank_type = KUA_FU_YING_XIONG_LU_RANK_TYPE_BEGIN; rank_type < KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX && rank_type < ARRAY_LENGTH(m_param.rank_list); rank_type++)
	{
		if (0 != m_param.rank_list[rank_type].is_give_rand_activity_reward_flag || m_param.rank_list[rank_type].rank_end_timestamp <= 0) continue;
		if ((0 == m_param.is_has_ready_open) && now_second <= m_param.rank_list[rank_type].rank_end_timestamp) continue;
		if (now_second < m_param.rank_list[rank_type].next_check_timestamp) continue;

		this->OnActivityRankEnd(rank_type);
		m_param.rank_list[rank_type].next_check_timestamp = now_second + MAX_KUA_FU_YING_XIONG_LU_RANK_CHECK_INTERVAL_TIME;
	}
}

void KuaFuYingXiongLuManager::OnActivityRankEnd(int rank_type)
{
	//原服不做该操作
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	std::vector<KuaFuYingXiongLuRankCfg> rank_vec;
	LOGIC_CONFIG->GetKuaFuYingXiongLuConfig()->GetRankVec(m_param.activity_open_cross_day, rank_type, rank_vec);
	if (rank_vec.empty()) return;

	//发往跨服查询最新榜单
	crossgameprotocal::HiddenCrossReqKuaFuYingXiongLuRankEnd hcrkfyxlre;
	hcrkfyxlre.kfyxl_rank_type = rank_type;
	hcrkfyxlre.person_rank_type = this->GetPersonRankType(rank_type);
	if (-1 == hcrkfyxlre.person_rank_type)
	{
		return;
	}
	UNSTD_STATIC_CHECK((unsigned int)MAX_KUA_FU_YING_XIONG_LU_RANK_TYPE_SEQ_NUM <= PERSON_RANK_MAX_SIZE);
	hcrkfyxlre.rank_count = GetMin((int)rank_vec.size(), MAX_KUA_FU_YING_XIONG_LU_RANK_TYPE_SEQ_NUM);
	hcrkfyxlre.activity_unique_id = m_param.activity_unique_id;

	InternalComm::Instance().SendToCross((const char *)&hcrkfyxlre, sizeof(hcrkfyxlre));
}

void KuaFuYingXiongLuManager::OnCheckOpenNew(unsigned int now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}
	//查看是否有待开启的状态
	if (0 == m_param.is_has_ready_open)
	{
		return;
	}

	//检测一下榜单奖励是否已全部发放
	if (!this->IsAllSendReward())
	{
		return;
	}
	long long unique_id = 0;
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_id))
	{
		return;
	}


	m_param.ReopenReset();
	m_param.is_has_ready_open = 0;
	m_param.activity_begin_timestamp = (unsigned int)GetZeroTime(EngineAdapter::Instance().Time());
	m_param.activity_open_cross_day = World::GetInstWorld()->GetDayByTimestamp((unsigned int)SHAREDCONFIG->GetOpenServerConfig().GetOpenServerZeroTime(), m_param.activity_begin_timestamp);
	m_param.activity_unique_id = unique_id;

	for (int rank_type = KUA_FU_YING_XIONG_LU_RANK_TYPE_BEGIN; rank_type < KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX && rank_type < ARRAY_LENGTH(m_param.rank_list); rank_type++)
	{
		const KuaFuYingXiongLuTimeCfg * time_cfg = LOGIC_CONFIG->GetKuaFuYingXiongLuConfig()->GetTimeCfg(rank_type);
		if (NULL == time_cfg || time_cfg->start_time <= 0 || time_cfg->continuou_times <= 0)
		{
			continue;
		}

		int last_day = time_cfg->continuou_times + time_cfg->start_time - 1;
		if (last_day <= 0)
		{
			last_day = 1;
		}

		m_param.rank_list[rank_type].rank_end_timestamp = m_param.activity_begin_timestamp + SECOND_PER_DAY * last_day;
		m_param.activty_end_timestamp = GetMax(m_param.rank_list[rank_type].rank_end_timestamp, m_param.activty_end_timestamp);
	}
	this->OnHiddenSynDataToGameworld();
	this->SendAllInfo();
}

bool KuaFuYingXiongLuManager::IsAllSendReward() const
{
	bool is_send_all = true;
	for (int rank_type = KUA_FU_YING_XIONG_LU_RANK_TYPE_BEGIN; rank_type < KUA_FU_YING_XIONG_LU_RANK_TYPE_MAX && rank_type < ARRAY_LENGTH(m_param.rank_list); rank_type++)
	{
		if (m_param.rank_list[rank_type].IsInvalid())
		{
			continue;
		}
		if (0 == m_param.rank_list[rank_type].is_give_rand_activity_reward_flag)
		{
			is_send_all = false;
			break;
		}
	}

	return is_send_all;
}
