#ifndef __ROLE_H__
#define __ROLE_H__

#include "character.h"
#include "inetworkmodule.h"
#include "servercommon/crossdef.hpp"
#include "other/rolestatusmanager.hpp"
#include "other/rolescenestatus.hpp"
#include "npc/npcpool.h"

class RoleInitParam;
class RoleOtherInitParam;
class RoleModuleManager;
class ItemListParam;
class SkillListParam;
class Team;
class FriendListParam;
class BlacklistsParam;
class MailListParam;
class AchievementListParam;
class RoleFirstKillParamList;
class Guild;
class RandActivityRoleParamList;
class RoleCommonSaveDataParam;
class RoleCourageChallengeLevelParamList;
class GodPrintItemListParam;
class NewCourageChallengeLevelParamList;

enum ADD_EXP_TYPE
{
	ADD_EXP_TYPE_NORMAL = 0,			//!< 0 默认原因
	ADD_EXP_TYPE_GM,					//!< 1 gm命令
	ADD_EXP_TYPE_TASK,					//!< 2 任务
	ADD_EXP_TYPE_KILL_NORMAL_MONSTER,	//!< 3 击杀普通怪物
	ADD_EXP_TYPE_QUESTION,				//!< 4 答题
	ADD_EXP_TYPE_MAIL,					//!< 5 邮件
	ADD_EXP_TYPE_NOREX_ITEM,			//!< 6 使用主动物品
	ADD_EXP_TYPE_SCHOOL_TASK,			//!< 7 学院任务
	ADD_EXP_TYPE_FIND_REWARD,			//!< 8 找回奖励
	ADD_EXP_TYPE_TASK_CHAIN,			//!< 9 任务链
	ADD_EXP_TYPE_COLOSSEUM,				//!< 10 竞技场
	ADD_EXP_TYPE_QUESTION_BUY_EXP,		//!< 11 答题买经验
	ADD_EXP_TYPE_TRANSACTION_TASK,		//!< 12 购买任务 - 丝绸之路
	ADD_EXP_TYPE_FANTASY_FB,			//!< 13 日常活动 - 四象幻境副本
	ADD_EXP_TYPE_TEAM_FIGHT,			//!< 14 小队竞技
	ADD_EXP_TYPE_EXP_ITEM,				//!< 15 经验丹
	ADD_EXP_TYPE_FB_GOLD_BOX,			//!< 16 副本金宝箱
	ADD_EXP_TYPE_EXP_RETRIEVE,			//!< 17 经验回收
	ADD_EXP_TYPE_JING_YAN_BIAO_SHENG,	//!< 18 经验回收
	ADD_EXP_TYPE_CRYSTAL_FB,			//!< 19 水晶副本
	ADD_EXP_TYPE_ANECDOTE_TASK,			//!< 20 奇闻任务
	ADD_EXP_TYPE_MONPOLY_GIFT,			//!< 21 牛运当头
	ADD_EXP_TYPE_LEVEL_COMPLEMENT,		//!< 22 等级助力
	ADD_EXP_TYPE_ONLY_FIGHT,			//!< 23 以一敌百
	ADD_EXP_TYPE_GUILD_FIGHT,			//!< 24 家族大乱斗
	ADD_EXP_TYPE_CHIEF_ELECTION,		//!< 25 首席竞选
	ADD_EXP_TYPE_HELP_OTHERS,			//!< 26 义胆助人
	ADD_EXP_TYPE_ACTIVE_SYSTEM,			//!< 27 活跃系统
	ADD_EXP_TYPE_REALM_BREAK,			//!< 28 境界系统时间间隔经验增长(客户端会屏蔽飘字)
	ADD_EXP_TYPE_REALM_BREAK_BREAK,		//!< 29 境界系统突破
	ADD_EXP_TYPE_MATERIAL,				//!< 30 玉虚境(材料副本)
	ADD_EXP_TYPE_PET_MONOPOLY,			//!< 31 灵宠秘境
	ADD_EXP_TYPE_ROLE_JOIN_ACTIVITY_REWARD,	//!< 32 角色参与显示活动奖励
	ADD_EXP_TYPE_ACHIEVEMENT_PROG,		//!< 33 成就阶段
	ADD_EXP_TYPE_GUILD_ACTIVE_GIFT,		//!< 34 家族活跃礼包
	ADD_EXP_TYPE_PET_GOD_FB,			//!< 35 神印之地
	ADD_EXP_TYPE_DREAM_NOTES_CHALLENGE, //!< 36 梦渊笔录
	ADD_EXP_TYPE_SUMMER_LI_LIAN,		//!< 37 初夏历练
	ADD_EXP_TYPE_MAZE_SUMMER,			//!< 38 夏日迷宫
	ADD_EXP_TYPE_HELP_OTHERS_2,			//!< 39 义胆助人2（桃园一三事）
	ADD_EXP_TYPE_REALM_BREAK_FIGHT,		//!< 40 境界-心灵之境(客户端屏蔽飘字)
	ADD_EXP_TYPE_SHAN_HAI_QUESTION,		//!< 41 山海问答
	ADD_EXP_TYPE_FATE_ENTANGLEMENT,		//!< 42 山海缘起	
	ADD_EXP_TYPE_COURSE_OF_GROWTH,		//!< 43 成就(历程/目标小任务)
	ADD_EXP_TYPE_WAN_LING_FB,			//!< 44 万灵话本
	ADD_EXP_TYPE_YUNZE_ER_SAN_SHI,		//!< 45 云泽二三事
	ADD_EXP_TYPE_NEW_LEVEL_COMPLEMENT,	//!< 46 新等级助力
	ADD_EXP_TYPE_SHI_TU_CHUAN_GONG,		//!< 47 师徒传功
	ADD_EXP_TYPE_ROLE_FLY_UP_FINISH,		//!< 48 角色完成飞升重算等级
	ADD_EXP_TYPE_ROLE_ONE_CLICK_FINISH,		//!< 49 一键完成活跃任务
};

