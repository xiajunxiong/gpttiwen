#include "roleactivityanimalrace.hpp"

#include "engineadapter.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/randactivityconfig/impl/randactivityanimalraceconfig.hpp"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/randactivity/randactivityimpl/randactivityanimalrace.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "other/animalrace/animalracemanager.hpp"
#include "other/route/mailroute.hpp"

#include "protocol/randactivity/msgraanimalrace.hpp"
#include "servercommon/userprotocal/msgchatmsg.h"

#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "servercommon/string/gameworldstr.h"
#include "servercommon/serverconfig/crossconfig.hpp"

RoleActivityAnimalRace::RoleActivityAnimalRace(int type)
	:RoleActivity(type)
{

}

RoleActivityAnimalRace::~RoleActivityAnimalRace()
{

}

void RoleActivityAnimalRace::Init(RoleModuleManager *mgr, const RoleActivityRoleData &p)
{
	m_role_module_mgr = mgr;
	memcpy(&m_param, p.base_data, sizeof(RAAnimalRaceParam));
}

void RoleActivityAnimalRace::GetInitParam(RoleActivityRoleData *p)
{
	memcpy( p->base_data, &m_param, sizeof(RAAnimalRaceParam));
}

void RoleActivityAnimalRace::OnLoginSendInfo()
{
	this->SendRAAnimalRaceInfo();
}

void RoleActivityAnimalRace::Update(time_t now_second)
{
	/*if (now_second < RandActivityManager::Instance().GetRandActivityBeginTime(RAND_ACTIVITY_TYPE_ANIMAL_RACE) && now_second > RandActivityManager::Instance().GetRandActivityEndTime(RAND_ACTIVITY_TYPE_ANIMAL_RACE))
	{
	return;
	}*/

	if (m_param.next_refresh_game_info_stamp != 0 && m_param.next_refresh_game_info_stamp < now_second)
	{
		int win_number = 0;
		int response = 0;
		if (AnimalRaceManager::Instance().GetWinNumberAndResponse(m_param.server_id, m_param.date_id, &win_number, &response))
		{
			if (win_number <= 0 || win_number > MAX_ANIMAL_RACE_NUM || response <= 0)
			{
				gamelog::g_log_rand_activity.printf(LL_WARNING, "%s role[%d %s] server_id[%d] date_id[%d] bet_count[%d,%d,%d,%d] activity_type[%d]", __FUNCTION__,
					m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), m_param.server_id, m_param.date_id, m_param.ra_animal_race_bet_type_count[0],
					m_param.ra_animal_race_bet_type_count[1], m_param.ra_animal_race_bet_type_count[2], m_param.ra_animal_race_bet_type_count[3], RAND_ACTIVITY_TYPE_ANIMAL_RACE);
			}
			else if (m_param.ra_animal_race_bet_type_count[win_number - 1] > 0)
			{
				const RandActivityAnimalRaceConfig * config = static_cast<const RandActivityAnimalRaceConfig * >
					(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANIMAL_RACE));
				if (NULL == config)
				{
					return;
				}

				const RAAnimalRaceCfg & cfg = config->GetAnimalRaceCfg();
				static MailContentParam contentparam;
				int item_num = m_param.ra_animal_race_bet_type_count[win_number - 1] * response / 100;
				for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM; i++)
				{
					ItemDataWrapper item_info;
					item_info.item_id = cfg.item_consume_id;
					item_info.is_bind = true;
					if (item_num > MAX_INT16)
					{
						item_info.num = MAX_INT16;
						item_num -= MAX_INT16;
					}
					else
					{
						item_info.num = item_num;
						contentparam.item_list[i] = item_info;
						break;
					}
					contentparam.item_list[i] = item_info;
				}
				char response1[20];
				int c = 37;
				SNPRINTF(response1, sizeof(response1), "%d%c", response, c);
				SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_animal_race_subject);
				SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_animal_race_content, win_number, response1);
				MailRoute::MailToUser(m_role_module_mgr->GetRole()->GetUID(), MAIL_REASON_DEFAULT, contentparam);

			}
		}
		unsigned int tmp_timestamp = m_param.ra_animal_race_begin_timestamp;
		m_param.Reset();
		m_param.ra_animal_race_begin_timestamp = tmp_timestamp;

		this->SetRoleDataChange();
	}
}

