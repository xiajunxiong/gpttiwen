#include <algorithm>

#include "scene/scene.h"
#include "world.h"
#include "npc/npcmanager.h"
#include "obj/character/npc_monster.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "item/itempool.h"
#include "item/itembase.h"
#include "global/usercache/usercache.hpp"
#include "scene/scenemanager.hpp"
#include "obj/character/role.h"
#include "world.h"
#include "global/worldstatus/worldstatus.hpp"
#include "globalconfig/globalconfig.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/roleactivity/roleactivity.hpp"
#include "config/sharedconfig/sharedconfig.h"

#include "config/logicconfigmanager.hpp"
#include "config/activityconfig/worldboss/worldbossconfig.hpp"
#include "protocol/msgworldboss.h"
#include "activityworldboss.hpp"
#include "battle/battle_manager_local.hpp"
#include "other/funopen/funopendef.hpp"
#include "other/funopen/funopenconfig.hpp"
#include "gameworld/internalcomm.h"
#include "gamelog.h"
#include "global/topicmanager/topicmanager.hpp"

static bool  WorldBossHealSort(const WorldBossRoleInfo & a, const WorldBossRoleInfo & b);
static bool  WorldBossHurtSort(const WorldBossRoleInfo & a, const WorldBossRoleInfo & b);

ActivityWorldBoss::ActivityWorldBoss(ActivityManager *activity_manager) : Activity(activity_manager, ACTIVITY_TYPE_WORLD_BOSS)
{
	m_boss_npc_id = 0;
	m_boss_npc_seq = 0;
	m_send_mail_boss_npc_id = 0;

	m_boss_scene_id = 0;
	m_boss_scene_key = 0;
	m_boss_pos = Posi(0, 0);

	m_boss_cur_hp = 0;
	m_boss_max_hp = 0;

	m_next_broadcast_time = 0;

	m_is_kill = false;

	m_has_send_reward = true;
	m_is_need_update_rank = 0;
	m_is_need_update_rank = false;

	m_reduce_hp_begin_time = 0;
}

ActivityWorldBoss::~ActivityWorldBoss()
{

}

void ActivityWorldBoss::OnActivityStatusChange(int from_status, int to_status)
{
	Activity::OnActivityStatusChange(from_status, to_status);
	switch (to_status)
	{
		case ACTIVITY_STATUS_CLOSE:
			{
				this->ActBossBroadcast(m_is_kill);
	
				if (!CrossConfig::Instance().IsHiddenServer())
				{
					return;
				}

				BattleManagerLocal::GetInstance().ForceFinishBattleMode(BATTLE_MODE_WORLD_BOSS, 0, USE_BATTLE_SERVER_INDEX); // 只发给指定战斗服

				this->ClearBoss();
			}
			break;
		case ACTIVITY_STATUS_STANDY:
			{
				m_open_day = GetDayIndex(SHAREDCONFIG->GetOpenServerConfig().GetOpenServerZeroTime(), EngineAdapter::Instance().Time()) + 1;
				m_is_kill = false;
				m_next_broadcast_time = EngineAdapter::Instance().Time() + 300;
				this->Broadcast(10);
			}
			break;
		case ACTIVITY_STATUS_OPEN:
			{
				m_role_info.clear();
				m_req_boss_info_role.clear();

				if (!CrossConfig::Instance().IsHiddenServer())
				{
					return;
				}

				if (!this->CreateBoss())
				{
					this->ForceToCloseState();
					return;
				}

				this->SendBossPosInfo();
				this->ActBossBroadcast();
				this->SendRoleInfo();
				this->SyncInfoToLocalServer(true);

				m_is_kill = false;
				m_has_send_reward = false;

				m_last_update_rank_time = 0;
				m_is_need_update_rank = false;
				m_hurt_rank.clear();
				m_heal_rank.clear();

				m_reduce_hp_begin_time = EngineAdapter::Instance().Time();
			}
			break;
	}
}