enum REDUCE_EXP_TYPE
{
	REDUCE_EXP_TYPE_NORMAL = 0,			//!< 0 默认原因
	REDUCE_EXP_TYPE_GM,					//!< 1 gm命令
	REDUCE_EXP_TYPE_DUEL,				//!< 2 决斗
};

class Team; 
class Role : public Character
{
public:
	enum AUTHORITY_TYPE
	{
		AUTHORITY_TYPE_INVALID = 0,									// 无任何权限
		AUTHORITY_TYPE_GUIDER,										// 新手指导员
		AUTHORITY_TYPE_GM,											// GM
		AUTHORITY_TYPE_TEST,										// 测试帐号（内部号）
		AUTHORITY_TYPE_MAX,
	};

public:
	Role();
	Role(const GSNetID &_netid, const GameName _name);
	virtual ~Role();

	void				Init(int plat_spid, PlatName pname, const UserID &user_id, RoleModuleManager* role_module_manager,
							 const RoleInitParam &param,
							 const RoleOtherInitParam &other_p,
							 const ItemListParam& item_list_param,
							 const SkillListParam& skill_list_param,
							 const MailListParam& maillist_param,
							 const RoleFirstKillParamList& first_kill_list,
							 const RandActivityRoleParamList & rand_activity_role_data_list,
							 const RoleCommonSaveDataParam & role_common_save_data_list, 
							 const RoleCourageChallengeLevelParamList & role_courage_challenge_list,
							 const GodPrintItemListParam & god_print_item_list,
							 const NewCourageChallengeLevelParamList & new_courage_challenge_list);

	void				OnInitFinish();

	void				GetPlatName(PlatName pname);
	inline const char*	GetPlatName() { return m_plat_name; }
	int					GetAccountUserNum() { return m_account_user_num; }

	inline const		GSNetID& GetNetId() const { return m_netid; }
	inline const		BGNetID& GetBGNetID() const { return m_bg_netid; }

	inline int			GetUID() const { return m_uid; }
	inline const		UserID& GetUserId() const { return m_user_id; }
	IP					GetIP() const { return m_userip; }
	int					GetPlatSpid() { return m_plat_spid; }

