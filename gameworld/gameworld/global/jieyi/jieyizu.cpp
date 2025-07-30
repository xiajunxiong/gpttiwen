#include "world.h"
#include "obj/character/role.h"
#include "global/usercache/usercache.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "internalcomm.h"
#include "friend/friendintimacy.hpp"
#include "friend/friendintimacy.hpp"
#include "config/logicconfigmanager.hpp"
#include "item/knapsack.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"

#include "global/jieyi/jieyizuconfig.h"
#include "servercommon/errornum.h"
#include "global/cross/crossmanager.h"
#include "global/commonsave/commonsavemanager.hpp"
#include "protocol/msgjieyi.h"
#include "global/jieyi/jieyizumanager.h"
#include "jieyizu.h"
#include "other/event/eventhandler.hpp"
#include "gamelog.h"

JieYiZu::JieYiZu(long long unique_id)
{
	m_unique_id = unique_id;
	m_common_save = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_JIE_YI_ZU);
	assert(m_common_save != NULL);
	m_data = m_common_save->GetTypeData<CommonSaveJieYiZu>(m_unique_id, true);	
	assert(m_data != NULL);
}

JieYiZu::~JieYiZu()
{
}

void JieYiZu::OnUserLogin(Role * role)
{	
	if (!role)
	{
		return;
	}

	int role_id = role->GetUID();
	this->ModifyMemberInfo(role_id);
	this->NoticeOnlineStatus(role_id, CrossConfig::Instance().IsHiddenServer() ? UserStatusNode::STATUS_TYPE_CROSS : UserStatusNode::STATUS_TYPE_ONLINE);
	this->SendInfo(role);
	this->CheckBreakTanHe(role_id);

	CommonSaveJieYiZuRoleInfoItem * member_data = this->GetZuMemberData(role_id);
	if (NULL != member_data)
	{
		unsigned int create_timestamp = this->GetZuCreateTime();
		EventHandler::Instance().OnJieYiRoleChnage(role, this->GetJoinJieYiDay(create_timestamp, member_data->join_time));
	}
}

void JieYiZu::OnUserLoginOriginGame(int role_id)
{
	this->ModifyMemberInfo(role_id);
	this->NoticeOnlineStatus(role_id, UserStatusNode::STATUS_TYPE_ONLINE);
	this->CheckBreakTanHe(role_id);
}

void JieYiZu::OnUserLogout(Role * role)
{
	int role_id = role->GetUID();
	this->ModifyMemberInfo(role_id);
	this->NoticeOnlineStatus(role_id, UserStatusNode::STATUS_TYPE_OFFLINE);
}

void JieYiZu::OnDayChange(Role * role)
{
	CommonSaveJieYiZuRoleInfoItem * member_data = this->GetZuMemberData(role->GetUID());
	if (NULL != member_data)
	{
		unsigned int create_timestamp = this->GetZuCreateTime();
		EventHandler::Instance().OnJieYiRoleChnage(role, this->GetJoinJieYiDay(create_timestamp, member_data->join_time));
	}
}

void JieYiZu::ModifyMemberInfo(int role_id)
{
	UserCacheNode * node = UserCacheManager::Instance().GetUserNode(role_id);
	if (node == NULL)
	{
		return;
	}

	CommonSaveJieYiZuRoleInfoItem * member_data = this->GetZuMemberData(role_id);
	if (member_data == NULL)
	{
		return;
	}

	node->GetName(member_data->role_name);
	member_data->level = node->level;
	member_data->avatar_type = node->avatar_type;
	member_data->prof = node->GetProfession();
	member_data->headshot_id = node->appearance.headshot_id;
	member_data->last_login_time = (unsigned int)EngineAdapter::Instance().Time();
	this->SyncMemberDataHiddenToGame(role_id);
}

void JieYiZu::NoticeOnlineStatus(int role_id, int online_status)
{
	CommonSaveJieYiZuRoleInfoItem * member_data = this->GetZuMemberData(role_id);
	if (member_data == NULL)
	{
		return;
	}

	Protocol::SCJieYiZuRoleOnlineStatusNotice protocol;
	protocol.role_id = role_id;
	protocol.last_login_time = member_data->last_login_time;
	protocol.online_status = online_status;
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->SendMsgAllMemberAny(&protocol, sizeof(protocol));
	}	
	else
	{
		this->SendMsgAllMember(&protocol, sizeof(protocol));
	}
}

void JieYiZu::Update(unsigned long interval, time_t now_second)
{
	this->UpdateLeave(interval, now_second);
	this->UpdateTanHe(interval, now_second);
}

void JieYiZu::UpdateLeave(unsigned long interval, time_t now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	CommonSaveJieYiZu * zu_info_ptr = this->GetCommonSaveJieYiZu(false);
	if (zu_info_ptr)
	{
		bool need_send = false;

		for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
		{
			CommonSaveJieYiZuRoleInfoItem & curr = zu_info_ptr->role_info[i];
			if (curr.role_id == 0)
			{
				continue;
			}

			if (curr.leave_end_time != 0 && now_second > curr.leave_end_time)
			{
				gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d | jie_yi_zu[%lld] role_id[%d]", __FUNCTION__, __LINE__, this->GetJieYiZuId(), curr.role_id);

				static MailContentParam contentparam; contentparam.Reset();
				SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_jie_yi_leave_subject);
				SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_jie_yi_leave_content, curr.role_id, curr.role_name);
				this->SendMailAllMember(MAIL_REASON_DEFAULT, contentparam, curr.role_id);

				this->OnRoleLeave(curr.role_id, __FUNCTION__);
				curr.Reset();
				need_send = true;
			}
		}

		if (need_send)
		{
			this->AutoSetZuZhang();
			this->SendInfo();
			this->SyncDataHiddenToGame();
		}
	}
}

