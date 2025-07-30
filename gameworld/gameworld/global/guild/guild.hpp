#ifndef __GUILD_HPP__
#define __GUILD_HPP__

#include "global/guild/guildmembermanager.hpp"
#include <list>
#include <map>
#include <set>
#include <vector>
#include "gameworld/item/itembase.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "servercommon/skiplist.hpp"

class GuildManager;

class Guild
{
public:
	struct WeekTaskRankInfo
	{
		bool operator < (const WeekTaskRankInfo& other)
		{
			if (contribute != other.contribute)
			{
				return contribute > other.contribute;
			}
			else if (last_change_time != other.last_change_time)
			{
				return last_change_time < other.last_change_time;
			}
			else
			{
				return role_id < other.role_id;
			}
		}

		int contribute;
		int role_id;
		unsigned int last_change_time;
	};

	typedef std::vector<WeekTaskRankInfo> WeekTaskRankList;

	struct WeekActiveInfo
	{
		WeekActiveInfo():role_id(0), active_val(0) , last_change_time(0) 
		{
		}

		int role_id;
		int active_val;
		unsigned int last_change_time;

		bool operator==(const WeekActiveInfo & _a) const
		{
			if (role_id == _a.role_id)
				return true;
			return false;
		}

		bool operator < (const WeekActiveInfo& _a) const
		{
			if (active_val != _a.active_val)
			{
				return active_val > _a.active_val;
			}
			else if (last_change_time != _a.last_change_time)
			{
				return last_change_time < _a.last_change_time;
			}
			else
			{
				return role_id < _a.role_id;
			}
		}

		bool operator <= (const WeekActiveInfo& _a) const
		{
			return *this < _a || *this == _a;
		}
	};

	enum HALLOFFAME_TYPE
	{
		HALLOFFAME_TYPE_BATTLE = 0,//ս���
		HALLOFFAME_TYPE_RICH = 1,//������
		HALLOFFAME_TYPE_KINDER = 2,//���˰�
		HALLOFFAME_TYPE_MAX,
	};
	struct HallOfFameInfo
	{
		HallOfFameInfo() : value(0), role_id(0), last_change_time(0), prof(0)
		{
			memset(name, 0, sizeof(name));
		}
		bool operator < (const HallOfFameInfo& other)
		{
			if (value != other.value)
			{
				return value > other.value;
			}
			else if (last_change_time != other.last_change_time)
			{
				return last_change_time < other.last_change_time;
			}
			else
			{
				return role_id < other.role_id;
			}
		}

		int value;
		int role_id;
		GameName name;
		unsigned int last_change_time;
		int prof;
	};

	typedef std::vector<HallOfFameInfo> HallOfFameRankList;

public:
	Guild(GuildManager *guild_manager);
	~Guild();

	void * operator new(size_t c);
	void operator delete(void *m);
	GuildMemberManager* GetMemberManager() { return &m_guild_member_manager; }
	GuildMemberManager & GetMemberManagerRef() { return m_guild_member_manager; }
	void SetNeedApproveJoin(Role * role, bool is_need);
	bool IsNeedApproveJoin();		//�Ƿ���Ҫ��׼������빤�����		
	bool JoinGuild(Role * role);

	void Init(const AllGuildParam::GuildParam &guild_param, const GuildGatherDreamParam * guild_dream_param = NULL, bool is_sync_from_origin = false);
	void GetInitParam(AllGuildParam::GuildParam* out_param, bool is_save);
	void GetInitGatherDreamParam(GuildGatherDreamParam * out_param);
	void Save();

	void OnLoginBroadcast(Role * role, bool is_gm = false);
	void OnRoleLogin(Role* role);
	void OnRoleLogout(Role* role);
	void OnRoleLevelUp(Role* role, int level);
	bool IsMember(int uid);

	void Update(unsigned long interval, time_t now_second);
	bool OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnWeekChange();
	bool OnResetData(unsigned int old_dayid, unsigned int now_dayid);

