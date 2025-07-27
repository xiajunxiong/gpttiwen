#include "battle_condition_config.hpp"
#include "servercommon/struct/battle/battle_def.hpp"

BattleConditionConfig* g_battle_cond_cfg = NULL;

BattleConditionConfig* BattleConditionConfig::Instance()
{
	if (NULL == g_battle_cond_cfg)
	{
		g_battle_cond_cfg = new BattleConditionConfig();
	}

	return g_battle_cond_cfg;
}

bool BattleConditionConfig::Init(const std::string &configname, std::string *err)
{
	char errinfo[1024] = { 0 };
	int iRet = 0;

	TiXmlDocument document;

	if (configname == "" || !document.LoadFile(configname))
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: Load BattleConditionConfig Error,\n %s.", configname.c_str(), document.ErrorDesc());
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
		// 条件配置
		TiXmlElement *element = RootElement->FirstChildElement("condition");
		if (NULL == element)
		{
			*err = configname + ": no [condition].";
			return false;
		}

		iRet = InitConditionCfg(element);
		if (iRet)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: InitConditionCfg failed %d", configname.c_str(), iRet);
			*err = errinfo;
			return false;
		}
	}

	return true;
}

bool BattleConditionConfig::Reload(const std::string &configname, std::string *err)
{
	BattleConditionConfig *temp = new BattleConditionConfig();

	if (!temp->Init(configname, err))
	{
		delete temp; temp = NULL;
		return false;
	}

	if (NULL != g_battle_cond_cfg) delete g_battle_cond_cfg;

	g_battle_cond_cfg = temp;

	return true;
}

const BattleCondCfg* BattleConditionConfig::GetCondCfg(int condition_id)
{
	if (condition_id < 0 || condition_id >= m_cond_cfg_num || condition_id >= CONDITION_CFG_NUM)
	{
		return NULL;
	}

	return &m_cond_cfg_list[condition_id];
}

BattleConditionConfig::BattleConditionConfig() : m_cond_cfg_num(0)
{
	memset(m_cond_cfg_list, 0, sizeof(m_cond_cfg_list));
}

BattleConditionConfig::~BattleConditionConfig()
{

}

int BattleConditionConfig::InitConditionCfg(TiXmlElement *RootElement)
{
	TiXmlElement *dataElement = RootElement->FirstChildElement("data");
	if (NULL == dataElement)
	{
		return -10000;
	}

	m_cond_cfg_num = 0;
	int last_condition_id = -1;
	while (NULL != dataElement)
	{
		if (m_cond_cfg_num >= CONDITION_CFG_NUM)
		{
			return -1000;
		}
	
		int condition_id;
		if (!GetSubNodeValue(dataElement, "condition_id", condition_id) 
			|| condition_id < 0 
			|| condition_id >= CONDITION_CFG_NUM 
			|| last_condition_id + 1 != condition_id)
		{
			return -1;
		}
		last_condition_id = condition_id;

		BattleCondCfg& cfg = m_cond_cfg_list[m_cond_cfg_num];
		cfg.condition_id = condition_id;
		if (!GetSubNodeValue(dataElement, "condition_type", cfg.cond.cond_type) 
			|| cfg.cond.cond_type < 0 
			|| cfg.cond.cond_type >= BATTLE_COND_TYPE_NUM)
		{
			return -2;
		}

		if (!GetSubNodeValue(dataElement, "param_1", cfg.cond.param1))
		{
			return -3;
		}

		if (!GetSubNodeValue(dataElement, "param_2", cfg.cond.param2))
		{
			return -4;
		}

		if (!GetSubNodeValue(dataElement, "param_3", cfg.cond.param3))
		{
			return -5;
		}

		if (!this->CheckCondCfg(cfg))
		{
			return -6;
		}

		m_cond_cfg_num += 1;
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

bool BattleConditionConfig::CheckCondCfg(BattleCondCfg& cfg)
{
	BattleCond& cond = cfg.cond;
	switch (cond.cond_type)
	{
	case BATTLE_COND_WIN_ROUND_BELOW_X:
		{
			if (cond.param1 <= 0) return false;
		}
		break;
		
	case BATTLE_COND_DEAD_CHARACTER_BELOW_X:
		{
			if (cond.param1 != 0 && cond.param1 != 1) return false;
			if (cond.param2 < 0 || 1 == cond.param2) return false; // 不允许param2为1 策划需求
		}
		break;

	case BATTLE_COND_CHARACTER_HP_PERCENT:
		{
			if (cond.param1 != 0 && cond.param1 != 1) return false;
			if (cond.param2 <= 0) return false;
		}
		break;

	case BATTLE_COND_KILL_MONSTER:
		{
			if (cond.param1 <= 0) return false;
			if (cond.param2 < 0 || cond.param2 >= SIDE_CHARACTER_NUM) return false;
			if (cond.param3 <= 0) return false;
		}
		break;

	case BATTLE_COND_WIN_WITH_PARTNER:
		{
			if (cond.param1 <= 0) return false;
		}
		break;

	case BATTLE_COND_WIN_WITH_PARTNER_AND_PET:
		{
			if (cond.param1 <= 0) return false;
			if (cond.param2 <= 0) return false;
		}
		break;
	case BATTLE_COND_WIN_WITH_PET_QUALITY:
		{
			if (cond.param1 <= 0) return false;
			if (cond.param2 <= 0) return false;
		}
		break;
	case BATTLE_COND_WIN_WITH_PARTNER_PROF:
		{
			if (cond.param1 <= 0) return false;
			if (cond.param2 <= 0) return false;
		}
		break;
	case BATTLE_COND_WIN_WITH_PET_LEVEL:
		{
			if (cond.param1 <= 0) return false;
			if (cond.param2 <= 0) return false;
		}
		break;
	case BATTLE_COND_WIN_WITH_PARTNER_QUALITY:
		{
			if (cond.param1 <= 0) return false;
			if (cond.param2 <= 0) return false;
		}
		break;
	case BATTLE_COND_WIN_WITH_PARTNER_SEX:
		{
			if (cond.param1 <= 0) return false;
			if (cond.param2 < 0 || cond.param2 > 1) return false;
		}
		break;
	case BATTLE_COND_WIN_WITH_PETID_STR_LEVEL:
		{
			if (cond.param1 <= 0) return false;
			if (cond.param2 <= 0) return false;
		}
		break;
	case BATTLE_COND_WIN_WITH_NUM_LEVEL_QUA:
		{
			if (cond.param1 <= 0) return false;
			if (cond.param2 <= 0) return false;
			if (cond.param3 <= 0) return false;
		}
		break;
	default:
		break;
	}

	return true;
}
