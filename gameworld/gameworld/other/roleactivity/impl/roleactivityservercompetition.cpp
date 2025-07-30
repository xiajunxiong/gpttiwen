#include "roleactivityservercompetition.hpp"
#include "gamelog.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityservercompetitionconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityimpl/randactivityservercompetition.hpp"
#include "global/rank/rankmanager.hpp"
#include "global/colosseum/colosseummanager.hpp"
#include "global/couragechallengerank/couragechallengerank.hpp"
#include "global/colosseum/gladiator.hpp"
#include "global/usercache/usercache.hpp"

#include "servercommon/errornum.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "equipment/equipmentmanager.h"

#include "other/pet/pet.hpp"
#include "obj/character/role.h"
#include "other/partner/partner.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/capability/capability.hpp"
#include "other/couragechallenge/couragechallenge.h"
#include "other/roleactivity/roleactivitymanager.hpp"
#include "gameworld/internalcomm.h"

RoleActivityServerCompetition::RoleActivityServerCompetition(int type)
	:RoleActivity(type)
{
}

RoleActivityServerCompetition::~RoleActivityServerCompetition()
{
}

void RoleActivityServerCompetition::Init(RoleModuleManager * mgr, const RoleActivityRoleData & p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RARoleServerCompetitionParam));
}

void RoleActivityServerCompetition::GetInitParam(RoleActivityRoleData * p)
{
	memcpy(p->base_data, &m_param, sizeof(RARoleServerCompetitionParam));
}

void RoleActivityServerCompetition::Update(time_t now_second)
{
	this->OnCheckTime();
}

void RoleActivityServerCompetition::OnLoginSendInfo()
{
	this->SendAllInfo();
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityServerCompetition * activity = (RandActivityServerCompetition *)ramgr->ForceGetRandActivity(m_role_activity_type);
	if (NULL == activity)
	{
		return;
	}
	activity->OnSendAllInfo(m_role_module_mgr->GetRole());
}

void RoleActivityServerCompetition::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;
	//检查刷新
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}
	m_param.is_fetch_day_gift_flag = 0;
	this->SendOtherInfo();
	this->SetRoleDataChange();
}

void RoleActivityServerCompetition::CheckInitRandActivity(int rand_act_type, bool is_force)
{
	if (m_role_activity_type != rand_act_type)
	{
		return;
	}

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity * activity = ramgr->ForceGetRandActivity(rand_act_type);
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_server_competition_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		this->OnCheckTime();	//先检测是否有上次活动漏补发奖励,有则先补发再重置
		m_param.Reset();
		m_param.ra_server_competition_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());
		m_param.ra_activity_begin_time_timestamp = ramgr->GetRandActivityBeginTime(m_role_activity_type);
		for (int i = SEVER_COMPETITION_RANK_TYPE_BEGIN; i < SEVER_COMPETITION_RANK_TYPE_MAX && i < ARRAY_LENGTH(m_param.rank_info_list); i++)
		{
			m_param.rank_info_list[i].max_rank_value = this->GetCurRankValueByType(i);
		}

		this->SetRoleDataChange();
	}
}

void RoleActivityServerCompetition::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq * raor)
{
	if (NULL == raor) return;

	switch (raor->opera_type)
	{
		case Protocol::RA_SERVER_COMPETITION_REQ_TYPE_ROLE_ALL_INFO:
		{
			this->SendAllInfo();
		}
		break;
		case Protocol::RA_SERVER_COMPETITION_REQ_TYPE_FETCH_REWARD:
		{
			this->OnFetchReward(raor->param_1, raor->param_2);
		}
		break;
		case Protocol::RA_SERVER_COMPETITION_REQ_TYPE_ACTIVITY_INFO:
		{
			RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
			if (NULL == ramgr) return;

			RandActivityServerCompetition * activity = (RandActivityServerCompetition *)ramgr->ForceGetRandActivity(m_role_activity_type);
			if (NULL == activity)
			{
				return;
			}
			activity->OnSendAllInfo(m_role_module_mgr->GetRole());
		}
		break;
		case RA_SERVER_COMPETITION_REQ_TYPE_FETCH_DAY_GIFT:
			{
				this->OnFetchDayGift();
			}
			break;
		case RA_SERVER_COMPETITION_REQ_TYPE_ALL_RANK_INFO:
			{
				this->SendAllRankInfo();
			}
			break;
	}
}

