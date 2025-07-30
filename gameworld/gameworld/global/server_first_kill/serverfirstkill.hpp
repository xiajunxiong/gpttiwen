#ifndef __SERVER_FIRST_KILL_HPP__
#define __SERVER_FIRST_KILL_HPP__

#include "servercommon/struct/global/serverfirstkilldef.h"
#include "global/server_first_kill/serverfirstkillconfig.hpp"
#include "other/rolemodulemanager.hpp"
#include "obj/character/role.h"
#include "gameworld/protocol/msgfirstkill.h"
#include "servercommon/internalprotocal/battleprotocol.h"
#include "servercommon/internalprotocal/crossgameprotocal.h"

struct FirstKillDelayInfo
{
	FirstKillDelayInfo() : delay_timestamp(0), video_length(0), video_buffer(NULL)
	{
		info_param.Reset();
		info.Reset();
	}

	unsigned int delay_timestamp;
	int video_length;
	char *video_buffer;
	BattleVideoParam info_param;
	ServerFirstKillParam::ServerFirstKillSaveEntry info;
};

class ServerFirstKill
{
	friend class RMIInitServerFirstKillBackObjectImpl;

public:
	static ServerFirstKill & Instance();

	void OnServerStart();
	void OnServerStop();
	bool LoadServerFirstKillData(long long id_from);
	
	void Update(time_t now_second);

	bool IsLoadFinish() { return m_load_flag; }

	void Init(const ServerFirstKillParam &param);
	void GetInitParam(ServerFirstKillParam * param);
	void ClearDirtyMark(ServerFirstKillParam *param);

	void OnUserResetName(const UserID &user_id, GameName name);
	void ClearFirstKillItem(int type_index, int index);

	bool SendFirstKillInfo(Role * role, int type, int level_group);
	void SendFirstKillInfoToHidden(long long  uuid, int type, int level_group);
	void SendFirstKillInfoToGame(int uid, int type, int level_group);

	int OnGiveFirstKillReward(battlegameprotocol::BattleGameFightResultAck::RoleList * role_list, int round_num, int monster_group_id, int battle_mode, int record_score, long long record_file_id);
	void OnGameHiddenFirstKillReq(crossgameprotocal::GameHiddenFirstKillReq & data_info);

	bool IsFirstKill(int monster_group_id, bool is_judge = true);
	bool GetChangeFlag() { return change_flag; }
	long long GetBattleVideoRecordFileIDByMonsterGroupID(int monster_group_id);

	void OnServerLoadDataFinish();
private:
	ServerFirstKill();
	~ServerFirstKill();

	void Save(const ServerFirstKillParam *param = NULL);

	bool IsValidIndex(int index) { return index >= 0 && index < FIRST_KILL_BOSS_TYPE_MAX_NUM; }
	void GetSendInfo(Protocol::SCServerFirstKillInfo & info, int type_index, int index);
	void CheckConfigIsChange(int type_index, int index, const ServerFirstKillParam & param);
	void CheckInfoDelay(unsigned int now_second);
	void SendFirstKillReward(FirstKillDelayInfo & data_item);

	void LoadServerFirstKillSucc();

	ServerFirstKillParam server_param;

	time_t m_first_kill_next_save_time;
	typedef std::map<int, std::vector<FirstKillDelayInfo> > FirstKillInfoMap;
	FirstKillInfoMap m_info_delay_map;	//暂存首杀信息，延迟3秒确认最终击杀者	key:monster_group_id

	bool m_dirty_mark[FIRST_KILL_TYPE_MAX][FIRST_KILL_BOSS_TYPE_MAX_NUM];
	bool m_old_state[FIRST_KILL_TYPE_MAX][FIRST_KILL_BOSS_TYPE_MAX_NUM];
	bool change_flag;
	bool m_load_flag;
};

#endif