	char * GetName() { return m_base_param.guild_name; }
	void  GetName2(GuildName & dst);
	void GetPatriarchName(GameName & dst);
	int AddGongXian(const UserID& user_id, int value, const char* reason);		//ͨ����ӹ���(��һ��ҹ���+���幱��+�ܹ��ף�
	void OnAddGongXian(const UserID& user_id, int value, const char* reason);	//�˴���ӽ�������Ӽ����ܹ���
	int SetNotice(Role* role, GuildNotice notice);
	int AddExp(int value);		//�����ʽ�
	int LevelUp(Role* role);
	int AddEventhandler(const GameName name, time_t now_time, int type, int param, const ItemGridData *item_grid = NULL);
	int AddEventhandler(GuildEventhandlerParam * eventhandler_node);
	void GuildRename(GuildName guild_name);
	void GuildSetBanner(GuildBanner guild_banner);
	const char* GetBanner() const { return m_base_param.guild_banner; }
	bool HasSetBannerToday() const { return m_base_param.today_set_banner_flag > 0; }
	int GetPlatType() const { return m_base_param.origin_plat_type; }
	int GetServerId() const { return m_base_param.origin_server_id; }

	void SetDirty(bool is_dirty) { m_is_dirty = is_dirty; }
	bool GetDirty() const { return m_is_dirty; }

	int GetEmptyMemberNum()const;
	int GetMaxMemberCount() const;
	GuildID GetGuildID() { return m_base_param.guild_id; }
	int GetGuildLevel() { return m_base_param.guild_level; }
	const char* GetGuildName() { return m_base_param.guild_name; }
	bool GetGuildName(GuildName guild_name);
	const char * GetGuildNamePtr();
	bool GetGuildNotice(GuildNotice guild_notice);

	void GameToHiddenSyncGuildMemberData(int role_id);
	void SendGuildBaseInfo(Role* role = NULL);
	void SendGuildMemberInfo(Role* role = NULL);
	void SendGuildMemberInfoChange(int change_role_id);
	void SendGuildNeedApproveInfo(Role * role = NULL);
	void SendGuildEventInfo();
	time_t GetGuildCreateTime() { return m_base_param.create_timestamp; }

	void GmGuildHandler(int op_type, int param);

	//����
	void GmTanHe();
	void UpdateTanHe(time_t now_second);
	int TanHeZuZhang(Role * role);
	void OnTanHeZuZhang();
	void OnGuildBaseInfoChange();

	//������Ҷ�
	void SetGuildFightTitle(int title_id);
	int GetGuildFightTitle();

	//������������
	void BuildingBuild(Role* role, int build_type, int op_type);
	int GetBuildLevel(int build_type) { return m_base_param.build_level[build_type]; }

//	void LoadJoinSpeedUpMember();
	bool IsCanSpeedUp() { return !!m_base_param.can_speed_up; }
	bool SpeedUpBuilding(Role* role, int speed_time);		//����ֵ,T-���μ���ֱ����ɽ���
//	bool IsJoinCurSpeedUp(UserID use_id);

	void SendBuildInfo(int build_type, Role* role = NULL);
	void SendAllBuildInfo(Role* role = NULL);
	void SendAllUseBulidMaterialInfo(Role* role, int item_id, int num);
	void SendSpeedUpInfo(Role* role = NULL);
	void SendBuildingInfo(Role* role = NULL);
	void SednBuildingOpenerInfo(Role* role = NULL);

	void OnGameHiddenSynSpeedUp();					// ԭ��ͬ���������ݵ����ط�
	void OnGameHiddenRecviceSpeedUp(crossgameprotocal::GameHiddenSynGuildSpeedUpInfo* msg);
	int OnHiddenGameSynSpeedUp(Role* role);			// ���ط���ԭ�������������
	void OnHiddenGameRecviceSpeedUp(crossgameprotocal::HiddenGameSynGuildSpeedUpInfo* msg);
	void OnGameHiddenSynBuildAllInfo();				// ��Ϸ��ͬ�����彨����Ϣ�����ط�
	void OnGameHiddenRecviceBuildAllInfo(crossgameprotocal::GameHiddenSynGuildBuildAllInfo* msg);		//ͬ��������Ϸ��������
	void OnSyncBuildingNotifyInfo(Role * role, Protocol::SCGuildUseBuildMaterialInfo& info);		// ��������ԭ��ͬ������֪ͨ��Ϣ
	void OnSyncBuildingNotifyRecviceInfo(crossgameprotocal::SyncGuildBuildingNotifyInfo* msg);		// �������Կ������ԭ��ͬ���Ľ���֪ͨ��Ϣ

