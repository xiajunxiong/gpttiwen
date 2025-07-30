#ifndef _PLATFORM_BATTLE_HPP_
#define _PLATFORM_BATTLE_HPP_

#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"
#include "servercommon/struct/global/platformbattleparam.hpp"
#include "servercommon/platformbattledef.hpp"

#include <map>
#include <vector>
#include <list>
#include <set>


struct PlatformBattleRankItemInfoNode	//һ����̨�����а��һ���¼ĳ����ҳ�ս��Ϣ
{
	PlatformBattleRankItemInfoNode()
	{
		is_dirty = true;
	}

	bool IsRobot() const;

	bool is_dirty;
	PlatformBattleRoleData rank_item;  //�������
};

struct PlatformBattleRoleInfo			//���������̨�ĳ���ͻ����Ϣ
{
	PlatformBattleRoleInfo();
	PlatformBattleRoleInfo(const MsgPlatformBattleRoleInfoItemList & role_info_list);

	//�ж� ���������еĻ�顢���ﲻ��������Ϊ���� ���� ��̨�Ĺ��������������Ա������������Ϊ���� �� ��̨�Ĺ��������������Ա
	bool IsInPlatformBattle(int exclude_platform_number, std::set<unsigned long long> pet_unique_ids, std::set<int> partner_ids);


	std::map<int, PlatformBattleRoleBattleInfoItem> platform_play_data;  //��̨�� ��Ӧʹ�õĻ�������Ϣ
};

struct  PlatformBattleRobotPetItem
{
	PlatformBattleRobotPetItem()
	{
		pet_capability = 0;
		pos = 0;
	}

	int pet_capability;
	int pos;
	PetBattleData pet_data;
};

struct  PlatformBattleRobotPartnerItem
{
	PlatformBattleRobotPartnerItem()
	{
		partner_capability = 0;
		pos = 0;
	}

	void Reset()
	{
		partner_capability = 0;
		pos = 0;
		partner_data.Reset();
	}

	int partner_capability;
	int pos ;
	PartnerBattleData partner_data;
};

struct PlatformBattleRobotInfo
{
	PlatformBattleRobotInfo()
	{
		robot_role_level = 0;
		robot_role_avatar_type = 0;
		robot_role_prof = 0;
		memset(robot_role_name, 0, sizeof(robot_role_name));
	}

	int robot_role_level;										//���������ݡ�        �����̨���ڻ����ˣ�������Щ���� ��Ŀǰһ����̨���ֻ��һ��������
	int robot_role_avatar_type;									//���������ݡ������̨���ڻ����ˣ�������Щ����
	int robot_role_prof;									//���������ݡ������̨���ڻ����ˣ�������Щ����
	GameName robot_role_name;								//���������ݡ�			�����̨���ڻ����ˣ�������Щ����
	std::vector<PlatformBattleRobotPetItem> pet_robot_data;				//���������ݡ������̨���ڻ����ˣ�������Щ����
	std::vector<PlatformBattleRobotPartnerItem> partner_robot_data;			//���������ݡ������̨���ڻ����ˣ�������Щ����
};

struct PlatformBattlePlatformInfo       //�����̨��ҳ�ս��Ϣ
{
	PlatformBattlePlatformInfo();

	bool Add(int rank, const PlatformBattleRoleData & rank_item);
	bool SwapRank(int rank1, int rank2, const  PlatformBattleRoleData * rank1_update_item);
	void RemoveRankNode(int rank);
	void RemoveRedundant(int num = PLATFORM_BATTLE_RANK_ITEM_MAX);  //ɾ������50����
	int GetRankItemSize() const;
	PlatformBattleRoleData * GetPlatformBattleRoleData(int rank_index);
	PlatformBattleRankItemInfoNode * GetPlatformBattleRankItemInfoNode(int rank_index);
	PlatformBattleRoleData  GetPlatformBattleRoleDataTemp(int rank_index) const;
	void SetPlatformBattleRankItemInfoChange(int rank_index);
	void AddRobot();

	int platform_num;
	int exist_num;
	std::vector<PlatformBattleRankItemInfoNode>		rank_list;				//��̨������б���Ϣ
	std::vector<PlatformBattleRobotInfo> robot_infos;
	std::set<int> robot_number;		
};

enum PLATFORM_BATTLE_ARG_TYPE
{
	PLATFORM_BATTLE_ARG_TYPE_PET = 1,
	PLATFORM_BATTLE_ARG_TYPE_PARTNER,
};

class Role;
class CommonSave;
struct CommonSavePlatformBattleRoleRecord;
struct CommonSavePlatformBattleReceordData;
struct CommonSavePlatformBattleAchievementData;
struct CommonSavePlatformBattleOtherData;
class PlatformBattle
{
public:
	friend struct PlatformBattlePlatformInfo;
public:
	static PlatformBattle & Instance();

	void OnServerStart();
	void OnServerStop();
	bool LoadPlatformBattleData(long long inc_id_from);
	void LoadPlatformBattleDataSucc();
	bool IsLoadFinish();
	void OnGameServerConnect(std::set<int> server_id);

	void OnDayChange();
	void OnUserLogin(Role *role);
	void OnUserLogout(Role *role);
	void OnUserLoginOriginGame(int role_id);	
	void OnDelPet(Role * role,unsigned long long pet_uuid);

	void InitPlatformInfo(int platform_num, int rank, const PlatformBattleRoleData& role_data);

