#include "item/knapsack.h"
#include "guildmembermanager.hpp"
#include "guild.hpp"
#include "guildmember.hpp"
#include "config/otherconfig/guildconfig.hpp"
#include "engineadapter.h"
#include "global/rmibackobjdef.h"
#include "internalcomm.h"
#include "protocol/msgother.h"

#include "guildmanager.hpp"

#include "servercommon/string/globalstr.h"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"

#include "other/rolemodulemanager.hpp"
#include "other/route/mailroute.hpp"
#include "obj/character/role.h"
#include "world.h"
#include "config/logicconfigmanager.hpp"

#include <algorithm>
#include "servercommon/string/gameworldstr.h"
#include "config/otherconfig/guildweektaskconfig.hpp"
#include "global/usercache/usercache.hpp"
#include "item/itempool.h"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "chat/chatmanager.h"
#include "friend/friendmanager.h"
#include "servercommon/userprotocal/msgchatmsg.h"
#include "gameworld/gamelog.h"
#include "monster/monster_group_pool.hpp"
#include "monster/monsterpool.hpp"
#include "other/pet/petconfig.hpp"
#include "other/partner/partnerconfig.hpp"
#include "other/rolegatherdream/rolegatherdream.hpp"
#include "other/facescore/facescore.hpp"
#include "other/rolecross/rolecross.hpp"
#include "global/gatherdreammanager/gatherdreamrecordmanager.hpp"
#include "config/activityconfig/guildfight/activityguildfightconfig.hpp"
#include "global/guildhonorbattlemanager/guildhonorbattlemanager.hpp"

#include "global/cross/crossuserregister.hpp"
#include "global/rank/rankmanager.hpp"
#include "other/title/title.hpp"
#include "other/flyupsystem/flyupsystem.hpp"
#include "global/worldstatus/worldstatus.hpp"
#include "servercommon/commondata.hpp"
#include "other/roleguildtask/roleguildtask.hpp"
#include "task/taskpool.h"
#include "other/roleguildtask/roleguildtaskconfig.hpp"

bool WeekActiveRankCompare(const Guild::WeekActiveInfo & _a, const  Guild::WeekActiveInfo & _b)
{
	if (_a == _b)
	{
		return true;
	}
	return _a < _b;
}


bool FallOfFameRankSort(const Guild::HallOfFameInfo& a, const Guild::HallOfFameInfo& b)
{
	if (a.value != b.value)
	{
		return a.value > b.value;
	}
	else if (a.last_change_time != b.last_change_time)
	{
		return a.last_change_time < b.last_change_time;
	}
	else
	{
		return a.role_id < b.role_id;
	}
}

Guild::Guild(GuildManager *guild_manager)
	: m_guild_manager(guild_manager), m_next_save_time(0),
	m_need_refresh_gwt_contribution_rank(false), m_next_gwt_contribution_rank_refresh_timestamp(0),
	m_guild_boss_damage_rank_need_update(false), m_guild_boss_damage_rank_next_update_time(0),
	m_is_dirty(false), m_next_guild_sync_cross_time(0u), m_battle_refresh_tamp(0u), m_rich_refresh_tamp(0u), m_kinder_refresh_tamp(0u)
{
	m_guild_member_manager.SetGuild(this);

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		m_next_guild_sync_cross_time = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + RandomNum(50, 70);
	}
	m_gather_dream_map.clear();
	m_gm_flag = 0;
	m_gather_dream_timestamp_map.clear();

	m_week_active_rank.SetDefaultMaxSize(GUILD_MAX_MEMBER_COUNT);
	m_week_active_rank.SetCompare(&WeekActiveRankCompare);
}

Guild::~Guild()
{

}

void Guild::SetNeedApproveJoin(Role * role, bool is_need)
{
	GuildMember * mem_info = this->GetMemberManagerRef().GetMember(role->GetUID());
	if (NULL == mem_info)
	{
		return;
	}

	switch (mem_info->GetPost())
	{
	case GUILD_POST_PATRIARCH:
	case GUILD_POST_VICE_PATRIARCH:
	case GUILD_POST_ELDER:
		{
			m_base_param.is_need_approval = is_need ? 1 : 0;
			this->SendGuildNeedApproveInfo();
		}
		break;
	default:
		{
			role->NoticeNum(errornum::EN_GUILD_INSUFFICIENT_AUTHORITY);
		}
		break;
	}
}

bool Guild::IsNeedApproveJoin()
{
	return m_base_param.is_need_approval != 0;
}

bool Guild::JoinGuild(Role * role)
{
	if (role == NULL)
	{
		return false;
	}

	if (role->GetGuildID() != 0)//有家族
	{
		return false;
	}

	GameName role_name = { 0 };
	role->GetName(role_name);
	if (m_guild_member_manager.AddMember(role->GetUID(), role_name) == NULL)
	{
		return false;
	}

	this->GuildTaskSetFinishTimes(role);

	this->SendGuildBaseInfo(role);
	UserCacheNode* leadernode = UserCacheManager::Instance().GetUserNode(this->GetMemberManagerRef().GetPatriarchUserID());
	GuildManager::Instance().NewbieJoinGuild(leadernode, role->GetUID(), role->GetGuildID());
	role->GetRoleModuleManager()->ReCalcAttr(AttributeList::RECALC_REASON_GUILD_QIFU, __FUNCTION__);//检查祈福属性
	this->SendQifuInfo(role);
	this->SendAllBuildInfo(role);
	this->SendSpeedUpInfo(role);
	this->SendBuildingInfo(role);
	this->SednBuildingOpenerInfo(role);

	gamelog::g_log_guild.printf(LL_INFO, "role[%d %s] join guild[%d,%s] ", this->GetGuildID(), this->GetGuildName(),
		role->GetUID(), role->GetName());

	return true;
}

void Guild::Init(const AllGuildParam::GuildParam &guild_param, const GuildGatherDreamParam * guild_dream_param /* = NULL */, bool is_sync_from_origin /* = false */)
{
	if (m_base_param.guild_fight_title_id != guild_param.base_param.guild_fight_title_id)
	{
		m_base_param.guild_fight_title_id = guild_param.base_param.guild_fight_title_id;
		m_guild_member_manager.UpdateGuildFightTitle();
	}
	int old_guild_level = m_base_param.guild_level;
	m_base_param = guild_param.base_param;
	m_gwt_param = guild_param.gwt_param;
	m_boss_param = guild_param.guild_boss_param;

	if (0 == m_boss_param.reset_flag)
	{
		m_boss_param.reset_flag = 1;
		this->ResetBossDataOnVersionChange();
	}

	// 处理首日上传更新代码时七日活跃的数值容错
	m_base_param.seven_day_active[GUILD_SEVEN_DAY_ACTIVE - 1] = m_base_param.day_total_guild_active_val;

	if (NULL != guild_dream_param)
	{
		m_gather_dream_param = *guild_dream_param;
		for (int i = 0; i < m_gather_dream_param.count && i < GUILD_MAX_MEMBER_COUNT; i++)
		{
			m_gather_dream_map[m_gather_dream_param.data_list[i].role_id] = i;
		}
	}

	m_guild_member_manager.Init(guild_param.member_param, is_sync_from_origin);
	this->InitWeekActivePersonRank();	// 公会成员数据加载完以后 ,根据公会成员信息初始化周活跃个人排行榜信息
	m_guild_boss_damage_rank = m_guild_member_manager.GetMemberGuildBossItem();
	std::sort(m_guild_boss_damage_rank.begin(), m_guild_boss_damage_rank.end());
	m_next_save_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (is_sync_from_origin && old_guild_level != m_base_param.guild_level)
	{
		m_guild_member_manager.OnGuildLevelUp(m_base_param.guild_level);
	}

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		m_base_param.origin_plat_type = LocalConfig::Instance().GetPlatType();
		m_base_param.origin_server_id = LocalConfig::Instance().GetMergeServerId();
	}
	this->RefreshContributionRank(false);
	this->InitHallOfFameRank();
//	this->LoadJoinSpeedUpMember();
}

void Guild::GetInitParam(AllGuildParam::GuildParam* out_param, bool is_save)
{
	if (is_save)
	{
		m_guild_member_manager.GetChangeMemberList(&out_param->member_param);
	}
	else
	{
		m_guild_member_manager.GetMemberList(&out_param->member_param);
	}
	out_param->base_param = m_base_param;
	out_param->gwt_param = m_gwt_param;
	out_param->guild_boss_param = m_boss_param;
}

void Guild::GetInitGatherDreamParam(GuildGatherDreamParam * out_param)
{
	*out_param = m_gather_dream_param;
}

void Guild::Save()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	static AllGuildParam all_guild_param;
	all_guild_param.count = 1;
	static AllGuildGathersDreamParam all_guild_gather_dream_param;
	all_guild_gather_dream_param.count = 1;

	AllGuildParam::GuildParam & guild_param = all_guild_param.guild_list[0];
	this->GetInitParam(&guild_param, true);
	GuildGatherDreamParam & guild_gather_dream_param = all_guild_gather_dream_param.guild_gather_dream_list[0];
	this->GetInitGatherDreamParam(&guild_gather_dream_param);

	RMISaveGuildBackObjectImpl *impl = new RMISaveGuildBackObjectImpl();

	RMIGlobalClient ac;
	ac.__bind_session(InternalComm::Instance().GetDBHandler().GetGlobalDB());
	if (ac.SaveGuildAsyn(m_base_param.guild_id, all_guild_param, all_guild_gather_dream_param, impl))
	{
		m_guild_member_manager.ClearDirtyMark();
	}
}

void Guild::OnLoginBroadcast(Role * role,bool is_gm)
{
	if (role == NULL)
	{
		return;
	}

	time_t now = EngineAdapter::Instance().Time();
	CommonDataParam & data = role->GetRoleModuleManager()->GetCommonData();
	int rank = GuildHonorBattleManager::Instance().GetMatchRank(this->GetGuildID());

	if (is_gm)
	{
		rank = 1;
	}

	if (is_gm || (1 <= rank && rank <= 3 && now >= data.guild_honor_login_broadcast_time + 3 * 3600))
	{
		data.guild_honor_login_broadcast_time = (unsigned int)now;
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_guild_honor_winner_login_broadcast,
				      rank, this->GetName(), role->GetName());
		if (length > 0)
		{
			Protocol::SCSystemMsg sm;
			int sendlen = 0;
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT, SYS_MSG_ROLLING_NOTICE)))
			{
				World::GetInstWorld()->SendToALLGameAllGateway((const char *)&sm, sendlen);
			}
		}
	}
}

void Guild::OnRoleLogin(Role * role)
{
	GuildMember * member = m_guild_member_manager.GetMember(role->GetUserId());
	if (NULL == member) return;

	member->OnRoleLogin(role);
	role->SetGuildPostion(member->GetPost());
	this->SendGuildBaseInfo(role);

	// 家族任务
	this->SendGuildWeekTaskInfo(role);
	this->SendContrRankListInfo(role);

	if (member->GetPost() == GUILD_POST_PATRIARCH)
	{
		this->OnLoginBroadcast(role);
	}

	// 家族活跃度信息
	this->SendGuildActiveInfo(role);
	role->SendGuildActiveInfo();
	this->SendQifuInfo(role);

	//家族内政建设信息
	this->SendAllBuildInfo(role);
	this->SendSpeedUpInfo(role);
	if (this->IsCanSpeedUp())
	{
		this->SendBuildingInfo(role);		
	}
	this->SednBuildingOpenerInfo(role);
	this->GuildTaskSetFinishTimes(role);
}

void Guild::OnRoleLogout(Role * role)
{
	GuildMember * member = m_guild_member_manager.GetMember(role->GetUserId());
	if (NULL == member) return;
	member->OnRoleLogout(role);
}

void Guild::OnRoleLevelUp(Role * role, int level)
{
	GuildMember * member = m_guild_member_manager.GetMember(role->GetUserId());
	if (NULL == member) return;

	member->SetLevel(level);
	if (GUILD_POST_PATRIARCH == member->GetPost())
	{
		m_guild_member_manager.SetPatriarchLevel(level);
	}
	m_guild_member_manager.OnMemberInfoChange(role->GetUserId());
}

bool Guild::IsMember(int uid)
{
	if (NULL != m_guild_member_manager.GetMember(IntToUid(uid)))
	{
		return true;
	}
	return false;
}

void Guild::Update(unsigned long interval, time_t now_second)
{
	if (now_second >= m_next_save_time)
	{
		m_next_save_time = static_cast<unsigned int>(now_second) + RandomNum(GUILD_SAVE_INTERVAL, GUILD_SAVE_INTERVAL + 10);
		this->Save();
	}

	if (now_second >= m_gwt_param.next_refresh_task_timestamp)		// 刷新家族任务
	{
		this->IssueDayContributionsAward();
		this->RefreshGuildWeekTask();
	}

	if (now_second >= m_gwt_param.next_awarding_timestamp)			// 进行周结算
	{
		this->IssueGuildWeekTaskAward();
		this->IssueWeekContributionsAward();
		this->ClearGuildWeekTaskRank();
	}

	if (m_need_refresh_gwt_contribution_rank && now_second >= m_next_gwt_contribution_rank_refresh_timestamp)		// 下发贡献度排行榜
	{
		m_need_refresh_gwt_contribution_rank = false;
		m_next_gwt_contribution_rank_refresh_timestamp = now_second + 60;		// 间隔 60s 刷新一次
		this->RefreshContributionRank();	// 如果表是空的，那就不用发了
		this->SendGuildWeekTaskInfo();
	}

	// 家族建筑建造完成
	if (m_base_param.single_building_finish_timestamp != 0 && now_second >= m_base_param.single_building_finish_timestamp)
	{
		this->BuildingFinish();
	}

	m_guild_member_manager.Update(now_second);

	if (m_guild_boss_damage_rank_need_update && now_second >= m_guild_boss_damage_rank_next_update_time)
	{
		m_guild_boss_damage_rank_need_update = false;
		m_guild_boss_damage_rank_next_update_time = now_second + 60;
		m_guild_boss_damage_rank = m_guild_member_manager.GetMemberGuildBossItem();
		std::sort(m_guild_boss_damage_rank.begin(), m_guild_boss_damage_rank.end());
	}


	this->UpdateTanHe(now_second);

	if (now_second > m_next_guild_sync_cross_time && this->GetDirty() && !CrossConfig::Instance().IsHiddenServer())
	{
		this->SetDirty(false);
		m_next_guild_sync_cross_time = now_second + RandomNum(50, 70);

		crossgameprotocal::GameHiddenSyncGuildChange msg;
		msg.guild_id = this->GetGuildID();
		msg.plat_type = LocalConfig::Instance().GetPlatType();
		msg.op_type = crossgameprotocal::GameHiddenSyncGuildChange::OP_TYPE_UPDATE;
		this->GetInitParam(&msg.param, false);
		InternalComm::Instance().SendSerializeMsgToHiddenThroughCross(&msg);
	}

	if (m_battle_refresh_tamp > 0 && m_battle_refresh_tamp <= now_second)
	{
		std::sort(m_battle_rank.begin(), m_battle_rank.end(), FallOfFameRankSort);
		m_battle_refresh_tamp = 0;
	}
	if (m_rich_refresh_tamp > 0 && m_rich_refresh_tamp <= now_second)
	{
		std::sort(m_rich_rank.begin(), m_rich_rank.end(), FallOfFameRankSort);
		m_rich_refresh_tamp = 0;
	}
	if (m_kinder_refresh_tamp > 0 && m_kinder_refresh_tamp <= now_second)
	{
		std::sort(m_kinder_rank.begin(), m_kinder_rank.end(), FallOfFameRankSort);
		m_kinder_refresh_tamp = 0;
	}
}

bool Guild::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (CrossConfig::Instance().IsHiddenServer() || old_dayid == now_dayid)
	{
		return true;
	}
	bool is_dirty = false;

	// 七日活跃
	memmove(m_base_param.seven_day_active, m_base_param.seven_day_active + 1, sizeof(m_base_param.seven_day_active[0]) * (GUILD_SEVEN_DAY_ACTIVE - 1));
	m_base_param.seven_day_active[GUILD_SEVEN_DAY_ACTIVE - 1] = 0;

	m_base_param.qifu_value = 0;
	m_base_param.is_open_qifu = 0;

	time_t now_sec = EngineAdapter::Instance().Time();
	if (now_sec >= m_base_param.protect_timestamp)	// 保护期内不会扣维护金，也不会被降级，只有超过保护期，才会扣维护金和降级
	{
		const GuildRoomCfg * cfg = LOGIC_CONFIG->GetGuildConfig()->GetRoomCfg(m_base_param.guild_level);
		if (NULL != cfg)
		{
			const GuildOtherCfg* other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg();
			if (NULL != other_cfg)
			{
				int cost = 0;
				for (int i = GUILD_BUILD_TYPE_HALL; i < GUILD_BUILD_TYPE_MAX; ++i)
				{
					cost += m_base_param.build_level[i];
				}
				cost = cost * other_cfg->serve_build_capital + cfg->cost;

				gamelog::g_log_guild.printf(LL_INFO, "guild[%d,%s] guild_exp:%lld level:%d cost:%d,other_cfg->serve_build_capital:%d cfg->cost:%d", this->GetGuildID(), this->GetGuildName(),
					m_base_param.guild_exp, m_base_param.guild_level, cost, other_cfg->serve_build_capital, cfg->cost);

				m_base_param.guild_exp -= cost;		// 维护
	
			}
		}

		if (m_base_param.guild_exp <= 0)		// 如果家族资金小于等于 0，则降级或解散；
		{
			if (m_base_param.guild_level > 1)		// 家族等级大于 1，降级
			{
				--m_base_param.guild_level;			// 降级

				// 返还家族资金
				const GuildOtherCfg & other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfgRef();
				m_base_param.protect_timestamp = static_cast<unsigned int>(now_sec) + other_cfg.protect_time;

				const GuildRoomCfg * room_cfg = LOGIC_CONFIG->GetGuildConfig()->GetRoomCfg(m_base_param.guild_level);
				if (NULL != room_cfg)
				{
					int return_fund = (int)(other_cfg.fund_return_rate * room_cfg->exp / 100.0F);

					gamelog::g_log_guild.printf(LL_INFO, "guild[%d,%s] guild_exp:%lld level:%d return_fund:%d", this->GetGuildID(), this->GetGuildName(),
						m_base_param.guild_exp, m_base_param.guild_level, return_fund);

					this->AddExp(return_fund);
				}

				// 邮件通知降级
				static MailContentParam mail;
				int lensub = SNPRINTF(mail.subject, sizeof(mail.subject), gamestring::g_guild_demotion_subject);
				int length = SNPRINTF(mail.contenttxt, sizeof(mail.contenttxt), gamestring::g_guild_demotion_content, this->GetName(), m_base_param.guild_level);
				if (lensub > 0 && length > 0) m_guild_member_manager.SendMailToAllMember(MAIL_REASON_DEFAULT, mail);

				is_dirty = true;

				//家族降级刷新排行榜
				// 公会等级排行榜
				RankManager::Instance().GetGuildRank()->OnGuildRankInfoChange(this, GUILD_RANK_TYPE_LEVEL);
			}
			else		// 否则返回假，解散
			{
				return false;		// 返回假说明要解散；
			}
		}
	}

	//祈福
	if (m_base_param.is_open_qifu == 0)
	{
		const GuildOtherCfg*  other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg();
		if (NULL != other_cfg)
		{
			if (m_base_param.guild_level >= other_cfg->bless_open_level && m_base_param.guild_exp >= other_cfg->bless_open_fund && m_base_param.guild_exp >= other_cfg->bless_consume)
			{
				m_base_param.guild_exp -= other_cfg->bless_consume;
				m_base_param.is_open_qifu = 1;
			}
		}
	}

	//家族集愿屋
	{
		//一些还未领取的集愿,通过邮件发放
		for (int i = 0; i < m_gather_dream_param.count && i < GUILD_MAX_MEMBER_COUNT; i++)
		{
			int count = m_gather_dream_param.data_list[i].gather_num - m_gather_dream_param.data_list[i].fetch_num;
			if (count <= 0) continue;

			static MailContentParam contentparam; contentparam.Reset();
			contentparam.item_list[0].item_id = m_gather_dream_param.data_list[i].item_id;
			contentparam.item_list[0].num = count;
			contentparam.item_list[0].is_bind = true;

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_gather_dream_daychange_reward_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_gather_dream_daychange_reward_content);
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(m_gather_dream_param.data_list[i].role_id, MAIL_REASON_DEFAULT, contentparam);
			}
		}

		m_gather_dream_param.DayReset();
		m_gather_dream_map.clear();
		is_dirty = true;
	}

	{
		// 家族BOSS
		this->SendRankReward(m_boss_param.chapter_id, old_dayid);
		m_guild_boss_damage_rank.clear();
		this->ResetBossDataByDay();
		
		is_dirty = true;
	}

	{	//家族建造相关
		m_base_param.is_send_will_overflow_mail = 0;
		m_base_param.is_send_overflow_mail = 0;
		m_base_param.quicken_building_count = 0;
		m_base_param.building_count = 0;

		this->SendSpeedUpInfo();
		this->SendAllBuildInfo();

		is_dirty = true;
	}

	// 家族日活跃结算
	this->GuildActiveDayActiveSettlement();

	m_guild_member_manager.OnDayChange(old_dayid, now_dayid);

	

	if (is_dirty)
	{
		this->SetDirty(true);
	}

	this->SendGuildBossInfo();

	return true;
}

void Guild::OnWeekChange()
{
	m_base_param.guild_fight_rank = 0;

	this->ResetBossDataByWeek();

	// 家族周活跃个人排行榜结算
	this->GuildActiveWeekActivePersonRankSettlement();
	// 重置家族活跃度记录
	m_base_param.week_total_guild_active_val = 0;
	m_base_param.last_guild_active_change_times = (unsigned int)EngineAdapter::Instance().Time();
	this->SendGuildActiveInfo(NULL);

	//重置名人堂
	this->WeekRankReward();
	m_battle_rank.clear();
	m_battle_refresh_tamp = 0;
	m_rich_rank.clear();
	m_rich_refresh_tamp = 0;
	m_kinder_rank.clear();
	m_kinder_refresh_tamp = 0;
	this->SendQifuInfo();
	m_guild_member_manager.OnWeekChange();
}

bool Guild::OnResetData(unsigned int old_dayid, unsigned int now_dayid)
{
	if (CrossConfig::Instance().IsHiddenServer() || old_dayid == now_dayid)
	{
		return false;
	}

	bool is_dirty = false;

	if (0 != m_base_param.today_set_banner_flag)
	{
		m_base_param.today_set_banner_flag = 0;
		is_dirty = true;
	}

	if (is_dirty)
	{
		this->SetDirty(is_dirty);
	}

	return true;
}

void Guild::GetName2(GuildName & dst)
{
	memcpy(dst, m_base_param.guild_name, sizeof(dst));
}

void Guild::GetPatriarchName(GameName & dst)
{
	m_guild_member_manager.GetPatriarchName(dst);
}

int Guild::AddGongXian(const UserID& user_id, int value, const char* reason)
{
	//	int ret = this->GetMemberManager()->AddGongXian(user_id, value, reason);
	//	if (0 == ret)
	{
		Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(user_id);
		if (NULL != role)
		{
			role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(value, MONEY_TYPE_GUILD_CONTRIBUTION, reason);
		}
		else
		{
			OfflineRegisterManager::Instance().OnAddGuildGongXian(UidToInt(user_id), value, reason);
		}
	}

	return 0;
}

void Guild::OnAddGongXian(const UserID & user_id, int value, const char * reason)
{
	m_guild_member_manager.AddGongXian(user_id, value, reason);
}

int Guild::SetNotice(Role * role, GuildNotice notice)
{
	if (NULL == role) return -1;
	memcpy(m_base_param.guild_notice, notice, sizeof(GuildNotice));

	static Protocol::SCSendGuildNotic info;
	info.guild_id = m_base_param.guild_id;
	this->GetGuildNotice(info.notice);

	m_guild_member_manager.SendInfoToAllMember((const char*)&info, sizeof(info));

	this->SetDirty(true);
	return 0;
}

int Guild::AddExp(int value)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		long long &exp = m_base_param.guild_exp;

		const GuildBaseConstructCfg * g_construct_cfg = LOGIC_CONFIG->GetGuildConfig()->GetConstructCfg(GUILD_BUILD_TYPE_VAULT, m_base_param.build_level[GUILD_BUILD_TYPE_VAULT]);
		if (NULL == g_construct_cfg) return -1;

		if (exp < g_construct_cfg->paramet_1)
		{
			exp = ((exp + value) >= g_construct_cfg->paramet_1) ? g_construct_cfg->paramet_1 : (exp + value);
		}
		else  //本身就超过限制最大值不做处理,也不给增加了
		{
		}
	}
	else
	{
		crossgameprotocal::HiddenGameChangeGuildInfo msg;
		msg.change_type = crossgameprotocal::HIDDEN_GAME_GUILD_CHANGE_TYPE_ADD_EXP;
		msg.guild_id = this->GetGuildID();
		msg.param = value;

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(msg.guild_id).db_index, (const char *)&msg, sizeof(msg));
		return 0;
	}

	do
	{
		const GuildBaseConstructCfg * gbc_cfg = LOGIC_CONFIG->GetGuildConfig()->GetConstructCfg(GUILD_BUILD_TYPE_VAULT, m_base_param.build_level[GUILD_BUILD_TYPE_VAULT]);
		if (NULL != gbc_cfg)
		{
			if (m_base_param.guild_exp >= (gbc_cfg->paramet_1 * 90 / 100))
			{
				bool is_overflow = m_base_param.guild_exp >= gbc_cfg->paramet_1 ? true : false;
				if ((is_overflow && m_base_param.is_send_overflow_mail == 1) || (!is_overflow && m_base_param.is_send_will_overflow_mail == 1)) break;

				std::vector<GuildMember*> t_member_list = m_guild_member_manager.GetAllGuildMember();
				if (!t_member_list.empty())
				{
					for (std::vector<GuildMember*>::const_iterator it_member = t_member_list.begin(); it_member != t_member_list.end(); ++it_member)
					{
						const GuildMemberTypeCfg *member_type_cfg = LOGIC_CONFIG->GetGuildConfig()->GetMemberTypeCfg((*it_member)->GetPost());
						if (NULL != member_type_cfg && member_type_cfg->build_limit == 1)
						{
							static MailContentParam exp_overflow_mail;
							exp_overflow_mail.Reset();
							int lensub = SNPRINTF(exp_overflow_mail.subject, sizeof(exp_overflow_mail.subject), gamestring::g_guild_exp_remind_subject);
							int length = SNPRINTF(exp_overflow_mail.contenttxt, sizeof(exp_overflow_mail.contenttxt),
								is_overflow ? gamestring::g_guild_exp_remind_overflow_content : gamestring::g_guild_exp_remind_will_overflow_content, m_base_param.guild_name);
							if (lensub > 0 && length > 0) MailRoute::MailToUser((*it_member)->GetUid(), MAIL_REASON_DEFAULT, exp_overflow_mail);

							if (is_overflow) m_base_param.is_send_overflow_mail = 1;	//今日发送邮件标识符
							else m_base_param.is_send_will_overflow_mail = 1;
						}
					}
				}
			}
		}
	} while(0);

	this->SetDirty(true);

	return 0;
}