void ActivityWorldBoss::Update(unsigned long interval, time_t now_second)
{
	Activity::Update(interval, now_second);
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (this->IsActivityStandy() && m_next_broadcast_time != 0 && now_second >= m_next_broadcast_time)
	{
		m_next_broadcast_time = 0;

		this->Broadcast(5);
	}

	if (this->IsActivityOpen())
	{
		if (m_is_need_update_rank && now_second >= m_last_update_rank_time + 3)
		{
			m_last_update_rank_time = now_second;
			m_is_need_update_rank = false;

			this->UpdateRank();
		}

		if (!m_is_kill && now_second >= m_reduce_hp_begin_time + 30)
		{
			m_reduce_hp_begin_time = now_second;

			double one_percent_hp = 0.01 * m_boss_max_hp;
			if (m_boss_cur_hp > one_percent_hp)
			{
				m_boss_cur_hp -= (int)one_percent_hp;
				battlegameprotocol::GameBattleWorldBossChangeHp req;
				req.cur_hp = m_boss_cur_hp;
				req.change_value = -(long long)one_percent_hp;
				World::GetInstWorld()->SendMsgToOneBattle(USE_BATTLE_SERVER_INDEX, (const char *)&req, sizeof(req));
			}
		}
	}


}

void ActivityWorldBoss::OnUserLogin(Role * role)
{
	Activity::OnUserLogin(role);

	if (this->IsActivityOpen())
	{
		this->SendBossPosInfo(role);
		this->SendRoleInfo(role);
	}
}

void ActivityWorldBoss::OnUserLogout(Role * role)
{
	this->CancelBossInfo(role);
}

void ActivityWorldBoss::SetBossCurHp(int cur_hp)
{
	if (m_boss_cur_hp <= 0)
	{
		return; // boss已死
	}

	m_boss_cur_hp = GetMin(cur_hp, m_boss_max_hp);
}

void ActivityWorldBoss::SyncBossCurHp(int cur_hp, int modify_hp)
{
	if (m_boss_cur_hp <= 0)
	{
		return; // boss已死
	}

	int local_cur_hp = m_boss_cur_hp + modify_hp;
	if (local_cur_hp <= 0)
	{
		if (cur_hp <= 0)
		{
			cur_hp = 0;
		}
		else
		{
			cur_hp = 1;
		}
	}
	else
	{
		cur_hp = local_cur_hp;
	}

	cur_hp = GetMin(cur_hp, m_boss_max_hp);
	this->SetBossCurHp(cur_hp);

	this->SendBossInfo();
	this->SyncInfoToLocalServer();
}

void ActivityWorldBoss::OnKillBoss()
{
	// 打赢了BOSS，给奖励 且结束活动。。。
	if (this->IsActivityOpen())
	{
		m_is_kill = true;
		this->ForceToNextState(); // 结束活动
	}
}

void ActivityWorldBoss::AddJoinRecord(int uid, int add_hurt, int add_heal)
{
	WorldBossRoleInfo & role_info = m_role_info[uid];
	role_info.uid = uid;
	role_info.hurt += add_hurt;
	role_info.heal += add_heal;

	m_is_need_update_rank = true;

	Role * join_role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
	if (join_role)
	{
		this->SendRoleInfo(join_role);
	}
}