	UniqueUserID		GetUniqueUserID();
	long long			GetUUID();
	static UniqueUserID GetUniqueUserID(const UserID& user_id);

	void				OnRecvFromClient(unsigned long game_time, IP user_ip);

	void				SetForceKickDelay() { m_isforce_kickdelay = true; }
	bool				IsForceKickDelay() { return m_isforce_kickdelay; }

	void				OnLoginBroadCast(bool is_gm = false);
	void				OnLogin(long long last_save_time, IP userip, int plat_spid);
	void				OnLogout();
	bool				Save(bool is_logout = false, int* target_scene_id = NULL, Posi* target_pos = NULL);
	bool				IsRoleSaveDBSessionDisconnect();

	void				GetRoleInitParam(RoleInitParam *param);
	void				TodayOnlineStatiTime(unsigned int now);
	unsigned int		GetTodayOnlineTime() { return m_today_online_time; }

	virtual void		OnEnterScene();
	virtual void		OnLeaveScene();
	virtual void		OnAOIEnter(ObjID obj_id);
	virtual void		OnAOILeave(ObjID obj_id);
	UInt32				GetAOIHandle() const { return m_aoi_handle; }

	void				OnAOIEnterRole(Obj* obj);
	void				OnAOIEnterNPCMonster(Obj* obj);
	void				OnAOIEnterNPCGathers(Obj* obj);
	void				OnAOIEnterSpecialTransport(Obj* obj);
	void				OnAOIEnterTreasureChest(Obj* obj);
	void				OnAOIEnterNPCFuncNpc(Obj * obj);
	void				OnAOIEnterNPCChosed(Obj * obj);
	void				OnAOIEnterRobot(Obj* obj);
	void				OnAOIEnterXunYouCar(Obj* obj);
	void				OnAOIEnterPeakTokenObj(Obj* obj);
	void				OnAOIEnterTuZiKuaiPaoTuZi(Obj * obj);

	int					GetRoleBaseAva();

	inline long long	GetAvatarTimestamp() const { return m_avatar_timestamp; }
	inline short		GetAvatarType() const { return m_avatar_type; }
	inline short		GetSex() const { return SEX_ARRAY[(int)m_avatar_type]; }
	inline short		GetHeadshotID() const { return m_headshot_id; }
	inline int			GetLevel() const { return m_level; }
	inline int			GetTopLevel() const { return m_top_level; }
	inline unsigned int GetNextTopLevelRefreshTimestamp()  { return m_next_top_level_refresh_timestamp; }
	inline int			GetVipLevel() const { return 0; }
	inline int          GetProfession() const { return m_profession; }
	inline int			GetAdvanceTimes() const { return m_profession % PROFESSION_BASE; }
	inline int          GetBaseProfession() const { return m_profession / PROFESSION_BASE * PROFESSION_BASE; }
	inline char			GetAppearanceProf() const { return m_profession / PROFESSION_BASE; }
	int					SetProfession(int new_profession, const char* reason);
	inline char			GetAuthorityType() const { return m_authority_type; }
	bool				SetAuthorityType(char type, bool save_now);
	inline char			GetColor() const { return m_color; }
	void				SetColor(char color) { m_color = color; }
	unsigned int        GetLastOnlineTimestamp() { return m_last_online_stati_time; }

	int					GetLastScene(Posi *pos) { if (NULL != pos) *pos = m_last_scene_pos; return m_last_scene_id; }

	inline long long	GetExp() const { return m_exp; }
	inline long long	GetMaxExp() const { return m_max_exp; }
	long long			AddExp(long long add_exp, short add_reason = ADD_EXP_TYPE_NORMAL, const char* reason_str = NULL, bool is_extra = true);
	long long			ReduceExp(long long reduce_exp, short reduce_reason = REDUCE_EXP_TYPE_NORMAL, const char* reason_str = NULL);
	void				OnRoleResetTopLevelAndCheck();		//重置巅峰等级,并检测当前经验
	void				GmSetRoleLevel(int to_level);
	void				GmSetRoleTopLevel(int top_level);
	void				GmRetrieveExp(long long retrieve_exp);
	void				SetTopLevelNextRefreshTimestamp(unsigned int next_top_level_refresh_timestamp) { m_next_top_level_refresh_timestamp = next_top_level_refresh_timestamp; }

