#include "servercommon/serverconfig/appearanceconfig.hpp"
#include "servercommon/configcommon.h"

AppearanceConfig* g_appearance_cfg = NULL;

AppearanceConfig& AppearanceConfig::Instance()
{
	if (NULL == g_appearance_cfg)
	{
		g_appearance_cfg = new AppearanceConfig();
	}
	return *g_appearance_cfg;
}

bool AppearanceConfig::Init(const std::string &path, const std::string &configname, std::string *err)
{
	int iRet = 0;
	char errinfo[1024] = { 0 };

	TiXmlDocument document;
	if (path == "" || !document.LoadFile(path + configname))
	{
		*err = path + " Load fail: " + document.ErrorDesc();
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();
	if (!RootElement)
	{
		*err = path + ": xml node error in root";
		return false;
	}

	TiXmlElement *NodeElement = RootElement->FirstChildElement("image_chan");
	if (NULL == NodeElement)
	{
		*err = configname + ": xml node error in root->image_chan";
		return false;
	}

	iRet = this->InitAppearanceCfg(NodeElement);
	if (0 != iRet)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: InitAppearanceCfg failed %d", path.c_str(), iRet);
		*err = errinfo;
		return false;
	}

	TiXmlElement *image_job_Element = RootElement->FirstChildElement("image_job");
	if (NULL == image_job_Element)
	{
		*err = configname + ": xml node error in root->image_job";
		return false;
	}

	iRet = this->InitChangeImageJobCfg(image_job_Element);
	if (0 != iRet)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: InitChangeImageJobCfg failed %d", path.c_str(), iRet);
		*err = errinfo;
		return false;
	}

	TiXmlElement *headshotElement = RootElement->FirstChildElement("head_job");
	if (NULL == image_job_Element)
	{
		*err = configname + ": xml node error in root->head_job";
		return false;
	}

	iRet = this->InitHeadshotCfg(headshotElement);
	if (0 != iRet)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: headshotElement failed %d", path.c_str(), iRet);
		*err = errinfo;
		return false;
	}

	TiXmlElement *role_element = RootElement->FirstChildElement("role");
	if (NULL == role_element)
	{
		*err = configname + ": xml node error in root->role";
		return false;
	}

	iRet = this->InitRoleCfg(role_element);
	if (0 != iRet)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: role_element failed %d", path.c_str(), iRet);
		*err = errinfo;
		return false;
	}

	return true;
}

bool AppearanceConfig::Reload(const std::string &path, const std::string &configname, std::string *err)
{
	AppearanceConfig *temp = new AppearanceConfig();

	if (!temp->Init(path, configname, err))
	{
		delete temp; temp = NULL;
		return false;
	}

	if (NULL != g_appearance_cfg) delete g_appearance_cfg;

	g_appearance_cfg = temp;

	return true;
}

bool AppearanceConfig::IsColorMatchAvatar(int avatar_type, int advance_times, int color)
{
	if (avatar_type <= AVATAR_TYPE_INVALID || avatar_type >= AVATAR_TYPE_MAX) return false;

	if (advance_times <= 0 || advance_times > PROFESSION_ADVANCE_TIMES) return false;

	ColorVec& colorVec = m_avatar_color_list[avatar_type][advance_times];
	for (size_t i = 0; i < colorVec.size(); ++i)
	{
		if (colorVec[i] == color)
		{
			return true;
		}
	}

	return false;
}

bool AppearanceConfig::CheckAvatarAndColor(int avatar_type, int color, int job_id)
{
	if (avatar_type <= AVATAR_TYPE_INVALID || avatar_type >= AVATAR_TYPE_MAX) return false;
	if (color < 0 || color >= INIT_APPREARANCE_COLOR_NUM) return false;
	JobVec * cfg = this->GetJobVec(avatar_type, job_id % PROFESSION_BASE);
	bool ture_job = false;
	if (cfg != NULL)
	{
		for (std::vector<int>::const_iterator it = cfg->begin(); it != cfg->end(); it++)
		{
			if (*it == job_id)
			{
				ture_job = true;
			}
		}
	}
	else
	{
		ture_job = true;
	}
	if (!ture_job)
	{
		return false;
	}
	return true;
}

int AppearanceConfig::RandProfessionByAvatarType(int avatar_type, int advance_level)
{
	JobVec* jobvec = this->GetJobVec(avatar_type, advance_level);
	if (NULL == jobvec || jobvec->empty() )
	{
		return 0;
	}
	return (*jobvec)[rand() % jobvec->size()];
}

int AppearanceConfig::RandAvatarByProfession(int profession)
{
	ProfessionAvatarMap::iterator it = m_profession_avatar_map.find(profession);
	if (it == m_profession_avatar_map.end())
	{
		return AVATAR_TYPE_1;
	}

	if (it->second.empty())
	{
		return AVATAR_TYPE_3;
	}

	AvatarVec& vec = it->second;
	return vec[rand() % vec.size()];
}

