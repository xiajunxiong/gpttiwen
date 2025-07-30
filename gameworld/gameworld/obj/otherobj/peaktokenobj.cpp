#include "peaktokenobj.hpp"
#include "engineadapter.h"
#include "gamelog.h"

#include "servercommon/uniqueidgenerator.hpp"
#include "servercommon/string/gameworldstr.h"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/peaktoken/rolepeaktoken.hpp"
#include "other/peaktoken/peaktokenconfig.hpp"
#include "config/logicconfigmanager.hpp"

#include "scene/scene.h"
#include "protocol/msgpeaktoken.hpp"


PeakTokenObj::PeakTokenObj()
	: Obj(OBJ_TYPE_PEAK_TOKEN_OBJ)
{
	m_peak_token_act_id = 0;
	m_reward_group_id = 0;

	m_creat_time = 0;
	m_remove_time = 0;

	m_role_uid = 0;
	memset(m_role_name, 0, sizeof(m_role_name));
	m_role_appearance.Reset();

	m_next_notice_time = 0;
}

PeakTokenObj::~PeakTokenObj()
{
}

void PeakTokenObj::Init(Role * creat_role, int reward_group_id)
{
	const PeakTokenConfig::OtherCfg & other_cfg = LOGIC_CONFIG->GetPeakTokenConfig()->GetOtherCfg();

	UniqueIDGenerator::Instance()->GenUniqueID(&m_peak_token_act_id);
	m_reward_group_id = reward_group_id;

	m_creat_time = EngineAdapter::Instance().Time();
	m_remove_time = m_creat_time + other_cfg.act_duration_s;

	m_role_uid = creat_role->GetUID();
	creat_role->GetName(m_role_name);
	m_role_appearance = creat_role->GetRoleModuleManager()->GetAppearance();

	m_next_notice_time = m_creat_time;
}

void PeakTokenObj::Update(unsigned long interval, time_t now_second, unsigned int now_dayid)
{
	if (m_remove_time < now_second)
	{//自删
		this->GetScene()->DeleteObj(this->GetId());
		return;
	}

	if (m_next_notice_time < now_second)
	{//公告
		this->ActNoticeAll();

		const PeakTokenConfig::OtherCfg & other_cfg = LOGIC_CONFIG->GetPeakTokenConfig()->GetOtherCfg();
		m_next_notice_time += other_cfg.notice_interval_s;
	}

}

void PeakTokenObj::RoleJoin(Role * role)
{
	if (NULL == role)
	{
		return;
	}

	if (0 != this->GetRoleJoinTime(role))
	{//已参与
		this->SendRoleStatus(role);
		return;
	}
	int uid = role->GetUID();
	JoinInfo & join_info = m_jion_info_map[uid];
	join_info.m_role_uid = uid;
	join_info.m_begin_time = EngineAdapter::Instance().Time();

	this->SendRoleStatus(role);

	//log
	gamelog::g_log_role_activity.printf(LL_INFO, "PeakTokenObj::RoleJoin join_user[%d,%s] "
		"creat_uid:[%d] act_id:[%lld].",
		role->GetUID(), role->GetName(),
		m_role_uid, m_peak_token_act_id);
}

