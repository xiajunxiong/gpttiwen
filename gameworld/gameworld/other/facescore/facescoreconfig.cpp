#include "facescoreconfig.hpp"

FaceScoreConfig::FaceScoreConfig()
{
}

FaceScoreConfig::~FaceScoreConfig()
{
}

bool FaceScoreConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG

	LOAD_CONFIG("other", InitFacesScoresOthersCfg)
	LOAD_CONFIG("level", InitLevelAttrAdditionCfg)
	LOAD_CONFIG("resource", InitFaceItemsBuyPriceCfg)

	return true;
}

int FaceScoreConfig::GetSeqByTitleID(int title_id)
{
	std::map<int, int>::iterator it = m_title_to_res_map.find(title_id);
	if (m_title_to_res_map.end() != it) return it->second;

	return 0;
}

int FaceScoreConfig::GetUplevelNeedExp(int level, int job)
{
	if (job < 0 || job >= PROFESSION_TYPE_NUM) return NULL;
	if (level >= 0 && level < m_face_score_attr_add_map.rbegin()->first)
	{
		std::map<int, FaceAttrAddInfoProGroup>::iterator it = m_face_score_attr_add_map.find(level);
		if (m_face_score_attr_add_map.end() != it) 
		{
			return it->second.node_cfg[job].need_score;
		}
	}

	return MAX_INT;
}

void FaceScoreConfig::InitLevelAndExpByTotalExp(int * out_level, int * out_exp, int job)
{
	if (job < 0 || job >= PROFESSION_TYPE_NUM) return;
	for (std::map<int, FaceAttrAddInfoProGroup>::iterator loop = m_face_score_attr_add_map.begin(); loop != m_face_score_attr_add_map.end(); ++loop)
	{
		if (*out_exp < loop->second.node_cfg[job].need_score)
		{
			*out_level = loop->first;
			return;
		}

		*out_exp -= loop->second.node_cfg[job].need_score;
	}

	*out_level = m_face_score_attr_add_map.rbegin()->first;
}

const FaceAttrAddInfo * FaceScoreConfig::GetAttrAddCfgByLevel(int level, int job)
{
	if(job < 0 || job >= PROFESSION_TYPE_NUM) return NULL;
	std::map<int, FaceAttrAddInfoProGroup>::iterator level_it = m_face_score_attr_add_map.find(level);
	if (m_face_score_attr_add_map.end() != level_it) 
	{
		return & level_it->second.node_cfg[job];
	}
	return NULL;
}

const FaceResourceInfo * FaceScoreConfig::GetResInfoCfg(int seq)
{
	std::map<int, FaceResourceInfo>::iterator it = m_face_res_info_map.find(seq);
	if (m_face_res_info_map.end() != it) return & it->second;

	return NULL;
}

int FaceScoreConfig::InitFacesScoresOthersCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	if (!GetSubNodeValue(root_element, "ratio", m_face_others.gold_exchange_rate) || m_face_others.gold_exchange_rate < 0)
	{
		return -1;
	}

	return 0;
}

int FaceScoreConfig::InitLevelAttrAdditionCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		FaceAttrAddInfo tmp_con;
		int face_level = 0;
		int job = 0;
		if (!GetSubNodeValue(root_element, "level", face_level) || face_level < 0)
		{
			return -1;
		}

		if (!GetSubNodeValue(root_element, "job", job) || job < 0 || job >= PROFESSION_TYPE_NUM)
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "exp", tmp_con.need_score) || tmp_con.need_score < 0)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "maxhp", tmp_con.max_hp_add) || tmp_con.max_hp_add < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "maxmp", tmp_con.max_mp_add) || tmp_con.max_mp_add < 0)
		{
			return -5;
		}

		const char * base_attr_reader[BATTLE_BASE_ATTR_NUM] = { "gongji", "fangyu", "minjie", "huifu", "jingshen" };
		for (int base_type = 0; base_type < BATTLE_BASE_ATTR_NUM; ++base_type)
		{
			if (!GetSubNodeValue(root_element, base_attr_reader[base_type], tmp_con.base_add[base_type]) || tmp_con.base_add[base_type] < 0)
			{
				return -(100 + base_type);
			}
		}

		const char * amen_attr_reader[] = { "bisha", "mingzhong", "shanduo", "fanji", "mogong", "mokang", "bisha_dmg", "jianren" };
		for (int amen_type = 0; amen_type < BATTLE_AMEN_ATTR_NUM && amen_type < ARRAY_LENGTH(amen_attr_reader); ++amen_type)
		{
			if (!GetSubNodeValue(root_element, amen_attr_reader[amen_type], tmp_con.amen_add[amen_type]) || tmp_con.amen_add[amen_type] < 0)
			{
				return -(200 + amen_type);
			}
		}

		const char * resi_attr_reader[] = { "def_zhongdu", "def_hunshui", "def_shihua", "def_hunluan", "def_jiuzui" };
		for (int resi_type = 0; resi_type < ARRAY_LENGTH(resi_attr_reader) && resi_type < ARRAY_LENGTH(tmp_con.resi_add); ++resi_type)
		{
			if (!GetSubNodeValue(root_element, resi_attr_reader[resi_type], tmp_con.resi_add[resi_type]) || tmp_con.resi_add[resi_type] < 0)
			{
				return -(300 + resi_type);
			}
		}

		m_face_score_attr_add_map[face_level].node_cfg[job] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int FaceScoreConfig::InitFaceItemsBuyPriceCfg(TiXmlElement * RootElement)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	while (NULL != root_element)
	{
		int seq = 0; FaceResourceInfo tmp_con;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0 || seq >= FACE_RESOURCE_NUM_MAX)		
		{
			return -1;
		}

		if (m_face_res_info_map.end() != m_face_res_info_map.find(seq))
		{
			return -2;
		}

		if (!GetSubNodeValue(root_element, "res_type", tmp_con.type) || tmp_con.type < 0 || tmp_con.type >= FACE_TYPE_MAX)
		{
			return -3;
		}

		if (!GetSubNodeValue(root_element, "add_exp", tmp_con.exp_add) || tmp_con.exp_add < 0)
		{
			return -4;
		}

		if (!GetSubNodeValue(root_element, "title_id", tmp_con.title_id) || tmp_con.title_id < 0)
		{
			return -5;
		}

		if (!GetSubNodeValue(root_element, "time", tmp_con.time) || tmp_con.time < 0)
		{
			return -6;
		}

		switch (tmp_con.type)
		{
		case FACE_TITLENAME:
			m_title_to_res_map[tmp_con.title_id] = seq;
			break;
		}

		m_face_res_info_map[seq] = tmp_con;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}