void RoleActivityServerCompetition::OnRankValueChange(int rank_type, int rank_value)
{
	if(rank_type < SEVER_COMPETITION_RANK_TYPE_BEGIN || rank_type >= SEVER_COMPETITION_RANK_TYPE_MAX) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(m_role_activity_type))
	{
		return;
	}
	RandActivityServerCompetition * activity = (RandActivityServerCompetition *)ramgr->ForceGetRandActivity(m_role_activity_type);
	if (NULL == activity || !activity->IsInTime(rank_type))		//该榜单如果已经结束,则不再去修改数值
	{
		return;
	}

	int old_max_value = m_param.rank_info_list[rank_type].max_rank_value;
	switch (rank_type)
	{
	case SEVER_COMPETITION_RANK_TYPE_ROLE_CAP:
	case SEVER_COMPETITION_RANK_TYPE_PET_CAP:
	case SEVER_COMPETITION_RANK_TYPE_PARTNER_CAP:
	case SEVER_COMPETITION_RANK_TYPE_EQUIP_CAP:
	case SEVER_COMPETITION_RANK_TYPE_COURAGE_CHALLENGE:
	case SEVER_COMPETITION_RANK_TYPE_COMPLEX_CAP:
	{
		m_param.rank_info_list[rank_type].max_rank_value = GetMax(m_param.rank_info_list[rank_type].max_rank_value, rank_value);
	}
	break;
	case SEVER_COMPETITION_RANK_TYPE_COLOSSEUM:
	{
		m_param.rank_info_list[rank_type].max_rank_value = (0 == m_param.rank_info_list[rank_type].max_rank_value) ? rank_value : GetMin(m_param.rank_info_list[rank_type].max_rank_value, rank_value);
	}
	break;
	default:
		break;
	}
	this->SendSingleInfo(rank_type);	//当前数值发生变化
	if (old_max_value != m_param.rank_info_list[rank_type].max_rank_value)
	{
		this->SetRoleDataChange();
	}
}

void RoleActivityServerCompetition::SendAllRankInfo()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		crossgameprotocal::HiddenGameServerCompetitionRankReq req;
		req.role_uid = m_role_module_mgr->GetUid();

		InternalComm::Instance().SendToGameThroughCross2((const char *)&req, sizeof(req), IntToUid(req.role_uid).db_index);
		return;
	}

	Protocol::SCRAServerCompetitionAllRankInfo msg;

	const RandActivityServerCompetitionConfig * config = static_cast<const RandActivityServerCompetitionConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_SERVER_COMPETITION));
	if (NULL == config) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(LocalConfig::Instance().GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityServerCompetition * activity = (RandActivityServerCompetition *)ramgr->ForceGetRandActivity(RAND_ACTIVITY_TYPE_SERVER_COMPETITION);
	if (NULL == activity) return;

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

				int temp_param = activity->GetRankTypeParam(rank_uid_list[i], type);
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

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}


void RoleActivityServerCompetition::SendAllInfo()
{
	Protocol::SCRAServerCompetitionRoleAllInfo info;
	info.is_fetch_day_gift_flag = m_param.is_fetch_day_gift_flag;
	info.reserve_ch = 0;
	info.reserve_sh = 0;
	info.count = 0;
	for (int i = 0; i < SEVER_COMPETITION_RANK_TYPE_MAX && i < ARRAY_LENGTH(m_param.rank_info_list); i++)
	{
		info.rank_info_list[info.count].reward_fetch_flag = m_param.rank_info_list[i].reward_fetch_flag;
		info.rank_info_list[info.count].cur_rank_type_value = this->GetCurRankValueByType(i);
		info.rank_info_list[info.count].max_rank_type_value = m_param.rank_info_list[i].max_rank_value;
		info.count++;
	}
	
	int length = sizeof(info) - (ARRAY_LENGTH(info.rank_info_list) - info.count) * sizeof(info.rank_info_list[0]);
	m_role_module_mgr->NetSend(&info, length);
}