struct JieYiTanHeVotesNum
{
	bool operator<(const JieYiTanHeVotesNum & other)const
	{
		if (vote_num > other.vote_num)
			return true;
		if (vote_num < other.vote_num)
			return false;

		if (role_id < other.role_id)
			return true;
		if (role_id > other.role_id)
			return false;

		return false;
	}

	int role_id;
	int vote_num;
};

void JieYiZu::UpdateTanHe(unsigned long interval, time_t now_second)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	CommonSaveJieYiZu * zu_info_ptr = this->GetCommonSaveJieYiZu(false);
	if (!zu_info_ptr)
	{
		return;
	}

	if (zu_info_ptr->tan_he_vote_end_time == 0 || now_second < zu_info_ptr->tan_he_vote_end_time)
	{
		return;
	}

	zu_info_ptr->tan_he_vote_end_time = 0;

	std::vector<JieYiTanHeVotesNum> role_votes_num_vec;
	for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
	{
		CommonSaveJieYiZuRoleInfoItem & curr = zu_info_ptr->role_info[i];
		if (curr.role_id == 0 || curr.is_zu_zhang)
		{
			continue;
		}
		JieYiTanHeVotesNum tmp;
		tmp.role_id = curr.role_id;
		tmp.vote_num = curr.tan_he_number_of_votes;
		role_votes_num_vec.push_back(tmp);

		curr.is_vote = 0;
		curr.tan_he_number_of_votes = 0;
	}

	if (role_votes_num_vec.empty())
	{
		return;
	}

	std::sort(role_votes_num_vec.begin(), role_votes_num_vec.end());
	int role_id = role_votes_num_vec[0].role_id;
	int old_zu_zhang_role_id=this->GetZuZhangRoleId();

	CommonSaveJieYiZuRoleInfoItem * role_info = this->GetZuMemberData(role_id);
	CommonSaveJieYiZuRoleInfoItem * old_zu_zhang_info = this->GetZuMemberData(old_zu_zhang_role_id);

	if (!role_info || !old_zu_zhang_info)
	{
		return;
	}

	role_info->is_zu_zhang = 1;
	old_zu_zhang_info->is_zu_zhang = 0;

	static MailContentParam contentparam; contentparam.Reset();
	SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_jie_yi_tan_he_subject);
	SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_jie_yi_tan_he_content, role_info->role_id, role_info->role_name);
	this->SendMailAllMember(MAIL_REASON_DEFAULT, contentparam);

	this->SendInfo();
	this->SyncDataHiddenToGame();
}

void JieYiZu::JieYiZuMemberName(Role * role, const JieYIZuMemberName & name)
{
	if (!role)
	{
		return;
	}

	if (!CrossManager::Instance().IsHiddenServerAlive())
	{
		role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
		return;
	}

	int role_id = role->GetUID();
	CommonSaveJieYiZuRoleInfoItem * role_info = this->GetZuMemberData(role_id);
	if (NULL != role_info && role_info->has_fisrt_member_name)
	{
		const JieYiOtherCfg & ohter_cfg = LOGIC_CONFIG->GetJieYiConfig()->GetOtherCfg();
		if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(ohter_cfg.jie_yi_member_name_consume_item_id, ohter_cfg.jie_yi_member_name_consume_num, __FUNCTION__))
		{
			return;
		}
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenJieYiZuMemberName protocol;
		protocol.jie_yi_zu_id = this->GetJieYiZuId();
		protocol.role_id = role_id;
		SameArrayTypeCopy(protocol.member_name, name);
		InternalComm::Instance().SendToHiddenThroughCross((char *)&protocol, sizeof(protocol));
		return;
	}

	this->JieYiZuMemberName(role_id, name);
}

void JieYiZu::JieYiZuName(Role * role, const JieYIZuName name)
{
	if (!role)
	{
		return;
	}

	if (!CrossManager::Instance().IsHiddenServerAlive())
	{
		role->NoticeNum(errornum::EN_CROSS_NOT_CONNECTED);
		return;
	}

	int role_id = role->GetUID();

	if (!this->IsZuZhang(role_id))
	{
		return;
	}

	const JieYiOtherCfg & ohter_cfg = LOGIC_CONFIG->GetJieYiConfig()->GetOtherCfg();
	if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(ohter_cfg.jie_yi_name_consume_item_id, ohter_cfg.jie_yi_name_consume_num, __FUNCTION__))
	{
		return;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenJieYiZuName protocol;
		protocol.jie_yi_zu_id = this->GetJieYiZuId();
		protocol.role_id = role_id;
		protocol.ji_yi_zu_name = name;
		InternalComm::Instance().SendToHiddenThroughCross((char *)&protocol, sizeof(protocol));
		return;
	}

	this->JieYiZuNameHelper(name);
}

