#include "gamelog.h"
#include "guildmember.hpp"
#include "guildmembermanager.hpp"
#include "guild.hpp"
#include "guildmanager.hpp"
#include "world.h"
#include "engineadapter.h"
#include "internalcomm.h"
#include "global/usercache/usercache.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "item/money.h"
#include "other/event/eventhandler.hpp"
#include "servercommon/gatherdreamdef.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "other/title/title.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/guildconfig.hpp"
#include "other/route/mailroute.hpp"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "servercommon/errornum.h"

GuildMember::GuildMember(GuildMemberManager *member_manager)
	: m_member_manager(member_manager)
{
	m_index = 0;
	m_uid = 0;
	m_level = 0;
	memset(m_name, 0, sizeof(m_name));
	m_profession = 0;
	m_post = 0;
	m_online_state = 0;
	m_gongxian = 0;
	m_total_gongxian = 0;
	m_join_time = 0;
	m_last_login_time = 0;

#ifdef _DEBUG
	m_index_FOR_TEST = 0;
#endif
}

GuildMember::~GuildMember()
{
#ifdef _DEBUG
	assert(m_index == m_index_FOR_TEST);
#endif
}

void GuildMember::Init(const GuildMemberParam::Member &member_param)
{
	m_index = member_param.index;
	m_uid = member_param.uid;
	m_level = member_param.level;
	m_profession = member_param.m_profession;
	F_STRNCPY(m_name, member_param.name, sizeof(GameName));
	m_appearance = member_param.appearance;
	m_post = member_param.post;
	m_gongxian = member_param.gongxian;
	m_total_gongxian = member_param.total_gongxian;
	m_join_time = member_param.join_time;
	m_last_login_time = member_param.last_login_time;
	m_member_data = member_param.member_data;

#ifdef _DEBUG
	m_index_FOR_TEST = m_index;
#endif
}

void GuildMember::GetAttr(GuildMemberParam::Member *member_attr) const
{
	member_attr->index = m_index;

	member_attr->uid = m_uid;
	member_attr->level = m_level;
	member_attr->m_profession = m_profession;
	F_STRNCPY(member_attr->name, m_name, sizeof(GameName));
	member_attr->appearance = m_appearance;
	member_attr->post = m_post;

	member_attr->gongxian = m_gongxian;
	member_attr->total_gongxian = m_total_gongxian;

	member_attr->join_time = m_join_time;
	member_attr->last_login_time = m_last_login_time;

	member_attr->member_data = m_member_data;
}

bool GuildMember::GetInfo(Protocol::MemberInfo* info)
{
	if (NULL == info) return false;
	const GuildMemberData & data = this->GetMemberData();

	F_STRNCPY(info->name, m_name, sizeof(GameName));
	info->level = m_level;
	info->m_profession = m_profession;
	info->appearance = m_appearance;

	info->uid = m_uid;
	info->post = m_post;
	info->gongxian = m_gongxian;
	info->total_gongxian = m_total_gongxian;
	info->online = UserCacheManager::Instance().IsUserOnline(m_uid) ? 1 : 0;
	info->join_time = (long long)m_join_time;
	info->last_online_timestamp = (long long)m_last_login_time;
	info->seven_active = this->GetSevenDayTotalActive();
	info->seven_active_times = this->GetLastChangeWeekActiveValTimes();
	info->top_level = UserCacheManager::Instance().GetUserTopLevel(m_uid);

	info->role_guild_task_finish_times= data.role_guild_task_finish_times;		//完成次数
	info->role_guild_task_is_mail_notice = data.role_guild_task_is_mail_notice;		//邮件通知
	info->role_guild_task_is_solicit = data.role_guild_task_is_solicit;		//是否征集
	info->role_guild_task_solicit_task_id = data.role_guild_task_solicit_task_id;		//征集任务id
	info->role_guild_task_solicit_num = data.role_guild_task_solicit_num;		//征集进度
	info->role_guild_task_solicit_fetch_num = data.role_guild_task_solicit_fetch_num;	//征集进度是否领取

	return true;
}

