#include "global/usercache/usercache.hpp"
#include "obj/character/role.h"
#include "engineadapter.h"
#include "servercommon/string/gameworldstr.h"
#include "other/route/mailroute.hpp"
#include "item/itempool.h"
#include "item/itembase.h"
#include "global/usercache/usercache.hpp"
#include "world.h"
#include "other/rolemodulemanager.hpp"
#include "internalcomm.h"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "item/knapsack.h"
#include "gamelog.h"
#include "other/rolemodulemanager.hpp"
#include "other/title/title.hpp"
#include "other/event/eventhandler.hpp"
#include "global/worldstatus/worldstatus.hpp"

#include "config/logicconfigmanager.hpp"
#include "global/commonsave/commonsavemanager.hpp"
#include "config/activityconfig/braveground/activitybravegroundconfig.hpp"
#include "protocol/msgbraveground.h"
#include "activitybraveground.hpp"

void FillHiddenGameSyncBraveGroundInfo(crossgameprotocal::HiddenGameSyncBraveGroundInfo & protocol, int(&top_role_id)[BRAVE_GROUND_TOP_ITEM_NUM][BRAVE_GROUND_TOP_ROLE_NUM], bool is_notice);

int ActivityBraveGround::GetRecordKey()
{
	if (m_record_key == 0)
	{
		RetCommonSaveData data;
		if (m_team_record_save_data->GetLastData(data))
		{
			m_record_key = data.key.param1 + 1;
		}
		else
		{
			m_record_key = 1;
		}
	}

	return m_record_key++;
}

void ActivityBraveGround::UpdateTeamRecord(int record_key, const CommonSaveBraveGroundTeamReceordData & team_record_data)
{
	if (team_record_data.reached_level <= 0)
	{
		return;
	}

	CommonSaveBraveGroundTeamReceordData * team_data = m_team_record_save_data->GetTypeData<CommonSaveBraveGroundTeamReceordData>(record_key);
	if (team_data != NULL && team_data->reached_level < team_record_data.reached_level)
	{
		m_is_need_update_rank = true;
		m_team_record_save_data->SetDataChange(record_key);
		*team_data = team_record_data;
	}

	for (int i = 0; i < team_record_data.role_info_count && i < ARRAY_LENGTH(team_record_data.role_info); ++i)
	{
		this->SetRoleReachedLevel(team_record_data.role_info[i].role_id, team_record_data.reached_level);
	}
}

void ActivityBraveGround::UpdateTopRole(int(&top_role_id)[BRAVE_GROUND_TOP_ITEM_NUM][BRAVE_GROUND_TOP_ROLE_NUM], bool is_notice)
{
	int old_top_role_id[BRAVE_GROUND_TOP_ITEM_NUM][BRAVE_GROUND_TOP_ROLE_NUM] = { {0} };
	for (int i = 0; i < BRAVE_GROUND_TOP_ITEM_NUM; ++i)
		for (int z = 0; z < BRAVE_GROUND_TOP_ROLE_NUM; ++z)
		{
			old_top_role_id[i][z] = m_top_role_id[i][z];
			m_top_role_id[i][z] = top_role_id[i][z];
		}

	for (int i = 0; i < BRAVE_GROUND_TOP_ITEM_NUM; ++i)
	{
		std::vector<int> diff;
		std::sort(m_top_role_id[i], m_top_role_id[i] + BRAVE_GROUND_TOP_ROLE_NUM);
		std::sort(old_top_role_id[i], old_top_role_id[i] + BRAVE_GROUND_TOP_ROLE_NUM);
		int * role_end = std::unique(m_top_role_id[i], m_top_role_id[i] + BRAVE_GROUND_TOP_ROLE_NUM);
		int * old_role_end = std::unique(old_top_role_id[i], old_top_role_id[i] + BRAVE_GROUND_TOP_ROLE_NUM);

		std::set_symmetric_difference(m_top_role_id[i], role_end, old_top_role_id[i], old_role_end, std::inserter(diff, diff.begin()));
		for (std::vector<int>::iterator iter = diff.begin(); iter != diff.end(); ++iter)
		{
			Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(*iter);
			if (role != NULL)
			{
				role->GetRoleModuleManager()->GetTitle()->CalBraveGroundTitle(is_notice ? TITLE_NOTICE_TYPE_DEFAULT : TITLE_NOTICE_TYPE_NO_NOTICE);
			}
		}
	}


	if (CrossConfig::Instance().IsHiddenServer())
	{
		CommonSave * activity_save_data = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_BRAVE_GROUND_SAVE_DATA);
		if (activity_save_data != NULL)
		{
			CommonSaveBraveGroundData * data_ptr = activity_save_data->GetTypeData<CommonSaveBraveGroundData>(0);
			if (data_ptr)
			{
				CommonSaveBraveGroundData & data = *data_ptr;
				memset(data.top_role_id, 0, sizeof(data.top_role_id));

				for (int i = 0; i < BRAVE_GROUND_TOP_ITEM_NUM; ++i)
					for (int z = 0; z < BRAVE_GROUND_TOP_ROLE_NUM; ++z)
					{
						data.top_role_id[i][z] = m_top_role_id[i][z];
					}

				activity_save_data->Save();
			}
		}

		crossgameprotocal::HiddenGameSyncBraveGroundInfo protocol;
		FillHiddenGameSyncBraveGroundInfo(protocol, m_top_role_id, is_notice);

		InternalComm::Instance().SendToAllGameThroughCross((const char *)&protocol, sizeof(protocol));
	}
}

