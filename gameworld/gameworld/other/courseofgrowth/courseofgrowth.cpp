#include "courseofgrowth.hpp"
#include "courseofgrowthconfig.hpp"
#include "gamelog.h"
#include "protocol/msgcourseofgrowth.h"
#include "other/rolemodulemanager.hpp"
#include "other/pet/pet.hpp"
#include "other/skill/roleskill.hpp"
#include "other/couragechallenge/couragechallenge.h"
#include "task/taskrecorder.h"
#include "obj/character/role.h"
#include "servercommon/errornum.h"
#include "config/logicconfigmanager.hpp"
#include "servercommon/bitmapfunction.hpp"
#include "item/money.h"
#include "item/itempool.h"
#include "item/knapsack.h"
#include "friend/friendmanager.h"
#include "other/pet/petconfig.hpp"
#include "battle/battle_manager_local.hpp"
#include "scene/fbmanager.hpp"
#include "other/route/mailroute.hpp"
#include "servercommon/string/gameworldstr.h"
#include "other/miyi/miyiconfig.hpp"
#include "gameworld/obj/obj.h"
#include "gameworld/scene/scene.h"
#include "other/train/train.hpp"
#include "other/miyi/rolemiyi.hpp"
#include "other/event/eventhandler.hpp"
#include "other/bountytask/bountytask.hpp"
#include "other/fabao/fabao.hpp"
#include "other/mentor/shitu.hpp"
#include "other/partner/partner.hpp"
#include "other/surface/surface.hpp"
#include "other/facescore/facescore.hpp"
#include "other/yinyangeye/yinyangeye.hpp"
#include "other/capability/capability.hpp"
#include "other/realmbreak/realmbreak.hpp"
#include "other/petmonopoly/petmonopoly.hpp"
#include "other/anecdotetask/anecdotetask.hpp"
#include "other/rolecrystalfb/rolecrystalfb.h"
#include "other/rolefirstkill/rolefirstkill.hpp"
#include "other/lunhuiwangchuan/lunhuiwangchuan.hpp"
#include "other/rolecrystalfb/rolecrystalfbconfig.hpp"
#include "other/salary/salary.hpp"
#include "scene/scene.h"
#include "scene/speciallogic.hpp"
#include "global/courseofgrowthmanager/courseofgrowthmanager.hpp"

CourseOfGrowth::CourseOfGrowth() : m_course_level(0), m_course_cur_fraction(0)
{
	memset(m_task_prog_list, 0, sizeof(m_task_prog_list));
	m_prog_num_map.clear();
	memset(m_course_prog_list, 0, sizeof(m_course_prog_list));
	m_course_prog_num_map.clear();
}

CourseOfGrowth::~CourseOfGrowth()
{
}

void CourseOfGrowth::Init(RoleModuleManager * role_module_manager, const CourseOfGrowthParam & param, int buff_len, const char *buff_cur, const RoleCourseParam & course_param)
{
	m_mgr = role_module_manager;
	m_param = param;
	m_course_param = course_param;

	if (NULL != buff_cur && buff_len >= (int)sizeof(short))
	{
		short total_count = *(short *)buff_cur;
		buff_cur += sizeof(short);
		buff_len -= sizeof(short);

		for (int i = 0; i < total_count; ++i)
		{
			if (buff_len <= 0) break;

			UInt32 key = *(UInt32 *)buff_cur;
			buff_cur += sizeof(UInt32);
			buff_len -= sizeof(UInt32);

			UInt32 val = *(UInt32 *)buff_cur;
			buff_cur += sizeof(UInt32);
			buff_len -= sizeof(UInt32);

			m_prog_num_map[key] = val;
		}
	}
}

void CourseOfGrowth::GetCourseOfGrowthParam(CourseOfGrowthParam * param, char * buff_cur, int * buff_len, RoleCourseParam * course_param)
{
	*param = m_param;
	*course_param = m_course_param;

	if (NULL == buff_cur || NULL == buff_len) return;

	*buff_len = 0;

	short total_count = (short)(m_prog_num_map.size());
	if (total_count > MAX_COG_RECORD_NUM) total_count = MAX_COG_RECORD_NUM;

	*(short *)buff_cur = total_count;
	buff_cur += sizeof(short);
	*buff_len += sizeof(short);

	int idx = 0;
	for (std::map<UInt32, UInt32>::iterator loop = m_prog_num_map.begin(); loop != m_prog_num_map.end(); ++loop)
	{
		*(UInt32 *)buff_cur = loop->first;
		buff_cur += sizeof(UInt32);
		*buff_len += sizeof(UInt32);

		*(UInt32 *)buff_cur = loop->second;
		buff_cur += sizeof(UInt32);
		*buff_len += sizeof(UInt32);

		if (++idx >= MAX_COG_RECORD_NUM) break;
	}
}

void CourseOfGrowth::InitEnd()
{
	if (m_param.cur_star > GROWTH_TASK_NUM_PER_TYPE_STAR)//检测星级问题
	{
		int old_prog = m_param.cur_prog;
		LOGIC_CONFIG->GetCourseOfGrowthConfig()->SetNextProgAndStar(&m_param.cur_prog, &m_param.cur_star);
		this->SendStarAwardMail(m_param.cur_reward_prog, GROWTH_TASK_NUM_PER_TYPE_STAR);   //发放当前层数奖励后再修改
		m_param.cur_reward_prog = m_param.cur_prog;
		m_param.cur_reward_star = m_param.cur_star;
		m_param.award_fetch_flag.Reset();
		m_param.finish_notify_flag.Reset();
		memset(m_task_prog_list, 0, sizeof(m_task_prog_list));
		m_mgr->OnAddGuildEvent(m_param.cur_prog, m_param.cur_star, GUILD_EVENTHANDLER_TYPE_COURSE_OF_GROWTH);
		this->SendCOGAllInfo();

		if (old_prog != m_param.cur_prog)
		{
			EventHandler::Instance().OnCourseOfGrowthProgChange(m_mgr->GetRole(), old_prog, m_param.cur_prog);
		}
	}
	this->RefreshAllTaskProgress(true, false);
}

void CourseOfGrowth::OnLogin()
{
	this->OnGiveOldUserBaseInfo();
	this->OnCourseInit();
}