int Guild::LevelUp(Role * role)
{
	if (NULL == role) return -100;

	const GuildRoomCfg * next_cfg = LOGIC_CONFIG->GetGuildConfig()->GetRoomCfg(m_base_param.guild_level + 1);
	if (NULL == next_cfg)
	{
		role->NoticeNum(errornum::EN_GUILD_LEVEL_ALREADY_MAX);
		return -3;
	}

	const GuildRoomCfg * cfg = LOGIC_CONFIG->GetGuildConfig()->GetRoomCfg(m_base_param.guild_level);
	if (NULL == cfg) return -1;

	if (m_base_param.guild_exp < cfg->exp)
	{
		role->NoticeNum(errornum::EN_GUILD_EXP_NOT_ENOUTH);
		return -2;
	}

	m_base_param.guild_exp -= cfg->exp;
	++m_base_param.guild_level;

	if (next_cfg->hearsay == 1)
	{
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_guild_up_level, this->GetName(), this->GetGuildLevel());
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT));
		}
	}

	// 升级后，设置保护时间
	const GuildOtherCfg & other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfgRef();
	m_base_param.protect_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time()) + other_cfg.protect_time;

	const GuildJournalCfg * journal_cfg = LOGIC_CONFIG->GetGuildConfig()->GetJournalCfg(GUILD_EVENTHANDLER_TYPE_GUILD_LEVEL_UP);
	if (NULL != journal_cfg && m_base_param.guild_level >= journal_cfg->condition)
	{
		this->AddEventhandler(NULL, EngineAdapter::Instance().Time(), GUILD_EVENTHANDLER_TYPE_GUILD_LEVEL_UP, m_base_param.guild_level);
	}
	this->SendGuildBaseInfo(role);

	m_guild_member_manager.OnGuildLevelUp(m_base_param.guild_level);
	// 公会等级排行榜
	RankManager::Instance().GetGuildRank()->OnGuildRankInfoChange(this, GUILD_RANK_TYPE_LEVEL);

	this->SetDirty(true);
	return 0;
}

int Guild::AddEventhandler(const GameName name, time_t now_time, int type, int param, const ItemGridData *item_grid)
{
	//static GuildEventhandlerParam eventhandler_node; eventhandler_node.Reset();
	//
	//eventhandler_node.eventhandler_time = (unsigned int)now_time;
	//eventhandler_node.eventhandler_type = type * GUILD_EVENTHANDLER_PARAM + param;
	//F_STRNCPY(eventhandler_node.eventhandler_name, name, sizeof(GameName));
	//if (NULL != item_grid)
	//{
	//	const ItemBase * item_base = ITEMPOOL->GetItem(item_grid->item_id);
	//	if (NULL != item_base)
	//	{
	//		InfoTypeReserve & item_info = eventhandler_node.item_info;
	//		item_info.item_id = item_grid->item_id;
	//		item_info.is_bind = item_grid->is_bind;
	//		item_info.column = 0;
	//		item_info.index = 0;
	//		item_info.num = item_grid->num;
	//		item_info.has_param = false;
	//		item_info.invalid_time = item_grid->invalid_time;
	//		if (NULL != item_grid->param)
	//		{
	//			item_info.has_param = true;
	//			item_info.param_length = item_grid->param->GetDataByItemType(item_base->GetItemType(), item_info.param_data);
	//		}
	//	}
	//}

	//if (CrossConfig::Instance().IsHiddenServer())
	//{
	//	//跨服中无法加入与离开家族，此时进入该接口是从本服立马同步过来，所以不需要在转发回原服
	//	if (type == GUILD_EVENTHANDLER_ADD_MEMBER || type == GUILD_EVENTHANDLER_DEC_MEMBER) return 0;
	//	crossgameprotocal::HiddenGameGuildEventhandlerSend hgges;
	//	hgges.guild_id = this->GetGuildID();
	//	hgges.eventhandler_node = eventhandler_node;
	//
	//	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(hgges.guild_id).db_index, (const char*)&hgges, sizeof(hgges));
	//
	//	return 0;
	//}
	//
	//m_eventhandler_list.push_back(eventhandler_node);
	//
	//if ((int)m_eventhandler_list.size() > GUILD_EVENTHANDLER_COUNT)
	//{
	//	m_eventhandler_list.erase(m_eventhandler_list.begin());
	//}
	//this->SendGuildEventInfo();
	//this->SetDirty(true);
	//
	//gamelog::g_log_guild.printf(LL_INFO, "Guild::AddEventhandler guild[%d,%s] user_name[%s] F_STRNCPY_name[%s] type[%d] param[%d] now_time[%u]",
	//	this->GetGuildID(), this->GetGuildName(), name, eventhandler_node.eventhandler_name, type, param, static_cast<unsigned int>(now_time));
	//
	//
	if ((GUILD_EVENTHANDLER_APPOINTMENT == type && GUILD_POST_PATRIARCH == param) ||
		GUILD_EVENTHANDLER_ADD_MEMBER == type ||
		GUILD_EVENTHANDLER_DEC_MEMBER == type)
	{
		// 出现了更换族长的事件 或添加成员 或 减少成员  需要刷新一下排行榜内的数据
		RankManager::Instance().GetGuildRank()->OnBaseInfoChange(this);
	}
	
	return 0;
}

int Guild::AddEventhandler(GuildEventhandlerParam * eventhandler_node)
{
	//if (NULL == eventhandler_node) return -1;
	//int eventhandler_type = eventhandler_node->eventhandler_type / GUILD_EVENTHANDLER_PARAM;
	//int param = eventhandler_node->eventhandler_type % GUILD_EVENTHANDLER_PARAM;
	//if (eventhandler_type <= 0 || eventhandler_type >= GUILD_EVENTHANDLER_TYPE_MAX) return -2;
	//
	//m_eventhandler_list.push_back(*eventhandler_node);
	//
	//if ((int)m_eventhandler_list.size() > GUILD_EVENTHANDLER_COUNT)
	//{
	//	m_eventhandler_list.erase(m_eventhandler_list.begin());
	//}
	//
	//this->SendGuildEventInfo();
	//this->SetDirty(true);
	//
	//gamelog::g_log_guild.printf(LL_INFO, "Guild::AddEventhandler guild[%d,%s]  F_STRNCPY_name[%s] type[%d] param[%d] now_time[%u]",
	//	this->GetGuildID(), this->GetGuildName(), eventhandler_node->eventhandler_name, eventhandler_type, param, static_cast<unsigned int>(eventhandler_node->eventhandler_time));
	//
	return 0;
}

void Guild::GuildRename(GuildName guild_name)
{
	SNPRINTF(m_base_param.guild_name, sizeof(m_base_param.guild_name) - 1, "%s", guild_name);
	GuildBossChapterRank info;
	this->GetGuildBossChapterInfo(&info);
	m_guild_manager->UpdateGuildBossChapterRank(&info);

	// 家族改名刷新排行榜内数据
	RankManager::Instance().GetGuildRank()->OnBaseInfoChange(this);
}

void Guild::GuildSetBanner(GuildBanner guild_banner)
{
	m_base_param.today_set_banner_flag = 1;
	SNPRINTF(m_base_param.guild_banner, sizeof(m_base_param.guild_banner) - 1, "%s", guild_banner);
	GuildBossChapterRank info;
	this->GetGuildBossChapterInfo(&info);
	m_guild_manager->UpdateGuildBossChapterRank(&info);

	// 家族族徽修改 刷新排行榜内数据
	RankManager::Instance().GetGuildRank()->OnBaseInfoChange(this);
}

int Guild::GetMaxMemberCount() const
{
	int count = 50;
	const GuildRoomCfg * cfg = LOGIC_CONFIG->GetGuildConfig()->GetRoomCfg(m_base_param.guild_level);
	if (NULL != cfg)
	{
		const GuildBaseConstructCfg * gc_cfg = LOGIC_CONFIG->GetGuildConfig()->GetConstructCfg(GUILD_BUILD_TYPE_GUEST_ROOM, m_base_param.build_level[GUILD_BUILD_TYPE_GUEST_ROOM]);
		if (NULL != gc_cfg)
		{
			count = cfg->max_member + gc_cfg->paramet_1;
		}		
	}

	return count;
}

int Guild::GetEmptyMemberNum() const
{
	int max_count = this->GetMaxMemberCount();
	return max_count > m_guild_member_manager.GetMemberCount() ? max_count - m_guild_member_manager.GetMemberCount() : 0;
}

bool Guild::GetGuildName(GuildName guild_name)
{
	if (NULL == guild_name) return false;
	memcpy(guild_name, m_base_param.guild_name, sizeof(GuildName));
	return true;
}

const char * Guild::GetGuildNamePtr()
{
	return m_base_param.guild_name;
}

bool Guild::GetGuildNotice(GuildNotice guild_notice)
{
	if (NULL == guild_notice) return false;
	memcpy(guild_notice, m_base_param.guild_notice, sizeof(GuildNotice));
	return true;
}

void Guild::OnFightGuildBoss(Role* role, int chapter_id, int  boss_seq, short* remain_hp_pecent_list, bool is_win, long long total_damage)
{
	if (NULL == role) return;

	bool need_update_rank = false;
	m_guild_member_manager.UpdateGuildBossHighestDamage(role->GetUID(), total_damage, &need_update_rank);
	if (need_update_rank)
	{
		m_guild_boss_damage_rank_need_update = true;
	}

	if (0 == role->GetRoleModuleManager()->GetGuildCommonData().guild_boss_participate_reward_fetch_flag)
	{
		role->GetRoleModuleManager()->GetGuildCommonData().guild_boss_participate_reward_fetch_flag = 1;
		
		const GuildBossCfg::GuildBossCfgItem * cfg = LOGIC_CONFIG->GetGuildConfig()->GetBossCfg(m_boss_param.chapter_id);
		if (NULL != cfg)
		{
			this->AddExp(cfg->family_coin);
			role->GetRoleModuleManager()->GetMoney()->AddCoinBind(cfg->join_coin, __FUNCTION__);
			role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(cfg->join_gongxian, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__);
			role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(cfg->immortal, MONEY_TYPE_SILVER_COIN, __FUNCTION__);
		}
	}

	this->SendGuildBossInfo(role);

	this->SetDirty(true);

	// 立刻排序立刻下发
	if (m_guild_boss_damage_rank_need_update)
	{
		m_guild_boss_damage_rank = m_guild_member_manager.GetMemberGuildBossItem();
		std::sort(m_guild_boss_damage_rank.begin(), m_guild_boss_damage_rank.end());
		this->SendGuildBossRank(role);
	}
}

void Guild::ForceStopGuildBossFight(int chapter_id, int boss_seq)
{
	battlegameprotocol::GameBattleForceFinishBattleModeReq req;
	req.battle_mode = BATTLE_MODE_GUILD_BOSS;
	req.param1 = this->GetGuildID();
	req.param2 = boss_seq;
	req.param3 = chapter_id;

	World::GetInstWorld()->SendMsgToOneBattle(USE_BATTLE_SERVER_INDEX, (const char*)&req, sizeof(req));
}

bool GuildBossKillRewardFilterFunc(GuildMember *guild_member, void *func_param)
{
	int boss_index = *(int *)func_param;

	if (guild_member->GetMemberData().guild_boss_join_flag & (1 << boss_index))
	{
		return true;
	}

	return false;
}

void Guild::GameToHiddenSyncGuildMemberData(int role_id)
{
	m_guild_member_manager.GameToHiddenSyncGuildMemberData(role_id);
}

void Guild::SendGuildBaseInfo(Role * role)
{
	if (NULL == role) return;

	static Protocol::SCGuildBaseInfo info;
	info.patriarch_last_login_time = 0;
	info.patriarch_tan_he_start_time = 0;
	info.is_join_tan_he_patriarch = 0;
	info.can_tan_he_patriarch = 0;

	m_guild_member_manager.GetPatriarchName(info.patriarch_name);

	GuildMember * patriarch_member = m_guild_member_manager.GetPatriarchMember();
	if (patriarch_member != NULL)
	{
		info.patriarch_last_login_time = patriarch_member->GetLastLogintime();
	}

	if (UserCacheManager::Instance().IsUserOnline(m_guild_member_manager.GetPatriarchUid()))
	{
		info.patriarch_last_login_time = (unsigned int)EngineAdapter::Instance().Time();
	}

	info.patriarch_tan_he_start_time = m_base_param.tan_he_zu_zhang_start_time;
	info.can_tan_he_patriarch = m_base_param.can_tan_he;


	F_STRNCPY(info.guild_name, m_base_param.guild_name, sizeof(GuildName));
	F_STRNCPY(info.guild_notice, m_base_param.guild_notice, sizeof(GuildNotice));
	F_STRNCPY(info.guild_banner, m_base_param.guild_banner, sizeof(GuildBanner));
	info.guild_id = m_base_param.guild_id;
	info.level = m_base_param.guild_level;
	info.exp = m_base_param.guild_exp;
	info.create_time = (unsigned int)m_base_param.create_timestamp;
	info.join_time = 0;
	info.is_need_approval = m_base_param.is_need_approval;
	info.guild_buff_rank = m_base_param.guild_fight_buff_rank;
	GuildMember *member = m_guild_member_manager.GetMember(role->GetUserId());
	if (NULL != member)
	{
		info.join_time = (unsigned int)member->GetJoinTime();
		info.is_join_tan_he_patriarch = member->GetSetTanHeJoinFlag();
	}

	EngineAdapter::Instance().NetSendSerializeMsg(role->GetNetId(), &info);
}

void Guild::SendGuildMemberInfo(Role * role)
{
	m_guild_member_manager.SendGuildMemberInfo(role);
}

void Guild::SendGuildMemberInfoChange(int change_role_id)
{
	m_guild_member_manager.SendGuildMemberInfoChange(change_role_id);
}

void Guild::SendGuildNeedApproveInfo(Role * role)
{
	Protocol::SCGuildNeedApproveInfo info;
	info.is_need_approve = m_base_param.is_need_approval;

	if (role != NULL)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));
	}
	else
	{
		m_guild_member_manager.SendInfoToAllMember((const char *)&info, sizeof(info));
	}
}

void Guild::SendGuildEventInfo()
{
	//Protocol::SCGuildEventInfo info;
	//info.count = 0;
	//if ((int)m_eventhandler_list.size() > GUILD_EVENTHANDLER_COUNT)
	//{
	//	for (std::list<GuildEventhandlerParam>::const_reverse_iterator it = m_eventhandler_list.rbegin(), end = m_eventhandler_list.rend();
	//		info.count < GUILD_EVENTHANDLER_COUNT && end != it; ++it)
	//	{
	//		info.count++;
	//		info.eventhandler_list[GUILD_EVENTHANDLER_COUNT - info.count] = *it;
	//	}
	//}
	//else
	//{
	//	for (std::list<GuildEventhandlerParam>::const_iterator it = m_eventhandler_list.begin(), end = m_eventhandler_list.end();
	//		info.count < GUILD_EVENTHANDLER_COUNT && end != it; ++it)
	//	{
	//		info.eventhandler_list[info.count++] = *it;
	//	}
	//}
	//m_guild_member_manager.SendSerializeMsgToAllMember(&info, sizeof(info));
}

void Guild::GmGuildHandler(int op_type, int param)
{
	enum GuildHandlerOperatorType
	{
		GUILD_RESET_PROTECT_TIME = 0,		// 重置家族降级保护时间
		GUILD_PRINT_PROTECT_TIME = 1,		// 输出家族降级保护时间
		GUILD_SET_WEEK_TASK_PROG = 2,		// 设置家族周任务的进度
		GUILD_NOW_WEEK_TASK_DONE = 3,		// 立刻结算周任务的奖励
		GUILD_SET_WEEK_TASK_TIME = 4,		// 设置周任务完成的次数
		GUILD_NOW_TASK_WEEK_SETT = 5,		// 周任务立刻结算周奖励
	};

	long long now_sec = (long long)EngineAdapter::Instance().Time();
	switch (op_type)
	{
	case GUILD_RESET_PROTECT_TIME:
		m_base_param.protect_timestamp = param;
		break;
	case GUILD_PRINT_PROTECT_TIME:
		printf("\nPrint Protect Time --> GuildID: %d | ProtectTime: %u\n", m_base_param.guild_id, m_base_param.protect_timestamp);
		{
			Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(param));
			if (NULL != role)
			{
				char date_time[2048] = { 0 };
				char out_str[2048] = { 0 };

				sprintf(out_str, "The family's[%s] demoted protection expires on", m_base_param.guild_name);

				const time_t protect_timestamp = m_base_param.protect_timestamp;
				strftime(date_time, 64, " %Y-%m-%d %H:%M:%S", localtime(&protect_timestamp));

				strcat(out_str, date_time);
				int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), out_str);
				if (length <= 0) return;

				Protocol::SCSystemMsg sm;
				int sendlen = 0;
				if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
				{
					role->GetRoleModuleManager()->NetSend((const void *)&sm, sendlen);
				}
			}
		}
		break;
	case GUILD_SET_WEEK_TASK_PROG:
		m_gwt_param.cur_gwt_task_prog = param;
		m_need_refresh_gwt_contribution_rank = true;
		break;
	case GUILD_NOW_WEEK_TASK_DONE:
		m_gwt_param.next_refresh_task_timestamp = now_sec;
		break;
	case GUILD_SET_WEEK_TASK_TIME:
		if (param < 0 || param > 5) param = 5;		// 最大是 5;
		m_gwt_param.cur_finish_times = param;
		this->SendGuildWeekTaskInfo();
		break;
	case GUILD_NOW_TASK_WEEK_SETT:
		m_gwt_param.next_awarding_timestamp = now_sec;
		break;
	}
}

void Guild::GmTanHe()
{
	if (m_gm_flag >= 1)
	{
		if (m_base_param.tan_he_zu_zhang_start_time == 0 || m_base_param.can_tan_he == 0)
		{
			return;
		}
	}

	m_gm_flag += 1;
}

void Guild::UpdateTanHe(time_t now_second)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	const GuildOtherCfg & other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfgRef();
	time_t patriarch_last_login_time = this->GetPatriarchLastLoginTime();

	if (patriarch_last_login_time == 0)
	{
		m_gm_flag = 0;
	}

	if (m_gm_flag > 0 && patriarch_last_login_time != 0)
	{
		patriarch_last_login_time = 1;
	}

	char old = m_base_param.can_tan_he;
	if (patriarch_last_login_time != 0 && now_second >= patriarch_last_login_time + other_cfg.tan_he_zu_zhang_logout_time_length)
	{
		m_base_param.can_tan_he = 1;
		if (old != m_base_param.can_tan_he)
		{
			std::vector<GuildMember * > guild_member_arr = m_guild_member_manager.GetAllGuildMember();
			for (int i = 0; i < (int)guild_member_arr.size(); ++i)
			{
				GuildMember * curr = guild_member_arr[i];
				curr->SetTanHeJoinFlag(0);
			}

			this->OnGuildBaseInfoChange();

			static MailContentParam contentparam; contentparam.Reset();

			int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::guild_tan_he_notice_title);
			int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::guild_tan_he_notice_content);
			if (length1 > 0 && length2 > 0)
			{
				m_guild_member_manager.SendMailToAllMember(MAIL_REASON_DEFAULT, contentparam, MAIL_TYPE_SYSTEM, FilterZuZhang);
			}
		}
	}
	else
	{
		m_base_param.can_tan_he = 0;
		m_base_param.tan_he_zu_zhang_start_time = 0;
		if (old != m_base_param.can_tan_he)
		{
			this->OnGuildBaseInfoChange();
		}
	}

	if (m_gm_flag > 1 && m_base_param.can_tan_he != 0 && m_base_param.tan_he_zu_zhang_start_time != 0)
	{
		m_gm_flag = 0;
		this->OnTanHeZuZhang();
	}

	if (m_base_param.can_tan_he != 0 && m_base_param.tan_he_zu_zhang_start_time != 0
		&& now_second >= (time_t)m_base_param.tan_he_zu_zhang_start_time + other_cfg.tan_he_zu_zhang_continued_time)
	{
		this->OnTanHeZuZhang();
	}

}

int Guild::TanHeZuZhang(Role * role)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		role->NoticeNum(errornum::EN_OPERATE_FORBID_IN_CROSS);
		return -__LINE__;
	}

	if (m_base_param.can_tan_he == 0)
	{
		return -__LINE__;
	}

	GuildMember * member = m_guild_member_manager.GetMember(role->GetUserId());
	if (member == NULL)
	{
		return -__LINE__;
	}

	member->SetTanHeJoinFlag(1);

	if (m_base_param.tan_he_zu_zhang_start_time == 0)
	{
		m_base_param.tan_he_zu_zhang_start_time = (unsigned int)EngineAdapter::Instance().Time();
		this->OnGuildBaseInfoChange();
	}
	else
	{
		this->SendGuildBaseInfo(role);
	}

	return 0;
}

void Guild::OnTanHeZuZhang()
{
	m_base_param.can_tan_he = 0;
	m_base_param.tan_he_zu_zhang_start_time = 0;

	GuildMember * win_member = NULL;
	GuildMember * old_patriarch = m_guild_member_manager.GetPatriarchMember();

	std::vector<GuildMember * > guild_member_arr = m_guild_member_manager.GetAllGuildMember();
	for (int i = 0; i < (int)guild_member_arr.size(); ++i)
	{
		GuildMember * curr = guild_member_arr[i];
		if (curr->GetSetTanHeJoinFlag() == 0)continue;
		curr->SetTanHeJoinFlag(0);
		if (win_member == NULL)
		{
			win_member = curr;
		}

		if (curr->GetTotalGongXian() > win_member->GetTotalGongXian())
		{
			win_member = curr;
		}
	}

	if (win_member != old_patriarch)
	{
		if (old_patriarch != NULL)
			old_patriarch->SetPost(GUILD_POST_MEMBER);
		if (win_member != NULL)
		{
			win_member->SetPost(GUILD_POST_PATRIARCH);

			static MailContentParam contentparam; contentparam.Reset();

			int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::guild_tan_he_succ_title);
			int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::guild_tan_he_succ_content, win_member->GetUid(), win_member->GetName());
			if (length1 > 0 && length2 > 0)
			{
				m_guild_member_manager.SendMailToAllMember(MAIL_REASON_DEFAULT, contentparam, MAIL_TYPE_SYSTEM);
			}
		}
	}

	this->OnGuildBaseInfoChange();
	this->Save();
}

void Guild::OnGuildBaseInfoChange()
{
	std::vector<GuildMember * > guild_member_arr = m_guild_member_manager.GetAllGuildMember();
	for (int i = 0; i < (int)guild_member_arr.size(); ++i)
	{
		Role * temp_role = World::GetInstWorld()->GetSceneManager()->GetRole(guild_member_arr[i]->GetUid());
		if (temp_role != NULL)
		{
			this->SendGuildBaseInfo(temp_role);
		}
	}

}

void Guild::SetGuildFightTitle(int title_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameChangeGuildInfo msg;
		msg.change_type = crossgameprotocal::HIDDEN_GAME_GUILD_CHANGE_TYPE_SET_GUILD_FIGHT_TITLE;
		msg.guild_id = this->GetGuildID();
		msg.param = title_id;

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(msg.guild_id).db_index, (const char *)&msg, sizeof(msg));
	}
	else
	{
		if (m_base_param.guild_fight_title_id != title_id)
		{
			m_base_param.guild_fight_title_id = title_id;
			m_guild_member_manager.UpdateGuildFightTitle();
		}
		this->SetDirty(true);
		m_next_guild_sync_cross_time = EngineAdapter::Instance().Time();
	}
}

int Guild::GetGuildFightTitle()
{
	return m_base_param.guild_fight_title_id;
}