int ActivityBraveGround::GetTopRoleTitle(int role_id)
{
	if (!m_is_init && CrossConfig::Instance().IsHiddenServer())
	{
		m_is_init = true;
		CommonSave * activity_save_data = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_BRAVE_GROUND_SAVE_DATA);
		if (activity_save_data != NULL)
		{
			CommonSaveBraveGroundData * data_ptr = activity_save_data->GetTypeData<CommonSaveBraveGroundData>(0);
			if (data_ptr)
			{
				CommonSaveBraveGroundData & data = *data_ptr;
				for (int i = 0; i < BRAVE_GROUND_TOP_ITEM_NUM; ++i)
					for (int z = 0; z < BRAVE_GROUND_TOP_ROLE_NUM; ++z)
					{
						m_top_role_id[i][z] = data.top_role_id[i][z];
					}
			}
		}
	}

	for (int i = 0; i < BRAVE_GROUND_TOP_ITEM_NUM; ++i)
		for (int z = 0; z < BRAVE_GROUND_TOP_ROLE_NUM; ++z)
		{
			if (m_top_role_id[i][z] == role_id)
			{
				const BraveGroundOtherCfg & other_cfg = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetOtherCfg();
				return other_cfg.top_role_title[i];
			}
		}

	return 0;
}


void ActivityBraveGround::GameHiddenSetRoleInfoReq(int role_id, int reached_level)
{

}

int ActivityBraveGround::SaoDang(Role * role)
{
	//A3 需求54971  去掉了扫荡
	return 0;
}

bool ActivityBraveGround::SortRankFunc(const CommonSaveBraveGroundTeamReceordData & rank_item_1, const CommonSaveBraveGroundTeamReceordData & rank_item_2)
{
	if (rank_item_2.reached_level < rank_item_1.reached_level)
		return true;
	if (rank_item_1.reached_level < rank_item_2.reached_level)
		return false;

	if (rank_item_1.time < rank_item_2.time)
		return true;
	if (rank_item_2.time < rank_item_1.time)
		return false;

	if (rank_item_1.record_key < rank_item_2.record_key)
		return true;
	if (rank_item_2.record_key < rank_item_1.record_key)
		return false;

	return false;
}

