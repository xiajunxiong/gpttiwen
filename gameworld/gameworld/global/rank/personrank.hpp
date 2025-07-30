#ifndef __PERSON_RANK_DEF_HPP__
#define __PERSON_RANK_DEF_HPP__

#include <map>
#include <set>
#include "servercommon/skiplist.hpp"
#include "protocol/msgcsrank.h"
#include "protocol/msgscrank.h"
#include "servercommon/struct/global/rankparam.hpp"
#include "servercommon/internalprotocal/gglobalprotocal.h"
#include "servercommon/internalprotocal/ggprotocal.h"
#include "servercommon/activitydef.hpp"
#include "engineadapter.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"

class RankManager;
class Role;
struct UserCacheNode;
class PersonRank
{
	struct IndexData
	{
		IndexData() { this->Reset(); }

		void Reset()
		{
			uid = 0;
			rank_type = -1;
			rank_value = 0;
			flexible_int = 0;
			flexible_ll = 0;
			top_level = 0;
		}

		int uid;
		int rank_type;
		long long rank_value;
		int flexible_int;
		long long flexible_ll;
		int top_level;
	};

public:
	PersonRank();
	~PersonRank();

	void SetRankManager(RankManager *rank_manager) { m_rank_manager = rank_manager; }

	void Init(const PersonRankParam &personrank_param, int _rank_type);
	void Update(time_t now_second);

	void OnUserLogin(Role *user);
	void OnRankLog(time_t now_second);

	void OnGameworldAccept();

	void GetChangeRankItemList(PersonRankParam *personrank_param, int _rank_type);
	void OnGetRankList(Role *role, Protocol::CSGetPersonRankList &rank_list_req);
	void OnGetRankTopUser(Role *role, Protocol::CSGetPersonRankTopUser &top_user_req);
	void OnHiddenGameGetRankList(crossgameprotocal::HiddenGameGetRankListReq * req);
	void OnHiddenGameGetFriendRankList(crossgameprotocal::HiddenGameGetFriendRankListReq * req);
	int GetRankList(int rank_type, int count, PersonRankItem *rank_list);
	void GetRankList(int rank_type, int count, std::vector<int> & rank_uid_list);

	// 角色职业变更
	void OnBaseInfoChange(Role *_role, int _old_prof, int _cur_prof);
	// 同步角色信息到排行榜
	void SyncPersonRankInfo(Role *_role, int _rank_type );
	void UserResetName(Role * _role);
	void OnTopLevelResetSyn(UserCacheNode *node);

	int GetPersonRankIndex(int rank_type, const UserID &user_id);
	const PersonRankItem * GetPersonRankItem(int rank_type, const UserID &user_id);
	int GetRankTopUser(int rank_type, int max_user, UserID *user_list);					// 注意：GetRankTopUser是在最新数据里边找到当前排名靠前的角色id
	int GetRankTopItem(int rank_type, int item_count, PersonRankItem *item_list);		// 注意：GetRankTopUser是在最新数据里边找到当前排名靠前的角色id
	
	bool IsUserOnRank(UserID user_id, int rank_type = -1);
	bool IsRankLoadFinish();
	void ClearRank(int rank_type);
	void ClearDirtyMark(int _rank_type);

	// 个人 主服:同步来源于隐藏服的数据
	void SyncRankDataFromHidden(crossgameprotocal::HiddenGameRoleInfoChange * _msg);	

	// 隐藏服收到游戏服同步的等级助力相关信息
	void OnHiddenRecvGameSynLevelComplementInfo(crossgameprotocal::GameHiddenSynLevelComplementInfo * msg);

	// 获取指定榜单排名前N的平均等级
	int GetAverageByType(int rank_type, int max_count);	//向上取整
	int GetAverageByLevelComplement(int server_id);

	// 获取等级榜前十取平均等级
	int GetAverageLevel();

private:
	// 初始化 m_user_index_map
	void InitUserIndex(int _rank_type , bool _is_fault_tolerant , std::set<int> & _check_set);
	// 同步数据到跨服
	void SyncRankAllDataToCross(int _rank_type);																// 整表
	void SyncRankDataToCross(int _rank_type, int _sync_status, int _old_index, PersonRankItem & _rank_item);	// 个人

	// 角色职业变更 : 主服
	void OnBaseInfoChangeByGame(RoleBaseInfo &_role_info , int _old_prof, int _cur_prof);
	void UserResetNameByGame(int _uid, GameName & _user_name);

	// 角色职业变更 : 跨服
	void OnBaseInfoChangeByHidden(Role *_role, int _old_prof, int _cur_prof);
	void OnRankInfoChangeByHidden(Role * _role, PersonRankItem & _rank_item);
	void UserResetNameByHidden(Role * _role);

	bool IsValidIndex(int rank_type, int index);

	void UpdateRankList(PersonRankItem &rank_item );
	void UpdateBaseInfo(RoleBaseInfo &_role_info, Role * _role);
	void UpdateBaseInfo(RoleBaseInfo &_dest_role_info,const RoleBaseInfo & _src_role_info);

	bool AddUserIndex(int rank_type, const UserID &user_id, PersonRankItem & _check_item);
	bool RemoveUserIndex(int rank_type, const UserID &user_id);
	int GetRankTypeSize(int rank_type);
	void OnRankLoadFinish();

	void CalcRankLoadFinish();

	void ConstructionCheckItem(PersonRankItem & _check_item, const IndexData & _index_data);
	void ConstructionIndexData(IndexData & _index_data, const PersonRankItem & _check_item);
	
	void OnRankChange(int rank_type);
	void OnSetAverageLevelByLevelComplement();
	void OnGameHiddenSynLevelComplementInfo();

	RankManager *m_rank_manager;

	SkipList<PersonRankItem> m_rank_skip_list[PERSON_RANK_TYPE_MAX];

	std::set < int>  m_dirty_mark[PERSON_RANK_TYPE_MAX];					// < 活动类型 , < index , 旧状态 > >
	std::set < int>  m_old_state[PERSON_RANK_TYPE_MAX];						// < 活动类型 , < index , 旧状态 > >

	bool m_change_flag[PERSON_RANK_TYPE_MAX];
	bool m_load_flag[PERSON_RANK_TYPE_MAX];

	typedef std::map<UserID, IndexData> UserIndexMap;
	typedef std::map<UserID, IndexData>::iterator UserIndexMapIt;

	UserIndexMap m_user_index_map[PERSON_RANK_TYPE_MAX];

	unsigned int m_next_log_timestamp;												// 下次记录排行榜日志的时间
	int m_average_level_by_level_complement;				// 等级助力所需前N名平均等级		
	std::map<int, int>	m_average_level_by_level_map;		// 等级助力所需前N名平均等级 key:server_id   跨服使用

	bool m_is_all_load_finish;
};

#endif  // __PERSON_RANK_DEF_HPP__

