#include "monster_group_pool.hpp"
#include "servercommon/configcommon.h"
#include "drop/droppool.hpp"
#include "gamelog.h"
#include "checkresourcecenter.hpp"
#include "monsterpool.hpp"
#include "config/otherconfig/lilianconfig.hpp"
#include "config/logicconfigmanager.hpp"
#include "config/otherconfig/fastforwardbattleconfig.hpp"

MonsterGroupPool* g_monster_group_pool = NULL;
MonsterGroupPool& MonsterGroupPool::GetInstance()
{
	if (NULL == g_monster_group_pool)
	{
		g_monster_group_pool = new MonsterGroupPool();
	}
	return *g_monster_group_pool;
}

bool MonsterGroupPool::Init(const std::string &configname, std::string *err)
{
	char errinfo[1024] = { 0 };
	int iRet = 0;

	TiXmlDocument document;

	if (configname == "" || !document.LoadFile(configname))
	{
		sprintf(errinfo, "%s: Load MonsterGroupPool Error,\n %s.", configname.c_str(), document.ErrorDesc());
		*err = errinfo;
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();

	if (NULL == RootElement)
	{
		*err = configname + ": xml root node error.";
		return false;
	}

	{
		// monster_group_list
		TiXmlElement *element = RootElement->FirstChildElement("monster_group");
		if (NULL == element)
		{
			*err = configname + ": no [monster_group].";
			return false;
		}

		iRet = InitMonsterGroupCfg(element);
		if (iRet)
		{
			sprintf(errinfo, "%s: InitMonsterGroupCfg failed %d", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	return true;
}

bool MonsterGroupPool::Reload(const std::string &configname, std::string *err)
{
	MonsterGroupPool* temp = new MonsterGroupPool();

	if (!temp->Init(configname, err))
	{
		delete temp;
		temp = NULL;
		return false;
	}

	if (NULL != g_monster_group_pool) delete g_monster_group_pool;

	g_monster_group_pool = temp;

	return true;
}

const MonsterGroupCfg* MonsterGroupPool::GetMonsterGroupCfg(int monster_group_id)
{
	if (monster_group_id < 0 || monster_group_id >= MAX_GROUP_ID) return NULL;

	return m_group_list[monster_group_id];
}

MonsterGroupPool::MonsterGroupPool()
{
	memset(m_group_list, 0, sizeof(m_group_list));
}

MonsterGroupPool::~MonsterGroupPool()
{
	this->Release();
}

void MonsterGroupPool::Release()
{
	for (int i = 0; i < MAX_GROUP_ID; ++i)
	{
		if (NULL != m_group_list[i])
		{
			delete m_group_list[i];
			m_group_list[i] = NULL;
		}
	}
}

int MonsterGroupPool::InitMonsterGroupCfg(TiXmlElement *RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	while (NULL != dataElement)
	{
		int monster_group_id = 0;
		if (!GetSubNodeValue(dataElement, "monster_group_id", monster_group_id) || monster_group_id < 0 || monster_group_id >= MAX_GROUP_ID)
		{
			return -1;
		}

		if (NULL != m_group_list[monster_group_id])
		{
			return -1000;
		}

		int client_show_id = 0;
		if (!GetSubNodeValue(dataElement, "show_monster", client_show_id))
		{
			return -2;
		}

		int is_task_monster = 0;
		if (!GetSubNodeValue(dataElement, "is_task_monster", is_task_monster))
		{
			return -3;
		}

		int is_guide_monster = 0;
		if (!GetSubNodeValue(dataElement, "is_guide_monster", is_guide_monster))
		{
			return -4;
		}

		unsigned short drop_times = 0;
		if (!GetSubNodeValue(dataElement, "drop_num", drop_times))
		{
			return -5;
		}

		int item_id = 0;
		if (!GetSubNodeValue(dataElement, "item_id", item_id))
		{
			return -6;
		}

		int kill_exp = 0;
		if (!GetSubNodeValue(dataElement, "kill_exp", kill_exp))
		{
			return -7;
		}

		int captain_only_drop = 0;
		if (!GetSubNodeValue(dataElement, "captain_only_drop", captain_only_drop))
		{
			return -8;
		}

		int is_challenge_monster = 0;
		if (!GetSubNodeValue(dataElement, "is_challage", is_challenge_monster))
		{
			return -9;
		}

		int is_team_challenge_monster = 0;
		if (!GetSubNodeValue(dataElement, "is_team_challenge", is_team_challenge_monster))
		{
			return -9;
		}

		int is_branch_monster = 0;
		if (!GetSubNodeValue(dataElement, "is_branch", is_branch_monster))
		{
			return -10;
		}

		int is_advanced_monster = 0;
		if (!GetSubNodeValue(dataElement, "is_advanced", is_advanced_monster))
		{
			return -11;
		}

		if (item_id > 0)
		{
			ItemDropMonsterGroupHash::iterator it = m_item_drop_monster_group_hash.find(item_id);
			if (it != m_item_drop_monster_group_hash.end())
			{
				// item_id与怪物组应是1:1关系
				return -777;
			}
			m_item_drop_monster_group_hash.insert(ItemDropMonsterGroupHash::value_type(item_id, monster_group_id));
		}
		
		MonsterGroupCfg* group = new MonsterGroupCfg();
		m_group_list[monster_group_id] = group;
		group->monster_group_id = monster_group_id;
		group->head_monster_id = client_show_id;
		group->is_challenge_monster = (is_challenge_monster > 0);
		group->is_team_challenge_monster = (is_team_challenge_monster > 0);
		if (is_challenge_monster > 0 && is_team_challenge_monster > 0)return -999;
		group->is_task_monster = (is_task_monster > 0);
		group->is_guide_monster = (is_guide_monster > 0);
		group->is_branch_task_monster = (is_branch_monster > 0);
		group->is_advanced_task_monster = (is_advanced_monster > 0);
		group->drop_times = drop_times;
		group->drop_item_id_checker = item_id;
		group->kill_exp = kill_exp;
		group->captain_only_drop = (captain_only_drop > 0);
		group->all_monster_total_hp_num = 0;

		{
			TiXmlElement *dropidListElement = dataElement->FirstChildElement("drop_list");
			if (NULL == dropidListElement)
			{
				return -2000;
			}
			if (!this->ReadDropIDList(dropidListElement, group->dropid_list))
			{
				return -3000;
			}
		}

		{
			TiXmlElement *dropidListElement = dataElement->FirstChildElement("captain_drop_list");
			if (NULL == dropidListElement)
			{
				return -2001;
			}
			if (!this->ReadDropIDList(dropidListElement, group->captain_drop_list))
			{
				printf("MonsterGroupPool::InitMonsterGroupCfg monster_group_id[%d] captain_drop_list error!\n", monster_group_id);
				return -3001;
			}
		}

		{
			TiXmlElement *dropidListElement = dataElement->FirstChildElement("decay_drop_list");
			if (NULL == dropidListElement)
			{
				return -2002;
			}
			if (!this->ReadDropIDList(dropidListElement, group->decay_drop_list))
			{
				return -3002;
			}
		}

		{
			for (int i = 0; i < SIDE_CHARACTER_NUM; ++i)
			{
				char tmp[20] = { 0 };
				snprintf(tmp, sizeof(tmp), "monster_id_%d", i);
				
				if (!GetSubNodeValue(dataElement, tmp, group->monster_id_list[i]) )
				{
					group->monster_id_list[i] = 0;
				}
				if (group->monster_id_list[i] > 0)
				{
					const MonsterCfg* monster_cfg = MonsterPool::GetInstance().GetMonsterCfg(group->monster_id_list[i]);
					if (NULL == monster_cfg)
					{
						printf("MonsterGroupPool::InitMonsterGroupCfg monster_group_id[%d] monster[%d] not found!\n", monster_group_id, group->monster_id_list[i]);
						return -3003;
					}
					group->all_monster_total_hp_num += monster_cfg->m_max_hp;
				}
			}
		}

		if ((group->dropid_list.size() > 0 || group->captain_drop_list.size() > 0) && group->drop_times <= 0)
		{
			gamelog::g_log_world.printf(LL_WARNING, "InitMonsterGroupCfg monster_group_id[%d] drop_list not empty but drop_times <= 0", monster_group_id);
		}

		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

bool MonsterGroupPool::ReadDropIDList(TiXmlElement *dropListElement, std::vector<UInt16> &dropid_list) const
{
	TiXmlElement *dropidElement = dropListElement->FirstChildElement("dropid");
	if (NULL == dropidElement) return false;

	while (NULL != dropidElement)
	{
		UInt16 dropid = 0;
		if (!GetNodeValue(dropidElement, dropid)) return false;

		if (0 != dropid)
		{
			if (NULL == DropPool::Instance()->GetDropConfig(dropid)) return false;

			dropid_list.push_back(dropid);
			if ((int)dropid_list.size() >= DROP_LIST_MAX_COUNT) return false;
		}

		dropidElement = dropidElement->NextSiblingElement();
	}

	return true;
}

int MonsterGroupPool::GetMonsterGroupIDByLimitedDropItemID(int item_id)
{
	ItemDropMonsterGroupHash::iterator it = m_item_drop_monster_group_hash.find(item_id);
	if (it == m_item_drop_monster_group_hash.end()) return 0;

	return it->second; 
}

long long MonsterGroupPool::GetMonsterGroupAllMonsterTotalHp(int monster_group_id)
{
	const MonsterGroupCfg* cfg = this->GetMonsterGroupCfg(monster_group_id);
	if (NULL == cfg)
	{
		return 0LL;
	}
	return cfg->all_monster_total_hp_num;
}

int MonsterGroupPool::GetFastForwardRoundByMonsterGroup(int group_id, int role_level)
{
	const MonsterGroupCfg* group = this->GetMonsterGroupCfg(group_id);
	if (NULL == group) return 0;

	if (group->is_challenge_monster)
	{
		int limit_lv = LOGIC_CONFIG->GetFastForwardBattleConfig()->GetChallengeTaskLevelLimit();
		if (role_level >= limit_lv)
		{
			return LOGIC_CONFIG->GetFastForwardBattleConfig()->GetFastBattleDefaultRounds();
		}
	}

	return 0;
}