void GuildMember::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	// 七日活跃
	memmove(m_member_data.seven_day_active, m_member_data.seven_day_active + 1, sizeof(m_member_data.seven_day_active[0]) * (GUILD_SEVEN_DAY_ACTIVE - 1));
	m_member_data.seven_day_active[GUILD_SEVEN_DAY_ACTIVE - 1] = 0;

	// 家族boss
	m_member_data.guild_boss_total_damage = 0;
	m_member_data.guild_boss_fetch_chapter_reward_times = 0;
	m_member_data.is_fetch_boss_chapter_damage_rank_reward = 0;
	m_member_data.guild_boss_today_highest_damage = 0;
	this->ClearQiFuFlag();
	m_member_data.role_guild_task_finish_times = 0;
	m_member_data.role_guild_task_is_mail_notice = 0;
}

void GuildMember::OnWeekChange()
{
	m_member_data.guild_boss_join_flag = 0;
	m_member_data.guild_boss_baodi_reward_flag = 0;

	m_member_data.battle_value = 0;
	m_member_data.battle_value_change_last_tamp = 0;

	m_member_data.rich_value = 0;
	m_member_data.rich_value_change_last_tamp = 0;

	m_member_data.kinder_value = 0;
	m_member_data.kinder_value_change_last_tamp = 0;
}

const char* GuildMember::GetName()
{
	return m_name;
}

void GuildMember::GetOriginalName(GameName original_name)
{
	std::string cross_role_name(m_name);
	int ret = (int)cross_role_name.find_last_of('_');
	if (ret < 0)
	{
		F_STRNCPY(original_name, m_name, sizeof(GameName))//无后坠
	}
	else 
	{
		if (ret + 1 <= (int)sizeof(GameName))F_STRNCPY(original_name, cross_role_name.substr(0, cross_role_name.find_last_of('_')).c_str(), ret + 1);
	}
}

void GuildMember::SetName(GameName name)
{
	F_STRNCPY(m_name, name, sizeof(GameName));
}

void GuildMember::SetLevel(int level)
{
	m_level = level;
}

int GuildMember::GetLevel()
{
	return m_level;
}

void GuildMember::SetAppearance(const Appearance& appearance)
{
	m_appearance = appearance;
}

void GuildMember::SetHonorBattleFlag()
{
	 m_member_data.join_honor_battle_flag = 1; 
	 gamelog::g_log_guild_honor_battle.printf(LL_INFO ,"%s uid[%d] m_member_data.join_honor_battle_flag[%d]", __FUNCTION__, this->GetUid(), m_member_data.join_honor_battle_flag);
}

void GuildMember::ClearHonorBattleFlag()
{
	m_member_data.join_honor_battle_flag = 0;
	gamelog::g_log_guild_honor_battle.printf(LL_INFO, "%s uid[%d] m_member_data.join_honor_battle_flag[%d]", __FUNCTION__, this->GetUid(), m_member_data.join_honor_battle_flag);
}

void GuildMember::SetProfession(int profession)
{
	m_profession = profession;
}

int GuildMember::AddGongXian(int value, const char* type)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameChangeGuildInfo hgcgi;
		hgcgi.change_type = crossgameprotocal::HIDDEN_GAME_GUILD_CHANGE_TYPE_ADD_GONGXIAN;
		hgcgi.guild_id = m_member_manager->GetGuild()->GetGuildID();
		hgcgi.param = m_uid;
		hgcgi.param2 = value;

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(hgcgi.guild_id).db_index, (const char *)&hgcgi, sizeof(hgcgi));
		return 0;
	}

	if (m_total_gongxian < MAX_INT - value)
	{
		m_total_gongxian += value;
	}
	else
	{
		m_total_gongxian = MAX_INT;
	}

	m_member_manager->SetDirtyMark(m_index);
	m_member_manager->OnMemberInfoChange(IntToUid(m_uid));
	gamelog::g_log_guild.printf(LL_INFO, "%s user[%d] value[%d] type[%s]", __FUNCTION__, m_uid, value, type);
	return 0;
}

GuildMemberData & GuildMember::GetMemberDataAndSetDirty()
{
	m_member_manager->SetDirtyMark(m_index);
	return m_member_data;
}

