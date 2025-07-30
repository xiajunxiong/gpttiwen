#ifndef __CYCLE_ACTIVITY_LANTERN_RIDDLE__
#define __CYCLE_ACTIVITY_LANTERN_RIDDLE__

#include "obj/character/role.h"

class Role;
class RoleModuleManager;
class LanernRiddieManager
{
public:
	LanernRiddieManager();
	~LanernRiddieManager();
	static LanernRiddieManager & Instance();

	void CreateNPCQuestion(Role* role);
	void StartAnswering(Role* role);
	void Answer(RoleModuleManager * m_module_mgr, int question_id, int answer_id);


	void SynCardUse(Role * role);				//同步卡片使用
	void SynQuestion(Role* role,int question_id);			// 换题卡同步队友题目
	void SynTimeOut(Role* role,unsigned int timeout_timestamp);
	void SynErrorAnswer(Role* role, int error_answer);		// 排错卡同步队友错误答案
	void ClearMemberPastPraiseInfo(Role* role);

	void SendInfoToTeamMember(Role * role, const char* info, int len);
	void SendPraiseToTeamMember(Role* role);				// 组装点赞等信息给全队玩家
	void SendLastInfo(Role* role);		// 组装最后组队信息发给玩家
private:

};



#endif