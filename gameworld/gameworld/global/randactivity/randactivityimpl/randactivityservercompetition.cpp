#include "randactivityservercompetition.hpp"
#include "gamelog.h"

#include "servercommon/string/gameworldstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityservercompetitionconfig.hpp"

#include "global/rank/rankmanager.hpp"
#include "global/usercache/usercache.hpp"
#include "global/colosseum/colosseummanager.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/couragechallengerank/couragechallengerank.hpp"

#include "protocol/randactivity/msgraservercompetition.hpp"

#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/route/mailroute.hpp"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "global/randactivity/randactivityadapter.hpp"
#include "gameworld/internalcomm.h"

RandActivityServerCompetition::RandActivityServerCompetition(RandActivityManager * activity_manager)
	:RandActivity(activity_manager, RAND_ACTIVITY_TYPE_SERVER_COMPETITION), is_update_flag(false)
{
}

RandActivityServerCompetition::~RandActivityServerCompetition()
{
}

void RandActivityServerCompetition::Init(const ActivityData & arg_data)
{
	RandActivity::Init(arg_data);

	const RandActivityData * ra_data = &arg_data.rand_activity_data;
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(ra_data->data));
	memcpy(&m_param, ra_data->data, sizeof(m_param));
}

void RandActivityServerCompetition::GetInitParam(ActivityData & arg_data)
{
	RandActivity::GetInitParam(arg_data);

	const RandActivityData * ra_data = &arg_data.rand_activity_data;
	UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(ra_data->data));
	memcpy((void *)ra_data->data, &m_param, sizeof(m_param));
}

void RandActivityServerCompetition::Update(unsigned long interval, time_t now_second)
{
	RandActivity::Update(interval, now_second);
	if (m_rand_activity_status != ACTIVITY_STATUS_OPEN && is_update_flag) return;

	//只有活动进行时进行检测
	this->OnCheckTime(now_second, false);
	is_update_flag = true;
}

void RandActivityServerCompetition::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivity::OnDayChange(old_dayid, now_dayid);
}

void RandActivityServerCompetition::OnSpecialActivityStatusChange(int from_status, int to_status)
{
	switch (to_status)
	{
	case ACTIVITY_STATUS_CLOSE:
	{
		this->OnCheckTime((unsigned int)EngineAdapter::Instance().Time(), true);
	}
	break;
	case ACTIVITY_STATUS_OPEN:
	{
		this->OnActivityOpen();
	}
	break;
	default:
		break;
	}
	RandActivity::OnSpecialActivityStatusChange(from_status, to_status);
}

void RandActivityServerCompetition::SyncActivityDataFromCross(CrossRandActivityDataParam & param)
{
	RandActivity::SyncActivityDataFromCross(param);
	if (CrossConfig::Instance().IsHiddenServer())
	{
		UNSTD_STATIC_CHECK(sizeof(m_param) <= sizeof(param.data));
		m_param = *(RAServerCompetitionParam *)param.data;
	}
}

bool RandActivityServerCompetition::IsInTime(int rank_type)
{
	if(rank_type < SEVER_COMPETITION_RANK_TYPE_BEGIN || rank_type >= SEVER_COMPETITION_RANK_TYPE_MAX) return false;

	unsigned int now_second = (unsigned int)EngineAdapter::Instance().Time();
	if (0 >= m_param.rank_list[rank_type].rank_end_timestamp || now_second < m_begin_time || now_second > m_end_time)
	{
		return false;
	}
	if (now_second <= m_param.rank_list[rank_type].rank_end_timestamp)
	{
		return true;
	}

	return false;
}

void RandActivityServerCompetition::OnSendAllInfo(Role * role)
{
	Protocol::SCRAServerCompetitionActivityInfo info;
	info.count = 0;
	for (int i = SEVER_COMPETITION_RANK_TYPE_BEGIN; i < SEVER_COMPETITION_RANK_TYPE_MAX && i < ARRAY_LENGTH(m_param.rank_list) && info.count < ARRAY_LENGTH(info.rank_type_end_timestamp_list); i++)
	{
		info.rank_type_end_timestamp_list[info.count++] = m_param.rank_list[i].rank_end_timestamp;
	}
	
	int length = sizeof(info) - (ARRAY_LENGTH(info.rank_type_end_timestamp_list) - info.count) * sizeof(info.rank_type_end_timestamp_list[0]);
	if (NULL == role)
	{
		World::GetInstWorld()->SendToAllGateway((const char *)&info, length);
	}
	else
	{
		role->GetRoleModuleManager()->NetSend(&info, length);
	}
}