void GuildMember::SetWeekTaskContribute(int value)
{
	m_member_data.guild_task_contribute = value;
	m_member_data.last_change_task_contribute_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	m_member_manager->SetDirtyMark(m_index);
	gamelog::g_log_guild.printf(LL_INFO, "SetWeekTaskContribute user[%d] value[%d]", m_uid, value);
}

void GuildMember::AddWeekTaskContribute(int add_value)
{
	if (m_member_data.guild_task_contribute < MAX_INT - add_value)
	{
		m_member_data.guild_task_contribute += add_value;
	}
	else
	{
		m_member_data.guild_task_contribute = MAX_INT;
	}

	m_member_data.last_change_task_contribute_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	m_member_manager->SetDirtyMark(m_index);
	gamelog::g_log_guild.printf(LL_INFO, "AddWeekTaskContribute user[%d] add_value[%d] cur_value[%d]", m_uid, add_value, m_member_data.guild_task_contribute);
}

void GuildMember::SetOnline(int online_state)
{
	m_online_state = online_state;
}

int GuildMember::GetOnLinetate() const
{
	return m_online_state;
}

void GuildMember::SetPost(int post)
{
	m_member_manager->SetPostCount(m_post, -1);
	m_member_manager->SetPostCount(post, 1);
	m_post = post;
	if (GUILD_POST_PATRIARCH == post)
	{
		m_member_manager->SetPatriarchUserID(IntToUid(m_uid));
		m_member_manager->SetPatriarchLevel(m_level);
		m_member_manager->SetPatriarchName(m_name);
	}

	m_member_manager->SetDirtyMark(m_index);
	m_member_manager->OnMemberInfoChange(IntToUid(m_uid));
	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(m_uid));
	if (NULL != role)
	{
		role->SetGuildPostion(m_post);
		EventHandler::Instance().OnRoleChangeGatherDreamInfo(role, m_member_manager->GetGuild()->GetGuildID());
	}
	else
	{
		SynGatherDreamRecord syn_info;
		F_STRNCPY(syn_info.name, m_name, sizeof(syn_info.name));
		syn_info.post = m_post;
		syn_info.headshot_id = m_appearance.headshot_id;
		syn_info.avatar_type = m_appearance.avatar_type;
		EventHandler::Instance().OnRoleChangeGatherDreamInfo(NULL, m_member_manager->GetGuild()->GetGuildID(), &syn_info, &m_uid);
	}
}

void GuildMember::OnRoleLogin(Role * role)
{
	// 同步一次贡献值
	m_gongxian = (int)role->GetRoleModuleManager()->GetMoney()->GetOtherCoin(MONEY_TYPE_GUILD_CONTRIBUTION);
	this->SetOnline(UserStatusNode::STATUS_TYPE_ONLINE);

	// 同步一下活跃度奖励信息
	this->SetGuildDayActiveRewardFlag(role->GetRoleModuleManager()->GetCommonData().guild_day_active_reward_flag);

	this->UpdateMemberInfo(role); //放最下面
}

void GuildMember::OnRoleLogout(Role * role)
{
	m_last_login_time = EngineAdapter::Instance().Time();
	this->SetOnline(UserStatusNode::STATUS_TYPE_OFFLINE);

	this->UpdateMemberInfo(role); //放最下面
	m_member_manager->SetDirtyMark(m_index);
}

void GuildMember::UpdateMemberInfo(Role* role)
{
	m_level = role->GetLevel();
	m_profession = role->GetProfession();
	F_STRNCPY(m_name, role->GetName(), sizeof(GameName));
	m_appearance = role->GetRoleModuleManager()->GetAppearance();

	m_member_manager->OnMemberInfoChange(role->GetUserId());
}

void GuildMember::UpdateMemberInfoFromHidden(const GuildMemberParam::Member & member_param)
{
	// 跨服只同步一些跨服会改变的东西

	std::string cross_role_name(member_param.name);
	cross_role_name = cross_role_name.substr(0, cross_role_name.find_last_of('_'));

	m_level = member_param.level;
	m_profession = member_param.m_profession;
	F_STRNCPY(m_name, cross_role_name.c_str(), sizeof(GameName));
	m_appearance = member_param.appearance;
	m_last_login_time = member_param.last_login_time;

	m_member_data.battle_value = member_param.member_data.battle_value;
	m_member_data.rich_value = member_param.member_data.rich_value;
	m_member_data.kinder_value = member_param.member_data.kinder_value;
	m_member_data.is_get_guild_attr = member_param.member_data.is_get_guild_attr;
	m_member_data.join_honor_battle_flag = member_param.member_data.join_honor_battle_flag;
	m_member_manager->SetDirtyMark(m_index);
	gamelog::g_log_guild.printf(LL_INFO, "%s user[%d] battle_value[%d] rich_value[%d] kinder_value[%d]", __FUNCTION__,
		m_uid, m_member_data.battle_value, m_member_data.rich_value, m_member_data.kinder_value);
}