void Guild::BuildingBuild(Role * role, int build_type, int op_type)
{
	if (NULL == role) return;

	if (build_type <= 0 || build_type >= GUILD_BUILD_TYPE_MAX) return;

	bool is_hall = false;
	if (build_type == GUILD_BUILD_TYPE_HALL)
	{
		is_hall = true;
	}

	if (m_base_param.build_level[0] >= GUILD_BUILD_TYPE_MAX || m_base_param.build_level[0] < 0)
	{
		m_base_param.build_level[0] = 0;
	}

	if (m_base_param.build_level[0] != 0)	//判断是否在建造中
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_GUILD_BUILDING_IS_BEING_BUILD);
		return;
	}

	if (op_type == CSGuildConstructReq::OP_TYPE_BUILDING)
	{
		// 判断是否已达到等级上限
		if (m_base_param.build_level[build_type] >= LOGIC_CONFIG->GetGuildConfig()->GetMaxBuildLevel(is_hall))
		{
			role->GetRoleModuleManager()->NoticeNum(errornum::EN_GUILD_MAX_BUILD_LEVEL);
			return;
		}
		
		if (!is_hall)	// 判断大厅等级够不够
		{
			const GuildBaseConstructCfg* hall_cfg = LOGIC_CONFIG->GetGuildConfig()->GetConstructCfg(GUILD_BUILD_TYPE_HALL, m_base_param.build_level[GUILD_BUILD_TYPE_HALL]);
			if (NULL == hall_cfg) return;

			if (m_base_param.build_level[build_type] >= hall_cfg->paramet_1)
			{
				role->GetRoleModuleManager()->NoticeNum(errornum::EN_GUILD_LIMIT_BUILD_LEVEL);
				return;
			}
		}

		const GuildOtherCfg* other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg();
		if (NULL == other_cfg) return;

		if (m_base_param.building_count >= other_cfg->building_limit)
		{
			role->GetRoleModuleManager()->NoticeNum(errornum::EN_GUILD_LAST_BUILDING_TIMES);
			return;
		}

		GuildMember* member = m_guild_member_manager.GetMember(role->GetUID());
		if (NULL == member) return;

		//判断家族资金是否足够升级
		const GuildBaseConstructCfg* build_cfg = LOGIC_CONFIG->GetGuildConfig()->GetConstructCfg(build_type, m_base_param.build_level[build_type]);
		if (NULL == build_cfg) return;

		if (m_base_param.guild_exp < build_cfg->up_expend_capital)
		{
			role->GetRoleModuleManager()->NoticeNum(errornum::EN_GUILD_EXP_NOT_ENOUTH);
			return;
		}

		m_base_param.guild_exp -= build_cfg->up_expend_capital;
		m_base_param.single_building_finish_timestamp = EngineAdapter::Instance().Time() + build_cfg->up_expend_time;
		m_base_param.build_level[0] = (short)build_type;
		m_base_param.building_count += 1;

		if (m_base_param.quicken_building_count < other_cfg->speed_times)
		{
			m_base_param.quicken_building_count++;
			m_base_param.can_speed_up = 1;

			{	// 全员发一条漂字协议
				Protocol::SCGuildUseBuildMaterialInfo msg; msg.Reset();
				msg.op_type = SCGuildUseBuildMaterialInfo::OP_TYPE_OPEN_SUP;
				msg.uid = member->GetPost();			// op_type = OP_TYPE_OPEN_SUP 时,表示职位,tnnd不想再加字段了
				memcpy(msg.name, role->GetName(), sizeof(GameName));
				msg.build_type = build_type;
				msg.item_id = 0;
				msg.num = 0;
				m_guild_member_manager.SendInfoToAllMember((const char*)&msg, sizeof(msg));
				this->OnSyncBuildingNotifyInfo(role, msg);			// 通知跨服漂字
			}

			{	// 保存开启人信息,并发一条开启人信息的协议
				memcpy(m_base_param.opener_name, role->GetName(), sizeof(GameName));
				m_base_param.opener_post = member->GetPost();

				this->SednBuildingOpenerInfo();
			}

			gamelog::g_log_guild.printf(LL_INFO, "start build speed up, cur_open_num[%d]", m_base_param.quicken_building_count);
			this->SendSpeedUpInfo();		
		}

		this->SendBuildingInfo();
		this->SendGuildBaseInfo(role);
		this->OnGameHiddenSynBuildAllInfo();
		this->SetDirty(true);

		gamelog::g_log_guild.printf(LL_INFO, "%s guild[%d,%s] user[%d] building_type[%d] original_building_finish_timestamp[%d] use_exp[%d] lave_exp[%lld]", __FUNCTION__,
			this->GetGuildID(), this->GetGuildName(), role->GetUID(), build_type, m_base_param.single_building_finish_timestamp , build_cfg->up_expend_capital, m_base_param.guild_exp);
	}
	else if(op_type == CSGuildConstructReq::OP_TYPE_DESTROY)
	{
		if (m_base_param.build_level[build_type] == 0)
		{
			role->GetRoleModuleManager()->NoticeNum(errornum::EN_UNKNOWN_ERROR);
			return;
		}

		int ret = true;
		switch (build_type)
		{
			case GUILD_BUILD_TYPE_HALL:
				ret = this->DismantleHall(role);
				break;
			case GUILD_BUILD_TYPE_STUDY:
				m_base_param.build_level[build_type] -= 1;
				break;
			case GUILD_BUILD_TYPE_GUEST_ROOM:
				ret = this->DismantleGuestRoom(role);
				break;
			case GUILD_BUILD_TYPE_VAULT:
				m_base_param.build_level[build_type] -= 1;
				break;
			case GUILD_BUILD_TYPE_STORE:
				m_base_param.build_level[build_type] -= 1;
				break;
		}
		if (!ret) return;

		this->SendBuildInfo(build_type);
		this->OnGameHiddenSynBuildAllInfo();
		this->SetDirty(true);
		gamelog::g_log_guild.printf(LL_INFO, "%s guild[%d,%s] user[%d] demolished building build_type[%d] build_level[%d] ", __FUNCTION__,
			this->GetGuildID(), this->GetGuildName(), role->GetUID(), build_type, m_base_param.build_level[build_type]);
	}

	return;
}

void Guild::BuildingFinish()
{
	int build_type = m_base_param.build_level[0];	//[0]下标存的是当前正在建造的建筑类型

	if (this->IsCanSpeedUp())		//在can_speed_up置零前判断是否有加速
	{
		Protocol::SCGuildUseBuildMaterialInfo finish_info; finish_info.Reset();
		finish_info.op_type = SCGuildUseBuildMaterialInfo::OP_TYPE_SUP_FINISH;
		finish_info.build_type = build_type;
		finish_info.uid = 0;
		memset(finish_info.name, 0, sizeof(GameName));
		finish_info.item_id = 0;
		finish_info.num = 0;

		m_guild_member_manager.SendInfoToAllMember((const char *)&finish_info, sizeof(finish_info));
	}

	// 初始化
	m_base_param.build_level[build_type] += 1;
	m_base_param.build_level[0] = 0;		//建造完成后初始化
	m_base_param.single_building_finish_timestamp = 0;
	m_base_param.can_speed_up = 0;
	memset(m_base_param.opener_name, 0, sizeof(GameName));
	m_base_param.opener_post = 0;

	Protocol::SCGuildBuildingFinishInfo msg;
	msg.build_finish_info.build_type = build_type;
	msg.build_finish_info.build_level = m_base_param.build_level[build_type];

//	m_building_speed_up_list.clear();
	this->SendBuildingInfo();
	this->SendSpeedUpInfo();
	this->OnGameHiddenSynBuildAllInfo();
	
	m_guild_member_manager.SendInfoToAllMember((const char *)&msg, sizeof(msg));

	gamelog::g_log_guild.printf(LL_INFO, "%s guild[%d,%s]  build_type[%d] build_level_up[%d]", __FUNCTION__,
		this->GetGuildID(), this->GetGuildName(), build_type, m_base_param.build_level[build_type]);
	this->SetDirty(true);

	if (CrossConfig::Instance().IsHiddenServer()) return;		//邮件只发原服

	//给族长/副族长发邮件
	std::vector<GuildMember*> t_member_list = m_guild_member_manager.GetAllGuildMember();
	if (!t_member_list.empty())
	{
		for (std::vector<GuildMember*>::const_iterator it_member = t_member_list.begin(); it_member != t_member_list.end(); ++it_member)
		{
			if ((*it_member)->GetPost() == GUILD_POST_PATRIARCH) //发给族长
			{
				static MailContentParam build_leader_mail;
				build_leader_mail.Reset();
				int lensub = SNPRINTF(build_leader_mail.subject, sizeof(build_leader_mail.subject), gamestring::g_guild_building_finish_subject);
				int length = SNPRINTF(build_leader_mail.contenttxt, sizeof(build_leader_mail.contenttxt), gamestring::g_guild_building_finish_leader_content, m_base_param.guild_name, build_type);
				if (lensub > 0 && length > 0) MailRoute::MailToUser((*it_member)->GetUid(), MAIL_REASON_DEFAULT, build_leader_mail);			
			}
			else if((*it_member)->GetPost() == GUILD_POST_VICE_PATRIARCH)	//发给副族长
			{
				static MailContentParam build_deputyleader_mail;
				build_deputyleader_mail.Reset();
				int lensub = SNPRINTF(build_deputyleader_mail.subject, sizeof(build_deputyleader_mail.subject), gamestring::g_guild_building_finish_subject);
				int length = SNPRINTF(build_deputyleader_mail.contenttxt, sizeof(build_deputyleader_mail.contenttxt), gamestring::g_guild_building_finish_deputyleader_content, m_base_param.guild_name, build_type);
				if (lensub > 0 && length > 0) MailRoute::MailToUser((*it_member)->GetUid(), MAIL_REASON_DEFAULT, build_deputyleader_mail);
			}
		}
	}
}

bool Guild::DismantleHall(Role * role)
{
	if (m_base_param.single_building_finish_timestamp != 0)
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_GUILD_OTHER_BUILD_BUILDING);
		return false;
	}
	
	const GuildBaseConstructCfg * hall_cfg = LOGIC_CONFIG->GetGuildConfig()->GetConstructCfg(GUILD_BUILD_TYPE_HALL, m_base_param.build_level[GUILD_BUILD_TYPE_HALL] - 1);
	if (NULL == hall_cfg) return false;

	const GuildBaseConstructCfg * guestroom_cfg = LOGIC_CONFIG->GetGuildConfig()->GetConstructCfg(GUILD_BUILD_TYPE_GUEST_ROOM, hall_cfg->paramet_1);
	if (NULL == guestroom_cfg) return false;

	const GuildRoomCfg * cfg = LOGIC_CONFIG->GetGuildConfig()->GetRoomCfg(m_base_param.guild_level);
	if (NULL == cfg) return false;

	if (m_guild_member_manager.GetMemberCount() > cfg->max_member + guestroom_cfg->paramet_1)
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_GUILD_MEMBER_EXCEED);
		return false;
	}
	
	m_base_param.build_level[GUILD_BUILD_TYPE_HALL] -= 1;
	for (int i = GUILD_BUILD_TYPE_HALL + 1; i < GUILD_BUILD_TYPE_MAX; ++i)
	{
		if (m_base_param.build_level[i] > hall_cfg->paramet_1)
		{
			m_base_param.build_level[i] = hall_cfg->paramet_1;
			this->SendBuildInfo(i);
		}
	}

	this->SetDirty(true);
	return true;
}

bool Guild::DismantleGuestRoom(Role * role)
{
	const GuildRoomCfg * cfg = LOGIC_CONFIG->GetGuildConfig()->GetRoomCfg(m_base_param.guild_level);
	if (NULL == cfg) return false;

	//获取降级后的建筑参数
	const GuildBaseConstructCfg * gc_cfg = LOGIC_CONFIG->GetGuildConfig()->GetConstructCfg(GUILD_BUILD_TYPE_GUEST_ROOM, m_base_param.build_level[GUILD_BUILD_TYPE_GUEST_ROOM] - 1);
	if (NULL == gc_cfg) return false;

	int count = cfg->max_member + gc_cfg->paramet_1;
		
	if (m_guild_member_manager.GetMemberCount() > count)
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_GUILD_MEMBER_EXCEED);
		return false;
	}

	m_base_param.build_level[GUILD_BUILD_TYPE_GUEST_ROOM] -= 1;
	return true;
}

void Guild::SendBuildInfo(int build_type, Role* role)
{
	Protocol::SCGuildConstructInfo msg;
	msg.build_info.build_type = build_type;
	msg.build_info.build_level = m_base_param.build_level[build_type];

	if (NULL != role)
	{
		role->GetRoleModuleManager()->NetSend((const void *)&msg, sizeof(msg));
	}
	else
	{
		m_guild_member_manager.SendInfoToAllMember((const char *)&msg, sizeof(msg));
	}
}

void Guild::SendAllBuildInfo(Role * role)
{
	Protocol::SCGuildConstructAllInfo msg;

	for (int i = GUILD_BUILD_TYPE_HALL; i < GUILD_BUILD_TYPE_MAX; ++i)
	{
		msg.build_list[i-1].build_type = i;
		msg.build_list[i-1].build_level = m_base_param.build_level[i];
	}
	msg.building_build = m_base_param.build_level[0];
	msg.building_finish_timestamp = m_base_param.single_building_finish_timestamp;
	msg.building_count = m_base_param.building_count;

	if (NULL != role)
	{
		role->GetRoleModuleManager()->NetSend(&msg, sizeof(msg));
	}
	else
	{
		m_guild_member_manager.SendInfoToAllMember((const char *)&msg, sizeof(msg));
	}
}

void Guild::SendAllUseBulidMaterialInfo(Role * role, int item_id, int num)
{
	if (NULL == role) return;

	Protocol::SCGuildUseBuildMaterialInfo msg;
	msg.Reset();

	msg.op_type = SCGuildUseBuildMaterialInfo::OP_TYPE_USE_ITEM;
	memcpy(msg.name, role->GetName(), sizeof(GameName));
	msg.item_id = item_id;
	msg.num = num;
	msg.uid = role->GetUID();
	msg.build_type = 0;

	m_guild_member_manager.SendInfoToAllMember((const char *)&msg, sizeof(msg));
	this->OnSyncBuildingNotifyInfo(role, msg);
}

void Guild::SendSpeedUpInfo(Role * role)
{
	Protocol::SCGuildSpeedUpInfo msg;
	msg.quicken_building_count = m_base_param.quicken_building_count;
	msg.can_speed_up = m_base_param.can_speed_up;
	
	if (NULL != role)
	{
		GuildMember * member = m_guild_member_manager.GetMember(role->GetUID());
		if (NULL == member) return;

		msg.is_cur_building_help = role->GetRoleModuleManager()->GetCommonData().guild_build_join_speed_up_count;
		role->GetRoleModuleManager()->NetSend(&msg, sizeof(msg));
	}
	else
	{
		m_guild_member_manager.SendSPUPInfoToAllMember(msg);
	}
}

void Guild::SendBuildingInfo(Role * role)
{
	Protocol::SCGuildConstructTimeStampInfo msg;
	msg.building_build = m_base_param.build_level[0];
	msg.building_finish_timestamp = m_base_param.single_building_finish_timestamp;
	msg.building_count = m_base_param.building_count;

	if (NULL != role)
	{
		role->GetRoleModuleManager()->NetSend(&msg, sizeof(msg));
	}
	else
	{
		m_guild_member_manager.SendInfoToAllMember((const char *)&msg, sizeof(msg));
	}
}

void Guild::SednBuildingOpenerInfo(Role * role)
{
	Protocol::SCGuildConstructOpenSUPInfo msg;
	memcpy(msg.name, m_base_param.opener_name, sizeof(GameName));
	msg.open_post = m_base_param.opener_post;

	if (NULL != role)
	{
		role->GetRoleModuleManager()->NetSend((const void *)&msg, sizeof(msg));
	}
	else
	{
		m_guild_member_manager.SendInfoToAllMember((const char *)&msg, sizeof(msg));
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		gamelog::g_log_guild.printf(LL_INFO, "%s post[%d] name[%s]", __FUNCTION__, msg.open_post, msg.name);
	}
}

void Guild::OnGameHiddenSynSpeedUp()
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenSynGuildSpeedUpInfo syn_gh;
	syn_gh.guild_id = m_base_param.guild_id;
	syn_gh.single_building_finish_timestamp = m_base_param.single_building_finish_timestamp;

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&syn_gh, sizeof(syn_gh));
}

void Guild::OnGameHiddenRecviceSpeedUp(crossgameprotocal::GameHiddenSynGuildSpeedUpInfo* msg)
{
	if (msg == NULL) return;

	Role* role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->user_id);
	if (NULL == role) return;

	const GuildOtherCfg* other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg();
	if (other_cfg == NULL) return;

	if (msg->ret < 0)
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	else if (msg->ret == 1)
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_GUILD_SPEED_UP_FINISH);
	}
	role->GetRoleModuleManager()->NoticeNum(errornum::EN_GUILD_SPEED_UP_SUCCESS);

	if (NULL != role)
	{	//参与加速的家族聊天框显示
		Protocol::SCGuildUseBuildMaterialInfo msg; msg.Reset();
		msg.op_type = SCGuildUseBuildMaterialInfo::OP_TYPE_MEMBER_SUP;
		msg.uid = role->GetUID();
		memcpy(msg.name, role->GetName(), sizeof(GameName));
		msg.build_type = m_base_param.build_level[0];
		msg.item_id = 0;
		msg.num = 0;

		m_guild_member_manager.SendInfoToAllMember((const char*)&msg, sizeof(msg));
		this->OnSyncBuildingNotifyInfo(role, msg);
	}

	role->GetRoleModuleManager()->GetMoney()->UseCoinBind(other_cfg->expend_coin, __FUNCTION__);

	m_base_param.single_building_finish_timestamp = msg->single_building_finish_timestamp;

	role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(other_cfg->get_contribution, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__);

	gamelog::g_log_guild.printf(LL_INFO, "%s role[%d, %s] add_gongxian[%d] join building speed up", __FUNCTION__, role->GetUID(), role->GetName(), other_cfg->get_contribution);

	this->SetDirty(true);
	this->SendBuildingInfo();
	this->SendSpeedUpInfo(role);
}

int Guild::OnHiddenGameSynSpeedUp(Role* role)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return -100;

	const GuildOtherCfg* other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg();
	if (NULL == other_cfg) return -101;

	if (role->GetRoleModuleManager()->GetCommonData().guild_build_join_speed_up_count >= other_cfg->person_times)
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_GUILD_CANNOT_REPEAT_SPEED_UP);
		return -102;
	}

	if (!role->GetRoleModuleManager()->GetMoney()->CoinBindMoreThan(other_cfg->expend_coin))
	{
		role->GetRoleModuleManager()->NoticeNum(errornum::EN_GUILD_JOIN_SPEED_UP_BUILDING_LACK_COIN);
		return -103;
	}

	role->GetRoleModuleManager()->GetCommonData().guild_build_join_speed_up_count += 1;

	crossgameprotocal::HiddenGameSynGuildSpeedUpInfo syn_gh;
	syn_gh.guild_id = m_base_param.guild_id;
	syn_gh.user_id = role->GetUID();

	InternalComm::Instance().SendToGameThroughCross2((const char *)&syn_gh, sizeof(syn_gh), IntToUid(role->GetUID()).db_index);
	return 0;
}

void Guild::OnHiddenGameRecviceSpeedUp(crossgameprotocal::HiddenGameSynGuildSpeedUpInfo * msg)
{
	int ret = 0;
	do
	{
		if (msg == NULL && CrossConfig::Instance().IsHiddenServer())
		{
			ret = -2;
			break;
		}

		const GuildOtherCfg* other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg();
		if (other_cfg == NULL)
		{
			ret = -3;
			break;
		}

		unsigned int cur_time = EngineAdapter::Instance().Time();

		if ((m_base_param.single_building_finish_timestamp - other_cfg->speed_up) <= cur_time)
		{
			m_base_param.single_building_finish_timestamp = cur_time;
			ret = 1;
		}
		else
		{
			m_base_param.single_building_finish_timestamp -= other_cfg->speed_up;
		}

		this->SetDirty(true);
		this->SendBuildingInfo();
	} while (0);

	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenSynGuildSpeedUpInfo syn_gh;
	syn_gh.guild_id = m_base_param.guild_id;
	syn_gh.user_id = msg->user_id;
	syn_gh.single_building_finish_timestamp = m_base_param.single_building_finish_timestamp;
	syn_gh.ret = ret;

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&syn_gh, sizeof(syn_gh));

}

void Guild::OnGameHiddenSynBuildAllInfo()
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenSynGuildBuildAllInfo syn_all;
	syn_all.guild_id = m_base_param.guild_id;
	memcpy(syn_all.build_level,m_base_param.build_level,sizeof(syn_all.build_level));
	syn_all.single_building_finish_timestamp = m_base_param.single_building_finish_timestamp;
	syn_all.quicken_building_count = m_base_param.quicken_building_count;
	syn_all.can_speed_up = m_base_param.can_speed_up;
	memcpy(syn_all.opener_name, m_base_param.opener_name, sizeof(syn_all.opener_name));
	syn_all.opener_post = m_base_param.opener_post;

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&syn_all, sizeof(syn_all));
}

void Guild::OnGameHiddenRecviceBuildAllInfo(crossgameprotocal::GameHiddenSynGuildBuildAllInfo * msg)
{
	if (NULL == msg || !CrossConfig::Instance().IsHiddenServer()) return;

	memcpy(m_base_param.build_level, msg->build_level, sizeof(m_base_param.build_level));
	m_base_param.single_building_finish_timestamp = msg->single_building_finish_timestamp;
	m_base_param.quicken_building_count = msg->quicken_building_count;
	m_base_param.can_speed_up = msg->can_speed_up;
	memcpy(m_base_param.opener_name, msg->opener_name, sizeof(m_base_param.opener_name));
	m_base_param.opener_post = msg->opener_post;

	this->SendAllBuildInfo();
	this->SendSpeedUpInfo();
	this->SendBuildingInfo();
	this->SednBuildingOpenerInfo();
	this->SetDirty(true);
}

void Guild::OnSyncBuildingNotifyInfo(Role * role, Protocol::SCGuildUseBuildMaterialInfo & info)
{
	crossgameprotocal::SyncGuildBuildingNotifyInfo notify;
	notify.guild_id = m_base_param.guild_id;
	notify.info = info;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		InternalComm::Instance().SendToGameThroughCrossByRoleId((const char *)&notify, sizeof(notify), role->GetUID());
	}
	else
	{
		InternalComm::Instance().SendToHiddenThroughCross((const char *)&notify, sizeof(notify));
	}
}

void Guild::OnSyncBuildingNotifyRecviceInfo(crossgameprotocal::SyncGuildBuildingNotifyInfo * msg)
{
	if (msg == NULL) return;

	m_guild_member_manager.SendInfoToAllMember((const char *)&msg->info, sizeof(msg->info));
}

void Guild::GuildTaskSetFinishTimes(Role * role)
{
	if (!role)
	{
		return;
	}

	int role_id = role->GetUID();
	int times = role->GetRoleModuleManager()->GetRoleGuildTask()->GetFinishTimes();

	this->GuildTaskSetFinishTimes(role_id,times);
}

void Guild::GuildTaskSetFinishTimes(int role_id, int times)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameChangeGuildInfo msg;
		msg.change_type = crossgameprotocal::HIDDEN_GAME_GUILD_CHANGE_TYPE_ROLE_GUILD_TASK_FINISH;
		msg.guild_id = this->GetGuildID();
		msg.param = role_id;
		msg.param2 = times;
		msg.param3 = 0;
		msg.param4 = 0;

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(msg.guild_id).db_index, (const char *)&msg, sizeof(msg));
		return;
	}

	GuildMember * mem = m_guild_member_manager.GetMember(role_id);
	if (!mem)
	{
		return;
	}

	if (mem->GetMemberData().role_guild_task_finish_times == times)
	{
		return;
	}

	GuildMemberData & data = mem->GetMemberDataAndSetDirty();
	data.role_guild_task_finish_times = times;

	this->GameToHiddenSyncGuildMemberData(role_id);
	this->SendGuildMemberInfoChange(role_id);
}

void Guild::GuildTaskClearSolicit(int role_id, bool is_send)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameChangeGuildInfo msg;
		msg.change_type = crossgameprotocal::HIDDEN_GAME_GUILD_CHANGE_TYPE_ROLE_GUILD_TASK_CLEAR_SOLICIT;
		msg.guild_id = this->GetGuildID();
		msg.param = role_id;
		msg.param2 = is_send;
		msg.param3 = 0;
		msg.param4 = 0;

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(msg.guild_id).db_index, (const char *)&msg, sizeof(msg));

		return;
	}

	GuildMember * member = m_guild_member_manager.GetMember(role_id);
	if (!member)
	{
		return;
	}
	GuildMemberData & data = member->GetMemberDataAndSetDirty();
	data.role_guild_task_is_solicit = 0;
	data.role_guild_task_solicit_task_id = 0;
	data.role_guild_task_solicit_num = 0;
	data.role_guild_task_solicit_fetch_num = 0;

	if (is_send)
	{
		this->GameToHiddenSyncGuildMemberData(role_id);
		this->SendGuildMemberInfoChange(role_id);
	}
}

bool Guild::GuildTaskSolicit(int role_id, int task_id, int task_index)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameChangeGuildInfo msg;
		msg.change_type = crossgameprotocal::HIDDEN_GAME_GUILD_CHANGE_TYPE_ROLE_GUILD_TASK_SOLICIT;
		msg.guild_id = this->GetGuildID();
		msg.param = role_id;
		msg.param2 = task_id;
		msg.param3 = task_index;
		msg.param4 = 0;

		return InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(msg.guild_id).db_index, (const char *)&msg, sizeof(msg));
	}

	GuildMember * member = m_guild_member_manager.GetMember(role_id);
	if (!member)
	{
		return false;
	}
	GuildMemberData & data = member->GetMemberDataAndSetDirty();
	if (data.role_guild_task_solicit_task_id != 0 && data.role_guild_task_solicit_num != 0)
	{
		return false;
	}

	data.role_guild_task_is_solicit = 1;
	data.role_guild_task_solicit_task_id = task_id;
	data.role_guild_task_solicit_num = 0;
	data.role_guild_task_solicit_fetch_num = 0;

	this->GameToHiddenSyncGuildMemberData(role_id);
	this->SendGuildMemberInfoChange(role_id);

	return true;
}

