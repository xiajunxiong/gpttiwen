#ifndef __KE_JU_EXAM_MANAGER__
#define __KE_JU_EXAM_MANAGER__

#include "obj/character/role.h"

class Role;
class RoleModuleManager;
class KeJuExamManager
{
public:
	KeJuExamManager();
	~KeJuExamManager();
	static KeJuExamManager & Instance();

	// 创建npc
	void CreateNPCQuestion(Role* role);
	// 开始答题
	void StartAnswering(Role* role);
	// 答题
	void Answer(RoleModuleManager * m_module_mgr, int question_id, int answer_id);

	//同步卡片使用
 	void SynCardUse(Role * role);	
	// 换题卡同步队友题目
	void SynQuestion(Role* role, int question_id);
	// 同步超时时间
	void SynTimeOut(Role* role, unsigned int timeout_timestamp);
	// 同步排除的错误答案
 	void SynErrorAnswer(Role* role, int error_answer);
	// 清除队伍内所有人上一题的数据
	void ClearMemberPastPraiseInfo(Role* role);

	
	// 组装点赞等信息给全队玩家
	void SendPraiseToTeamMember(Role* role);
	// 组装点赞等信息给特定玩家(只给对应的人发)/ 点赞特殊逻辑
	void SendPraiseToSpecificMember(Role* role, int specific_uid, int word_id);
	// 组装最后组队信息发给玩家
	void SendLastInfo(Role* role);
	// 发送协议给队伍全部玩家
	void SendInfoToTeamMember(Role * role, const char* info, int len);
private:

};


#endif