void CourseOfGrowth::EventOperator(int condition, int param1, int param2)
{
	if (condition < 0 || condition >= COG_CON_TYPE_MAX) return;

	if (this->IsNeedSaveInValueList(condition))
	{
		UInt32 key = condition * GROWTH_KEY_MULT + param1;
		UInt32 & value = m_prog_num_map[key];

		long long course_key = 1LL * condition * COURSE_ENTRY_KEY_MULT + param1;
		UInt32 & course_value = m_course_prog_num_map[course_key];

		if (this->IsTaskProgressAddUp(condition))
		{
			if (value >= MAX_UINT32 - param2)
			{
				course_value = value = MAX_UINT32;
			}
			else
			{
				course_value = value += param2;
			}
		}
		else
		{
			if (value < param2)
			{
				course_value = value = param2;
			}
		}
	}

	this->RefreshAllTaskProgress(false, true, condition);
	this->OnRefreshCourseProgress(condition, param1, param2);
}

void CourseOfGrowth::GetTaskAward(int seq)
{
	if (m_param.award_fetch_flag.IsBitSet(seq))
	{
		m_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	if (!this->IsTaskMeetTheCondition(seq))
	{
		m_mgr->NoticeNum(errornum::EN_CONDITIONS_NOT_REACHED);
		return;
	}

	const COGTaskCon * cog_task = LOGIC_CONFIG->GetCourseOfGrowthConfig()->GetCOGTaskInfo(m_param.cur_prog, m_param.cur_star, seq);
	if (NULL == cog_task)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (cog_task->award_con.reward_num > 0 && !m_mgr->GetKnapsack()->CheckForPutList(cog_task->award_con.reward_num, cog_task->award_con.reward_list))
	{
		return;
	}

	if (cog_task->award_con.reward_coin > 0) m_mgr->GetMoney()->AddCoinBind(cog_task->award_con.reward_coin, "CourseOfGrowthRewardCoin");
	if (cog_task->award_con.reward_exp > 0) m_mgr->GetRole()->AddExp(cog_task->award_con.reward_exp, ADD_EXP_TYPE_COURSE_OF_GROWTH, __FUNCTION__);
	if (cog_task->award_con.reward_num > 0) m_mgr->GetKnapsack()->PutList(cog_task->award_con.reward_num, cog_task->award_con.reward_list, PUT_REASON_ACHIEVEMENT_REWARD);

	m_param.award_fetch_flag.SetBit(seq);
	this->SendSingleFlag(seq);
}

void CourseOfGrowth::BeginToFightCOGBoss()
{
	if (!m_mgr->GetRole()->GetScene()->GetSpecialLogic()->CanJoinOtherActivity())
	{
		m_mgr->NoticeNum(errornum::EN_PLAY_ACTIVITY_NOW_ERR);
		return;
	}
	for (int seq = 0; seq < GROWTH_TASK_NUM_PER_STAR; ++seq)
	{
		if (!m_param.award_fetch_flag.IsBitSet(seq))
		{
			m_mgr->NoticeNum(errornum::EN_REWARD_HAS_NOT_GET);
			return;
		}
	}

	const int boss_group_id = LOGIC_CONFIG->GetCourseOfGrowthConfig()->GetBossGroupID(m_param.cur_prog, m_param.cur_star);
	if (boss_group_id <= 0)
	{
		m_mgr->NoticeNum(errornum::EN_MONSTER_GROUP_NOT_EXIST);
		return;
	}

	BattleManagerLocal::GetInstance().StartBattleModeFightReq(m_mgr->GetRole(), BATTLE_MODE_COURSE_OF_GROWTH, boss_group_id, 0, 0, true);
}

void CourseOfGrowth::OnDefeatCOGBoss()
{
	this->GoToTheNextStarLevel();
}

void CourseOfGrowth::OnFetchCourseLevelReward(int level)
{
	int level_seq = level - 1;
	if (this->IsCourseLevelSeqInvaild(level_seq))
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (m_course_level < level)
	{
		m_mgr->NoticeNum(errornum::EN_COURSE_LEVEL_NOT_ENOUGH_FETCH_FAILED);
		return;
	}
	const CourseLevelRewardCfg * level_cfg = LOGIC_CONFIG->GetCourseOfGrowthConfig()->GetCourseLevelRewardCfg(level);
	if (NULL == level_cfg)
	{
		m_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	if (m_course_param.level_reward_fetch_flag.IsBitSet(level_seq))
	{
		m_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}
	if (!level_cfg->reward_list.empty())
	{
		if (!m_mgr->GetKnapsack()->CheckForPutList2(level_cfg->reward_list, false))
		{
			m_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
			return;
		}
		m_mgr->GetKnapsack()->PutVec(level_cfg->reward_list, PUT_REASON_ACHIEVEMENT_REWARD);
	}

	m_mgr->GetMoney()->AddCoinBind(level_cfg->coin, __FUNCTION__);
	if (level_cfg->exp > 0)
	{
		m_mgr->GetRole()->AddExp(level_cfg->exp, ADD_EXP_TYPE_COURSE_OF_GROWTH, __FUNCTION__);
	}
	
	m_course_param.level_reward_fetch_flag.SetBit(level_seq);
	m_course_param.course_level_version_list[level_seq] = level_cfg->version;
	this->SendCourseNoticeInfo(Protocol::SCRoleCourseNoticeInfo::ROLE_COURSE_NOTICE_TYPE_LEVEL_FECHT, level, 1);
	gamelog::g_log_achievement.printf(LL_INFO, "%s level[%d] version[%d]", __FUNCTION__, level, level_cfg->version);
}

bool CourseOfGrowth::IsReachCourseOfGrowth(int prog, int star)
{
	if (m_param.cur_prog >= prog && m_param.cur_star >= star)
	{
		return true;
	}

	return false;
}

void CourseOfGrowth::SendCOGAllInfo()
{
	Protocol::SCCOGAllInformations info;

	info.cur_prog = m_param.cur_prog;
	info.cur_star = m_param.cur_star;
	info.cur_reward_prog = m_param.cur_reward_prog;
	info.cur_reward_star = m_param.cur_reward_star;
	memcpy(info.task_prog_list, m_task_prog_list, sizeof(info.task_prog_list));
	info.award_fetch_flag = m_param.award_fetch_flag;

	m_mgr->NetSend((const void *)&info, sizeof(info));
}

void CourseOfGrowth::SendCourseAllInfo()
{
	Protocol::SCRoleCourseAllInfo info;
	info.course_level = m_course_level;
	info.course_cur_fraction = m_course_cur_fraction;
	info.finish_record_flag = m_course_param.finish_record_flag;
	info.level_reward_fetch_flag = m_course_param.level_reward_fetch_flag;
	info.count = LOGIC_CONFIG->GetCourseOfGrowthConfig()->GetMaxCourseSeq();
	if (info.count > ARRAY_LENGTH(m_course_prog_list))
	{
		info.count = ARRAY_LENGTH(m_course_prog_list);
	}
	if (info.count > 0)
	{
		memcpy(info.course_prog_list, m_course_prog_list, sizeof(info.course_prog_list[0]) * info.count);
	}
	else
	{
		info.count = 0;
	}

	int length  = sizeof(info) - (ARRAY_LENGTH(info.course_prog_list) - info.count) * sizeof(info.course_prog_list[0]);
	m_mgr->NetSend(&info, length);
}

void CourseOfGrowth::GmAddProgress(int seq, int prog)
{
	if (seq < 0 || seq >= GROWTH_TASK_NUM_PER_STAR) return;

	const COGTaskCon * task_info = LOGIC_CONFIG->GetCourseOfGrowthConfig()->GetCOGTaskInfo(m_param.cur_prog, m_param.cur_star, seq);
	if (NULL == task_info) return;

	this->EventOperator(task_info->condtion, task_info->param1, prog);
}

void CourseOfGrowth::GmSetProgress(int prog, int star)
{
	if(prog < GROWTH_PROG_BEGINNING || prog > GROWTH_TASK_NUM_PER_TYPE_STAGE || star < GROWTH_STAR_BEGINNING || star > GROWTH_TASK_NUM_PER_TYPE_STAR) return;

	m_param.cur_prog = prog;
	m_param.cur_star = star;
	this->SendCOGAllInfo();
	this->RefreshAllTaskProgress(false, false);
}

void CourseOfGrowth::RefreshAllTaskProgress(bool is_init, bool is_floating_word, int condition)
{
	UInt32 tmp_prog_list[GROWTH_TASK_NUM_PER_STAR] = { 0 };
	memcpy(tmp_prog_list, m_task_prog_list, sizeof(tmp_prog_list));

	for (int seq = 0; seq < GROWTH_TASK_NUM_PER_STAR; ++seq)
	{
		const COGTaskCon * cog_task = LOGIC_CONFIG->GetCourseOfGrowthConfig()->GetCOGTaskInfo(m_param.cur_prog, m_param.cur_star, seq);
		if (NULL == cog_task) continue;

		if (-1 == condition || cog_task->condtion == condition)
		{
			UInt32 key = cog_task->condtion * GROWTH_KEY_MULT;

			UNSTD_STATIC_CHECK(77 == COG_CON_TYPE_MAX)
			switch (cog_task->condtion)
			{
			case COG_CON_REACHS_FB_TIER_NUMB:
				key += cog_task->param1;
				switch (cog_task->param1)
				{
				case FBManager::FB_TYPE_COURAGE_CHALLENGE:
					m_prog_num_map[key] = m_mgr->GetCourageChallenge()->GetReachStarScoreCount();
					break;
				case FBManager::FB_TYPE_BRAVE_GROUND:
					m_prog_num_map[key] = m_mgr->GetCommonData().enter_brave_ground_times;
					break;
				}
				break;
			case COG_CON_SKILLS_REACHS_LEVEL:
				m_prog_num_map[key] = m_mgr->GetRoleSkill()->TheNumOfLevelSkill(cog_task->param1);
				break;
			case COG_CON_PET_NUM_REACH_LEVEL:
				m_prog_num_map[key] = m_mgr->GetPet()->TheNumOfLevelPet(cog_task->param1);
				break;
			case COG_CON_REACHS_APPOINTED_LV:
				m_prog_num_map[key] = m_mgr->GetRole()->GetLevel() >= cog_task->param1 ? 1 : 0;
				break;
			case COG_CON_APPOINTED_TASK_DONE:
				m_prog_num_map[key] = m_mgr->GetTaskRecorder()->HaveFinished(cog_task->param1) ? 1 : 0;
				break;
			case COG_CON_PARTNER_UP_LEVEL:
				m_prog_num_map[key] = m_mgr->GetPartner()->GetPartnerCurCount(cog_task->param1);
				break;
			case COG_CON_PET_PRACTICE_LEVEL:
				m_prog_num_map[key] = m_mgr->GetTrain()->GetRolePetTrainSkillLevel();
				break;
			/*case COG_CON_BOUNTY_TASK_DONE:
				m_prog_num_map[key] = m_mgr->GetBountyTask()->GetTodayFinish();
				break;*/
			case COG_CON_MIYI_KILL_LAYER_NUM:
				key += cog_task->param1;
				m_prog_num_map[key] = m_mgr->GetMiYi()->GetCurLayerFinishNum(cog_task->param1);
				break;
			case COG_CON_KILL_MONTER_GROUP:
				m_prog_num_map[key] = m_mgr->GetRoleFirstKill()->HasKillMonsterGroup(cog_task->param1) ? 1 : 0;
				break;
			case COG_CON_PET_MONOPOLY_LEVEL:
				m_prog_num_map[key] = m_mgr->GetPetMonopoly()->GetCurLevel();
				break;
			case COG_CON_TRAIN_SKILL_LEVEL:
				m_prog_num_map[key] = m_mgr->GetTrain()->GetRoleTrainSkillLevelNum(cog_task->param1);
				break;
			case COG_CON_LUN_HUI_WANG_CHUAN:
				m_prog_num_map[key] = m_mgr->GetLunHuiWangChuan()->GetPassTimes();
				break;
			case COG_CON_SHAN_HAI_XIANG_YAO:
				m_prog_num_map[key] = m_mgr->GetPassShanHaiTimes();
				break;
			case COG_CON_REALM_BREAK_LEVEL:
				m_prog_num_map[key] = m_mgr->GetRealmBreak()->GetCurLevel() >= cog_task->param1 ? 1 : 0;
				break;
			case COG_CON_PET_INTENSIFY_LEVEL:
				if (0 != cog_task->param1)
				{
					m_prog_num_map[key] = m_mgr->GetPet()->GetPetNumByQualityAndStr(cog_task->param1, cog_task->param2);
				}
				else
				{
					m_prog_num_map[key] = m_mgr->GetPet()->GetPetIntensifyMax();
				}
				break;
			case COG_CON_CRYSTAL_FB_XI_YOU:
				m_prog_num_map[key] = m_mgr->GetRoleCrystalFb()->HasFristPassCrystalFb(cog_task->param1) ? 1 : 0;
				break;			
			case COG_CON_REAML_BREAK_FIGHT:
				m_prog_num_map[key] = m_mgr->GetRealmBreak()->IsPassFightLevel(cog_task->param1) ? 1 : 0;
				break;
			case COG_CON_REACH_CAPABILITY:
				if (COG_CON_REACH_CAPABILITY_TYPE_COUNT == cog_task->param1)
				{
					m_prog_num_map[key] = m_mgr->GetRole()->GetCapabilityWithFightPetAndPartner();
				}
				else
				{
					m_prog_num_map[key] = m_mgr->GetCapability()->GetRoleCapability();
				}
				break;	
			case COG_CON_HAS_FA_BAO_COUNT:
				m_prog_num_map[key] = m_mgr->GetFaBao()->GetFaBaoNumByColorAndLevel(cog_task->param1, 1);
				break;
			case COG_CON_HAS_ACTIVE_FACE:
				m_prog_num_map[key] = m_mgr->GetFaceScore()->GetActiveCount(cog_task->param1);
				break;
			case COG_CON_HAS_SURFACE:
				m_prog_num_map[key] = m_mgr->GetSurface()->GetActiveCount(cog_task->param1);
				break;
			case COG_CON_GUILD_FIGHT_RANK:
				m_prog_num_map[key] = m_mgr->IsMeetGuildFightRank(cog_task->param1) ? 1 : 0;
				break;
			case COG_CON_GUILD_HONOR_RANK:
				m_prog_num_map[key] = m_mgr->IsMeetGuildHonorRank(cog_task->param1) ? 1 : 0;	
				break;
			case COG_CON_FETCH_GUILD_HONGBAO:
				m_prog_num_map[key] = m_mgr->GetCommonData().guild_hong_bao_fetch_count;
				break;
			case COG_CON_GUILD_ANSWER:
				m_prog_num_map[key] = m_mgr->GetCommonData().guild_answer_correct_count;
				break;
			case COG_CON_CHANNEL_CHAT:
				m_prog_num_map[key] = m_mgr->GetChannelChatByType(cog_task->param1);
				break;
			case COG_CON_HAS_FRIEND_COUNT:
				m_prog_num_map[key] = FriendManager::Instance().GetFriendNumber(m_mgr->GetRole()->GetUID(), cog_task->param1); 
				break;
			case COG_CON_APPRENTICE_COUNT:
				m_prog_num_map[key] = m_mgr->GetShiTu()->GetApprenticeCount();
				break;
			case COG_CON_ANECDOTE_TASK:
				m_prog_num_map[key] = m_mgr->GetAnecdoteTask()->IsFinishTask(cog_task->param1);
				break;
			case COG_CON_CHIVALROUS:
				m_prog_num_map[key] = m_mgr->GetSalary()->GetHistoryChivalrous();
				break;
			default:
				key += cog_task->param1;
				break;
			}

			m_task_prog_list[seq] = m_prog_num_map[key];
			if (!is_init && tmp_prog_list[seq] != m_task_prog_list[seq])
			{
				this->SendSingleProg(seq);

				if (this->IsTaskMeetTheCondition(seq))
				{
					if (is_floating_word)
						this->SendSingleNoti(seq);
					else
					{
						if (m_param.finish_notify_flag.IsBitSet(seq)) return;
						m_param.finish_notify_flag.SetBit(seq);
					}
				}
			}
		}

	}
}

bool CourseOfGrowth::IsNeedSaveInValueList(int condition)
{
	bool need = true;
	switch (condition)
	{
	case COG_CON_REACHS_FB_TIER_NUMB:
	case COG_CON_SKILLS_REACHS_LEVEL:
	case COG_CON_PET_NUM_REACH_LEVEL:
	case COG_CON_REACHS_APPOINTED_LV:
	case COG_CON_APPOINTED_TASK_DONE:
	case COG_CON_PARTNER_UP_LEVEL:
	case COG_CON_PET_PRACTICE_LEVEL:
	case COG_CON_MIYI_KILL_LAYER_NUM:
	case COG_CON_KILL_MONTER_GROUP:
	case COG_CON_PET_MONOPOLY_LEVEL:
	case COG_CON_TRAIN_SKILL_LEVEL:
	case COG_CON_LUN_HUI_WANG_CHUAN:
	case COG_CON_SHAN_HAI_XIANG_YAO:
	case COG_CON_REALM_BREAK_LEVEL:
	case COG_CON_PET_INTENSIFY_LEVEL:
	case COG_CON_CRYSTAL_FB_XI_YOU:
	case COG_CON_REAML_BREAK_FIGHT:
	case COG_CON_REACH_CAPABILITY:
	case COG_CON_HAS_FA_BAO_COUNT:
	case COG_CON_HAS_ACTIVE_FACE:
	case COG_CON_HAS_SURFACE:
	case COG_CON_GUILD_FIGHT_RANK:
	case COG_CON_GUILD_HONOR_RANK:
	case COG_CON_FETCH_GUILD_HONGBAO:
	case COG_CON_GUILD_ANSWER:
	case COG_CON_CHANNEL_CHAT:
	case COG_CON_HAS_FRIEND_COUNT:
	case COG_CON_APPRENTICE_COUNT:
	case COG_CON_ANECDOTE_TASK:
	case COG_CON_CHIVALROUS:
		need = false;
		break;
	}
	UNSTD_STATIC_CHECK(77 == COG_CON_TYPE_MAX)

	return need;
}

void CourseOfGrowth::GoToTheNextStarLevel()
{
	for (int seq = 0; seq < GROWTH_TASK_NUM_PER_STAR; ++seq)
	{
		if (!m_param.award_fetch_flag.IsBitSet(seq))
		{
			m_mgr->NoticeNum(errornum::EN_REWARD_HAS_NOT_GET);
			return;
		}
	}

	int old_prog = m_param.cur_prog;
	LOGIC_CONFIG->GetCourseOfGrowthConfig()->SetNextProgAndStar(&m_param.cur_prog, &m_param.cur_star);
	this->SendStarAwardMail(m_param.cur_reward_prog, m_param.cur_reward_star);   //发放当前层数奖励后再修改
	m_param.cur_reward_prog = m_param.cur_prog;
	m_param.cur_reward_star = m_param.cur_star;
	m_param.award_fetch_flag.Reset();
	m_param.finish_notify_flag.Reset();
	memset(m_task_prog_list, 0, sizeof(m_task_prog_list));
	m_mgr->OnAddGuildEvent(m_param.cur_prog, m_param.cur_star, GUILD_EVENTHANDLER_TYPE_COURSE_OF_GROWTH);
	this->SendCOGAllInfo();
	this->RefreshAllTaskProgress(false, false);

	if (old_prog != m_param.cur_prog)
	{
		EventHandler::Instance().OnCourseOfGrowthProgChange(m_mgr->GetRole(), old_prog,m_param.cur_prog);
	}
}

bool CourseOfGrowth::IsTaskMeetTheCondition(int seq)
{
	if (seq < 0 || seq >= GROWTH_TASK_NUM_PER_STAR) return false;

	const COGTaskCon * cog_task = LOGIC_CONFIG->GetCourseOfGrowthConfig()->GetCOGTaskInfo(m_param.cur_prog, m_param.cur_star, seq);
	if (NULL == cog_task) return false;

	UInt32 need_progress = 0;
	switch (cog_task->condtion)
	{
	case COG_CON_REACHS_APPOINTED_LV:
	case COG_CON_APPOINTED_TASK_DONE:
	case COG_CON_PET_INTENSIFY_LEVEL:
	case COG_CON_CRYSTAL_FB_XI_YOU:
	case COG_CON_REAML_BREAK_FIGHT:
	case COG_CON_GUILD_FIGHT_RANK:
	case COG_CON_GUILD_HONOR_RANK:
	case COG_CON_ANECDOTE_TASK:
		need_progress = 1;
		break;
	case COG_CON_PET_PRACTICE_LEVEL:
		need_progress = cog_task->param1;
		break;
	default:
		need_progress = cog_task->param2;
		break;
	}
	UNSTD_STATIC_CHECK(77 == COG_CON_TYPE_MAX);

	return m_task_prog_list[seq] >= need_progress;
}

bool CourseOfGrowth::IsTaskProgressAddUp(int condition)
{
	bool ret = true;
	switch (condition)
	{
		case COG_CON_OBTAIN_TITLE_NUMBER:
		case COG_CON_JIE_YI_DAY_NUM:
		case COG_CON_CLOUD_ARENA_SCORE:
		case COG_CON_APPRENTICE_DAY_NUM:
		case COG_CON_HUAN_JIE_ZHAN_CHANG:
		ret = false;
		break;
		default:
		break;
	}
	UNSTD_STATIC_CHECK(77 == COG_CON_TYPE_MAX);
	return ret;
}

void CourseOfGrowth::SendSingleProg(int seq)
{
	if (seq < 0 || seq >= GROWTH_TASK_NUM_PER_STAR) return;

	Protocol::SCCOGSingleTaskProgs stp;
	stp.seq = seq;
	stp.prog_num = m_task_prog_list[seq];

	m_mgr->NetSend((const void *)&stp, sizeof(stp));
}

void CourseOfGrowth::SendSingleFlag(int seq)
{
	if (seq < 0 || seq >= GROWTH_TASK_NUM_PER_STAR) return;

	Protocol::SCCOGRewardFetchFlag rff;
	rff.seq = seq;
	rff.flag = m_param.award_fetch_flag.IsBitSet(seq) ? 1 : 0;

	m_mgr->NetSend((const void *)&rff, sizeof(rff));
}

void CourseOfGrowth::SendSingleNoti(int seq)
{
	if (m_param.finish_notify_flag.IsBitSet(seq)) return;

	Protocol::SCCOGSingleSeqNotify ssn;
	ssn.notify_seq = seq;

	m_mgr->NetSend((const void *)&ssn, sizeof(ssn));
	m_param.finish_notify_flag.SetBit(seq);
}

void CourseOfGrowth::SendCurStarAwardStatus()
{
	Protocol::SCCOGStarAwardStatus sas;
	sas.cur_reward_prog = m_param.cur_reward_prog;
	sas.cur_reward_star = m_param.cur_reward_star;

	m_mgr->NetSend((const void *)&sas, sizeof(sas));
}

void CourseOfGrowth::SendStarAwardMail(int prog, int star)
{
	const COGAwardCon * star_award = LOGIC_CONFIG->GetCourseOfGrowthConfig()->GetStarAward(prog, star);
	if (NULL == star_award)
	{
		return;
	}

	if (star_award->reward_num > 0)
	{
		if (m_mgr->GetKnapsack()->CheckForPutList(star_award->reward_num, star_award->reward_list))
		{
			if (star_award->reward_coin > 0) m_mgr->GetMoney()->AddCoinBind(star_award->reward_coin, __FUNCTION__, false, true);
			if (star_award->reward_exp > 0) m_mgr->GetRole()->AddExp(star_award->reward_exp, ADD_EXP_TYPE_ACHIEVEMENT_PROG, __FUNCTION__);
			if (star_award->reward_num > 0) m_mgr->GetKnapsack()->PutList(star_award->reward_num, star_award->reward_list, PUT_REASON_ACHIEVEMENT_PROG_REWARD);
		}
		else
		{
			// 背包跟临时背包满了发邮件
			static MailContentParam contentparam; contentparam.Reset();
			if (star_award->reward_coin > 0) contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] += star_award->reward_coin;
			if (star_award->reward_exp > 0) contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] += star_award->reward_exp;
			if (star_award->reward_num > 0)
			{
			int num = 0;
			for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM && i < MAX_COG_AWARD_NUM; ++i)
			{
			const ItemBase * item_base = ITEMPOOL->GetItem(star_award->reward_list[i].item_id);
			if (item_base == NULL)
			{
			continue;
			}

			contentparam.item_list[num].item_id = star_award->reward_list[i].item_id;
			contentparam.item_list[num].num = star_award->reward_list[i].num;
			contentparam.item_list[num].is_bind = star_award->reward_list[i].is_bind ? 1 : 0;
			++num;
			}
			}

			int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_knapsack_full_mail_subject);
			int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_knapsack_full_send_mail);
			if (lensub > 0 && length > 0)
			{
			MailRoute::MailToUser(m_mgr->GetRole()->GetUID(), MAIL_REASON_DEFAULT, contentparam);
			}
		}
	}
	else
	{
		if (star_award->reward_coin > 0) m_mgr->GetMoney()->AddCoinBind(star_award->reward_coin, __FUNCTION__, false, true);
		if (star_award->reward_exp > 0) m_mgr->GetRole()->AddExp(star_award->reward_exp, ADD_EXP_TYPE_ACHIEVEMENT_PROG, __FUNCTION__);
		if (star_award->reward_num > 0) m_mgr->GetKnapsack()->PutList(star_award->reward_num, star_award->reward_list, PUT_REASON_ACHIEVEMENT_PROG_REWARD);
	}

	// 发邮件
	/*static MailContentParam contentparam; contentparam.Reset();
	if (star_award->reward_coin > 0) contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] += star_award->reward_coin;
	if (star_award->reward_exp > 0) contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_EXP] += star_award->reward_exp;
	if (star_award->reward_num > 0)
	{
		int num = 0;
		for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM && i < MAX_COG_AWARD_NUM; ++i)
		{
			const ItemBase * item_base = ITEMPOOL->GetItem(star_award->reward_list[i].item_id);
			if (item_base == NULL)
			{
				continue;
			}

			contentparam.item_list[num].item_id = star_award->reward_list[i].item_id;
			contentparam.item_list[num].num = star_award->reward_list[i].num;
			contentparam.item_list[num].is_bind = star_award->reward_list[i].is_bind ? 1 : 0;
			++num;
		}
	}

	int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_course_of_growth_uplevel_subject);
	int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_course_of_growth_uplevel_content, m_param.cur_prog, m_param.cur_star);
	if (lensub > 0 && length > 0)
	{
		MailRoute::MailToUser(m_mgr->GetRole()->GetUID(), MAIL_REASON_DEFAULT, contentparam);
	}*/
}

