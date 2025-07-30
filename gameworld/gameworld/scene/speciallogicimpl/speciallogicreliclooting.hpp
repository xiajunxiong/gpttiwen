#ifndef __SPECIAL_LOGIC_RELIC_LOOTING_HPP__
#define __SPECIAL_LOGIC_RELIC_LOOTING_HPP__

#include "scene/speciallogic.hpp"
#include "protocol/msgscene.h"
#include "servercommon/taskdef.h"
#include "global/activity/activityimpl/activityreliclooting.hpp"
#include "global/team/team.hpp"
#include <vector>
#include <map>
#include "config/activityconfig/reliclooting/activityreliclootingconfig.hpp"
#include <math.h>
#include "servercommon/reliclootingdef.h"

struct RoleSpecialEffect
{
	RoleSpecialEffect() {}
	enum ROLE_SPECIAL_EFFECT_TYPE
	{
		ROLE_SPECIAL_EFFECT_TYPE_NORMAL = 0,	//����
		ROLE_SPECIAL_EFFECT_TYPE_1 = 1,			//��Ч ץ��
		ROLE_SPECIAL_EFFECT_TYPE_2 = 2,			//��Ч ��ץ������
		ROLE_SPECIAL_EFFECT_TYPE_3 = 3,			//��Ч ����
		ROLE_SPECIAL_EFFECT_TYPE_4 = 4,			//��Ч ���� ������
		ROLE_SPECIAL_EFFECT_TYPE_5 = 5,			//��Ч ���� ������ �Ĵ������
		ROLE_SPECIAL_EFFECT_TYPE_6 = 6,			//��Ч ����
		ROLE_SPECIAL_EFFECT_TYPE_7 = 7,			//��Ч ��Ԯ
		ROLE_SPECIAL_EFFECT_TYPE_8 = 8,			//��Ч ��Ԯ ����Ԯ����
		ROLE_SPECIAL_EFFECT_TYPE_9 = 9,			//��Ч ����� ����
		ROLE_SPECIAL_EFFECT_TYPE_10 = 10,		//��Ч ������
		ROLE_SPECIAL_EFFECT_TYPE_11 = 11,		//��Ч ����������ٱȼ��ߴ�
		ROLE_SPECIAL_EFFECT_TYPE_12 = 12,		//��Ч ����� �ļ��ٽ����ļ���Ч��
		ROLE_SPECIAL_EFFECT_TYPE_13 = 13,		//��Ч ���� ���������
		ROLE_SPECIAL_EFFECT_TYPE_14 = 14,		//��Ч ���� ���ɵ���Ч
		ROLE_SPECIAL_EFFECT_TYPE_15 = 15,		//��Ч Ų�Ƴ� ��ʩ����Ч
		ROLE_SPECIAL_EFFECT_TYPE_16 = 16,		//��Ч �������
		ROLE_SPECIAL_EFFECT_TYPE_MAX,
	};
	int uid;
	int effect_type;
};

class SpecialLogicRelicLootingFb : public SpecialLogic
{
public:
	SpecialLogicRelicLootingFb(Scene * scene);
	virtual ~SpecialLogicRelicLootingFb();

	virtual void OnRoleEnterScene(Role *role);					//!< ���ǽ��볡��
	virtual void OnRoleLeaveScene(Role *role, bool is_logout);	//!< �����뿪����
	virtual void OnRoleLeaveTeam(Role * role);
	virtual bool CanDestroy() { return true; }
	virtual bool CanMoveOn(Role* role);					//!< �Ƿ���������ƶ�
	virtual void Update(unsigned long interval, time_t now_second);

	//virtual bool CanGather(Role* role, NPCGather* gather) { return false; }	//!< �Ƿ����ڳ�����ɼ�
	virtual bool CanJoinOtherActivity() { return false; }
	virtual bool CanInvitationRole(Role *role, int uid) { return false; }
	virtual bool CanChangeTeamLeader(Role * role, Team * team) { return false; }
	virtual void OnTeamDismiss(Role * leader_role, Team * team) {}	//!< ���������ɢ������ֻ�ɶӳ�����һ�Σ����۶������ж�������Ա
	virtual void OnAOIEnterRole(Role * role, Role * target_role);	//!< Ŀ���ɫ�����ҵ���Ұ

	int GetXYDistend(int x1, int y1, int x2, int y2);

	bool OnEnterFbBeforeSet(Role * role, bool is_defender);//����
	void TeamRecord(Team * team_info);//��¼������Ϣ

	//��Ҳ���
	void GetSkillItem(Role * role, int param1);
	void GiveUpSkillItem(Role * role);
	void OnRoleUseSkill(Role * role, int param1, int param2, int param3, int param4);//ʹ�ü���
	void GiveUpSkillStatus(Role * role);//�жϵ�ǰ���ܣ���������Ч����
	void OnRoleTs(Role*role, int member_uid);//���ܴ���
	void SendMake(Role * role, int mark_type, int pos_x, int pos_y);

