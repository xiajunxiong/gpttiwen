#include "titleconfig.hpp"
#include "checkresourcecenter.hpp"

TitleConfig::TitleConfig()
{
}

TitleConfig::~TitleConfig()
{
}
bool TitleConfig::Init(const std::string & configname, std::string * err)
{
	PRE_LOAD_CONFIG;	
	
	LOAD_CONFIG("title_list", InitTitleListCfg);
	LOAD_CONFIG("title_get_way", InitAutoTitleListCfg);
	return true;
}

const TitleCfg * TitleConfig::GetTitleCfg(int seq)
{
	std::map<int, TitleCfg>::const_iterator it = m_title_map.find(seq);
	if (it == m_title_map.end()) return NULL;
	return &it->second;
}

const AutoTitleCfg * TitleConfig::GetAutoTitleCfg(int seq)
{
	std::map<int, AutoTitleCfg>::const_iterator it = m_auto_title_map.find(seq);
	if (it == m_auto_title_map.end()) return NULL;
	return &it->second;
}

const AutoTitleCfg * TitleConfig::CheckAutoTitleFirst(int *seq,int conditon_type, int param1, int param2, int param3, int param4)
{
	for (std::map<int, AutoTitleCfg>::iterator it = m_auto_title_map.begin(); it != m_auto_title_map.end();it++)
	{
		if (it->second.cond_type == conditon_type)
		{
			switch (conditon_type)//查找每种称号第一个阶段，传入参数均用作比较
			{
			case AUTO_TITLE_TYPE_1:				//达到指定好友人数				参数1：好友人数
			case AUTO_TITLE_TYPE_2:				//通关试炼之地层数				参数1：层数
			case AUTO_TITLE_TYPE_3:				//参加单服首届竞技对抗赛		参数1：季军
			case AUTO_TITLE_TYPE_4:				//封妖成功次数					参数1：封妖次数
			case AUTO_TITLE_TYPE_5:				//宠物参与战斗次数				参数1：宠物战斗次数
			case AUTO_TITLE_TYPE_6:				//一天之内在商城使用元宝数量	参数1：使用元宝数量
			case AUTO_TITLE_TYPE_7:				//累计冲值元宝数量				参数1：充值元宝数量
			case AUTO_TITLE_TYPE_8:				//开启3种宝箱累计到达指定次数	参数1：开启宝箱次数
			case AUTO_TITLE_TYPE_9:				//累计寻宝次数					参数1：寻宝次数
				*seq = it->first;
				return &it->second;
				break;
			case AUTO_TITLE_TYPE_10:			//到达指定的技能等级			参数1：技能ID  参数2：技能等级
				if (param1 == it->second.cond_param1)
				{
					*seq = it->first;
					return &it->second;
				}
			default:
				break;
			}
			
		}
	}
	return NULL;
}

std::set<int> TitleConfig::GetNotSaveTitle()
{
	return m_not_save_title_id;
}

bool TitleConfig::IsNeedSaveTitleType(int title_type)
{
	UNSTD_STATIC_CHECK(TITLESOURCETYPE_MAX == 9);
	switch (title_type)
	{
		case TITLESOURCETYPE_DEFAULT:
		case TITLESOURCETYPE_CLOUD_ARENA:
		{
			return true;
		}
		break;
	default:
		break;
	}
	return false;
}

int TitleConfig::InitTitleListCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		int id = 0;
		if (!GetSubNodeValue(root_element, "title_id", id) || id <= 0 || id >= MAX_UINT16)return -1;
		if (m_title_map.find(id) != m_title_map.end()) return -2;
		TitleCfg& node_cfg = m_title_map[id];
		node_cfg.title_id = id;

		if (!GetSubNodeValue(root_element, "time", node_cfg.time) || node_cfg.time < 0)return -3;
		if (!GetSubNodeValue(root_element, "source_type", node_cfg.source_type) || node_cfg.source_type <= TITLESOURCETYPE_INVALID || node_cfg.source_type >= TITLESOURCETYPE_MAX)return -4;

		if (!TitleConfig::IsNeedSaveTitleType(node_cfg.source_type))
		{
			m_not_save_title_id.insert(id);
		}

		static const int TITLE_ATTR_COUNT = 20;
		char str[TITLE_ATTR_COUNT][20] = { "maxhp","maxmp","gongji","fangyu","minjie","jingshen","huifu",
			"bisha","jianren","fanji","mingzhong","shanduo",
			"def_zhongdu","def_shihua","def_hunshui","def_jiuzui","def_hunluan",
			"mogong","mokang", "ture_dmg"};
		int attr_id[TITLE_ATTR_COUNT] = {
			BATTLE_ATTR_MAX_HP,
			BATTLE_ATTR_MAX_MP,
			BATTLE_ATTR_ATTACK,
			BATTLE_ATTR_DEFENSE,
			BATTLE_ATTR_AGILITY,
			BATTLE_ATTR_MENTAL,
			BATTLE_ATTR_RECOVERY,

			BATTLE_ATTR_CRITICAL,
			BATTLE_ATTR_CRITICAL_DEC_VALUE,
			BATTLE_ATTR_COUNTER_ATTACK,
			BATTLE_ATTR_HIT,
			BATTLE_ATTR_DODGE,

			BATTLE_ATTR_ANTI_POISON,
			BATTLE_ATTR_ANTI_PETRIFY,
			BATTLE_ATTR_ANTI_FREEZE,
			BATTLE_ATTR_ANTI_DRUNK,
			BATTLE_ATTR_ANTI_CHAOS,

			BATTLE_ATTR_MAGIC_ATTACK,
			BATTLE_ATTR_MAGIC_DEFENSE,

			BATTLE_ATTR_REAL_DAMAGE,
		};

		for (int i = 0; i < TITLE_ATTR_COUNT; ++i)
		{
			if (!GetSubNodeValue(root_element, str[i], node_cfg.attrs[attr_id[i]]) || node_cfg.attrs[attr_id[i]] < 0)
			{
				return -100 - i;
			}
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int TitleConfig::InitAutoTitleListCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)return -999;
	while (NULL != root_element)
	{
		int seq = 0;
		if (!GetSubNodeValue(root_element, "seq", seq) || seq < 0 || seq >= MAX_UINT32)return -1;
		if (m_auto_title_map.find(seq) != m_auto_title_map.end()) return -2;

		AutoTitleCfg& node_cfg = m_auto_title_map[seq];

		if (!GetSubNodeValue(root_element, "title_id", node_cfg.title_id) || node_cfg.title_id < 0 || node_cfg.title_id >= MAX_UINT16)return -3;

		if (!GetSubNodeValue(root_element, "cond_type", node_cfg.cond_type) || node_cfg.cond_type <= 0 || node_cfg.cond_type >= AUTO_TITLE_TYPE_MAX)return -4;

		if (!GetSubNodeValue(root_element, "cond_param1", node_cfg.cond_param1))return -5;

		if (!GetSubNodeValue(root_element, "cond_param2", node_cfg.cond_param2))return -6;

		if (!GetSubNodeValue(root_element, "cond_param3", node_cfg.cond_param3))return -7;

		if (!GetSubNodeValue(root_element, "cond_param4", node_cfg.cond_param4))return -8;

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}