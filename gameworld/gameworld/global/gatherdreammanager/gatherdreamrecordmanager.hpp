#ifndef __GATHER_DREAM_RECORD_MANAGER_HPP__
#define __GATHER_DREAM_RECORD_MANAGER_HPP__

#include "servercommon/gatherdreamdef.hpp"
#include "servercommon/guilddef.hpp"

struct GatherDreamRecordKey
{
	GatherDreamRecordKey(): role_id(0), index(0) {}
	GatherDreamRecordKey(int __role_id, int __index) : role_id(__role_id), index(__index) {}

	bool operator< (const GatherDreamRecordKey& key) const
	{
		if (role_id != key.role_id)
		{
			return role_id < key.role_id;
		}
		else
		{
			return index < key.index;
		}
	}

	int role_id;
	int index;	
};

class Role;
class GatherDreamRecordManager
{
public:
	friend class RMIInitGatherDreamRecordBackObjectImpl;

public:
	static GatherDreamRecordManager  & Instance();

	void OnServerStart();
	void OnServerStop();
	bool LoadGatherDreamData(long long id_from);

	void Update(time_t now_second);

	bool IsLoadFinish() { return m_load_flag; }

	void Init(const GatherDreamRecordParam &param);
	void GetInitParam(GatherDreamRecordParam * param);
	void ClearDirtyMark(GatherDreamRecordParam *param);

	void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);

	void AddRecord(GatherDreamRecordInfo & get_record, GatherDreamRecordInfo & gift_record, GuildGatherDreamParam & param);	//添加记录
	void UpdateDataInfo(SynGatherDreamRecord info, int uid, int guild_id);	//玩家信息修改，同步所有相关记录
	void LeaveGuild(int uid);												//玩家离开家族，删除自身记录，其他与之相关不删
	
	int GetHelpNum(int role_uid, int gift_role_uid);	
	
	void SendRecordListInfo(Role * role, int type);						//发送玩家的赠予或受赠列表
	void SendRecordListInfoToCross(int uid, int type);					//发送协议信息到跨服

private:
	GatherDreamRecordManager();
	~GatherDreamRecordManager();

	void Save(GatherDreamRecordParam * param = NULL);
	void LoadGatherDreamDataSucc();

	typedef std::map<int, std::vector<GatherDreamRecordParam::GatherDreamRecordSaveEntry> > RecordMap;
	typedef std::map<int, std::vector<GatherDreamRecordParam::GatherDreamRecordSaveEntry> >::iterator RecodMapIt;
	typedef std::vector<GatherDreamRecordParam::GatherDreamRecordSaveEntry>::iterator RecodIndexMapIt;
	typedef std::vector<GatherDreamRecordParam::GatherDreamRecordSaveEntry>::reverse_iterator RecordReverseMapIt;

	GatherDreamRecordParam m_param;

	time_t m_gather_dream_record_next_save_time;

	std::map<GatherDreamRecordKey, bool> m_old_state;
	std::map<GatherDreamRecordKey, bool> m_dirty_mark;

	bool m_change_flag;
	bool m_load_flag;
};

#endif