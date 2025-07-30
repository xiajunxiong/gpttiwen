#ifndef __PET_RANK_HPP__
#define __PET_RANK_HPP__

#include "servercommon/rankdef.hpp"
#include "servercommon/struct/global/rankparam.hpp"
#include "servercommon/internalprotocal/gglobalprotocal.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "protocol/msgscrank.h"
#include "servercommon/skiplist.hpp"

#include <map>
#include <set>

class Role;
class RankManager;
class PetRank
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
			pet_unique_id = 0;
		}

		bool operator< (const IndexData & _a) const
		{
			if (rank_value == _a.rank_value)
			{
				return flexible_ll > _a.flexible_ll;
			}
			return rank_value < _a.rank_value;
		}

		int uid;
		int rank_type;
		long long rank_value;
		int flexible_int;
		long long flexible_ll;
		unsigned long long pet_unique_id;
	};

public:
	PetRank();
	~PetRank();

	void SetRankManager(RankManager *rank_manager) { m_rank_manager = rank_manager; }

	void Init(const PetRankParam &petrank_param, int _rank_type);
	void Update(time_t now_second);

	void OnRankLog(time_t now_second);
	void OnGameworldAccept();

	void GetChangeRankItemList(PetRankParam *petrank_param, int _rank_type);
	void ClearDirtyMark(int _rank_type);
	void OnGetRankList(Role *role, int rank_type, int from_index, int to_index);
	void OnGetRankDetailedInfo(Role *role, int target_uid, int rank_type, unsigned long long pet_unique_item_id);

	void OnRoleInfoChange(Role *_role);																	// 玩家信息改变

	void OnPetRemove(Role * _role, unsigned long long _pet_unique_id);									// 宠物被移除
	void SyncPetRankInfo(Role * _role, int _rank_type, int _index , bool _is_force = false);			// 排行榜信息变更

	void ClearRank(int rank_type);

	const PetRankItem * GetPetRankItem(int rank_type, const PetRankKey &pet_key);

	bool IsRankLoadFinish();

	// 个人 主服:同步来源于隐藏服的数据
	void SyncRankDataFromHidden(crossgameprotocal::HiddenGamePetInfoChange * _msg);

	void GetTopRankList(int rank_type, int max_num, const PetRankItem** out_rank_list, int* out_num);
private:
	// 初始化 
	void InitIndex(int _rank_type, bool _is_fault_tolerant, std::set<int> & _check_set);

	// 同步数据到跨服
	void SyncRankAllDataToCross(int _rank_type);																// 整表
	void SyncRankDataToCross(int _rank_type, int _sync_status, int _old_index, PetRankItem & _rank_item);		// 单项

	// 主服
	void OnRoleInfoChangeByGame(const RoleBaseInfo &_role_info);												// 玩家信息改变

	void OnPetRemoveByGame(const RoleBaseInfo & _role_info , unsigned long long _pet_unique_id);				// 宠物被移除
	void SyncPetRankInfoByGame(Role * _role, int _rank_type, int _index ,bool _is_force = false);				// 排行榜信息变更

	// 跨服
	void OnRoleInfoChangeByHidden(Role * _role);																// 玩家信息改变

	void OnPetRemoveByHidden(Role * _role, unsigned long long _pet_unique_id);									// 宠物被移除
	void SyncPetRankInfoByHidden(Role * _role, int _rank_type, int _index ,bool _is_force = false);				// 排行榜信息变更


	void UpdateRankList(PetRankItem &rank_item, bool _is_force = false);
	void UpdateBaseInfo(RoleBaseInfo &_role_info,  Role * _role);
	void UpdateBaseInfo(RoleBaseInfo &_dest_role_info,const RoleBaseInfo & _src_role_info);
	void UpdatePetInfo(PetRankItem & _rank_item, Role * _role , int _pet_index);

	bool AddUserIndex(int rank_type , int  _uid , const PetRankKey &pet_rank_key, const PetRankItem & _rank_item);
	bool AddPetIndex(int rank_type, const PetRankKey &pet_rank_key, const  PetRankItem & _rank_item);
	bool RemoveUserIndex(int rank_type, int _uid, const PetRankKey & _pet_key);
	bool RemovePetIndex(int rank_type, const PetRankKey & _pet_key);
	bool UpdataUserIndex(int rank_type, int  _uid, const PetRankKey &_pet_key, const PetRankItem & _nex_rank_item);
	int GetRankTypeSize(int rank_type);

	void CalcRankLoadFinish();

	void ConstructionCheckItem(PetRankItem & _check_item , const IndexData & _index_data);
	void ConstructionIndexData( IndexData & _index_data , const PetRankItem & _check_item);

	void GetSimpleInfo(Protocol::SCGetPetRankListAck::PetRankListItem * pet_rank_info, const PetRankItem & pet_rank_item);

	RankManager *m_rank_manager;
	SkipList<PetRankItem> m_rank_skip_list[PET_RANK_TYPE_MAX];

	std::set < int>  m_dirty_mark[PET_RANK_TYPE_MAX];						// < index > 
	std::set < int>  m_old_state[PET_RANK_TYPE_MAX];						// < index > 

	bool m_change_flag[PET_RANK_TYPE_MAX];
	bool m_load_flag[PET_RANK_TYPE_MAX];
	bool m_is_all_load_finish;

	typedef std::map<PetRankKey, IndexData>  PetIndexMap;
	typedef std::map<PetRankKey, IndexData>::iterator  PetIndexMapIt;
	PetIndexMap m_pet_index_map[PET_RANK_TYPE_MAX];

	typedef std::map<UserID, std::map<PetRankKey, IndexData> > UserIndexMap;
	typedef std::map<UserID, std::map<PetRankKey, IndexData> >::iterator UserIndexMapIt;

	UserIndexMap m_user_index_map[PET_RANK_TYPE_MAX];

	unsigned int m_last_change_time[PET_RANK_TYPE_MAX];

	unsigned int m_next_log_timestamp;
};

#endif