void ActivityBraveGround::SetRoleReachedLevel(int role_id, int reached_level)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	if (reached_level <= 0 || role_id == 0)
	{
		return;
	}

	CommonSaveBraveGroundRoleData * data_node = m_role_save_data->GetTypeData<CommonSaveBraveGroundRoleData>(role_id);
	if (!data_node)
	{
		return;
	}

	int old_reached_level = data_node->reached_level;
	if (reached_level <= old_reached_level)
	{
		return;
	}

	data_node->role_id = role_id;
	data_node->reached_level = reached_level;
	m_role_save_data->SetDataChange(role_id);

	//层数奖励
	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
	if (role != NULL)
	{
		int role_level = role->GetLevel();
		for (int i = old_reached_level + 1; i <= reached_level; ++i)
		{
			const BraveGroundLevelRewardCfg * level_cfg = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetLevelRewardCfg(i, role_level);
			if (level_cfg != NULL)
			{
				role->AddExp(level_cfg->exp_reward);
				role->GetRoleModuleManager()->GetMoney()->AddCoinBind(level_cfg->coin, __FUNCTION__);
				if (!level_cfg->rewards.empty())
				{
					role->GetRoleModuleManager()->GetKnapsack()->PutList((short)level_cfg->rewards.size(), &level_cfg->rewards[0], PUT_REASON_BRAVE_GROUND);
				}
			}
		}
		this->SendLevelRewardInfo(old_reached_level, reached_level, role);
	}
}

ActivityBraveGround::ActivityBraveGround(ActivityManager * activity_manager) : Activity(activity_manager, ACTIVITY_TYPE_BRAVE_GROUND)
, m_is_need_update_rank(false), m_next_updata_rank_time(0)
{
	m_record_key = 0;
	m_is_need_update_rank = true;
	m_next_updata_rank_time = 0;
	m_is_init = false;

	for (int i = 0; i < BRAVE_GROUND_TOP_ITEM_NUM; ++i)
		for (int z = 0; z < BRAVE_GROUND_TOP_ROLE_NUM; ++z)
	{
			m_top_role_id[i][z] = 0;
	}

	m_is_sync_reached_level_flag = true;



	m_team_record_save_data = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_BRAVE_GROUND_TEAM_RECORD);
	m_role_save_data = CommonSaveManager::Instance().GetCommonSave(COMMON_SAVE_DATA_TYPE_BRAVE_GROUND_ROLE);
	assert(m_team_record_save_data != NULL);
	assert(m_role_save_data != NULL);

}

ActivityBraveGround::~ActivityBraveGround()
{
}

void ActivityBraveGround::OnActivityStatusChange(int from_status, int to_status)
{
	Activity::OnActivityStatusChange(from_status, to_status);

	switch (to_status)
	{
		case ACTIVITY_STATUS_CLOSE:
			{
				World::GetInstWorld()->GetSceneManager()->KickOutAllBraveGround();
				this->SendReward();
			}
			break;
		case ACTIVITY_STATUS_STANDY:
			{
				this->ClearData();
			}
			break;

		case ACTIVITY_STATUS_OPEN:
			{
				this->ClearData();
			}
			break;
	}

	this->SetDirty();
}

void ActivityBraveGround::Update(unsigned long interval, time_t now_second)
{
	Activity::Update(interval, now_second);

	if (m_is_need_update_rank && now_second >= m_next_updata_rank_time)
	{
		m_is_need_update_rank = false;
		m_next_updata_rank_time = now_second + 60;
		
		this->UpdateRank();
	}

	if (m_is_sync_reached_level_flag)   //开服第一次update 同步一次团队通关最高层数到个人数据上
	{
		m_is_sync_reached_level_flag = false;
		std::map<int, int> role_max_reached_level;

		RetCommonSaveData get_data;
		m_team_record_save_data->BeginDataForeach();
		while (m_team_record_save_data->GetNextData(get_data))
		{
			const CommonSaveBraveGroundTeamReceordData & team_data = get_data.data.brave_ground_team_record_data;
			for (int i = 0; i < team_data.role_info_count && i < ARRAY_LENGTH(team_data.role_info); ++i)
			{
				int level = std::max(role_max_reached_level[team_data.role_info[i].role_id], team_data.reached_level);
				role_max_reached_level[team_data.role_info[i].role_id] = level;
			}
		}

		for (std::map<int, int>::iterator iter = role_max_reached_level.begin(); iter != role_max_reached_level.end(); ++iter)
		{
			int role_id = iter->first;
			int reached_level = iter->second;
			this->SetRoleReachedLevel(role_id, reached_level);
		}
	}
}

