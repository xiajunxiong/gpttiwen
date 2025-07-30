#include "marriage.hpp"
#include "marriagetask.hpp"
#include "obj/character/role.h"
#include "other/rolemodulemanager.hpp"
#include "servercommon/errornum.h"
#include "config/logicconfigmanager.hpp"
#include "other/marriage/marriageconfig.hpp"
#include "item/money.h"
#include "world.h"
#include "protocol/msgmarry.hpp"
#include "item/knapsack.h"
#include "battle/battle_manager_local.hpp"

MarriageTask & MarriageTask::Instance()
{
	static MarriageTask instance;
	return instance;
}

void MarriageTask::OnUserLogin(Role * role)
{
}

void MarriageTask::OnUserLogout(Role * role)
{
	for (std::map<std::set<int>, MarriageTaskPuzzleInfo>::iterator iter = m_duet_puzzle_info.begin(); iter != m_duet_puzzle_info.end(); )
	{
		if (iter->first.count(role->GetUID()) != 0)
		{
			for (std::set<int> ::iterator role_iter = iter->first.begin(); role_iter != iter->first.end(); ++role_iter)
			{
				Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(*role_iter);
				if (role != NULL)
				{
					this->SendRoleLogoutNotice(role);
				}
			}

			m_duet_puzzle_info.erase(iter++);
			break;
		}
		else
		{
			++iter;
		}
	}
	

	for (std::map<std::set<int>, MarriageTaskQuestionInfo>::iterator iter = m_duet_question_info.begin(); iter != m_duet_question_info.end(); )
	{
		if (iter->first.count(role->GetUID()) != 0)
		{
			for (std::set<int> ::iterator role_iter = iter->first.begin(); role_iter != iter->first.end(); ++role_iter)
			{
				Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(*role_iter);
				if (role != NULL)
				{
					this->SendRoleLogoutNotice(role);
				}
			}

			m_duet_question_info.erase(iter++);
			break;
		}
		else
		{
			++iter;
		}
	}

	m_single_question_info.erase(role->GetUID());
}

