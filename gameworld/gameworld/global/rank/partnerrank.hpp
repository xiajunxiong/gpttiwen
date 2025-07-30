#ifndef __PARTNER_RANK_HPP__
#define __PARTNER_RANK_HPP__

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
class PartnerRank
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
			partner_id = 0;
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
		int partner_id;
	};

public:
	PartnerRank();
	~PartnerRank();

	void SetRankManager(RankManager *rank_manager) { m_rank_manager = rank_manager; }

	void Init(const PartnerRankParam &partnerrank_param, int _rank_type);
	void Update(time_t now_second);

	void OnUserLogin(Role *user);
	void OnRankLog(time_t now_second);

	void GetChangeRankItemList(PartnerRankParam *partnerrank_param, int _rank_type);
	void ClearDirtyMark(int _rank_type);

	void SyncPartnerRankInfo(Role * _role, int _rank_type, int _partner_id, bool _is_force = false);			// 排行榜信息变更

	// 获取排行榜前列
	void GetTopRankList(int rank_type, int max_num, const PartnerRankItem** out_rank_list, int* out_num);

	void ClearRank(int rank_type);

	const PartnerRankItem * GetPartnerRankItem(int rank_type, const PartnerRankKey &partner_key);

	bool IsRankLoadFinish();

private:
	// 初始化 
	void InitIndex(int _rank_type, bool _is_fault_tolerant, std::set<int> & _check_set);


	// 排行榜信息变更
	void _SyncPartnerRankInfo(Role * _role, int _rank_type, int _partner_id, bool _is_force = false);

	void UpdateRankList(PartnerRankItem &rank_item, bool _is_force = false);
	void UpdatePartnerInfo(PartnerRankItem & _rank_item, Role * _role, int _partner_id);

	bool AddUserIndex(int rank_type, int  _uid, const PartnerRankKey &partner_rank_key, const PartnerRankItem & _rank_item);
	bool AddPartnerIndex(int rank_type, const PartnerRankKey &partner_rank_key, const  PartnerRankItem & _rank_item);
	bool RemoveUserIndex(int rank_type, int _uid, const PartnerRankKey & _partner_key);
	bool RemovePartnerIndex(int rank_type, const PartnerRankKey & _partner_key);
	bool UpdataUserIndex(int rank_type, int  _uid, const PartnerRankKey &_partner_key, const PartnerRankItem & _new_rank_item);

	void ConstructionCheckItem(PartnerRankItem & _check_item, const IndexData & _index_data);
	void ConstructionIndexData(IndexData & _index_data, const PartnerRankItem & _check_item);

	void CalcRankLoadFinish();

	RankManager *m_rank_manager;
	SkipList<PartnerRankItem> m_rank_skip_list[PARTNER_RANK_TYPE_MAX];

	std::set < int>  m_dirty_mark[PARTNER_RANK_TYPE_MAX];						// < index > 
	std::set < int>  m_old_state[PARTNER_RANK_TYPE_MAX];						// < index > 

	bool m_change_flag[PARTNER_RANK_TYPE_MAX];
	bool m_load_flag[PARTNER_RANK_TYPE_MAX];
	bool m_is_all_load_finish;

	typedef std::map<PartnerRankKey, IndexData>  PartnerIndexMap;
	typedef std::map<PartnerRankKey, IndexData>::iterator  PartnerIndexMapIt;
	PartnerIndexMap m_partner_index_map[PARTNER_RANK_TYPE_MAX];

	typedef std::map<UserID, std::map<PartnerRankKey, IndexData> > UserIndexMap;
	typedef std::map<UserID, std::map<PartnerRankKey, IndexData> >::iterator UserIndexMapIt;

	UserIndexMap m_user_index_map[PARTNER_RANK_TYPE_MAX];

	unsigned int m_last_change_time[PARTNER_RANK_TYPE_MAX];

	unsigned int m_next_log_timestamp;
};

#endif