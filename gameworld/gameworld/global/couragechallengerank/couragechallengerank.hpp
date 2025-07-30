#ifndef __COURAGE_CHALLENGE_RANK_HPP__
#define __COURAGE_CHALLENGE_RANK_HPP__

#include "servercommon/struct/global/couragechallengerankdef.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"

class Role;
class CourageChallengeRankManager
{
	friend class RMIInitCourageChallengeRankBackObjectImpl;

public:
	static CourageChallengeRankManager & Instance();

	void Update(unsigned int now_second);
	
	void OnServerStart();
	void OnServerStop();
	bool LoadCourageChallengeRankData(long long id_from);
	bool IsLoadFinish() { return m_load_flag; }

	void Init(const CourageChallengeRankParam & param);
	void GetInitParam(CourageChallengeRankParam * param);
	void ClearDirtyMark(CourageChallengeRankParam *param);

	void RoleInfoChange(SynCourageChallengeRankInfo * role_info);
	const CourageChallengeRankItem * GetRankItem(int role_id);
	void OnGetRankUidList(std::vector<int> & rank_uid_list);

	void GameHiddenRankList(crossgameprotocal::HiddenGameCourageChallengeRankReq *req);
	void SendRankListInfo(Role * role, int rank_type, int count, int *role_uid_list);

private:
	CourageChallengeRankManager();
	~CourageChallengeRankManager();

	//int GetConformRoleRankPos(const SynCourageChallengeRankInfo &role_info, int role_pos = 0);			//获取此时符合玩家排行的位置
	//void UpdataRankList(SynCourageChallengeRankInfo* role_info = NULL, int conform_role_pos = 0);
	void UpdataRankList();
	void Save(const CourageChallengeRankParam * param = NULL);
	void LoadCourageChallengeRankSucc();
	void OnGetInfo(SynCourageChallengeRankInfo & role_info);

	void PrintfLog();		//输出排行前十log
	bool IsDirty() { return m_change_flag; }

	typedef std::map<int, int>::iterator UidIndexMapIt;

	CourageChallengeRankItem m_rank_list[MAX_COURAGE_CHALLENGE_RANK_NUM];

	//std::map<int, int> m_user_index_map;								// 排行位置  key:uid  second:排行榜中的下标

	CourageChallengeRankParam m_rank_param;								// 本服所有玩家信息
	std::map<int, bool> m_dirty_mark;
	std::map<int, bool> m_old_state;

	bool m_change_flag;
	bool m_load_flag;							
	unsigned int m_next_save_db_timestamp;								// 下次保存到数据库的时间戳
};

#endif