bool CourseOfGrowth::IsCourseMeetTheCondition(int seq, int condition, int param1, int param2)
{
	if(this->IsCourseSeqInvaild(seq)) return false;

	UInt32 need_progress = 0;
	switch (condition)
	{
		case COG_CON_REACHS_APPOINTED_LV:
		case COG_CON_APPOINTED_TASK_DONE:
		case COG_CON_PET_INTENSIFY_LEVEL:
		case COG_CON_CRYSTAL_FB_XI_YOU:
		case COG_CON_REAML_BREAK_FIGHT:
		case COG_CON_GUILD_FIGHT_RANK:
		case COG_CON_GUILD_HONOR_RANK:
		case COG_CON_ANECDOTE_TASK:
		need_progress = 1;
		break;
		case COG_CON_PET_PRACTICE_LEVEL:
		need_progress = param1;
		break;
		default:
		need_progress = param2;
		break;
	}
	UNSTD_STATIC_CHECK(77 == COG_CON_TYPE_MAX);

	return m_course_prog_list[seq] >= need_progress;
}

void CourseOfGrowth::SendSingleCourseInfo(int seq)
{
	if(this->IsCourseSeqInvaild(seq)) return;

	Protocol::SCRoleCourseSingleInfo info;
	info.seq = seq;
	info.course_prog = m_course_prog_list[seq];
	
	m_mgr->NetSend(&info, sizeof(info));
}