void MarriageTask::Update(unsigned long interval, time_t now_second)
{
	for (std::map<std::set<int>, MarriageTaskQuestionInfo>::iterator iter = m_duet_question_info.begin(); iter != m_duet_question_info.end(); )
	{
		if (!this->CheckDuetQuestion(iter->first, iter->second))
		{
			++iter;
			continue;
		}

		if (iter->second.question_num >= 3)
		{
			for (std::set<int>::iterator role_iter = iter->first.begin(); role_iter != iter->first.end(); ++role_iter)
			{
				Role * role1 = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(*role_iter));
				if (role1 != NULL)
				{
					for (std::map<int, bool>::iterator answer_record_iter = iter->second.answer_record.begin(); answer_record_iter != iter->second.answer_record.end(); ++answer_record_iter)
					{
						const MarriageTaskQuestionCfg *  cfg = LOGIC_CONFIG->GetMarriageConfig()->GetTaskDuetQuestionCfg(answer_record_iter->first);
						if (cfg != NULL)
						{
							answer_record_iter->second ? role1->GetRoleModuleManager()->GetMoney()->AddCoinBind(cfg->coin, __FUNCTION__) : role1->GetRoleModuleManager()->GetMoney()->AddCoinBind(cfg->coin / 2, __FUNCTION__);
						}
					}

					this->GiveTaskReward(role1, MARRIAGE_TASK_2);
					role1->GetRoleModuleManager()->GetMarriage()->SetPlayTimes(MARRIAGE_TASK_2);
				}
			}

			m_duet_question_info.erase(iter++);
			continue;
		}
		else if (now_second >= iter->second.next_end_time)
		{
			this->RefreshDuetQuestion(iter->first);
		}

		++iter;
	}

	for (std::map<int, MarriageTaskQuestionInfo>::iterator iter = m_single_question_info.begin(); iter != m_single_question_info.end(); )
	{
		if (!this->CheckSingleQuestion(iter->first, iter->second))
		{
			++iter;
			continue;
		}

		if (iter->second.question_num >= 5)
		{
			Role * role1 = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(iter->first));
			if (role1 != NULL)
			{
				for (std::map<int, bool>::iterator answer_record_iter = iter->second.answer_record.begin(); answer_record_iter != iter->second.answer_record.end(); ++answer_record_iter)
				{
					const MarriageTaskQuestionCfg *  cfg = LOGIC_CONFIG->GetMarriageConfig()->GetTaskSingleQuestionCfg(answer_record_iter->first);
					if (cfg != NULL)
					{
						answer_record_iter->second ? role1->GetRoleModuleManager()->GetMoney()->AddCoinBind(cfg->coin, __FUNCTION__) : role1->GetRoleModuleManager()->GetMoney()->AddCoinBind(cfg->coin / 2, __FUNCTION__);
					}					
				}

				this->GiveTaskReward(role1, MARRIAGE_TASK_5);
				role1->GetRoleModuleManager()->GetMarriage()->SetPlayTimes(MARRIAGE_TASK_5);
			}
			
			m_single_question_info.erase(iter++);
			continue;
		}
		else if (now_second >= iter->second.next_end_time)
		{
			this->RefreshSingleQuestion(iter->first);
		}

		++iter;
	}

	for (std::map<std::set<int>, MarriageTaskPuzzleInfo>::iterator iter = m_duet_puzzle_info.begin(); iter != m_duet_puzzle_info.end(); )
	{
		if (iter->second.complete_role.size() >= 2)
		{
			for (std::set<int> ::iterator role_iter = iter->first .begin(); role_iter != iter->first.end(); ++role_iter)
			{
				Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(*role_iter);
				if (role!=NULL)
				{
					this->GiveTaskReward(role, MARRIAGE_TASK_1);
					role->GetRoleModuleManager()->GetMarriage()->SetPlayTimes(MARRIAGE_TASK_1);
					this->SendDuetPuzzleInfo(role, Protocol::SCMarriageDuetPuzzleInfo::DUET_PUZZLE_END, 1);
				}
			}

			m_duet_puzzle_info.erase(iter++);
			continue;
		}

		if (now_second >= iter->second.end_time)
		{
			for (std::set<int> ::iterator role_iter = iter->first.begin(); role_iter != iter->first.end(); ++role_iter)
			{
				Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(*role_iter);
				if (role != NULL)
				{
					this->SendDuetPuzzleInfo(role, Protocol::SCMarriageDuetPuzzleInfo::DUET_PUZZLE_END, 0);
				}
			}

			m_duet_puzzle_info.erase(iter++);
			continue;
		}

		++iter;
	}
}

