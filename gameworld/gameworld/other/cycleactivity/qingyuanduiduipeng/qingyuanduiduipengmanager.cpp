#include "qingyuanduiduipengmanager.hpp"
#include "qingyuanduiduipengconfig.hpp"
#include "servercommon/maildef.hpp"
#include "other/route/mailroute.hpp"
#include "config/logicconfigmanager.hpp"
#include "servercommon/string/gameworldstr.h"
#include "global/team/team.hpp"
#include "gamelog.h"
#include "other/rolemodulemanager.hpp"
#include "engineadapter.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "protocol/cycleactivity/msgqingyuanduiduipeng.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "internalcomm.h"
#include "servercommon/errornum.h"
#include "global/zhouqimanager/zhouqimanager.h"
#include "servercommon/zhouqidef.hpp"
#include "other/title/title.hpp"
#include "other/title/titleconfig.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "other/funopen/funopen.hpp"

QingYuanDuiDuiPengManager::QingYuanDuiDuiPengManager()
{
}
QingYuanDuiDuiPengManager::~QingYuanDuiDuiPengManager()
{

}

QingYuanDuiDuiPengManager & QingYuanDuiDuiPengManager::Instance()
{
	static QingYuanDuiDuiPengManager qy_ddp_manger;
	return qy_ddp_manger;
}

void QingYuanDuiDuiPengManager::Init(const QingYuanDuiDuiPengRankParam & param)
{
	m_param = param;
}

void QingYuanDuiDuiPengManager::GetInitParam(QingYuanDuiDuiPengRankParam * param)
{
	*param = m_param;
}

void QingYuanDuiDuiPengManager::OnRoleUpDate(int uid, GameName name, short jifen, short pass_times)
{
	int updata_rank = -1;
	for (int i = 0; i < m_param.rank_count && i < ARRAY_LENGTH(m_param.rank_list); i++)
	{
		if (m_param.rank_list[i].uid == uid)				// 先看排行榜里有没有自己
		{
			if (jifen <= m_param.rank_list[i].jifen)
				return;

			m_param.rank_list[i].jifen = jifen;
			m_param.rank_list[i].pass_times = pass_times;
			updata_rank = i;
			break;
		}
	}

	if (updata_rank == -1)
	{
		QingYuanDuiDuiPengRankData new_data;
		new_data.uid = uid;
		memcpy(new_data.name, name, sizeof(GameName));
		new_data.jifen = jifen;
		new_data.pass_times = pass_times;

		if (m_param.rank_count < ARRAY_LENGTH(m_param.rank_list))		// 排行榜没满 直接往后面塞
		{
			memcpy(&m_param.rank_list[m_param.rank_count], &new_data, sizeof(QingYuanDuiDuiPengRankData));
			updata_rank = m_param.rank_count;
			m_param.rank_count++;
		}
		else
		{
			if (m_param.rank_list[m_param.rank_count - 1].jifen > jifen)	// 积分小于最后一名 直接PASS
				return;
			// 积分相同 但是通关时间比最后一名长也PASS
			if (m_param.rank_list[m_param.rank_count - 1].jifen == jifen && m_param.rank_list[m_param.rank_count].pass_times <= pass_times)
				return;
			
			memcpy(&m_param.rank_list[m_param.rank_count - 1], &new_data, sizeof(QingYuanDuiDuiPengRankData));
			updata_rank = m_param.rank_count - 1;
		}
	}

	if (updata_rank == 0) return;		// 都第一名了就不用比较了

	for (int i = updata_rank; updata_rank > 0; updata_rank--)
	{
		QingYuanDuiDuiPengRankData & front_data = m_param.rank_list[updata_rank - 1];
		QingYuanDuiDuiPengRankData & data = m_param.rank_list[updata_rank];

		if(front_data.jifen > data.jifen)
			break;

		if(front_data.jifen == data.jifen && front_data.pass_times <= data.pass_times)
			break;

		QingYuanDuiDuiPengRankData tem_data;
		memcpy(&tem_data, &front_data, sizeof(QingYuanDuiDuiPengRankData));
		memcpy(&front_data, &data, sizeof(QingYuanDuiDuiPengRankData));
		memcpy(&data, &tem_data, sizeof(QingYuanDuiDuiPengRankData));
	}
}

void QingYuanDuiDuiPengManager::OnDayChange()
{
	if (m_param.rank_count > 0)
	{
		this->OnReward();
	}

	m_param.Reset();
}

bool QingYuanDuiDuiPengManager::CanEnterFB(Role * role)
{
	if (NULL == role) return false;

	if (!ZhouQiManager::Instance().IsInZhouQi(ZHOU_QI_PLAY_TYPE_QING_YUAN_DUI_DUI_PENG) || !ZhouQiManager::Instance().IsDayGameOpen(ZHOU_QI_PLAY_TYPE_QING_YUAN_DUI_DUI_PENG))
	{
		role->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return false; 
	}

	if (!FunOpen::IsFunOpen(role, FUN_TYPE_ZHOU_QI))return false;

	Team * team = role->GetMyTeam();
	if (!team || team->GetMemberCount() != 2)
	{
		return false;
	}

	int rober = team->GetRobotMemberNum();
	if (rober > 0)
	{
		return false;
	}

	bool is_can_enterfb = false;
	for (int i = 0; i < team->GetMemberCount(); ++i)
	{
		Role* member = team->GetMemberRoleByIndex(i);
		if (NULL == member) continue;
		
		// 只要有一个人有次数就可以进本
		if (member->GetRoleModuleManager()->GetCommonData().qingyuan_duiduipeng_times < LOGIC_CONFIG->GetQingYuanDuiDuiPengConfig()->GetOtherCfg().reward_times)
		{
			is_can_enterfb = true;
			break;
		}
	}

	if (!is_can_enterfb)
	{
		role->NoticeNum(errornum::EN_TEAM_MEMBER_LIMIT_PLAY_TIMES);
	}

	return is_can_enterfb;
}