unsigned int GuildMember::GetLastLogintime()
{
	return (unsigned int)m_last_login_time;
}

void GuildMember::SetTanHeJoinFlag(unsigned char f)
{
	m_member_data.tan_he_join_flag = f;
}

unsigned char GuildMember::GetSetTanHeJoinFlag()
{
	return m_member_data.tan_he_join_flag;
}

void GuildMember::AddWeekActiveVal(int _add_val)
{
	unsigned int now_sec = (unsigned int)EngineAdapter::Instance().Time();

	m_member_data.week_active_val += _add_val;
	m_member_data.last_change_week_active_val_times = now_sec;
	m_member_manager->SetDirtyMark(m_index);

	m_member_data.seven_day_active[GUILD_SEVEN_DAY_ACTIVE - 1] += _add_val;
}

void GuildMember::SetWeekActiveVal(int _week_val, int _member_seven_day_active_today_active)
{
	unsigned int now_sec = (unsigned int)EngineAdapter::Instance().Time();

	m_member_data.week_active_val = _week_val;
	m_member_data.last_change_week_active_val_times = now_sec;
	m_member_manager->SetDirtyMark(m_index);

	m_member_data.seven_day_active[GUILD_SEVEN_DAY_ACTIVE - 1] = _member_seven_day_active_today_active;
}

void GuildMember::ClearWeekActiveValInfo()
{
	m_member_data.week_active_val = 0;
	m_member_data.last_change_week_active_val_times = 0;
	m_member_manager->SetDirtyMark(m_index);
}

void GuildMember::SetGuildDayActiveRewardFlag(int _flag)
{
	m_member_data.guild_day_active_reawrd_flag = _flag;
	m_member_manager->SetDirtyMark(m_index);
}

void GuildMember::ClearGuildDayActiveRewardFlag()
{
	m_member_data.guild_day_active_reawrd_flag = 0;
	m_member_manager->SetDirtyMark(m_index);
}