	//����Ч��
	void SkillEffect(Role *role, RelicLootingFBSkillInfo *effect_skill, bool *is_delete = NULL);
	void OpenLock(Role *role, const RelicLootingSkillCfg * skill_info, RelicLootingFBSkillInfo * skill);//����
	void HelpTeamMember(Role *role, const RelicLootingSkillCfg * skill_info, RelicLootingFBSkillInfo * skill);//��Ԯ
	void CatchPlayer(Role * role, const RelicLootingSkillCfg * skill_info, RelicLootingFBSkillInfo * skill);//ץ��
	void Galloping(Role *role, const RelicLootingSkillCfg * skill_info);//����
	void Transmt(Role *role, Posi* transmt_pos = NULL);//����or������
	void RefreshPos(Role * role, Posi * transmt_pos, int distance);
	void SoftBody(Role *role, const RelicLootingSkillCfg * skill_info);//�����
	void DaoCaoRen(Role *role, const RelicLootingSkillCfg * skill_info);//������
	void ChuanSongFaZhen(Role *role, const RelicLootingSkillCfg * skill_info);//���ͷ���

	//�������˼���Ч��
	void InquiryTransmt(Role *role, Role * member, unsigned int end_tamp);

	void SendObjInfo(Role * role = NULL);//�·�����obj��Ϣ
	void SendTeamMemberPos();//�·���Ӫ����λ��--ˢ��1��һ��
	void SendTeamInfo(Role *role = NULL);

	//������Ҷ������˾͵������׼��
	void OnMemberAllEnterFb();

	virtual bool CanEnterTeam(Role * role, Team * team, bool is_from_create) { return true; }
private:
	void OnGameBegin();//��ʽ��ʼ��Ϸ
	void CreateBox();

	void RefreshSkillBox();
	void ClearAllPayerEffect();

	bool HasUser(int uid, bool *is_defender = NULL);
	void OnGameEndMakeRoleInTeam();//����볡���������
	void SendSceneTampInfoToAll(int tamp_type, unsigned int tamp, Role * role = NULL);
	void OnPlayEnd(int win_type);

	void OnCheckAllBoxBeSteal();//������б����Ƿ񶼱���ȡ
	void OnAllBoxBeSteal();//���б��ض�����ȡ

	void OnCheckAllAttackerBeCatch();//���������Ƿ񶼱�ץס��
	void OnAllAttackerBeCATCH();//���е����߱�ץ��
	void SendPlayerSkillInfo(Role * role);//��Ҽ�����Ϣ

	void HasUserSkillBelong(Role *role);
	//------------------------------------------------------------------------
	void OnRoleStartCatch(int uid);
	void OnRoleStartBeCatch(int uid);
	void OnRoleStartSprint(int uid);
	void OnRoleStartTransfer(int uid);
	void OnRoleStartTransferBack(int uid);
	void OnRoleStartUnlock(int uid);
	void OnRoleStartHelp(int uid);
	void OnRoleStartBeHelp(int uid);
	void OnRoleStartSneack(int uid);
	void OnRoleStartDaoCaoRen(int uid);
	void OnRoleStartSoftBody(int uid);
	void OnRoleStartSoftBodyEnd(int uid);
	void OnRoleStartBeTrap(int uid);
	void OnRoleStartDoll(int uid);
	void OnRoleStartRefresh(int uid);

	void OnRoleStopMoveStatus(int uid);

	void SetSpecialEffect(int uid, int effect_type);
	void SendRoleSpecialEffect(Role * role, RoleSpecialEffect *info);

	void CheckAllTeamHasOutFollow();//������ж����Ա�Ƿ�����

	unsigned int check_out_follow_tamp;//������ȫԱ����
	bool all_member_ready;//�Ƿ�������Ҷ�׼������
	unsigned int member_enter_wait_tamp;//�����ȴ�10��-�ȿͻ��˷�����ҵ�λ��ϲ�׼������ʱ
	unsigned int fb_ready_tamp;// ׼��ʱ��
	unsigned int m_fb_end_tamp;//��Ϸ����ʱ���
	unsigned int m_kick_out_tamp;
	unsigned int m_drop_skill_tamp;//���似��ʱ���
	unsigned int play_time;	//��Ϸ��ʼʱ���
	int win_type;//ʤ������

	bool allow_move;//����Ƿ������ƶ�

	PlayerInfo * GetPlayerInfo(int uid ,bool* is_defender = NULL);
	RoleSpecialEffect * GetRoleSpecialEffect(int uid);

	int defender_num;
	PlayerInfo defender_list[RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM];//������

	int attacker_num;
	PlayerInfo attacker_list[RELIC_LOOT_MAX_ONE_SIDE_PLAYER_NUM];//������

	//std::vector<RelicLootingTeamRecodeInfo> m_team_recode_vec;//�����¼

	std::map<ObjID, Protocol::SCRelicLootingObjInfo::Objinfo> m_obj_map;
	//std::map<ObjID ,Protocol::SCRelicLootingObjInfo::Objinfo> m_skill_vec;
	std::map<UserID, Protocol::SCRelicLootingObjInfo::Objinfo> skill_belog_map;

	std::map<int, RoleSpecialEffect> m_special_effect_map;

	unsigned next_refresh_team_member_pos_tamp;
};

#endif