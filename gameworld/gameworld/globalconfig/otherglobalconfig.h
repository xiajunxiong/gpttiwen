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
		int speedup_hello_count_max;			// 加速检测消息数量最大值
		int speedup_hello_timeout_s;			// 加速检测消息最大间隔
		int speedup_hello_count_forbit_time_s;	// 加速检测 消息数量不合理禁号时长
		int speedup_hello_timeout_forbit_time_s;// 加速检测 消息发送间隔超时禁号时长
		int speedup_hello_all_pack_percent_limit;// 加速检测 消息数量最大容忍百分比
		bool speedup_hello_open_check;			// 是否开启加速检测（总闸）

		//////////////////////////////////////////////////////////////////////////
		int server_max_role_num;				// 服务器容纳的最大玩家数 

		/////////////////////////////////////////////////////////////////////////
		int get_cmd_time_interval_s;			// 查询command表的秒数间隔

		//////////////////////////////////////////////////////////////////////////
		int center_city_scene_id;				// 主城场景ID
		int center_city_born_pos_x;				// 主城出生点X
		int center_city_born_pos_y;				// 主城出生点Y

		//////////////////////////////////////////////////////////////////////////
		int world_talk_is_limit;				// 世界聊天是否检测
		int world_talk_effect_msg_min_len;		// 世界聊天检测消息最小长度
		int world_talk_torelate_max_count;		// 世界聊天最多容忍相同消息数
		int world_talk_accumulate_count_reset_interval; // 世界聊天累计数清零时间间隔
	};
};

#endif