void MarriageTask::ReqStartTask(Role * role, int param1)
{	
	if (role == NULL)
	{
		return;
	}

	switch (param1)
	{
		case MARRIAGE_TASK_1:
			{
				Role * lover = role->GetRoleModuleManager()->GetMarriage()->GetLover();
				if (lover == NULL)
				{
					role->NoticeNum(errornum::EN_ROLE_NOT_ONLINE);
					return;
				}

				if (!role->GetRoleModuleManager()->GetMarriage()->IsOnlyLoverInTeam())
				{
					role->NoticeNum(errornum::EN_NOT_ONLY_LOVER_IN_TEAM);
					return;
				}

				if (!role->GetRoleModuleManager()->GetMarriage()->CanStart(param1))
				{
					role->NoticeNum(errornum::EN_CAN_NOT_START_TASK);
					lover->NoticeNum(errornum::EN_LOVER_CAN_NOT_START_TASK);
					return;
				}

				if (!lover->GetRoleModuleManager()->GetMarriage()->CanStart(param1))
				{
					lover->NoticeNum(errornum::EN_CAN_NOT_START_TASK);
					role->NoticeNum(errornum::EN_LOVER_CAN_NOT_START_TASK);
					return;
				}

				std::set<int> s;
				s.insert(role->GetUID());
				s.insert(lover->GetUID());

				MarriageTaskPuzzleInfo puzzle_info;
				puzzle_info.end_time = EngineAdapter::Instance().Time() + 120;

				m_duet_puzzle_info[s] = puzzle_info;

				this->SendDuetPuzzleInfo(role, Protocol::SCMarriageDuetPuzzleInfo::DUET_PUZZLE_START, (unsigned int)puzzle_info.end_time);
				this->SendDuetPuzzleInfo(lover, Protocol::SCMarriageDuetPuzzleInfo::DUET_PUZZLE_START, (unsigned int)puzzle_info.end_time);
			}
			break;
		case MARRIAGE_TASK_2:
			{
				Role * lover = role->GetRoleModuleManager()->GetMarriage()->GetLover();
				if (lover == NULL)
				{
					role->NoticeNum(errornum::EN_ROLE_NOT_ONLINE);
					return;
				}

				if (!role->GetRoleModuleManager()->GetMarriage()->IsOnlyLoverInTeam())
				{
					role->NoticeNum(errornum::EN_NOT_ONLY_LOVER_IN_TEAM);
					return;
				}

				if (!role->GetRoleModuleManager()->GetMarriage()->CanStart(param1))
				{
					role->NoticeNum(errornum::EN_CAN_NOT_START_TASK);
					lover->NoticeNum(errornum::EN_LOVER_CAN_NOT_START_TASK);
					return;
				}

				if (!lover->GetRoleModuleManager()->GetMarriage()->CanStart(param1))
				{
					lover->NoticeNum(errornum::EN_CAN_NOT_START_TASK);
					role->NoticeNum(errornum::EN_LOVER_CAN_NOT_START_TASK);
					return;
				}

				std::set<int> s;
				s.insert(role->GetUID());
				s.insert(lover->GetUID());
				this->RefreshDuetQuestion(s, true);
			}
			break;

		case MARRIAGE_TASK_3:
			{
				Role * lover = role->GetRoleModuleManager()->GetMarriage()->GetLover();
				if (lover == NULL)
				{
					role->NoticeNum(errornum::EN_ROLE_NOT_ONLINE);
					return;
				}

				if (!role->GetRoleModuleManager()->GetMarriage()->IsOnlyLoverInTeam())
				{
					role->NoticeNum(errornum::EN_NOT_ONLY_LOVER_IN_TEAM);
					return;
				}

				if (!role->GetRoleModuleManager()->GetMarriage()->CanStart(param1))
				{
					role->NoticeNum(errornum::EN_CAN_NOT_START_TASK);
					lover->NoticeNum(errornum::EN_LOVER_CAN_NOT_START_TASK);
					return;
				}

				if (!lover->GetRoleModuleManager()->GetMarriage()->CanStart(param1))
				{
					lover->NoticeNum(errornum::EN_CAN_NOT_START_TASK);
					role->NoticeNum(errornum::EN_LOVER_CAN_NOT_START_TASK);
					return;
				}

				MarriageParam & param = role->GetRoleModuleManager()->GetMarriage()->GetParam();
				const MarriageTaskFbCfg * fb_cfg = LOGIC_CONFIG->GetMarriageConfig()->GetMarriageTaskFbCfg(param.curr_wave);
				if (fb_cfg == NULL)
				{
					return;
				}

				BattleManagerLocal::GetInstance().StartBattleModeFightReq(role, BATTLE_MODE_MARRIAGE_FB, fb_cfg->monster_id, 0, 0, true);
			}
			break;

		case MARRIAGE_TASK_4:
			{
				if (!role->GetRoleModuleManager()->GetMarriage()->CanStart(param1))
				{
					return;
				}

				this->GiveTaskReward(role, param1);

				role->GetRoleModuleManager()->GetMarriage()->SetPlayTimes(param1);
			}
			break;

		case MARRIAGE_TASK_5:
			{
				if (!role->GetRoleModuleManager()->GetMarriage()->CanStart(param1))
				{
					role->NoticeNum(errornum::EN_CAN_NOT_START_TASK);
					return;
				}

				this->RefreshSingleQuestion(role->GetUID(), true);
			}
			break;
		default:
			break;
	}
}