void RoleActivityAnimalRace::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	if (RAND_ACTIVITY_TYPE_ANIMAL_RACE != rand_act_type)
	{
		return;
	}


	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity *activity = ramgr->ForceGetRandActivity(rand_act_type);
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	bool in_activity_time = ramgr->IsInRandActivityOpenTime(rand_act_type, m_param.ra_animal_race_begin_timestamp);
	if (is_force || !in_activity_time)
	{
		m_param.Reset();
		m_param.ra_animal_race_begin_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time());

		this->SetRoleDataChange();
	}
}

void RoleActivityAnimalRace::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq *raor)
{
	if (NULL == raor)
	{
		return;
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (!ramgr->IsRandActivityOpen(RAND_ACTIVITY_TYPE_ANIMAL_RACE))
	{
		return;
	}
	this->CheckInitRandActivity(RAND_ACTIVITY_TYPE_ANIMAL_RACE);

	switch (raor->opera_type)
	{
	case Protocol::RA_ANIMAL_RACE_OPERA_TYPE_ROLE_INFO:
		{
			this->SendRAAnimalRaceInfo();
		}
		break;
	case Protocol::RA_ANIMAL_RACE_OPERA_TYPE_BET:	
		{
			this->OnRAAnimalRaceBet(raor->param_1, raor->param_2);
		}
		break;
	case Protocol::RA_ANIMAL_RACE_OPERA_TYPE_ACTIVITY_INFO:
		{
			this->SendRAAnimalRaceActivityInfo();
		}
		break;
	case Protocol::RA_ANIMAL_RACE_OPERA_TYPE_ENTER_SCENE:
		{
			this->OnRAAnimalRaceEnterScene();
		}
		break;
	case Protocol::RA_ANIMAL_RACE_OPERA_TYPE_EXIT_SCENE:
		{
			this->OnRAAnimalRaceExitScene();
		}
		break;
	case Protocol::RA_ANIMAL_RACE_OPERA_TYPE_ANIMATION_INFO:
		{
			//发送动画数据,给客户端准备时间
			this->SendRAAnimalRaceAnimationInfo();	
		}
		break;
	}

}

void RoleActivityAnimalRace::SendRAAnimalRaceInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityAnimalRace * rand_activity = ramgr->GetRAAnimalRace();
	if (rand_activity == NULL) return;

	Protocol::SCRAAnimalRaceRoleInfo info;
	memcpy(info.bet_type_count, m_param.ra_animal_race_bet_type_count, sizeof(info.bet_type_count));
	rand_activity->GetTypeCountAndTypeResponse(info.everything_bet_type_count, info.type_response_rate);

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityAnimalRace::SendRAAnimalRaceActivityInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityAnimalRace * rand_activity = ramgr->GetRAAnimalRace();
	if (rand_activity == NULL) return;

	int op_type = 0;
	char rank[MAX_ANIMAL_RACE_NUM] = { 0 };
	Protocol::SCRAAnimalRaceInfo info;
	info.timestamp = rand_activity->GetTimestamp(&op_type, rank, MAX_ANIMAL_RACE_NUM);
	if (info.timestamp <= 0 || op_type < 0 || op_type > Protocol::SCRAAnimalRaceInfo::ANIMAL_RACE_INFO_TYPE_NOT_TIME) return;

	info.op_type = op_type;
	memcpy(info.rank, rank, sizeof(info.rank));
	info.count = 0;
	rand_activity->GetSceneRoleApp(info.scene_appearance, &info.count, m_role_module_mgr->GetRole());

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityAnimalRace::SendRAAnimalRaceAnimationInfo()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityAnimalRace * rand_activity = ramgr->GetRAAnimalRace();
	if (rand_activity == NULL) return;

	Protocol::SCRAAnimalRaceAnimationInfo info;
	rand_activity->GetAnimationData(&info.animation_data);

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityAnimalRace::OnRAAnimalRaceBet(int animal_race_type, int count)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (animal_race_type <= 0 || animal_race_type > MAX_ANIMAL_RACE_NUM || count <= 0) return;

	RandActivityAnimalRace * rand_activity = ramgr->GetRAAnimalRace();
	if (NULL == rand_activity) return;

	if (!rand_activity->IsBetTime())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_ANIMAL_RACE_BET_TIME);
		return;
	}

	const RandActivityAnimalRaceConfig * config = static_cast<const RandActivityAnimalRaceConfig *>
		(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(RAND_ACTIVITY_TYPE_ANIMAL_RACE));
	if (NULL == config)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_UNKNOWN_ERROR);
		return;
	}

	const RAAnimalRaceCfg & cfg = config->GetAnimalRaceCfg();
	const RAOtherCfg & other_cfg = config->GetOtherCfg();

	int item_count = m_role_module_mgr->GetKnapsack()->Count(cfg.item_consume_id);
	if (item_count < count)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_ANIMAL_RACE_ITEM_NOT_ENOUGH);
		return;
	}
	if (m_param.ra_animal_race_bet_type_count[animal_race_type - 1] + count > other_cfg.bet_upper_limit)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_ANIMAL_RACE_BET_LIMIT);
		return;
	}

	if (!m_role_module_mgr->GetKnapsack()->ConsumeItem(cfg.item_consume_id, count, __FUNCTION__))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ITEM_AMOUNT_NOT_ENOUGH);
		return;
	}
	if (m_param.next_refresh_game_info_stamp == 0)
	{
		rand_activity->GetNeedInfo(&m_param.server_id, &m_param.date_id, &m_param.next_refresh_game_info_stamp);
	}

	m_param.ra_animal_race_bet_type_count[animal_race_type - 1] += count;
	rand_activity->AddBetCount(count, animal_race_type, m_role_module_mgr->GetRole()->GetUID());
	m_role_module_mgr->NoticeNum(noticenum::NT_ANIMACE_RACE_BET_SUCC);
	this->SendRAAnimalRaceInfo();
	gamelog::g_log_rand_activity.printf(LL_WARNING, "%s role[%d %s] add_bet[type:%d count:%d] server_id[%d] date_id[%d] bet_count[%d,%d,%d,%d] activity_type[%d]", __FUNCTION__,
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), animal_race_type, count,m_param.server_id, m_param.date_id, m_param.ra_animal_race_bet_type_count[0],
		m_param.ra_animal_race_bet_type_count[1], m_param.ra_animal_race_bet_type_count[2], m_param.ra_animal_race_bet_type_count[3], RAND_ACTIVITY_TYPE_ANIMAL_RACE);
	UNSTD_STATIC_CHECK(MAX_ANIMAL_RACE_NUM == 4);
	this->SetRoleDataChange();
}

