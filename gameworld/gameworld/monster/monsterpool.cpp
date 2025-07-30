#include "monsterpool.hpp"
#include "servercommon/configcommon.h"
#include "servercommon/servercommon.h"

MonsterPool* g_monster_pool = NULL;

MonsterPool& MonsterPool::GetInstance()
{
	if (NULL == g_monster_pool)
	{
		g_monster_pool = new MonsterPool();
	}
	return *g_monster_pool;
}

bool MonsterPool::Init(const std::string &config_dir, const std::string& config_name, std::string *err)
{
	char errinfo[1024] = { 0 };

	TiXmlDocument document;
	if (config_name == "" || !document.LoadFile(config_dir + config_name))
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: Load MonsterManager Config Error, %s.", (config_dir + config_name).c_str(), document.ErrorDesc());
		*err = errinfo;
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();
	TiXmlElement *currentElement = NULL;

	if (!RootElement)
	{
		*err = config_name + ": xml root node error.";
		return false;
	}

	TiXmlElement *listElement = RootElement->FirstChildElement("monster");
	if (NULL == listElement)
	{
		*err = config_name + ": xml node error in getting node [monster]. ";
		return false;
	}

	while (NULL != listElement)
	{
		currentElement = listElement->FirstChildElement("path");
		if (NULL == currentElement)
		{
			*err = config_name + ": xml node error in getting node [path]. ";
			return false;
		}

		while (NULL != currentElement)
		{
			MonsterCfg* cfg = new MonsterCfg();

			std::string path;

			if (!GetNodeValue(currentElement, path))
			{
				*err = config_name + ": xml node error in node [monstermanager->path] ";
				delete cfg;
				this->Release();
				return false;
			}

			if (!cfg->Init(config_dir + path, *err))
			{
				delete cfg;
				this->Release();
				return false;
			}

			if (cfg->m_monster_id <= 0 || cfg->m_monster_id >= MAX_MONSTER_ID)
			{
				SNPRINTF(errinfo, sizeof(errinfo), "load path: %s error: monster id[%d] is not allowed.", path.c_str(), cfg->m_monster_id);
				*err = errinfo;

				delete cfg;
				this->Release();
				return false;
			}

			if (m_monster_list[cfg->m_monster_id] != 0)
			{
				SNPRINTF(errinfo, sizeof(errinfo), "load path: %s error: monster id[%d] repeat", path.c_str(), cfg->m_monster_id);
				*err = errinfo;

				delete cfg;
				this->Release();
				return false;
			}

			m_monster_list[cfg->m_monster_id] = cfg;

			currentElement = currentElement->NextSiblingElement();
		}

		listElement = listElement->NextSiblingElement();
	}
	

	return true;
}

bool MonsterPool::Reload(const std::string &config_dir, const std::string& config_name, std::string *err)
{
	MonsterPool *temp = new MonsterPool();

	if (!temp->Init(config_dir, config_name, err))
	{
		delete temp; temp = NULL;
		return false;
	}

	if (NULL != g_monster_pool) delete g_monster_pool;

	g_monster_pool = temp;

	return true;
}

bool MonsterPool::ReloadOneMonster(const std::string &dir, int monster_id, std::string *err)
{
	MonsterCfg* cfg = new MonsterCfg();

	std::string path = STRING_SPRINTF("monster/%d.xml", monster_id);

	if (!cfg->Init(dir + path, *err))
	{
		delete cfg;
		return false;
	}

	if (cfg->m_monster_id <= 0 || cfg->m_monster_id >= MAX_MONSTER_ID)
	{
		*err = STRING_SPRINTF("load path: %s error: monster id[%d] is not allowed.", path.c_str(), cfg->m_monster_id);

		delete cfg;
		return false;
	}

	if (NULL != m_monster_list[cfg->m_monster_id])
	{
		delete m_monster_list[cfg->m_monster_id];
	}
	m_monster_list[cfg->m_monster_id] = cfg;

	return true;
}

const MonsterCfg* MonsterPool::GetMonsterCfg(int monster_id)
{
	if (monster_id < 0 || monster_id >= MAX_MONSTER_ID) return NULL;

	return m_monster_list[monster_id];
}