void JieYiZu::JieYiZuMemberName(int role_id, const JieYIZuMemberName & name)
{
	CommonSaveJieYiZuRoleInfoItem * member_data = this->GetZuMemberData(role_id);
	if (!member_data)
	{
		return;
	}

	member_data->has_fisrt_member_name = 1;
	SameArrayTypeCopy(member_data->member_name, name);
	this->SendInfo();
	this->SyncDataHiddenToGame();
}

void JieYiZu::JieYiZuNameHelper(const JieYIZuName name)
{
	CommonSaveJieYiZu * jie_yi_zu_data = this->GetCommonSaveJieYiZu(false);
	if (!jie_yi_zu_data)
	{
		return;
	}

	jie_yi_zu_data->jie_yi_name = name;
	this->SendInfo();
	this->SyncDataHiddenToGame();
}

void JieYiZu::JieYiZuText(int role_id, const JieYIZuText & text)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenJieYiZuText protocol;
		protocol.jie_yi_zu_id = this->GetJieYiZuId();
		protocol.role_id = role_id;
		SameArrayTypeCopy(protocol.text, text);
		InternalComm::Instance().SendToHiddenThroughCross((char *)&protocol, sizeof(protocol));
		return;
	}

	if (!this->IsZuZhang(role_id))
	{
		return;
	}

	CommonSaveJieYiZu * zu_info_ptr = this->GetCommonSaveJieYiZu();
	if (!zu_info_ptr)
	{
		return;
	}

	SameArrayTypeCopy(zu_info_ptr->jie_yi_text, text);
	this->SendInfo();
	this->SyncDataHiddenToGame();
}

void JieYiZu::JieYiZuLeave(int role_id, bool is_cancel)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenJieYiZuLeave protocol;
		protocol.jie_yi_zu_id = this->GetJieYiZuId();
		protocol.role_id = role_id;
		protocol.is_cancel = is_cancel;
		InternalComm::Instance().SendToHiddenThroughCross((char *)&protocol, sizeof(protocol));
		return;
	}
	
	CommonSaveJieYiZuRoleInfoItem * member_data = this->GetZuMemberData(role_id);
	if (member_data == NULL)
	{
		return;
	}
	const JieYiOtherCfg & ohter_cfg = LOGIC_CONFIG->GetJieYiConfig()->GetOtherCfg();
	member_data->leave_end_time = (unsigned int)EngineAdapter::Instance().Time() + ohter_cfg.jie_yi_zu_leave_wait_time;
	if (is_cancel)
	{
		member_data->leave_end_time = 0;
	}
	
	gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d | jie_yi_zu[%lld] role_id[%d] is_cancel:%d time:%ud", __FUNCTION__, __LINE__, this->GetJieYiZuId(), role_id, (int)is_cancel, member_data->leave_end_time);

	this->SendInfo();
	this->SyncDataHiddenToGame();
	this->OnHiddenGameRoleChange(member_data);
}

int JieYiZu::JieYiZuKick(int role_id, int target_role_id)
{
	if (role_id == target_role_id)
	{
		return -__LINE__;
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenJieYiZuKick protocol;
		protocol.jie_yi_zu_id = this->GetJieYiZuId();
		protocol.role_id = role_id;
		protocol.target_role_id = target_role_id;
		InternalComm::Instance().SendToHiddenThroughCross((char *)&protocol, sizeof(protocol));
		return -__LINE__;
	}

	if (!this->IsZuZhang(role_id))
	{
		return -__LINE__;
	}

	if (UserCacheManager::Instance().IsUserOnline(target_role_id))
	{
		World::GetInstWorld()->NoticeNum(role_id, errornum::EN_TARGET_JIE_YI_ONLINE);
		return -__LINE__;
	}

	time_t now = EngineAdapter::Instance().Time();
	UserCacheNode * node = UserCacheManager::Instance().GetUserNode(target_role_id);
	if (node == NULL)
	{		
		World::GetInstWorld()->NoticeNum(role_id, errornum::EN_GET_ROLE_INFO_FAIL);
		return -__LINE__;
	}

	if (now < node->last_online_timestamp + JIE_YI_ZU_KICK_LOGOUT_TIME)
	{
		World::GetInstWorld()->NoticeNum(role_id, errornum::EN_TARGET_JIE_YI_OFFLINE_SHORT);
		return -__LINE__;
	}

	CommonSaveJieYiZuRoleInfoItem * member_data = this->GetZuMemberData(target_role_id);
	if (member_data == NULL)
	{
		return -__LINE__;
	}

	static MailContentParam contentparam; contentparam.Reset();
	SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_jie_yi_kick_subject);
	SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_jie_yi_kick_content, member_data->role_id, member_data->role_name);
	MailRoute::MailToUser(member_data->role_id, MAIL_REASON_DEFAULT, contentparam);
	this->OnRoleLeave(member_data->role_id, __FUNCTION__);

	member_data->Reset();
	this->SendInfo();
	this->SyncDataHiddenToGame();
	this->OnHiddenGameRoleChange(member_data);

	gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d | jie_yi_zu[%lld] role_id[%d] kick  target_role_id[%d]", __FUNCTION__, __LINE__, this->GetJieYiZuId(), role_id, target_role_id);

	return 0;
}

