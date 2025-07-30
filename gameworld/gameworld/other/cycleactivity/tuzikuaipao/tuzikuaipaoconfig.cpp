#include "servercommon/tuzikuaipaodef.hpp"
#include "tuzikuaipaoconfig.hpp"

TuZiKuaiPaoConfig::TuZiKuaiPaoConfig()
{
}

TuZiKuaiPaoConfig::~TuZiKuaiPaoConfig()
{
}

bool TuZiKuaiPaoConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitOtherCfg);
	LOAD_CONFIG("rabbit", InitTuZiKuaiPaoTuZiCfg);
	LOAD_CONFIG("encourage", InitTuZiKuaiPaoEncourageCfg);
	LOAD_CONFIG("track", InitTuZiKuaiPaoTrackCfg);

	return true;
}

const TuZiKuaiPaoTuZiCfg * TuZiKuaiPaoConfig::GetTuZiKuaiPaoTuZiCfg(int tu_zi_type)
{
	return MapValuesPtr(m_tu_zi_cfg, tu_zi_type);
}

const TuZiKuaiPaoEncourageCfg * TuZiKuaiPaoConfig::GetTuZiKuaiPaoEncourageCfg(int encourage_type)
{
	return MapValuesPtr(m_encourage_cfg, encourage_type);
}

const TuZiKuaiPaoTrackCfg * TuZiKuaiPaoConfig::GetTuZiKuaiPaoTrackCfg(int track_type)
{
	return MapValuesPtr(m_track_cfg, track_type);
}

const TuZiKuaiPaoOtherCfg & TuZiKuaiPaoConfig::GetTuZiKuaiPaoOtherCfg()
{
	return m_other_cfg;
}

std::set<int> TuZiKuaiPaoConfig::GetAllTuZiType()
{
	return m_all_tu_zi_type;
}

int TuZiKuaiPaoConfig::InitTuZiKuaiPaoEncourageCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}


	while (NULL != root_element)
	{
		TuZiKuaiPaoEncourageCfg cfg;
		if (!GetSubNodeValue(root_element, "encourage", cfg.encourage_type))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "effect", cfg.has_add_speed))
		{
			ret_line;
		}

		for (int i = 0; i < 3; ++i)
		{
			std::string sx = STRING_SPRINTF("succ_rate_%d", i + 1);
			int t = 0;
			if (!GetSubNodeValue(root_element, sx, t))
			{
				ret_line;
			}

			cfg.succ_rate.push_back(t);
		}

		m_encourage_cfg[cfg.encourage_type] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TuZiKuaiPaoConfig::InitTuZiKuaiPaoTrackCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}


	while (NULL != root_element)
	{
		TuZiKuaiPaoTrackCfg cfg;
		if (!GetSubNodeValue(root_element, "track", cfg.track_id))
		{
			ret_line;
		}


		for (int i = 0; i < TU_ZI_KUAI_PAO_ROLE_NUM; ++i)
		{
			std::string sx = STRING_SPRINTF("start_x%d", i);
			std::string sy = STRING_SPRINTF("start_y%d", i);
			std::string ex = STRING_SPRINTF("end_x%d", i);
			std::string ey = STRING_SPRINTF("end_y%d", i);

			TuZiKuaiPaoTrackPointGroup p;
			if (!GetSubNodeValue(root_element, sx, p.start_pos.x))
			{
				ret_line;
			}

			if (!GetSubNodeValue(root_element, sy, p.start_pos.y))
			{
				ret_line;
			}

			if (!GetSubNodeValue(root_element, ex, p.end_pos.x))
			{
				ret_line;
			}

			if (!GetSubNodeValue(root_element, ey, p.end_pos.y))
			{
				ret_line;
			}

			cfg.point_cfg[i] = p;
		}

		m_track_cfg[cfg.track_id] = cfg;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TuZiKuaiPaoConfig::InitTuZiKuaiPaoTuZiCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}


	while (NULL != root_element)
	{
		TuZiKuaiPaoTuZiCfg cfg;
		if (!GetSubNodeValue(root_element, "rabbit", cfg.tu_iz_type))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "speed", cfg.speed))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "rest_space_min", cfg.rand_run_min_sec))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "rest_space_max", cfg.rand_run_max_sec))
		{
			ret_line;
		}

		if (!GetSubNodeValue(root_element, "rest_time", cfg.rest_sec))
		{
			ret_line;
		}

		m_tu_zi_cfg[cfg.tu_iz_type] = cfg;
		m_all_tu_zi_type.insert(cfg.tu_iz_type);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TuZiKuaiPaoConfig::InitOtherCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "carrot_num", m_other_cfg.use_carrot_max_times))
	{
		ret_line;
	}

	if (!GetSubNodeValue(root_element, "carrot_times",m_other_cfg.add_speed_sec))
	{
		ret_line;
	}

	if (!GetSubNodeValue(root_element, "carrot_speed", m_other_cfg.carrot_add_speed))
	{
		ret_line;
	}

	if (!GetSubNodeValue(root_element, "reward_times", m_other_cfg.reward_times))
	{
		ret_line;
	}

	if (!GetSubNodeValue(root_element, "scene_id", m_other_cfg.scene_id))
	{
		ret_line;
	}

	if (ReadItemConfigDataList(root_element, "succ_reward", m_other_cfg.win_rewards) != 0)
	{
		ret_line;
	}

	if (ReadItemConfigDataList(root_element, "fail_reward", m_other_cfg.lose_rewards) != 0)
	{
		ret_line;
	}

	return 0;
}
