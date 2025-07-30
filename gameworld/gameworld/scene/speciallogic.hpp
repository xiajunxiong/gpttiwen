#ifndef __SPECIAL_LOGIC_HPP__
#define __SPECIAL_LOGIC_HPP__

#include "servercommon/serverdef.h"
#include "changescenedelayer.h"
#include "servercommon/internalprotocal/battleprotocol.h"
#include "other/attributelist.hpp"
#include "servercommon/teamdef.hpp"
#include <set>

class Role;
class Scene;
class Obj;
class NPCGather;
class SpecialTransport;
class TreasureChest;
class Team;

class SpecialLogic
{
public:
	SpecialLogic(Scene *scene);
	virtual ~SpecialLogic() {};

	Scene* GetScene() const { return m_scene; }
	int GetSceneType();
	static SpecialLogic* CreateSpecialLogic(Scene* scene, int scene_type);

	virtual void Update(unsigned long interval, time_t now_second) {}

	virtual void OnAddObj(Obj *obj) {}								//!< �½�objʱ
	virtual void OnRemoveObj(Obj *obj) {}							//!< obj��ʧʱ

	virtual void OnRoleEnterScene(Role *role) {};					//!< ���ǽ��볡��
	virtual void OnRoleLeaveScene(Role *role, bool is_logout) {}	//!< �����뿪����
	
	virtual bool CanJoinOtherActivity() { return true; }			//!< �Ƿ�������������

	//�ɼ�,�����ֱ�Ӹ������put/putlist��reason��Ҫʹ��PUT_REASON_GATHER_OBJECT ������ݴ�,��Ӧ���ȵ��òɼ�����CheckIsGather�ӿ�,�ȴ����ɼ�����,�������ʱput/putlist��reason��Ҫʹ��ǰ��
	virtual void OnGather(Role *role, int gather_id, int seq_id, const Posi &pos) {}    
	virtual void OnStartGather(Role *role, int gather_id, int seq_id, const Posi &pos) {}
	virtual void OnAddExp(Role* role, int add_exp, int reason) {}
	virtual void OnFish(Role* role) {}

	virtual bool CanDestroy() = 0;									//!< �����Ƿ��ܱ��ݻ� ����ʵ�ֵ��߼�

	virtual bool CanTransport(Role *role) { return true; }			//!< �Ƿ����ڳ����ﴫ��
	virtual bool CanGather(Role* role, NPCGather* gather) { return true; }	//!< �Ƿ����ڳ�����ɼ�
	virtual bool CanMoveOn(Role* role) { return true; }						//!< �Ƿ���������ƶ�
	virtual bool CanUseItem(Role *role, ItemID item_id, int num) { return true; }
	virtual bool CanLeaveGuild() { return true; }					//!< �Ƿ������뿪����
	virtual bool CanFish(Role * role) { return false; } 			//!< �Ƿ����ڳ����е���

	virtual void OnUseItem(Role *role, ItemID item_id, int num) {}
	virtual void OnGetItemByGather(Role * role, ItemID item_id, int num) {}

	virtual bool IsDropCard() { return true; }						//!< �Ƿ����ͼ����
	virtual bool IsLevelOneMonsterOccur() { return true; }			//!< 1�����Ƿ���� (����ץ���

	virtual void OnLeaveFB(Role *role) {};							//!< ��ɫ�����뿪����
	virtual void OnNPCChosedReward(Role *role,ObjID npc_objid,int option_index) {};	//!< NPCѡ�����ֱ�ӽ���
	virtual void OnNPCChosedTalk(Role *role, ObjID npc_objid) {};//!< NPC�Ի�
	virtual bool OnGatherActiveAdventures() { return false; }		//!< �Ƿ񼤻���������

	virtual bool OnTimeout() { return false; }