bool Guild::GuildTaskGive(Role * role, int target_role_id)
{
	if (!role)
	{
		return false;
	}

	GuildMember * member = m_guild_member_manager.GetMember(target_role_id);
	if (!member)
	{
		return false;
	}
	GuildMemberData & data = member->GetMemberDataAndSetDirty();

	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(data.role_guild_task_solicit_task_id);
	if (!task_cfg)
	{
		return false;
	}

	if (data.role_guild_task_solicit_num >= task_cfg->param2)
	{
		return false;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->HasItem(task_cfg->param1, 1))
	{
		return false;
	}

	const RoleGuildTaskItemCfg * cfg = LOGIC_CONFIG->GetRoleGuildTaskConfig()->GetRoleGuildTaskItemCfg(data.role_guild_task_solicit_task_id);
	if (!cfg)
	{
		return false;
	}

	const RoleGuildTaskRewardGroupItemCfg * rand_reward_cfg = LOGIC_CONFIG->GetRoleGuildTaskConfig()->RandGuildTaskRewardGroupItemCfg(cfg->give_reward_group_id);
	if (rand_reward_cfg)
	{
		if (!role->GetRoleModuleManager()->GetKnapsack()->PutVec(rand_reward_cfg->rewards, PUT_REASON_GUILD_TASK_GIVE))
		{
			return false;
		}		
	}
	this->AddExp(cfg->give_guild_exp);
	role->GetRoleModuleManager()->GetMoney()->OnAddMoney(MONEY_TYPE_GUILD_CONTRIBUTION, cfg->give_guild_gong_xian, __FUNCTION__);
	role->GetRoleModuleManager()->GetKnapsack()->ConsumeItem(task_cfg->param1, 1, __FUNCTION__);
	this->GuildTaskGiveAddNum(target_role_id, data.role_guild_task_solicit_task_id);

	gamelog::g_log_role_guild_task.printf(LL_INFO, "%s line:%d |%s target_role_id:%d  task_id:%d", __FUNCTION__, __LINE__,
		role->GetRoleModuleManager()->GetRoleInfoString().c_str(), target_role_id, data.role_guild_task_solicit_task_id);

	return true;
}

void Guild::GuildTaskGiveAddNum(int role_id, int task_id)
{
	GuildMember * member = m_guild_member_manager.GetMember(role_id);
	if (!member)
	{
		return;
	}
	GuildMemberData & data = member->GetMemberDataAndSetDirty();
	if (data.role_guild_task_solicit_task_id != task_id)
	{
		return;
	}

	data.role_guild_task_solicit_num += 1;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameChangeGuildInfo msg;
		msg.change_type = crossgameprotocal::HIDDEN_GAME_GUILD_CHANGE_TYPE_ROLE_GUILD_TASK_GIVE;
		msg.guild_id = this->GetGuildID();
		msg.param = role_id;
		msg.param2 = data.role_guild_task_solicit_task_id;
		msg.param3 = 0;
		msg.param4 = 0;

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(msg.guild_id).db_index, (const char *)&msg, sizeof(msg));
	}
	else
	{
		this->GameToHiddenSyncGuildMemberData(role_id);
		this->SendGuildMemberInfoChange(role_id);
	}

	gamelog::g_log_role_guild_task.printf(LL_INFO, "%s line:%d | target_role_id:%d  task_id:%d", __FUNCTION__, __LINE__,
		role_id, task_id);
}

bool Guild::GuildTaskFetch(Role * role)
{
	if (!role)
	{
		return false;
	}

	int role_id = role->GetUID();
	GuildMember * member = m_guild_member_manager.GetMember(role_id);
	if (!member)
	{
		return false;
	}
	GuildMemberData & data = member->GetMemberDataAndSetDirty();

	const TaskInfomaConfig * task_cfg = TASKPOOL->GetTaskCfgByID(data.role_guild_task_solicit_task_id);
	if (!task_cfg)
	{
		return false;
	}

	int fetch_num = data.role_guild_task_solicit_num - data.role_guild_task_solicit_fetch_num;
	if (fetch_num <= 0)
	{
		return false;
	}

	ItemConfigData reward;
	reward.item_id = task_cfg->param1;
	reward.num = fetch_num;
	reward.is_bind = true;

	if (!role->GetRoleModuleManager()->GetKnapsack()->Put(reward, PUT_REASON_GUILD_TASK_FETCH))
	{
		return false;
	}

	this->GuildTaskFetchAddNum(role_id, data.role_guild_task_solicit_task_id, fetch_num);

	gamelog::g_log_role_guild_task.printf(LL_INFO, "%s line:%d |%s  task_id:%d", __FUNCTION__, __LINE__,
		role->GetRoleModuleManager()->GetRoleInfoString().c_str(), data.role_guild_task_solicit_task_id);
	
	return true;
}

bool Guild::GuildTaskFetchAddNum(int role_id, int task_id, int fetch_num)
{
	GuildMember * member = m_guild_member_manager.GetMember(role_id);
	if (!member)
	{
		return false;
	}
	GuildMemberData & data = member->GetMemberDataAndSetDirty();
	if (data.role_guild_task_solicit_task_id != task_id)
	{
		return false;
	}

	data.role_guild_task_solicit_fetch_num += fetch_num;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameChangeGuildInfo msg;
		msg.change_type = crossgameprotocal::HIDDEN_GAME_GUILD_CHANGE_TYPE_ROLE_GUILD_TASK_FETCH;
		msg.guild_id = this->GetGuildID();
		msg.param = role_id;
		msg.param2 = data.role_guild_task_solicit_task_id;
		msg.param3 = fetch_num;
		msg.param4 = 0;

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(msg.guild_id).db_index, (const char *)&msg, sizeof(msg));
	}
	else
	{

		this->SendGuildMemberInfoChange(role_id);
	}

	gamelog::g_log_role_guild_task.printf(LL_INFO, "%s line:%d |role_id:%d task_id:%d fetch_num:%d", __FUNCTION__, __LINE__,
		role_id, data.role_guild_task_solicit_task_id, fetch_num);

	return true;
}

void Guild::GuildTaskMailNotice(int role_id, bool is_all, int target_role_id)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameChangeGuildInfo msg;
		msg.change_type = crossgameprotocal::HIDDEN_GAME_GUILD_CHANGE_TYPE_ROLE_GUILD_TASK_MAIL_NOTICE;
		msg.guild_id = this->GetGuildID();
		msg.param = role_id;
		msg.param2 = is_all;
		msg.param3 = target_role_id;
		msg.param4 = 0;

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(msg.guild_id).db_index, (const char *)&msg, sizeof(msg));
	}

	GuildMember * verify_member = m_guild_member_manager.GetMember(role_id);
	if (!verify_member)
	{
		return;
	}

	// 判断权限
	int my_post = verify_member->GetPost();
	const GuildMemberTypeCfg * member_type_cfg = LOGIC_CONFIG->GetGuildConfig()->GetMemberTypeCfg(my_post);
	if (NULL == member_type_cfg || 0 == member_type_cfg->privilege[GUILD_TASK_MAIL_NOTICE])
	{
		return;
	}

	static MailContentParam contentparam; contentparam.Reset();
	SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_role_guild_task_notice_subject);
	SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_role_guild_task_notice_content);

	if (is_all)
	{
		std::vector<GuildMember *> all_member = m_guild_member_manager.GetAllGuildMember();
		for (int i = 0; i < (int)all_member.size(); ++i)
		{
			GuildMember * curr = all_member[i];
			const GuildMemberData & data_const = curr->GetMemberData();
			if (data_const.role_guild_task_is_mail_notice || curr->GetUid() == role_id)
			{
				continue;
			}

			GuildMemberData & data = curr->GetMemberDataAndSetDirty();
			data.role_guild_task_is_mail_notice = 1;
			MailRoute::MailToUser(curr->GetUid(), MAIL_REASON_DEFAULT, contentparam);

			this->GameToHiddenSyncGuildMemberData(curr->GetUid());
			this->SendGuildMemberInfoChange(curr->GetUid());
		}

	}
	else
	{
		GuildMember * member = m_guild_member_manager.GetMember(target_role_id);
		if (!member)
		{
			return;
		}
		const GuildMemberData & data_const = member->GetMemberData();
		if (data_const.role_guild_task_is_mail_notice)
		{
			return;
		}

		GuildMemberData & data = member->GetMemberDataAndSetDirty();
		data.role_guild_task_is_mail_notice = 1;
		MailRoute::MailToUser(member->GetUid(), MAIL_REASON_DEFAULT, contentparam);

		this->SendGuildMemberInfoChange(target_role_id);
	}
}

// void Guild::LoadJoinSpeedUpMember()
// {
// 	if (m_base_param.can_speed_up == 0) return;
// 
// 	int member_count = this->GetMemberManager()->GetMemberCount();
// 	for (int i = 0; i < member_count; ++i)
// 	{
// 		GuildMember* member = this->GetMemberManager()->GetMemberByIndex(i);
// 		if (NULL == member) continue;
// 
// 		if (member->IsSpeedUp())
// 		{
// 			m_building_speed_up_list.insert(member->GetUid());
// 		}
// 	}
// }

bool Guild::SpeedUpBuilding(Role * role, int speed_time)
{
	unsigned int cur_time = EngineAdapter::Instance().Time();

	bool is_finish = false;
	if ((m_base_param.single_building_finish_timestamp - speed_time) <= cur_time)
	{
		m_base_param.single_building_finish_timestamp = cur_time;
		is_finish = true;
	}
	else
	{
		m_base_param.single_building_finish_timestamp -= speed_time;
	}

	if(NULL != role)
	{	//参与加速的家族聊天框显示
		Protocol::SCGuildUseBuildMaterialInfo msg; msg.Reset();
		msg.op_type = SCGuildUseBuildMaterialInfo::OP_TYPE_MEMBER_SUP;
		msg.uid = role->GetUID();
		memcpy(msg.name, role->GetName(), sizeof(GameName));
		msg.build_type = m_base_param.build_level[0];
		msg.item_id = 0;
		msg.num = 0;

		m_guild_member_manager.SendInfoToAllMember((const char*)&msg, sizeof(msg));
		this->OnSyncBuildingNotifyInfo(role, msg);
	}
	
	this->SendBuildingInfo();
	this->SetDirty(true);
	return is_finish;
}

// bool Guild::IsJoinCurSpeedUp(UserID use_id)
// {
// 	std::set<UserID>::iterator it = m_building_speed_up_list.find(use_id);
// 	if (m_building_speed_up_list.end() != it) return true;
// 
// 	return false;
// }

void Guild::OnGuildWeekTaskEvent(const UserID& user_id, int operate_type, int param, int add_prog_num /* = 1 */)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameChangeGuildInfo msg;
		msg.change_type = crossgameprotocal::HIDDEN_GAME_GUILD_CHANGE_TYPE_WEEK_TASK_EVENT;
		msg.guild_id = this->GetGuildID();
		msg.param = operate_type;
		msg.param2 = param;
		msg.param3 = add_prog_num;
		msg.param4 = UidToInt(user_id);

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(msg.guild_id).db_index, (const char *)&msg, sizeof(msg));

		// 这里可以不return, 可以暂时修改数据给玩家看，然后等待从本服同步数据过来
	}

	if (operate_type <= 0 || operate_type >= GWT_COND_TYPE_MAX) return;

	const GWTTaskInfor * task_cfg = LOGIC_CONFIG->GetGuildWeekTaskConfig()->GetGWTTaskInforCfg(m_gwt_param.cur_gwt_task_id);
	if (NULL == task_cfg || operate_type != task_cfg->task_condition || m_gwt_param.cur_gwt_task_prog >= task_cfg->need_prog) return;

	UNSTD_STATIC_CHECK(7 == GWT_COND_TYPE_MAX)
		switch (operate_type)
		{
		case GWT_COND_KILL_RACE_MONSTER:
		case GWT_COND_ITEM_TYPE_GATHERS:
			if (param != task_cfg->param1) return;
			break;
		}

	GuildMember* member = this->GetMemberManager()->GetMember(user_id);
	if (NULL == member) return;

	m_gwt_param.cur_gwt_task_prog = GetMin(m_gwt_param.cur_gwt_task_prog + add_prog_num, task_cfg->need_prog);
	member->AddWeekTaskContribute(task_cfg->contribution_per_times * add_prog_num);

	m_need_refresh_gwt_contribution_rank = true;
	this->SendGuildWeekTaskInfo(user_id);

	this->SetDirty(true);

	gamelog::g_log_guild.printf(LL_INFO, "OnGuildWeekTaskEvent guild[%d,%s] user[%d] opertype[%d] param[%d] add_num[%d] cur_num[%d]",
		this->GetGuildID(), this->GetGuildName(), UidToInt(user_id), operate_type, param, add_prog_num, m_gwt_param.cur_gwt_task_prog);
}

void Guild::RefreshGuildWeekTask()
{
	const GWTTaskInfor * task_cfg = LOGIC_CONFIG->GetGuildWeekTaskConfig()->GetGWTTaskInforCfg(m_gwt_param.cur_gwt_task_id);
	if (NULL != task_cfg && m_gwt_param.cur_gwt_task_prog >= task_cfg->need_prog)	// 当日任务完成
	{
		++m_gwt_param.cur_finish_times;

		// 如果完成了当日的家族任务，则给予家族资金奖励；
		const GWTOtherInfo & gwt_other = LOGIC_CONFIG->GetGuildWeekTaskConfig()->GetGWTOtherCfg();
		this->AddExp(gwt_other.day_fin_fund_award);
	}

	m_gwt_param.cur_gwt_task_id = 0;
	m_gwt_param.cur_gwt_task_prog = 0;

	const tm * time_clock = EngineAdapter::Instance().LocalTime();
	if (SUNDAY != time_clock->tm_wday && SATURDAY != time_clock->tm_wday)	// 写死：如果今天既不是周六，也不是周日，随机一个任务；
	{
		m_gwt_param.cur_gwt_task_id = LOGIC_CONFIG->GetGuildWeekTaskConfig()->RandGuildWeekTask(m_guild_member_manager.GetPatriarchLevel());
	}

	this->SendGuildWeekTaskInfo();

	const GWTOtherInfo & gwt_other = LOGIC_CONFIG->GetGuildWeekTaskConfig()->GetGWTOtherCfg();
	int tmp_interval = EngineAdapter::Instance().NextDayInterval(gwt_other.daily_refresh_time / 100, gwt_other.daily_refresh_time % 100, 0);
	m_gwt_param.next_refresh_task_timestamp = EngineAdapter::Instance().Time() + tmp_interval;

	this->SetDirty(true);
}

bool WeekTaskRankSort(const Guild::WeekTaskRankInfo& a, const Guild::WeekTaskRankInfo& b)
{
	if (a.contribute != b.contribute)
	{
		return a.contribute > b.contribute;
	}
	else if (a.last_change_time != b.last_change_time)
	{
		return a.last_change_time < b.last_change_time;
	}
	else
	{
		return a.role_id < b.role_id;
	}
}

void Guild::RefreshContributionRank(bool is_notify)
{
	m_gwt_rank_list.clear();

	int member_count = this->GetMemberManager()->GetMemberCount();
	for (int i = 0; i < member_count; ++i)
	{
		GuildMember* member = this->GetMemberManager()->GetMemberByIndex(i);
		if (NULL == member || member->GetWeekTaskContribute() <= 0) continue;

		static WeekTaskRankInfo info;
		info.role_id = member->GetUid();
		info.contribute = member->GetWeekTaskContribute();
		info.last_change_time = member->GetLastTaskContributeChangeTime();
		m_gwt_rank_list.push_back(info);
	}

	std::sort(m_gwt_rank_list.begin(), m_gwt_rank_list.end(), WeekTaskRankSort);

	if (is_notify) this->SendContrRankListInfo();

	this->SetDirty(true);
}

void Guild::ClearGuildWeekTaskRank()
{
	int member_count = this->GetMemberManager()->GetMemberCount();
	for (int i = 0; i < member_count; ++i)
	{
		GuildMember * member = this->GetMemberManager()->GetMemberByIndex(i);
		if (NULL != member)
		{
			member->SetWeekTaskContribute(0); // 重置贡献
		}
	}

	this->RefreshContributionRank();
}

void Guild::IssueGuildWeekTaskAward()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	const GWTWeekAward * gwt_award_cfg = LOGIC_CONFIG->GetGuildWeekTaskConfig()->GetGWTWeekAwardCfg(m_gwt_param.cur_finish_times);
	if (NULL != gwt_award_cfg)
	{
		static MailContentParam contentparam; contentparam.Reset();

		const ItemConfigData & item_ref = gwt_award_cfg->item_award;
		if (0 != item_ref.item_id) contentparam.item_list[0] = ItemDataWrapper(item_ref.item_id, item_ref.num, item_ref.is_bind ? 1 : 0);
		if (gwt_award_cfg->coin_award > 0) contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = gwt_award_cfg->coin_award;
		if (gwt_award_cfg->exp_award > 0) contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = gwt_award_cfg->exp_award;
		if (gwt_award_cfg->guild_contribution > 0) contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GUILDGONGXIAN] = gwt_award_cfg->guild_contribution;

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_week_task_settlement_reward_title);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_week_task_issue_settlement_reward, m_gwt_param.cur_finish_times);
		if (lensub > 0 && length > 0) m_guild_member_manager.SendMailToAllMember(MAIL_REASON_DEFAULT, contentparam);

		//this->AddExp(gwt_award_cfg->guild_fund);
	}

	m_gwt_param.cur_finish_times = 0;
	this->SendGuildWeekTaskInfo();

	const GWTOtherInfo & gwt_other = LOGIC_CONFIG->GetGuildWeekTaskConfig()->GetGWTOtherCfg();
	int tmp_interval = EngineAdapter::Instance().NextWeekInterval(gwt_other.reward_day, gwt_other.reward_time / 100, gwt_other.reward_time % 100, 0);
	m_gwt_param.next_awarding_timestamp = EngineAdapter::Instance().Time() + tmp_interval;

	this->SetDirty(true);
}

void Guild::IssueDayContributionsAward()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	this->RefreshContributionRank(true);

	for (size_t i = 0; i < m_gwt_rank_list.size(); ++i)
	{
		int rank = i + 1;

		WeekTaskRankInfo & info = m_gwt_rank_list[i];
		bool is_send = false;
		static MailContentParam contentparam; contentparam.Reset();
		const ItemConfigData * award_item = LOGIC_CONFIG->GetGuildWeekTaskConfig()->GetContributeAward(info.contribute);

		if (NULL != award_item)
		{
			is_send = true;
			contentparam.item_list[0] = ItemDataWrapper(award_item->item_id, award_item->num, award_item->is_bind ? 1 : 0);
		}
		const GWTDayRankReawardCfg * cfg = LOGIC_CONFIG->GetGuildWeekTaskConfig()->GetGWTDayRankReawardCfg(rank);
		if (cfg != NULL)
		{
			if (cfg->guild_gong_xian > 0)
			{
				is_send = true;
				contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GUILDGONGXIAN] = cfg->guild_gong_xian;
			}
		}

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_week_task_contribute_reward_title);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_week_task_issue_contribute_reward, info.contribute, rank);
		if (is_send && lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(IntToUid(info.role_id), MAIL_REASON_DEFAULT, contentparam);
			gamelog::g_log_guild.printf(LL_INFO, "%s mail succ,rank[%d] role[%d] contribute[%d] last_change_time[%d]", __FUNCTION__, rank, info.role_id, info.contribute, info.last_change_time);
		}
	}

	this->SetDirty(true);
}

void Guild::IssueWeekContributionsAward()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	this->RefreshContributionRank();

	for (size_t i = 0; i < m_gwt_rank_list.size(); ++i)
	{
		int rank = i + 1;
		WeekTaskRankInfo & info = m_gwt_rank_list[i];
		static MailContentParam contentparam; contentparam.Reset();
		const GWTWeekRankReawardCfg * cfg = LOGIC_CONFIG->GetGuildWeekTaskConfig()->GetGWTWeekRankReawardCfg(rank);
		if (cfg == NULL || cfg->reward.empty())
		{
			gamelog::g_log_guild.printf(LL_INFO, "%s mail fail, rank[%d] role[%d] contribute[%d] last_change_time[%d]", __FUNCTION__, rank, info.role_id, info.contribute, info.last_change_time);
			continue;
		}

		for (int i = 0; i < (int)cfg->reward.size() && i < MAX_ATTACHMENT_ITEM_NUM; ++i)
		{
			const ItemConfigData & reward = cfg->reward[i];
			const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);
			if (item_base != NULL)
			{
				contentparam.item_list[i].item_id = reward.item_id;
				contentparam.item_list[i].num = reward.num;
				contentparam.item_list[i].is_bind = (reward.is_bind) ? 1 : 0;
				contentparam.item_list[i].invalid_time = item_base->CalInvalidTime();
			}
		}

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_week_task_settle_contribute_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_week_task_settle_contribute_content, info.contribute, rank);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(IntToUid(info.role_id), MAIL_REASON_DEFAULT, contentparam);
			gamelog::g_log_guild.printf(LL_INFO, "%s mail succ, rank[%d] role[%d] contribute[%d] last_change_time[%d]", __FUNCTION__, rank, info.role_id, info.contribute, info.last_change_time);
		}
	}

	m_gwt_rank_list.clear();
	this->SendContrRankListInfo();

	this->SetDirty(true);
}

void Guild::SendGuildWeekTaskInfo(Role * role)
{
	Protocol::SCGuildWeekTaskAllInfor gwtai;
	gwtai.cur_finish_times = m_gwt_param.cur_finish_times;
	gwtai.cur_task_id = m_gwt_param.cur_gwt_task_id;
	gwtai.cur_task_prog = m_gwt_param.cur_gwt_task_prog;

	if (NULL != role)
	{
		role->GetRoleModuleManager()->NetSend((const void *)&gwtai, sizeof(gwtai));
	}
	else
	{
		m_guild_member_manager.SendInfoToAllMember((const char *)&gwtai, sizeof(gwtai));
	}
}

void Guild::SendGuildWeekTaskInfo(const UserID& user_id)
{
	Protocol::SCGuildWeekTaskAllInfor gwtai;
	gwtai.cur_finish_times = m_gwt_param.cur_finish_times;
	gwtai.cur_task_id = m_gwt_param.cur_gwt_task_id;
	gwtai.cur_task_prog = m_gwt_param.cur_gwt_task_prog;

	World::GetInstWorld()->GetSceneManager()->SendToUser(user_id, &gwtai, sizeof(gwtai));
}

void Guild::SendContrRankListInfo(Role * role)
{
	Protocol::SCGuildWeekTaskRankList gwtrl;
	gwtrl.list_count = 0;
	for (size_t i = 0; i < m_gwt_rank_list.size(); ++i)
	{
		if (gwtrl.list_count >= GUILD_WEEK_TASK_RANK_LIST_NUM_MAX) break;

		WeekTaskRankInfo& info = m_gwt_rank_list[i];

		UserCacheNode * uc = UserCacheManager::Instance().GetUserNode(info.role_id);
		if (NULL == uc) continue;

		Protocol::SCGuildWeekTaskRankList::GWTRankItem & rank_info = gwtrl.rank_list[gwtrl.list_count++];
		uc->GetName(rank_info.name);
		rank_info.avatar_type = uc->avatar_type;
		rank_info.headshot_id = uc->appearance.headshot_id;
		rank_info.profession = uc->GetBaseProfession();
		rank_info.contribution = info.contribute;
	}

	int send_len = sizeof(gwtrl) - (GUILD_WEEK_TASK_RANK_LIST_NUM_MAX - gwtrl.list_count) * sizeof(gwtrl.rank_list[0]);
	if (send_len <= 0) return;

	if (NULL != role)
	{
		role->GetRoleModuleManager()->NetSend((const void *)&gwtrl, send_len);
	}
	else
	{
		m_guild_member_manager.SendInfoToAllMember((const char *)&gwtrl, send_len);
	}
}

void Guild::UpdateGuildHonorTitleId(int rank)
{
	m_guild_member_manager.UpdateGuildHonorTitleId(rank);
}

void Guild::UpdateGuildHonorTitleIdFromHidden()
{
	m_guild_member_manager.UpdateGuildHonorTitleIdFromHidden();
}

bool Guild::GetGuildBossChapterInfo(GuildBossChapterRank * info)
{
	if (NULL == info) return false;

	info->guild_id = m_base_param.guild_id;
	info->chapter_id = m_boss_param.chapter_id;
	info->boss_index = 0;
	info->boss_num = m_boss_param.week_kill_boss_num;
	info->kill_timestamp = m_boss_param.kill_timestamp;
	info->create_guild_timestamp = m_base_param.create_timestamp;
	F_STRNCPY(info->guild_name, m_base_param.guild_name, sizeof(GuildName));
	F_STRNCPY(info->guild_banner, m_base_param.guild_banner, sizeof(GuildBanner));

	return true;
}

void Guild::SendGuildBossRank(Role * role)
{
	Protocol::SCGuildBossRankInfo protocol;
	protocol.count = 0;

	for (int i = 0; i < (int)m_guild_boss_damage_rank.size(); ++i)
	{
		Protocol::SCGuildBossRankInfo::GuildBossRankItem & rank_item = protocol.rank_item[protocol.count];

		rank_item.role_id = m_guild_boss_damage_rank[i].role_id;
		rank_item.total_damage = m_guild_boss_damage_rank[i].guild_boss_total_damage;

		UserCacheNode *user_cache_node = UserCacheManager::Instance().GetUserNode(rank_item.role_id);
		if (NULL != user_cache_node)
		{
			rank_item.role_level = user_cache_node->level;
			rank_item.avatar_type = user_cache_node->avatar_type;
			rank_item.headshot_id = user_cache_node->appearance.headshot_id;
			rank_item.prof_base = user_cache_node->appearance.prof_base;
			user_cache_node->GetName(rank_item.role_name);
			rank_item.top_level = user_cache_node->top_level;
		}

		protocol.count += 1;
	}

	unsigned int send_length = sizeof(protocol) - (GUILD_MAX_MEMBER_COUNT - protocol.count) * sizeof(protocol.rank_item[0]);
	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, send_length);
	}
	else
	{
		m_guild_member_manager.SendInfoToAllMember((const char *)&protocol, send_length);
	}


}

