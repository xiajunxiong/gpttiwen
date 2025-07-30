#include "marriage.hpp"
#include "engineadapter.h"
#include "world.h"
#include "other/marriage/marriageconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "obj/character/role.h"
#include "item/knapsack.h"
#include "item/money.h"
#include "servercommon/errornum.h"
#include "other/rolemodulemanager.hpp"
#include "protocol/msgmarry.hpp"
#include "servercommon/noticenum.h"
#include "global/offlineregister/offlineregistermanager.hpp"
#include "global/team/team.hpp"
#include "other/event/eventhandler.hpp"
#include "global/usercache/usercache.hpp"
#include "friend/friendmanager.h"
#include "other/marriage/marriage.hpp"
#include "other/funopen/funopen.hpp"
#include "friend/friendintimacy.hpp"

Marriage::Marriage() : m_role_module_mgr(NULL), m_req_divorce_time(0)
{
}

Marriage::~Marriage()
{
}

void Marriage::Init(RoleModuleManager *role_module_manager, const MarriageParam &param)
{
	m_role_module_mgr = role_module_manager;
	m_param = param;
}

void Marriage::GetOtherInitParam(MarriageParam * param)
{
	* param = m_param;
}

void Marriage::OnLogout()
{
	this->CancelPropose();
}

void Marriage::Update(unsigned long interval, time_t now_second)
{
	if (m_param.apply_divorce_time != 0)
	{		
		const MarriageOtherCfg & other_cfg = LOGIC_CONFIG->GetMarriageConfig()->GetOtherCfg();
		if (m_param.apply_divorce_time + other_cfg.force_divorce_time <= now_second)
		{
			this->OnDivorce((unsigned int)now_second, true);
		}		
	}
}

void Marriage::OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid)
{
	m_param.puzzle_play_times = 0;			//快乐拼图玩的次数
	m_param.question_play_times = 0;			//答题玩的次数
	m_param.curr_wave = 0;
	m_param.is_auto_fb = 0;
}

void Marriage::Propose(int marry_type, int target_uid)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_MARRIGE, true))
	{
		return;
	}

	time_t now = EngineAdapter::Instance().Time();

	if (m_propose_info.target_uid != 0 && now <= m_propose_info.time)
	{
		return;
	}

	const MarriageMarryCfg * marry_cfg = LOGIC_CONFIG->GetMarriageConfig()->GetMarriageMarryCfg(marry_type);
	if (marry_cfg == NULL)
	{
		return;
	}

	if (marry_cfg->need_coin > 0 && !m_role_module_mgr->GetMoney()->CoinBindMoreThan(marry_cfg->need_coin))
	{		
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
		return;
	}

	if (marry_cfg->item_id > 0 && !m_role_module_mgr->GetKnapsack()->HasItem(marry_cfg->item_id, marry_cfg->need_item_num))
	{		
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_STUFF_NOT_ENOUGH);
		return;
	}

	const MarriageOtherCfg & other_cfg = LOGIC_CONFIG->GetMarriageConfig()->GetOtherCfg();

	Role *marry_target_role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(target_uid));
	if (NULL == marry_target_role)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_TARGET_NOT_ONLINE);
		return;
	}

	if (marry_target_role->GetLevel() < other_cfg.need_role_level || m_role_module_mgr->GetRole()->GetLevel() < other_cfg.need_role_level)
	{
		return;
	}

	if (this->IsMarried() || marry_target_role->GetRoleModuleManager()->GetMarriage()->IsMarried())
	{		
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_MARRY_TARGET_MARRIED);
		return;
	}

	if (m_role_module_mgr->GetRole()->GetSex() == marry_target_role->GetSex())
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_MARRY_MUST_DIFFERENT_SEX_IN_TEAM);
		return;
	}

	if(FriendIntimacyManager::Instance().GetFriendIntimacy(m_role_module_mgr->GetRole()->GetUID(), target_uid)< other_cfg.need_intimacy)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_NOT_ENOUGH_FRIEND_INTIMACY);
		return;
	}

	SendReqInfo & target_propose_info = marry_target_role->GetRoleModuleManager()->GetMarriage()->m_propose_info;
	if (target_propose_info.target_uid != 0 && now <= target_propose_info.time)
	{
		m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_TARGET_ROLE_IS_PROPOSE);
		return;
	}

	m_propose_info.target_uid = target_uid;
	m_propose_info.param1 = marry_type;
	m_propose_info.time = now + 300;
	m_propose_info.is_sender = true;

	target_propose_info.Reset();
	target_propose_info.target_uid = m_role_module_mgr->GetRole()->GetUID();
	target_propose_info.time = now + 300;

	this->SendOpResult(Protocol::SCMarryProposeNotice::OP_RESULT_PROPOSE_SUCC);

	Protocol::SCMarryReqRoute mrr;
	mrr.marry_type = marry_type;
	mrr.req_uid = UidToInt(m_role_module_mgr->GetRole()->GetUserId());
	m_role_module_mgr->GetRole()->GetName(mrr.req_name);
	EngineAdapter::Instance().NetSend(marry_target_role->GetNetId(), (const char*)&mrr, sizeof(mrr));
}