void MarriageTask::ReqAutoFb(Role * role)
{
	Role * lover = role->GetRoleModuleManager()->GetMarriage()->GetLover();
	if (lover == NULL)
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_ONLINE);
		return;
	}

	if (!role->GetRoleModuleManager()->GetMarriage()->IsOnlyLoverInTeam())
	{
		role->NoticeNum(errornum::EN_NOT_ONLY_LOVER_IN_TEAM);
		return;
	}

	if (!role->GetRoleModuleManager()->GetMarriage()->CanAutoFb() || !lover->GetRoleModuleManager()->GetMarriage()->CanAutoFb())
	{
		role->NoticeNum(errornum::EN_CAN_NOT_AUTO_FB);
		lover->NoticeNum(errornum::EN_CAN_NOT_AUTO_FB);
		return;
	}

	MarriageParam & lover_param = lover->GetRoleModuleManager()->GetMarriage()->GetParam();
	MarriageParam & param = role->GetRoleModuleManager()->GetMarriage()->GetParam();
	int auto_wave = param.max_pass_fb_wave % 10 * 10;

	role->GetRoleModuleManager()->GetMarriage()->SendMarriageFbReward(auto_wave - param.curr_wave, true);
	lover->GetRoleModuleManager()->GetMarriage()->SendMarriageFbReward(auto_wave - param.curr_wave, true);

	lover_param.curr_wave = auto_wave;
	lover_param.is_auto_fb = 1;

	param.curr_wave = auto_wave;
	param.is_auto_fb = 1;

	role->GetRoleModuleManager()->GetMarriage()->SendInfo();
	lover->GetRoleModuleManager()->GetMarriage()->SendInfo();
}

void MarriageTask::AnswerDuetQuestion(Role * role, int param1)
{
	if (role == NULL)
	{
		return;
	}

	std::set<int> s;
	s.insert(role->GetUID());
	s.insert(role->GetRoleModuleManager()->GetMarriage()->GetLoverUid());

	std::map<std::set<int>, MarriageTaskQuestionInfo>::iterator iter = m_duet_question_info.find(s);

	if (iter == m_duet_question_info.end())
	{
		return;
	}

	MarriageTaskQuestionInfo & question_info = iter->second;

	if (question_info.role_id1 != role->GetUID() && question_info.role_id2 != role->GetUID())
	{
		if (question_info.role_id1 == 0)
		{
			question_info.role_id1 = role->GetUID();
			question_info.answer1 = param1;
		}
		else if (question_info.role_id2 == 0)
		{
			question_info.role_id2 = role->GetUID();
			question_info.answer2 = param1;
		}
	}
}

void MarriageTask::NextDuetQuestion(Role * role)
{
	for (std::map<std::set<int>, MarriageTaskQuestionInfo>::iterator iter = m_duet_question_info.begin(); iter != m_duet_question_info.end(); ++iter)
	{
		if (iter->first.count(role->GetUID()) != 0 && iter->second.next_end_time != 0)
		{
			iter->second.next_end_time = EngineAdapter::Instance().Time();
			break;
		}
	}
}

void MarriageTask::AnswerSingleQuestion(Role * role, int param1)
{
	if (role == NULL)
	{
		return;
	}

	std::map<int, MarriageTaskQuestionInfo>::iterator iter = m_single_question_info.find(role->GetUID());

	if (iter == m_single_question_info.end())
	{
		return;
	}

	MarriageTaskQuestionInfo & question_info = iter->second;

	if (question_info.role_id1 != role->GetUID() && question_info.role_id2 != role->GetUID())
	{
		if (question_info.role_id1 == 0)
		{
			question_info.role_id1 = role->GetUID();
			question_info.answer1 = param1;
		}
		else if (question_info.role_id2 == 0)
		{
			question_info.role_id2 = role->GetUID();
			question_info.answer2 = param1;
		}
	}
}

void MarriageTask::NextSingleQuestion(Role * role)
{
	std::map<int, MarriageTaskQuestionInfo>::iterator iter = m_single_question_info.find(role->GetUID());
	if (iter != m_single_question_info.end())
	{
		if (iter->second.next_end_time != 0)
		{
			iter->second.next_end_time = EngineAdapter::Instance().Time();
		}
		
	}
}