void ActivityWorldBoss::SendReward()
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return; // 统一在跨服发放奖励
	}

	if (m_role_info.size() == 0)
	{
		return;
	}

	if (m_has_send_reward)
	{
		return;
	}

	m_has_send_reward = true;

	const WorldBossOtherCfg &  other_cfg = LOGIC_CONFIG->GetWorldBossConfig()->GetOtherCfg();

	//每个榜单n个人，输出奖和治疗奖为输出和治疗的前n名，幸运奖随机抽取n个参与活动的玩家（剔除已在输出和治疗榜上的玩家）
	this->UpdateRank();
	const std::vector<WorldBossRoleInfo> & hurt_rank = m_hurt_rank;
	const std::vector<WorldBossRoleInfo> & heal_rank = m_heal_rank;
	std::vector<WorldBossRoleInfo> luck_list = this->GetLucklist();
	Protocol::SCWorldBossResult result_protocol;

	{
		//发放参与奖励
		for (std::map<int, WorldBossRoleInfo>::const_iterator iter = m_role_info.begin(); iter != m_role_info.end(); ++iter)
		{
			UserCacheNode * node = UserCacheManager::Instance().GetUserNode(iter->second.uid);
			if (node == NULL)
			{
				continue;
			}

			WorldBossRankRewardGroupCfg  join_reward_cfg = LOGIC_CONFIG->GetWorldBossConfig()->GetWorldBossRankRewardCfg(node->level, WORLD_BOSS_REWARD_TYPE_JOIN);
			//发放参予奖励
			static MailContentParam contentparam; contentparam.Reset();
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = join_reward_cfg.bind_coin;
			::FillMailContentParam(contentparam, join_reward_cfg.rewards);

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_boss_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_boss_reward, m_send_mail_boss_npc_id);

			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(IntToUid(iter->first), MAIL_REASON_DEFAULT, contentparam);
			}
		}
	}
	
	{			
		for (int rank_index = 0; rank_index < (int)hurt_rank.size() && rank_index < other_cfg.world_boss_hurt_rank_num; ++rank_index)
		{
			UserCacheNode * node = UserCacheManager::Instance().GetUserNode(hurt_rank[rank_index].uid);
			if (node == NULL)
			{
				continue;
			}

			WorldBossRankRewardGroupCfg hurt_reward_cfg = LOGIC_CONFIG->GetWorldBossConfig()->GetWorldBossRankRewardCfg(node->level, WORLD_BOSS_REWARD_TYPE_HURT);
			static MailContentParam contentparam; contentparam.Reset();
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = hurt_reward_cfg.bind_coin;
			::FillMailContentParam(contentparam, hurt_reward_cfg.rewards);

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_boss_hurt_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_boss_hurt_reward, rank_index + 1);
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(IntToUid(hurt_rank[rank_index].uid), MAIL_REASON_DEFAULT, contentparam);
			}

			if (rank_index < ACTIVITY_WORLD_BOSS_MAX_RANK_ROLE_NUM)
			{
				Protocol::SCWorldBossResult::RankRoleInfo role_info;

				role_info.role_id = node->uid;
				node->GetName(role_info.role_name);
				result_protocol.hurt_rank[rank_index] = role_info;
			}
		}		
	}

	{			
		for (int rank_index = 0; rank_index < (int)heal_rank.size() && rank_index < other_cfg.world_boss_heal_rank_num; ++rank_index)
		{
			UserCacheNode * node = UserCacheManager::Instance().GetUserNode(heal_rank[rank_index].uid);
			if (node == NULL)
			{
				continue;
			}

			WorldBossRankRewardGroupCfg  heal_reward_cfg = LOGIC_CONFIG->GetWorldBossConfig()->GetWorldBossRankRewardCfg(node->level, WORLD_BOSS_REWARD_TYPE_HEAL);
			static MailContentParam contentparam; contentparam.Reset();
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = heal_reward_cfg.bind_coin;
			::FillMailContentParam(contentparam, heal_reward_cfg.rewards);

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_boss_heal_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_boss_heal_reward, rank_index + 1);
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(IntToUid(heal_rank[rank_index].uid), MAIL_REASON_DEFAULT, contentparam);
			}

			if (rank_index < ACTIVITY_WORLD_BOSS_MAX_RANK_ROLE_NUM)
			{
				Protocol::SCWorldBossResult::RankRoleInfo role_info;
				role_info.role_id = node->uid;
				node->GetName(role_info.role_name);
				result_protocol.heal_rank[rank_index] = role_info;
			}
		}		
	}

	{
		std::random_shuffle(luck_list.begin(), luck_list.end());
		//发放幸运奖
		for (int rank_index = 0; rank_index < (int)luck_list.size() && rank_index < other_cfg.world_boss_luck_rank_num; ++rank_index)
		{
			UserCacheNode * node = UserCacheManager::Instance().GetUserNode(luck_list[rank_index].uid);
			if (NULL == node)
			{
				continue;
			}
			WorldBossRankRewardGroupCfg  luck_reward_cfg = LOGIC_CONFIG->GetWorldBossConfig()->GetWorldBossRankRewardCfg(node->level, WORLD_BOSS_REWARD_TYPE_LUCK);

			static MailContentParam contentparam; contentparam.Reset();
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = luck_reward_cfg.bind_coin;
			for (int i = 0; i < (int)luck_reward_cfg.rewards.size() && i < MAX_ATTACHMENT_ITEM_NUM; ++i)
			{
				const ItemConfigData & reward = luck_reward_cfg.rewards[i];

				const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);

				if (item_base != NULL)
				{
					contentparam.item_list[i].item_id = reward.item_id;
					contentparam.item_list[i].num = reward.num;
					contentparam.item_list[i].is_bind = (reward.is_bind) ? 1 : 0;
					contentparam.item_list[i].invalid_time = item_base->CalInvalidTime();
				}
			}

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_world_boss_luck_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_world_boss_luck_reward, rank_index + 1);
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(IntToUid(luck_list[rank_index].uid), MAIL_REASON_DEFAULT, contentparam);
			}

			if (rank_index < ACTIVITY_WORLD_BOSS_MAX_RANK_ROLE_NUM)
			{
				Protocol::SCWorldBossResult::RankRoleInfo role_info;
				role_info.role_id = node->uid;
				node->GetName(role_info.role_name);
				result_protocol.luck_rank[rank_index] = role_info;
			}
		}
		
	}

	crossgameprotocal::HiddenGameWorldBossSyncParticipateUidList sync_uid_list;
	sync_uid_list.uid_num = 0;
	sync_uid_list.is_start = 1;
	for (std::map<int, WorldBossRoleInfo>::iterator iter = m_role_info.begin(); iter != m_role_info.end(); ++iter)
	{
		Role * join_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(iter->first));

		if (NULL == join_role)
		{	
			sync_uid_list.uid_list[sync_uid_list.uid_num++] = iter->first;
			if (sync_uid_list.uid_num >= ARRAY_ITEM_COUNT(sync_uid_list.uid_list))
			{
				InternalComm::Instance().SendToAllGameThroughCross(&sync_uid_list, sizeof(sync_uid_list));
				sync_uid_list.uid_num = 0;
				sync_uid_list.is_start = 0;
			}
					
			continue;
		}

		EngineAdapter::Instance().NetSend(join_role->GetNetId(), &result_protocol, sizeof(result_protocol));
	}

	if (sync_uid_list.uid_num > 0)
	{
		InternalComm::Instance().SendToAllGameThroughCross(&sync_uid_list, sizeof(sync_uid_list));
		sync_uid_list.uid_num = 0;
		sync_uid_list.is_start = 0;
	}

	{
		crossgameprotocal::HiddenGameWorldBossSyncRankResultMsg sync_result;
		UNSTD_STATIC_CHECK(sizeof(sync_result.msg_content) > sizeof(result_protocol));
		sync_result.length = sizeof(result_protocol);
		memcpy(sync_result.msg_content, &result_protocol, sizeof(result_protocol));

		int length = sizeof(sync_result) - sizeof(sync_result.msg_content) + sync_result.length;
		InternalComm::Instance().SendToAllGameThroughCross(&sync_result, length);
	}
}