	//��ս��̨
	void ChallengePlatform(Role * role, int platform_number, int rank, MsgPlatformBattlePlayItem(&pet_index)[PLATFORM_BATTLE_PET_PARTNER_MAX], MsgPlatformBattlePlayItem(&partner_id)[PLATFORM_BATTLE_PET_PARTNER_MAX]);
	//������̨�ϻ��ͳ��� 
	void RemovePlatformPlayInfo(Role * role, int platform_number, bool is_client_req, const char * reason);
	void RemovePlatformPlayInfoByRoleId(int role_id, int platform_number, const char * reason);

	void Update(unsigned long interval, time_t now_second);

	void OnBattleFinish( int platform_number, int winner, int loser, BattleData_Business business_data, bool is_win);

	void SendRoleInfo(Role * role);
	void SendPlatformInfo(Role * role, int platform_number, int role_id = 0);
	void SendPlatformFirstInfo(Role * role, const std::vector<unsigned char> & platform_number_list);

	static int GetMinPetStrengthLevel(const PlatformBattleRoleData & role_data);

	void SetChangeFlag(bool flag = true);

	void GmReq(Role * role,int type, int param);

	//---------------------------------------------------
	PlatformBattleRoleInfo * GetPlatformBattleRoleInfo(int role_id);   
	//------------------------ս��----------------------
	CommonSavePlatformBattleReceordData * GetRoleRecordData(int role_id, bool is_auto_create = true, bool set_data_change = true);
	void AddRecord(int role_id, const CommonSavePlatformBattleRoleRecord * record);
	void SendRecord(Role * role, int role_id = 0);
	void SendLastRankInfo(Role * role, int role_id = 0);
	void SendRecordNotice(int role_id);
	void SetClientNotice(int role_id, int type, int param);		//�����û��
	void ClearLastRankRecord();
	void ClearRecord();

	//------------------���|����|��ɫ�����ı�ʱͬ�����ݵ��Ѿ����޵��л������
	void SyncPartnerData(Role * role);
	void SyncPetData(int role_id, const PlatformBattlePet & platform_battle_pet_data);
	void SynRoleInfo(Role * role);
	//----------------------------ͬ�����һЩ����--------------------------
	enum
	{
		PLATFORM_BATTLE_SYNC_ROLE_INFO_DEFAULT = 0,
		PLATFORM_BATTLE_SYNC_ROLE_INFO_UPDATE,
	};
	void SetRoleInfo(int type, const std::vector<MsgPlatformBattleRoleInfoItemList> & vec);
	void SyncRoleInfoHiddenToGameworld(int server_id = 0);	
	void ChangeRoleInfoHiddenToGameworld2(int role_id, const std::vector<int> & platform_num);
	void ChangeRoleInfoHiddenToGameworld(int role_id, int platform_num);

	//-------------------------�ɾ�-----------------------------
	int FetchAchievementInfoReward(Role * role, int type, int seq);
	int GameHiddenFetchAchievementInfoReward(int role_id, int type, int seq);
	void ResetAchievement();
	void OnRankChange(int role_id, int platform_number, int rank, int old_rank);
	void OnRankUp(int role_id, int platform_number, int rank);
	void OnTop1(int role_id, int platform_number);
	void OnAckSucc(int role_id, int platform_number);
	void OnDefSucc(int role_id, int platform_number);
	void OnPartnerAndPetNumChange(int role_id);
	void OnTop1DayChange(int role_id, int platform_number, int day_count);
	void OnWinHighCapacity(int role_id, int platform_number);	
	void SendAchievementInfo(int role_id, bool is_hidden_to_game = false);
private:
	PlatformBattle();
	virtual ~PlatformBattle();
	
	void Save(bool is_stop = false);
	void ClearDirtyMark(int platform_num);

	void CheckAllPlatform();

	void RemoveRoleInfoMapItemByRoleId(int platform_num, int role_id);
	void RemoveRoleInfoMapItem(int platform_num, const PlatformBattleRoleData & rank_item);
	void UpdateRoleInfoMapItem(int platform_num,int rank, const PlatformBattleRoleData & rank_item);

	bool GetBattleData(int platform_num, int challenge_rank, int ARG_OUT & def_uid, std::vector<PetBattleData> ARG_OUT & pet_battle_data, std::vector<int> ARG_OUT & pet_pos, std::vector<PartnerBattleData> ARG_OUT & partner_battle_data, std::vector<int> ARG_OUT & partner_pos);

	void OnTimeOutSettleAccounts();
	void SettleAccounts();
	void ClearRoleInfo();
	void ResetPlatformBattle();
	int CanJoinPlatformBattle(int role_id, int platform_number, int challenge_rank, std::set<unsigned long long> pet_unique_id, std::set<int> partner_ids);
	int CheckArg(Role * role, const MsgPlatformBattlePlayItem(&arg_pet_play_info)[PLATFORM_BATTLE_PET_PARTNER_MAX], const MsgPlatformBattlePlayItem(&arg_partner_play_info)[PLATFORM_BATTLE_PET_PARTNER_MAX], std::set<unsigned long long> ARG_OUT & pet_indexs, std::set<int> ARG_OUT & partner_ids);
private:
	CommonSavePlatformBattleAchievementData * GetCommonSavePlatformBattleAchievementData(int role_id, bool is_auto_create = true, bool set_data_change = true);
	CommonSavePlatformBattleOtherData * GetCommonSavePlatformBattleOtherData();
private:
	bool m_load_flag;
	time_t m_next_save_time;
	bool m_change_flag;

	std::map<int, PlatformBattlePlatformInfo> m_platform_info;		//���е���̨��ҳ�ս��Ϣ
	std::map<int, PlatformBattleRoleInfo> m_role_info;  //����ÿ����̨����ҹ����������ѯ���ռ����Щ��̨����Ϸ���зݿ���

	CommonSave * m_record_save_data;
	CommonSave * m_role_achievement_save_data;
	CommonSave * m_other_save_data;
};

#endif