void MarriageTask::SendRoute(Role * role, int p1, int p2, int p3)
{
	if (role == NULL)
	{
		return;
	}

	Role * lover = role->GetRoleModuleManager()->GetMarriage()->GetLover();
	if (lover == NULL)
	{
		role->NoticeNum(errornum::EN_ROLE_NOT_ONLINE);
		return;
	}

	Protocol::SCMarriageTaskRoute protocol;
	protocol.role_id = role->GetUID();
	protocol.param1 = p1;
	protocol.param2 = p2;
	protocol.param3 = p3;
	EngineAdapter::Instance().NetSend(lover->GetNetId(), &protocol, sizeof(protocol));
	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
}

void MarriageTask::OnDuetPuzzleReq(Role * role, int is_complete)
{
	for (std::map<std::set<int>, MarriageTaskPuzzleInfo>::iterator iter = m_duet_puzzle_info.begin(); iter != m_duet_puzzle_info.end(); )
	{
		if (iter->first.count(role->GetUID()) != 0)
		{
			if (is_complete == 0)
			{
				for (std::set<int> ::iterator role_iter = iter->first.begin(); role_iter != iter->first.end(); ++role_iter)
				{
					Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(*role_iter);
					if (role != NULL)
					{
						this->SendDuetPuzzleInfo(role, Protocol::SCMarriageDuetPuzzleInfo::DUET_PUZZLE_CANCEL, 0);
					}
				}

				m_duet_puzzle_info.erase(iter++);
				break;
			}
			else
			{
				iter->second.complete_role.insert(role->GetUID());
			}
		}
		++iter;				
	}
}

MarriageTask::MarriageTask()
{
}

MarriageTask::~MarriageTask()
{
}

bool MarriageTask::CheckDuetQuestion(std::set<int> role_ids,  MarriageTaskQuestionInfo & info)
{
	time_t now = EngineAdapter::Instance().Time();

	if (info.next_end_time != 0)
	{
		return true;
	}

	if (now > info.check_end_time || (info.role_id1 != 0 && info.role_id2 != 0))
	{
		const MarriageTaskQuestionCfg *  cfg = LOGIC_CONFIG->GetMarriageConfig()->GetTaskDuetQuestionCfg(info.question_id);
		if (cfg != NULL)
		{
			bool is_same = (info.answer1 == info.answer2) && info.answer1 != 0 && info.answer2 != 0;
			info.answer_record[info.question_id] = is_same;
			info.next_end_time = now + 5;			//延迟几秒下发题目，客户端播放动画

			for (std::set<int> ::iterator iter = role_ids.begin(); iter != role_ids.end(); ++iter)
			{
				Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(*iter);
				if (role == NULL)
				{
					continue;
				}
				this->SendQuestionResult(role, info.question_id, is_same, true);
			}
		}

		return true;
	}

	return false;
}

void MarriageTask::RefreshDuetQuestion(const std::set<int> & s, bool reset_question_num)
{	
	MarriageTaskQuestionInfo & question_info = m_duet_question_info[s];
	if (reset_question_num)
	{
		question_info.Reset();
	}

	question_info.question_id = LOGIC_CONFIG->GetMarriageConfig()->GetTaskRandQuestion(QUESTION_TYPE_DUET, question_info.already_have_question);
	question_info.check_end_time = EngineAdapter::Instance().Time() + 30;
	question_info.next_end_time = 0;
	question_info.question_num += 1;

	question_info.role_id1 = 0;
	question_info.answer1 = 0;
	question_info.role_id2 = 0;
	question_info.answer2 = 0;
	
	if (question_info.question_id != 0)
	{
		question_info.already_have_question.insert(question_info.question_id);
	}
	

	for (std::set<int>::iterator iter = s.begin(); iter != s.end(); ++iter)
	{
		Role * role1 = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(*iter));
		if (role1 != NULL)
		{
			this->SendQuestionInfo(role1, question_info, QUESTION_TYPE_DUET);
		}
	}
}