void Guild::SetGuildBossChapter(Role * role, int chapter_id, bool is_auto_select)
{
	if (role == NULL)
	{
		return;
	}

	/*if (this->GetMemberManagerRef().GetPatriarchUserID() != role->GetUID())
	{
		return;
	}*/

	if (!LOGIC_CONFIG->GetGuildConfig()->IsVaildGuildBossChapterId(chapter_id))
	{
		return;
	}

	if (chapter_id > m_boss_param.max_chapter_OBSOLETE)
	{
		return;
	}

	// 判断家族是否存在
	Guild *guild = GuildManager::Instance().GetGuild(role->GetGuildID());
	if (NULL == guild) return;

	// 判断人是否在家族内
	GuildMember *patriarch = guild->GetMemberManager()->GetMember(role->GetUserId());
	if (NULL == patriarch) return;

	if (!is_auto_select)
	{
		// 判断权限
		const GuildMemberTypeCfg *member_type_cfg = LOGIC_CONFIG->GetGuildConfig()->GetMemberTypeCfg(patriarch->GetPost());
		if (NULL == member_type_cfg) return;

		if (member_type_cfg->modify_target != 1)
		{
			role->NoticeNum(errornum::EN_GUILD_INSUFFICIENT_AUTHORITY);
			return;
		}
	}

	m_boss_param.select_chapter_id_OBSOLETE = chapter_id;
	this->SendGuildBossInfo(role);
}

void Guild::SendGuildBossInfo(Role* role)
{
	Protocol::SCGuildBossInfo info;
	info.guild_boss_chapter_id = m_boss_param.chapter_id;
	info.boss_participate_reward_fetch_flag = 0;

	int today_max_times = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfgRef().guild_boss_play_times;
	if (role != NULL)
	{
		const GuildMember * guild_member = m_guild_member_manager.GetMember(role->GetUID());
		if (guild_member != NULL)
		{
			const GuildMemberData & member_data = guild_member->GetMemberData();
			info.boss_participate_reward_fetch_flag = role->GetRoleModuleManager()->GetGuildCommonData().guild_boss_participate_reward_fetch_flag;
			EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&info, sizeof(info));
		}
	}
	else
	{
		std::vector<GuildMember * > guild_member_arr = m_guild_member_manager.GetAllGuildMember();
		for (int i = 0; i < (int)guild_member_arr.size(); ++i)
		{
			Role * temp_role = World::GetInstWorld()->GetSceneManager()->GetRole(guild_member_arr[i]->GetUid());
			if (temp_role != NULL)
			{
				info.boss_participate_reward_fetch_flag = temp_role->GetRoleModuleManager()->GetGuildCommonData().guild_boss_participate_reward_fetch_flag;
				EngineAdapter::Instance().NetSend(temp_role->GetNetId(), (const char*)&info, sizeof(info));
			}
		}
	}
}

void Guild::SendBaoDiReward(int chapter_id, int boss_index)
{
	//m_guild_member_manager.SendGuildBaoDiReward(chapter_id, boss_index);
}

void Guild::SendRankReward(int chapter_id, int day_id)
{
	m_guild_boss_damage_rank = m_guild_member_manager.GetMemberGuildBossItem();
	std::sort(m_guild_boss_damage_rank.begin(), m_guild_boss_damage_rank.end());
	m_guild_boss_damage_rank_next_update_time += 60;
	m_guild_boss_damage_rank_need_update = false;

	for (int i = 0; i < (int)m_guild_boss_damage_rank.size(); i++)
	{
		GuildMember * guild_member = m_guild_member_manager.GetMember(m_guild_boss_damage_rank[i].role_id);
		if (NULL == guild_member) continue;

		gamelog::g_log_guild.printf(LL_INFO, "%s role[%d,%s] guild_name[%s] rank[%d] start", 
			__FUNCTION__, guild_member->GetUid(), guild_member->GetName(), m_base_param.guild_name, i);

		const GuildRankRewardCfg * rank_reward_cfg = LOGIC_CONFIG->GetGuildConfig()->GetRankRewardCfg(chapter_id, i + 1);
		if (NULL == rank_reward_cfg) continue;

		GuildMemberData &  member_data = guild_member->GetMemberDataAndSetDirty();
		if (member_data.is_fetch_boss_chapter_damage_rank_reward != 0)
		{
			gamelog::g_log_guild.printf(LL_INFO, "%s role[%d,%s] guild_name[%s] has_fetch_reward[%d]", 
				__FUNCTION__, guild_member->GetUid(), guild_member->GetName(), m_base_param.guild_name, member_data.is_fetch_boss_chapter_damage_rank_reward);
			continue;
		}

		static MailContentParam contentparam; contentparam.Reset();

		const GuildBossChapterRewardCfg::ItemCfg * reward_cfg = LOGIC_CONFIG->GetGuildConfig()->GetGuildBossChapterRankReward(rank_reward_cfg->reward_group_id);
		if (NULL != reward_cfg)
		{
			for (int i = 0; i < reward_cfg->item_count && i < MAX_GUILD_REWARD_GROUP_ITEM_NUM && i < MAX_ATTACHMENT_ITEM_NUM; i++)
			{
				contentparam.item_list[i].item_id = reward_cfg->reward_list[i].item_id;
				contentparam.item_list[i].num = reward_cfg->reward_list[i].num;
				contentparam.item_list[i].is_bind = (reward_cfg->reward_list[i].is_bind) ? 1 : 0;
			}
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = reward_cfg->coin + rank_reward_cfg->bind_coin;
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GUILDGONGXIAN] = reward_cfg->gongxian;
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_SILVER_COIN] = rank_reward_cfg->immortal;
			this->AddExp(reward_cfg->guild_coin_per_join);
		}
		contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = rank_reward_cfg->exp;
		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_boss_chapter_rank_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_boss_chapter_rank_content, i + 1);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_guild_boss_damage_rank[i].role_id, MAIL_REASON_DEFAULT, contentparam);
		}
		
		gamelog::g_log_guild.printf(LL_INFO, "%s role_uid[%d] guild_name[%s] succ", __FUNCTION__, m_guild_boss_damage_rank[i].role_id, m_base_param.guild_name);

		member_data.is_fetch_boss_chapter_damage_rank_reward = 1;
		Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(guild_member->GetUid());
		if (NULL == role)
		{
			OfflineRegisterManager::Instance().OnGuildBossChapterDamageRankReward(guild_member->GetUid(), EngineAdapter::Instance().DayID());
		}
		else
		{
			CommonDataParam & common_data = role->GetRoleModuleManager()->GetCommonData();
			common_data.is_fetch_boss_chapter_damage_rank_reward = member_data.is_fetch_boss_chapter_damage_rank_reward;
			common_data.fetch_boss_chapter_rank_reward_day_id = day_id;
		}
	}

}

void Guild::OnPublishGatherDream(Role * role, ItemID item_id, int type)
{
	if (NULL == role || type < GUILD_GATHER_DREAM_ITEM_TYPE_PET || type >= GUILD_GATHER_DREAM_ITEM_TYPE_MAX) return;

	if (role->GetRoleModuleManager()->GetRoleGatherDream()->IsTodayPublish())
	{
		role->NoticeNum(errornum::EN_GUILD_GATHER_DREAM_NOT_REPEATABLE);
		return;
	}

	const GuildOtherCfg& other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfgRef();
	if (other_cfg.pray_family_level > m_base_param.guild_level)
	{
		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_guild_gather_dream_level_not_enough, other_cfg.pray_family_level);
		if (length > 0)
		{
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				EngineAdapter::Instance().NetSend(role->GetNetId(), (const char*)&sm, sendlen);
			}
		}
		return;
	}
	int quality = -1;
	if (GUILD_GATHER_DREAM_ITEM_TYPE_PET == type)
	{
		int pet_reward_id = LOGIC_CONFIG->GetPetConfig()->GetPetGroupIdByItemId(item_id);
		int pet_id = LOGIC_CONFIG->GetPetConfig()->GetPetComposeRandPetId(pet_reward_id);
		const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
		if (NULL == pet_cfg)
		{
			role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return;
		}
		quality = pet_cfg->quality;
	}
	else
	{
		quality = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerQualityByItemId(item_id);
	}
	if (quality < 0)
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	const GuildGatherDreamCfg * gather_dream_cfg = LOGIC_CONFIG->GetGuildConfig()->GetGatherDreamCfg(quality, type);
	if (NULL == gather_dream_cfg)
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (gather_dream_cfg->is_can_pray == 0)
	{
		role->NoticeNum(errornum::EN_GUILD_GATHER_DREAM_NOT_CAN_PRAY);
		return;
	}

	static GuildGatherDreamInfo role_gather_dream_info; role_gather_dream_info.Reset();
	role->GetName(role_gather_dream_info.from_name);
	role_gather_dream_info.role_id = role->GetUID();
	role_gather_dream_info.item_id = item_id;
	role_gather_dream_info.headshot_id = role->GetHeadshotID();
	role_gather_dream_info.gather_num = 0;
	role_gather_dream_info.fetch_num = 0;
	role_gather_dream_info.need_max_num = gather_dream_cfg->pray_patch_num;
	role_gather_dream_info.quality = quality;
	role_gather_dream_info.avatar_type = role->GetAvatarType();
	role_gather_dream_info.gather_dream_type = type;
	role_gather_dream_info.pulish_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

	if (CrossConfig::Instance().IsHiddenServer())
	{
		if (!this->IsCanContOp(role))
		{
			return;
		}

		crossgameprotocal::HiddenGameGuildGatherDreamPublishReq hgggdpr;
		hgggdpr.uid = role_gather_dream_info.role_id;
		hgggdpr.is_publish_send = role->GetRoleModuleManager()->GetRoleGatherDream()->IsPublishSend() ? 1 : 0;
		hgggdpr.info = role_gather_dream_info;
		hgggdpr.guild_id = m_base_param.guild_id;

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(hgggdpr.uid).db_index, (const char *)&hgggdpr, sizeof(hgggdpr));
		return;
	}

	if (role->GetRoleModuleManager()->GetRoleGatherDream()->IsPublishSend())
	{
		//发送频道消息
		Protocol::SCChannelChat channel_chat;
		channel_chat.face_chatframe = role->GetRoleModuleManager()->GetFaceScore()->GetFaceEquipSeq(FACE_CHATFRAME);
		channel_chat.face_signature = role->GetRoleModuleManager()->GetFaceScore()->GetFaceEquipSeq(FACE_SIGNATURE);
		channel_chat.channel_type = chatdef::CHANNEL_TYPE_GUILD;
		channel_chat.msg_length = SNPRINTF(channel_chat.content, sizeof(channel_chat.content), gamestring::g_guild_gather_dream_publish, item_id);
		channel_chat.content_type = 0;
		channel_chat.from_uid = role_gather_dream_info.role_id;
		role->GetName(channel_chat.username);
		channel_chat.headshot_id = role_gather_dream_info.headshot_id;
		channel_chat.avatar_type = role_gather_dream_info.avatar_type;
		channel_chat.prof = static_cast<short>(role->GetProfession());
		channel_chat.level = (short)role->GetLevel();
		channel_chat.avatar_timestamp = role->GetAvatarTimestamp();
		channel_chat.top_level = role->GetTopLevel();
		channel_chat.fly_flag = role->GetRoleModuleManager()->GetFlyUpSystem()->GetFlyUpFlag();
		channel_chat.msg_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		channel_chat.msg_type = Protocol::SCChannelChat::CHANNEL_CHAT_MSG_TYPE_BEGIN;

		m_guild_member_manager.SendInfoToAllMemberInfo((const char *)&channel_chat, sizeof(channel_chat));

	}
	role->GetRoleModuleManager()->GetRoleGatherDream()->SetTodayPublishFlag(true);
	role->NoticeNum(noticenum::NT_GUILD_GATHER_DREAM_PUBLISH_SUCC);
	if (m_gather_dream_param.count < 0)
	{
		m_gather_dream_param.count = 0;
	}
	else if (m_gather_dream_param.count >= GUILD_MAX_MEMBER_COUNT)	//默认覆盖最后一位
	{
		m_gather_dream_param.count = GUILD_MAX_MEMBER_COUNT - 1;
	}

	int index = m_gather_dream_map[role_gather_dream_info.role_id] = m_gather_dream_param.count;
	m_gather_dream_param.data_list[m_gather_dream_param.count++] = role_gather_dream_info;

	this->SendGatherDreamSignleInfo(role, &index);
}

void Guild::OnHiddenGamePublishGatherDream(crossgameprotocal::HiddenGameGuildGatherDreamPublishReq * req)
{
	if (NULL == req || CrossConfig::Instance().IsHiddenServer()) return;

	if (m_gather_dream_map.find(req->uid) != m_gather_dream_map.end())
	{
		return;
	}

	int quality = -1;
	if (GUILD_GATHER_DREAM_ITEM_TYPE_PET == req->info.gather_dream_type)
	{
		int pet_reward_id = LOGIC_CONFIG->GetPetConfig()->GetPetGroupIdByItemId(req->info.item_id);
		int pet_id = LOGIC_CONFIG->GetPetConfig()->GetPetComposeRandPetId(pet_reward_id);
		const PetCfg* pet_cfg = LOGIC_CONFIG->GetPetConfig()->GetPetCfg(pet_id);
		if (NULL != pet_cfg)
		{
			quality = pet_cfg->quality;
		}
	}
	else
	{
		quality = LOGIC_CONFIG->GetPartnerConfig()->GetPartnerQualityByItemId(req->info.item_id);
	}
	if (quality < 0)
	{
		this->OnHiddenGamePublishGatherDreamRet(req->is_publish_send, crossgameprotocal::GameHiddenGuildGatherDreamPublishRet::RESULT_TYPE_CONFIG_ERROR, req->uid, req->info.item_id);
		return;
	}

	const GuildGatherDreamCfg * gather_dream_cfg = LOGIC_CONFIG->GetGuildConfig()->GetGatherDreamCfg(quality, req->info.gather_dream_type);
	if (NULL == gather_dream_cfg)
	{
		this->OnHiddenGamePublishGatherDreamRet(req->is_publish_send, crossgameprotocal::GameHiddenGuildGatherDreamPublishRet::RESULT_TYPE_CONFIG_ERROR, req->uid, req->info.item_id);
		return;
	}
	if (gather_dream_cfg->is_can_pray == 0)
	{
		this->OnHiddenGamePublishGatherDreamRet(req->is_publish_send, crossgameprotocal::GameHiddenGuildGatherDreamPublishRet::RESULT_TYPE_NOT_PRAY, req->uid, req->info.item_id);
		return;
	}

	if (m_gather_dream_param.count < 0)
	{
		m_gather_dream_param.count = 0;
	}
	else if (m_gather_dream_param.count >= GUILD_MAX_MEMBER_COUNT)	//默认覆盖最后一位
	{
		m_gather_dream_param.count = GUILD_MAX_MEMBER_COUNT - 1;
	}

	int index = m_gather_dream_map[req->info.role_id] = m_gather_dream_param.count;
	m_gather_dream_param.data_list[m_gather_dream_param.count++] = req->info;
	this->OnHiddenGamePublishGatherDreamRet(req->is_publish_send, crossgameprotocal::GameHiddenGuildGatherDreamPublishRet::RESULT_TYPE_SUCC, req->uid, req->info.item_id);

	this->SendGatherDreamSignleInfoToCross(req->uid, &index);
}

void Guild::OnHiddenGamePublishGatherDreamRet(int is_publish_send, int result, int uid, int item_id)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenGuildGatherDreamPublishRet ghggdpr;
	ghggdpr.is_publish_send = is_publish_send;
	ghggdpr.result = result;
	ghggdpr.guild_id = m_base_param.guild_id;
	ghggdpr.uid = uid;
	ghggdpr.item_id = item_id;

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghggdpr, sizeof(ghggdpr));
}

void Guild::OnGameHiddenPublishGatherDream(crossgameprotocal::GameHiddenGuildGatherDreamPublishRet * msg)
{
	if (NULL == msg || !CrossConfig::Instance().IsHiddenServer()) return;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->uid);
	if (NULL != role)
	{
		switch (msg->result)
		{
		case crossgameprotocal::GameHiddenGuildGatherDreamPublishRet::RESULT_TYPE_SUCC:
			{
				role->GetRoleModuleManager()->GetRoleGatherDream()->SetTodayPublishFlag(true);
				role->NoticeNum(noticenum::NT_GUILD_GATHER_DREAM_PUBLISH_SUCC);
				if (msg->is_publish_send == 1)
				{
					Protocol::SCChannelChat channel_chat;
					channel_chat.face_chatframe = role->GetRoleModuleManager()->GetFaceScore()->GetFaceEquipSeq(FACE_CHATFRAME);
					channel_chat.face_signature = role->GetRoleModuleManager()->GetFaceScore()->GetFaceEquipSeq(FACE_SIGNATURE);
					channel_chat.channel_type = chatdef::CHANNEL_TYPE_GUILD;
					channel_chat.msg_length = SNPRINTF(channel_chat.content, sizeof(channel_chat.content), gamestring::g_guild_gather_dream_publish, msg->item_id);
					channel_chat.content_type = 0;
					channel_chat.from_uid = msg->uid;
					role->GetName(channel_chat.username);
					channel_chat.headshot_id = role->GetHeadshotID();
					channel_chat.avatar_type = role->GetAvatarType();
					channel_chat.prof = static_cast<short>(role->GetProfession());
					channel_chat.level = (short)role->GetLevel();
					channel_chat.avatar_timestamp = role->GetAvatarTimestamp();
					channel_chat.top_level = role->GetTopLevel();
					channel_chat.fly_flag = role->GetRoleModuleManager()->GetFlyUpSystem()->GetFlyUpFlag();
					channel_chat.msg_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
					channel_chat.msg_type = Protocol::SCChannelChat::CHANNEL_CHAT_MSG_TYPE_BEGIN;

					m_guild_member_manager.SendInfoToAllMemberInfo((const char *)&channel_chat, sizeof(channel_chat));
				}
			}
			break;
		case crossgameprotocal::GameHiddenGuildGatherDreamPublishRet::RESULT_TYPE_CONFIG_ERROR:
			{
				role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			}
			break;
		case crossgameprotocal::GameHiddenGuildGatherDreamPublishRet::RESULT_TYPE_NOT_PRAY:
			{
				role->NoticeNum(errornum::EN_GUILD_GATHER_DREAM_NOT_CAN_PRAY);
			}
			break;
		default:
			break;
		}
	}
	else
	{
		if (msg->result == crossgameprotocal::GameHiddenGuildGatherDreamPublishRet::RESULT_TYPE_SUCC)
		{
			OfflineRegisterManager::Instance().OnGuildGatherDreamPublishFlag(msg->uid);
		}
	}
}

void Guild::OnGiftGatherDream(Role * role, int get_role_id)
{
	if (NULL == role) return;

	if (role->GetUID() == get_role_id)
	{
		role->NoticeNum(errornum::EN_GUILD_GATHER_DREAM_GIFT_MY);
		return;
	}

	if (!role->GetRoleModuleManager()->GetRoleGatherDream()->IsHaveGiftNum())
	{
		role->NoticeNum(errornum::EN_GUILD_GATHER_DREAM_NOT_HAVE_GIFT_NUM);
		return;
	}

	if (!role->GetRoleModuleManager()->GetRoleGatherDream()->IsCanTodayGiftUser(get_role_id))
	{
		role->NoticeNum(errornum::EN_GUILD_GATHER_DREAM_NOT_CAN_GIFT);
		return;
	}

	if (CrossConfig::Instance().IsHiddenServer())
	{
		if (!this->IsCanContOp(role))
		{
			return;
		}

		crossgameprotocal::HiddenGameGuildGatherDreamGiftReq hgggdgr;
		hgggdgr.get_role_uid = get_role_id;
		hgggdgr.guild_id = m_base_param.guild_id;
		hgggdgr.gift_role_uid = role->GetUID();

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(hgggdgr.gift_role_uid).db_index, (const char *)&hgggdgr, sizeof(hgggdgr));
		return;
	}

	std::map<int, int>::iterator it = m_gather_dream_map.find(get_role_id);
	if (it == m_gather_dream_map.end() || it->second >= m_gather_dream_param.count)
	{
		role->NoticeNum(errornum::EN_GUILD_GATHER_DREAM_NOT_ROLE);
		return;
	}
	GuildGatherDreamInfo & data_info = m_gather_dream_param.data_list[it->second];
	if (data_info.gather_num >= data_info.need_max_num)
	{
		role->NoticeNum(errornum::EN_GUILD_GATHER_DREAM_NOT_GIFT);
		return;
	}

	const GuildGatherDreamCfg * gather_dream_cfg = LOGIC_CONFIG->GetGuildConfig()->GetGatherDreamCfg(data_info.quality, data_info.gather_dream_type);
	if (NULL == gather_dream_cfg)
	{
		role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	GuildMember * gift_member = m_guild_member_manager.GetMember(role->GetUserId());
	GuildMember * get_member = m_guild_member_manager.GetMember(get_role_id);
	if (NULL == gift_member || NULL == get_member)
	{
		role->NoticeNum(errornum::EN_TEAMMANAGER_INV_ERR_DIF_GUILD);
		return;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeKnapsackItem(data_info.item_id, 1, __FUNCTION__))
	{
		role->NoticeNum(errornum::EN_GUILD_GATHER_DREAM_NOT_ENOUGH);
		return;
	}

	data_info.gather_num++;
	this->AddExp(gather_dream_cfg->guild_exp);
	role->GetRoleModuleManager()->GetMoney()->AddCoinBind(gather_dream_cfg->coin, __FUNCTION__, false, true);
	role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(gather_dream_cfg->gongxian, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__, false, true);
	bool is_put_succ = false;
	if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(gather_dream_cfg->reward_item.item_id, gather_dream_cfg->reward_item.num))
	{
		if (role->GetRoleModuleManager()->GetKnapsack()->Put(gather_dream_cfg->reward_item, PUT_REASON_GUILD_GATHER_DREAM))
		{
			is_put_succ = true;
		}
	}
	if (!is_put_succ)
	{
		role->GetRoleModuleManager()->GetKnapsack()->SendMail(&gather_dream_cfg->reward_item, SNED_MAIL_TYPE_DEFAULT, true);
	}

	role->GetRoleModuleManager()->GetRoleGatherDream()->OnAddGiftUid(get_role_id);
	role->GetRoleModuleManager()->SendSpecialNotice(Protocol::SCSpecialNotice::SPECIAL_NOTICE_TYPE_GUILD_GATHER_DREAM_GIFT, gather_dream_cfg->guild_exp,
		gather_dream_cfg->coin, gather_dream_cfg->gongxian, gather_dream_cfg->reward_item.item_id, gather_dream_cfg->reward_item.num);

	unsigned int now_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	static GatherDreamRecordInfo get_record_info; get_record_info.Reset();
	static GatherDreamRecordInfo gift_record_info; gift_record_info.Reset();
	//受赠记录
	get_record_info.role_id = role->GetUID();
	get_record_info.post = gift_member->GetPost();
	role->GetName(get_record_info.name);
	get_record_info.headshot_id = role->GetHeadshotID();
	get_record_info.avatar_type = role->GetAvatarType();
	get_record_info.gather_dream_type = data_info.gather_dream_type;
	get_record_info.item_id = data_info.item_id;
	get_record_info.timestamp = now_time;
	//赠送记录
	gift_record_info.role_id = data_info.role_id;
	gift_record_info.post = get_member->GetPost();
	F_STRNCPY(gift_record_info.name, data_info.from_name, sizeof(gift_record_info.name));
	gift_record_info.headshot_id = data_info.headshot_id;
	gift_record_info.avatar_type = data_info.avatar_type;
	gift_record_info.gather_dream_type = data_info.gather_dream_type;
	gift_record_info.item_id = data_info.item_id;
	gift_record_info.timestamp = now_time;

	GatherDreamRecordManager::Instance().AddRecord(get_record_info, gift_record_info, m_gather_dream_param);
	role->NoticeNum(noticenum::NT_GUILD_GATHER_DREAM_GIFT_SUCC);

	Role * get_role = World::GetInstWorld()->GetSceneManager()->GetRole(get_role_id);
	if (NULL != get_role)
	{
		this->SendGatherDreamSignleInfo(get_role, &it->second);
	}
	else
	{
		this->SendGatherDreamSignleInfoToCross(get_role_id, &it->second);
	}
	this->SendGatherDreamSignleInfo(role, &it->second);
}

void Guild::OnHiddenGameGiftGatherDream(crossgameprotocal::HiddenGameGuildGatherDreamGiftReq * req)
{
	if (NULL == req || CrossConfig::Instance().IsHiddenServer()) return;

	SynGuildGatherDreamGiftInfo info;
	info.guild_id = m_base_param.guild_id;
	info.gift_role_uid = req->gift_role_uid;
	info.get_role_uid = req->get_role_uid;

	std::map<int, int>::iterator it = m_gather_dream_map.find(req->get_role_uid);
	if (it == m_gather_dream_map.end() || it->second >= m_gather_dream_param.count)
	{
		this->OnHiddenGameGiftGatherDreamRet(crossgameprotocal::GameHiddenGuildGatherDreamGiftRet::RESULT_TYPE_NOT_PUBLISH, info);
		return;
	}

	GuildGatherDreamInfo & data_info = m_gather_dream_param.data_list[it->second];
	if (data_info.gather_num >= data_info.need_max_num)
	{
		this->OnHiddenGameGiftGatherDreamRet(crossgameprotocal::GameHiddenGuildGatherDreamGiftRet::RESULT_TYPE_IS_FINISH, info);
		return;
	}

	info.index = it->second;
	const GuildGatherDreamCfg * gather_dream_cfg = LOGIC_CONFIG->GetGuildConfig()->GetGatherDreamCfg(data_info.quality, data_info.gather_dream_type);
	if (NULL == gather_dream_cfg)
	{
		this->OnHiddenGameGiftGatherDreamRet(crossgameprotocal::GameHiddenGuildGatherDreamGiftRet::RESULT_TYPE_CONFIG_ERROR, info);
		return;
	}

	info.gather_dream_type = data_info.gather_dream_type;
	info.quality = data_info.quality;
	GuildMember * gift_member = m_guild_member_manager.GetMember(req->gift_role_uid);
	GuildMember * get_member = m_guild_member_manager.GetMember(req->get_role_uid);
	if (NULL == gift_member || NULL == get_member)
	{
		this->OnHiddenGameGiftGatherDreamRet(crossgameprotocal::GameHiddenGuildGatherDreamGiftRet::RESULT_TYPE_ERR_DIF_GUILD, info);
		return;
	}

	info.item_id = data_info.item_id;
	info.gift_post = gift_member->GetPost();
	info.get_post = get_member->GetPost();
	this->OnHiddenGameGiftGatherDreamRet(crossgameprotocal::GameHiddenGuildGatherDreamGiftRet::RESULT_TYPE_SUCC, info);
}