void CourseOfGrowth::SendCourseNoticeInfo(int notice_type, int param1, int param2)
{
	if(notice_type < 0 || notice_type >= Protocol::SCRoleCourseNoticeInfo::ROLE_COURSE_NOTICE_TYPE_MAX) return;

	Protocol::SCRoleCourseNoticeInfo info;
	info.notice_type = notice_type;
	info.course_level = m_course_level;
	info.course_cur_fraction = m_course_cur_fraction;
	info.param1 = param1;
	info.param2 = param2;

	m_mgr->NetSend(&info, sizeof(info));
}

void CourseOfGrowth::OnRefreshCourseProgress(int condition, int param1, int param2)
{
	if (condition < 0 || condition >= COG_CON_TYPE_MAX) return;

	const std::vector<CourseEntryCfg> * course_cfg = LOGIC_CONFIG->GetCourseOfGrowthConfig()->GetCourseEntryCfg(condition);
	if (NULL == course_cfg) return;

	for (int i = 0; i < (int)course_cfg->size(); ++i)
	{
		UInt32 cur_value = 0;
		if (this->IsNeedSaveInValueList(condition))
		{
			long long course_key = 1LL * condition * COURSE_ENTRY_KEY_MULT + (*course_cfg)[i].param1;
			cur_value = m_course_prog_num_map[course_key];
		}
		else
		{
			long long course_key = 1LL * condition * COURSE_ENTRY_KEY_MULT + (*course_cfg)[i].param1;
			UInt32 & value = m_course_prog_num_map[course_key];
			this->GetNeedSaveValue(condition, (*course_cfg)[i].param1, (*course_cfg)[i].param2, value);
			cur_value = value;
		}

		const int & seq = (*course_cfg)[i].seq;
		if (this->IsCourseSeqInvaild(seq)) continue;

		m_course_prog_list[seq] = cur_value;
		if (m_course_param.finish_record_flag.IsBitSet(seq)) continue;

		this->SendSingleCourseInfo(seq);
		if (this->IsCourseMeetTheCondition(seq, condition, (*course_cfg)[i].param1, (*course_cfg)[i].param2))
		{
			m_course_param.course_fraction_count += (*course_cfg)[i].course_reward;
			m_course_param.finish_record_flag.SetBit(seq);
			LOGIC_CONFIG->GetCourseOfGrowthConfig()->CalcLevelByCourseFraction(m_course_param.course_fraction_count, &m_course_level, &m_course_cur_fraction);
			CourseOfGrowthManager::Instance().OnCourseFinishChange(m_mgr->GetRole(), seq, true);
			this->SendCourseNoticeInfo(Protocol::SCRoleCourseNoticeInfo::ROLE_COURSE_NOTICE_TYPE_FINISH_ENTRY, seq, 1);
		}
	}
}

