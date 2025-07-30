#ifndef __COURSE_OF_GROWTH_MANAGER_HPP__
#define __COURSE_OF_GROWTH_MANAGER_HPP__

#include "servercommon/courseofgrowthdef.h"
#include "servercommon/serverconfig/configstruct.h"
#include "servercommon/mysqlutility.hpp"

class Role;
class CourseOfGrowthManager
{
public:
	static CourseOfGrowthManager & Instance();

	bool LoadAllUser(TMysqlStatement &stMysqlStatement);

	void OnRoleLogin(Role * role);
	void OnCourseFinishChange(Role * role, int seq, bool is_add);
	void OnCourseFinishChangeFromHidden(int role_uid, int seq, bool is_add);

	void OnSendAllInfo(Role * role);
	void OnSendSingleInfo(Role * role, int seq);

	void OnSendAllInfoToHidden(int role_uid); 
	void OnSendSingleInfoToHidden(int role_uid, int seq);

private:
	CourseOfGrowthManager();
	~CourseOfGrowthManager();

	int m_course_finish_count_list[MAX_COURSE_ENTRY_LIMIT_NUM];		// 本服完成人数累计
};

#endif