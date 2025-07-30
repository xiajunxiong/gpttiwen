#include "mapunlockconfig.hpp"
#include "world.h"
#include "task/taskpool.h"
#include "scene/fbmanager.hpp"
#include "servercommon/flyupsystemdef.h"

MapUnlockConfig::MapUnlockConfig()
{
}

MapUnlockConfig::~MapUnlockConfig()
{
	for (std::map<int, std::vector<UnlockSceneInfo> >::iterator it = m_unlock_scene_map.begin(); it != m_unlock_scene_map.end(); it++)
	{
		std::vector<UnlockSceneInfo>().swap(it->second);
	}
}

bool MapUnlockConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("scene_unlock", InitSceneUnlockCon);
	LOAD_CONFIG("word_type_show", InitWorldAreasInfo);		// 必须放在场景解锁条件之后，会检测解锁条件是否存在

	return true;
}

const UnlockCondition * MapUnlockConfig::GetUnlockCondition(int condition_id)
{
	std::map<int, UnlockCondition>::iterator f_it = m_unlock_condi_map.find(condition_id);
	if (m_unlock_condi_map.end() != f_it) return & f_it->second;

	return NULL;
}

void MapUnlockConfig::GetConditionIDListByIrregularID(int ir_id, std::set<int> & out_list)
{
	out_list.clear();

	std::map<int, std::set<int> >::iterator f_it = m_irregular_id_to_condition_id.find(ir_id);
	if (m_irregular_id_to_condition_id.end() != f_it) out_list = f_it->second;
}

void MapUnlockConfig::GetUnlockInfoBySceneID(int scene_id, std::vector<UnlockSceneInfo> & out_list)
{
	out_list.clear();

	std::map<int, std::vector<UnlockSceneInfo> >::iterator f_it = m_unlock_scene_map.find(scene_id);
	if (m_unlock_scene_map.end() != f_it) out_list.assign(f_it->second.begin(), f_it->second.end());
}

bool MapUnlockConfig::IsCanEnterScene(int scene_id, int role_level)
{
	std::map<int, std::vector<UnlockSceneInfo> >::iterator f_it = m_unlock_scene_map.find(scene_id);
	if (m_unlock_scene_map.end() != f_it && !f_it->second.empty())
	{
		int min_level_limit = MAX_INT;
		for (std::vector<UnlockSceneInfo>::iterator loop = f_it->second.begin(); loop != f_it->second.end(); ++loop)
		{
			std::map<int, UnlockCondition>::iterator cond_it = m_unlock_condi_map.find(loop->condition_id);
			if (m_unlock_condi_map.end() == cond_it || cond_it->second.unlock_type != MAP_UNLOCK_TYPE_LEVEL)
			{
				continue;
			}
			min_level_limit = GetMin(cond_it->second.unlock_param, min_level_limit);
		}
		if (MAX_INT != min_level_limit && role_level < min_level_limit)
		{
			return false;
		}
	}

	return true;
}

int MapUnlockConfig::GetActiveID(int scene_id)
{
	return m_scene_id_to_active_id[scene_id];
}

int MapUnlockConfig::GetIrregularID(int scene_id)
{
	return m_scene_id_to_irregular[scene_id];
}