bool CourseOfGrowth::GetNeedSaveValue(int condition, int param1, int param2, UInt32 & value)
{
	bool ret = true;
	UNSTD_STATIC_CHECK(77 == COG_CON_TYPE_MAX);
	switch (condition)
	{
		case COG_CON_REACHS_FB_TIER_NUMB:
		switch (param1)
		{
			case FBManager::FB_TYPE_COURAGE_CHALLENGE:
			value = m_mgr->GetCourageChallenge()->GetReachStarScoreCount();
			break;
			case FBManager::FB_TYPE_BRAVE_GROUND:
			value = m_mgr->GetCommonData().enter_brave_ground_times;
			break;
		}
		break;
		case COG_CON_SKILLS_REACHS_LEVEL:
		value = m_mgr->GetRoleSkill()->TheNumOfLevelSkill(param1);
		break;
		case COG_CON_PET_NUM_REACH_LEVEL:
		value = m_mgr->GetPet()->TheNumOfLevelPet(param1);
		break;
		case COG_CON_REACHS_APPOINTED_LV:
		value = m_mgr->GetRole()->GetLevel() >= param1 ? 1 : 0;
		break;
		case COG_CON_APPOINTED_TASK_DONE:
		value = m_mgr->GetTaskRecorder()->HaveFinished(param1) ? 1 : 0;
		break;
		case COG_CON_PARTNER_UP_LEVEL:
		value = m_mgr->GetPartner()->GetPartnerCurCount(param1);
		break;
		case COG_CON_PET_PRACTICE_LEVEL:
		value = m_mgr->GetTrain()->GetRolePetTrainSkillLevel();
		break;
		/*case COG_CON_BOUNTY_TASK_DONE:
		value = m_mgr->GetBountyTask()->GetTodayFinish();
		break;*/
		case COG_CON_MIYI_KILL_LAYER_NUM:
		value = m_mgr->GetMiYi()->GetCurLayerFinishNum(param1);
		break;
		case COG_CON_KILL_MONTER_GROUP:
		value = m_mgr->GetRoleFirstKill()->HasKillMonsterGroup(param1) ? 1 : 0;
		break;
		case COG_CON_PET_MONOPOLY_LEVEL:
		value = m_mgr->GetPetMonopoly()->GetCurLevel();
		break;
		case COG_CON_TRAIN_SKILL_LEVEL:
		value = m_mgr->GetTrain()->GetRoleTrainSkillLevelNum(param1);
		break;
		case COG_CON_LUN_HUI_WANG_CHUAN:
		value = m_mgr->GetLunHuiWangChuan()->GetPassTimes();
		break;
		case COG_CON_SHAN_HAI_XIANG_YAO:
		value = m_mgr->GetPassShanHaiTimes();
		break;
		case COG_CON_REALM_BREAK_LEVEL:
		value = m_mgr->GetRealmBreak()->GetCurLevel() >= param1 ? 1 : 0;
		break;
		case COG_CON_PET_INTENSIFY_LEVEL:
		if (0 != param1)
		{
			value = m_mgr->GetPet()->GetPetNumByQualityAndStr(param1, param2);
		}
		else
		{
			value = m_mgr->GetPet()->GetPetIntensifyMax();
		}
		break;
		case COG_CON_CRYSTAL_FB_XI_YOU:
		value = m_mgr->GetRoleCrystalFb()->HasFristPassCrystalFb(param1) ? 1 : 0;
		break;			
		case COG_CON_REAML_BREAK_FIGHT:
		value = m_mgr->GetRealmBreak()->IsPassFightLevel(param1) ? 1 : 0;
		break;
		case COG_CON_REACH_CAPABILITY:
		if (COG_CON_REACH_CAPABILITY_TYPE_COUNT == param1)
		{
			value = m_mgr->GetRole()->GetCapabilityWithFightPetAndPartner();
		}
		else
		{
			value = m_mgr->GetCapability()->GetRoleCapability();
		}
		break;
		case COG_CON_HAS_FA_BAO_COUNT:
		value = m_mgr->GetFaBao()->GetFaBaoNumByColorAndLevel(param1, 1);
		break;
		case COG_CON_HAS_ACTIVE_FACE:
		value = m_mgr->GetFaceScore()->GetActiveCount(param1);
		break;
		case COG_CON_HAS_SURFACE:
		value = m_mgr->GetSurface()->GetActiveCount(param1);
		break;
		case COG_CON_GUILD_FIGHT_RANK:
		value = m_mgr->IsMeetGuildFightRank(param1) ? 1 : 0;
		break;
		case COG_CON_GUILD_HONOR_RANK:
		value = m_mgr->IsMeetGuildHonorRank(param1) ? 1 : 0;
		break;
		case COG_CON_FETCH_GUILD_HONGBAO:
		value = m_mgr->GetCommonData().guild_hong_bao_fetch_count;
		break;
		case COG_CON_GUILD_ANSWER:
		value = m_mgr->GetCommonData().guild_answer_correct_count;
		break;
		case COG_CON_CHANNEL_CHAT:
		value = m_mgr->GetChannelChatByType(param1);
		break;
		case COG_CON_HAS_FRIEND_COUNT:
		value = FriendManager::Instance().GetFriendNumber(m_mgr->GetRole()->GetUID(), param1); 
		break;
		case COG_CON_APPRENTICE_COUNT:
		value = m_mgr->GetShiTu()->GetApprenticeCount();
		break;
		case COG_CON_ANECDOTE_TASK:
		value = m_mgr->GetAnecdoteTask()->IsFinishTask(param1);
		break;
		case COG_CON_CHIVALROUS:
		value = m_mgr->GetSalary()->GetHistoryChivalrous();
		default:
		ret = false;
		break;
	}

	return ret;
}