int RandActivityServerCompetition::GetRankTypeParam(int user_uid, int type)
{
	int param = -1;

	UNSTD_STATIC_CHECK(SEVER_COMPETITION_RANK_TYPE_MAX == 7);	// 修改枚举可能需要改这里
	switch (type)
	{
	case SEVER_COMPETITION_RANK_TYPE_ROLE_CAP:
	{
		const PersonRankItem * rank_info = RankManager::Instance().GetPersonRank()->GetPersonRankItem(PERSON_RANK_TYPE_ROLE_CAPABILITY, user_uid);
		if (NULL == rank_info) return -1;

		param = rank_info->rank_value;
	}
	break;
	case SEVER_COMPETITION_RANK_TYPE_PET_CAP:
	{
		const PersonRankItem * rank_info = RankManager::Instance().GetPersonRank()->GetPersonRankItem(PERSON_RANK_TYPE_PET_MAX_CAPABILITY, user_uid);
		if (NULL == rank_info) return -1;

		param = rank_info->rank_value;
	}
	break;
	case SEVER_COMPETITION_RANK_TYPE_PARTNER_CAP:
	{
		const PersonRankItem * rank_info = RankManager::Instance().GetPersonRank()->GetPersonRankItem(PERSON_RANK_TYPE_FOUR_MAX_PARTNER, user_uid);
		if (NULL == rank_info) return -1;

		param = rank_info->rank_value;
	}
	break;
	case SEVER_COMPETITION_RANK_TYPE_EQUIP_CAP:
	{
		const PersonRankItem * rank_info = RankManager::Instance().GetPersonRank()->GetPersonRankItem(PERSON_RANK_TYPE_EQUIPMENT_CAP, user_uid);
		if (NULL == rank_info) return -1;

		param = rank_info->rank_value;
	}
	break;
	case SEVER_COMPETITION_RANK_TYPE_COLOSSEUM:
	{
		param = ColosseumManager::Instance().GetRoleRank(user_uid);
	}
	break;
	case SEVER_COMPETITION_RANK_TYPE_COURAGE_CHALLENGE:
	{
		const CourageChallengeRankItem * courage_challenge_info = CourageChallengeRankManager::Instance().GetRankItem(user_uid);
		if (NULL == courage_challenge_info) return -1;

		param = courage_challenge_info->star;
	}
	break;
	case SEVER_COMPETITION_RANK_TYPE_COMPLEX_CAP:
	{
		const PersonRankItem * rank_info = RankManager::Instance().GetPersonRank()->GetPersonRankItem(PERSON_RANK_TYPE_CAPABILITY, user_uid);
		if (NULL == rank_info) return -1;

		param = rank_info->rank_value;
	}
	break;
	}

	return param;
}