void JieYiZu::JieYiZuInvite(int role_id, int target_role_id)
{
	if (JieYiZuManager::Instance().HasJieYiZu(target_role_id))
	{
		World::GetInstWorld()->NoticeNum(role_id, errornum::EN_TARGET_HAS_JIE_YIE_ZU);
		return;
	}

	if (!this->IsZuZhang(role_id))
	{
		return;
	}

	const JieYiOtherCfg & ohter_cfg = LOGIC_CONFIG->GetJieYiConfig()->GetOtherCfg();
	int friend_intimacy = FriendIntimacyManager::Instance().GetFriendIntimacy(role_id, target_role_id);
	if (friend_intimacy < ohter_cfg.intimacy_request)
	{
		return;
	}

	Protocol::SCJieYiInviteRoute protocol;
	protocol.jie_yi_zu_id = this->GetJieYiZuId();
	protocol.role_id = role_id;
	World::GetInstWorld()->SendToRoleAny(target_role_id, &protocol, sizeof(protocol));
}

int JieYiZu::JieYiZuInviteReply(int role_id, bool is_agree)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenJieYiZuInviteReply protocol;
		protocol.jie_yi_zu_id = this->GetJieYiZuId();
		protocol.role_id = role_id;
		protocol.is_agree = is_agree;
		InternalComm::Instance().SendToHiddenThroughCross((char *)&protocol, sizeof(protocol));
		return -__LINE__;
	}

	UserCacheNode * node = UserCacheManager::Instance().GetUserNode(role_id);
	if (node == NULL)
	{
		return -__LINE__;
	}

	if (!is_agree)
	{
		return -__LINE__;
	}

	if (JieYiZuManager::Instance().HasJieYiZu(role_id, true))
	{
		return -__LINE__;
	}

	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
	CommonSaveJieYiZuRoleInfoItem role_info;
	role_info.Reset();
	role_info.role_id = node->uid;
	SameArrayTypeCopy(role_info.role_name, node->role_name);
	role_info.level = node->level;
	role_info.avatar_type = node->avatar_type;
	role_info.prof = node->profession;
	role_info.headshot_id = node->appearance.headshot_id;
	role_info.join_time = now;
	role_info.last_login_time = now;
	this->AddJieYiMember(role_info);

	this->SendInfo();
	this->SyncDataHiddenToGame();

	return 0;
}

int JieYiZu::TanHe(int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenJieYiZuTanHe protocol;
		protocol.jie_yi_zu_id = this->GetJieYiZuId();
		protocol.role_id = role_id;
		InternalComm::Instance().SendToHiddenThroughCross((char *)&protocol, sizeof(protocol));
		ret_line;
	}

	int target_role_id = this->GetZuZhangRoleId();

	if (UserCacheManager::Instance().IsUserOnline(target_role_id))
	{
		World::GetInstWorld()->NoticeNum(role_id, errornum::EN_JIE_YI_TARGET_OFFLINE_TIME_NOT_ENOUTH);
		ret_line;
	}

	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
	UserCacheNode * node = UserCacheManager::Instance().GetUserNode(target_role_id);
	if (node == NULL)
	{
		World::GetInstWorld()->NoticeNum(role_id, errornum::EN_GET_ROLE_INFO_FAIL);
		ret_line;
	}

	const JieYiOtherCfg & ohter_cfg = LOGIC_CONFIG->GetJieYiConfig()->GetOtherCfg();
	if (now < node->last_online_timestamp + ohter_cfg.jie_yi_tan_he_offline_time)
	{
		World::GetInstWorld()->NoticeNum(role_id, errornum::EN_JIE_YI_TARGET_OFFLINE_TIME_NOT_ENOUTH);
		ret_line;
	}

	CommonSaveJieYiZu * zu_param = this->GetCommonSaveJieYiZu();
	if (!zu_param)
	{
		ret_line;
	}

	if (zu_param->tan_he_vote_end_time != 0)
	{
		ret_line;
	}

	CommonSaveJieYiZuRoleInfoItem * member_data = this->GetZuMemberData(target_role_id);
	if (member_data == NULL)
	{
		ret_line;
	}

	zu_param->tan_he_vote_end_time = now + ohter_cfg.jie_yi_tan_he_vote_time;

	this->SendInfo();
	this->SyncDataHiddenToGame();

	gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d | jie_yi_zu[%lld] role_id[%d] tan he target_role_id[%d]", __FUNCTION__, __LINE__, this->GetJieYiZuId(), role_id, target_role_id);

	return 0;
}

int JieYiZu::TanHeVote(int role_id, int target_role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::GameHiddenJieYiZuTanHeVote protocol;
		protocol.jie_yi_zu_id = this->GetJieYiZuId();
		protocol.role_id = role_id;
		protocol.target_uid = target_role_id;
		InternalComm::Instance().SendToHiddenThroughCross((char *)&protocol, sizeof(protocol));
		ret_line;
	}

	CommonSaveJieYiZu * zu_param = this->GetCommonSaveJieYiZu();
	if (!zu_param)
	{
		ret_line;
	}

	if (zu_param->tan_he_vote_end_time == 0)
	{
		ret_line;
	}

	CommonSaveJieYiZuRoleInfoItem * info = this->GetZuMemberData(role_id);
	if (!info)
	{
		ret_line;
	}


	if (info->is_vote)
	{
		ret_line;
	}

	CommonSaveJieYiZuRoleInfoItem * target_info = this->GetZuMemberData(target_role_id);
	if (!target_info)
	{
		ret_line;
	}

	if (target_info->is_zu_zhang)
	{
		ret_line;
	}

	info->is_vote = 1;
	target_info->tan_he_number_of_votes += 1;

	this->SendInfo();
	this->SyncDataHiddenToGame();

	return 0;
}

