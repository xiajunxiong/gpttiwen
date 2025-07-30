#include "otherglobalconfig.h"
#include "servercommon/configcommon.h"
#include "checkresourcecenter.hpp"

namespace globalconfig
{
	OtherGlobalConfig::OtherGlobalConfig()
		: is_check_valid(false), max_move_speed(0), move_check_length(0), 
		max_transport_length(0), max_gather_distance(0),
		max_npc_touch_distance(0), bag_sortinorder_interval(0), gm_switch(0), 
		is_open_msg_cache(0), is_open_bigcross(false), speedup_hello_count_max(0), 
		speedup_hello_timeout_s(0), speedup_hello_count_forbit_time_s(0),
		speedup_hello_timeout_forbit_time_s(0), speedup_hello_all_pack_percent_limit(0), 
		speedup_hello_open_check(true), server_max_role_num(0), get_cmd_time_interval_s(GET_CMD_TIME_INTERVAL_S),
		center_city_scene_id(0), center_city_born_pos_x(0), center_city_born_pos_y(0),
		world_talk_is_limit(0), world_talk_effect_msg_min_len(0), world_talk_torelate_max_count(0), 
		world_talk_accumulate_count_reset_interval(0)
	{
		
	}

	bool OtherGlobalConfig::Init(const std::string& configname, std::string* err)
	{
		char errinfo[1024] = { 0 };

		TiXmlDocument document;
		if (configname == "" || !document.LoadFile(configname))
		{
			*err = configname + " Load fail: " + document.ErrorDesc();
			return false;
		}

		TiXmlElement *RootElement = document.RootElement();
		TiXmlElement *CurrElement = 0;

		if (!RootElement)
		{
			*err = configname + ": xml node error in root";
			return false;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////
		CurrElement = RootElement->FirstChildElement("check_move_valid");
		if (!CurrElement)
		{
			*err = configname + ": xml node error in [config->check_move_valid]. cannot find the node.";
			return false;
		}
		if (!GetSubNodeValue(CurrElement, "is_check_valid", is_check_valid))
		{
			*err = configname + ": xml node error in [config->check_move_valid->is_check_valid]";
			return false;
		}
		if (!GetSubNodeValue(CurrElement, "max_move_speed", max_move_speed))
		{
			*err = configname + ": xml node error in [config->check_move_valid->max_move_speed]";
			return false;
		}
		if (!GetSubNodeValue(CurrElement, "move_check_length", move_check_length))
		{
			*err = configname + ": xml node error in [config->check_move_valid->move_check_length]";
			return false;
		}

		if (!GetSubNodeValue(RootElement, "max_transport_length", max_transport_length))
		{
			*err = configname + ": xml node error in [config->max_transport_length]";
			return false;
		}

		if (!GetSubNodeValue(RootElement, "max_gather_distance", max_gather_distance) || max_gather_distance <= 0)
		{
			*err = configname + ": xml node error in [config->max_gather_distance]";
			return false;
		}

		if (!GetSubNodeValue(RootElement, "max_npc_touch_distance", max_npc_touch_distance) || max_npc_touch_distance <= 0)
		{
			*err = configname + ": xml node error in [config->max_npc_touch_distance]";
			return false;
		}

		if (!GetSubNodeValue(RootElement, "bag_sortinorder_interval", bag_sortinorder_interval))
		{
			*err = configname + ": xml node error in [config->bag_sortinorder_interval]";
			return false;
		}

		if (!GetSubNodeValue(RootElement, "gm_switch", gm_switch))
		{
			*err = configname + ": xml node error in [config->gm_switch]";
			return false;
		}

		if (!GetSubNodeValue(RootElement, "is_open_msg_cache", is_open_msg_cache))
		{
			*err = configname + ": xml node error in [config->is_open_msg_cache]";
			return false;
		}

		int open_bigcross_flag = 0;
		if (!GetSubNodeValue(RootElement, "is_open_bigcross", open_bigcross_flag))
		{
			*err = configname + ": xml node error in [config->is_open_bigcross]";
			return false;
		}
		is_open_bigcross = open_bigcross_flag > 0;

		{
			if (!GetSubNodeValue(RootElement, "speedup_hello_count_max", speedup_hello_count_max) || speedup_hello_count_max <= 0)
			{
				*err = configname + ": xml node error in [config->speedup_hello_count_max]";
				return false;
			}
			if (!GetSubNodeValue(RootElement, "speedup_hello_timeout_s", speedup_hello_timeout_s))
			{
				*err = configname + ": xml node error in [config->speedup_hello_timeout_s]";
				return false;
			}
			if (!GetSubNodeValue(RootElement, "speedup_hello_count_forbit_time_s", speedup_hello_count_forbit_time_s) || speedup_hello_count_forbit_time_s < 0)
			{
				*err = configname + ": xml node error in [config->speedup_hello_count_forbit_time_s]";
				return false;
			}
			if (!GetSubNodeValue(RootElement, "speedup_hello_timeout_forbit_time_s", speedup_hello_timeout_forbit_time_s) || speedup_hello_timeout_forbit_time_s < 0)
			{
				*err = configname + ": xml node error in [config->speedup_hello_timeout_forbit_time_s]";
				return false;
			}
			if (!GetSubNodeValue(RootElement, "speedup_hello_all_pack_percent_limit", speedup_hello_all_pack_percent_limit) || speedup_hello_all_pack_percent_limit < 100)
			{
				*err = configname + ": xml node error in [config->speedup_hello_all_pack_percent_limit]";
				return false;
			}
			int open_check = 0;
			if (!GetSubNodeValue(RootElement, "speedup_hello_open_check", open_check) || open_check < 0)
			{
				*err = configname + ": xml node error in [config->speedup_hello_all_pack_percent_limit]";
				return false;
			}
			speedup_hello_open_check = (0 != open_check);
		}

		{
			if (!GetSubNodeValue(RootElement, "server_max_role_num", server_max_role_num) || server_max_role_num < 1000)
			{
				*err = configname + ": xml node error in [config->server_max_role_num]";
				return false;
			}
		}

		{
			get_cmd_time_interval_s = GET_CMD_TIME_INTERVAL_S;
			int temp_s = 0;
			if (GetSubNodeValue(RootElement, "get_cmd_time_inrerval_s", temp_s))
			{
				if (temp_s < 3 || temp_s > 20)
				{
					*err = configname + ": xml node error in [config->get_cmd_time_inrerval_s]";
					return false;
				}
				else
				{
					get_cmd_time_interval_s = temp_s;
				}
			}
		}


		{
			if (!GetSubNodeValue(RootElement, "center_city_scene_id", center_city_scene_id) || center_city_scene_id <= 0)
			{
				*err = configname + ": xml node error in [config->center_city_scene_id]";
				return false;
			}
			CheckResourceCenter::Instance().GetSceneCheck()->Add(center_city_scene_id, __FUNCTION__);

			if (!GetSubNodeValue(RootElement, "center_city_born_pos_x", center_city_born_pos_x) || center_city_born_pos_x < 0)
			{
				*err = configname + ": xml node error in [config->center_city_born_pos_x]";
				return false;
			}

			if (!GetSubNodeValue(RootElement, "center_city_born_pos_y", center_city_born_pos_y) || center_city_born_pos_y < 0)
			{
				*err = configname + ": xml node error in [config->center_city_born_pos_y]";
				return false;
			}
		}

		return true;
	}
}



