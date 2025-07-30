#ifndef __GUILD_MANAGER_HPP__
#define __GUILD_MANAGER_HPP__

#include <map>
#include <set>

#include "servercommon/serverdef.h"
#include "protocol/msgguild.hpp"
#include "servercommon/guilddef.hpp"

struct UserCacheNode;
class Guild;
class Role;
class GuildManager
{
	friend class RMIInitGuildBackObjectImpl;
	friend class RMICreateGuildBackObjectImpl;
	friend class RMIDismissGuildBackObjectImpl;

public:
	typedef std::map<GuildID, Guild*> GuildMap;
	typedef std::map<GuildID, Guild*>::iterator GuildMapIt;
	typedef std::map<GuildID, Guild*>::const_iterator GuildMapConstIt;

	static GuildManager & Instance();
	void Init(int ret, const AllGuildParam &guild_list, const AllGuildGathersDreamParam &guild_dream_list);
	void Update(unsigned long interval, time_t now_second);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnWeekChange();
	void OnResetData(unsigned int old_dayid, unsigned int now_dayid);

	void OnConnectCrossServerSucc();

	void SyncAllGuildToCross();
	void RequestSyncAllGuild(int server_id, bool is_all_server);
	void InitGuildBySync(int plat_type, int server_id, const AllGuildParam &guild_list, bool is_first, bool is_last);

	bool OnServerStart();
	void OnServerStop();

	void OnRoleLogin(Role* role);
	void OnRoleLogout(Role* role);
	void OnRoleLevelUp(Role* role, int level);
	void GuildRecommend(Role * role);

	//  创建 (创建家族之后是由客户端请求家族信息协议的)
	int CreateGuild(Role* role, Protocol::CSCreateGuild* req);
	// 退出
	int QuitGuild(Role* role, Protocol::CSQuitGuild* req);
	// 申请入族
	int ApplyForJoin(Role *role, GuildID guild_id);
	// 申请入族
	int ApplyForJoinOneClick(Role *role);
	// 审批入族申请
	int ApplyForJoinAck(Role *role, Protocol::CSApplyJoinGuildAck *join_ack);
	// 踢人
	int ExpelOut(Role* role, Protocol::CSGuildExpelOutReq* req);
	// 设置家族公告
	int SetGuildNotice(Role* role, Protocol::CSSetGuildNoticeReq* req);
	// 获取家族信息
	int GetGuildInfo(Role* role, Protocol::CSGetGuildList* req);
	// 强化训练
	int GuildTrainingUp(Role* role, int type);
	// 训练宠物
	int GuildTraining(Role* role, int pet_index, int places_index);
	// 清除所有训练中宠物
	void ClearAllGuildTraining(Role* role, time_t end_tamp);
	// 转让族长
	int OnGuildAbdicationOp(Role* role, Protocol::CSGuildAbdicationOp* req);
	// 弹劾
	int OnGuildImpeachmentOp(Role* role, Protocol::CSGuildImpeachmentOp* req);
	// 任命
	int OnGuildAppointmentOp(Role* role, Protocol::CSGuildAppointmentOp* op);
	// 商店购买
	int OnGuildShopBuy(Role* role, Protocol::CSGuildShopBuy* op);
	// 邀请加入家族
	void OnGuildInviteOther(Role * role, int target_uid);
	void OnGuildInviteReply(Role * role, int result);
	// 家族升级
	int OnLevelUp(Role* role);
	// 家族改名
	int GuildRename(Role* role, GuildName new_name);
	// 新成员进家族的祝福语句
	void NewbieJoinGuild(UserCacheNode* role,int uid, GuildID guild_id);
	// 家族族徽修改
	int GuildSetBanner(Role* role, GuildBanner new_banner);
	// 建造/拆除内政建筑
	int BuildingGuildBuild(Role* role, Protocol::CSGuildConstructReq* req);
	// 加速家族建造
	int SpeedUpBuilding(Role* role);

	Guild* GetGuild(GuildID guild_id);
	Guild* GetGuild(UserID user_id); // 暴力检索，效率偏低，仅用于可能获取不到Role的情况
	void RemoveGuild(GuildID guild_id);

	bool IsInvildName(GuildName name);
	void SendGuildList(Role* role);
	void SendGuildNotice(Role* role, GuildID guild_id);
	void SendPetTrainingInfo(Role* role);
	bool IsGuildLoadFinish() { return m_load_flag; }

	const GuildMap& GetGuildMap() { return m_guild_map; }
	void EraseApplyGuild(int uid, int guild_id);
	void DeleteApplyRecord(int uid);

	void GmGuildManagerHandler(int op_type, int guild_id);

	void OnSyncGuildChangeFromGame(int guild_id, int plat_type, int op_type, const AllGuildParam::GuildParam& param);

	void RemoveCreatingGuildRole(int role_id);

	void SendGuildBossChapterRankList(Role * role);
	void HiddenGameBossChapterRankReq(long long uuid);			//隐藏服向本服请求家族试炼进度排行
	void UpdateGuildBossChapterRank(GuildBossChapterRank *info);			//更新排行榜信息
	void GmDismissGuild(GuildID guild_id);
	void GmGuildActiveSettlement();

	void OnGameQueryHiddenIfSyncGuildSucceed(int plat_type, int server_id);
	void OnHiddenNoticeGameSyncGuildSucc();
	void OnDisconnectFromCrossServer();
	void OnOriginDisconnectFromCrossServer(int plat_type, int server_id);
private:
	Guild * AddGuild(const AllGuildParam::GuildParam &guild_param, bool is_create, bool replace_on_repeat = false, const GuildGatherDreamParam * guild_dream_param = NULL);
	int DismissGuild(GuildID guild_id);
	void SendMyApplyList(Role * role);
	bool IsBannerUsed(GuildBanner banner);
	void RefreshGuildBossChapterRank();		//重新刷新各个家族之间家族试炼的进度排行
	void QueryHiddenIfSyncGuildSucceed();
	bool IsServerSyncGuildSucc(int plat_type, int server_id);

	void AddServerToSyncSuccRecorder(int plat_type, int server_id);
	void RemoveAllServerToSyncSuccRecorder();
	void RemoveServerToSyncSuccRecorder(int plat_type, int server_id);

	GuildManager();
	~GuildManager();

	bool m_load_flag;
	GuildMap m_guild_map;
	std::vector<GuildBossChapterRank> m_guild_boss_chapter_rank_list;	//各个家族之间的家族试炼进度排行

	std::map<int, std::set<int> > m_user_apply_guild;
	std::map<int, time_t> m_send_limit_map;
	std::map<int, int> m_invitee_to_inviter_map;		// 邀请加入家族： 受邀者 <-> 邀请者 映射表

	std::set<int> m_creating_guild_role;					//正在创建工会的人，rmi 返回重这个列表删除
	std::set<int> m_guild_banner_hash_set;					//记录已用的族徽

	std::set<int> m_sync_guild_succ_recorder;

	unsigned int m_check_sync_succ_timestamp;
};

#endif // __GUILD_MANAGER_HPP__