int JieYiZu::CheckBreakTanHe(int login_role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		ret_line;
	}

	if (!this->IsZuZhang(login_role_id))
	{
		ret_line;
	}

	CommonSaveJieYiZu * zu_param = this->GetCommonSaveJieYiZu();
	if (!zu_param)
	{
		ret_line;
	}

	if (zu_param->tan_he_vote_end_time == 0)
	{
		ret_line;
	}

	zu_param->tan_he_vote_end_time = 0;
	for (int i = 0; i < ARRAY_LENGTH(zu_param->role_info); ++i)
	{
		CommonSaveJieYiZuRoleInfoItem & curr = zu_param->role_info[i];
		curr.is_vote = 0;
		curr.tan_he_number_of_votes = 0;
	}

	this->SendInfo();
	this->SyncDataHiddenToGame();

	static MailContentParam contentparam; contentparam.Reset();
	SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_jie_yi_tan_he_subject);
	SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_jie_yi_break_tan_he_content);
	this->SendMailAllMember(MAIL_REASON_DEFAULT, contentparam);

	return 0;
}

void JieYIZuDataConv(Protocol::SCJieYiZuInfo::RoleInfo & dst, const CommonSaveJieYiZuRoleInfoItem & src)
{
	dst.role_base_info.uid = src.role_id;
	SameArrayTypeCopy(dst.role_base_info.name, src.role_name);
	dst.role_base_info.role_level = src.level;
	dst.role_base_info.prof = src.prof;
	dst.role_base_info.avatar_type = src.avatar_type;
	dst.role_base_info.headshot_id = src.headshot_id;
	dst.is_zu_zhang = src.is_zu_zhang;			//是否是组长
	dst.has_fisrt_member_name = src.has_fisrt_member_name;
	dst.join_time = src.join_time;			//加入时间
	dst.leave_end_time = src.leave_end_time;		//到时间就离开， 申请离开时间, ②点击割袍断义，弹出确认弹窗，若玩家点击确认则进入24小时冷静期
	SameArrayTypeCopy(dst.member_name, src.member_name);
	dst.last_login_time = src.last_login_time;
	dst.online_state = UserCacheManager::Instance().GetUserOnlineStatus(src.role_id);
	dst.is_vote = src.is_vote;
	dst.tan_he_number_of_votes = src.tan_he_number_of_votes;
	
	UserCacheNode * node = UserCacheManager::Instance().GetUserNode(src.role_id);
	if (node != NULL)
	{
		dst.role_base_info.role_level = node->level;
		node->GetName(dst.role_base_info.name);
		dst.role_base_info.prof = node->profession;
		dst.role_base_info.avatar_type = node->avatar_type;
		dst.role_base_info.headshot_id = node->appearance.headshot_id;
	}
}

void JieYiZu::SendInfo(Role * role,int info_type)
{
	Protocol::SCJieYiZuInfo protocol;
	CommonSaveJieYiZu * zu_info_ptr = this->GetCommonSaveJieYiZu(false);
	if (!zu_info_ptr)
	{
		return;
	}

	protocol.info_type = info_type;
	protocol.jie_yi_zu_id = zu_info_ptr->jie_yi_zu_id;
	protocol.create_time = zu_info_ptr->create_time;;
	protocol.jie_yi_name = this->GetJieYiZuName();
	SameArrayTypeCopy(protocol.jie_yi_text, zu_info_ptr->jie_yi_text);
	protocol.vote_end_time = zu_info_ptr->tan_he_vote_end_time;

	protocol.role_info_count = 0;
	for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info) && protocol.role_info_count < ARRAY_LENGTH(protocol.role_info); ++i)
	{
		const CommonSaveJieYiZuRoleInfoItem & src = zu_info_ptr->role_info[i];
		Protocol::SCJieYiZuInfo::RoleInfo & dst = protocol.role_info[protocol.role_info_count];
		if (src.role_id == 0)
		{
			continue;
		}
		JieYIZuDataConv(dst, src);
		protocol.role_info_count += 1;
	}

	int send_len = sizeof(protocol) - (ARRAY_LENGTH(protocol.role_info) - protocol.role_info_count) * sizeof(protocol.role_info[0]);
	if (role == NULL)
	{
		this->SendMsgAllMember(&protocol, send_len);
	}
	else
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, send_len);
	}

}

void JieYiZu::SendDismissInfo(const std::set<int> & leave_role_id)
{
	Protocol::SCJieYiZuLeaveNotice notice;
	notice.jie_yi_zu_id = this->GetJieYiZuId();
	for (std::set<int> ::const_iterator iter = leave_role_id.begin(); iter != leave_role_id.end(); ++iter)
	{
		World::GetInstWorld()->SendToRole(*iter, &notice, sizeof(notice));
	}
}