void ActivityBraveGround::OnUserLogin(Role * role)
{
	if (NULL == role) return;

	Activity::OnUserLogin(role);

	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}	

	this->RewardReissue(role);
	this->SendInfo(role);
	int role_id = role->GetUID();
	int reached_level = this->GetRoleReachedLevel(role_id);
	role->GetRoleModuleManager()->SetBraveGroundMaxReachedLevel(reached_level);
	this->SendActivityBraveGroundCommonInfo(role);
}

void ActivityBraveGround::OnWeekChange()
{

}

void ActivityBraveGround::OnConnectCrossServerSucc()
{
}

void ActivityBraveGround::OnUserLoginOriginGame(int role_id)
{
	this->HiddenToGameSendInfo(role_id);
	this->SendActivityBraveGroundCommonInfo(NULL, role_id);
}

void ActivityBraveGround::OnGameServerConnect(std::set<int> server_id)
{
	crossgameprotocal::HiddenGameSyncBraveGroundInfo protocol;
	FillHiddenGameSyncBraveGroundInfo(protocol, m_top_role_id, false);
	InternalComm::Instance().SendToAllGameThroughCross((const char *)&protocol, sizeof(protocol));
}


void ActivityBraveGround::SendRankInfo(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	Protocol::SCBraveGroundRankInfo protocol;

	protocol.count = 0;
	for (int i = 0; i < (int)m_rank_info.size() && protocol.count < ARRAY_LENGTH(protocol.rank_list); ++i)
	{
		const CommonSaveBraveGroundTeamReceordData & rank_item_data = m_rank_info[i];		
		protocol.rank_list[protocol.count] = rank_item_data;
		protocol.count += 1;		
	}

	unsigned int length = sizeof(protocol) - (ARRAY_LENGTH(protocol.rank_list) - protocol.count) * sizeof(protocol.rank_list[0]);

	EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&protocol, length);
}

void ActivityBraveGround::SendInfo(Role * role)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	Protocol::SCBraveGroundInfo protocol;
	protocol.reached_level = 0;

	if (role != NULL)
	{
		CommonSaveBraveGroundRoleData * role_data = m_role_save_data->GetTypeData<CommonSaveBraveGroundRoleData>(CommonSaveKey(role->GetUID()), false);
		if (role_data != NULL)
		{
			protocol.reached_level = role_data->reached_level;
		}

		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&protocol, sizeof(protocol));
	}
	else
	{
		World::GetInstWorld()->SendToALLGameAllGateway((const char *)&protocol, sizeof(protocol));
	}
}

void ActivityBraveGround::HiddenToGameSendInfo(int role_id)
{
	Protocol::SCBraveGroundInfo protocol;
	protocol.reached_level = 0;

	CommonSaveBraveGroundRoleData * role_data = m_role_save_data->GetTypeData<CommonSaveBraveGroundRoleData>(role_id, false);
	if (role_data != NULL)
	{
		protocol.reached_level = role_data->reached_level;
	}

	InternalComm::Instance().SendToGameUserThroughCross(role_id, (const char *)&protocol, sizeof(protocol));
}

void ActivityBraveGround::HiddenToGameSendRankInfo(int role_id)
{
	Protocol::SCBraveGroundRankInfo protocol;

	protocol.count = 0;
	for (int i = 0; i < (int)m_rank_info.size() && protocol.count < ARRAY_LENGTH(protocol.rank_list); ++i)
	{
		const CommonSaveBraveGroundTeamReceordData & rank_item_data = m_rank_info[i];
		protocol.rank_list[protocol.count] = rank_item_data;
		protocol.count += 1;
	}

	unsigned int length = sizeof(protocol) - (ARRAY_LENGTH(protocol.rank_list) - protocol.count) * sizeof(protocol.rank_list[0]);
	InternalComm::Instance().SendToGameUserThroughCross(role_id, (const char *)&protocol, length);
}