	virtual void OnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack) {}
	virtual void OnRoleRunAway(battlegameprotocol::BattleGameRoleRunAway* ack) {}
	virtual void OnBattleForcedFinish(battlegameprotocol::BattleGameForceFinishFight* ack) {}

	virtual bool CanTeamMemberGotoLeaderFromOtherScene(Role* member, Role* leader) { return false; }

	//��ɫ�ɿ�ʼս��
	struct CanRoleStartFightParam
	{
		CanRoleStartFightParam()
		{
			battle_mode = 0;
		}
		int battle_mode;
	};
	virtual bool CanRoleStartFight(Role * role, CanRoleStartFightParam &fight_param) { return true; };

	void DelayKickOutRole(int role_id, int reason = CHANGE_SCENE_REASON_INVALID, int delay_ms = 0);
	void DelayKickOutRole(Role *role, int reason = CHANGE_SCENE_REASON_INVALID, int delay_ms = 0);
	void DelayKickOutAllRole(int reason = CHANGE_SCENE_REASON_INVALID, int delay_ms = 0);

	void ClearAllNpcMonster();

	virtual bool CanChallengeOthers(Role * role, Role * target) { return true; }

	/**
	* \brief ��ȡ������ս�������趨
	* \param role ���ʵ��
	* \param speed ���ڷ��ر����趨
	* \param is_force_speed ���ڷ����Ƿ�ǿ�Ʊ����趨�������ս�����޷�����
	* \return �����Ƿ���ս�������趨
	*/
	virtual bool GetBattleAnimationSpeedType(Role * role, ARG_OUT int* speed, ARG_OUT bool* is_force_speed) { return false; }
	
	virtual int GetMineMonsterBattleMode() { return BATTLE_MODE_MINE_MONSTER; }

	/**
	* @brief �Ƿ�������Ҵ����������顣�����ڼ�������������߼��ĳ���ʹ�ã��������Ƴ����ڲ�������ӡ�����ֻ�������ض������������ӵȣ�
	* @note ע�⣬�˺����� role��team �������п����� NULL��������ʹ�õ�ʱ������ؿ��� role��team ����Ϊ NULL �������
	*		Ŀǰ��֪ team ����Ϊ��ʱ�ļ��ʱ��Ϊ����Ҵ�������ʱ���Զ�ƥ���Լ�����������ʱ��
	*		Ŀǰ��֪ role ����Ϊ��ʱ�ļ��ʱ��Ϊ�������ڸ����в�����ƥ��״̬
	*/
	virtual bool CanEnterTeam(Role * role, Team * team, bool is_from_create);
	virtual bool CanInvitationRole(Role *role, int uid) { return true; }
	virtual int CreateTeamType() { return TEAM_TYPE_DEFAULT; }
	virtual bool CanChangeTeamLeader(Role * role, Team * team) { return true; }
	virtual void OnRoleLeaveTeam(Role * role) {}
	virtual void OnTeamDismiss(Role * leader_role, Team * team) {}	//!< ���������ɢ������ֻ�ɶӳ�����һ�Σ����۶������ж�������Ա
	virtual void OnAOIEnterRole(Role * role, Role * target_role) {}	//!< Ŀ���ɫ�����ҵ���Ұ

	/**
	* @brief �����Ƿ�ǿ�������֣�����ǵĻ������ڽ��볡��ʱ��ʼ����ʱ
	* @note ���ǿ�������֣�����Ҫ��OnRoleEnterScene�п���������, ����OnRoleLeaveScene�����û�ԭ����ǰ�İ�������
	*/
	virtual bool IsForceEncounterMineMonster() { return false; }

	/**
	* @brief �ж�ĳ����Ƿ���������
	* @note ���ڵ���ʱ����ǰ�����һ���жϣ��Լ��ٵ��ô���
	*/
	virtual bool CanEncounterMineMonster(Role * role) { return true; }
	virtual long long GetMineMonsterEncounterInterval(Role * role);

	virtual void OnTransport(Role *role, SpecialTransport * transport_obj) {}
	virtual bool OnOpenTreasureChest(Role *role, TreasureChest * transport_obj) { return true; }

	virtual const AttributeList & ReCalcAttrPer(AttributeList &base_add, const AttributeList & cpy_attr_list);
	virtual bool IsAutoRecover() { return true; }				//�Ƿ�����Զ��ظ�Ѫ��ħ��

	void UseGoldRecoveryHp(Role * role,int param1, int param2,int param3);
	void UseGoldRecoveryMp(Role * role, int param1, int param2, int param3);

	//---�ܷ�תְ
	virtual  bool CanChangeProf(Role * role);
protected:
	bool IsAllMonsterDead();
	void CheckSceneMonsterStatus(unsigned long interval, time_t now_second);
	virtual void OnAllMonsterDead() {}

	void SyncHpMp(const char* sync_data);

	//���� ս�����
	void DuelOnFightFinish(battlegameprotocol::BattleGameFightResultAck* ack);
	void OnDuelFightOver(std::set<int> uid_list, int duel_type, bool is_attack, bool is_win, int level_diff, bool target_is_team, bool target_is_red, GameName name, UInt64 coin);

	Scene * m_scene;
	AttributeList m_attr_inc;

	unsigned int m_last_check_monster_status_time;				    // �ϴμ�鳡��״̬ʱ��
};

#endif