void GuildMember::GuildActiveDayActiveReward(int _day_active_val, unsigned int _now_sec)
{
	// 先给玩家重置日活跃领取标记
	int flag = this->GetGuildDayActiveRewardFlag();
	this->SetGuildDayActiveRewardFlag(0);

	static MailContentParam content_param;
	content_param.Reset();
	int lensub = SNPRINTF(content_param.subject, sizeof(content_param.subject), gamestring::g_guild_active_day_active_subject);
	int length = SNPRINTF(content_param.contenttxt, sizeof(content_param.contenttxt), gamestring::g_guild_active_day_active_content, _day_active_val);
	if (lensub <= 0 || length <= 0)
	{
		gamelog::g_log_guild.printf(LL_INFO, "GuildMember::GuildActiveDayActiveReward[ERROR_1]  guild[%d %s] day_active_val:%d ",
			m_member_manager->GetGuild()->GetGuildID(), m_member_manager->GetGuild()->GetName(), _day_active_val);
		return;
	}

	// 计算能否发奖励
	unsigned int diff_val = _now_sec - this->GetLastLogintime();
	if (diff_val > 86400)
	{
		// 则昨天没有登录过 ,记录一下日志
		gamelog::g_log_guild.printf(LL_INFO, "GuildMember::GuildActiveDayActiveReward[ERROR_2]  guild[%d %s]  role[%d %s]  "
			"now_sec:%u  last_login_time:%u  diff_val:%u",
			m_member_manager->GetGuild()->GetGuildID(), m_member_manager->GetGuild()->GetName(),
			this->GetUid(), this->GetName(),
			_now_sec, this->GetLastLogintime(), diff_val);
		return;
	}

	// 因为是int记录的领奖标记 , 所以最大为32尺循环 , 实际不会那么多次
	for (int i = 0; i < 32; ++i)
	{
		if (flag & LL_ONE_BIT << i)
			continue;

		const GuildActiveGiftCfg::GiftCfg * gift_cfg = LOGIC_CONFIG->GetGuildConfig()->GetGuildActiveGiftCfg(i , this->GetLevel());
		if (NULL == gift_cfg)
			return;

		if (_day_active_val < gift_cfg->active_limit)
		{
			return;
		}

		int old_flag = flag;
		flag |= LL_ONE_BIT << i;

		// 发奖励
		content_param.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = gift_cfg->exp;

		if (0 < gift_cfg->count)
		{
			memset(content_param.item_list, 0, sizeof(content_param.item_list));
			for (int reward_index = 0; reward_index < gift_cfg->count; ++reward_index)
			{
				content_param.item_list[reward_index].item_id = gift_cfg->reward_list[reward_index].item_id;
				content_param.item_list[reward_index].num = gift_cfg->reward_list[reward_index].num;
				content_param.item_list[reward_index].is_bind = gift_cfg->reward_list[reward_index].is_bind;
			}
		}

		MailRoute::MailToUser(IntToUid(this->GetUid()), MAIL_REASON_DEFAULT, content_param);

		// 日志
		gamelog::g_log_guild.buff_printf("GuildMember::GuildActiveDayActiveReward  guild[%d %s]  role[%d %s]  role_level[%d]  "
			"guild_day_active_val:%d  seq:%d  old_flag:%d  new_flag:%d  reward_exp:%d" ,
			m_member_manager->GetGuild()->GetGuildID(), m_member_manager->GetGuild()->GetName(),
			this->GetUid(), this->GetName(), this->GetLevel(),
			_day_active_val, i, old_flag, flag , gift_cfg->exp);
		for (int reward_index = 0; reward_index < gift_cfg->count; ++reward_index)
		{
			gamelog::g_log_guild.buff_printf("item[index:%d  item_id:%d  num:%d]  ",
				reward_index, gift_cfg->reward_list[reward_index].item_id, gift_cfg->reward_list[reward_index].num);
		}
		gamelog::g_log_guild.commit_buff(LL_INFO);
	}
}

void GuildMember::GuildActiveWeekActivePersonRankReward(int _rank_index, int _val, unsigned int _now_sec)
{
	const GuildActivePersonRankRewardCfg * reward_cfg = LOGIC_CONFIG->GetGuildConfig()->GetGuildActivePersonRankRewardCfg(_rank_index);
	if (NULL == reward_cfg)
	{
		gamelog::g_log_guild.printf(LL_INFO, "GuildMember::GuildActiveWeekActivePersonRankReward[ERROR_1] "
			" guild[%d %s]  role[%d]  rank_index:%d   val:%d",
			m_member_manager->GetGuild()->GetGuildID(), m_member_manager->GetGuild()->GetName(),
			this->GetUid(), _rank_index, _val);
		return;
	}

	// 发放对应名次的奖励
	static MailContentParam content_param;
	content_param.Reset();
	int lensub = SNPRINTF(content_param.subject, sizeof(content_param.subject), gamestring::g_guild_active_week_active_person_rank_subject);
	int length = SNPRINTF(content_param.contenttxt, sizeof(content_param.contenttxt), gamestring::g_guild_active_week_active_person_rank_content, _rank_index);
	if (lensub <= 0 || length <= 0)
	{
		gamelog::g_log_guild.printf(LL_INFO, "GuildMember::GuildActiveWeekActivePersonRankReward[ERROR_2]  guild[%d %s] role[%d %s] rank_index[%d] val[%d]",
			m_member_manager->GetGuild()->GetGuildID(), m_member_manager->GetGuild()->GetName(),
			this->GetUid(), this->GetName(), _rank_index, _val);
		return;
	}

	content_param.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GUILDGONGXIAN] = reward_cfg->gongxian;
	content_param.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = reward_cfg->coin;
	memset(content_param.item_list, 0, sizeof(content_param.item_list));
	for (int reward_i = 0; reward_i < reward_cfg->count; ++reward_i)
	{
		content_param.item_list[reward_i].item_id = reward_cfg->reward_list[reward_i].item_id;
		content_param.item_list[reward_i].num = reward_cfg->reward_list[reward_i].num;
		content_param.item_list[reward_i].is_bind = reward_cfg->reward_list[reward_i].is_bind;
	}

	MailRoute::MailToUser(IntToUid(this->GetUid()), MAIL_REASON_DEFAULT, content_param);

	//发头衔
	unsigned int dec_times = GetWeekZeroTime(_now_sec) + 86400 * 7 - 60 * 5;
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(this->GetUid()));
	if (NULL == role)
	{
		// 不在线
		OfflineRegisterManager::Instance().OnTitleSendReq(IntToUid(this->GetUid()), reward_cfg->title, dec_times);
	}
	else
	{
		// 在线
		role->GetRoleModuleManager()->GetTitle()->AddTitle(reward_cfg->title, __FUNCTION__, dec_times);
	}

	// 日志
	gamelog::g_log_guild.buff_printf("GuildMember::GuildActiveWeekActivePersonRankReward  guild[%d %s]  role[%d %s]  "
		"rank_index:%d  val:%d  tital:%d  coin:%d  gongxian:%d",
		m_member_manager->GetGuild()->GetGuildID(), m_member_manager->GetGuild()->GetName(),
		this->GetUid(), this->GetName(),
		_rank_index, _val, reward_cfg->title, reward_cfg->coin, reward_cfg->gongxian);
	for (int reward_index = 0; reward_index < reward_cfg->count; ++reward_index)
	{
		gamelog::g_log_guild.buff_printf("item[index:%d  item_id:%d  num:%d]  ",
			reward_index, reward_cfg->reward_list[reward_index].item_id, reward_cfg->reward_list[reward_index].num);
	}
	gamelog::g_log_guild.commit_buff(LL_INFO);
}