void ActivityBraveGround::SendActivityBraveGroundCommonInfo(Role * role, int role_id)
{
	if (!CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	Protocol::SCBraveGroundCommonInfo protocol;
	protocol.brave_ground_world_level = WorldStatus::Instance().GetBraveGroundWorldLevel();

	if (role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
	}

	if (role_id)
	{
		InternalComm::Instance().SendToGameUserThroughCross(role_id, &protocol, sizeof(protocol));
	}

	if (role == NULL && role_id == 0)
	{
		World::GetInstWorld()->SendToALLGameAllGateway(&protocol, sizeof(protocol));
	}
}

int ActivityBraveGround::GetRoleReachedLevel(int role_id)
{
	CommonSaveBraveGroundRoleData * data_node = m_role_save_data->GetTypeData<CommonSaveBraveGroundRoleData>(role_id);
	if (data_node == NULL)
	{
		return 0;
	}

	return data_node->reached_level;
}

void ActivityBraveGround::SendLevelRewardInfo(int start_level, int end_level, Role * role)
{
	Protocol::SCBraveGroundLevelRewardInfo info;
	info.start_level = start_level;
	info.end_level = end_level;

	if (NULL != role)
	{
		EngineAdapter::Instance().NetSend(role->GetNetId(), (const char *)&info, sizeof(info));
	}
}


void ActivityBraveGround::UpdateRank()
{
	m_rank_info.clear();

	std::vector<CommonSaveBraveGroundTeamReceordData> tmp_rank;
	RetCommonSaveData get_data;
	m_team_record_save_data->BeginDataForeach();
	while (m_team_record_save_data->GetNextData(get_data))
	{
		const CommonSaveBraveGroundTeamReceordData & data = get_data.data.brave_ground_team_record_data;
		if (data.reached_level <= 0 || data.role_info_count <= 0)
		{
			continue;
		}

		tmp_rank.push_back(data);
	}

	std::sort(tmp_rank.begin(), tmp_rank.end(), &ActivityBraveGround::SortRankFunc);

	std::set<int> role_id_list;
	for (int i = 0; i < (int)tmp_rank.size(); ++i)
	{
		const CommonSaveBraveGroundTeamReceordData & curr = tmp_rank[i];
		bool is_all_role_has_rank = true;
		for (int role_index = 0; role_index < ARRAY_LENGTH(curr.role_info); ++role_index)
		{
			const CommonSaveBraveGroundRankRoleInfoItem & role_info = curr.role_info[role_index];
			if (role_info.role_id == 0)
			{
				continue;
			}

			if (role_id_list.find(role_info.role_id) == role_id_list.end())
			{
				is_all_role_has_rank = false;
				role_id_list.insert(role_info.role_id);
			}
		}

		if (is_all_role_has_rank)
		{
			continue;
		}

		m_rank_info.push_back(curr);
	}

	if ((int)m_rank_info.size() >= ACTIVITY_BRAVE_GROUND_MAX_RANK)
	{
		m_rank_info.erase(m_rank_info.begin() + ACTIVITY_BRAVE_GROUND_MAX_RANK, m_rank_info.end());
	}
}

void ActivityBraveGround::SendReward()
{
	this->UpdateRank();

	std::map<int, int> uid_to_rank;

	int top_role_id[BRAVE_GROUND_TOP_ITEM_NUM][BRAVE_GROUND_TOP_ROLE_NUM];
	memset(top_role_id, 0, sizeof(top_role_id));

	//发奖励
	for (int rank = 0; rank < (int)m_rank_info.size(); ++rank)
	{
		const CommonSaveBraveGroundTeamReceordData & curr = m_rank_info[rank];
		for (int i = 0; i < curr.role_info_count && i < ARRAY_LENGTH(curr.role_info); ++i)
		{
			gamelog::g_log_brave_ground.printf(LL_INFO, " ActivityBraveGround::SendReward role_id:%d,role_name:%s rank:%d", curr.role_info[i].role_id, curr.role_info[i].role_name, rank + 1);

			if (uid_to_rank.find(curr.role_info[i].role_id) == uid_to_rank.end())
			{
				uid_to_rank[curr.role_info[i].role_id] = rank + 1;

				if (0 <= rank && rank < BRAVE_GROUND_TOP_ITEM_NUM && 0 <= i && i < BRAVE_GROUND_TOP_ROLE_NUM) //这个要里面，前面排名可能已经有他了
				{
					top_role_id[rank][i] = curr.role_info[i].role_id;
				}
			}
		}			
	}

	this->UpdateTopRole(top_role_id, true);

	for (std::map<int, int>::iterator iter = uid_to_rank.begin(); iter != uid_to_rank.end(); ++iter)
	{
		int role_id = iter->first;
		int rank = iter->second;

		UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(role_id);
		if (NULL == user_cache_node)
		{
			continue;
		}

		const BraveGroundRankRewardCfg::ItemCfg * rank_reward_cfg = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetRankRewardCfg(rank, user_cache_node->level);
		if (rank_reward_cfg == NULL)
		{
			continue;
		}

		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		for (int k = 0; k < (int)rank_reward_cfg->rewards.size() && k < MAX_ATTACHMENT_ITEM_NUM; ++k)
		{
			const ItemConfigData & reward = rank_reward_cfg->rewards[k];

			const ItemBase * item_base = ITEMPOOL->GetItem(reward.item_id);
			if (item_base == NULL)
			{
				continue;
			}

			contentparam.item_list[k].item_id = reward.item_id;
			contentparam.item_list[k].num = reward.num;
			contentparam.item_list[k].is_bind = (reward.is_bind) ? 1 : 0;
			contentparam.item_list[k].invalid_time = item_base->CalInvalidTime();
		}

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_brave_ground_reward_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_brave_ground_reward, rank);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(IntToUid(role_id), MAIL_REASON_DEFAULT, contentparam);
		}
	}
}