void RoleActivityServerCompetition::SendSingleInfo(int rank_type)
{
	if(rank_type < SEVER_COMPETITION_RANK_TYPE_BEGIN || rank_type >= SEVER_COMPETITION_RANK_TYPE_MAX) return;

	Protocol::SCRAServerCompetitionSingleRoleInfo info;
	info.rank_type = rank_type;
	info.rank_info.reward_fetch_flag = m_param.rank_info_list[rank_type].reward_fetch_flag;
	info.rank_info.cur_rank_type_value = this->GetCurRankValueByType(rank_type);
	info.rank_info.max_rank_type_value = m_param.rank_info_list[rank_type].max_rank_value;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityServerCompetition::SendOtherInfo()
{
	Protocol::SCRAServerCompetitionOtherInfo info;
	info.is_fetch_day_gift_flag = m_param.is_fetch_day_gift_flag;
	info.reserve_ch = 0;
	info.reserve_sh = 0;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityServerCompetition::OnFetchReward(int rank_type, int seq)
{
	if(rank_type < SEVER_COMPETITION_RANK_TYPE_BEGIN || rank_type >= SEVER_COMPETITION_RANK_TYPE_MAX || seq < 0 || seq >= MAX_SERVER_COMPETITION_RANK_TYPE_SEQ_NUM) return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityServerCompetitionConfig * config = static_cast<const RandActivityServerCompetitionConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	RandActivityServerCompetition * activity = (RandActivityServerCompetition *)ramgr->ForceGetRandActivity(m_role_activity_type);
	if (NULL == activity)
	{
		return;
	}
	const RASeverCompetitionRewardCfg * reward_cfg = config->GetRewardCfg(ramgr, rank_type, seq);
	if (NULL == reward_cfg)
	{
		return;
	}	

	if (m_param.rank_info_list[rank_type].reward_fetch_flag.IsBitSet(seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}
	int activity_open_day = config->GetActOpenDay(ramgr, m_role_activity_type);
	if (activity_open_day < reward_cfg->start_time)
	{
		return;
	}

	if (!activity->IsInTime(rank_type))
	{
		return;
	}

	switch (reward_cfg->reward_type)
	{
		case SERVER_COMPETITION_REWARD_TYPE_SCORE:
		case SERVER_COMPETITION_REWARD_TYPE_COURAGE_CHALLENGE:
		{
			if(m_param.rank_info_list[rank_type].max_rank_value < reward_cfg->param1) return;
		}
		break;
		case SERVER_COMPETITION_REWARD_TYPE_COLOSSEUM_RANK:
		{
			if (m_param.rank_info_list[rank_type].max_rank_value > reward_cfg->param1) return;
		}
		break;
	default:
		return;		//其它类型不可手动领取
	}

	if (!reward_cfg->reward_list.empty())
	{
		bool is_put_succ = false;
		if (m_role_module_mgr->GetKnapsack()->CheckForPutList2(reward_cfg->reward_list, false))
		{
			if (m_role_module_mgr->GetKnapsack()->PutVec(reward_cfg->reward_list, PUT_REASON_SERVER_COMPETITION))
			{
				is_put_succ = true;
			}
		}
		if (!is_put_succ)
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&reward_cfg->reward_list[0], reward_cfg->reward_list.size(), SEND_MAIL_TYPE_ACTIVITY_REWARD, true, m_role_activity_type);
		}
	}

	m_param.rank_info_list[rank_type].reward_fetch_flag.SetBit(seq);
	this->SetRoleDataChange();
	this->SendSingleInfo(rank_type);
	this->OnParticipateRA(__FUNCTION__);

	gamelog::g_log_server_competition.printf(LL_INFO, "%s user[%d, %s] rank_type[%d] seq[%d]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), rank_type, seq);
}

void RoleActivityServerCompetition::OnFetchDayGift()
{
	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityServerCompetitionConfig * config = static_cast<const RandActivityServerCompetitionConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}
	if (0 != m_param.is_fetch_day_gift_flag)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_COLLECT_WORD_NOT_FETCH);
		return;
	}

	const RAServerCompetitionOtherCfg & other_cfg = config->GetOtherCfg();
	if (!other_cfg.day_reward_list.empty())
	{
		if (!m_role_module_mgr->GetKnapsack()->CheckForPutList2(other_cfg.day_reward_list, false))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
		if (!m_role_module_mgr->GetKnapsack()->PutVec(other_cfg.day_reward_list, PUT_REASON_SERVER_COMPETITION))
		{
			m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
	}

	m_param.is_fetch_day_gift_flag = 1;
	this->SendOtherInfo();
	this->SetRoleDataChange();

	gamelog::g_log_server_competition.printf(LL_INFO, "%s user[%d, %s]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName());
}

void RoleActivityServerCompetition::OnCheckTime()
{
	if(m_param.ra_server_competition_begin_timestamp <= 0) return;

	RandActivityManager * ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	const RandActivityServerCompetitionConfig * config = static_cast<const RandActivityServerCompetitionConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(m_role_activity_type));
	if (NULL == config) return;

	RandActivityServerCompetition * activity = (RandActivityServerCompetition *)ramgr->ForceGetRandActivity(m_role_activity_type);
	if (NULL == activity) return;

	bool is_send = false;
	bool is_jump_time_jud = (m_param.ra_activity_begin_time_timestamp == activity->GetBeginTime()) ? false : true;
	for (int rank_type = SEVER_COMPETITION_RANK_TYPE_BEGIN; rank_type < SEVER_COMPETITION_RANK_TYPE_MAX && rank_type < ARRAY_LENGTH(m_param.rank_info_list); rank_type++)
	{
		if(0 != m_param.rank_info_list[rank_type].is_fetch_rank_end_reward_flag) continue;		//已经经过检测查询补发
		if(!is_jump_time_jud && activity->IsInTime(rank_type)) continue;

		std::vector<RASeverCompetitionRewardCfg> competition_reward_vec;
		config->GetRewardMapCfgByRankType(ramgr, rank_type, competition_reward_vec);
		if (competition_reward_vec.empty()) continue;

		for (int i = 0; i < (int)competition_reward_vec.size(); i++)
		{
			if (SERVER_COMPETITION_REWARD_TYPE_RANK == competition_reward_vec[i].reward_type) continue;
			if (m_param.rank_info_list[rank_type].reward_fetch_flag.IsBitSet(competition_reward_vec[i].seq)) continue;

			bool is_conform = false;
			switch (competition_reward_vec[i].reward_type)
			{
			case SERVER_COMPETITION_REWARD_TYPE_SCORE:
			case SERVER_COMPETITION_REWARD_TYPE_COURAGE_CHALLENGE:
			{
				if (m_param.rank_info_list[rank_type].max_rank_value >= competition_reward_vec[i].param1)
				{
					is_conform = true;
				}
			}
			break;
			case SERVER_COMPETITION_REWARD_TYPE_COLOSSEUM_RANK:
			{
				if(0 != m_param.rank_info_list[rank_type].max_rank_value && m_param.rank_info_list[rank_type].max_rank_value <= competition_reward_vec[i].param1)
				{
					is_conform = true;
				}
			}
			break;
			default:
				break;
			}
			if(!is_conform) continue;

			if (!competition_reward_vec[i].reward_list.empty())
			{
				m_role_module_mgr->GetKnapsack()->SendMail(&competition_reward_vec[i].reward_list[0], competition_reward_vec[i].reward_list.size(), SEND_MAIL_TYPE_ACTIVITY_END, false, m_role_activity_type);
			}
			m_param.rank_info_list[rank_type].reward_fetch_flag.SetBit(competition_reward_vec[i].seq);
			gamelog::g_log_server_competition.printf(LL_INFO, "%s user[%d, %s] rank_type[%d] seq[%d]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), rank_type, competition_reward_vec[i].seq);
		}

		is_send = true;
		m_param.rank_info_list[rank_type].is_fetch_rank_end_reward_flag = 1;
		this->SetRoleDataChange();
	}
	if (is_send && ACTIVITY_STATUS_OPEN == activity->GetStatus())
	{
		this->SendAllInfo();
	}
}

int RoleActivityServerCompetition::GetCurRankValueByType(int rank_type)
{
	int cur_rank_value = 0;
	switch (rank_type)
	{
		case SEVER_COMPETITION_RANK_TYPE_ROLE_CAP:
		{
			cur_rank_value = m_role_module_mgr->GetCapability()->GetRoleCapability();
		}
		break;
		case SEVER_COMPETITION_RANK_TYPE_PET_CAP:
		{
			cur_rank_value = m_role_module_mgr->GetPet()->GetPetCapabilityMax();
		}
		break;
		case SEVER_COMPETITION_RANK_TYPE_PARTNER_CAP:
		{
			cur_rank_value = m_role_module_mgr->GetPartner()->GetMaxFourPartnerCap();
		}
		break;
		case SEVER_COMPETITION_RANK_TYPE_EQUIP_CAP:
		{
			cur_rank_value = m_role_module_mgr->GetEquipmentManager()->GetEquipJewelryCap();
		}
		break;
		case SEVER_COMPETITION_RANK_TYPE_COLOSSEUM:
		{
			cur_rank_value = m_role_module_mgr->GetCommonData().colosseum_rank;
		}
		break;
		case SEVER_COMPETITION_RANK_TYPE_COURAGE_CHALLENGE:
		{
			cur_rank_value = m_role_module_mgr->GetCourageChallenge()->GetReachStarScoreCount();
		}
		break;
		case SEVER_COMPETITION_RANK_TYPE_COMPLEX_CAP:
		{
			cur_rank_value = m_role_module_mgr->GetRole()->GetCapabilityWithFight();
		}
		break;
	default:
		break;
	}
	return cur_rank_value;
}