void ActivityWorldBoss::HiddenGameSendRankResult(crossgameprotocal::HiddenGameWorldBossSyncRankResultMsg* msg)
{
	if (msg->length != sizeof(Protocol::SCWorldBossResult))
	{
		return;
	}

	for (std::map<int, WorldBossRoleInfo>::iterator iter = m_role_info.begin(); iter != m_role_info.end(); ++iter)
	{
		Role * join_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(iter->first));

		if (join_role == NULL)
		{
			continue;
		}

		EngineAdapter::Instance().NetSend(join_role->GetNetId(), msg->msg_content, msg->length);
	}

	// 发给那些从跨服回了原服的人
	std::set<int>::iterator it = m_send_rank_result_uid_set.begin();
	for (; it != m_send_rank_result_uid_set.end(); ++it)
	{
		if (m_role_info.find(*it) != m_role_info.end()) // 之前已经发过就不发了
		{
			continue;
		}

		Role * join_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(*it));

		if (join_role == NULL)
		{
			continue;
		}

		EngineAdapter::Instance().NetSend(join_role->GetNetId(), msg->msg_content, msg->length);
	}

	this->GameClearBossInfo();

	m_role_info.clear();
}

void ActivityWorldBoss::HiddenGameSyncUidList(crossgameprotocal::HiddenGameWorldBossSyncParticipateUidList* msg)
{
	if (msg->is_start > 0)
	{
		m_send_rank_result_uid_set.clear();
	}

	const std::set<int>& allow_server_set = LocalConfig::Instance().GetAllowServerIdSet();
	for (int i = 0; i < msg->uid_num; ++i)
	{
		if (allow_server_set.find(IntToUid(msg->uid_list[i]).db_index) == allow_server_set.end())
		{
			continue; // 不是本服的，跳过
		}
		
		m_send_rank_result_uid_set.insert(msg->uid_list[i]);
	}
}