void RandActivityServerCompetition::SendAllRankInfoToCross(int role_id)
{
	if (CrossConfig::Instance().IsHiddenServer()) return;

	Protocol::SCRAServerCompetitionAllRankInfo msg;

	const RandActivityServerCompetitionConfig * config = static_cast<const RandActivityServerCompetitionConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SERVER_COMPETITION));
	if (NULL == config) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL == ramgr) return;

	for (int type = SEVER_COMPETITION_RANK_TYPE_BEGIN; type < SEVER_COMPETITION_RANK_TYPE_MAX; type++)
	{
		std::vector<int> rank_uid_list;

		SCRAServerCompetitionRankType rank_list[MAX_SERVER_COMPETITION_RANK_USER_COUNT];

		std::vector<RASeverCompetitionRewardCfg> competition_reward_vec;
		config->GetRewardMapCfgByRankType(ramgr, type, competition_reward_vec);


		UNSTD_STATIC_CHECK(SEVER_COMPETITION_RANK_TYPE_MAX == 7);	// 修改枚举可能需要改这里
		switch (type)
		{
		case SEVER_COMPETITION_RANK_TYPE_ROLE_CAP:
		{
			RankManager::Instance().OnGetPersonRankUidList(PERSON_RANK_TYPE_ROLE_CAPABILITY, MAX_SERVER_COMPETITION_RANK_USER_COUNT, rank_uid_list);
		}
		break;
		case SEVER_COMPETITION_RANK_TYPE_PET_CAP:
		{
			RankManager::Instance().OnGetPersonRankUidList(PERSON_RANK_TYPE_PET_MAX_CAPABILITY, MAX_SERVER_COMPETITION_RANK_USER_COUNT, rank_uid_list);
		}
		break;
		case SEVER_COMPETITION_RANK_TYPE_PARTNER_CAP:
		{
			RankManager::Instance().OnGetPersonRankUidList(PERSON_RANK_TYPE_FOUR_MAX_PARTNER, MAX_SERVER_COMPETITION_RANK_USER_COUNT, rank_uid_list);
		}
		break;
		case SEVER_COMPETITION_RANK_TYPE_EQUIP_CAP:
		{
			RankManager::Instance().OnGetPersonRankUidList(PERSON_RANK_TYPE_EQUIPMENT_CAP, MAX_SERVER_COMPETITION_RANK_USER_COUNT, rank_uid_list);
		}
		break;
		case SEVER_COMPETITION_RANK_TYPE_COLOSSEUM:
		{
			ColosseumManager::Instance().OnGetRankUidList(MAX_SERVER_COMPETITION_RANK_USER_COUNT, rank_uid_list);
		}
		break;
		case SEVER_COMPETITION_RANK_TYPE_COURAGE_CHALLENGE:
		{
			CourageChallengeRankManager::Instance().OnGetRankUidList(rank_uid_list);
		}
		break;
		case SEVER_COMPETITION_RANK_TYPE_COMPLEX_CAP:
		{
			RankManager::Instance().OnGetPersonRankUidList(PERSON_RANK_TYPE_CAPABILITY, MAX_SERVER_COMPETITION_RANK_USER_COUNT, rank_uid_list);
		}
		break;
		}

		for (size_t i = 0; i < rank_uid_list.size() && i < MAX_SERVER_COMPETITION_RANK_USER_COUNT; i++)
		{
			if (rank_uid_list[i] <= 0) continue;

			SCRAServerCompetitionRankType rank_info;
			{
				UserCacheNode * user_node = UserCacheManager::Instance().GetUserNode(rank_uid_list[i]);
				if (NULL == user_node) continue;

				int temp_param = this->GetRankTypeParam(rank_uid_list[i], type);
				if (temp_param < 0) continue;

				rank_info.uid = rank_uid_list[i];
				rank_info.param = temp_param;

				memcpy(rank_info.appearance_info.name, user_node->role_name, sizeof(GameName));
				rank_info.appearance_info.profession = user_node->profession;
				rank_info.appearance_info.level = user_node->level;
				rank_info.appearance_info.top_level = user_node->top_level;
				rank_info.appearance_info.capability = user_node->capability;
				rank_info.appearance_info.headshot_id = user_node->appearance.headshot_id;
				rank_info.appearance_info.avatar_type = user_node->appearance.avatar_type;
			}

			int rank_start = -1;
			for (size_t j = 0; j < competition_reward_vec.size(); j++)
			{
				if (SERVER_COMPETITION_REWARD_TYPE_RANK != competition_reward_vec[j].reward_type) continue;

				if (competition_reward_vec[j].param3 <= rank_info.param)
				{
					rank_start = competition_reward_vec[j].param1;
					break;
				}
			}
			if (rank_start <= 0 || rank_start >= MAX_SERVER_COMPETITION_RANK_USER_COUNT) break;

			for (int j = rank_start - 1; j < MAX_SERVER_COMPETITION_RANK_USER_COUNT; j++)
			{
				if (rank_list[j].uid != 0) continue;

				rank_list[j] = rank_info;
				break;
			}
		}

		memcpy(msg.rank_info[type], rank_list, sizeof(msg.rank_info[type]));
	}

	crossgameprotocal::GameHiddenServerCompetitionRankResp resp;
	resp.role_uid = role_id;
	resp.msg = msg;

	InternalComm::Instance().SendToHiddenThroughCross((const char *)&resp, sizeof(resp));
}