void CourseOfGrowth::OnCourseInit()
{
	const std::map<int, CourseEntryCfg> & course_entry_map = LOGIC_CONFIG->GetCourseOfGrowthConfig()->GetCourseEntryCfg();
	for (std::map<int, CourseEntryCfg>::const_iterator it = course_entry_map.begin(); it != course_entry_map.end(); ++it)
	{
		if(this->IsCourseSeqInvaild(it->second.seq)) continue;

		if (it->second.version != (int)m_course_param.course_entry_version_list[it->second.seq])		//版号不同则该条目记录移除
		{
			m_course_param.course_entry_version_list[it->second.seq] = it->second.version;
			if (m_course_param.finish_record_flag.IsBitSet(it->second.seq))
			{
				m_course_param.finish_record_flag.UnSetBit(it->second.seq);
				CourseOfGrowthManager::Instance().OnCourseFinishChange(m_mgr->GetRole(), it->second.seq, false);
			}
		}

		long long course_key = 1LL * it->second.condition_type * COURSE_ENTRY_KEY_MULT + it->second.param1;
		UInt32 & value = m_course_prog_num_map[course_key];
		if (this->IsNeedSaveInValueList(it->second.condition_type))
		{
			UInt32 key = it->second.condition_type * GROWTH_KEY_MULT + it->second.param1;
			std::map<UInt32, UInt32>::const_iterator prog_it = m_prog_num_map.find(key);
			if (prog_it == m_prog_num_map.end())
			{
				continue;
			}
			value = prog_it->second;
		}
		else
		{
			this->GetNeedSaveValue(it->second.condition_type, it->second.param1, it->second.param2, value);
		}
		m_course_prog_list[it->second.seq] = value;		
		if (!m_course_param.finish_record_flag.IsBitSet(it->second.seq) && this->IsCourseMeetTheCondition(it->second.seq, it->second.condition_type, it->second.param1, it->second.param2))
		{
			m_course_param.course_fraction_count += it->second.course_reward;
			m_course_param.finish_record_flag.SetBit(it->second.seq);
			CourseOfGrowthManager::Instance().OnCourseFinishChange(m_mgr->GetRole(), it->second.seq, true);
		}
	}

	const std::map<int, CourseLevelRewardCfg> & course_level_reward_cfg = LOGIC_CONFIG->GetCourseOfGrowthConfig()->GetCourseLevelRewardCfg();
	for (std::map<int, CourseLevelRewardCfg>::const_iterator level_it = course_level_reward_cfg.begin(); level_it != course_level_reward_cfg.end(); ++level_it)
	{
		int level_seq = level_it->first;
		if(this->IsCourseLevelSeqInvaild(level_seq)) continue;

		if (level_it->second.version != (int)m_course_param.course_level_version_list[level_seq])		//版号不同则该等级奖励记录移除
		{
			m_course_param.course_level_version_list[level_seq] = level_it->second.version;
			m_course_param.level_reward_fetch_flag.UnSetBit(level_seq);
		}
	}
	LOGIC_CONFIG->GetCourseOfGrowthConfig()->CalcLevelByCourseFraction(m_course_param.course_fraction_count, &m_course_level, &m_course_cur_fraction);

	this->SendCourseAllInfo();
}

