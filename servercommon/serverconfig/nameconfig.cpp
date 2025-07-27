#include "nameconfig.h"
#include "servercommon/namefilter.hpp"

NameConfig *st_name_config = NULL;

NameConfig * NameConfig::Instance()
{
	if (NULL == st_name_config)
	{
		st_name_config = new NameConfig();
	}
	return st_name_config;
}

bool NameConfig::Init(const std::string &path, const std::string &configname, std::string *err)
{
	int iRet = 0;
	char errinfo[1024] = {0};

	TiXmlDocument document;
	if (path == "" || !document.LoadFile(path + configname))
	{
		*err = path + " Load fail: " +document.ErrorDesc();
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();
	if (!RootElement)
	{
		*err = path + ": xml node error in root";
		return false;
	}
	
	iRet = this->InitNameCfg(RootElement);
	if (0 != iRet)
	{
		sprintf(errinfo,"%s: InitNameConfig failed %d", path.c_str(), iRet);
		*err = errinfo;
		return false;
	}
	return true;
}

int NameConfig::InitNameCfg(TiXmlElement* RootElement)
{
	int ret = this->InitNameCfgImpl(RootElement, m_name_cfg.first_name[FEMALE_FIRST], "female_first");
	if (0 != ret)
	{
		printf("InitNameCfg female_first failed! ret=%d\n", ret);
		return ret;
	}

	this->InitNameCfgImpl(RootElement, m_name_cfg.middle_name[FEMALE_MIDDLE], "female_middle");
	if (0 != ret)
	{
		printf("InitNameCfg female_middle failed! ret=%d\n", ret);
		return ret;
	}

	this->InitNameCfgImpl(RootElement, m_name_cfg.last_name[FEMALE_LAST], "female_last");
	if (0 != ret)
	{
		printf("InitNameCfg female_last failed! ret=%d\n", ret);
		return ret;
	}
	
	this->InitNameCfgImpl(RootElement, m_name_cfg.first_name[MALE_FIRST], "male_first");
	if (0 != ret)
	{
		printf("InitNameCfg male_first failed! ret=%d\n", ret);
		return ret;
	}
	
	this->InitNameCfgImpl(RootElement, m_name_cfg.middle_name[MALE_MIDDLE], "male_middle");
	if (0 != ret)
	{
		printf("InitNameCfg male_middle failed! ret=%d\n", ret);
		return ret;
	}
	
	this->InitNameCfgImpl(RootElement, m_name_cfg.last_name[MALE_LAST], "male_last");
	if (0 != ret)
	{
		printf("InitNameCfg male_last failed! ret=%d\n", ret);
		return ret;
	}

	return 0;
}

bool NameConfig::GetRandomName(int sex, GameName out_name)
{
	if (sex != MALE && sex != FEMALE) return false;
	if (NULL == out_name) return false;

	const NameCfg& name_cfg = NameConfig::Instance()->GetNameCfg();

	unsigned int rand_fir_length = (unsigned int)(name_cfg.first_name[sex].name_str_list.size());
	unsigned int rand_last_length = (unsigned int)(name_cfg.last_name[sex].name_str_list.size());
	unsigned int rand_mid_length = (unsigned int)(name_cfg.middle_name[sex].name_str_list.size());

	unsigned int rand_index = 0;

	std::string name;
	name.reserve(sizeof(GameName));

	if (rand_fir_length > 0)
	{
		// 随机前缀
		rand_index = rand() % rand_fir_length;
		name += name_cfg.first_name[sex].name_str_list[rand_index].name_str;
	}

	if (rand_mid_length > 0)
	{
		// 随机中缀
		rand_index = rand() % rand_mid_length;
		name += name_cfg.middle_name[sex].name_str_list[rand_index].name_str;
	}

	if (rand_last_length > 0)
	{
		// 随机后缀
		rand_index = rand() % rand_last_length;
		name += name_cfg.last_name[sex].name_str_list[rand_index].name_str;
	}
	
	if (0 == name.length()) return false;

	F_STRNCPY(out_name, name.c_str(), sizeof(GameName));
	return true;
}

int NameConfig::InitNameCfgImpl(TiXmlElement* RootElement, NameCfg::NameList& name_list, const char* sheet_name)
{
	if (NULL == sheet_name) return -99;

	GameName game_name;
	bool ret = true;

	{		
		TiXmlElement *role_element = RootElement->FirstChildElement(sheet_name);
		if (NULL == role_element)
		{
			return -1000;
		}

		// 允许没有数据
		TiXmlElement *data_element = role_element->FirstChildElement("data");
		if (NULL == data_element)
		{
			return 0;
		}

		while (NULL != data_element)
		{
			memset(game_name, 0, sizeof(game_name));
			int id = 0;
			// 不允许个数大于1000
			if (!GetSubNodeValue(data_element, "id", id) || 0 >= id || MAX_NAME_CFG_NUM <= id)
			{
				return -1;
			}

			if (!GetSubNodeValue(data_element, "name", game_name) || NameFilter::Instance().HasSensitiveWord(sizeof(game_name), game_name))
			{
				ret = false;
				printf("NameFilter check pass fail on sheet[%s]\tid[%d]\n", sheet_name, id);
			}

			static NameCfg::Name name;
			name.name_id = id;
			F_STRNCPY(name.name_str, game_name, sizeof(name.name_str));

			name_list.name_str_list.push_back(name);

			data_element = data_element->NextSiblingElement();
		}
	}

#ifndef _DEBUG
	if (!ret)
	{
		assert(0);
	}
#endif

	return 0;
}
