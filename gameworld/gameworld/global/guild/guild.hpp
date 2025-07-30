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
		HALLOFFAME_TYPE_BATTLE = 0,//战神榜
		HALLOFFAME_TYPE_RICH = 1,//富豪榜
		HALLOFFAME_TYPE_KINDER = 2,//善人榜
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
	bool IsNeedApproveJoin();		//是否需要批准申请加入工会的人		
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
	int AddGongXian(const UserID& user_id, int value, const char* reason);		//通用添加贡献(玩家货币贡献+家族贡献+总贡献）
	void OnAddGongXian(const UserID& user_id, int value, const char* reason);	//此处添加仅限于添加家族总贡献
	int SetNotice(Role* role, GuildNotice notice);
	int AddExp(int value);		//家族资金
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

	//弹劾
	void GmTanHe();
	void UpdateTanHe(time_t now_second);
	int TanHeZuZhang(Role * role);
	void OnTanHeZuZhang();
	void OnGuildBaseInfoChange();

	//家族大乱斗
	void SetGuildFightTitle(int title_id);
	int GetGuildFightTitle();

	//家族内政建设
	void BuildingBuild(Role* role, int build_type, int op_type);
	int GetBuildLevel(int build_type) { return m_base_param.build_level[build_type]; }

//	void LoadJoinSpeedUpMember();
	bool IsCanSpeedUp() { return !!m_base_param.can_speed_up; }
	bool SpeedUpBuilding(Role* role, int speed_time);		//返回值,T-本次加速直接完成建造
//	bool IsJoinCurSpeedUp(UserID use_id);

	void SendBuildInfo(int build_type, Role* role = NULL);
	void SendAllBuildInfo(Role* role = NULL);
	void SendAllUseBulidMaterialInfo(Role* role, int item_id, int num);
	void SendSpeedUpInfo(Role* role = NULL);
	void SendBuildingInfo(Role* role = NULL);
	void SednBuildingOpenerInfo(Role* role = NULL);

	void OnGameHiddenSynSpeedUp();					// 原服同步建造数据到隐藏服
	void OnGameHiddenRecviceSpeedUp(crossgameprotocal::GameHiddenSynGuildSpeedUpInfo* msg);
	int OnHiddenGameSynSpeedUp(Role* role);			// 隐藏服向原服发起加速请求
	void OnHiddenGameRecviceSpeedUp(crossgameprotocal::HiddenGameSynGuildSpeedUpInfo* msg);
	void OnGameHiddenSynBuildAllInfo();				// 游戏服同步家族建造信息到隐藏服
	void OnGameHiddenRecviceBuildAllInfo(crossgameprotocal::GameHiddenSynGuildBuildAllInfo* msg);		//同步来自游戏服的数据
	void OnSyncBuildingNotifyInfo(Role * role, Protocol::SCGuildUseBuildMaterialInfo& info);		// 向跨服或者原服同步建造通知信息
	void OnSyncBuildingNotifyRecviceInfo(crossgameprotocal::SyncGuildBuildingNotifyInfo* msg);		// 接收来自跨服或者原服同步的建造通知信息

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
	void BuildingFinish();		//不做检测,调用记得在外部检测好
	bool DismantleHall(Role * role);
	bool DismantleGuestRoom(Role * role);