bool MarriageTask::CheckSingleQuestion(int role_id, MarriageTaskQuestionInfo & info)
{
	const MarriageTaskQuestionCfg *  cfg = LOGIC_CONFIG->GetMarriageConfig()->GetTaskSingleQuestionCfg(info.question_id);

	time_t now = EngineAdapter::Instance().Time();

	if (info.next_end_time != 0)
	{
		return true;
	}

	if (now > info.check_end_time || info.role_id1 != 0)
	{
		if (cfg != NULL)
		{
			Role * role1 = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));

			bool is_right = info.answer1 == cfg->answer;

			info.answer_record[info.question_id] = is_right;
			info.next_end_time = now + 5;			//延迟几秒下发题目，客户端播放动画

			this->SendQuestionResult(role1, info.question_id, is_right, false);
		}

		return true;
	}

	return false;
}

void MarriageTask::RefreshSingleQuestion(int role_id, bool reset_question_num)
{
	MarriageTaskQuestionInfo & question_info = m_single_question_info[role_id];
	if (reset_question_num)
	{
		question_info.Reset();
	}

	question_info.question_id = LOGIC_CONFIG->GetMarriageConfig()->GetTaskRandQuestion(QUESTION_TYPE_SINGLE, question_info.already_have_question);
	question_info.check_end_time = EngineAdapter::Instance().Time() + 10;
	question_info.next_end_time = 0;
	question_info.question_num += 1;
	question_info.role_id1 = 0;
	question_info.answer1 = 0;
	question_info.role_id2 = 0;
	question_info.answer2 = 0;
	
	if (question_info.question_id != 0)
	{
		question_info.already_have_question.insert(question_info.question_id);
	}

	Role * role = World::GetInstWorld()->GetSceneManager()->GetRole(IntToUid(role_id));
	if (role != NULL)
	{
		this->SendQuestionInfo(role, question_info, QUESTION_TYPE_SINGLE);
	}
}

void MarriageTask::SendQuestionInfo(Role * role, const MarriageTaskQuestionInfo & question_info, int question_type)
{
	if (role == NULL)
	{
		return;
	}

	Protocol::SCMarriageTaskQuestionInfo protocol;
	protocol.question_id = (short)question_info.question_id;
	protocol.end_time = (unsigned int)question_info.check_end_time;
	protocol.question_num = (char)question_info.question_num;
	protocol.question_type = question_type;

	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
}

void MarriageTask::SendQuestionResult(Role * role, int question_id, bool is_right, bool is_duet)
{
	if (role == NULL)
	{
		return;
	}

	Protocol::SCMarriageTaskQuestionResult protocol;
	protocol.question_id = question_id;
	protocol.is_right = is_right;
	protocol.question_type = is_duet ? QUESTION_TYPE_DUET : QUESTION_TYPE_SINGLE;

	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
}

void MarriageTask::GiveTaskReward(Role * role, int param1)
{
	if (role == NULL)
	{
		return;
	}

	const MarriageTaskRewardCfg *  cfg = LOGIC_CONFIG->GetMarriageConfig()->GetMarriageTaskRewardCfg(param1);
	if (cfg == NULL)
	{
		return;
	}

	if (cfg->reward_list.size() > 0)
	{
		role->GetRoleModuleManager()->GetKnapsack()->PutList((short)cfg->reward_list.size(), &cfg->reward_list[0], PUT_REASON_MARRIAGE_TASK);
	}

}

void MarriageTask::SendDuetPuzzleInfo(Role * role, int info_type, int p1)
{
	if (role == NULL)
	{
		return;
	}

	Protocol::SCMarriageDuetPuzzleInfo protocol;
	protocol.info_type = info_type;
	protocol.param1 = p1;

	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
}

void MarriageTask::SendRoleLogoutNotice(Role * role)
{
	if (role == NULL)
	{
		return;
	}

	Protocol::SCMarriageDuetTaskLogoutNotice protocol;
	EngineAdapter::Instance().NetSend(role->GetNetId(), &protocol, sizeof(protocol));
}