	virtual	void		Update(unsigned long interval, time_t now_second, unsigned int now_dayid);

	void				OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void				OnWeekChange();
	void				OnTwoWeekChange(int week_falg);
	void				OnMonthChange();

	void				OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);
	void				OnResetModuleDataWeek();
	void				OnResetModuleDataMonth();
	void				RecalNextResetDataTimeStamp();

	void				SendRoleInfo();
	void				SendRoleAuthorityInfo();
	void				SendAllInfo();
	
	void				SetRoleModuleManager(RoleModuleManager* role_module_manager) { m_role_module_manager = role_module_manager; }
	RoleModuleManager*	GetRoleModuleManager() const { return m_role_module_manager; }

	bool				ResetPos(Axis x, Axis y, short reset_type = 0);

	virtual bool		Move(Axis _x, Axis _y, Dir _dir, Scalar distance, short height, bool check_valid = false);

	void				NoticeNum(int notice_num);
	void				NoticeItemLack(int item_id, int nun);

	bool				RoleChangeLevel(int to_level, bool is_role_level = true);

	unsigned int		GetLastMsgFromClientTimestamp() { return m_last_msg_from_client_timestamp; }
	unsigned long		GetLastMsgFromClientTime() { return m_last_msg_from_client_time; }

	Guild *				GetGuild();
	GuildID				GetGuildID();
	void				SetGuildID(GuildID id, bool is_join_guild = false);
	// 获取家族贡献
	int					GetGuildTotalGongXian();
	void				SetGuildPostion(int positon);
	int					GetGuildPostion();

	RoleStatusManager*	GetRoleStatusManager() { return &m_role_status_manager; }
	RoleSceneStatus*	GetSceneStatus() { return &m_scene_status; }

	bool				InTeam();			//自己是否在队伍里
	bool				IsLeader();			//自己是不是队长
	bool				IsFollow();			//自己是否跟随状态，如果不是，则是暂离；
	bool				IsMember(int uid);	//uid这个角色是否在自己队伍里
	Team*				GetMyTeam(bool is_auto_create = false);	// 获取队伍
	bool				CanJoinTeam(bool is_notice = true);		// 能否组队
	void				SetTeam(Team* team) { m_team = team; }
	bool				CanRoleStartFight(int battle_mode);
	
	/**
	 * @brief			获取队员uid
	 * @return			获取到的队员人数
	 * @param uid_list	队员的uid
	 * @param oneself	是否包括自己
	 * @param follow	是否需要跟随
	 * @param online	是否需要在线
	 */
	int					GetTeamMember(int *uid_list, int max_count, bool oneself, bool follow, bool online);
	int					GetTeamCount();
	long long			GetTeamExpUpCountByMineMonster(long long total_exp);
	long long			GetGuaJiExpAttenuation(long long total_exp);

	bool				CanEnterFB(bool is_notice = false);

	bool				RoleChangeCareer(int to_profession, int avatar_type, int color);
	void				SendProfExtraInfo();

	void				UpdateCacheNode();

	void				RoleSetBattlePosition(char position); // 设置战斗时自己站后排还是前排  0:后排  1:前排
	char				GetBattlePosition() const;
	void				SendBattlePosition();

	void				SetEncounterMonsterFlag(bool is_encounter); // 设置是否遭遇暗怪
	void				RefreshEncounterInterval();
	void				RestoreLastEncounterMonsterFlag();
	void				SetPatrolFlag(bool is_patrol);

	void				SetBattleAuto(int is_auto);	
	void				SetBattleAutoMove(bool is_first_move, short move_type, UInt16 skill_id, short skill_level, bool is_check, bool is_notice, bool is_update_pet_auto_move = false);
	void				SetBattleAutoAfterInitialFight();
	void				SetBattleAutoOnInitialFightDisabled();
	void				SendBattleAutoInfo();
	void				OnRoleSkillUplevel(UInt16 skill_id, short skill_level);
	void				OnFightPetSkillLevelChange(UInt16 skill_id, short skill_level);

	void				OnResetNameReq(GameName new_name);
	bool				ResetName(GameName name);
	void				OnResetNameRetConsumeItem();

	//聊天
	void				ForbidTalk(time_t forbid_time, const char *reason, const char *last_msg, int forbid_talk_type);

	// 采集
	void				OnStartsGather(int seq_id,int is_auto_gather);
	int					GetGatherSpeedupPercent();
	void				UpdateGather(time_t now);
	void				OnStopGathering(bool is_succ);
	void				OnEndGather(int seq_id, ObjID gather_id, int is_auto_gather);
	bool				IsAutoGather() { return m_is_auto_gather; }
	int					GetGatherSeqId() { return m_gather_seq_id; }
	bool				OnGiveGatherReward(const ServerNPCGatherParam & gather_param);

	//钓鱼
	void				OnStartFish(int fish_fb_seq);
	void				OnStopFishing(int reason, int param1 = 0, int param2 = 0);
	void				OnEndFishing(int fish_fb_seq);

	bool				OnGiveGatherReward(ItemConfigData reward, bool is_gather, ARG_OUT int * out_item_num = NULL);
	void				OnSendLifeSkillNotice(int item_id, int item_num, bool is_double, bool is_gather, int new_life_skill_type);

	void				ForbidRole(const char* reason, int forbid_time);

	void *				operator new(size_t c);
	void				operator delete(void *m);
	int GetAuthority();

	int					GetWildBossRewardTimes();
	bool				HasWildBossRewardTimes();
	void				SetWildBossRewardTimes(int times);
	void				AddPosData(PosData data);
	const PosData*		GetPosData();

	void				ChangeAppearance(short avatar_type, short color);
	void				ChangeHeadshot(short headshot_id);
	void				SendChangeAppearanceNotice();
	void				NotifyMyStatusToOthers();

	bool				IsInCross();
	void				SetCurrencyType(int currency_type);
	int					GetCurrencyType();
	void				SetClientShowSpid(int spid);
	int					GetClientShowSpid();

	void ReturnToOriginServer();

	unsigned int GetLastCheckReallyInBattleTime() const { return m_last_check_really_in_battle_time; }
	void OnCheckReallyInBattle(unsigned int now);

	bool IsProfSuitable(int prof);

	int	GetBattleIndex();

	void GmRoleOrder(int type, int param);

	long long GetRoleCreateTime() { return m_create_time; }
	void PrintToScreen(const char * sentence);
	inline time_t GetForbidTalkTime() const { return m_forbid_talk_time; }
	inline int GetForbidTalkType() const { return m_forbid_talk_type; }
	bool IsForbidTalk();
	int GetCapability();
	//老版综合战力(主角+出战伙伴+出战宠物)(max:1+4+1)
	int GetCapabilityWithFightPetAndPartner();		
	//新版综合战力(主角+出战伙伴+出战/待战宠物)(max:1+4+5)
	int GetCapabilityWithFight();

	void SendServerTimeDate();
	void SendServerTime(char is_server_driving_send = 1);
	void LoginOriginGameNoticeHidden();
	void LogoutOriginGameNoticeHidden();

	const UniqueServerID& GetUniqueServerID() const { return m_unique_server_id; }

	void AddSomeVisibleRole(int add_num);
	void RemoveSomeVisibleRole(int remove_num);
	void OnVisibleRoleNumSettingChange(int cur_setting_num);
	time_t &GetChatGroupInv() { return pre_inv_chat_group_tamp; }
	bool IsRoleInSight(Role* other_role);

	MsgRoleBaseInfo GetMsgRoleBaseInfo() const;
	
	void SendGuildActiveInfo(int _seq = -1);

	inline long long GetRoleCheckUniqueId() const { return m_role_unique_id_in_scene; }

	void LogActive(int log_avtive_type, int param1 = 0, int param2 = 0, int param3 = 0, int param4 = 0);

	bool IsGM() const;
	bool IsFromDev() const;
	
