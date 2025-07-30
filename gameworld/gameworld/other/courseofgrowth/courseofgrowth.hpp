#ifndef __COURSE_OF_GROWTH_HPP__
#define __COURSE_OF_GROWTH_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/courseofgrowthdef.h"

class RoleModuleManager;
class CourseOfGrowth		// 成长之路
{
public:
	CourseOfGrowth();
	~CourseOfGrowth();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager * role_module_manager, const CourseOfGrowthParam & param, int buff_len, const char *buff_cur, const RoleCourseParam & course_param);
	void GetCourseOfGrowthParam(CourseOfGrowthParam *param, char * buff_cur, int * buff_len, RoleCourseParam * course_param);
	void InitEnd();

	void OnLogin();
	void EventOperator(int condition, int param1, int param2);
	void GetTaskAward(int seq);					// 领取成长之路任务的奖励
	void BeginToFightCOGBoss();					// 开始打守星BOSS
	void OnDefeatCOGBoss();						// 击败守星BOSS后
	void OnFetchCourseLevelReward(int level);	// 领取历程等级奖励
	int GetCourseLevel() { return m_course_level; }	//获取历程等级
	int GetCourseFractionCount() { return m_course_param.course_fraction_count; }	//获取总历程点数
	bool IsReachCourseOfGrowth(int prog, int star);	//是否达到目标阶段星级

	void SendCOGAllInfo();				// 成就-目标
	void SendCourseAllInfo();			// 成就-历程

	void GmAddProgress(int seq, int prog);
	void GmSetProgress(int prog, int star);

private:
	void RefreshAllTaskProgress(bool is_init = false, bool is_floating_word = true, int condition = -1);
	bool IsNeedSaveInValueList(int condition);
	void GoToTheNextStarLevel();
	bool IsTaskMeetTheCondition(int seq);
	bool IsTaskProgressAddUp(int condition);
	void SendSingleProg(int seq);
	void SendSingleFlag(int seq);
	void SendSingleNoti(int seq);
	void SendCurStarAwardStatus();
	void SendStarAwardMail(int prog, int star);
	bool IsCourseSeqInvaild(int seq) { return seq < 0 || seq >= MAX_COURSE_ENTRY_LIMIT_NUM; }
	bool IsCourseLevelSeqInvaild(int level_seq) { return level_seq < 0 || level_seq >= MAX_COURSE_LEVEL_NUM; }
	bool IsCourseMeetTheCondition(int seq, int condition, int param1, int param2);
	void SendSingleCourseInfo(int seq);
	void SendCourseNoticeInfo(int notice_type, int param1, int param2);

	void OnRefreshCourseProgress(int condition, int param1, int param2);
	bool GetNeedSaveValue(int condition, int param1, int param2, UInt32 & value);
	void OnCourseInit();		//历程信息初始化
	void OnGiveOldUserBaseInfo();

	RoleModuleManager *		m_mgr;
	CourseOfGrowthParam		m_param;
	RoleCourseParam			m_course_param;

	UInt32 m_task_prog_list[GROWTH_TASK_NUM_PER_STAR];
	std::map<UInt32, UInt32> m_prog_num_map;		// 成就进度数值：键 - 条件ID * GROWTH_KEY_MULT + P1（子类型具备）

	int m_course_level;					// 当前历程等级
	int m_course_cur_fraction;			// 当前历程点
	UInt32 m_course_prog_list[MAX_COURSE_ENTRY_LIMIT_NUM];	
	std::map<long long, UInt32> m_course_prog_num_map;			// 历程进度数值 : 键 - 条件ID * COURSE_ENTRY_KEY_MULT + P1(子类型具备） 
};

#endif // __COURSE_OF_GROWTH_HPP__