void ActivityWorldBoss::SendBossInfo(Role * role /* = NULL */)
{
	Protocol::SCWorldBossInfo boss_info_protocol;
	boss_info_protocol.boss_npc_id = m_boss_npc_id;
	boss_info_protocol.boss_curr_hp = m_boss_cur_hp;
	boss_info_protocol.boss_max_hp = m_boss_max_hp;

	if (role != NULL)
	{
		m_req_boss_info_role.insert(role->GetUID());
		EngineAdapter::Instance().NetSend(role->GetNetId(), &boss_info_protocol, sizeof(boss_info_protocol));
	}
	else
	{
		for (std::set<int>::iterator iter = m_req_boss_info_role.begin(); iter != m_req_boss_info_role.end(); ++iter)
		{
			Role * temp_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(*iter));

			if (temp_role == NULL)
			{
				continue;
			}

			EngineAdapter::Instance().NetSend(temp_role->GetNetId(), &boss_info_protocol, sizeof(boss_info_protocol));
		}
	}
}

void ActivityWorldBoss::CancelBossInfo(Role * role)
{
	if (role == NULL)
	{
		return;
	}
	m_req_boss_info_role.erase(role->GetUID());
}

void ActivityWorldBoss::SendBossPosInfo(Role * role)
{
	Protocol::SCWorldBossPosInfo boss_pos_info_protocol;
	boss_pos_info_protocol.boss_npc_id = m_boss_npc_id;
	boss_pos_info_protocol.scene_id = m_boss_scene_id;
	boss_pos_info_protocol.pos_x = m_boss_pos.x;
	boss_pos_info_protocol.pos_y = m_boss_pos.y;

	if (role != NULL)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &boss_pos_info_protocol, sizeof(boss_pos_info_protocol));
	}
	else
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&boss_pos_info_protocol, sizeof(boss_pos_info_protocol));
	}
}

void ActivityWorldBoss::SendRoleInfo(Role * role)
{
	Protocol::SCWorldBossRoleInfo protocol;
	protocol.play_times = 0;
	protocol.hurt = 0;
	protocol.heal = 0;
	if (role != NULL)
	{
		std::map<int, WorldBossRoleInfo>::iterator it = m_role_info.find(role->GetUID());
		if (it != m_role_info.end())
		{
			WorldBossRoleInfo & role_info = m_role_info[role->GetUID()];
			protocol.play_times = role_info.play_times;
			protocol.hurt = role_info.hurt;
			protocol.heal = role_info.heal;
		}
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}
	else
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&protocol, sizeof(protocol));
	}
}

int ActivityWorldBoss::GetWorldBossPlayTimes(int uid)
{
	//std::map<int, WorldBossRoleInfo>::iterator iter = m_role_info.find(uid);
	//if (iter != m_role_info.end())
	//{
	//	return iter->second.play_times;
	//}

	return 0;
}

void ActivityWorldBoss::AddWorldBossPlayTimes(int uid, int play_times)
{
	WorldBossRoleInfo & role_info = m_role_info[uid];
	role_info.play_times += 1;
	
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(uid);
	if (role != NULL)
	{
		this->SendRoleInfo(role);
	}
}

