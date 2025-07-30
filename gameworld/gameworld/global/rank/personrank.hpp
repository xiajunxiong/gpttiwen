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

	// ��ɫְҵ���
	void OnBaseInfoChange(Role *_role, int _old_prof, int _cur_prof);
	// ͬ����ɫ��Ϣ�����а�
	void SyncPersonRankInfo(Role *_role, int _rank_type );
	void UserResetName(Role * _role);
	void OnTopLevelResetSyn(UserCacheNode *node);

	int GetPersonRankIndex(int rank_type, const UserID &user_id);
	const PersonRankItem * GetPersonRankItem(int rank_type, const UserID &user_id);
	int GetRankTopUser(int rank_type, int max_user, UserID *user_list);					// ע�⣺GetRankTopUser����������������ҵ���ǰ������ǰ�Ľ�ɫid
	int GetRankTopItem(int rank_type, int item_count, PersonRankItem *item_list);		// ע�⣺GetRankTopUser����������������ҵ���ǰ������ǰ�Ľ�ɫid
	
	bool IsUserOnRank(UserID user_id, int rank_type = -1);
	bool IsRankLoadFinish();
	void ClearRank(int rank_type);
	void ClearDirtyMark(int _rank_type);

	// ���� ����:ͬ����Դ�����ط�������
	void SyncRankDataFromHidden(crossgameprotocal::HiddenGameRoleInfoChange * _msg);	

	// ���ط��յ���Ϸ��ͬ���ĵȼ����������Ϣ
	void OnHiddenRecvGameSynLevelComplementInfo(crossgameprotocal::GameHiddenSynLevelComplementInfo * msg);

	// ��ȡָ��������ǰN��ƽ���ȼ�
	int GetAverageByType(int rank_type, int max_count);	//����ȡ��
	int GetAverageByLevelComplement(int server_id);

	// ��ȡ�ȼ���ǰʮȡƽ���ȼ�
	int GetAverageLevel();

private:
	// ��ʼ�� m_user_index_map
	void InitUserIndex(int _rank_type , bool _is_fault_tolerant , std::set<int> & _check_set);
	// ͬ�����ݵ����
	void SyncRankAllDataToCross(int _rank_type);																// ����
	void SyncRankDataToCross(int _rank_type, int _sync_status, int _old_index, PersonRankItem & _rank_item);	// ����

	// ��ɫְҵ��� : ����
	void OnBaseInfoChangeByGame(RoleBaseInfo &_role_info , int _old_prof, int _cur_prof);
	void UserResetNameByGame(int _uid, GameName & _user_name);

	// ��ɫְҵ��� : ���
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

	std::set < int>  m_dirty_mark[PERSON_RANK_TYPE_MAX];					// < ����� , < index , ��״̬ > >
	std::set < int>  m_old_state[PERSON_RANK_TYPE_MAX];						// < ����� , < index , ��״̬ > >

	bool m_change_flag[PERSON_RANK_TYPE_MAX];
	bool m_load_flag[PERSON_RANK_TYPE_MAX];

	typedef std::map<UserID, IndexData> UserIndexMap;
	typedef std::map<UserID, IndexData>::iterator UserIndexMapIt;

	UserIndexMap m_user_index_map[PERSON_RANK_TYPE_MAX];

	unsigned int m_next_log_timestamp;												// �´μ�¼���а���־��ʱ��
	int m_average_level_by_level_complement;				// �ȼ���������ǰN��ƽ���ȼ�		
	std::map<int, int>	m_average_level_by_level_map;		// �ȼ���������ǰN��ƽ���ȼ� key:server_id   ���ʹ��

	bool m_is_all_load_finish;
};

#endif  // __PERSON_RANK_DEF_HPP__

