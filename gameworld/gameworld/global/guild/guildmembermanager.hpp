#ifndef __GUILD_MEMBER_MANAGER_HPP__
#define __GUILD_MEMBER_MANAGER_HPP__

#include <map>
#include <vector>
#include "servercommon/guilddef.hpp"
#include "servercommon/maildef.hpp"
#include "protocol/msgguild.hpp"

class Guild;
class GuildMember;
class Role;
bool FilterZuZhang(GuildMember * guild_member, void * func_param);  //不发给族长
bool FilterJoinHonor(GuildMember * guild_member, void * func_param);		//判断是否参与, func_param: true-判断参与了的, false-判断未参与的

class GuildMemberManager
{
public:
	typedef bool(*GuildMemberFilterFunc)(GuildMember *guild_member, void *func_param);
public:
	GuildMemberManager();
	~GuildMemberManager();

	void SetGuild(Guild *guild) { m_guild = guild; }
	Guild * GetGuild() { return m_guild; }

	void Init(const GuildMemberParam &member_param, bool is_sync_from_origin = false);
	void GetChangeMemberList(GuildMemberParam *member_param);
	void GetMemberList(GuildMemberParam *member_param);
	void ClearDirtyMark();
	void Dismiss();
	void Update(time_t now_sec);
	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	void OnWeekChange();

	void OnGuildLevelUp(int new_guild_level);
	bool AddApplyFor(Role * role, bool is_notice = true);
	bool ApplyJoinAck(const UserID &user_id, int result);
	void SendApplyList(Role* role);

	GuildMember * AddMember(UserID user_id, GameName name, int post = GUILD_POST_MEMBER, int baodi_reward_flag = 0, char rank_fetch_flag = 0);
	int RemoveMember(UserID user_id , int reason);
	GuildMember * GetMember(UserID user_id, int *member_index = NULL);
	GuildMember* GetMemberByIndex(int member_index);
	GuildMember * GetPatriarchMember(int *member_index = NULL);
	std::vector<GuildMember *> GetAllGuildMember();

	int AddGongXian(const UserID& user_id, int value, const char* reason);

	int GetPostCount(int post);
	int SetPostCount(int post, int add_value);

	inline int GetMemberCount() const { return m_member_count; }
	bool IsHavePlace();
	UserID GetPatriarchUserID() { return m_patriarch_user_id; }
	int GetPatriarchUid() { return UidToInt(m_patriarch_user_id); }
	int GetPatriarchLevel() { return m_patriarch_level; }
	bool GetPatriarchName(GameName name);
	void SetPatriarchUserID(UserID user_id) { m_patriarch_user_id = user_id; }
	void SetPatriarchLevel(int level) { m_patriarch_level = level; }
	void SetPatriarchName(GameName name);

	void SendInfoToAllMember(const char* info, int len);
	void SendSerializeMsgToAllMember(IMessageSerializer* info, int len);
	void SendMailToAllMember(int reason, const MailContentParam &contentparam, int mail_kind = MAIL_TYPE_SYSTEM, GuildMemberFilterFunc guild_member_filter = NULL, void * func_param = NULL);
	void SendTitleToAllMember(int title_id, const char* reason);
	void SendGuildMemberInfo(Role* role);
	void SendGuildMemberInfoChange(int change_role_id);
	void SendInfoToAllMemberFromHidden(const char* info, int len);
	void SendInfoToAllMemberInfo(const char* info, int length);
	void ClearAllMemberHonorBattleFlag();

	void SetDirtyMark(int index);

	void OnChat(const char* buff, int total_len, bool need_sync = false);
	
	bool IsGuildMember(int role_id);

	// Gm命令
	void GmGuildMemberOrder(int op_type);

	//家族荣耀战
	void UpdateGuildHonorTitleId(int rank);
	void UpdateGuildHonorTitleIdFromHidden();

	//家族大乱斗
	void UpdateGuildFightTitle();

	//家族试炼
	void AddGuildBossTotalDamage(int role_id, long long add_value,int boss_index);
	void UpdateGuildBossHighestDamage(int role_id, long long total_damage, ARG_OUT bool* need_update_rank);
	std::vector<GuildBossRankItem> GetMemberGuildBossItem();
	void SendGuildBaoDiReward(int chapter_id, int boss_index);

	void OnMemberInfoChange(const UserID& user_id);
	void OnGameSyncMemberChange(int op_type, int param1, const GuildMemberParam::Member & member_param);
	void OnHiddenSyncMemberChange(int op_type, int param1, const GuildMemberParam::Member & member_param);

	//家族加速建造
	void SendSPUPInfoToAllMember(Protocol::SCGuildSpeedUpInfo msg);

	void GameToHiddenSyncGuildMemberData(int role_id);
private:
	void SendApplyListToLegalPost();
	void OnAddMember(const GuildMemberParam::Member& member, int index);
	void OnRemoveMember(int uid, int index);
	void OnMemberInfoChange(const GuildMemberParam::Member& member, int online_state);

	//禁止拷贝复制（请不要公有化以下这两个函数， 除非做好深拷贝， 因为在析构时会delete掉某些成员变量）
	GuildMemberManager(const GuildMemberManager&);
	GuildMemberManager& operator=(const GuildMemberManager &);

	Guild *m_guild;

	UserID m_patriarch_user_id;
	GameName m_patriarch_name;
	int m_patriarch_level;
	int m_post_count[GUILD_POST_MAX];
	int m_member_count;
	GuildMember *m_member_list[GUILD_MAX_MEMBER_COUNT];

	bool m_dirty_mark[GUILD_MAX_MEMBER_COUNT];
	bool m_old_state[GUILD_MAX_MEMBER_COUNT];
	bool m_change_flag;

	typedef std::map<UserID, int> UserIDIndexMap;
	typedef std::map<UserID, int>::iterator UserIDIndexMapIt;
	UserIDIndexMap m_userid_index_map;

	typedef std::map<UserID, GuildApplyUserInfo> UserIDApplyMap;
	typedef std::map<UserID, GuildApplyUserInfo>::iterator UserIDApplyMapIt;
	UserIDApplyMap m_apply_map;
};

#endif // __GUILD_MEMBER_MANAGER_HPP__