void Guild::OnHiddenGameGiftGatherDreamRet(int result, SynGuildGatherDreamGiftInfo info)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenGuildGatherDreamGiftRet ghggdgr;
	ghggdgr.result = result;
	ghggdgr.info = info;

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghggdgr, sizeof(ghggdgr));
}

void Guild::OnGameHiddenGiftGatherDream(crossgameprotocal::GameHiddenGuildGatherDreamGiftRet * msg)
{
	if (NULL == msg || !CrossConfig::Instance().IsHiddenServer()) return;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->info.gift_role_uid);
	if (NULL != role)
	{
		switch (msg->result)
		{
		case crossgameprotocal::GameHiddenGuildGatherDreamGiftRet::RESULT_TYPE_SUCC:
			{
				if (!role->GetRoleModuleManager()->GetRoleGatherDream()->IsHaveGiftNum())
				{
					role->NoticeNum(errornum::EN_GUILD_GATHER_DREAM_NOT_HAVE_GIFT_NUM);
					return;
				}

				if (!role->GetRoleModuleManager()->GetRoleGatherDream()->IsCanTodayGiftUser(msg->info.get_role_uid))
				{
					role->NoticeNum(errornum::EN_GUILD_GATHER_DREAM_NOT_CAN_GIFT);
					return;
				}

				const GuildGatherDreamCfg * gather_dream_cfg = LOGIC_CONFIG->GetGuildConfig()->GetGatherDreamCfg(msg->info.quality, msg->info.gather_dream_type);
				if (NULL == gather_dream_cfg)
				{
					role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
					return;
				}

				if (!role->GetRoleModuleManager()->GetKnapsack()->ConsumeKnapsackItem(msg->info.item_id, 1, __FUNCTION__))
				{
					role->NoticeNum(errornum::EN_GUILD_GATHER_DREAM_NOT_ENOUGH);
					return;
				}
				this->AddExp(gather_dream_cfg->guild_exp);
				role->GetRoleModuleManager()->GetMoney()->AddCoinBind(gather_dream_cfg->coin, __FUNCTION__, false, true);
				role->GetRoleModuleManager()->GetMoney()->AddOtherCoin(gather_dream_cfg->gongxian, MONEY_TYPE_GUILD_CONTRIBUTION, __FUNCTION__, false, true);
				bool is_put_succ = false;
				if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(gather_dream_cfg->reward_item.item_id, gather_dream_cfg->reward_item.num))
				{
					if (role->GetRoleModuleManager()->GetKnapsack()->Put(gather_dream_cfg->reward_item, PUT_REASON_GUILD_GATHER_DREAM))
					{
						is_put_succ = true;
					}
				}
				if (!is_put_succ)
				{
					role->GetRoleModuleManager()->GetKnapsack()->SendMail(&gather_dream_cfg->reward_item, SNED_MAIL_TYPE_DEFAULT, true);
				}
				role->GetRoleModuleManager()->GetRoleGatherDream()->OnAddGiftUid(msg->info.get_role_uid);
				role->NoticeNum(noticenum::NT_GUILD_GATHER_DREAM_GIFT_SUCC);
				role->GetRoleModuleManager()->SendSpecialNotice(Protocol::SCSpecialNotice::SPECIAL_NOTICE_TYPE_GUILD_GATHER_DREAM_GIFT, gather_dream_cfg->guild_exp,
					gather_dream_cfg->coin, gather_dream_cfg->gongxian, gather_dream_cfg->reward_item.item_id, gather_dream_cfg->reward_item.num);
				crossgameprotocal::HiddenGameGuildGatherDreamGiftAddRecord hgggdgar;
				hgggdgar.info = msg->info;

				InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(hgggdgar.info.get_role_uid).db_index, (const char *)&hgggdgar, sizeof(hgggdgar));
			}
			break;
		case crossgameprotocal::GameHiddenGuildGatherDreamGiftRet::RESULT_TYPE_NOT_PUBLISH:
			{
				role->NoticeNum(errornum::EN_GUILD_GATHER_DREAM_NOT_ROLE);
			}
			break;
		case crossgameprotocal::GameHiddenGuildGatherDreamGiftRet::RESULT_TYPE_IS_FINISH:
			{
				role->NoticeNum(errornum::EN_GUILD_GATHER_DREAM_NOT_GIFT);
			}
			break;
		case crossgameprotocal::GameHiddenGuildGatherDreamGiftRet::RESULT_TYPE_CONFIG_ERROR:
			{
				role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			}
			break;
		case crossgameprotocal::GameHiddenGuildGatherDreamGiftRet::RESULT_TYPE_ERR_DIF_GUILD:
			{
				role->NoticeNum(errornum::EN_TEAMMANAGER_INV_ERR_DIF_GUILD);
			}
			break;
		}
	}
}

void Guild::OnHiddenGameGiftGatherDreamAddRecord(crossgameprotocal::HiddenGameGuildGatherDreamGiftAddRecord * msg)
{
	if (NULL == msg || CrossConfig::Instance().IsHiddenServer()) return;

	//此时集愿还未被清除
	bool is_clear = false;
	std::map<int, int>::iterator it = m_gather_dream_map.find(msg->info.get_role_uid);
	if (it != m_gather_dream_map.end() && msg->info.index == it->second && it->second < m_gather_dream_param.count &&  m_gather_dream_param.data_list[it->second].gather_num < m_gather_dream_param.data_list[it->second].need_max_num)
	{
		m_gather_dream_param.data_list[it->second].gather_num++;
	}
	else
	{
		is_clear = true;
	}
	unsigned int now_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	static GatherDreamRecordInfo get_record_info; get_record_info.Reset();
	static GatherDreamRecordInfo gift_record_info; gift_record_info.Reset();
	UserCacheNode* get_user = UserCacheManager::Instance().GetUserNode(msg->info.get_role_uid);
	UserCacheNode* gift_user = UserCacheManager::Instance().GetUserNode(msg->info.gift_role_uid);
	//受赠记录
	if (NULL != gift_user)
	{
		get_record_info.role_id = gift_user->uid;
		get_record_info.post = msg->info.gift_post;
		gift_user->GetName(get_record_info.name);
		get_record_info.headshot_id = gift_user->appearance.headshot_id;
		get_record_info.avatar_type = gift_user->appearance.avatar_type;
		get_record_info.gather_dream_type = msg->info.gather_dream_type;
		get_record_info.item_id = msg->info.item_id;
		get_record_info.timestamp = now_time;
	}
	//赠送记录
	if (NULL != get_user)
	{
		gift_record_info.role_id = get_user->uid;
		gift_record_info.post = msg->info.get_post;
		get_user->GetName(gift_record_info.name);
		gift_record_info.headshot_id = get_user->appearance.headshot_id;
		gift_record_info.avatar_type = get_user->appearance.avatar_type;
		gift_record_info.gather_dream_type = msg->info.gather_dream_type;
		gift_record_info.item_id = msg->info.item_id;
		gift_record_info.timestamp = now_time;
	}
	if (NULL != get_user && NULL != gift_user)
	{
		GatherDreamRecordManager::Instance().AddRecord(get_record_info, gift_record_info, m_gather_dream_param);
	}

	if (!is_clear)
	{
		Role * get_role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->info.get_role_uid);
		if (NULL != get_role)
		{
			this->SendGatherDreamSignleInfo(get_role, &it->second);
		}
		else
		{
			this->SendGatherDreamSignleInfoToCross(msg->info.get_role_uid, &it->second);
		}

		Role * gift_role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->info.gift_role_uid);
		if (NULL != gift_role)
		{
			this->SendGatherDreamSignleInfo(gift_role, &it->second);
		}
		else
		{
			this->SendGatherDreamSignleInfoToCross(msg->info.gift_role_uid, &it->second);
		}
	}
}

void Guild::OnFetchGatherDream(Role * role)
{
	if (NULL == role) return;
	
	if (CrossConfig::Instance().IsHiddenServer())
	{
		if (!this->IsCanContOp(role))
		{
			return;
		}

		crossgameprotocal::HiddenGameGuildGatherDreamFetchReq hgggdfr;
		hgggdfr.uid = role->GetUID();
		hgggdfr.guild_id = m_base_param.guild_id;

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(hgggdfr.uid).db_index, (const char *)&hgggdfr, sizeof(hgggdfr));
		return;
	}
	std::map<int, int>::iterator it = m_gather_dream_map.find(role->GetUID());
	if (it == m_gather_dream_map.end() || it->second >= m_gather_dream_param.count)
	{
		role->NoticeNum(errornum::EN_GUILD_GATHER_DREAM_NOT_ROLE);
		return;
	}

	GuildGatherDreamInfo & data_info = m_gather_dream_param.data_list[it->second];
	int count = data_info.gather_num - data_info.fetch_num;
	if (count <= 0)
	{
		role->NoticeNum(errornum::EN_GUILD_GATHER_DREAM_NOT_FETCH);
		return;
	}

	if (!role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(data_info.item_id, count))
	{
		role->NoticeNum(errornum::EN_HAS_NO_SPACING);
		return;
	}
	if (!role->GetRoleModuleManager()->GetKnapsack()->Put(ItemConfigData(data_info.item_id, true, count), PUT_REASON_GUILD_GATHER_DREAM))
	{
		role->NoticeNum(errornum::EN_HAS_NO_SPACING);
		return;
	}
	data_info.fetch_num += count;
	this->SendGatherDreamSignleInfo(role, &it->second);
}

void Guild::OnHiddenGameFetchGatherDream(int uid)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	std::map<int, int>::iterator it = m_gather_dream_map.find(uid);
	if (it == m_gather_dream_map.end() || it->second >= m_gather_dream_param.count)
	{
		this->OnHiddenGameFetchGatherDreamRet(crossgameprotocal::GameHiddenGuildGatherDreamFetchRet::RESULT_TYPE_NOT_PUBLISH, uid, 0, 0, 0);
		return;
	}
	GuildGatherDreamInfo & data_info = m_gather_dream_param.data_list[it->second];
	int count = data_info.gather_num - data_info.fetch_num;
	if (count <= 0)
	{
		this->OnHiddenGameFetchGatherDreamRet(crossgameprotocal::GameHiddenGuildGatherDreamFetchRet::RESULT_TYPE_NOT_HAS_FETCH, uid, 0, 0, 0);
		return;
	}
	//先修改结果
	m_gather_dream_param.data_list[it->second].fetch_num += count;
	this->OnHiddenGameFetchGatherDreamRet(crossgameprotocal::GameHiddenGuildGatherDreamFetchRet::RESULT_TYPE_SUCC, uid, it->second, data_info.item_id, count);
}

void Guild::OnHiddenGameFetchGatherDreamRet(int result, int uid, int index, ItemID item_id, short fetch_count)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	crossgameprotocal::GameHiddenGuildGatherDreamFetchRet ghggdfr;
	ghggdfr.result = result;
	ghggdfr.uid = uid;
	ghggdfr.index = index;
	ghggdfr.guild_id = m_base_param.guild_id;
	ghggdfr.item_id = item_id;
	ghggdfr.fetch_count = fetch_count;
	InternalComm::Instance().SendToHiddenThroughCross((const char *)&ghggdfr, sizeof(ghggdfr));
}

void Guild::OnGameHiddenFetchGatherDream(crossgameprotocal::GameHiddenGuildGatherDreamFetchRet * msg)
{
	if (NULL == msg || !CrossConfig::Instance().IsHiddenServer()) return;

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->uid);
	switch (msg->result)
	{
	case crossgameprotocal::GameHiddenGuildGatherDreamFetchRet::RESULT_TYPE_SUCC:
	{
		bool is_put_succ = false;
		if (NULL != role)
		{
			if (role->GetRoleModuleManager()->GetKnapsack()->CheckForPut(msg->item_id, msg->fetch_count))
			{
				if (role->GetRoleModuleManager()->GetKnapsack()->Put(ItemConfigData(msg->item_id, true, msg->fetch_count), PUT_REASON_GUILD_GATHER_DREAM))
				{
					is_put_succ = true;
				}
			}
			if (!is_put_succ)
			{
				role->NoticeNum(noticenum::NT_KNAPSACKFULL_SEND_MAIL);
			}
		}
		if (!is_put_succ)
		{
			static MailContentParam contentparam; contentparam.Reset();
			FillMailContentParam(contentparam, ItemConfigData(msg->item_id, true, msg->fetch_count));

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_gather_dream_daychange_reward_subject);
			int length = 0;
			if (NULL != role)
			{
				length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_knapsack_full_send_mail);
			}
			else
			{
				length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_gather_dream_reissue_reward_content);
			}
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(msg->uid, MAIL_REASON_DEFAULT, contentparam);
			}
			is_put_succ = true;
		}

		crossgameprotocal::HiddenGameGuildGatherDreamFetchChange hgggdfc;
		hgggdfc.uid = msg->uid;
		hgggdfc.index = msg->index;
		hgggdfc.guild_id = msg->guild_id;
		hgggdfc.fetch_count = msg->fetch_count;
		hgggdfc.is_succ = is_put_succ ? 1 : 0;

		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(hgggdfc.uid).db_index, (const char *)&hgggdfc, sizeof(hgggdfc));
	}
	break;
	case crossgameprotocal::GameHiddenGuildGatherDreamFetchRet::RESULT_TYPE_NOT_PUBLISH:
	{
		if(NULL != role)
		{ 
			role->NoticeNum(errornum::EN_GUILD_GATHER_DREAM_NOT_ROLE);
		}
	}
	break;
	case crossgameprotocal::GameHiddenGuildGatherDreamFetchRet::RESULT_TYPE_NOT_HAS_FETCH:
	{
		if (NULL != role)
		{
			role->NoticeNum(errornum::EN_GUILD_GATHER_DREAM_NOT_FETCH);
		}
	}
	break;
	}
}

void Guild::OnHiddenGameFetchGatherDreamChange(crossgameprotocal::HiddenGameGuildGatherDreamFetchChange * msg)
{
	if (NULL == msg || CrossConfig::Instance().IsHiddenServer()) return;

	int index = -1;
	std::map<int, int>::iterator it = m_gather_dream_map.find(msg->uid);
	if (it != m_gather_dream_map.end() && it->second < m_gather_dream_param.count && it->second == msg->index)
	{
		if (0 == msg->is_succ)
		{
			m_gather_dream_param.data_list[it->second].fetch_num -= msg->fetch_count;
			if (m_gather_dream_param.data_list[it->second].fetch_num < 0)
			{
				m_gather_dream_param.data_list[it->second].fetch_num = 0;
			}
		}
		index = it->second;
	}
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(msg->uid);
	if (NULL != role)
	{
		this->SendGatherDreamSignleInfo(role, &index);
	}
	else
	{
		this->SendGatherDreamSignleInfoToCross(msg->uid, &index);
	}
}

void Guild::OnRemoveMember(int uid)
{
	//名人堂
	//战神榜
	for (HallOfFameRankList::iterator it = m_battle_rank.begin(); it != m_battle_rank.end(); it++)
	{
		if (it->role_id == uid)
		{
			m_battle_rank.erase(it);
			std::sort(m_battle_rank.begin(), m_battle_rank.end(), FallOfFameRankSort);
			break;
		}
	}
	//富豪榜
	for (HallOfFameRankList::iterator it = m_rich_rank.begin(); it != m_rich_rank.end(); it++)
	{
		if (it->role_id == uid)
		{
			m_rich_rank.erase(it);
			std::sort(m_rich_rank.begin(), m_rich_rank.end(), FallOfFameRankSort);
			break;
		}
	}
	//善人榜
	for (HallOfFameRankList::iterator it = m_kinder_rank.begin(); it != m_kinder_rank.end(); it++)
	{
		if (it->role_id == uid)
		{
			m_kinder_rank.erase(it);
			std::sort(m_kinder_rank.begin(), m_kinder_rank.end(), FallOfFameRankSort);
			break;
		}
	}

	// 集愿
	std::map<int, int>::iterator it = m_gather_dream_map.find(uid);
	if (it == m_gather_dream_map.end() || it->second >= GUILD_MAX_MEMBER_COUNT || it->second < 0) return;

	int fetch_num = m_gather_dream_param.data_list[it->second].gather_num - m_gather_dream_param.data_list[it->second].fetch_num;
	if (fetch_num > 0)
	{
		static MailContentParam contentparam; contentparam.Reset();
		contentparam.item_list[0].item_id = m_gather_dream_param.data_list[it->second].item_id;
		contentparam.item_list[0].num = fetch_num;
		contentparam.item_list[0].is_bind = true;

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_gather_dream_daychange_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_gather_Dream_leave_guild_reward_content);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(uid, MAIL_REASON_DEFAULT, contentparam);
		}
	}

	m_gather_dream_param.count--;
	//后面直接往前覆盖
	for (int i = it->second; i < GUILD_MAX_MEMBER_COUNT - 1 && i < m_gather_dream_param.count; i++)
	{
		m_gather_dream_param.data_list[i] = m_gather_dream_param.data_list[i + 1];
		m_gather_dream_map[m_gather_dream_param.data_list[i].role_id] = i;
	}
	m_gather_dream_param.data_list[m_gather_dream_param.count].Reset();
	m_gather_dream_map.erase(it);
	GatherDreamRecordManager::Instance().LeaveGuild(uid);
}

void Guild::OnUpdateRoleInfo(SynGatherDreamRecord info, int uid)
{
	std::map<int, int>::iterator it = m_gather_dream_map.find(uid);
	if (it == m_gather_dream_map.end() || it->second >= GUILD_MAX_MEMBER_COUNT || it->second < 0) return;

	GuildGatherDreamInfo & gather_info = m_gather_dream_param.data_list[it->second];
	F_STRNCPY(gather_info.from_name, info.name, sizeof(gather_info.from_name));
	gather_info.avatar_type = info.avatar_type;
	gather_info.headshot_id = info.headshot_id;
}

void Guild::SendGatherDreamList(Role * role)
{
	if (NULL == role) return;

	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameGuildGatherDreamPublishListReq hgggdplr;
		hgggdplr.guild_id = m_base_param.guild_id;
		hgggdplr.uid = role->GetUID();
		InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), IntToUid(hgggdplr.uid).db_index, (const char*)&hgggdplr, sizeof(hgggdplr));
		return;
	}

	static Protocol::SCGuildGatherDreamInfoList info;
	info.count = 0;
	for (int i = 0; i < m_gather_dream_param.count && i < GUILD_MAX_MEMBER_COUNT; i++)
	{
		info.gather_dream_list[i] = m_gather_dream_param.data_list[i];
		info.gather_dream_list[i].help_num = GatherDreamRecordManager::Instance().GetHelpNum(role->GetUID(), info.gather_dream_list[i].role_id);
		info.count++;
	}

	int length = sizeof(info) - sizeof(GuildGatherDreamInfo) * (GUILD_MAX_MEMBER_COUNT - info.count);
	role->GetRoleModuleManager()->NetSend(&info, length);
}

void Guild::SendGatherDreamListToCross(int uid)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	static Protocol::SCGuildGatherDreamInfoList info;
	info.count = 0;
	for (int i = 0; i < m_gather_dream_param.count && i < GUILD_MAX_MEMBER_COUNT; i++)
	{
		info.gather_dream_list[i] = m_gather_dream_param.data_list[i];
		info.gather_dream_list[i].help_num = GatherDreamRecordManager::Instance().GetHelpNum(uid, info.gather_dream_list[i].role_id);
		info.count++;
	}

	int length = sizeof(info) - sizeof(GuildGatherDreamInfo) * (GUILD_MAX_MEMBER_COUNT - info.count);
	InternalComm::Instance().SendToHiddenUserThroughCross(uid, (const char*)&info, length);
}

void Guild::SendGatherDreamSignleInfo(Role * role, int *index)
{
	if (NULL == role || CrossConfig::Instance().IsHiddenServer()) return;

	int data_index = 0;
	if (NULL != index && *index >= 0 && *index < GUILD_MAX_MEMBER_COUNT)
	{
		data_index = *index;
	}
	else
	{
		std::map<int, int>::iterator it = m_gather_dream_map.find(role->GetUID());
		if (it == m_gather_dream_map.end() || it->second < 0 || it->second >= GUILD_MAX_MEMBER_COUNT) return;

		data_index = it->second;
	}

	Protocol::SCGuildGatherDreamSignleInfo info;
	info.gather_dream_info = m_gather_dream_param.data_list[data_index];
	info.gather_dream_info.help_num = GatherDreamRecordManager::Instance().GetHelpNum(role->GetUID(), info.gather_dream_info.role_id);
	role->GetRoleModuleManager()->NetSend(&info, sizeof(info));
}

void Guild::SendGatherDreamSignleInfoToCross(int uid, int *index)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	int data_index = 0;
	if (NULL != index && *index >= 0 && *index < GUILD_MAX_MEMBER_COUNT)
	{
		data_index = *index;
	}
	else
	{
		std::map<int, int>::iterator it = m_gather_dream_map.find(uid);
		if (it == m_gather_dream_map.end() || it->second < 0 || it->second >= GUILD_MAX_MEMBER_COUNT) return;

		data_index = it->second;
	}
	Protocol::SCGuildGatherDreamSignleInfo info;
	info.gather_dream_info = m_gather_dream_param.data_list[data_index];
	info.gather_dream_info.help_num = GatherDreamRecordManager::Instance().GetHelpNum(uid, info.gather_dream_info.role_id);
	InternalComm::Instance().SendToHiddenUserThroughCross(uid, (const char *)&info, sizeof(info));
}

bool Guild::IsCanContOp(Role * role)
{
	if(NULL == role) return false;

	unsigned int now_second = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	std::map<int, unsigned int>::iterator time_it = m_gather_dream_timestamp_map.find(role->GetUID());
	if (time_it != m_gather_dream_timestamp_map.end() && time_it->second >= now_second)
	{
		m_gather_dream_timestamp_map[role->GetUID()] = now_second;
		role->NoticeNum(errornum::EN_OPERATE_TOO_FREQUENT);
		return false;
	}
	m_gather_dream_timestamp_map[role->GetUID()] = now_second;
	return true;
}

unsigned int Guild::GetPatriarchLastLoginTime()
{
	unsigned int patriarch_last_login_time = 0;

	GuildMember * patriarch_member = m_guild_member_manager.GetPatriarchMember();
	if (patriarch_member != NULL)
	{
		patriarch_last_login_time = patriarch_member->GetLastLogintime();
	}

	if (UserCacheManager::Instance().IsUserOnline(m_guild_member_manager.GetPatriarchUid()))
	{
		patriarch_last_login_time = 0;
	}

	return patriarch_last_login_time;
}

void Guild::GmSetNoLeader()
{
	GuildMember* leader = m_guild_member_manager.GetPatriarchMember();
	if (NULL != leader)
	{
		leader->SetPost(GUILD_POST_MEMBER);
		this->SendGuildBaseInfo();
	}
}

void Guild::GmSetName(const char * name)
{
	STRNCPY(m_base_param.guild_name, name, sizeof(m_base_param.guild_name));
	this->SetDirty(true);
}

void Guild::GmSetNotice(const char * notice)
{
	STRNCPY(m_base_param.guild_notice, notice, sizeof(m_base_param.guild_notice));
	this->SetDirty(true);
}

void Guild::AddActiveVal(int _role_id, int _add_active_val)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->AddActiveValByHidden(_role_id, _add_active_val);
	}
	else
	{
		this->AddActiveValByGameWorld(_role_id, _add_active_val);
	}
}

void Guild::AddActiveValByGameWorld(int _role_id, int _add_active_val)
{
	m_base_param.day_total_guild_active_val += _add_active_val;
	m_base_param.week_total_guild_active_val += _add_active_val;
	m_base_param.last_guild_active_change_times = (unsigned int)EngineAdapter::Instance().Time();

	m_base_param.seven_day_active[GUILD_SEVEN_DAY_ACTIVE - 1] += _add_active_val;

	gamelog::g_log_guild.printf(LL_INFO, "%s guild[%d %s] role[%d] add_val:%d  day_guild_active_val:%d  week_guild_active_val:%d",
		__FUNCTION__, m_base_param.guild_id, m_base_param.guild_name, _role_id, _add_active_val, m_base_param.day_total_guild_active_val, m_base_param.week_total_guild_active_val);

	this->UpdateActiveRankByGameWorld(_role_id, _add_active_val);

	this->SendGuildActiveInfo(NULL);

	// 更新到帮派排行榜
	RankManager::Instance().GetGuildRank()->OnGuildRankInfoChange(this, GUILD_RANK_TYPE_WEEK_ACTIVE);

	this->SetDirty(true);
}

void Guild::AddActiveValByHidden(int _role_id, int _add_active_val)
{
	const CrossUserRegister::CrossUserInfo* cui = CrossUserRegister::Instance().GetCrossUserInfo(IntToUid(_role_id));
	if (NULL == cui)
	{
		return;
	}

	static crossgameprotocal::HiddenGameRoleActiveChange change;
	change.server_id.plat_type = cui->original_plat_type;
	change.server_id.server_id = cui->original_server_id;
	change.guild_id = m_base_param.guild_id;
	change.role_id = _role_id;
	change.add_val = _add_active_val;

	InternalComm::Instance().SendToCross((const char *)&change, sizeof(change));
}

