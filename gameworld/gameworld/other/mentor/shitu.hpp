#ifndef __SHI_TU_HPP__
#define __SHI_TU_HPP__

#include "servercommon/shitudef.h"
#include "other/attributelist.hpp"

class RoleModuleManager;
class Role;

enum SHITU_RELATION_OPERATE_TYPE
{
	SHITU_OPERATE_TYPE_APPRENTICE,						//徒弟操作（拜师   |  出师）
	SHITU_OPERATE_TYPE_MASTER,							//师傅操作（收徒   |  毕业）

	SHITU_OPERATE_TYPE_MAX,
};

enum SHITU_RELATION_NOTICE_BREAK_TYPE
{
	SHITU_RELATION_BREAK_NOTICE_TYPE_APPRENTICE_TO_MASTER,			//徒弟强制解除关系，通知师傅删除徒弟
	SHITU_RELATION_BREAK_NOTICE_TYPE_MASTER_TO_APPRENTICE,			//师傅强制解除关系，通知徒弟删除师傅

	SHITU_RELATION_BREAK_NOTICE_TYPE_MAX,
};

enum SHITU_RELATION_BREAK_TYPE
{
	SHITU_RELATION_BREAK_TYPE_AUTO = 0,			//自动解除
	SHITU_RELATION_BREAK_TYPE_FORCE,			//强制解除
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

	void OnMakeRelationReq(int type, int target_uid);				// 缔结师徒关系请求
	void OnMakeRelationAck(int type, int target_uid, bool is_agree);		// 缔结师徒关系确认
	void OnBreakRelationReq(int type, int target_uid);			// 出师或毕业请求
	void OnBreakRelationAck(int target_uid, bool is_agree);			// 回复出师或毕业请求
	void OnBreakRelationForece(int target_uid, bool is_use_gold);	// 强制解除关系
	void OnCancelBreakRelation(int target_uid);	// 取消解除关系

	void OnRoleLogin();
	void OnRoleUpLevel(int from_level, int to_level);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	void SendRelationInfo();

	bool IsMasterFull();
	bool HasMaster();

	int GetApprenticeUIDList(int apprentice_uid_list[], int list_len);
	int GetApprenticeCount();			// 获取此时徒弟数量
	
	bool IsMaster(int uid);				//参数uid是否师傅
	bool IsApprentice(int uid);				//参数uid是否徒弟
	bool IsApprenticeFull();

	void SetAutoBreakTime(int target_uid, int type, unsigned int time);					//设置自动解除师徒关系时间，在update那里判断
	void DeleteRelation(int type, int target_uid);

	void InvitationShiTuFb();
	int GetShiTuRewardFlag();
	void SetShiTuRewardFlag(bool is_get);
	void SendShiTuCommonInfo();

	RelationInfo * GetShiTuRelationInfo(int role_id);
	int GetShiTuDay(int role_id);

	//师徒传功
	void OnShiTuReq(int type, int p_1, int p_2);
	//里程
	void FetchMile(int target_uid, int index);
	void FetchAllMile(int target_uid);
	//传功
	void FinishChuanGong(int target_uid, int score);
	//同时在线通知
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

	void SendBreakTimeMailNotice(int target_uid, unsigned int break_end_time);		//冷却期邮件通知
	void SendBreakMailNotice(int target_uid);		//解除师徒关系邮件通知

	RoleModuleManager * m_role_module_mgr;
	time_t m_check_remove_time;

	ShiTuParam m_param;
};

#endif