public:
	// ===================================== 家族周任务 ===================================== //
	void OnGuildWeekTaskEvent(const UserID& user_id, int operate_type, int param, int add_prog_num = 1);
	void RefreshGuildWeekTask();		// 刷新家族任务
	void RefreshContributionRank(bool is_notify = true);		// 刷新贡献度排行榜
	void ClearGuildWeekTaskRank();
	void IssueGuildWeekTaskAward();		// 给家族里的所有玩家发放周任务奖励
	void IssueDayContributionsAward();		// 每日贡献度排行奖励 -
	void IssueWeekContributionsAward();	// 每周贡献度排行奖励 -
	void SendGuildWeekTaskInfo(Role* role = NULL);
	void SendGuildWeekTaskInfo(const UserID& user_id);
	void SendContrRankListInfo(Role* role = NULL);

	//家族荣耀战
	void UpdateGuildHonorTitleId(int rank);			//工会战胜出名单发生变化
	void UpdateGuildHonorTitleIdFromHidden();

	//家族试炼
	void OnFightGuildBoss(Role* role, int chapter_id, int boss_seq, short* remain_hp_pecent_list, bool is_win, long long total_damage);
	void ForceStopGuildBossFight(int chapter_id, int boss_seq);
	short* GetGuildBossRemainHpPercentList() { return &m_boss_param.monster_remain_percent_list[0]; }
	int GetGuildBossSeq() const { return m_boss_param.boss_index; }
	int GetGuildBossChapter() const { return m_boss_param.chapter_id; }
	bool GetGuildBossChapterInfo(GuildBossChapterRank * info);
	void SendGuildBossRank(Role* role = NULL);
	void SetGuildBossChapter(Role* role,int chapter_id,bool is_auto_select = false);
	void SendGuildBossInfo(Role* role = NULL);
	void SendBaoDiReward(int chapter_id, int boss_index);												//章节通关,发放保底奖励
	void SendKillReward(Role * role, int chapter_id, int boss_seq);		//发放击杀奖励
	void SendRankReward(int chapter_id, int day_id);					//发放排名奖励

	//家族集愿屋
	void OnPublishGatherDream(Role * role, ItemID item_id, int type);			//发布集愿
	void OnHiddenGamePublishGatherDream(crossgameprotocal::HiddenGameGuildGatherDreamPublishReq * req);	//隐藏服发布集愿转发回原服
	void OnHiddenGamePublishGatherDreamRet(int is_publish_send, int result, int uuid, int item_id);		//原服发布结果返回隐藏服
	void OnGameHiddenPublishGatherDream(crossgameprotocal::GameHiddenGuildGatherDreamPublishRet *msg);	//原服发布集愿转发回隐藏服
	void OnGiftGatherDream(Role * role, int get_role_id);			//赠予集愿碎片
	void OnHiddenGameGiftGatherDream(crossgameprotocal::HiddenGameGuildGatherDreamGiftReq * req);		//隐藏服赠送集愿转发回原服
	void OnHiddenGameGiftGatherDreamRet(int result, SynGuildGatherDreamGiftInfo info);					//原服赠送结果返回隐藏服
	void OnGameHiddenGiftGatherDream(crossgameprotocal::GameHiddenGuildGatherDreamGiftRet *msg);		//原服赠送集愿转发回隐藏服
	void OnHiddenGameGiftGatherDreamAddRecord(crossgameprotocal::HiddenGameGuildGatherDreamGiftAddRecord * msg); //隐藏服转发回原服添加获赠|赠送记录
	void OnFetchGatherDream(Role * role);							//领取集愿碎片
	void OnHiddenGameFetchGatherDream(int uid);																	//隐藏服领取集愿请求转发回原服(此时通过会先修改数据)
	void OnHiddenGameFetchGatherDreamRet(int result, int uid, int index, ItemID item_id, short fetch_count);	//原服领取结果返回隐藏服
	void OnGameHiddenFetchGatherDream(crossgameprotocal::GameHiddenGuildGatherDreamFetchRet * msg);				//隐藏服接收原服结果后处理
	void OnHiddenGameFetchGatherDreamChange(crossgameprotocal::HiddenGameGuildGatherDreamFetchChange * msg);	//隐藏服转发回原服修改结果
	void OnRemoveMember(int uid);									//玩家退出或被踢出家族
	void OnUpdateRoleInfo(SynGatherDreamRecord info, int uid);		//玩家信息变更,同步修改集愿信息
	void SendGatherDreamList(Role * role);							//下发集愿列表
	void SendGatherDreamListToCross(int uid);						//下发集愿列表到跨服
	void SendGatherDreamSignleInfo(Role * role, int *index = NULL);	//下发单个集愿信息
	void SendGatherDreamSignleInfoToCross(int uid, int *index = NULL);	//下发单个集愿信息到跨服
	bool IsCanContOp(Role * role);
	//--------------------------------
	unsigned int GetPatriarchLastLoginTime();

	//------------------------------------------
	std::map<int, time_t> & GetIntervalLimitTimeMap() { return m_invite_interval_limit; }

	void GmSetNoLeader(); // gm让该家族没有族长
	void GmSetName(const char * name);
	void GmSetNotice(const char * notice);

	// -----------------------------------------------------
	// 家族活跃礼包及排行榜 备注 : 信息以主服为主 , 所有信息一定是在主服处理后同步到隐藏服
	// 添加活跃值
	void AddActiveVal(int _role_id ,  int _add_active_val);
	// 添加活跃值 - 主服调用
	void AddActiveValByGameWorld(int _role_id,  int _add_active_val);
	// 添加活跃值 - 隐藏服调用
	void AddActiveValByHidden(int _role_id,  int _add_active_val);
	unsigned int GetActiveLastChangeTimes() { return m_base_param.last_guild_active_change_times; }
	// 同步活跃值信息到隐藏服
	void SyncActiveInfoToHidden(GuildMember * _member);
	// 同步来至主服的活跃值信息
	void SyncActiveInfoByGameWorld(crossgameprotocal::GameHiddenGuildActiveInfoChange * _change);
	// 发送活跃   值信息
	void SendGuildActiveInfo(Role * _role);
	// 发送活跃值个人排行榜
	void SendGuildActivePersonRank(Role * _role);
	// 更新活跃值个人排行榜和公会成员信息 - 主服调用
	void UpdateActiveRankByGameWorld(int _role_id,  int _add_active_val);
	// 更新活跃值个人排行榜和公会成员信息 - 隐藏服调用[只是备用, 所有数据应该以主服的为主]
	void UpdateActiveRankByHidden(GuildMember * _member , crossgameprotocal::GameHiddenGuildActiveInfoChange * _change);
	// 初始化个人排行榜
	void InitWeekActivePersonRank();
	long long GetWeekActive() { return m_base_param.week_total_guild_active_val; }
	// 领取帮派活跃礼包
	void OnGuildActiveGetGiftReq(Role * _role, int _seq);
	// 日活跃度结算
	void GuildActiveDayActiveSettlement();
	// 周活跃度结算
	void GuildActiveWeekActivePersonRankSettlement();
	// 移除家族成员
	void OnRemoveMemberByGuildActivePersonRank(GuildMember * _member);
	// 发放周排行榜奖励
	void GetCrossGuildActiveRankReward(int _rank_index , int _rank_val);
	// 同步来至隐藏服的玩家活跃礼包领取信息
	void  OnHiddenGameRoleActiveGiftInfo(crossgameprotocal::HiddenGameRoleActiveGiftInfo * _info);
	// 测试用
	void GmGuildActiveSettlement();
	// -------------------------------------------------------------------
	// 7日活跃
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
	void FromHiddenQifu(int role_id, int add_value);//祈福值来源于隐藏服
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

	std::map<int, int> m_gather_dream_map;				//集愿映射  key:role_uid second:index
	WeekTaskRankList m_gwt_rank_list;			// 家族任务贡献度排行榜

	GuildMemberManager m_guild_member_manager;

	unsigned int m_next_save_time;								// 下次保存时间

	bool m_need_refresh_gwt_contribution_rank;					// 是否需要刷新家族任务排行榜
	time_t m_next_gwt_contribution_rank_refresh_timestamp;		// 下一次家族任务排行榜刷新的时间戳
	
	bool m_guild_boss_damage_rank_need_update;
	time_t m_guild_boss_damage_rank_next_update_time;
	std::vector<GuildBossRankItem>	m_guild_boss_damage_rank;

	std::map<int, time_t> m_invite_interval_limit;		//role_id->邀请时间 邀请间隔限制 ,10分钟内不能对同一个角色发出多次邀请

	bool m_is_dirty;
	time_t m_next_guild_sync_cross_time;

	int m_gm_flag;			//没什么用，只是用来测试弹劾

	std::map<int, unsigned int> m_gather_dream_timestamp_map;	//记录集愿跨服操作回原服验证

	// 周活跃排行榜  不需要记录到数据库 , 读取数据的时候重新排序一下就好
	// 隐藏服里只记录来源于主服的同步数据
	SkipList<WeekActiveInfo> m_week_active_rank;	

	//名人堂
	HallOfFameRankList m_battle_rank;
	unsigned int m_battle_refresh_tamp;
	HallOfFameRankList m_rich_rank;
	unsigned int m_rich_refresh_tamp;
	HallOfFameRankList m_kinder_rank;
	unsigned int m_kinder_refresh_tamp;

	//家族建造加速,先注释吧,不然策划又改就寄了
// 	std::set<UserID> m_building_speed_up_list;		//init之后获取成员的标记信息就可以
};		

#endif // __GUILD_HPP__