protected:
	virtual bool CheckWaiGuaMove();

	UserID				m_user_id;										// 整个游戏唯一的用户ID
	int					m_uid;
	IP					m_userip;										// 用户的IP
	GSNetID				m_netid;										// 网络层id
	BGNetID				m_bg_netid;										// 用gateway index来标识的网络层id (用于其他进程直接下发协议给玩家的场合）
	int					m_plat_spid;									// 平台ID
	PlatName			m_plat_name;									// 平台名
	int					m_account_user_num;								// 该账号下第几个角色[1, 5]

	unsigned long		m_last_msg_from_client_time;					// 上次客户端消息时间
	unsigned int		m_last_msg_from_client_timestamp;				// 上次客户端消息时间戳

	bool				m_isforce_kickdelay;							// 是否要延迟踢这个玩家，主要是顶号的时候用

	Posi				m_birth_pos;									// 出生点
	UInt32				m_aoi_handle;									// AOI句柄

	int					m_last_scene_id;								// 上一个场景id
	Posi				m_last_scene_pos;								// 上一个场景位置

	char				m_authority_type;								// 权限
	char				m_color;										// 颜色
	
	short				m_avatar_type;	
	short				m_headshot_id;
	
	int					m_level;										// 等级
	int					m_profession;									// 职业
	int					m_top_level;									// 巅峰等级
	unsigned int        m_next_top_level_refresh_timestamp;				// 巅峰等级重置时间戳

	long long			m_avatar_timestamp;								// 玩家自上传头像时间戳

	long long			m_exp;											// 经验
	long long			m_max_exp;										// 最大经验

	unsigned int		m_last_online_dayid;							// 最后一天在线ID
	unsigned int		m_next_reset_module_data_timestamp;				// 下次重置角色数据的时间戳
	unsigned int		m_last_reset_module_data_timestamp;				// 最后一天重置角色数据的时间戳
	unsigned int		m_next_save_time;								// 下次保存时间	

	int					m_online_time;									// 在线时长
	int					m_today_online_time;							// 今日在线时长
	int					m_lastday_online_time;							// 昨日在线时长
	long long			m_create_time;									// 角色创建时间
	unsigned int		m_last_online_stati_time;						// 上次计算在线时长的时间
	unsigned int		m_last_today_online_stati_time;					// 上次计算当日在线时长的时间

	unsigned int 		m_login_timestamp;								// 登录时间戳
	int					m_guild_positon;

	long long			m_role_module_mgr_barrier_checker_1;			// 
	RoleModuleManager*	m_role_module_manager;							// 角色模块管理器
	long long			m_role_module_mgr_barrier_checker_2;			// 

	RoleStatusManager   m_role_status_manager;							// 角色状态管理器
	RoleSceneStatus		m_scene_status;									// 角色场景状态

	int					m_gather_seq_id;								// 采集物 seq_id
	unsigned int		m_gather_need_timestamp;						// 采集所需时间

	int					m_fish_fb_seq;									// 所在钓鱼的副本索引
	unsigned int		m_fish_need_timestamp;							// 钓鱼所需时间

	PosData m_pos_data;

	unsigned int		m_last_check_really_in_battle_time;				// 上次检查玩家是否确实在战斗中的时间
	bool				m_is_testing;									// 测试标识

	Team*				m_team;
	bool				m_is_auto_gather;								// 自动采集状态
	time_t				m_forbid_talk_time;								// 禁言时间记录
	int					m_forbid_talk_type;								// 禁言类型\see FORBID_TYPE_MAX

	UniqueServerID      m_unique_server_id;

	std::map<int, int>	m_visible_role_map;	// 可见角色列表

	time_t pre_inv_chat_group_tamp;	// 群组--玩家邀请他人时间间隔（临时记录）

	long long m_role_unique_id_in_scene;	// 用于追查m_role_module_mgr为空的BUG
};

#endif