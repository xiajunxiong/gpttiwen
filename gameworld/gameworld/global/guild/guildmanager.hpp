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

	//  ���� (��������֮�����ɿͻ������������ϢЭ���)
	int CreateGuild(Role* role, Protocol::CSCreateGuild* req);
	// �˳�
	int QuitGuild(Role* role, Protocol::CSQuitGuild* req);
	// ��������
	int ApplyForJoin(Role *role, GuildID guild_id);
	// ��������
	int ApplyForJoinOneClick(Role *role);
	// ������������
	int ApplyForJoinAck(Role *role, Protocol::CSApplyJoinGuildAck *join_ack);
	// ����
	int ExpelOut(Role* role, Protocol::CSGuildExpelOutReq* req);
	// ���ü��幫��
	int SetGuildNotice(Role* role, Protocol::CSSetGuildNoticeReq* req);
	// ��ȡ������Ϣ
	int GetGuildInfo(Role* role, Protocol::CSGetGuildList* req);
	// ǿ��ѵ��
	int GuildTrainingUp(Role* role, int type);
	// ѵ������
	int GuildTraining(Role* role, int pet_index, int places_index);
	// �������ѵ���г���
	void ClearAllGuildTraining(Role* role, time_t end_tamp);
	// ת���峤
	int OnGuildAbdicationOp(Role* role, Protocol::CSGuildAbdicationOp* req);
	// ����
	int OnGuildImpeachmentOp(Role* role, Protocol::CSGuildImpeachmentOp* req);
	// ����
	int OnGuildAppointmentOp(Role* role, Protocol::CSGuildAppointmentOp* op);
	// �̵깺��
	int OnGuildShopBuy(Role* role, Protocol::CSGuildShopBuy* op);
	// ����������
	void OnGuildInviteOther(Role * role, int target_uid);
	void OnGuildInviteReply(Role * role, int result);
	// ��������
	int OnLevelUp(Role* role);
	// �������
	int GuildRename(Role* role, GuildName new_name);
	// �³�Ա�������ף�����
	void NewbieJoinGuild(UserCacheNode* role,int uid, GuildID guild_id);
	// ��������޸�
	int GuildSetBanner(Role* role, GuildBanner new_banner);
	// ����/�����������
	int BuildingGuildBuild(Role* role, Protocol::CSGuildConstructReq* req);
	// ���ټ��彨��
	int SpeedUpBuilding(Role* role);

	Guild* GetGuild(GuildID guild_id);
	Guild* GetGuild(UserID user_id); // ����������Ч��ƫ�ͣ������ڿ��ܻ�ȡ����Role�����
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
	void HiddenGameBossChapterRankReq(long long uuid);			//���ط��򱾷��������������������
	void UpdateGuildBossChapterRank(GuildBossChapterRank *info);			//�������а���Ϣ
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
	void RefreshGuildBossChapterRank();		//����ˢ�¸�������֮����������Ľ�������
	void QueryHiddenIfSyncGuildSucceed();
	bool IsServerSyncGuildSucc(int plat_type, int server_id);

	void AddServerToSyncSuccRecorder(int plat_type, int server_id);
	void RemoveAllServerToSyncSuccRecorder();
	void RemoveServerToSyncSuccRecorder(int plat_type, int server_id);

	GuildManager();
	~GuildManager();

	bool m_load_flag;
	GuildMap m_guild_map;
	std::vector<GuildBossChapterRank> m_guild_boss_chapter_rank_list;	//��������֮��ļ���������������

	std::map<int, std::set<int> > m_user_apply_guild;
	std::map<int, time_t> m_send_limit_map;
	std::map<int, int> m_invitee_to_inviter_map;		// ���������壺 ������ <-> ������ ӳ���

	std::set<int> m_creating_guild_role;					//���ڴ���������ˣ�rmi ����������б�ɾ��
	std::set<int> m_guild_banner_hash_set;					//��¼���õ����

	std::set<int> m_sync_guild_succ_recorder;

	unsigned int m_check_sync_succ_timestamp;
};

#endif // __GUILD_MANAGER_HPP__