bool MonsterPool::IsMonsterExist(int monster_id)
{
	return NULL != this->GetMonsterCfg(monster_id);
}

void MonsterPool::Release()
{
	for (int i = 0; i < MAX_MONSTER_ID; ++i)
	{
		if (NULL != m_monster_list[i])
		{
			delete m_monster_list[i];
			m_monster_list[i] = NULL;
		}
	}
}

MonsterPool::MonsterPool()
{
	memset(m_monster_list, 0, sizeof(m_monster_list));
}

MonsterPool::~MonsterPool()
{
	this->Release();
}

bool MonsterCfg::Init(const std::string &configname, std::string &err)
{
	char errinfo[1024] = { 0 };

	TiXmlDocument document;
	if (configname == "" || !document.LoadFile(configname))
	{
		SNPRINTF(errinfo, sizeof(errinfo), "Load Monster Config Error:%s\n%s", configname.c_str(), document.ErrorDesc());
		err = errinfo;
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();

	if (NULL == RootElement)
	{
		err = configname + ": xml node error in root";
		return false;
	}

	// ¿ªÊ¼¼ÓÔØ
	if (!GetSubNodeValue(RootElement, "monster_id", m_monster_id) || m_monster_id <= 0)
	{
		err = configname + ": xml node error in node [monsters->monster->monster_id] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "monster_level", m_level) || m_level <= 0)
	{
		err = configname + ": xml node error in node [monsters->monster->monster_level] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "name", m_name))
	{
		err = configname + ": xml node error in node [monsters->monster->name] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "race", m_race))
	{
		err = configname + ": xml node error in node [monsters->monster->race] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "maxhp", m_max_hp) || m_max_hp <= 0)
	{
		err = configname + ": xml node error in node [monsters->monster->maxhp] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "catch_pet_id", m_catch_pet_id) || m_catch_pet_id < 0)
	{
		err = configname + ": xml node error in node [monsters->catch_pet_id] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "drop_card_id", m_drop_card_id))
	{
		err = configname + ": xml node error in node [monsters->drop_card_id] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "drop_card_probablity", m_drop_card_posibility))
	{
		err = configname + ": xml node error in node [monsters->drop_card_probablity] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "pet_refresh_probablity", m_pet_refresh_probablity) || m_pet_refresh_probablity < 0 || m_pet_refresh_probablity > 10000)
	{
		err = configname + ": xml node error in node [monsters->pet_refresh_probablity] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "drop_element_probablity", m_drop_element_probability) || m_drop_element_probability < 0 || m_drop_element_probability > 10000)
	{
		err = configname + ": xml node error in node [monsters->drop_element_probablity] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "water", m_element_list[ELEMENT_TYPE_WATER]) || m_element_list[ELEMENT_TYPE_WATER] < 0 || m_element_list[ELEMENT_TYPE_WATER] > 10)
	{
		err = configname + ": xml node error in node [monsters->water] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "fire", m_element_list[ELEMENT_TYPE_FIRE]) || m_element_list[ELEMENT_TYPE_FIRE] < 0 || m_element_list[ELEMENT_TYPE_FIRE] > 10)
	{
		err = configname + ": xml node error in node [monsters->fire] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "wind", m_element_list[ELEMENT_TYPE_WIND]) || m_element_list[ELEMENT_TYPE_WIND] < 0 || m_element_list[ELEMENT_TYPE_WIND] > 10)
	{
		err = configname + ": xml node error in node [monsters->wind] ";
		return false;
	}

	if (!GetSubNodeValue(RootElement, "earth", m_element_list[ELEMENT_TYPE_EARTH]) || m_element_list[ELEMENT_TYPE_EARTH] < 0 || m_element_list[ELEMENT_TYPE_EARTH] > 10)
	{
		err = configname + ": xml node error in node [monsters->water] ";
		return false;
	}

	return true;
}

bool MonsterCfg::IsRefreshLevelOneMonster() const
{
	return this->CanCatch() && rand() % 10000 < m_pet_refresh_probablity;
}

bool MonsterCfg::IsDropCard() const
{
	if (0 == m_drop_card_id) return false;

	int r = RandomNum(10000);
	return r < m_drop_card_posibility;
}
