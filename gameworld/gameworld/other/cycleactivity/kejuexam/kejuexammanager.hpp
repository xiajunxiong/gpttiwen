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

	// ����npc
	void CreateNPCQuestion(Role* role);
	// ��ʼ����
	void StartAnswering(Role* role);
	// ����
	void Answer(RoleModuleManager * m_module_mgr, int question_id, int answer_id);

	//ͬ����Ƭʹ��
 	void SynCardUse(Role * role);	
	// ���⿨ͬ��������Ŀ
	void SynQuestion(Role* role, int question_id);
	// ͬ����ʱʱ��
	void SynTimeOut(Role* role, unsigned int timeout_timestamp);
	// ͬ���ų��Ĵ����
 	void SynErrorAnswer(Role* role, int error_answer);
	// �����������������һ�������
	void ClearMemberPastPraiseInfo(Role* role);

	
	// ��װ���޵���Ϣ��ȫ�����
	void SendPraiseToTeamMember(Role* role);
	// ��װ���޵���Ϣ���ض����(ֻ����Ӧ���˷�)/ ���������߼�
	void SendPraiseToSpecificMember(Role* role, int specific_uid, int word_id);
	// ��װ��������Ϣ�������
	void SendLastInfo(Role* role);
	// ����Э�������ȫ�����
	void SendInfoToTeamMember(Role * role, const char* info, int len);
private:

};


#endif