void ActivityWorldBoss::SyncInfoToLocalServer(bool is_send_boss_pos /* = false */)
{
	crossgameprotocal::CrossGameSyncWorldBossInfo msg;

	msg.m_boss_npc_id = m_boss_npc_id;
	msg.m_boss_npc_seq = m_boss_npc_seq;
	msg.m_send_mail_boss_npc_id = m_send_mail_boss_npc_id;

	msg.m_boss_scene_id = m_boss_scene_id;
	msg.m_boss_scene_key = m_boss_scene_key;
	msg.m_boss_pos = m_boss_pos;

	msg.m_boss_cur_hp = m_boss_cur_hp;
	msg.m_boss_max_hp = m_boss_max_hp;

	msg.m_open_day = m_open_day;
	msg.m_is_kill = m_is_kill ? 1 : 0;
	msg.is_send_boss_pos = is_send_boss_pos ? 1 : 0;
	msg.reserve_sh = 0;
	InternalComm::Instance().SendToAllGameThroughCross((const char*)&msg, sizeof(msg));
}

void ActivityWorldBoss::SetInfo(crossgameprotocal::CrossGameSyncWorldBossInfo * msg)
{
	if (msg == NULL)return;
	m_boss_npc_id = msg->m_boss_npc_id;
	m_boss_npc_seq = msg->m_boss_npc_seq;
	m_send_mail_boss_npc_id = msg->m_send_mail_boss_npc_id;

	m_boss_scene_id = msg->m_boss_scene_id;
	m_boss_scene_key = msg->m_boss_scene_key;
	m_boss_pos = msg->m_boss_pos;

	m_boss_cur_hp = msg->m_boss_cur_hp;
	m_boss_max_hp = msg->m_boss_max_hp;

	m_open_day = msg->m_open_day;
	m_is_kill = msg->m_is_kill == 1 ? 1 : 0;
}

void ActivityWorldBoss::Settlement()
{
	if (!this->IsActivityClose())
	{
		return;
	}

	this->SendReward();

	m_role_info.clear();
	m_req_boss_info_role.clear();
}

void ActivityWorldBoss::SendRankInfo(Role * role)
{
	Protocol::SCWorldBossRankInfo protocol;
	protocol.type = 0;
	protocol.rank_count = 0;
	for (int i = 0; i < (int)m_hurt_rank.size() && i < ARRAY_LENGTH(protocol.rank_list); ++i)
	{
		protocol.rank_list[i].role_id = m_hurt_rank[i].uid;
		protocol.rank_list[i].parm1 = m_hurt_rank[i].hurt;
		memset(protocol.rank_list[i].role_name, 0, sizeof(protocol.rank_list[i].role_name));
		protocol.rank_list[i].level = 1;
		protocol.rank_list[i].prof = 0;

		UserCacheNode * node = UserCacheManager::Instance().GetUserNode(m_hurt_rank[i].uid);
		if (node != NULL)
		{
			protocol.rank_list[i].level = node->level;
			protocol.rank_list[i].prof = node->profession;
			node->GetName(protocol.rank_list[i].role_name);
		}

		protocol.rank_count += 1;
	}

	//缩减下发长度
	int length = sizeof(protocol) - (ARRAY_LENGTH(protocol.rank_list) - protocol.rank_count) * sizeof(protocol.rank_list[0]);
	if (NULL != role)
	{
		role->GetRoleModuleManager()->NetSend(&protocol, length);
	}
	else
	{
		TopicManager::Instance()->OnTopicUpdate(TOPIC_TYPE_WORLD_BOSS_1, (const char *)&protocol, length);
	}

	protocol.type = 1;
	protocol.rank_count = 0;
	for (int i = 0; i < (int)m_heal_rank.size() && i < ARRAY_LENGTH(protocol.rank_list); ++i)
	{
		protocol.rank_list[i].role_id = m_heal_rank[i].uid;
		protocol.rank_list[i].parm1 = m_heal_rank[i].heal;
		memset(protocol.rank_list[i].role_name, 0, sizeof(protocol.rank_list[i].role_name));
		protocol.rank_list[i].level = 1;
		protocol.rank_list[i].prof = 0;

		UserCacheNode * node = UserCacheManager::Instance().GetUserNode(m_heal_rank[i].uid);
		if (node != NULL)
		{
			protocol.rank_list[i].level = node->level;
			protocol.rank_list[i].prof = node->profession;
			node->GetName(protocol.rank_list[i].role_name);
		}

		protocol.rank_count += 1;
	}

	//缩减下发长度
	length = sizeof(protocol) - (ARRAY_LENGTH(protocol.rank_list) - protocol.rank_count) * sizeof(protocol.rank_list[0]);
	if (NULL != role)
	{
		role->GetRoleModuleManager()->NetSend(&protocol, length);
	}
	else
	{
		TopicManager::Instance()->OnTopicUpdate(TOPIC_TYPE_WORLD_BOSS_1, (const char *)&protocol, length);
	}
}