void Guild::SyncActiveInfoToHidden(GuildMember * _member)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	static crossgameprotocal::GameHiddenGuildActiveInfoChange change;
	change.guild_id = m_base_param.guild_id;
	change.day_active_val = m_base_param.day_total_guild_active_val;
	change.week_active_val = m_base_param.week_total_guild_active_val;
	change.change_times = m_base_param.last_guild_active_change_times;
	change.role_id = 0;
	change.member_seven_today_active = 0;

	if (NULL != _member)
	{
		change.role_id = _member->GetUid();
		change.member_week_active_val = _member->GetWeekActiveVal();
		change.reward_flag = _member->GetGuildDayActiveRewardFlag();
		change.member_seven_today_active = _member->GetSevenDayTodayActive();
	}

	InternalComm::Instance().SendToCross((const char *)&change, sizeof(change));
}

void Guild::SyncActiveInfoByGameWorld(crossgameprotocal::GameHiddenGuildActiveInfoChange * _change)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	m_base_param.day_total_guild_active_val = _change->day_active_val;
	m_base_param.week_total_guild_active_val = _change->week_active_val;
	m_base_param.seven_day_active[GUILD_SEVEN_DAY_ACTIVE - 1] = m_base_param.day_total_guild_active_val;
	m_base_param.last_guild_active_change_times = _change->change_times;

	GuildMember * member = m_guild_member_manager.GetMember(_change->role_id);
	if (NULL != member)
	{
		this->UpdateActiveRankByHidden(member , _change);
	}

	this->SendGuildActiveInfo(NULL);
}

void Guild::SendGuildActiveInfo(Role * _role)
{
	Protocol::SCGuildActiveInfo info;
	info.day_active_val = m_base_param.day_total_guild_active_val;
	info.week_active_val = m_base_param.week_total_guild_active_val;

	if (NULL == _role)
	{
		m_guild_member_manager.SendInfoToAllMember((const char *)&info, sizeof(info));
	}
	else
	{
		EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char *)&info, sizeof(info));
	}
}

void Guild::SendGuildActivePersonRank(Role * _role)
{
	if (NULL == _role) return;

	Protocol::SCGuildActivePersonRank rank_info;

	SkipList<WeekActiveInfo>::Iterator it_begin = m_week_active_rank.Begin();
	SkipList<WeekActiveInfo>::Iterator it_end = m_week_active_rank.End();

	rank_info.count = 0;
	for (; it_begin != it_end; ++it_begin, ++rank_info.count)
	{
		GuildMember * member = m_guild_member_manager.GetMember(IntToUid(it_begin->role_id));
		if (NULL == member)
		{
			gamelog::g_log_guild.printf(LL_INFO, "Guild::SendGuildActivePersonRank[ERROR_1] guild[%d  %s] rank_index[%d]  role_id[%d]",
				m_base_param.guild_id, m_base_param.guild_name, rank_info.count, it_begin->role_id);
			continue;
		}

		memcpy(rank_info.rank_info[rank_info.count].game_name, member->GetName(), sizeof(GameName));
		rank_info.rank_info[rank_info.count].level = member->GetLevel();
		rank_info.rank_info[rank_info.count].prof = member->GetProfession();
		rank_info.rank_info[rank_info.count].post = member->GetPost();
		rank_info.rank_info[rank_info.count].week_active_val = it_begin->active_val;
		rank_info.rank_info[rank_info.count].reserve_ch = 0;
	}

	// 求个人排名
	rank_info.index = -1;
	GuildMember * member = m_guild_member_manager.GetMember(_role->GetUserId());
	if (NULL != member)
	{
		WeekActiveInfo info;
		info.role_id = _role->GetUID();
		info.active_val = member->GetWeekActiveVal();
		info.last_change_time = member->GetLastChangeWeekActiveValTimes();

		rank_info.index = m_week_active_rank.GetIndex(info);
	}

	int length = sizeof(rank_info) - (GUILD_MAX_MEMBER_COUNT - rank_info.count) * sizeof(rank_info.rank_info[0]);
	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char *)&rank_info, length);
}

void Guild::UpdateActiveRankByHidden(GuildMember * _member , crossgameprotocal::GameHiddenGuildActiveInfoChange * _change)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;
	// 备注 同步来源于主服的数据 , 所有数据以主服为主 , 这里的日志只是做备用

	// 进行排序
	WeekActiveInfo info;
	info.role_id = _member->GetUid();
	info.active_val = _member->GetWeekActiveVal();
	info.last_change_time = _member->GetLastChangeWeekActiveValTimes();

	// 先记录旧数据 然后再改变新数据
	_member->SetWeekActiveVal(_change->member_week_active_val, _change->member_seven_today_active);
	_member->SetGuildDayActiveRewardFlag(_change->reward_flag);

	// 先查看是否存在 , 存在则更新 不存在则
	int index = m_week_active_rank.GetIndex(info);
	bool need_insert = false;
	bool is_error_insert = false;  // 是否是容错插入 , 用于日志查BUG

	info.active_val = _member->GetWeekActiveVal(); // 查询后 , 再取新值用于更新
	info.last_change_time = _member->GetLastChangeWeekActiveValTimes();

	if (-1 == index)
	{
		need_insert = true;
	}
	else
	{
		SkipList<WeekActiveInfo>::Iterator it = m_week_active_rank[index];
		if (m_week_active_rank.End() != it)
		{
			int old_val = it->active_val;
			int old_times = it->last_change_time;

			m_week_active_rank.UpdateData(*it, info);
			int new_index = m_week_active_rank.GetIndex(info);

			gamelog::g_log_guild.printf(LL_INFO, "[Guild::UpdateActiveRankByHidden Update] [guild[%d %s]  role[%d %s]  "
				"list_size[%u]  old_index[%d]  new_index[%d]  old_val[%d]  old_times[%u]  new_val[%d] new_times[%u] ",
				m_base_param.guild_id, m_base_param.guild_name, _member->GetUid(), _member->GetName(),
				m_week_active_rank.Size(), index, new_index, old_val, old_times, info.active_val, info.last_change_time);
		}
		else
		{
			// 记录错误日志 
			gamelog::g_log_guild.printf(LL_CRITICAL, "[Guild::UpdateActiveRankByHidden operation[] ERROR] [guild[%d %s]  check_user[%d %s]  "
				"list_size[%u]  old_index[%d]  new_val[%d] new_times[%u] ]",
				m_base_param.guild_id, m_base_param.guild_name, _member->GetUid(), _member->GetName(),
				m_week_active_rank.Size(), index, info.active_val, info.last_change_time);

			// 容错
			need_insert = true;
			is_error_insert = true;
		}
	}

	if (need_insert)
	{
		// 列表的长度是以最大帮派成员数量为长度的, 所以不需要判断列表是否已满
		m_week_active_rank.Insert(info);
		int new_index = m_week_active_rank.GetIndex(info);

		gamelog::g_log_guild.printf(LL_INFO, "[Guild::UpdateActiveRankByHidden Insert] [is_error_insert:%d] [guild[%d %s]  role[%d %s]  "
			"list_size[%u]  new_index[%d] new_val[%d] new_times[%u] ]",
			is_error_insert, m_base_param.guild_id, m_base_param.guild_name, _member->GetUid(), _member->GetName(),
			m_week_active_rank.Size(), new_index, info.active_val, info.last_change_time);
	}

}

void Guild::InitWeekActivePersonRank()
{
	//  初始化是 排行榜内一定为空 , 因为表长为成员最大总数 , 所以插入时无须判断是否已满
	m_week_active_rank.Clear();
	int member_count = m_guild_member_manager.GetMemberCount();
	for (int i = 0; i < member_count; ++i)
	{
		GuildMember * member = m_guild_member_manager.GetMemberByIndex(i);
		if (NULL == member)
			continue;

		int member_val = member->GetWeekActiveVal();;
		if (0 == member_val)
			continue;

		WeekActiveInfo info;
		info.role_id = member->GetUid();
		info.last_change_time = member->GetLastChangeWeekActiveValTimes();
		info.active_val = member_val;

		m_week_active_rank.Insert(info);
	}
}

void Guild::OnGuildActiveGetGiftReq(Role * _role, int _seq)
{
	if (NULL == _role) return;

	if (_role->GetRoleModuleManager()->GetCommonData().guild_day_active_reward_flag & LL_ONE_BIT << _seq)
	{
		_role->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	GuildMember * member = m_guild_member_manager.GetMember(_role->GetUserId());
	if (NULL == member || member->GetGuildDayActiveRewardFlag() != _role->GetRoleModuleManager()->GetCommonData().guild_day_active_reward_flag)
	{
		_role->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const GuildActiveGiftCfg::GiftCfg * gift_cfg = LOGIC_CONFIG->GetGuildConfig()->GetGuildActiveGiftCfg(_seq , _role->GetLevel());
	if (NULL == gift_cfg)
	{
		_role->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (m_base_param.day_total_guild_active_val < gift_cfg->active_limit)
	{
		_role->NoticeNum(errornum::EN_GUILD_ACTIVE_DAY_ACTIVE_LIMIT);
		return;
	}

	// 生效
	_role->GetRoleModuleManager()->GetCommonData().guild_day_active_reward_flag |= LL_ONE_BIT << _seq;
	member->SetGuildDayActiveRewardFlag(_role->GetRoleModuleManager()->GetCommonData().guild_day_active_reward_flag);

	// 发送奖励
	_role->AddExp(gift_cfg->exp, ADD_EXP_TYPE_GUILD_ACTIVE_GIFT , __FUNCTION__);
	_role->GetRoleModuleManager()->GetKnapsack()->PutList(gift_cfg->count, gift_cfg->reward_list, PUT_REASON_GUILD_ACTIVE_DAY_GIFT);

	// 
	_role->SendGuildActiveInfo(_seq);

	// 记录日志
	gamelog::g_log_guild.buff_printf("Guild::OnGuildActiveGetGiftReq  guild[%d %s]  role[%d %s] role_level:%d  day_active_val:%d  seq:%d  need_active_val:%d  reward_exp:%d",
		this->GetGuildID(), this->GetGuildName(), _role->GetUID(), _role->GetName(), _role->GetLevel() , m_base_param.day_total_guild_active_val, _seq, gift_cfg->active_limit , gift_cfg->exp);
	for (int i = 0; i < gift_cfg->count; ++i)
	{
		gamelog::g_log_guild.buff_printf("item[index:%d id:%d num:%d] ",
			i, gift_cfg->reward_list[i].item_id, gift_cfg->reward_list[i].num);
	}
	gamelog::g_log_guild.commit_buff(LL_INFO);


	if (CrossConfig::Instance().IsHiddenServer())
	{
		// 通知主服变更信息
		static crossgameprotocal::HiddenGameRoleActiveGiftInfo info;

		info.guild_id = m_base_param.guild_id;
		info.role_id = _role->GetUID();
		info.flag = _role->GetRoleModuleManager()->GetCommonData().guild_day_active_reward_flag;
		info.get_timestamp = (unsigned int)EngineAdapter::Instance().Time();

		InternalComm::Instance().SendToGameThroughCross(m_base_param.origin_plat_type, m_base_param.origin_server_id,(const char*)&info, sizeof(info));
	}
}

void Guild::GuildActiveDayActiveSettlement()
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	// 1. 先重置并且广播信息变更
	const int guild_active_day_val = m_base_param.day_total_guild_active_val;
	m_base_param.day_total_guild_active_val = 0;
	this->SendGuildActiveInfo(NULL);
	this->SyncActiveInfoToHidden(NULL);

	// 2. 发放奖励
	int member_count = m_guild_member_manager.GetMemberCount();
	unsigned int now_sec = (unsigned int)EngineAdapter::Instance().Time();

	for (int member_i = 0; member_i < member_count; ++member_i)
	{
		GuildMember * member = m_guild_member_manager.GetMemberByIndex(member_i);
		if (NULL == member) continue;

		member->GuildActiveDayActiveReward(guild_active_day_val, now_sec);
	}

	this->SetDirty(true);
}

void Guild::GuildActiveWeekActivePersonRankSettlement()
{
	SkipList<WeekActiveInfo>::Iterator it_begin = m_week_active_rank.Begin();
	SkipList<WeekActiveInfo>::Iterator it_end = m_week_active_rank.End();

	unsigned int now_sec = (unsigned int)EngineAdapter::Instance().Time();
	int rank_index = 1;
	for (; it_begin != it_end; ++it_begin , ++rank_index)
	{
		GuildMember * member = m_guild_member_manager.GetMember(IntToUid(it_begin->role_id));
		if (NULL == member)
		{
			// BUG  记录一下日志
			gamelog::g_log_guild.printf(LL_INFO, "Guild::GuildActiveWeekActivePersonRankSettlement[ERROR_1] "
				" guild[%d %s]  role[%d]  rank_index:%d   val:%d",
				this->GetGuildID(), this->GetGuildName(), it_begin->role_id, rank_index, it_begin->active_val);
			continue;
		}

		if (!CrossConfig::Instance().IsHiddenServer())
		{
			// 原服才发奖励, 跨服只是同步清理一下数据
			if (rank_index < GUILD_ACTIVE_PERSON_RANK_REWARD_COUNT)
			{
				// 前N名才有奖励
				member->GuildActiveWeekActivePersonRankReward(rank_index, it_begin->active_val, now_sec);
			}
		}

		// 清理玩家的周活跃记录
		member->ClearWeekActiveValInfo();
	}

	// 重置排行榜
	m_week_active_rank.Clear();
}

void Guild::OnRemoveMemberByGuildActivePersonRank(GuildMember * _member)
{
	Guild::WeekActiveInfo info;
	info.role_id = _member->GetUid();
	info.active_val = _member->GetWeekActiveVal();
	info.last_change_time = _member->GetLastChangeWeekActiveValTimes();

	m_week_active_rank.Erase(info);
}

void Guild::GetCrossGuildActiveRankReward(int _rank_index, int _rank_val)
{
	// 发放周活跃度排行榜奖励
	const GuildActiveRankRewardCfg * cfg = LOGIC_CONFIG->GetGuildConfig()->GetGuildActiveRankRewardCfg(_rank_index);
	if (NULL == cfg)
	{
		gamelog::g_log_guild.printf(LL_INFO, "Guild::GetCrossGuildActiveRankReward[ERROR_1]  guild[%d %s]  rank_index:%d  rank_val:%d",
			this->GetGuildID(), this->GetName(), _rank_index, _rank_val);
		return;
	}

	static MailContentParam content_param;
	content_param.Reset();
	int lensub = SNPRINTF(content_param.subject, sizeof(content_param.subject), gamestring::g_guild_active_week_active_rank_subject);
	int length = SNPRINTF(content_param.contenttxt, sizeof(content_param.contenttxt), gamestring::g_guild_active_week_active_rank_content, _rank_index);
	if (lensub <= 0 || length <= 0)
	{
		gamelog::g_log_guild.printf(LL_INFO, "Guild::GetCrossGuildActiveRankReward[ERROR_2]  guild[%d %s]  rank_index:%d  rank_val:%d ",
			this->GetGuildID(), this->GetName(), _rank_index, _rank_val);
		return;
	}

	if (0 < cfg->count)
	{
		for (int reward_index = 0; reward_index < cfg->count; ++reward_index)
		{
			content_param.item_list[reward_index].item_id = cfg->reward_list[reward_index].item_id;
			content_param.item_list[reward_index].num = cfg->reward_list[reward_index].num;
			content_param.item_list[reward_index].is_bind = cfg->reward_list[reward_index].is_bind;
		}
	}

	content_param.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GUILDGONGXIAN] = cfg->gongxian;
	content_param.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = cfg->coin;

	// 日志
	gamelog::g_log_guild.buff_printf("Guild::GetCrossGuildActiveRankReward  guild[%d %s] "
		"rank_index:%d  rank_val:%d  gongxian:%d  coin:%d" ,
		this->GetGuildID(), this->GetName(),
		_rank_index , _rank_val , cfg->gongxian , cfg->coin);
	for (int reward_index = 0; reward_index < cfg->count; ++reward_index)
	{
		gamelog::g_log_guild.buff_printf("item[index:%d  item_id:%d  num:%d]  ",
			reward_index, cfg->reward_list[reward_index].item_id, cfg->reward_list[reward_index].num);
	}
	gamelog::g_log_guild.commit_buff(LL_INFO);

	m_guild_member_manager.SendMailToAllMember(MAIL_REASON_DEFAULT, content_param);
}

void Guild::OnHiddenGameRoleActiveGiftInfo(crossgameprotocal::HiddenGameRoleActiveGiftInfo * _info)
{
	if (NULL == _info)
		return;

	int index = GetDayIndex(_info->get_timestamp, GetZeroTime((unsigned int)EngineAdapter::Instance().Time()));
	if (0 != index)
		return; // 说明隔天了 数据可以直接丢掉

	GuildMember * member = m_guild_member_manager.GetMember(IntToUid(_info->role_id));
	if (NULL != member)
	{
		member->SetGuildDayActiveRewardFlag(_info->flag);
	}
}

void Guild::GmGuildActiveSettlement()
{
	// 家族周活跃个人排行榜结算
	this->GuildActiveWeekActivePersonRankSettlement();

	// 重置家族活跃度记录
	m_base_param.week_total_guild_active_val = 0;
	m_base_param.last_guild_active_change_times = (unsigned int)EngineAdapter::Instance().Time();
	this->SendGuildActiveInfo(NULL);
}

int Guild::GetSevenDayTotalActive() const
{
	int total_active = 0;
	for (int i = 0; i < GUILD_SEVEN_DAY_ACTIVE; ++i)
	{
		total_active += m_base_param.seven_day_active[i];
	}

	return total_active;
}

int Guild::GetTodayBossChapterId()
{
	int world_level = WorldStatus::Instance().GetWorldLevel(true);
	return LOGIC_CONFIG->GetGuildConfig()->GetBossChapterByWorldLevel(world_level);
}

void Guild::ResetBossDataByWeek()
{

}

void Guild::ResetBossDataByDay()
{
	m_boss_param.chapter_id = this->GetTodayBossChapterId();
	m_boss_param.remain_hp_percent = GuildBossParam::BOSS_HP_PERCENT;
	for (int i = 0; i < GuildBossParam::MONSTER_NUM; ++i)
	{
		m_boss_param.monster_remain_percent_list[i] = (short)GuildBossParam::BOSS_HP_PERCENT;
	}

	this->SetDirty(true);
}

void Guild::ResetBossDataOnVersionChange()
{
	m_boss_param.reset_flag = 1;

	m_boss_param.chapter_id = this->GetTodayBossChapterId();
	m_boss_param.remain_hp_percent = GuildBossParam::BOSS_HP_PERCENT;
	for (int i = 0; i < GuildBossParam::MONSTER_NUM; ++i)
	{
		m_boss_param.monster_remain_percent_list[i] = (short)GuildBossParam::BOSS_HP_PERCENT;
	}

	this->SetDirty(true);
}

void Guild::GetBossHpNum(int chapter_id, int boss_index, int cur_hp_percent_w, ARG_OUT long long* cur_hp, ARG_OUT long long* max_hp)
{
	const GuildBossCfg::GuildBossCfgItem * cfg = LOGIC_CONFIG->GetGuildConfig()->GetBossCfg(chapter_id);
	if (NULL == cfg) return;

	int monster_group_id = cfg->boss_group_id;
	long long total_hp = MonsterGroupPool::GetInstance().GetMonsterGroupAllMonsterTotalHp(monster_group_id);
	if (NULL != max_hp) *max_hp = total_hp;
	if (NULL != cur_hp) *cur_hp = static_cast<long long>(1.0 * total_hp / 10000 * cur_hp_percent_w);
}

void Guild::GmSetQiFu(Role * _role, int type, int param)
{
	switch (type)
	{
	case 0:
		{
			m_base_param.is_open_qifu = 1;
			const GuildOtherCfg* other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg();
			if (other_cfg != NULL)
			{
				if (param >= other_cfg->bless_all_add)m_base_param.qifu_value = other_cfg->bless_all_add;
				else m_base_param.qifu_value = param;
			}
			this->SendQifuInfo(_role);
		}
		break;
	default:
		break;
	}
}

void Guild::InitHallOfFameRank()
{
	m_battle_rank.clear();
	m_rich_rank.clear();
	m_kinder_rank.clear();

	int member_count = this->GetMemberManager()->GetMemberCount();
	for (int i = 0; i < member_count; ++i)
	{
		GuildMember* member = this->GetMemberManager()->GetMemberByIndex(i);
		if (NULL == member) continue;
		
		static HallOfFameInfo info;
		info.role_id = member->GetUid();
		member->GetOriginalName(info.name);
		for (int rank_type = HALLOFFAME_TYPE_BATTLE; rank_type < HALLOFFAME_TYPE_MAX; rank_type++)
		{
			int rank_value = member->GetHallOfFameVal(rank_type);
			unsigned int rank_tamp = member->GetLastChangeHallOfFameValTimes(rank_type);
			gamelog::g_log_guild.printf(LL_INFO, "%s member_info[%s,%d] rank[type:%d,value:%d]", __FUNCTION__, member->GetName(), member->GetUid(), rank_type, rank_value);
			if (rank_value <= 0)continue;
			info.value = rank_value;
			info.last_change_time = rank_tamp;
			info.prof = member->GetProfession();
			if (rank_type == HALLOFFAME_TYPE_BATTLE)
			{
				m_battle_rank.push_back(info);
			}
			else if(rank_type == HALLOFFAME_TYPE_RICH)
			{
				m_rich_rank.push_back(info);
			}
			else if (rank_type == HALLOFFAME_TYPE_KINDER)
			{
				m_kinder_rank.push_back(info);
			}
		}
	}

	if ((int)m_battle_rank.size() > 1)std::sort(m_battle_rank.begin(), m_battle_rank.end(), FallOfFameRankSort);
	if ((int)m_rich_rank.size() > 1)std::sort(m_rich_rank.begin(), m_rich_rank.end(), FallOfFameRankSort);
	if ((int)m_kinder_rank.size() > 1)std::sort(m_kinder_rank.begin(), m_kinder_rank.end(), FallOfFameRankSort);

	this->SetDirty(true);
}

void Guild::HallOfFameReq(Role * _role, int type, int param)
{
	if (NULL == _role)return;
	
	switch (type)
	{
	case Protocol::CSGuildHallOfFameReq::REQ_TYPE_INFO:
		this->SendRankInfo(_role, param);
		break;
	case Protocol::CSGuildHallOfFameReq::REQ_TYPE_QIFU:
		if (!this->IsOpenQiFu())
		{
			_role->NoticeNum(errornum::EN_GUILD_QIFU_NOT_OPEN);
			return;
		}
		this->QiFuReq(_role);
		break;
	case Protocol::CSGuildHallOfFameReq::REQ_TYPE_REWARD:
		{
			if (!this->IsOpenQiFu())
			{
				_role->NoticeNum(errornum::EN_GUILD_QIFU_NOT_OPEN);
				return;
			}
			const GuildOtherCfg* other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg();
			if (NULL == other_cfg)return;
			if (m_base_param.qifu_value < other_cfg->bless_all_add)
			{
				_role->NoticeNum(errornum::EN_GUILD_QIFU_VALUE_NOT_ENOUGH);
				return;
			}
			GuildMember * guild_member = m_guild_member_manager.GetMember(_role->GetUserId());
			if (NULL != guild_member)
			{
				guild_member->SetQiFuFlag();
				_role->GetRoleModuleManager()->ReCalcAttr(AttributeList::RECALC_REASON_GUILD_QIFU, __FUNCTION__);
				this->SendQifuInfo(_role);
			}
		}
		break;
	case Protocol::CSGuildHallOfFameReq::REQ_TYPE_QIFU_INFO:
		this->SendQifuInfo(_role);
		break;
	}
}

void Guild::QiFuReq(Role * _role)
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		this->HiddenQiFu(_role);
	}
	else
	{
		this->GameWorldQiFu(_role);
	}
}

void Guild::GameWorldQiFu(Role * _role)
{
	const GuildOtherCfg* other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg();
	if (NULL == other_cfg)return;
	if (!this->IsOpenQiFu())
	{
		_role->NoticeNum(errornum::EN_GUILD_QIFU_NOT_OPEN);
		return;
	}
	int &today_times = _role->GetRoleModuleManager()->GetGuildCommonData().today_guild_qifu_times;
	const BlessTimesList * times_cfg = LOGIC_CONFIG->GetGuildConfig()->GetBlessBaseCfg(today_times + 1);
	if (NULL == times_cfg)return;

	if (!_role->GetRoleModuleManager()->GetMoney()->MoneyTypeMoreThan(times_cfg->cost_type, times_cfg->bless_consume))return;

	if (!_role->GetRoleModuleManager()->GetMoney()->UseMoneyType(times_cfg->cost_type, times_cfg->bless_consume, __FUNCTION__))
	{
		return;
	}

	if (m_base_param.qifu_value + times_cfg->bless_add_once >= other_cfg->bless_all_add)
	{
		m_base_param.qifu_value = other_cfg->bless_all_add;
	}
	else
	{
		m_base_param.qifu_value += times_cfg->bless_add_once;
	}
	today_times += 1;
	_role->GetRoleModuleManager()->GetMoney()->OnAddMoney(MONEY_TYPE_GUILD_CONTRIBUTION, times_cfg->gongxian_reward, __FUNCTION__);
	this->SetDirty(true);
	this->SendQifuInfo(_role);
	//数据先一步同步到隐藏服
	static crossgameprotocal::GameHiddenGuildQiFuValueChange change;
	change.guild_id = m_base_param.guild_id;
	change.role_id = 0;
	change.value = m_base_param.qifu_value;

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&change, sizeof(change));
}

