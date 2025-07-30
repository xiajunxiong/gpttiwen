#ifndef __SCHOOL_EXERCISE_HPP__
#define __SCHOOL_EXERCISE_HPP__

#include "servercommon/schoolexercisedef.hpp"

class RoleModuleManager;
class SchoolExercise
{
public:
	SchoolExercise();
	~SchoolExercise();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* mgr, const SchoolExerciseParam& param);
	void GetInitParam(ARG_OUT SchoolExerciseParam* param);

	void SendInfo();

	bool Challenge(int seq);
	void OnChallengeSucc(int seq);
	void FirstOpenUI();
private:
	void SendChallenSuccNotice(int seq, int has_reward);

	RoleModuleManager* m_mgr;
	SchoolExerciseParam m_param;
};

#endif