void JieYiZu::SendMsgAllMember(const void * msg, unsigned int send_len)
{
	const CommonSaveJieYiZu * zu_info_ptr = this->GetCommonSaveJieYiZu(false);
	if (!zu_info_ptr)
	{
		return;
	}

	for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
	{
		int role_id = zu_info_ptr->role_info[i].role_id;
		if (role_id == 0)
		{
			continue;
		}
		Role * tmp = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
		if (tmp)
		{
			EngineAdapter::Instance().NetSend(tmp->GetNetId(), msg, send_len);
		}
	}
}

void JieYiZu::SendMsgAllMemberAny(const void * msg, unsigned int send_len)
{
	const CommonSaveJieYiZu * zu_info_ptr = this->GetCommonSaveJieYiZu(false);
	if (!zu_info_ptr)
	{
		return;
	}

	for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
	{
		int role_id = zu_info_ptr->role_info[i].role_id;
		if (role_id == 0)
		{
			continue;
		}

		World::GetInstWorld()->SendToRoleAny(role_id, msg, send_len);	
	}
}

void JieYiZu::GetName(JieYIZuName * name, JieYIZuMemberName * member_name, int role_id)
{
	if (name)
	{
		*name = this->GetJieYiZuName();
	}

	if (member_name)
	{
		CommonSaveJieYiZuRoleInfoItem * member_data = this->GetZuMemberData(role_id);
		if (member_data)
		{
			SameArrayTypeCopy(*member_name, member_data->member_name);
		}
	}
}

int JieYiZu::GetJieYiCreateDay()
{
	CommonSaveJieYiZu * zu_info_ptr = this->GetCommonSaveJieYiZu(false);
	if (!zu_info_ptr)
	{
		return 1;
	}

	time_t now = EngineAdapter::Instance().Time();
	int day = GetDayIndex(zu_info_ptr->create_time, now) + 1;
	return day;
}

int JieYiZu::GetJieYiMutuaIntimacyMin()
{
	int ret = 0;

	const CommonSaveJieYiZu * zu_info_ptr = this->GetCommonSaveJieYiZu( false);
	if (!zu_info_ptr)
	{
		return ret;
	}

	for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
	{
		int role_id1 = zu_info_ptr->role_info[i].role_id;
		if (role_id1 == 0)continue;
		for (int k = i + 1; k < ARRAY_LENGTH(zu_info_ptr->role_info); ++k)
		{
			int role_id2 = zu_info_ptr->role_info[k].role_id;
			if (role_id2 == 0)continue;

			if (ret == 0)
			{
				ret = FriendIntimacyManager::Instance().GetFriendIntimacy(role_id1, role_id2);
			}
			else
			{
				int t = FriendIntimacyManager::Instance().GetFriendIntimacy(role_id1, role_id2);
				ret = std::min(ret, t);
			}
		}		
	}

	return ret;
}

void JieYiZu::AutoSetZuZhang()
{
	CommonSaveJieYiZu * zu_info_ptr = this->GetCommonSaveJieYiZu( false);
	if (zu_info_ptr)
	{
		int index = -1;
		for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
		{
			const CommonSaveJieYiZuRoleInfoItem & curr = zu_info_ptr->role_info[i];
			if (curr.role_id == 0)
			{
				continue;
			}

			if (curr.is_zu_zhang)
			{
				break;
			}

			if (index == -1)
			{
				index = i;
			}
		}

		if (index != -1)
		{
			zu_info_ptr->role_info[index].is_zu_zhang = true;;
		}
	}
}

int JieYiZu::GetMemberCount()
{
	int count = 0;
	const CommonSaveJieYiZu * zu_info_ptr = this->GetCommonSaveJieYiZu(false);
	if (zu_info_ptr)
	{
		for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
		{
			int role_id = zu_info_ptr->role_info[i].role_id;
			if (role_id == 0)
			{
				continue;
			}
			count += 1;
		}
	}

	return count;
}

std::vector<int> JieYiZu::GetMemberList()
{
	std::vector<int> ret;
	const CommonSaveJieYiZu * zu_info_ptr = this->GetCommonSaveJieYiZu(false);
	if (zu_info_ptr)
	{
		for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
		{
			int role_id = zu_info_ptr->role_info[i].role_id;
			if (role_id == 0)
			{
				continue;
			}
			ret.push_back(role_id);
		}
	}

	return ret;
}

int JieYiZu::GetZuZhangRoleId()
{
	const CommonSaveJieYiZu * zu_info_ptr = this->GetCommonSaveJieYiZu(false);
	if (zu_info_ptr)
	{
		for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
		{
			int role_id = zu_info_ptr->role_info[i].role_id;
			if (role_id == 0 || !zu_info_ptr->role_info[i].is_zu_zhang)
			{
				continue;
			}
			
			return role_id;
		}
	}

	return 0;
}

void JieYiZu::OnDismissJieYiZu()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		static MailContentParam contentparam; contentparam.Reset();
		SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_jie_yi_dismiss_subject);
		SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_jie_yi_dismiss_content);
		this->SendMailAllMember(MAIL_REASON_DEFAULT, contentparam);
	}

	std::vector<int> list = this->GetMemberList();
	for (int i = 0; i < (int)list.size(); ++i)
	{
		gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d | jie_yi_zu[%lld] role_id[%d]", __FUNCTION__, __LINE__, this->GetJieYiZuId(), list[i]);
		this->OnRoleLeave(list[i], __FUNCTION__);
	}

	this->DeleteData();
}