	void GuildTaskSetFinishTimes(Role * role);
	void GuildTaskSetFinishTimes(int role_id, int times);
	void GuildTaskClearSolicit(int role_id, bool is_send = false);
	bool GuildTaskSolicit(int role_id, int task_id, int task_index);
	bool GuildTaskGive(Role * role, int target_role_id);
	void GuildTaskGiveAddNum( int target_role_id, int task_id);
	bool GuildTaskFetch(Role * role);
	bool GuildTaskFetchAddNum(int role_id, int task_id,int fetch_num);
	void GuildTaskMailNotice(int role_id,bool is_all,int target_role_id);
private:
	void BuildingFinish();		//�������,���üǵ����ⲿ����
	bool DismantleHall(Role * role);
	bool DismantleGuestRoom(Role * role);

public:
	// ===================================== ���������� ===================================== //
	void OnGuildWeekTaskEvent(const UserID& user_id, int operate_type, int param, int add_prog_num = 1);
	void RefreshGuildWeekTask();		// ˢ�¼�������
	void RefreshContributionRank(bool is_notify = true);		// ˢ�¹��׶����а�
	void ClearGuildWeekTaskRank();
	void IssueGuildWeekTaskAward();		// ���������������ҷ�����������
	void IssueDayContributionsAward();		// ÿ�չ��׶����н��� -
	void IssueWeekContributionsAward();	// ÿ�ܹ��׶����н��� -
	void SendGuildWeekTaskInfo(Role* role = NULL);
	void SendGuildWeekTaskInfo(const UserID& user_id);
	void SendContrRankListInfo(Role* role = NULL);

	//������ҫս
	void UpdateGuildHonorTitleId(int rank);			//����սʤ�����������仯
	void UpdateGuildHonorTitleIdFromHidden();

	//��������
	void OnFightGuildBoss(Role* role, int chapter_id, int boss_seq, short* remain_hp_pecent_list, bool is_win, long long total_damage);
	void ForceStopGuildBossFight(int chapter_id, int boss_seq);
	short* GetGuildBossRemainHpPercentList() { return &m_boss_param.monster_remain_percent_list[0]; }
	int GetGuildBossSeq() const { return m_boss_param.boss_index; }
	int GetGuildBossChapter() const { return m_boss_param.chapter_id; }
	bool GetGuildBossChapterInfo(GuildBossChapterRank * info);
	void SendGuildBossRank(Role* role = NULL);
	void SetGuildBossChapter(Role* role,int chapter_id,bool is_auto_select = false);
	void SendGuildBossInfo(Role* role = NULL);
	void SendBaoDiReward(int chapter_id, int boss_index);												//�½�ͨ��,���ű��׽���
	void SendKillReward(Role * role, int chapter_id, int boss_seq);		//���Ż�ɱ����
	void SendRankReward(int chapter_id, int day_id);					//������������

	//���弯Ը��
	void OnPublishGatherDream(Role * role, ItemID item_id, int type);			//������Ը
	void OnHiddenGamePublishGatherDream(crossgameprotocal::HiddenGameGuildGatherDreamPublishReq * req);	//���ط�������Ըת����ԭ��
	void OnHiddenGamePublishGatherDreamRet(int is_publish_send, int result, int uuid, int item_id);		//ԭ����������������ط�
	void OnGameHiddenPublishGatherDream(crossgameprotocal::GameHiddenGuildGatherDreamPublishRet *msg);	//ԭ��������Ըת�������ط�
	void OnGiftGatherDream(Role * role, int get_role_id);			//���輯Ը��Ƭ
	void OnHiddenGameGiftGatherDream(crossgameprotocal::HiddenGameGuildGatherDreamGiftReq * req);		//���ط����ͼ�Ըת����ԭ��
	void OnHiddenGameGiftGatherDreamRet(int result, SynGuildGatherDreamGiftInfo info);					//ԭ�����ͽ���������ط�
	void OnGameHiddenGiftGatherDream(crossgameprotocal::GameHiddenGuildGatherDreamGiftRet *msg);		//ԭ�����ͼ�Ըת�������ط�
	void OnHiddenGameGiftGatherDreamAddRecord(crossgameprotocal::HiddenGameGuildGatherDreamGiftAddRecord * msg); //���ط�ת����ԭ����ӻ���|���ͼ�¼
	void OnFetchGatherDream(Role * role);							//��ȡ��Ը��Ƭ
	void OnHiddenGameFetchGatherDream(int uid);																	//���ط���ȡ��Ը����ת����ԭ��(��ʱͨ�������޸�����)
	void OnHiddenGameFetchGatherDreamRet(int result, int uid, int index, ItemID item_id, short fetch_count);	//ԭ����ȡ����������ط�
	void OnGameHiddenFetchGatherDream(crossgameprotocal::GameHiddenGuildGatherDreamFetchRet * msg);				//���ط�����ԭ���������
	void OnHiddenGameFetchGatherDreamChange(crossgameprotocal::HiddenGameGuildGatherDreamFetchChange * msg);	//���ط�ת����ԭ���޸Ľ��
	void OnRemoveMember(int uid);									//����˳����߳�����
	void OnUpdateRoleInfo(SynGatherDreamRecord info, int uid);		//�����Ϣ���,ͬ���޸ļ�Ը��Ϣ
	void SendGatherDreamList(Role * role);							//�·���Ը�б�
	void SendGatherDreamListToCross(int uid);						//�·���Ը�б����
	void SendGatherDreamSignleInfo(Role * role, int *index = NULL);	//�·�������Ը��Ϣ
	void SendGatherDreamSignleInfoToCross(int uid, int *index = NULL);	//�·�������Ը��Ϣ�����
	bool IsCanContOp(Role * role);
	//--------------------------------
	unsigned int GetPatriarchLastLoginTime();

	//------------------------------------------
	std::map<int, time_t> & GetIntervalLimitTimeMap() { return m_invite_interval_limit; }

	void GmSetNoLeader(); // gm�øü���û���峤
	void GmSetName(const char * name);
	void GmSetNotice(const char * notice);

	// -----------------------------------------------------
	// �����Ծ��������а� ��ע : ��Ϣ������Ϊ�� , ������Ϣһ���������������ͬ�������ط�
	// ��ӻ�Ծֵ
	void AddActiveVal(int _role_id ,  int _add_active_val);
	// ��ӻ�Ծֵ - ��������
	void AddActiveValByGameWorld(int _role_id,  int _add_active_val);
	// ��ӻ�Ծֵ - ���ط�����
	void AddActiveValByHidden(int _role_id,  int _add_active_val);
	unsigned int GetActiveLastChangeTimes() { return m_base_param.last_guild_active_change_times; }
	// ͬ����Ծֵ��Ϣ�����ط�
	void SyncActiveInfoToHidden(GuildMember * _member);
	// ͬ�����������Ļ�Ծֵ��Ϣ
	void SyncActiveInfoByGameWorld(crossgameprotocal::GameHiddenGuildActiveInfoChange * _change);
	// ���ͻ�Ծ   ֵ��Ϣ
	void SendGuildActiveInfo(Role * _role);
	// ���ͻ�Ծֵ�������а�
	void SendGuildActivePersonRank(Role * _role);
	// ���»�Ծֵ�������а�͹����Ա��Ϣ - ��������
	void UpdateActiveRankByGameWorld(int _role_id,  int _add_active_val);
	// ���»�Ծֵ�������а�͹����Ա��Ϣ - ���ط�����[ֻ�Ǳ���, ��������Ӧ����������Ϊ��]
	void UpdateActiveRankByHidden(GuildMember * _member , crossgameprotocal::GameHiddenGuildActiveInfoChange * _change);
	// ��ʼ���������а�
	void InitWeekActivePersonRank();
	long long GetWeekActive() { return m_base_param.week_total_guild_active_val; }
	// ��ȡ���ɻ�Ծ���
	void OnGuildActiveGetGiftReq(Role * _role, int _seq);
	// �ջ�Ծ�Ƚ���
	void GuildActiveDayActiveSettlement();
	// �ܻ�Ծ�Ƚ���
	void GuildActiveWeekActivePersonRankSettlement();
	// �Ƴ������Ա
	void OnRemoveMemberByGuildActivePersonRank(GuildMember * _member);
	// ���������а���
	void GetCrossGuildActiveRankReward(int _rank_index , int _rank_val);
	// ͬ���������ط�����һ�Ծ�����ȡ��Ϣ
	void  OnHiddenGameRoleActiveGiftInfo(crossgameprotocal::HiddenGameRoleActiveGiftInfo * _info);
	// ������
	void GmGuildActiveSettlement();
	// -------------------------------------------------------------------
	// 7�ջ�Ծ
	int GetSevenDayTotalActive() const;