void Marriage::CancelPropose()
{
	Role *marry_target_role = World::GetInstWorld()->GetSceneManager()->GetRole(m_propose_info.target_uid);
	if (NULL != marry_target_role)
	{
		Protocol::SCMarryReqRoute mrr;
		mrr.marry_type = -1;
		mrr.req_uid = m_role_module_mgr->GetRole()->GetUID();
		m_role_module_mgr->GetRole()->GetName(mrr.req_name);
		EngineAdapter::Instance().NetSend(marry_target_role->GetNetId(), (const char*)&mrr, sizeof(mrr));
	}

	m_propose_info.Reset();
}

void Marriage::ProposeReply(Role * marry_target_role, bool is_accept)
{
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_MARRIGE, true))
	{
		return;
	}

	if (NULL == marry_target_role)
	{
		return;
	}

	if (this->IsMarried() || marry_target_role->GetRoleModuleManager()->GetMarriage()->IsMarried())
	{
		return;
	}

	if (m_propose_info.target_uid != marry_target_role->GetUID() || !m_propose_info.is_sender)
	{
		return;
	}

	if (EngineAdapter::Instance().Time() > m_propose_info.time)
	{
		return;
	}

	if (!is_accept)
	{
		this->SendOpResult(Protocol::SCMarryProposeNotice::OP_RESULT_REFUSE);
		m_propose_info.Reset();
		marry_target_role->GetRoleModuleManager()->GetMarriage()->m_propose_info.Reset();
		return;
	}

	const MarriageMarryCfg * marry_cfg = LOGIC_CONFIG->GetMarriageConfig()->GetMarriageMarryCfg(m_propose_info.param1);
	if (marry_cfg == NULL)
	{
		return;
	}

	if (marry_cfg->need_coin > 0 && !m_role_module_mgr->GetMoney()->UseCoinBind(marry_cfg->need_coin, __FUNCTION__))
	{
		return;
	}

	if (marry_cfg->item_id > 0 && !m_role_module_mgr->GetKnapsack()->ConsumeItem(marry_cfg->item_id, marry_cfg->need_item_num, __FUNCTION__))
	{
		return;
	}

	m_param.last_marry_time = (unsigned int)EngineAdapter::Instance().Time();
	m_param.lover_uid = marry_target_role->GetUID();
	marry_target_role->GetName(m_param.lover_name);

	MarriageParam & target_param = marry_target_role->GetRoleModuleManager()->GetMarriage()->m_param;
	target_param.last_marry_time = (unsigned int)EngineAdapter::Instance().Time();
	target_param.lover_uid = m_role_module_mgr->GetRole()->GetUID();
	m_role_module_mgr->GetRole()->GetName(target_param.lover_name);

	this->SendInfo();
	marry_target_role->GetRoleModuleManager()->GetMarriage()->SendInfo();

	EventHandler::Instance().OnMarry(m_role_module_mgr->GetRole(), m_propose_info.param1);
	EventHandler::Instance().OnMarry(marry_target_role, m_propose_info.param1);

	m_propose_info.Reset();
	marry_target_role->GetRoleModuleManager()->GetMarriage()->m_propose_info.Reset();
}