int MapUnlockConfig::InitWorldAreasInfo(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	std::set<int> tmp_id_checker_set;	// 临时检查表，用于检测激活ID 是否已经存在
	while (NULL != root_element)
	{
		int scene_id = 0; UnlockSceneInfo t_us;
		if (!GetSubNodeValue(root_element, "scene_id", scene_id) || !World::GetInstWorld()->GetSceneManager()->CheckSceneIsExist(scene_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "restrict_in", t_us.condition_id) || m_unlock_condi_map.end() == m_unlock_condi_map.find(t_us.condition_id))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "id", t_us.active_id) || t_us.active_id < 0 ||
			tmp_id_checker_set.end() != tmp_id_checker_set.find(t_us.active_id))
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "irregular_id", t_us.irregular_id) || t_us.irregular_id < IRREGULAR_ID_BEGIN || t_us.irregular_id >= BIT_COUNT_OF_LONG_64 * IRREGULAR_ID_LIST_NUM_MAX)
		{
			return -4;
		}

		std::map<int, int>::iterator f_s_i_it = m_scene_id_to_irregular.find(scene_id);
		if (m_scene_id_to_irregular.end() != f_s_i_it)
		{
			if (f_s_i_it->second != t_us.irregular_id)		// 如果一个场景对应了多个不同的 irregular_id，视为非法
			{
				return -5;
			}
		}
		else
		{
			m_scene_id_to_irregular[scene_id] = t_us.irregular_id;
		}

		std::map<int, std::vector<UnlockSceneInfo> >::iterator f_it = m_unlock_scene_map.find(scene_id);
		if (m_unlock_scene_map.end() != f_it)
		{
			f_it->second.push_back(t_us);
		}
		else
		{
			std::vector<UnlockSceneInfo> tmp_vec_usi; tmp_vec_usi.push_back(t_us);
			m_unlock_scene_map.insert(std::pair<int, std::vector<UnlockSceneInfo> >(scene_id, tmp_vec_usi));
		}

		std::map<int, std::set<int> >::iterator f_i_c_it = m_irregular_id_to_condition_id.find(t_us.irregular_id);
		if (m_irregular_id_to_condition_id.end() != f_i_c_it)
		{
			f_i_c_it->second.insert(t_us.condition_id);
		}
		else
		{
			std::set<int> tmp_condition_id_set; tmp_condition_id_set.insert(t_us.condition_id);
			m_irregular_id_to_condition_id.insert(std::pair<int, std::set<int> >(t_us.irregular_id, tmp_condition_id_set));
		}

		m_scene_id_to_active_id[scene_id] = t_us.active_id;		// 唯一映射，强行覆盖
		tmp_id_checker_set.insert(t_us.active_id);
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int MapUnlockConfig::InitSceneUnlockCon(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int condition_id; UnlockCondition t_uc;
		if (!GetSubNodeValue(root_element, "seq", condition_id) || m_unlock_condi_map.end() != m_unlock_condi_map.find(condition_id))
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "unlock_type", t_uc.unlock_type) || t_uc.unlock_type <= MAP_UNLOCK_TYPE_INVALID || t_uc.unlock_type >= MAP_UNLOCK_TYPE_MAX)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "pram1", t_uc.unlock_param))
		{
			return -3;
		}

		UNSTD_STATIC_CHECK(6 == MAP_UNLOCK_TYPE_MAX)
		switch (t_uc.unlock_type)
		{
		case MAP_UNLOCK_TYPE_TASK:		// param - 解锁所需任务ID
			if (NULL == TASKPOOL->GetTaskCfgByID(t_uc.unlock_param)) return -4;
			break;
		case MAP_UNLOCK_TYPE_LEVEL:		// param - 解锁所需等级
			if (t_uc.unlock_param <= 0 || t_uc.unlock_param > MAX_ROLE_LEVEL) return -5;
			break;
		case MAP_UNLOCK_TYPE_PASS_FB:	// param - 副本类型
			if (t_uc.unlock_param <= 0 || t_uc.unlock_param >= FBManager::FB_TYPE_MAX) return -6;
			break;
		case MAP_UNLOCK_TYPE_BIG_DIPPER:
			if (t_uc.unlock_param < 0 || t_uc.unlock_param >= MAX_BIG_DIPPER_MONSTER_NUM) return -7;
			break;
		case MAP_UNLOCK_TYPE_FEISHEN:
			if (t_uc.unlock_param <= 0 || t_uc.unlock_param >= FlyUpSystemParam::FLY_UP_STAG_MAX) return -7;
			break;
		default:
			return -1;
			break;
		}

		m_unlock_condi_map.insert(std::pair<int, UnlockCondition>(condition_id, t_uc));
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}