void RoleActivityAnimalRace::OnRAAnimalRaceEnterScene()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityAnimalRace * rand_activity = ramgr->GetRAAnimalRace();
	if (NULL == rand_activity) return;

	rand_activity->SceneRoleNumAdd(m_role_module_mgr->GetRole());

	this->SendRAAnimalRaceActivityInfo();

	this->OnParticipateRA(__FUNCTION__);
}

void RoleActivityAnimalRace::OnRAAnimalRaceExitScene()
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	RandActivityAnimalRace * rand_activity = ramgr->GetRAAnimalRace();
	if (NULL == rand_activity) return;

	rand_activity->SceneRoleNumLess(m_role_module_mgr->GetRole()->GetUID());
}

void RoleActivityAnimalRace::OnRAAnimalRaceRoleComment(const char* msg, unsigned int msg_length)
{
	if (msg_length >= REAL_TIME_COMMENT_LEN) return;
	Scene *scene = m_role_module_mgr->GetRole()->GetScene();
	if (NULL == scene)
	{
		return;
	}

	Protocol::SCRealTimeComment info;
	info.comment_type = Protocol::CSRealTimeComment::COMMENT_TYPE_ACT_RACE_HORSE;
	info.sender_uid = m_role_module_mgr->GetRole()->GetUID();
	m_role_module_mgr->GetRole()->GetName(info.sender_name);
	info.msg_length = msg_length;
	memcpy(info.msg_content, msg, msg_length);

	int send_length = sizeof(info) - sizeof(info.msg_content) + msg_length;
	scene->SendToRole((const char*)&info, send_length);
}
