#ifndef __TU_ZI_KUAI_PAO_TU_ZI_HPP__
#define __TU_ZI_KUAI_PAO_TU_ZI_HPP__

#include "obj/character/character.h"
#include "servercommon/weddingdef.hpp"
#include "obj/character/npc_gather.h"


class Role;


struct TuZiKuaiPaoTuZiParam
{
	TuZiKuaiPaoTuZiParam()
	{
		encourage_fail_times = 0;
		encourage_succ_times = 0;
		add_speed_time = 0;
	}

	int encourage_fail_times;
	int encourage_succ_times;
	time_t add_speed_time;
};

class TuZiKuaiPaoTuZi : public Character
{
public:
	TuZiKuaiPaoTuZi();
	virtual ~TuZiKuaiPaoTuZi();

	void Init(int role_id, int tu_zi_type, int track_type, int track_point_group_index, Posi end_posi);
	virtual void Update(unsigned long interval, time_t now_second, unsigned int now_dayid);

	int GetTuZiType();
	int GetTrackType();
	int GetTrackPointGroupIndex();
	int GetCreateRoleId();
	void Encourage(bool is_add_speed);
	TuZiKuaiPaoTuZiParam & GetTuZiParam();
protected:
	int m_tu_zi_type;
	int m_role_id;
	int m_track_type;
	int m_track_point_group_index;
	Posi m_end_pos;
	
	time_t m_run_time;
	time_t m_rest_time;
	TuZiKuaiPaoTuZiParam m_param;

	time_t m_next_send_pos_info_time;
};



#endif