void RandActivityServerCompetition::GmTestGiveReward(Role* role)
{
	int rank_type = SEVER_COMPETITION_RANK_TYPE_COLOSSEUM;

	const RandActivityServerCompetitionConfig * config = static_cast<const RandActivityServerCompetitionConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SERVER_COMPETITION));
	if (NULL == config) return;

	std::vector<RASeverCompetitionRewardCfg> competition_reward_vec;
	config->GetRewardMapCfgByRankType(m_rand_activity_manager, rank_type, competition_reward_vec);
	if (competition_reward_vec.empty()) return;

	std::vector<int> rank_uid_list;
	rank_uid_list.push_back(role->GetUID());
	for (int i = 0; i < (int)competition_reward_vec.size(); i++)
	{
		if (SERVER_COMPETITION_REWARD_TYPE_RANK != competition_reward_vec[i].reward_type) continue;
		if (competition_reward_vec[i].param1 > (int)rank_uid_list.size()) continue;	//没有该排名范围

		int rank_pos_begin = competition_reward_vec[i].param1 - 1;
		int rank_pos_end = competition_reward_vec[i].param2 - 1;
		for (; rank_pos_begin <= rank_pos_end && rank_pos_begin < (int)rank_uid_list.size(); rank_pos_begin++)
		{
			if (rank_uid_list[rank_pos_begin] <= 0) continue;

			static MailContentParam contentparam; contentparam.Reset();
			FillMailContentParam(contentparam, competition_reward_vec[i].reward_list);
			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_ra_server_competition_end_rank_content, rank_type, rank_pos_begin + 1);
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(rank_uid_list[rank_pos_begin], MAIL_REASON_DEFAULT, contentparam);
				gamelog::g_log_server_competition.printf(LL_INFO, "%s uid[%d] rank[%d, %d] succ", __FUNCTION__, rank_uid_list[rank_pos_begin], rank_type, rank_pos_begin + 1);
			}
			if (competition_reward_vec[i].is_broadcast)
			{
				UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(rank_uid_list[rank_pos_begin]);
				if (NULL != user_cache_node)
				{
					length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_ra_server_competition_end_broadcast,
						user_cache_node->GetNamePtr(), rank_type, rank_pos_begin + 1);
					if (length > 0)
					{
						World::GetInstWorld()->GetSceneManager()->SystemMsgAllGame(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT), 0, 0, 0, 0);
					}
				}
			}
		}
	}
}

void RandActivityServerCompetition::OnActivityOpen()
{
	m_param.Reset();
	
	const RandActivityServerCompetitionConfig * config = static_cast<const RandActivityServerCompetitionConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SERVER_COMPETITION));
	if (NULL == config) return;

	const std::map<int, RASeverCompetitionTimeCfg>* competition_map = config->GetCompetitionMap(m_rand_activity_manager);
	if(NULL == competition_map) return;

	unsigned int rand_activity_open_zero_timestamp = GetZeroTime(m_begin_time);
	for (std::map<int, RASeverCompetitionTimeCfg>::const_iterator it = competition_map->begin(); it != competition_map->end(); ++it)
	{
		if(it->first < SEVER_COMPETITION_RANK_TYPE_BEGIN || it->first >= SEVER_COMPETITION_RANK_TYPE_MAX) continue;

		int last_day = it->second.continuou_times + it->second.start_time - 1;
		if (last_day <= 0)
		{
			last_day = 1;
		}
			
		m_param.rank_list[it->first].rank_end_timestamp = rand_activity_open_zero_timestamp + SECOND_PER_DAY * last_day;
	}
	this->SetDirty();
	this->OnSendAllInfo();
}

void RandActivityServerCompetition::OnCheckTime(unsigned int now_second, bool is_close)
{	
	//跨服不做该处理,防止发放重复奖励
	if(CrossConfig::Instance().IsHiddenServer()) return;

	for (int rank_type = SEVER_COMPETITION_RANK_TYPE_BEGIN; rank_type < SEVER_COMPETITION_RANK_TYPE_MAX && rank_type < ARRAY_LENGTH(m_param.rank_list); rank_type++)
	{
		if (0 != m_param.rank_list[rank_type].is_give_rand_activity_reward_flag || m_param.rank_list[rank_type].rank_end_timestamp <= 0) continue;
		if (!is_close && now_second <= m_param.rank_list[rank_type].rank_end_timestamp) continue;

		this->OnActivityRankEnd(rank_type, is_close);
		this->SetDirty();
	}
}