int GuildMember::GetSevenDayTotalActive()
{
	int total_active = 0;
	for (int i = 0; i < GUILD_SEVEN_DAY_ACTIVE; ++i)
	{
		total_active += m_member_data.seven_day_active[i];
	}

	return total_active;
}

int GuildMember::GetSevenDayTodayActive()
{
	return m_member_data.seven_day_active[GUILD_SEVEN_DAY_ACTIVE - 1];
}

void GuildMember::SetQiFuFlag()
{
	m_member_data.is_get_guild_attr = 1;
	m_member_manager->OnMemberInfoChange(IntToUid(m_uid));
	m_member_manager->SetDirtyMark(m_index);
}

void GuildMember::ClearQiFuFlag()
{
	m_member_data.is_get_guild_attr = 0;
	m_member_manager->OnMemberInfoChange(IntToUid(m_uid));
	m_member_manager->SetDirtyMark(m_index);
}

void GuildMember::AddHallOfFameVal(int type, int _add_val)
{
	unsigned int now_sec = (unsigned int)EngineAdapter::Instance().Time();
	switch (type)
	{
	case Guild::HALLOFFAME_TYPE_BATTLE:
		m_member_data.battle_value += _add_val;
		m_member_data.battle_value_change_last_tamp = now_sec;
		break;
	case Guild::HALLOFFAME_TYPE_RICH:
		m_member_data.rich_value += _add_val;
		m_member_data.rich_value_change_last_tamp = now_sec;
		break;
	case Guild::HALLOFFAME_TYPE_KINDER:
		m_member_data.kinder_value += _add_val;
		m_member_data.kinder_value_change_last_tamp = now_sec;
		break;
	}
	m_member_manager->SetDirtyMark(m_index);
	m_member_manager->OnMemberInfoChange(IntToUid(m_uid));
	gamelog::g_log_guild.printf(LL_INFO, "%s user[%d] type[%d] add_val[%d]", __FUNCTION__, m_uid, type, _add_val);
}

