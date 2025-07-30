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


	void SynCardUse(Role * role);				//ͬ����Ƭʹ��
	void SynQuestion(Role* role,int question_id);			// ���⿨ͬ��������Ŀ
	void SynTimeOut(Role* role,unsigned int timeout_timestamp);
	void SynErrorAnswer(Role* role, int error_answer);		// �Ŵ�ͬ�����Ѵ����
	void ClearMemberPastPraiseInfo(Role* role);

	void SendInfoToTeamMember(Role * role, const char* info, int len);
	void SendPraiseToTeamMember(Role* role);				// ��װ���޵���Ϣ��ȫ�����
	void SendLastInfo(Role* role);		// ��װ��������Ϣ�������
private:

};



#endif