void CourseOfGrowth::OnGiveOldUserBaseInfo()
{
	if (0 == m_param.is_give_base_pass_times_flag)		//给老玩家发放一些基础
	{
		m_param.is_give_base_pass_times_flag++;
		if (m_mgr->GetCommonData().role_create_count_day > 1)	
		{
			UInt32 key = COG_CON_PASS_CRYSTAL_XI_YOU * GROWTH_KEY_MULT;		//该类型参数1为0
			m_prog_num_map[key] += m_mgr->GetCommonData().role_create_count_day * LOGIC_CONFIG->GetRoleCrystalFbConfig()->GetCrystalFbOtherCfg().free_reward_time;

			this->RefreshAllTaskProgress(false, false, COG_CON_PASS_CRYSTAL_XI_YOU);
		}
	}
	if (1 == m_param.is_give_base_pass_times_flag)	//给老玩家发放一些基础数值
	{
		m_param.is_give_base_pass_times_flag++;
		if (m_mgr->GetCommonData().role_create_count_day > 1)	//新玩家不发放
		{
			for (int i = PET_QUALITY_INVALID; i < PET_QUALITY_MAX; i++)
			{
				UInt32 pet_key = COG_CON_GET_QUALITY_PET * GROWTH_KEY_MULT + i;
				if (PET_QUALITY_INVALID == i)
				{
					m_prog_num_map[pet_key] += m_mgr->GetPet()->GetPetAndStorageCount();
				}
				else
				{
					m_prog_num_map[pet_key] += m_mgr->GetPet()->GetPetNumByQualityAndStr(i);
				}
			}
			this->RefreshAllTaskProgress(false, false, COG_CON_GET_QUALITY_PET);
		}
	}
	if (2 == m_param.is_give_base_pass_times_flag)	
	{
		m_param.is_give_base_pass_times_flag++;
		if (m_mgr->GetCommonData().role_create_count_day > 1)	//新玩家不发放
		{
			for (int i = MONEY_TYPE_GOLD; i < MONEY_TYPE_MAX; i++)
			{
				UInt32 money_key = COG_CON_GET_MONEY * GROWTH_KEY_MULT + i;
				m_prog_num_map[money_key] += m_mgr->GetMoney()->GetMoneyByType(i);
			}
			if (NULL != m_mgr->GetRole()->GetGuild())
			{
				UInt32 guild_key = COG_CON_FIRST_ADD_GUILD * GROWTH_KEY_MULT;
				m_prog_num_map[guild_key] += 1;
			}
			this->RefreshAllTaskProgress(false, false, COG_CON_GET_MONEY);
			this->RefreshAllTaskProgress(false, false, COG_CON_FIRST_ADD_GUILD);
		}
	}
	if (3 == m_param.is_give_base_pass_times_flag)
	{
		m_param.is_give_base_pass_times_flag++;
		if (m_mgr->GetCommonData().role_create_count_day > 1)	//新玩家不发放
		{
			UInt32 yin_yan_eye_key = COG_CON_YIN_YANG_EYE_REWARD * GROWTH_KEY_MULT;		//该类型参数1为0
			m_prog_num_map[yin_yan_eye_key] += m_mgr->GetYinYangEye()->GetFinishRewardCount();
			this->RefreshAllTaskProgress(false, false, COG_CON_YIN_YANG_EYE_REWARD);
		}
	}
}
