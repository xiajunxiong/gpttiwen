#ifndef __SHI_TU_SEEKING_MANAGER_H__
#define __SHI_TU_SEEKING_MANAGER_H__

class Role;

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
#include "servercommon/shitudef.h"
#include "servercommon/commondata.hpp"
#include <map>


class ShiTuSeekingManager
{
	friend class RMIInitShiTuSeekingDataBackObjectImpl;
	friend class RMISaveShiTuSeekingDataBackObjectImpl;

public:
	static ShiTuSeekingManager & Instance();

	void OnServerStart();
	void OnServerStop();
	bool LoadShiTuSeekingData(long long id_from);

	void OnUserLogin(const UserID & user_id);
	
	void Update(time_t now_second);

	bool IsLoadFinish() { return m_load_flag; }

	void Init(const ShiTuSeekingParam & param);
	void GetInitParam(ShiTuSeekingParam * param);
	void ClearDirtyMark(ShiTuSeekingParam * param);

	void OnUserChangeSeeking(Role *role, int seeking_type, ShiTuSeekingNotice seeking_notice);

	void SetPreferences(Role * role, char(&arg)[SHI_TU_PREFERENCES_NUM]);  //设置偏好设置
	void SendPreferencesInfo(Role * role);  //设置偏好设置

	void SendShiTuSeekingNoticeList(int seeking_type, Role * role, int role_id = 0);
	void RemoveSeeking(Role * role, bool is_notice = false);
private:
	ShiTuSeekingManager();
	~ShiTuSeekingManager();

	void ClearShiTuSeekingItem(int seeking_type, int index);
	void CheckTimeOut();
	void Save(const ShiTuSeekingParam * param = NULL);
	void LoadShiTuSeekingSucc();
	void GetApprenticeSeekingData(int index,int uid, int seeking_type, ShiTuSeekingNotice seeking_notice);
	void GetMasterSeekingData(int index, int uid, int seeking_type, ShiTuSeekingNotice seeking_notice);
	
	int GetApprenticeSeekingIndexByUid(int uid);
	int GetMasterSeekingIndexByUid(int uid);

	void RemoveEarliestTimeApprenticeSeeking();
	void RemoveEarliestTimeMasterSeeking();
	
	ShiTuSeekingParam m_param;
	
	bool m_apprentice_dirty_mark[SHI_TU_SEEKING_MAX_COUNT];
	bool m_master_dirty_mark[SHI_TU_SEEKING_MAX_COUNT];
	bool m_apprentice_old_state[SHI_TU_SEEKING_MAX_COUNT];
	bool m_master_old_state[SHI_TU_SEEKING_MAX_COUNT];
	bool m_change_flag;
	bool m_load_flag;

	time_t m_shi_tu_seeking_next_save_time;

	std::map<int, ShiTuSeeking> m_apprentice_seeking_list;			//徒弟宣言列表
	std::map<int, ShiTuSeeking> m_master_seeking_list;				//师傅宣言列表
};

#endif


