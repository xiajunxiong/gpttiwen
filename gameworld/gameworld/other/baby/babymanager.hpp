#ifndef _BABY_CACHE_HPP_
#define _BABY_CACHE_HPP_

#include "servercommon/servercommon.h"
#include "servercommon/serverdef.h"
#include "servercommon/struct/global/babydataparam.hpp"
#include <map>

struct BabyDataNode
{
	BabyDataNode(): is_dirty(true), is_new_create(true)
	{
	}

	bool IsInvalid()
	{
		if (baby_data_param.baby_id == 0 || (baby_data_param.father_id == 0 && baby_data_param.mother_id == 0))
		{
			return true;
		}

		return false;
	}

	bool is_dirty;
	bool is_new_create;
	BabyDataParam baby_data_param;
};


class Role;

class BabyManager
{
public:
	const static int  BABY_SAVE_INTERVAL = 60;

	static BabyManager & Instance();

	void OnServerStart();
	void OnServerStop();
	bool LoadBaby(long long inc_id_from);
	void LoadBabySucc();
	bool IsLoadFinish();
	void Update(unsigned long interval, time_t now_second);
	
	void AddBaby(const BabyDataParam & baby_data);
	long long AddNewBaby(int db_index, const GameName baby_name, int father_uid, const GameName father_name = NULL, int mother_uid = 0, const GameName mother_name = NULL, bool is_duet = false);
	void RemoveBaby(long long baby_uid, int role_id);
	BabyDataNode * GetBabyData(long long baby_uuid);
	void SetBabyDataDirty(BabyDataNode * baby_node_data);
private:
	BabyManager();
	~BabyManager();
	void Save();

	long long GetNewUniqueId(int db_index);

	bool m_load_flag;
	int m_cur_sign;
	time_t m_next_save_time;
	bool m_is_chage;

	std::map<long long, BabyDataNode> m_all_baby_map;
};

#endif