void Marriage::DivorceReq()
{
	if (!this->IsMarried())
	{
		return;
	}

	const MarriageOtherCfg & other_cfg = LOGIC_CONFIG->GetMarriageConfig()->GetOtherCfg();

	time_t now = EngineAdapter::Instance().Time();
	if (m_param.last_marry_time + other_cfg.can_divorce_time > now)
	{
		return;
	}

	UserID target_userid = IntToUid(m_param.lover_uid);
	Role * target = World::GetInstWorld()->GetSceneManager()->GetRole(target_userid);
	if (NULL != target)
	{
		Protocol::SCDivorceReqRoute protocol;
		EngineAdapter::Instance().NetSend(target->GetNetId(), (const char*)&protocol, sizeof(protocol));

		m_req_divorce_time = (unsigned int)now;
	}	
}

void Marriage::DivorceReply(Role * reply_role, bool is_accpet)
{
	if (reply_role == NULL)
	{
		return;
	}

	if (!is_accpet)
	{
		return;
	}

	if (reply_role->GetUID() != m_param.lover_uid)
	{
		return;
	}

	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
	if (m_req_divorce_time != 0 && m_req_divorce_time + 300 < now)
	{
		return;
	}

	this->OnDivorce(now);
	reply_role->GetRoleModuleManager()->GetMarriage()->OnDivorce(now);
}

void Marriage::ForceDivorceReq()
{
	if (!this->IsMarried())
	{
		return;
	}

	m_param.apply_divorce_time = (unsigned int)EngineAdapter::Instance().Time();
}

void Marriage::CancelForceDivorce()
{
	m_param.apply_divorce_time = 0;
}

void Marriage::OnDivorce(unsigned int divorce_time, bool clear_lover)
{
	if (!this->IsMarried())
	{
		return;
	}

	int lover_uid = m_param.lover_uid;

	m_param.last_divorce_time = divorce_time;
	m_param.lover_uid = 0;
	memset(m_param.lover_name, 0, sizeof(m_param.lover_name));
	m_param.last_marry_time = 0;	
	m_param.apply_divorce_time = 0;
	m_req_divorce_time = 0;

	this->SendInfo();

	if (clear_lover)
	{
		UserID target_userid = IntToUid(lover_uid);
		Role *target = World::GetInstWorld()->GetSceneManager()->GetRole(target_userid);
		if (NULL != target)
		{
			target->GetRoleModuleManager()->GetMarriage()->OnDivorce(divorce_time);
		}
		else
		{
			OfflineRegisterManager::Instance().OnDivorceReq(target_userid, divorce_time);
		}
	}
}

bool Marriage::CanStart(int param1)
{
	switch (param1)
	{
		case MARRIAGE_TASK_1:
			{
				return m_param.puzzle_play_times == 0;
			}
			break;
		case MARRIAGE_TASK_2:
			{
				return m_param.question_play_times == 0;
			}
			break;

		case MARRIAGE_TASK_3:
			{
			}
			break;

		case MARRIAGE_TASK_4:
			{
				return m_param.puzzle_play_times == 0;
			}
			break;

		case MARRIAGE_TASK_5:
			{

				return m_param.question_play_times == 0;
			}
			break;
		default:
			break;
	}


	return true;
}

bool Marriage::CanAutoFb()
{

	bool f = m_param.max_pass_fb_wave % 10 * 10 > m_param.curr_wave;

	return  f || m_param.is_auto_fb;
}

void Marriage::SetPlayTimes(int param1, int times)
{
	switch (param1)
	{
		case MARRIAGE_TASK_1:
			{
				m_param.puzzle_play_times = times;
			}
			break;
		case MARRIAGE_TASK_2:
			{
				m_param.question_play_times = times;
			}
			break;

		case MARRIAGE_TASK_4:
			{
				m_param.puzzle_play_times = times;
			}
			break;

		case MARRIAGE_TASK_5:
			{

				m_param.question_play_times = times;
			}
			break;
		default:
			break;
	}

	this->SendInfo();
}

