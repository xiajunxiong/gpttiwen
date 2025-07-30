#ifndef __SHI_TU_HPP__
#define __SHI_TU_HPP__

#include "servercommon/shitudef.h"
#include "other/attributelist.hpp"

class RoleModuleManager;
class Role;

enum SHITU_RELATION_OPERATE_TYPE
{
	SHITU_OPERATE_TYPE_APPRENTICE,						//ͽ�ܲ�������ʦ   |  ��ʦ��
	SHITU_OPERATE_TYPE_MASTER,							//ʦ����������ͽ   |  ��ҵ��

	SHITU_OPERATE_TYPE_MAX,
};

enum SHITU_RELATION_NOTICE_BREAK_TYPE
{
	SHITU_RELATION_BREAK_NOTICE_TYPE_APPRENTICE_TO_MASTER,			//ͽ��ǿ�ƽ����ϵ��֪ͨʦ��ɾ��ͽ��
	SHITU_RELATION_BREAK_NOTICE_TYPE_MASTER_TO_APPRENTICE,			//ʦ��ǿ�ƽ����ϵ��֪ͨͽ��ɾ��ʦ��

	SHITU_RELATION_BREAK_NOTICE_TYPE_MAX,
};

enum SHITU_RELATION_BREAK_TYPE
{
	SHITU_RELATION_BREAK_TYPE_AUTO = 0,			//�Զ����
	SHITU_RELATION_BREAK_TYPE_FORCE,			//ǿ�ƽ��
};

class ShiTu
{
public:
	const static int CHECK_REMOVE_INTERVAL = 2;
	const static int AUTO_BREAK_TIME = 72 * 60 * 60;

	ShiTu();
	~ShiTu();

	void Init(RoleModuleManager * mgr, const ShiTuParam & param);
	void GetInitParam(ShiTuParam * param);

	void OnWeekChange();
	void Update(unsigned long interval, time_t now_second);

	void OnMakeRelationReq(int type, int target_uid);				// �޽�ʦͽ��ϵ����
	void OnMakeRelationAck(int type, int target_uid, bool is_agree);		// �޽�ʦͽ��ϵȷ��
	void OnBreakRelationReq(int type, int target_uid);			// ��ʦ���ҵ����
	void OnBreakRelationAck(int target_uid, bool is_agree);			// �ظ���ʦ���ҵ����
	void OnBreakRelationForece(int target_uid, bool is_use_gold);	// ǿ�ƽ����ϵ
	void OnCancelBreakRelation(int target_uid);	// ȡ�������ϵ

	void OnRoleLogin();
	void OnRoleUpLevel(int from_level, int to_level);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	void SendRelationInfo();

	bool IsMasterFull();
	bool HasMaster();

	int GetApprenticeUIDList(int apprentice_uid_list[], int list_len);
	int GetApprenticeCount();			// ��ȡ��ʱͽ������
	
	bool IsMaster(int uid);				//����uid�Ƿ�ʦ��
	bool IsApprentice(int uid);				//����uid�Ƿ�ͽ��
	bool IsApprenticeFull();

	void SetAutoBreakTime(int target_uid, int type, unsigned int time);					//�����Զ����ʦͽ��ϵʱ�䣬��update�����ж�
	void DeleteRelation(int type, int target_uid);

	void InvitationShiTuFb();
	int GetShiTuRewardFlag();
	void SetShiTuRewardFlag(bool is_get);
	void SendShiTuCommonInfo();

	RelationInfo * GetShiTuRelationInfo(int role_id);
	int GetShiTuDay(int role_id);

	//ʦͽ����
	void OnShiTuReq(int type, int p_1, int p_2);
	//���
	void FetchMile(int target_uid, int index);
	void FetchAllMile(int target_uid);
	//����
	void FinishChuanGong(int target_uid, int score);
	//ͬʱ����֪ͨ
	void OnMasterOnline(int master_uid, bool is_login = false);

	void* operator new(size_t c);
	void operator delete(void* m);

	inline const ShiTuParam& GetShiTuParam() const { return m_param; }
private:
	void CheckShitu();

	void AddMaster(int uid);
	void AddApprentice(int uid);
	void DeleteMaster(int uid);
	void DeleteApprentice(int uid);
	int GetApprenticeDayNum(const RelationInfo & master_info);

	void SendBreakTimeMailNotice(int target_uid, unsigned int break_end_time);		//��ȴ���ʼ�֪ͨ
	void SendBreakMailNotice(int target_uid);		//���ʦͽ��ϵ�ʼ�֪ͨ

	RoleModuleManager * m_role_module_mgr;
	time_t m_check_remove_time;

	ShiTuParam m_param;
};

#endif