long long JieYiZu::GetJieYiZuId()
{
	return m_unique_id;
}

bool JieYiZu::IsZuMember(int role_id)
{
	CommonSaveJieYiZu * zu_info_ptr = this->GetCommonSaveJieYiZu(false);
	if (zu_info_ptr)
	{
		for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
		{
			const CommonSaveJieYiZuRoleInfoItem & curr = zu_info_ptr->role_info[i];
			if (curr.role_id == role_id)
			{
				return true;
			}
		}
	}

	return false;
}

bool JieYiZu::IsZuZhang(int role_id)
{
	const CommonSaveJieYiZuRoleInfoItem * member_data = this->GetZuMemberData(role_id);
	if (member_data != NULL && member_data->is_zu_zhang)
	{
		return true;
	}

	return false;
}

unsigned int JieYiZu::GetZuCreateTime()
{
	CommonSaveJieYiZu * zu_info_ptr = this->GetCommonSaveJieYiZu();
	if (!zu_info_ptr)
	{
		return 0;
	}

	return (unsigned int)zu_info_ptr->create_time;
}

CommonSaveJieYiZuRoleInfoItem * JieYiZu::GetZuMemberData(int role_id, bool is_set_dirty)
{
	CommonSaveJieYiZu * zu_info_ptr = this->GetCommonSaveJieYiZu(is_set_dirty);
	if (!zu_info_ptr)
	{
		return NULL;
	}

	for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
	{
		CommonSaveJieYiZuRoleInfoItem & curr = zu_info_ptr->role_info[i];
		if (curr.role_id == role_id)
		{
			return &curr;
		}
	}

	return NULL;
}

CommonSaveJieYiZu * JieYiZu::GetCommonSaveJieYiZu(bool is_set_dirty)
{	
	is_set_dirty = true;  //先所有都保存
	if (is_set_dirty)
	{
		this->SetDataChange();
	}

	return m_data;
}

bool JieYiZu::SetCreateData(const JieYIZuName & jie_yi_name, const std::vector<CommonSaveJieYiZuRoleInfoItem> & list)
{
	CommonSaveJieYiZu * zu_info_ptr = this->GetCommonSaveJieYiZu(true);
	if (!zu_info_ptr)
	{
		return false;
	}

	std::string role_names;  //用来传闻,XXXX,YYYY,ZZZZ,CCCC,BBBB

	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
	CommonSaveJieYiZu & zu_info = *zu_info_ptr;
	zu_info.jie_yi_zu_id = m_unique_id;
	zu_info.create_time = now;
	zu_info.jie_yi_name = jie_yi_name;
	for (int i = 0; i < (int)list.size(); ++i)
	{
		if (i > 0)
		{
			role_names += ", ";
		}

		role_names += STRING_SPRINTF("{r;%d;%s}", list[i].role_id, list[i].role_name);
		this->AddJieYiMember(list[i], true);
	}

	// 传闻
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_jie_yi_create_broadcast, role_names.c_str());
	if (length > 0)
	{
		World::GetInstWorld()->GetSceneManager()->SystemMsgAllGame(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT));
	}

	return true;
}

int JieYiZu::SetSyncData(const CommonSaveJieYiZu & data)
{
	CommonSaveJieYiZu * zu_info_ptr = this->GetCommonSaveJieYiZu(true);
	if (!zu_info_ptr)
	{
		return -__LINE__;
	}

	std::set<int> leave_role_id;
	for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
	{
		leave_role_id.insert(zu_info_ptr->role_info[i].role_id);
	}

	for (int i = 0; i < ARRAY_LENGTH(data.role_info); ++i)
	{
		leave_role_id.erase(data.role_info[i].role_id);
	}

	*zu_info_ptr = data;
	//this->SendDismissInfo(leave_role_id);  
	this->SendInfo();

	gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d |   jie_yi_zu_id:%lld jie_yi_name:%s", __FUNCTION__, __LINE__,
		this->GetJieYiZuId(), JieYIZuNameToString(this->GetJieYiZuName()).c_str());

	for (std::set<int>::iterator iter = leave_role_id.begin(); iter != leave_role_id.end(); ++iter)
	{
		this->OnRoleLeave(*iter, __FUNCTION__);
	}

	std::vector<int> list = this->GetMemberList();
	for (int i = 0; i < (int)list.size(); ++i)
	{
		this->OnRoleAdd(list[i], __FUNCTION__);
	}

	return 0;
}

int JieYiZu::SetSyncRoleData(const CommonSaveJieYiZuRoleInfoItem & data)
{
	CommonSaveJieYiZuRoleInfoItem * member_data = this->GetZuMemberData(data.role_id);
	if (!member_data)
	{
		return -__LINE__;
	}

	*member_data = data;

	return 0;
}

void JieYiZu::DeleteData()
{
	m_common_save->RemoveData(this->GetJieYiZuId());
	m_data = NULL;
}