void Guild::HiddenQiFu(Role * _role)
{
	const GuildOtherCfg* other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg();
	if (NULL == other_cfg)return;
	if (!this->IsOpenQiFu())
	{
		_role->NoticeNum(errornum::EN_GUILD_QIFU_NOT_OPEN);
		return;
	}
	int &today_times = _role->GetRoleModuleManager()->GetGuildCommonData().today_guild_qifu_times;
	const BlessTimesList * times_cfg = LOGIC_CONFIG->GetGuildConfig()->GetBlessBaseCfg(today_times + 1);
	if (NULL == times_cfg)return;

	if (!_role->GetRoleModuleManager()->GetMoney()->MoneyTypeMoreThan(times_cfg->cost_type, times_cfg->bless_consume))return;

	if (!_role->GetRoleModuleManager()->GetMoney()->UseMoneyType(times_cfg->cost_type, times_cfg->bless_consume, __FUNCTION__))
	{
		return;
	}

	today_times += 1;
	_role->GetRoleModuleManager()->GetMoney()->OnAddMoney(MONEY_TYPE_GUILD_CONTRIBUTION, times_cfg->gongxian_reward, __FUNCTION__);

	static crossgameprotocal::HiddenGameQiFuChange change;
	change.guild_id = m_base_param.guild_id;
	change.role_id = _role->GetUID();
	change.add_val = times_cfg->bless_add_once;

	InternalComm::Instance().SendToGameThroughCross(LocalConfig::Instance().GetPlatType(), _role->GetUserId().db_index, (const char *)&change, sizeof(change));
}

void Guild::FromHiddenQifu(int role_id, int add_value)
{
	const GuildOtherCfg* other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg();
	if (NULL == other_cfg)return;
	if (!this->IsOpenQiFu())
	{
		return;
	}
	
	if (m_base_param.qifu_value + add_value >= other_cfg->bless_all_add)
	{
		m_base_param.qifu_value = other_cfg->bless_all_add;
	}
	else
	{
		m_base_param.qifu_value += add_value;
	}

	this->SetDirty(true);

	//数据先一步同步到隐藏服
	static crossgameprotocal::GameHiddenGuildQiFuValueChange change;
	change.guild_id = m_base_param.guild_id;
	change.role_id = role_id;
	change.value = m_base_param.qifu_value;

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&change, sizeof(change));
}

void Guild::FromGameWorld(int role_id, int value)
{
	const GuildOtherCfg* other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg();
	if (NULL == other_cfg)return;

	if (value >= other_cfg->bless_all_add)
	{
		m_base_param.qifu_value = other_cfg->bless_all_add;
	}
	else
	{
		m_base_param.qifu_value = value;
	}
	
	if (role_id > 0)
	{
		Role * _role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
		if (NULL != _role)
		{
			this->SendQifuInfo(_role);
		}
	}
}

void Guild::SetAllRankRefresh(int uid)
{
	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	if (m_battle_refresh_tamp == 0)m_battle_refresh_tamp = now_second + GUILD_HALL_OF_FAME_REFRESH_TAMP;//有数据之后20秒之后才刷新
	if (m_rich_refresh_tamp == 0)m_rich_refresh_tamp = now_second + GUILD_HALL_OF_FAME_REFRESH_TAMP;//有数据之后20秒之后才刷新
	if (m_kinder_refresh_tamp == 0)m_kinder_refresh_tamp = now_second + GUILD_HALL_OF_FAME_REFRESH_TAMP;//有数据之后20秒之后才刷新

	GuildMember* member = this->GetMemberManager()->GetMember(IntToUid(uid));
	if (NULL != member)
	{
		for (int rank_type = HALLOFFAME_TYPE_BATTLE; rank_type < HALLOFFAME_TYPE_MAX; rank_type++)
		{
			bool has_member = false;
			if (rank_type == HALLOFFAME_TYPE_BATTLE)
			{
				for (HallOfFameRankList::iterator it = m_battle_rank.begin(); it != m_battle_rank.end(); it++)
				{
					if (it->role_id == uid)
					{
						it->prof = member->GetProfession();
						it->value = member->GetHallOfFameVal(rank_type);
						it->last_change_time = member->GetLastChangeHallOfFameValTimes(rank_type);
						has_member = true;
					}
				}
			}
			else if (rank_type == HALLOFFAME_TYPE_RICH)
			{
				for (HallOfFameRankList::iterator it = m_rich_rank.begin(); it != m_rich_rank.end(); it++)
				{
					if (it->role_id == uid)
					{
						it->prof = member->GetProfession();
						it->value = member->GetHallOfFameVal(rank_type);
						it->last_change_time = member->GetLastChangeHallOfFameValTimes(rank_type);
						has_member = true;
					}
				}
			}
			else if (rank_type == HALLOFFAME_TYPE_KINDER)
			{
				for (HallOfFameRankList::iterator it = m_kinder_rank.begin(); it != m_kinder_rank.end(); it++)
				{
					if (it->role_id == uid)
					{
						it->prof = member->GetProfession();
						it->value = member->GetHallOfFameVal(rank_type);
						it->last_change_time = member->GetLastChangeHallOfFameValTimes(rank_type);
						has_member = true;
					}
				}
			}
			if (!has_member)
			{
				static HallOfFameInfo info;
				info.role_id = member->GetUid();
				member->GetOriginalName(info.name);
				int rank_value = member->GetHallOfFameVal(rank_type);
				unsigned int rank_tamp = member->GetLastChangeHallOfFameValTimes(rank_type);
				if (rank_value <= 0)continue;
				info.value = rank_value;
				info.last_change_time = rank_tamp;
				info.prof = member->GetProfession();
				if (rank_type == HALLOFFAME_TYPE_BATTLE)
				{
					m_battle_rank.push_back(info);
				}
				else if (rank_type == HALLOFFAME_TYPE_RICH)
				{
					m_rich_rank.push_back(info);
				}
				else if (rank_type == HALLOFFAME_TYPE_KINDER)
				{
					m_kinder_rank.push_back(info);
				}
			}
			gamelog::g_log_guild.printf(LL_INFO, "%s member_info[%s,%d] rank[type:%d,value:%d]", __FUNCTION__, member->GetName(), member->GetUid(), rank_type, member->GetHallOfFameVal(rank_type));
		}
	}
}

void Guild::OnRankValueAdd(Role * _role, int rank_type, int add_value)
{
	if (NULL == _role)return;
	if (rank_type < HALLOFFAME_TYPE_BATTLE || rank_type >= HALLOFFAME_TYPE_MAX)return;
	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	switch (rank_type)
	{
	case HALLOFFAME_TYPE_BATTLE:
		{
			GuildMember * guild_member = m_guild_member_manager.GetMember(_role->GetUID());
			if (NULL != guild_member)
			{
				guild_member->AddHallOfFameVal(rank_type, add_value);
				bool has_role = false;
				for (HallOfFameRankList::iterator it = m_battle_rank.begin(); it != m_battle_rank.end(); it++)
				{
					if (it->role_id == _role->GetUID())
					{
						it->value = guild_member->GetHallOfFameVal(rank_type);
						it->last_change_time = now_second;
						has_role = true;
					}
				}
				if (!has_role)
				{
					HallOfFameInfo new_role;
					new_role.role_id = _role->GetUID();
					new_role.last_change_time = now_second;
					new_role.value = guild_member->GetHallOfFameVal(rank_type);
					guild_member->GetOriginalName(new_role.name);
					new_role.prof = _role->GetProfession();
					m_battle_rank.push_back(new_role);
				}
				if (m_battle_refresh_tamp == 0)m_battle_refresh_tamp = now_second + GUILD_HALL_OF_FAME_REFRESH_TAMP;//有数据之后20秒之后才刷新
				gamelog::g_log_guild.printf(LL_INFO, "%s member_info[%s,%d] rank[type:%d,value:%d]", __FUNCTION__, guild_member->GetName(), guild_member->GetUid(), rank_type, guild_member->GetHallOfFameVal(rank_type));
			}
		}
		break;
	case HALLOFFAME_TYPE_RICH:
		{
			GuildMember * guild_member = m_guild_member_manager.GetMember(_role->GetUID());
			if (NULL != guild_member)
			{
				guild_member->AddHallOfFameVal(rank_type, add_value);
				bool has_role = false;
				for (HallOfFameRankList::iterator it = m_rich_rank.begin(); it != m_rich_rank.end(); it++)
				{
					if (it->role_id == _role->GetUID())
					{
						it->value = guild_member->GetHallOfFameVal(rank_type);
						it->last_change_time = now_second;
						has_role = true;
					}
				}
				if (!has_role)
				{
					HallOfFameInfo new_role;
					new_role.role_id = _role->GetUID();
					new_role.last_change_time = now_second;
					new_role.value = guild_member->GetHallOfFameVal(rank_type);
					guild_member->GetOriginalName(new_role.name);
					new_role.prof = _role->GetProfession();
					m_rich_rank.push_back(new_role);
				}
				if (m_rich_refresh_tamp == 0)m_rich_refresh_tamp = now_second + GUILD_HALL_OF_FAME_REFRESH_TAMP;//有数据之后20秒之后才刷新
				gamelog::g_log_guild.printf(LL_INFO, "%s member_info[%s,%d] rank[type:%d,value:%d]", __FUNCTION__, guild_member->GetName(), guild_member->GetUid(), rank_type, guild_member->GetHallOfFameVal(rank_type));
			}
		}
		break;
	case HALLOFFAME_TYPE_KINDER:
		{
			GuildMember * guild_member = m_guild_member_manager.GetMember(_role->GetUID());
			if (NULL != guild_member)
			{
				guild_member->AddHallOfFameVal(rank_type, add_value);
				bool has_role = false;
				for (HallOfFameRankList::iterator it = m_kinder_rank.begin(); it != m_kinder_rank.end(); it++)
				{
					if (it->role_id == _role->GetUID())
					{
						it->value = guild_member->GetHallOfFameVal(rank_type);
						it->last_change_time = now_second;
						has_role = true;
					}
				}
				if (!has_role)
				{
					HallOfFameInfo new_role;
					new_role.role_id = _role->GetUID();
					new_role.last_change_time = now_second;
					new_role.value = guild_member->GetHallOfFameVal(rank_type);
					guild_member->GetOriginalName(new_role.name);
					new_role.prof = _role->GetProfession();
					m_kinder_rank.push_back(new_role);
				}
				if (m_kinder_refresh_tamp == 0)m_kinder_refresh_tamp = now_second + GUILD_HALL_OF_FAME_REFRESH_TAMP;//有数据之后20秒之后才刷新
				gamelog::g_log_guild.printf(LL_INFO, "%s member_info[%s,%d] rank[type:%d,value:%d]", __FUNCTION__, guild_member->GetName(), guild_member->GetUid(), rank_type, guild_member->GetHallOfFameVal(rank_type));
			}
		}
		break;
	}

	if (m_battle_refresh_tamp > 0 && now_second > m_battle_refresh_tamp)
	{
		std::sort(m_battle_rank.begin(), m_battle_rank.end(), FallOfFameRankSort);
	}
	if (m_rich_refresh_tamp > 0 && now_second > m_rich_refresh_tamp)
	{
		std::sort(m_rich_rank.begin(), m_rich_rank.end(), FallOfFameRankSort);
	}
	if (m_kinder_refresh_tamp > 0 && now_second > m_kinder_refresh_tamp)
	{
		std::sort(m_kinder_rank.begin(), m_kinder_rank.end(), FallOfFameRankSort);
	}
}

void Guild::OnRankValueAddByUid(int uid, int rank_type, int add_value)
{
	if (rank_type < HALLOFFAME_TYPE_BATTLE || rank_type >= HALLOFFAME_TYPE_MAX)return;
	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	switch (rank_type)
	{
	case HALLOFFAME_TYPE_BATTLE:
		{
			GuildMember * guild_member = m_guild_member_manager.GetMember(IntToUid(uid));
			if (NULL != guild_member)
			{
				guild_member->AddHallOfFameVal(rank_type, add_value);
				bool has_role = false;
				for (HallOfFameRankList::iterator it = m_battle_rank.begin(); it != m_battle_rank.end(); it++)
				{
					if (it->role_id == uid)
					{
						it->value = guild_member->GetHallOfFameVal(rank_type);
						it->last_change_time = now_second;
						has_role = true;
					}
				}
				if (!has_role)
				{
					HallOfFameInfo new_role;
					new_role.role_id = uid;
					new_role.last_change_time = now_second;
					new_role.value = guild_member->GetHallOfFameVal(rank_type);
					guild_member->GetOriginalName(new_role.name);
					new_role.prof = guild_member->GetProfession();
					m_battle_rank.push_back(new_role);
				}
				if (m_battle_refresh_tamp == 0)m_battle_refresh_tamp = now_second + GUILD_HALL_OF_FAME_REFRESH_TAMP;//有数据之后20秒之后才刷新
				gamelog::g_log_guild.printf(LL_INFO, "%s member_info[%s,%d] rank[type:%d,value:%d]", __FUNCTION__, guild_member->GetName(), guild_member->GetUid(), rank_type, guild_member->GetHallOfFameVal(rank_type));
			}
		}
		break;
	case HALLOFFAME_TYPE_RICH:
		{
			GuildMember * guild_member = m_guild_member_manager.GetMember(IntToUid(uid));
			if (NULL != guild_member)
			{
				guild_member->AddHallOfFameVal(rank_type, add_value);
				bool has_role = false;
				for (HallOfFameRankList::iterator it = m_rich_rank.begin(); it != m_rich_rank.end(); it++)
				{
					if (it->role_id == uid)
					{
						it->value = guild_member->GetHallOfFameVal(rank_type);
						it->last_change_time = now_second;
						has_role = true;
					}
				}
				if (!has_role)
				{
					HallOfFameInfo new_role;
					new_role.role_id = uid;
					new_role.last_change_time = now_second;
					new_role.value = guild_member->GetHallOfFameVal(rank_type);
					guild_member->GetOriginalName(new_role.name);
					new_role.prof = guild_member->GetProfession();
					m_rich_rank.push_back(new_role);
				}
				if (m_rich_refresh_tamp == 0)m_rich_refresh_tamp = now_second + GUILD_HALL_OF_FAME_REFRESH_TAMP;//有数据之后20秒之后才刷新
				gamelog::g_log_guild.printf(LL_INFO, "%s member_info[%s,%d] rank[type:%d,value:%d]", __FUNCTION__, guild_member->GetName(), guild_member->GetUid(), rank_type, guild_member->GetHallOfFameVal(rank_type));
			}
		}
		break;
	case HALLOFFAME_TYPE_KINDER:
		{
			GuildMember * guild_member = m_guild_member_manager.GetMember(IntToUid(uid));
			if (NULL != guild_member)
			{
				guild_member->AddHallOfFameVal(rank_type, add_value);
				bool has_role = false;
				for (HallOfFameRankList::iterator it = m_kinder_rank.begin(); it != m_kinder_rank.end(); it++)
				{
					if (it->role_id == uid)
					{
						it->value = guild_member->GetHallOfFameVal(rank_type);
						it->last_change_time = now_second;
						has_role = true;
					}
				}
				if (!has_role)
				{
					HallOfFameInfo new_role;
					new_role.role_id = uid;
					new_role.last_change_time = now_second;
					new_role.value = guild_member->GetHallOfFameVal(rank_type);
					guild_member->GetOriginalName(new_role.name);
					new_role.prof = guild_member->GetProfession();
					m_kinder_rank.push_back(new_role);
				}
				if (m_kinder_refresh_tamp == 0)m_kinder_refresh_tamp = now_second + GUILD_HALL_OF_FAME_REFRESH_TAMP;//有数据之后20秒之后才刷新
				gamelog::g_log_guild.printf(LL_INFO, "%s member_info[%s,%d] rank[type:%d,value:%d]", __FUNCTION__, guild_member->GetName(), guild_member->GetUid(), rank_type, guild_member->GetHallOfFameVal(rank_type));
			}
		}
		break;
	}
}

void Guild::WeekRankReward()
{
	if (CrossConfig::Instance().IsHiddenServer())return;
	std::sort(m_battle_rank.begin(), m_battle_rank.end(), FallOfFameRankSort);
	std::sort(m_rich_rank.begin(), m_rich_rank.end(), FallOfFameRankSort);
	std::sort(m_kinder_rank.begin(), m_kinder_rank.end(), FallOfFameRankSort);
	const GuildOtherCfg* other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg();
	if (NULL == other_cfg)return;
	
	for (HallOfFameRankList::iterator it = m_battle_rank.begin(); it != m_battle_rank.end(); it++)
	{
		int rank_idx = std::distance(m_battle_rank.begin(), it) + 1;
		this->RankReward(&(*it), HALLOFFAME_TYPE_BATTLE, rank_idx);
	}

	for (HallOfFameRankList::iterator it = m_rich_rank.begin(); it != m_rich_rank.end(); it++)
	{
		int rank_idx = std::distance(m_rich_rank.begin(), it) + 1;
		this->RankReward(&(*it), HALLOFFAME_TYPE_RICH, rank_idx);
	}

	for (HallOfFameRankList::iterator it = m_kinder_rank.begin(); it != m_kinder_rank.end(); it++)
	{
		int rank_idx = std::distance(m_kinder_rank.begin(), it) + 1;
		this->RankReward(&(*it), HALLOFFAME_TYPE_KINDER, rank_idx);
	}
}

void Guild::RankReward(HallOfFameInfo * rank_info, int type, int rank_idx)
{
	const HallOfFameRank * reward_cfg = LOGIC_CONFIG->GetGuildConfig()->GetRankTypeCfg(type, rank_idx);
	if (NULL != reward_cfg)
	{
		static MailContentParam contentparam; contentparam.Reset();

		switch (type)
		{
		case HALLOFFAME_TYPE_BATTLE:
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_battle_rank_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_battle_rank_content, rank_idx);
			break;
		case HALLOFFAME_TYPE_RICH:
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_rich_rank_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_rich_rank_content, rank_idx);
			break;
		case HALLOFFAME_TYPE_KINDER:
			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_guild_kinder_rank_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_guild_kinder_rank_content, rank_idx);
			break;
		}

		if (reward_cfg->title > 0)
		{
			contentparam.item_list[0].item_id = reward_cfg->title;
			contentparam.item_list[0].num = 1;
			contentparam.item_list[0].is_bind = 1;
			const ItemBase *item_base = ITEMPOOL->GetItem(reward_cfg->title);
			if (item_base != NULL)contentparam.item_list[0].invalid_time = item_base->CalInvalidTime();
		}

		if (reward_cfg->gongxian > 0)
		{
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GUILDGONGXIAN] = reward_cfg->gongxian;
		}

		if (reward_cfg->coin > 0)
		{
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = reward_cfg->coin;
		}
		MailRoute::MailToUser(IntToUid(rank_info->role_id), MAIL_REASON_DEFAULT, contentparam);
	}
}

void Guild::SendRankInfo(Role * _role, int rank_type)
{
	if (NULL == _role)return;
	if (rank_type < HALLOFFAME_TYPE_BATTLE || rank_type >= HALLOFFAME_TYPE_MAX)return;
	Protocol::SCGuildHallOfFameInfo info;
	info.rank_type = rank_type;
	info.rank_count = 0;
	const GuildOtherCfg* other_cfg = LOGIC_CONFIG->GetGuildConfig()->GetOtherCfg();
	if (NULL == other_cfg)return;
	switch (rank_type)
	{
	case HALLOFFAME_TYPE_BATTLE:
		{
			for (HallOfFameRankList::iterator it = m_battle_rank.begin(); it != m_battle_rank.end(); it++)
			{
				if (other_cfg->rank_amount <= info.rank_count)break;
				if (info.rank_count == 0)
				{
					GuildMember * member = m_guild_member_manager.GetMember(IntToUid(it->role_id));
					if (member != NULL)
					{
						member->GetAppearance(&info.appearance_info);
					}
				}
				info.rank_list[info.rank_count].role_id = it->role_id;
				info.rank_list[info.rank_count].value = it->value;
				info.rank_list[info.rank_count].prof = it->prof;
				F_STRNCPY(info.rank_list[info.rank_count].name, it->name, sizeof(GameName));
				info.rank_count++;
			}
		}
		break;
	case HALLOFFAME_TYPE_RICH:
		{
			for (HallOfFameRankList::iterator it = m_rich_rank.begin(); it != m_rich_rank.end(); it++)
			{
				if (other_cfg->rank_amount <= info.rank_count)break;
				if (info.rank_count == 0)
				{
					GuildMember * member = m_guild_member_manager.GetMember(IntToUid(it->role_id));
					if (member != NULL)
					{
						member->GetAppearance(&info.appearance_info);
					}
				}
				info.rank_list[info.rank_count].role_id = it->role_id;
				info.rank_list[info.rank_count].value = it->value;
				info.rank_list[info.rank_count].prof = it->prof;
				F_STRNCPY(info.rank_list[info.rank_count].name, it->name, sizeof(GameName));
				info.rank_count++;
			}
		}
		break;
	case HALLOFFAME_TYPE_KINDER:
		{
			for (HallOfFameRankList::iterator it = m_kinder_rank.begin(); it != m_kinder_rank.end(); it++)
			{
				if (other_cfg->rank_amount <= info.rank_count)break;
				if (info.rank_count == 0)
				{
					GuildMember * member = m_guild_member_manager.GetMember(IntToUid(it->role_id));
					if (member != NULL)
					{
						member->GetAppearance(&info.appearance_info);
					}
				}
				info.rank_list[info.rank_count].role_id = it->role_id;
				info.rank_list[info.rank_count].value = it->value;
				info.rank_list[info.rank_count].prof = it->prof;
				F_STRNCPY(info.rank_list[info.rank_count].name, it->name, sizeof(GameName));
				info.rank_count++;
			}
		}
		break;
	}

	EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char *)&info, sizeof(info) - ((GUILD_MAX_MEMBER_COUNT - info.rank_count) * sizeof(info.rank_list[0])));
}

void Guild::SendQifuInfo(Role * _role)
{
	if (_role == NULL)return;
	GuildMember * guild_member = m_guild_member_manager.GetMember(_role->GetUserId());
	if (NULL == guild_member)return;
	Protocol::SCGuildQiFuInfo info;
	info.is_open = (int)m_base_param.is_open_qifu;
	info.qifu_value = m_base_param.qifu_value;
	info.today_qifu_times = _role->GetRoleModuleManager()->GetGuildCommonData().today_guild_qifu_times;
	info.is_get_qifu_attr = guild_member->GetQiFuFlag();
	if (NULL == _role)
	{
		m_guild_member_manager.SendInfoToAllMember((const char *)&info, sizeof(info));
	}
	else
	{
		EngineAdapter::Instance().NetSend(_role->GetNetId(), (const char *)&info, sizeof(info));
	}
}

void Guild::UpdateActiveRankByGameWorld(int _role_id, int _add_active_val)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	GuildMember * member = m_guild_member_manager.GetMember(IntToUid(_role_id));
	if (NULL == member) return;
	// 先更新帮派成员的活跃度信息
	int last_val = member->GetWeekActiveVal();

	// 进行排序
	WeekActiveInfo info;
	info.role_id = member->GetUid();
	info.active_val = last_val;		// 取旧值用于查询  
	info.last_change_time = member->GetLastChangeWeekActiveValTimes();

	// 记录了旧数据后 再更新玩家身上的数据
	member->AddWeekActiveVal(_add_active_val);

	// 先查看是否存在 , 存在则更新 不存在则
	int index = m_week_active_rank.GetIndex(info);
	bool need_insert = false;
	bool is_error_insert = false;  // 是否是容错插入 , 用于日志查BUG

	info.active_val = member->GetWeekActiveVal(); // 查询后 , 再取新值用于更新
	info.last_change_time = member->GetLastChangeWeekActiveValTimes();

	if (-1 == index)
	{
		need_insert = true;
	}
	else
	{
		SkipList<WeekActiveInfo>::Iterator it = m_week_active_rank[index];
		if (m_week_active_rank.End() != it)
		{
			int old_val = it->active_val;
			int old_times = it->last_change_time;

			m_week_active_rank.UpdateData(*it, info);
			int new_index = m_week_active_rank.GetIndex(info);

			gamelog::g_log_guild.printf(LL_INFO, "[Guild::UpdateActiveRankByGameWorld Update] [guild[%d %s]  role[%d %s]  "
				"list_size[%u]  old_index[%d]  new_index[%d]  old_val[%d]  old_times[%u]  new_val[%d] new_times[%u] ]",
				m_base_param.guild_id, m_base_param.guild_name, member->GetUid(), member->GetName(),
				m_week_active_rank.Size(), index, new_index, old_val, old_times, info.active_val, info.last_change_time);
		}
		else
		{
			// 记录错误日志 
			gamelog::g_log_guild.printf(LL_CRITICAL, "[Guild::UpdateActiveRankByGameWorld operation[] ERROR] [guild[%d %s]  check_user[%d %s]  "
				"list_size[%u]  old_index[%d]  new_val[%d] new_times[%u] ]",
				m_base_param.guild_id, m_base_param.guild_name, member->GetUid(), member->GetName(),
				m_week_active_rank.Size(), index, info.active_val, info.last_change_time);

			// 容错
			need_insert = true;
			is_error_insert = true;
		}
	}

	if (need_insert)
	{
		// 列表的长度是以最大帮派成员数量为长度的, 所以不需要判断列表是否已满
		m_week_active_rank.Insert(info);
		int new_index = m_week_active_rank.GetIndex(info);

		gamelog::g_log_guild.printf(LL_INFO, "[Guild::UpdateActiveRankByGameWorld Insert] [is_error_insert:%d] [guild[%d %s]  role[%d %s]  "
			"list_size[%u]  new_index[%d] new_val[%d] new_times[%u] ]",
			is_error_insert, m_base_param.guild_id, m_base_param.guild_name, member->GetUid(), member->GetName(),
			m_week_active_rank.Size(), new_index, info.active_val, info.last_change_time);
	}

	this->SyncActiveInfoToHidden(member );
}