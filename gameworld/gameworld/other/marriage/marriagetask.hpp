#ifndef __MARRIAGE_TASK_HPP__
#define __MARRIAGE_TASK_HPP__

#include "servercommon/marrydef.hpp"
#include <map>
#include <set>

class Role;
class RoleModuleManager;

struct MarriageTaskQuestionInfo
{
	MarriageTaskQuestionInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		question_id = 0;
		check_end_time = 0;
		next_end_time = 0;
		question_num = 0;

		role_id1 = 0;
		answer1 = 0;

		role_id2 = 0;
		answer2 = 0;

		already_have_question.clear();
	}

	int question_id;
	time_t check_end_time;
	time_t next_end_time;
	int question_num;

	int role_id1;			//答题人的role_id
	int answer1;			

	int role_id2;			//答题人的role_id
	int answer2;

	std::map<int, bool> answer_record;		//每道题目回答记录，答完后根据这个发奖励
	std::set<int> already_have_question;		//已经出现的题目，防止重复抽取
};

struct MarriageTaskPuzzleInfo
{
	MarriageTaskPuzzleInfo()
	{

	}

	time_t end_time;
	std::set<int> complete_role;
};

class MarriageTask
{
public:
	static MarriageTask & Instance();
	
	void OnUserLogin(Role *role);
	void OnUserLogout(Role *role);

	void Update(unsigned long interval, time_t now_second);

	void ReqStartTask(Role * role, int param1);
	void ReqAutoFb(Role * role);

	void AnswerDuetQuestion(Role * role, int param1);
	void NextDuetQuestion(Role * role);
	void AnswerSingleQuestion(Role * role, int param1);
	void NextSingleQuestion(Role * role);

	void SendRoute(Role * role,int p1,int p2 ,int p3);

	void OnDuetPuzzleReq(Role * role, int is_complete);
private:
	MarriageTask();
	~MarriageTask();

	bool CheckDuetQuestion(std::set<int> role_ids, MarriageTaskQuestionInfo & info);
	void RefreshDuetQuestion(const std::set<int> & s, bool reset_question_num = false);
	
	bool CheckSingleQuestion(int role_id,MarriageTaskQuestionInfo & info);
	void RefreshSingleQuestion(int role_id, bool reset_question_num = false);

	void SendQuestionInfo(Role * role, const MarriageTaskQuestionInfo & question_info,int question_type);
	void SendQuestionResult(Role * role, int question_id, bool is_right, bool is_duet);
	void GiveTaskReward(Role * role, int param1);

	void SendDuetPuzzleInfo(Role * role,int info_type,int p1);
	void SendRoleLogoutNotice(Role * role);			//双人游戏中，其中一方掉线通知

	std::map<std::set<int>, MarriageTaskQuestionInfo> m_duet_question_info;
	std::map<int, MarriageTaskQuestionInfo> m_single_question_info;

	std::map<std::set<int>, MarriageTaskPuzzleInfo> m_duet_puzzle_info;

};


#endif // !__MARRIAGE_HPP__
