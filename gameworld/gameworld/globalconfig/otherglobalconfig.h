#ifndef __OTHERGLOBALCONFIG_H__
#define __OTHERGLOBALCONFIG_H__

#include <string>
#include <vector>
#include "gamedef.h"
#include "servercommon/servercommon.h"

namespace globalconfig
{
	static const int GET_CMD_TIME_INTERVAL_S = 5;

	class OtherGlobalConfig
	{
	public:
		OtherGlobalConfig();
		~OtherGlobalConfig() {}

		bool Init(const std::string& path, std::string* err);

		//////////////////////////////////////////////////////////////////////////
		bool is_check_valid;
		unsigned int max_move_speed;
		unsigned int move_check_length;
		unsigned int max_transport_length;

		unsigned int max_gather_distance;
		unsigned int max_npc_touch_distance;

		unsigned int bag_sortinorder_interval;

		int gm_switch;

		int is_open_msg_cache;
		bool is_open_bigcross;

		//////////////////////////////////////////////////////////////////////////
		int speedup_hello_count_max;			// ���ټ����Ϣ�������ֵ
		int speedup_hello_timeout_s;			// ���ټ����Ϣ�����
		int speedup_hello_count_forbit_time_s;	// ���ټ�� ��Ϣ�������������ʱ��
		int speedup_hello_timeout_forbit_time_s;// ���ټ�� ��Ϣ���ͼ����ʱ����ʱ��
		int speedup_hello_all_pack_percent_limit;// ���ټ�� ��Ϣ����������̰ٷֱ�
		bool speedup_hello_open_check;			// �Ƿ������ټ�⣨��բ��

		//////////////////////////////////////////////////////////////////////////
		int server_max_role_num;				// ���������ɵ��������� 

		/////////////////////////////////////////////////////////////////////////
		int get_cmd_time_interval_s;			// ��ѯcommand����������

		//////////////////////////////////////////////////////////////////////////
		int center_city_scene_id;				// ���ǳ���ID
		int center_city_born_pos_x;				// ���ǳ�����X
		int center_city_born_pos_y;				// ���ǳ�����Y

		//////////////////////////////////////////////////////////////////////////
		int world_talk_is_limit;				// ���������Ƿ���
		int world_talk_effect_msg_min_len;		// ������������Ϣ��С����
		int world_talk_torelate_max_count;		// �����������������ͬ��Ϣ��
		int world_talk_accumulate_count_reset_interval; // ���������ۼ�������ʱ����
	};
};

#endif

