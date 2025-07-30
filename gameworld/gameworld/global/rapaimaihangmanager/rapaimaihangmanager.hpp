#ifndef __RA_PAI_MAI_HANG_MANAGER_HPP__
#define __RA_PAI_MAI_HANG_MANAGER_HPP__

#include "servercommon/roleactivity/rapaimaihangparam.hpp"

class Role;
class RAPaiMaiHangManager
{
	friend class RMIInitRAPaiMaiHangBackObjectImpl;

public:
	static RAPaiMaiHangManager & Instance();

	void OnServerStart();
	void OnServerStop();
	bool LoadRAPaiMaiHangData(long long id_from);
	bool IsLoadFinish() { return m_load_flag; }

	void Update(time_t now_second);

	void Init(const RARoleParMaiHangDBParam & param);
	void GetInitParam(RARoleParMaiHangDBParam * param);
	void ClearDirtyMark(RARoleParMaiHangDBParam *param);

	void OnAddRecord(int role_id, int type, int seq, int activity_open_day, int param1 = 0, int param2 = 0, int param3 = 0);
	void OnClearRecordByRole(int role_id);
	void OnClearAllRecord();
	void OnSendRecordToRole(Role * role);

private:
	RAPaiMaiHangManager();
	~RAPaiMaiHangManager();

	void Save(const RARoleParMaiHangDBParam * param = NULL);
	void LoadRAPaiMaiHangSucc();
	bool IsDirty() { return m_change_flag; }

	RARoleParMaiHangDBParam m_param;

	std::map<int, bool> m_dirty_mark;				
	std::map<int, bool> m_old_state;

	bool m_change_flag;
	bool m_load_flag;
	time_t m_next_save_db_timestamp;									// 下次保存到数据库的时间戳
};


#endif