void Marriage::SendMarriageFbReward( int wave,bool is_auto_fb)
{
	for (int i = 0; i < wave; ++i)
	{
		const MarriageTaskFbCfg * fb_cfg = LOGIC_CONFIG->GetMarriageConfig()->GetMarriageTaskFbCfg(m_param.curr_wave + i);
		if (fb_cfg == NULL)
		{
			break;
		}

		if (fb_cfg->rewards.size() >= 0)
		{
			m_role_module_mgr->GetKnapsack()->PutList((short)fb_cfg->rewards.size(), &fb_cfg->rewards[0], PUT_REASON_MARRIAGE_TASK);
		}		
	}

	if (!is_auto_fb)
	{
		const MarriageTaskRewardCfg *  cfg = LOGIC_CONFIG->GetMarriageConfig()->GetMarriageTaskRewardCfg(MARRIAGE_TASK_3);
		if (cfg != NULL)
		{
			if (cfg->reward_list.size() > 0)
			{
				m_role_module_mgr->GetKnapsack()->PutList((short)cfg->reward_list.size(), &cfg->reward_list[0], PUT_REASON_MARRIAGE_TASK);
			}
		}

		m_param.curr_wave += wave;

		if (m_param.max_pass_fb_wave < m_param.curr_wave)
		{
			m_param.max_pass_fb_wave = m_param.curr_wave;
		}

		this->SetPlayTimes(MARRIAGE_TASK_3);
	}
}

MarriageParam & Marriage::GetParam()
{
	return m_param;
}

void Marriage::QueryOtherMarriageInfo(int role_id)
{
	Role * query_role = World::GetInstWorld()->GetSceneManager()->GetRole(role_id);
	if (NULL != query_role)
	{
		Protocol::SCMarriageQueryRet protocol;
		protocol.role_id = role_id;
		protocol.lover_id = query_role->GetRoleModuleManager()->GetMarriage()->GetLoverUid();
		protocol.prof = query_role->GetProfession();
		protocol.sex = query_role->GetSex();

		EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), &protocol, sizeof(protocol));
	}
}

bool Marriage::IsOnlyLoverInTeam()
{
	Team * team = m_role_module_mgr->GetRole()->GetMyTeam();
	if (team == NULL)
	{
		return false;
	}

	if (team->GetMemberCount() != 2 || !team->IsMember(m_param.lover_uid))
	{
		return false;
	}

	return true;
}

bool Marriage::IsMarried()
{
	return m_param.lover_uid > 0;
}

bool Marriage::IsNewlyMarried()
{
	if (EngineAdapter::Instance().Time() < m_param.last_marry_time + LOGIC_CONFIG->GetMarriageConfig()->GetOtherCfg().newly_married_time)
	{
		return true;
	}

	return false;
}

Role * Marriage::GetLover()
{
	return World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(m_param.lover_uid));
}

int Marriage::GetLoverUid()
{
	return m_param.lover_uid;
}

void Marriage::SendInfo()
{
	Protocol::SCMarryInfo protocol;
	protocol.lover_uid = m_param.lover_uid;
	memset(protocol.lover_name, 0, sizeof(protocol.lover_name));

	UserCacheNode * user_cache_node = UserCacheManager::Instance().GetUserNode(m_param.lover_uid);
	if (NULL != user_cache_node)
	{
		user_cache_node->GetName(protocol.lover_name);
		protocol.avatar_type = user_cache_node->avatar_type;
	}


	protocol.last_marry_time = m_param.last_marry_time;
	protocol.last_divorce_time = m_param.last_divorce_time;
	protocol.apply_divorce_time = m_param.apply_divorce_time;

	protocol.puzzle_play_times = m_param.puzzle_play_times;
	protocol.question_play_times = m_param.question_play_times;
	protocol.max_pass_fb_wave = m_param.max_pass_fb_wave;
	protocol.curr_wave = m_param.curr_wave;
	protocol.is_auto_fb = m_param.is_auto_fb;

	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*)&protocol, sizeof(protocol));
}

void Marriage::SendOpResult(int type)
{
	Protocol::SCMarryProposeNotice notice;
	notice.result = type;
	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*)&notice, sizeof(notice));
}