void ActivityWorldBoss::Broadcast(int minute)
{
	const WorldBossMonsterCfg * cfg = LOGIC_CONFIG->GetWorldBossConfig()->GetWorldBossMonsterCfg(m_open_day);
	if (cfg == NULL)
	{
		return;
	}
	// 传闻
	int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_boss_broadcast, cfg->ncp_monster_id, minute, cfg->scene_id);
	if (length > 0)
	{
		World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT), 
		0, 0, this->GetActivityLimitLevel(ACTIVITY_TYPE_WORLD_BOSS), 0);
	}
}

void ActivityWorldBoss::ActBossBroadcast(bool is_kill)
{
	const WorldBossMonsterCfg * cfg = LOGIC_CONFIG->GetWorldBossConfig()->GetWorldBossMonsterCfg(m_open_day);
	if (cfg == NULL)
	{
		return;
	}
	if (this->IsActivityOpen())
	{
		// 传闻
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_boss_appear_broadcast, cfg->ncp_monster_id, cfg->scene_id);
		if (length > 0)
		{
			World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT),
				0, 0, this->GetActivityLimitLevel(ACTIVITY_TYPE_WORLD_BOSS), 0);
		}
	}
	else if (this->IsActivityClose())
	{
		if (is_kill)
		{
			// 传闻
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_boss_kill_broadcast, cfg->ncp_monster_id);
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT),
					 0, 0, this->GetActivityLimitLevel(ACTIVITY_TYPE_WORLD_BOSS), 0);
			}
		}
		else
		{
			// 传闻
			int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_world_boss_not_kill_broadcast, cfg->ncp_monster_id);
			if (length > 0)
			{
				World::GetInstWorld()->GetSceneManager()->SystemMsgAll(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_ROLLING_NOTICE, SYS_MSG_SYSTEM_CHAT), 
					0, 0, this->GetActivityLimitLevel(ACTIVITY_TYPE_WORLD_BOSS), 0);
			}
		}
	}
}

bool ActivityWorldBoss::CreateBoss()
{
	const WorldBossMonsterCfg * cfg = LOGIC_CONFIG->GetWorldBossConfig()->GetWorldBossMonsterCfg(m_open_day);
	if (cfg == NULL)
	{
		gamelog::g_log_activity.printf(LL_ERROR, "ActivityWorldBoss::CreateBoss WorldBossMonsterCfg NULL, open_day[%d]", m_open_day);
		return false;
	}

	Posi boss_pos = LOGIC_CONFIG->GetWorldBossConfig()->GetRandPosByPosGroupID(cfg->coordinate_group_id);
	Scene* scene = World::GetInstWorld()->GetSceneManager()->GetSceneById(cfg->scene_id, COMMON_SCENE_KEY);
	if (NULL == scene)
	{
		gamelog::g_log_activity.printf(LL_ERROR, "ActivityWorldBoss::CreateBoss scene NULL, scene_id[%d]", cfg->scene_id);
		return false;
	}

	NPCMonster* npc_boss = NpcManager::Instance().CreateNPCMonster(cfg->ncp_monster_id, scene, boss_pos, BATTLE_MODE_WORLD_BOSS);
	if (NULL == npc_boss)
	{
		gamelog::g_log_activity.printf(LL_ERROR, "ActivityWorldBoss::CreateBoss npc_boss NULL, ncp_monster_id[%d] pos[%d,%d]", cfg->ncp_monster_id, boss_pos.x, boss_pos.y);
		return false;
	}

	npc_boss->SetBattleMode(BATTLE_MODE_WORLD_BOSS);
	m_boss_npc_id = cfg->ncp_monster_id;
	m_boss_npc_seq = npc_boss->GetNpcSeq();

	m_boss_scene_id = cfg->scene_id;
	m_boss_scene_key = COMMON_SCENE_KEY;
	m_boss_pos = boss_pos;

	m_boss_max_hp = cfg->boss_max_hp;
	m_boss_cur_hp = m_boss_max_hp;

	return true;
}