void ActivityBraveGround::ClearData()
{
	m_is_need_update_rank = false;
	m_next_updata_rank_time = 0;
	m_rank_info.clear();

	m_role_save_data->ClearAllData();
	m_team_record_save_data->ClearAllData();

	this->SendInfo();
}

void ActivityBraveGround::RewardReissue(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	CommonDataParam & common = role->GetRoleModuleManager()->GetCommonData();
	if (common.brave_ground_reward_reissue_ver >= BRAVE_GROUND_ROLE_REWARD_REISSUE_VER)
	{
		return;
	}
	common.brave_ground_reward_reissue_ver = BRAVE_GROUND_ROLE_REWARD_REISSUE_VER;

	int role_level = role->GetLevel();
	int role_id = role->GetUID();

	const CommonSaveBraveGroundRoleData * data_node = m_role_save_data->GetTypeData<CommonSaveBraveGroundRoleData>(role_id);
	if (!data_node)
	{
		gamelog::g_log_brave_ground.printf(LL_INFO, " ActivityBraveGround::RewardReissue is null role_id:%d", role_id);
		return;
	}

	int reached_level = data_node->reached_level;

	gamelog::g_log_brave_ground.printf(LL_INFO, " ActivityBraveGround::RewardReissue role_id:%d,role_level:%d,reached_level:%d", role_id, role_level, reached_level);

	for (int i = 1; i <= reached_level; ++i)
	{
		const BraveGroundLevelRewardCfg * level_cfg = LOGIC_CONFIG->GetActivityBraveGroundConfig()->GetLevelRewardCfg(i, role_level);
		if (level_cfg != NULL)
		{	
			// 发邮件
			static MailContentParam contentparam; contentparam.Reset();
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] = level_cfg->exp_reward;
			contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = level_cfg->coin;
			FillMailContentParam(contentparam, level_cfg->rewards);

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_brave_ground_reward_reissue_ver1_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_brave_ground_reward_reissue_ver1_content, i);
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(role_id, MAIL_REASON_DEFAULT, contentparam);
			}
		}
	}	
}

void FillHiddenGameSyncBraveGroundInfo(crossgameprotocal::HiddenGameSyncBraveGroundInfo & protocol , int(&top_role_id)[BRAVE_GROUND_TOP_ITEM_NUM][BRAVE_GROUND_TOP_ROLE_NUM], bool is_notice)
{
	memset(protocol.top_role_id, 0, sizeof(protocol.top_role_id));
	protocol.is_notice = is_notice;

	for (int i = 0; i < BRAVE_GROUND_TOP_ITEM_NUM; ++i)
		for (int z = 0; z < BRAVE_GROUND_TOP_ROLE_NUM; ++z)
		{
			protocol.top_role_id[i][z] = top_role_id[i][z];
		}
}