//rank_type外部判断
void RandActivityServerCompetition::OnActivityRankEnd(int rank_type, bool is_close)
{
	const RandActivityServerCompetitionConfig * config = static_cast<const RandActivityServerCompetitionConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SERVER_COMPETITION));
	if (NULL == config) return;

	std::vector<RASeverCompetitionRewardCfg> competition_reward_vec;
	config->GetRewardMapCfgByRankType(m_rand_activity_manager, rank_type, competition_reward_vec);
	if(competition_reward_vec.empty()) return;

	UNSTD_STATIC_CHECK((unsigned int)MAX_COURAGE_CHALLENGE_RANK_NUM <= PERSON_RANK_MAX_SIZE);
	std::vector<int> rank_uid_list;
	switch (rank_type)
	{
	case SEVER_COMPETITION_RANK_TYPE_ROLE_CAP:
	{
		RankManager::Instance().OnGetPersonRankUidList(PERSON_RANK_TYPE_ROLE_CAPABILITY, MAX_COURAGE_CHALLENGE_RANK_NUM, rank_uid_list);
	}
	break;	
	case SEVER_COMPETITION_RANK_TYPE_PET_CAP:
	{
		RankManager::Instance().OnGetPersonRankUidList(PERSON_RANK_TYPE_PET_MAX_CAPABILITY, MAX_COURAGE_CHALLENGE_RANK_NUM, rank_uid_list);
	}
	break;
	case SEVER_COMPETITION_RANK_TYPE_PARTNER_CAP:
	{
		RankManager::Instance().OnGetPersonRankUidList(PERSON_RANK_TYPE_FOUR_MAX_PARTNER, MAX_COURAGE_CHALLENGE_RANK_NUM, rank_uid_list);
	}
	break;
	case SEVER_COMPETITION_RANK_TYPE_EQUIP_CAP:
	{
		RankManager::Instance().OnGetPersonRankUidList(PERSON_RANK_TYPE_EQUIPMENT_CAP, MAX_COURAGE_CHALLENGE_RANK_NUM, rank_uid_list);
	}
	break;
	case SEVER_COMPETITION_RANK_TYPE_COLOSSEUM:
	{
		ColosseumManager::Instance().OnGetRankUidList(MAX_COURAGE_CHALLENGE_RANK_NUM, rank_uid_list);
	}
	break;
	case SEVER_COMPETITION_RANK_TYPE_COURAGE_CHALLENGE:
	{
		CourageChallengeRankManager::Instance().OnGetRankUidList(rank_uid_list);
	}
	break;
	case SEVER_COMPETITION_RANK_TYPE_COMPLEX_CAP:
	{
		RankManager::Instance().OnGetPersonRankUidList(PERSON_RANK_TYPE_CAPABILITY, MAX_COURAGE_CHALLENGE_RANK_NUM, rank_uid_list);
	}
	break;
	default:
		break;
	}

	for (int i = 0; i < (int)competition_reward_vec.size(); i++)
	{
		if (SERVER_COMPETITION_REWARD_TYPE_RANK != competition_reward_vec[i].reward_type) continue;

		int rank_pos_begin = competition_reward_vec[i].param1 - 1;
		int rank_pos_end = competition_reward_vec[i].param2 - 1;
		for (size_t j = 0; j < rank_uid_list.size() && rank_pos_begin <= rank_pos_end; j++)
		{
			if (rank_uid_list[j] <= 0) continue;

			int param = this->GetRankTypeParam(rank_uid_list[j], rank_type);
			if (param < 0) continue;
			
			if (competition_reward_vec[i].param3 > param) continue;

			MailContentParam contentparam; contentparam.Reset();
			FillMailContentParam(contentparam, competition_reward_vec[i].reward_list);
			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_randactivity_reward_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_ra_server_competition_end_rank_content, rank_type, rank_pos_begin + 1);
			if (lensub > 0 && length > 0)
			{
				MailRoute::MailToUser(rank_uid_list[j], MAIL_REASON_DEFAULT, contentparam);
				gamelog::g_log_server_competition.printf(LL_INFO, "%s uid[%d] rank[%d, %d] succ", __FUNCTION__, rank_uid_list[j], rank_type, rank_pos_begin + 1);
			}
			if (competition_reward_vec[i].is_broadcast)
			{
				UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(rank_uid_list[j]);
				if (NULL != user_cache_node)
				{
					length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_ra_server_competition_end_broadcast,
						user_cache_node->GetNamePtr(), rank_type, rank_pos_begin + 1);
					if (length > 0)
					{
						World::GetInstWorld()->GetSceneManager()->SystemMsgAllGame(gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_SYSTEM_CHAT), 0, 0, 0, 0);
					}
				}
			}

			rank_uid_list[j] = 0;
			rank_pos_begin++;
		}

	}

	m_param.rank_list[rank_type].is_give_rand_activity_reward_flag = 1;
	gamelog::g_log_server_competition.printf(LL_INFO, "%s rank_type[%d] is_close[%d]", __FUNCTION__, rank_type, is_close ? 1 : 0);
}