	int GetTodayBossChapterId();

	void ResetBossDataByWeek();
	void ResetBossDataByDay();
	void ResetBossDataOnVersionChange();
	void GetBossHpNum(int chapter_id, int boss_index, int cur_hp_percent_w, ARG_OUT long long* cur_hp, ARG_OUT long long* max_hp);

	void GmSetQiFu(Role * _role, int type, int param);
	void InitHallOfFameRank();
	bool IsOpenQiFu(){ return m_base_param.is_open_qifu == 1; }

	void HallOfFameReq(Role * _role, int type, int param);
	
	void QiFuReq(Role * _role);
	void GameWorldQiFu(Role * _role);
	void HiddenQiFu(Role * _role);
	void FromHiddenQifu(int role_id, int add_value);//��ֵ��Դ�����ط�
	void FromGameWorld(int role_id, int value);

	void SetAllRankRefresh(int uid);

	void OnRankValueAdd(Role * _role, int type, int add_value);
	void OnRankValueAddByUid(int uid, int type, int add_value);
	void WeekRankReward();
	void RankReward(HallOfFameInfo * rank_info,int type,int rank_idx);
	void SendRankInfo(Role * _role, int rank_type);
	void SendQifuInfo(Role * _role = NULL);
private:
	GuildManager* m_guild_manager;
	GuildBaseParam m_base_param;
	GuildBossParam m_boss_param;
	GuildWeekTaskParam m_gwt_param;
	GuildGatherDreamParam m_gather_dream_param;

	std::map<int, int> m_gather_dream_map;				//��Ըӳ��  key:role_uid second:index
	WeekTaskRankList m_gwt_rank_list;			// ���������׶����а�

	GuildMemberManager m_guild_member_manager;

	unsigned int m_next_save_time;								// �´α���ʱ��

	bool m_need_refresh_gwt_contribution_rank;					// �Ƿ���Ҫˢ�¼����������а�
	time_t m_next_gwt_contribution_rank_refresh_timestamp;		// ��һ�μ����������а�ˢ�µ�ʱ���
	
	bool m_guild_boss_damage_rank_need_update;
	time_t m_guild_boss_damage_rank_next_update_time;
	std::vector<GuildBossRankItem>	m_guild_boss_damage_rank;

	std::map<int, time_t> m_invite_interval_limit;		//role_id->����ʱ�� ���������� ,10�����ڲ��ܶ�ͬһ����ɫ�����������

	bool m_is_dirty;
	time_t m_next_guild_sync_cross_time;

	int m_gm_flag;			//ûʲô�ã�ֻ���������Ե���

	std::map<int, unsigned int> m_gather_dream_timestamp_map;	//��¼��Ը���������ԭ����֤

	// �ܻ�Ծ���а�  ����Ҫ��¼�����ݿ� , ��ȡ���ݵ�ʱ����������һ�¾ͺ�
	// ���ط���ֻ��¼��Դ��������ͬ������
	SkipList<WeekActiveInfo> m_week_active_rank;	

	//������
	HallOfFameRankList m_battle_rank;
	unsigned int m_battle_refresh_tamp;
	HallOfFameRankList m_rich_rank;
	unsigned int m_rich_refresh_tamp;
	HallOfFameRankList m_kinder_rank;
	unsigned int m_kinder_refresh_tamp;

	//���彨�����,��ע�Ͱ�,��Ȼ�߻��ָľͼ���
// 	std::set<UserID> m_building_speed_up_list;		//init֮���ȡ��Ա�ı����Ϣ�Ϳ���
};		

#endif // __GUILD_HPP__