#ifndef __BATTLE_VIDEO_MANAGER_HPP__
#define __BATTLE_VIDEO_MANAGER_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/struct/battle/battle_video_def.hpp"
#include <map>

struct BattleVideoInfo
{
	BattleVideoInfo() : isdirty(false), oldstate(false), video_len(0), video_buffer(0)
	{
		param.Reset();
	}

	bool IsValid() const
	{
		return param.video_file_id > 0;
	}

	void Reset()
	{
		isdirty = false;
		oldstate = false;
		video_len = 0;
		video_buffer = NULL;
		param.Reset();
	}

	bool isdirty;
	bool oldstate;

	int video_len;
	char* video_buffer;

	BattleVideoParam param;
};

union BattleVideoKey
{
	long long battle_video_record_file_id;
	int monster_group_id;
};

namespace Protocol
{
	class SCBattleReport;
}

class Role;
class BattleVideoMgr
{
public:
	typedef std::map<int, BattleVideoInfo> VideoMap; // key is monster_group_id
	typedef std::map<long long, BattleVideoInfo> FirstKillVideoMap; // key is battle_video_record_file_id
	friend class RMISaveBattleVideoDataBackObjectImpl;
	friend class RMIInitBattleVideoDataBackObjectImpl;

public:
	static BattleVideoMgr& Instance();
	void Update(unsigned long interval, time_t now_second);
	void Save();

	bool OnServerStart();
	void OnServerStop();

	inline bool IsLoadFinish() const { return GLOBAL_SYSTEM_DATA_STATE_LOAD_FINISH == m_data_state; }

	bool NeedChangeVideo(int monster_group_id, int average_score);

	bool TryRecordVideo(short battle_mode, int monster_group_id, int average_score, long long video_file_id, bool is_server_first_kill);
	bool GameHiddenSynVideo(const BattleVideoParam & info_param, const int & video_length, char * video_buffer);
	long long GetVideoFileID(int monster_group_id, bool is_server_first_kill);
	bool IsHasServerFirstKillVideo(long long battle_video_record_file_id);
	const BattleVideoInfo* GetVideoInfoConst(int monster_group_id, bool is_server_first_kill);
	bool RemoveVideo(int monster_group_id, bool is_server_first_kill);
	void SendVideo(Role * role, int monster_group_id, bool is_server_first_kill);
	void GmWatchVideo(Role* role, long long video_file_id);
	void SendVideoToCross(int monster_group_id);
private:
	BattleVideoInfo* GetVideoInfo(int monster_group_id, bool is_server_first_kill);
	BattleVideoInfo* GetVideoInfo(BattleVideoKey video_key, bool is_server_first_kill);
	BattleVideoKey GetVideoKeyByMonsterGroupId(int monster_group_id, bool is_server_first_kill);
	bool LoadVideo(long long id_from);
	void LoadVideoFinish();
	void InitVideoList(const BattleVideoListParam& video_list);
	bool ReadVideoFile(long long file_id, ARG_OUT char** video_buffer, ARG_OUT int* video_len);
	void Release();
	bool CheckBattleReportEventVersion(long long file_id, const Protocol::SCBattleReport& report);

	int m_data_state;
	unsigned int m_last_save_time;
	VideoMap m_video_map;
	FirstKillVideoMap m_first_kill_video_map;

	INSTANCE_FORBID_COPY_AND_EXTERIAL_CREATE(BattleVideoMgr);
};

#endif