void ActivityWorldBoss::ClearBoss()
{
	//清掉BOSS（如果还活着的话）
	NPCMonster* npc_boss = NpcManager::Instance().GetNPCMonster(m_boss_npc_seq);
	if (NULL == npc_boss)
	{
		return;
	}

	if (npc_boss->GetScene()->GetSceneID() != m_boss_scene_id)
	{
		return;
	}

	if (npc_boss->GetScene()->GetSceneKey() != m_boss_scene_key)
	{
		return;
	}

	// 最后删掉npc_boss即可
	npc_boss->GetScene()->DeleteObj(npc_boss->GetId());

	//先赋值后再清0,否则后面发送邮件奖励会出错
	m_send_mail_boss_npc_id = m_boss_npc_id;
	m_boss_npc_id = 0;
	m_boss_npc_seq = 0;

	m_boss_scene_id = 0;
	m_boss_scene_key = 0;
	m_boss_pos = Posi(0, 0);

	m_boss_max_hp = 0;
	m_boss_cur_hp = 0;
	m_open_day = 0;
	m_next_broadcast_time = 0;
}

void ActivityWorldBoss::GameClearBossInfo()
{
	m_boss_npc_id = 0;
	m_boss_npc_seq = 0;

	m_boss_scene_id = 0;
	m_boss_scene_key = 0;
	m_boss_pos = Posi(0, 0);

	m_boss_max_hp = 0;
	m_boss_cur_hp = 0;
	m_open_day = 0;
	m_next_broadcast_time = 0;
}

std::vector<WorldBossRoleInfo> ActivityWorldBoss::GetLucklist()
{
	const WorldBossOtherCfg & other_cfg = LOGIC_CONFIG->GetWorldBossConfig()->GetOtherCfg();	
	std::map<int, WorldBossRoleInfo> temp_role_info = m_role_info;
	for (int rank_index = 0; rank_index < (int)m_hurt_rank.size() && rank_index < other_cfg.world_boss_hurt_rank_num; ++rank_index)
	{
		temp_role_info.erase(m_hurt_rank[rank_index].uid);			//剔除
	}
	
	for (int rank_index = 0; rank_index < (int)m_heal_rank.size() && rank_index < other_cfg.world_boss_heal_rank_num; ++rank_index)
	{
		temp_role_info.erase(m_heal_rank[rank_index].uid);			//剔除
	}
	
	return MapValuesAsVector(temp_role_info);
}

void ActivityWorldBoss::UpdateRank()
{
	m_hurt_rank.clear();
	m_heal_rank.clear();
	
	for (std::map<int, WorldBossRoleInfo>::const_iterator iter = m_role_info.begin(); iter != m_role_info.end(); ++iter)
	{
		UserCacheNode * node = UserCacheManager::Instance().GetUserNode(iter->second.uid);
		if (node == NULL)
		{
			continue;
		}

		if (iter->second.hurt > 0)
		{
			m_hurt_rank.push_back(iter->second);
		}

		if (iter->second.heal > 0)
		{
			m_heal_rank.push_back(iter->second);
		}
	}
	
	std::sort(m_hurt_rank.begin(), m_hurt_rank.end(), WorldBossHurtSort);
	std::sort(m_heal_rank.begin(), m_heal_rank.end(), WorldBossHealSort);

	this->SendRankInfo();
}

static bool  WorldBossHealSort(const WorldBossRoleInfo & a, const WorldBossRoleInfo & b)
{
	if (a.heal < b.heal)
		return false;
	if (a.heal > b.heal)
		return true;

	if (a.hurt < b.hurt)
		return false;
	if (a.hurt > b.hurt)
		return true;

	return a.uid < b.uid;
}

static bool  WorldBossHurtSort(const WorldBossRoleInfo & a, const WorldBossRoleInfo & b)
{
	if (a.hurt < b.hurt)
		return false;
	if (a.hurt > b.hurt)
		return true;

	if (a.heal < b.heal)
		return false;
	if (a.heal > b.heal)
		return true;

	return a.uid < b.uid;
}