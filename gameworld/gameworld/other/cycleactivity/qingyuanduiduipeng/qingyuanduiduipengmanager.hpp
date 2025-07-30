#ifndef __QINGYUAN_DUIDUIPENG_MANAGER_HPP__
#define __QINGYUAN_DUIDUIPENG_MANAGER_HPP__

#include "obj/character/role.h"
#include "servercommon/struct/global/worldstatusparam.hpp"



class Role;
class RoleModuleManager;
class QingYuanDuiDuiPengManager
{
public:
	QingYuanDuiDuiPengManager();
	~QingYuanDuiDuiPengManager();
	static QingYuanDuiDuiPengManager & Instance();

	void Init(const QingYuanDuiDuiPengRankParam & param);
	void GetInitParam(QingYuanDuiDuiPengRankParam *param);

	void OnRoleUpDate(int uid, GameName name, short jifen, short pass_times);
	void OnDayChange();

	bool CanEnterFB(Role * role);

	void SendRoleInfo(Role * role);
	void OnReqRank(Role * role);
	void SendInfo(Role * role);
	void SendToGameRankInfo(int role_id);

	void SendTitleToRole(int title_id, int uid, const char * reason);		//活动结束发放称号
private:
	void OnReward();

	QingYuanDuiDuiPengRankParam m_param;
};

#endif