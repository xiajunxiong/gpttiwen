#ifndef __SPECIAL_LOGIC_QINGYUAN_DUIDUIPENG_HPP__
#define __SPECIAL_LOGIC_QINGYUAN_DUIDUIPENG_HPP__

#include "scene/speciallogic.hpp"
#include <map>


class SpecialLogicQingYuanDuiDuiPeng : public SpecialLogic
{
public:
	SpecialLogicQingYuanDuiDuiPeng(Scene * scene);
	virtual ~SpecialLogicQingYuanDuiDuiPeng();

	virtual void Update(unsigned long interval, time_t now_second);

	virtual void OnRoleEnterScene(Role * role);	//!< ���ǽ��볡��
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);

	virtual bool CanDestroy() { return true; }

	//virtual void OnNPCChosedReward(Role *role, ObjID npc_objid, int option_index);

	void OnClientReq(Role * role, void * msg);

private:
	void OnStartGame(Role * role);
	void CompareForm();							// �Ƚ�˫���û�����
	void OnFinish(bool is_finish);				// ��������������
	void OnNpcForm(Role * role, ObjID npc_objid);

	void SendGameStart();
	void SendRoleForm();
	void SendFbInfo(Role * role = NULL);

	struct RoleData
	{
		void ResetForm()
		{
			seq = 0;
			npc_obj_id = INVALID_OBJ_ID;
			npc_id = 0;			
		}

		RoleData() : user_id(INVALID_USER_ID)/*, start_game(false)*/
		{
			this->ResetForm();
		}

		UserID user_id;
		//bool start_game;			// �Ƿ�ʼ��Ϸ

		char seq;
		ObjID npc_obj_id;			// �û���npc
		int npc_id;	
	};

	bool m_is_init;

	unsigned int begin_game_times;					// ��ʼʱ���
	unsigned int end_game_times;					// ����ʱ���
	unsigned int role_leave_times;					// ����������ɫ�뿪ʱ���

	struct NpcData
	{
		NpcData() {}
		NpcData(int _seq, int _npc_id): seq(_seq), npc_id(_npc_id) {}

		int seq;
		int npc_id;
	};

	std::map<ObjID, NpcData> m_npc_list;				// <npc, npc_data>

	RoleData role_list[2];

	int integral;				// ����
	int continuity;				// ��������
	int succ_match;				// �ɹ�ƥ�����

	int reward_level;							// ��������
};

#endif