void PeakTokenObj::RoleFetch(Role * role)
{
	if (NULL == role)
	{
		return;
	}

	if (0 == this->GetRoleJoinTime(role))
	{//未参与
		return;
	}

	const PeakTokenConfig::OtherCfg & other_cfg = LOGIC_CONFIG->GetPeakTokenConfig()->GetOtherCfg();
	JoinInfo & join_info = m_jion_info_map[role->GetUID()];

	//修改 奖励未领完 可以继续游戏
	//if (EngineAdapter::Instance().Time() > join_info.m_begin_time + other_cfg.game_time_s + 10)
	//{//超时
	//	return;
	//}

	if (other_cfg.role_reward_times <= join_info.fetch_reward_times)
	{//奖励上限
		return;
	}

	//配置
	const PeakTokenConfig::RewardCfg * reward_cfg = 
		LOGIC_CONFIG->GetPeakTokenConfig()->GetGroupRandCfg(m_reward_group_id);
	if (NULL == reward_cfg)
	{
		return;
	}

	//记录
	join_info.fetch_reward_times++;

	//奖励
	if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPut2(reward_cfg->item))
	{
		role->GetRoleModuleManager()->GetKnapsack()->Put(reward_cfg->item, PUT_REASON_PEAK_TOKEN);
	}
	else
	{
		role->GetRoleModuleManager()->GetKnapsack()->SendMail(&reward_cfg->item, 1, SNED_MAIL_TYPE_DEFAULT, true);
	}

	if (1 == reward_cfg->is_broadcast)
	{//传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF,
			sizeof(gamestring::GAMESTRING_BUF),
			gamestring::g_peak_token_reward_notice,
			role->GetName(),
			(int)reward_cfg->item.item_id);

		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
		}
	}

	{//奖励通知
		Protocol::SCPeaktokenRoleJoinInfo info;
		info.peak_token_act_id = m_peak_token_act_id;
		info.role_join_time = (unsigned int)join_info.m_begin_time;
		info.reward_times = join_info.fetch_reward_times;

		info.item_id = reward_cfg->item.item_id;
		info.num = reward_cfg->item.num;

		role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
	}

	//log
	gamelog::g_log_role_activity.printf(LL_INFO, "PeakTokenObj::RoleFetch join_user[%d,%s] "
		"creat_uid:[%d] act_id:[%lld] "
		"fetch_reward_times:[%d]",
		role->GetUID(), role->GetName(),
		m_role_uid, m_peak_token_act_id,
		join_info.fetch_reward_times);
}

time_t PeakTokenObj::GetRoleJoinTime(Role * role)
{
	if (NULL == role)
	{
		return 0;
	}
	JoinInfoMap::iterator it = m_jion_info_map.find(role->GetUID());
	if (it != m_jion_info_map.end())
	{
		return it->second.m_begin_time;
	}

	return 0;
}

int PeakTokenObj::GetRoleFetchTime(Role * role)
{
	if (NULL == role)
	{
		return 0;
	}
	JoinInfoMap::iterator it = m_jion_info_map.find(role->GetUID());
	if (it != m_jion_info_map.end())
	{
		return it->second.fetch_reward_times;
	}

	return 0;
}

void PeakTokenObj::GetCreatName(GameName & name) const
{
	if (NULL != name)
	{
		F_STRNCPY(name, m_role_name, sizeof(GameName));
	}
}

void PeakTokenObj::SendRoleStatus(Role * role)
{
	if (NULL == role)
	{
		return;
	}

	Protocol::SCPeaktokenRoleStatusInfo info;
	info.peak_token_act_id = m_peak_token_act_id;
	info.role_join_time = (unsigned int)this->GetRoleJoinTime(role);

	info.reward_times = 0;
	if (0 != info.role_join_time)
	{
		info.reward_times = m_jion_info_map[role->GetUID()].fetch_reward_times;
	}

	role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
}

void PeakTokenObj::ActNoticeAll()
{

	std::string act_id_str = STRING_SPRINTF("%lld", m_peak_token_act_id);

	int length = SNPRINTF(gamestring::GAMESTRING_BUF,
		sizeof(gamestring::GAMESTRING_BUF),
		gamestring::g_peak_token_notice,
		m_role_name,
		this->GetScene()->GetSceneID(),
		this->GetScene()->GetSceneID(),
		this->GetPos().x, this->GetPos().y,
		(int)m_remove_time,
		act_id_str.c_str());

	if (length > 0)
	{
		World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
	}
}

PeakTokenObj::JoinInfo::JoinInfo()
{
	m_role_uid = 0;
	m_begin_time = 0;
	fetch_reward_times = 0;
}

PeakTokenObj::JoinInfo::~JoinInfo()
{
}