const HeadshotCfg* AppearanceConfig::GetHeadshotCfg(short headshot_id)
{
	HeadshotCfgMap::iterator it = m_headshot_cfg_map.find(headshot_id);
	if (it != m_headshot_cfg_map.end())
	{
		return &it->second;
	}

	return NULL;
}

int AppearanceConfig::GetSexByAvatar(int avatar_type)
{
	if(avatar_type <= AVATAR_TYPE_INVALID || avatar_type >= AVATAR_TYPE_MAX) return 0;

	return m_avatar_sex_list[avatar_type];
}

int AppearanceConfig::InitAppearanceCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int avatar_type = 0;
		if (!GetSubNodeValue(dataElement, "avartar_type", avatar_type) || avatar_type <= AVATAR_TYPE_INVALID || avatar_type >= AVATAR_TYPE_MAX)
		{
			return -1;
		}

		int advance_times = 0;
		if (!GetSubNodeValue(dataElement, "camp_level", advance_times) || advance_times <= 0 || advance_times > PROFESSION_ADVANCE_TIMES)
		{
			return -2;
		}

		TiXmlElement* colorListElement = dataElement->FirstChildElement("color_list");
		if (NULL == colorListElement) return -3;

		TiXmlElement* colorNodeElement = colorListElement->FirstChildElement("node");
		while (colorNodeElement)
		{
			int color = 0;
			if (!GetNodeValue(colorNodeElement, color))
			{
				return -4;
			}
			m_avatar_color_list[avatar_type][advance_times].push_back(color);

			colorNodeElement = colorNodeElement->NextSiblingElement();
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int AppearanceConfig::InitChangeImageJobCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int image_show_id = 0;
		if (!GetSubNodeValue(dataElement, "image_show_id", image_show_id))
		{
			return -1;
		}

		int avartar_type = 0;
		if (!GetSubNodeValue(dataElement, "avartar_type", avartar_type) || avartar_type <= AVATAR_TYPE_INVALID || avartar_type >= AVATAR_TYPE_MAX)
		{
			return -2;
		}

		int camp_level = 0;
		if (!GetSubNodeValue(dataElement, "camp_level", camp_level) || camp_level <= 0 || camp_level > PROFESSION_ADVANCE_TIMES)
		{
			return -3;
		}

		TiXmlElement* colorListElement = dataElement->FirstChildElement("job_id");
		if (NULL == colorListElement) return -4;

		TiXmlElement* colorNodeElement = colorListElement->FirstChildElement("node");
		while (colorNodeElement)
		{
			int job = 0;
			if (!GetNodeValue(colorNodeElement, job))
			{
				return -5;
			}
			m_avatar_job_list[avartar_type][camp_level].push_back(job);
			
			if (avartar_type != AVATAR_TYPE_2)
			{
				m_profession_avatar_map[job].push_back(avartar_type);
			}

			colorNodeElement = colorNodeElement->NextSiblingElement();
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int AppearanceConfig::InitHeadshotCfg(TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int headshot_id = 0;
		if (!GetSubNodeValue(dataElement, "avatar_id", headshot_id) || headshot_id <= 0)
		{
			return -1;
		}

		if (m_headshot_cfg_map.find(headshot_id) != m_headshot_cfg_map.end())
		{
			return -2; // ÖØ¸´ÁË
		}

		HeadshotCfg cfg;
		cfg.headshot_id = headshot_id;

		if (!GetSubNodeValue(dataElement, "avartar_type", cfg.avatar_type) || cfg.avatar_type <= AVATAR_TYPE_INVALID || cfg.avatar_type >= AVATAR_TYPE_MAX)
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "type", cfg.type))
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "advancement_level", cfg.advance_times))
		{
			return -5;
		}
		switch (cfg.type)
		{
			case HeadshotCfg::COND_TYPE_ADVANCE_TIMES:
			{
				if(cfg.advance_times <= 0 || cfg.advance_times > PROFESSION_ADVANCE_TIMES) return -6;
			}
			break;
			case HeadshotCfg::COND_TYPE_SHIZHUANG:
			{
				if(0 != cfg.advance_times) return -7;
			}
			break;
			default:
			return -8;
		}

		if (!GetSubNodeValue(dataElement, "surface_id", cfg.surface_id) || cfg.surface_id < 0)
		{
			return -9;
		}

		m_headshot_cfg_map.insert(HeadshotCfgMap::value_type(headshot_id, cfg));

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

int AppearanceConfig::InitRoleCfg(TiXmlElement * RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement) return -10000;

	while (dataElement)
	{
		int avatar_type = 0, sex = 0;
		if (!GetSubNodeValue(dataElement, "role_id", avatar_type) || avatar_type <= AVATAR_TYPE_INVALID || avatar_type >= AVATAR_TYPE_MAX)
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "sex", sex) || sex < 0 || sex > 1)
		{
			return -2;
		}

		m_avatar_sex_list[avatar_type] = sex;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

AppearanceConfig::AppearanceConfig()
{
	memset(m_avatar_sex_list, 0, sizeof(m_avatar_sex_list));
}

AppearanceConfig::~AppearanceConfig()
{

}