void QingYuanDuiDuiPengManager::SendRoleInfo(Role * role)
{
	if (NULL == role) return;

	Protocol::SCQingYuanDuiDuiDayTimesInfo info;
	info.day_count = role->GetRoleModuleManager()->GetCommonData().qingyuan_duiduipeng_times;

	role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
}

void QingYuanDuiDuiPengManager::OnReqRank(Role * role)
{
	if (NULL == role) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->SendInfo(role);
	}
	else
	{
		static crossgameprotocal::GameHiddenQingYuanDuiDuiPengRankReq info;
		info.role_id = role->GetUID();

		InternalComm::Instance().SendToHiddenThroughCross((const char *)&info, sizeof(info));
	}
}

void QingYuanDuiDuiPengManager::SendInfo(Role * role)
{
	if (NULL == role)
		return;

	Protocol::SCQingYuanDuiDuiRankInfo info;
	info.count = m_param.rank_count;
	UNSTD_STATIC_CHECK(sizeof(info.rank_list) == sizeof(m_param.rank_list));

	for (int i = 0; i < info.count && i < ARRAY_LENGTH(m_param.rank_list); i++)
	{
		info.rank_list[i].uid = m_param.rank_list[i].uid;
		memcpy(info.rank_list[i].name, m_param.rank_list[i].name, sizeof(GameName));
		info.rank_list[i].jifen = m_param.rank_list[i].jifen;
		info.rank_list[i].pass_times = m_param.rank_list[i].pass_times;
	}

	int len = sizeof(info) - sizeof(info.rank_list) + info.count * sizeof(Protocol::SCQingYuanDuiDuiRankInfo::RoleData);
	role->GetRoleModuleManager()->NetSend(&info, len);
}

void QingYuanDuiDuiPengManager::SendToGameRankInfo(int role_id)
{
	crossgameprotocal::HiddenGameQingYuanDuiDuiPengUpRankInfo info;
	info.role_id = role_id;
	info.count = m_param.rank_count;

	for (int i = 0; i < info.count && i < ARRAY_LENGTH(m_param.rank_list); i++)
	{
		info.rank_list[i].uid = m_param.rank_list[i].uid;
		memcpy(info.rank_list[i].name, m_param.rank_list[i].name, sizeof(GameName));
		info.rank_list[i].jifen = m_param.rank_list[i].jifen;
		info.rank_list[i].pass_times = m_param.rank_list[i].pass_times;
	}

	int len = sizeof(info) - sizeof(info.rank_list) + info.count * sizeof(crossgameprotocal::HiddenGameQingYuanDuiDuiPengUpRankInfo::RoleData);

	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(role_id).db_index, (const char *)&info, len);
}

void QingYuanDuiDuiPengManager::SendTitleToRole(int title_id, int uid, const char * reason)
{
	const TitleCfg * title_cfg = LOGIC_CONFIG->GetTitleConfig()->GetTitleCfg(title_id);
	if (NULL == title_cfg) return;

	//先在该服查看玩家是否在线
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
	if (NULL != role)
	{
		role->GetRoleModuleManager()->GetTitle()->AddTitle(title_id, reason);
		return;
	}
	//玩家不在线此时查看是否在隐藏服
	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameRoleAddTitle msg;
		msg.title_id = title_id;
		msg.uid = uid;
		F_STRNCPY(msg.reason, reason, sizeof(msg.reason));

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(uid).db_index, (const char*)&msg, sizeof(msg));
		return;
	}
	//玩家处于离线状态
	OfflineRegisterManager::Instance().OnTitleSendReq(IntToUid(uid), title_id);
}

void QingYuanDuiDuiPengManager::OnReward()
{
	int rank_index = 0;
	for (int i = 0; i < m_param.rank_count && i < ARRAY_LENGTH(m_param.rank_list); i++)
	{
		if(m_param.rank_list[i].uid == 0) continue;

		if (i != 0 && (m_param.rank_list[i].jifen != m_param.rank_list[i - 1].jifen || m_param.rank_list[i].pass_times != m_param.rank_list[i - 1].pass_times))
			rank_index += 1;

		const QingYuanDuiDuiPengRankCfg * cfg = LOGIC_CONFIG->GetQingYuanDuiDuiPengConfig()->GetRankCfg(rank_index);
		if(NULL == cfg)
			continue;

		if (cfg->title_id > 0)
		{
			this->SendTitleToRole(cfg->title_id, m_param.rank_list[i].uid, __FUNCTION__);
		}

		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, cfg->reward);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_qingyuan_duiduipeng_rank_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_qingyuan_duiduipeng_rank_content, rank_index + 1);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(IntToUid(m_param.rank_list[i].uid), MAIL_REASON_DEFAULT, contentparam);

			//log
			gamelog::g_log_cycleactivity.printf(LL_INFO, "%s user[%d, %s], rank_index[%d]", __FUNCTION__,
				m_param.rank_list[i].uid, m_param.rank_list[i].name, rank_index + 1);
		}
	}
}