void JieYiZu::SyncDataHiddenToGame(int server_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	CommonSaveJieYiZu * data = this->GetCommonSaveJieYiZu(false);
	if (!data)
	{
		return;
	}

	gamelog::g_log_jie_yi.printf(LL_INFO, "%s line:%d |  send_server_id:%d jie_yi_zu_id:%lld jie_yi_name:%s", __FUNCTION__, __LINE__, server_id, this->GetJieYiZuId(), JieYIZuNameToString(this->GetJieYiZuName()).c_str());

	crossgameprotocal::HiddenGameJieYiZuSync protocol;
	protocol.sync_type = JIE_YI_ZU_SYNC_TYPE_UPDATE;
	protocol.jie_yi_zu_info = *data;
	InternalComm::Instance().SendToGameThroughCross2((char *)&protocol, sizeof(protocol), server_id);
}

void JieYiZu::SyncMemberDataHiddenToGame(int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	CommonSaveJieYiZuRoleInfoItem * member_data = this->GetZuMemberData(role_id);
	if (member_data == NULL)
	{
		return;
	}

	crossgameprotocal::HiddenGameJieYiSyncRoleData protocol;
	protocol.role_data = *member_data;
	InternalComm::Instance().SendToGameThroughCross2((char *)&protocol, sizeof(protocol), 0);	
}

void JieYiZu::OnHiddenGameRoleChange(const CommonSaveJieYiZuRoleInfoItem * role_info)
{
	if (!CrossConfig::Instance().IsHiddenServer() || NULL == role_info)
	{
		return;
	}
	unsigned int create_timestamp = this->GetZuCreateTime();
	crossgameprotocal::HiddenGameJieYiRoleChange hgjyrc;
	hgjyrc.role_id = role_info->role_id;
	hgjyrc.join_jie_yi_day = this->GetJoinJieYiDay(create_timestamp, role_info->join_time);

	InternalComm::Instance().SendToGameThroughCross2((const char *)&hgjyrc, sizeof(hgjyrc), IntToUid(hgjyrc.role_id).db_index);
}

JieYIZuName JieYiZu::GetJieYiZuName()
{
	JieYIZuName name;
	name.Reset();	
	CommonSaveJieYiZu * zu_info_ptr = this->GetCommonSaveJieYiZu(false);
	if (zu_info_ptr)
	{
		name = zu_info_ptr->jie_yi_name;		
		name.member_num = this->GetMemberCount();
	}

	return name;
}

void JieYiZu::SendMailAllMember(int reason, const MailContentParam& contentparam, int exclude_role_id)
{
	const CommonSaveJieYiZu* zu_info_ptr = this->GetCommonSaveJieYiZu(false);
	if (!zu_info_ptr)
	{
		return;
	}

	for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
	{
		int role_id = zu_info_ptr->role_info[i].role_id;
		if (role_id == 0 || role_id == exclude_role_id)
		{
			continue;
		}
		
		MailRoute::MailToUser(role_id, reason, contentparam);
	}
}

void JieYiZu::SetDataChange()
{
	long long jie_yi_zu_id = this->GetJieYiZuId();
	m_common_save->SetDataChange(jie_yi_zu_id);
}

void JieYiZu::AddJieYiMember(const CommonSaveJieYiZuRoleInfoItem & role_info,bool is_create)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (this->IsZuMember(role_info.role_id))
	{
		return;
	}

	CommonSaveJieYiZuRoleInfoItem * empty_info = this->GetEmptyRoleInfo();
	if (!empty_info)
	{
		return;
	}

	*empty_info = role_info;
	empty_info->join_time = (unsigned int)EngineAdapter::Instance().Time();
	this->OnHiddenGameRoleChange(empty_info);
	JieYiZuManager::Instance().RoleXuanYan(role_info.role_id, true);

	if (!is_create)
	{	// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_jie_yi_add_member_subject);
		SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_jie_yi_add_member_content, role_info.role_id, role_info.role_name);
		this->SendMailAllMember(MAIL_REASON_DEFAULT, contentparam, role_info.role_id);
	}

	this->OnRoleAdd(role_info.role_id, __FUNCTION__);
}

void JieYiZu::OnRoleLeave(int role_id, const char * reason)
{
	Protocol::SCJieYiZuLeaveNotice notice;
	notice.jie_yi_zu_id = this->GetJieYiZuId();
	World::GetInstWorld()->SendToRole(role_id, &notice, sizeof(notice));

	JieYiZuManager::Instance().OnRoleLeave(role_id, this->GetJieYiZuId(), reason);
}

void JieYiZu::OnRoleAdd(int role_id, const char * reason)
{
	JieYiZuManager::Instance().OnRoleAdd(this->GetJieYiZuId(), role_id, reason);
}

CommonSaveJieYiZuRoleInfoItem * JieYiZu::GetEmptyRoleInfo()
{
	CommonSaveJieYiZu * zu_info_ptr = this->GetCommonSaveJieYiZu(false);
	if (zu_info_ptr)
	{
		for (int i = 0; i < ARRAY_LENGTH(zu_info_ptr->role_info); ++i)
		{
			CommonSaveJieYiZuRoleInfoItem & curr = zu_info_ptr->role_info[i];
			if (curr.role_id == 0)
			{
				return &curr;
			}
		}
	}

	return NULL;
}

int JieYiZu::GetJoinJieYiDay(unsigned int create_time, unsigned int join_time)
{
	time_t now = EngineAdapter::Instance().Time();
	return GetDayIndex(join_time, now) + 1;
}