void GuildMember::SetHallOfFameVal(int type, int _add_val)
{
	unsigned int now_sec = (unsigned int)EngineAdapter::Instance().Time();
	switch (type)
	{
	case Guild::HALLOFFAME_TYPE_BATTLE:
		m_member_data.battle_value = _add_val;
		m_member_data.battle_value_change_last_tamp = now_sec;
		break;
	case Guild::HALLOFFAME_TYPE_RICH:
		m_member_data.rich_value = _add_val;
		m_member_data.rich_value_change_last_tamp = now_sec;
		break;
	case Guild::HALLOFFAME_TYPE_KINDER:
		m_member_data.kinder_value = _add_val;
		m_member_data.kinder_value_change_last_tamp = now_sec;
		break;
	}
	m_member_manager->SetDirtyMark(m_index);
	m_member_manager->OnMemberInfoChange(IntToUid(m_uid));
	gamelog::g_log_guild.printf(LL_INFO, "%s user[%d] type[%d] add_val[%d]", __FUNCTION__, m_uid, type, _add_val);
}

int GuildMember::GetHallOfFameVal(int type)
{
	int value = 0;
	switch (type)
	{
	case Guild::HALLOFFAME_TYPE_BATTLE:
		value = m_member_data.battle_value;
		break;
	case Guild::HALLOFFAME_TYPE_RICH:
		value = m_member_data.rich_value;
		break;
	case Guild::HALLOFFAME_TYPE_KINDER:
		value = m_member_data.kinder_value;
		break;
	}
	return value;
}

unsigned int GuildMember::GetLastChangeHallOfFameValTimes(int type)
{
	unsigned int value = 0;
	switch (type)
	{
	case Guild::HALLOFFAME_TYPE_BATTLE:
		value = m_member_data.battle_value_change_last_tamp;
		break;
	case Guild::HALLOFFAME_TYPE_RICH:
		value = m_member_data.rich_value_change_last_tamp;
		break;
	case Guild::HALLOFFAME_TYPE_KINDER:
		value = m_member_data.kinder_value_change_last_tamp;
		break;
	}
	return value;
}

void GuildMember::ClearHallOfFameValInfo(int type)
{
	switch (type)
	{
	case Guild::HALLOFFAME_TYPE_BATTLE:
		m_member_data.battle_value = 0;
		m_member_data.battle_value_change_last_tamp = 0;
		break;
	case Guild::HALLOFFAME_TYPE_RICH:
		m_member_data.rich_value = 0;
		m_member_data.rich_value_change_last_tamp = 0;
		break;
	case Guild::HALLOFFAME_TYPE_KINDER:
		m_member_data.kinder_value = 0;
		m_member_data.kinder_value_change_last_tamp = 0;
		break;
	}
	m_member_manager->SetDirtyMark(m_index);
	m_member_manager->OnMemberInfoChange(IntToUid(m_uid));
	gamelog::g_log_guild.printf(LL_INFO, "%s user[%d] type[%d]", __FUNCTION__, m_uid, type);
}

bool GuildMember::JoinSpeedUp(Role* role)
{
	const GuildOtherCfg* other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg();
	if (NULL == other_cfg) return false;

	if (role->GetRoleModuleManager()->GetCommonData().guild_build_join_speed_up_count >= other_cfg->person_times)
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_GUILD_CANNOT_REPEAT_SPEED_UP);
		return false;
	}

	if (!role->GetRoleModuleManager()->GetMoney()->CoinBindMoreThan(other_cfg->expend_coin))
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_GUILD_JOIN_SPEED_UP_BUILDING_LACK_COIN);
		return false;
	}

	role->GetRoleModuleManager()->GetMoney()->UseCoinBind(other_cfg->expend_coin, __FUNCTION__);

	role->GetRoleModuleManager()->GetCommonData().guild_build_join_speed_up_count += 1;
//	m_member_data.is_cur_building_help += 1;

// 	{	// 同步到跨服
// 		crossgameprotocal::GameHiddenSynGuildSpeedUpRoleInfo msg;
// 		msg.role_id = role->GetUID();
// 		msg.speed_up_count = m_member_data.is_cur_building_help;
// 
// 		InternalComm::Instance().SendToHiddenThroughCross((const char *)&msg, sizeof(msg));
// 	}

	gamelog::g_log_guild.printf(LL_INFO, "%s, join speed up usecoin[%d]", __FUNCTION__, other_cfg->expend_coin);
	return true;
}

void GuildMember::SynSpeedUpCount(int count)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

//	m_member_data.is_cur_building_help = count;
}
