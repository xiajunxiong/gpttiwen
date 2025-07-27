#include "hotfixconfig.hpp"
#include "servercommon/configcommon.h"
#include "servercommon/servercommon.h"
#include "hotfix/hotfix.h"

HotfixConfig::HotfixConfig()
{

}

HotfixConfig::~HotfixConfig()
{

}

HotfixConfig& HotfixConfig::Instance()
{
	static HotfixConfig instance;
	return instance;
}

bool HotfixConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("gameworld", InitGameWorldHotfix);
	LOAD_CONFIG("battle", InitBattleHotfix);

	return true;
}

bool HotfixConfig::DoHotfix(const std::string& so_path, const std::string& version_str, const HotfixCfgVec& hotfix_cfgs)
{
#ifdef __unix
	for (size_t i = 0; i < hotfix_cfgs.size(); ++i)
	{
		if (version_str != hotfix_cfgs[i].version_str) continue;

		std::string error_msg;
		bool succ = cmd_fix_2(so_path + hotfix_cfgs[i].so_name, hotfix_cfgs[i].symbol, error_msg);
		printf("HotfixConfig::DoHotfix so_file[%s] symbol[%s] is_succ[%s] errormsg[%s]\n",
			hotfix_cfgs[i].so_name.c_str(), hotfix_cfgs[i].symbol.c_str(), succ ? "TRUE" : "FALSE", error_msg.c_str());
	}
#endif

	return true;
}

int HotfixConfig::InitGameWorldHotfix(TiXmlElement *RootElement)
{
	return this->InitHotfixCfg(m_gameworld_hotfix, RootElement);
}

int HotfixConfig::InitBattleHotfix(TiXmlElement *RootElement)
{
	return this->InitHotfixCfg(m_battle_hotfix, RootElement);
}

int HotfixConfig::InitHotfixCfg(HotfixCfgVec& cfg_vec, TiXmlElement* RootElement)
{
	TiXmlElement* dataElement = RootElement->FirstChildElement("data");
	
	while (NULL != dataElement)
	{
		HotfixCfg cfg;
		if (!GetSubNodeValue(dataElement, "so_name", cfg.so_name))
		{
			return -1;
		}

		if (!GetSubNodeValue(dataElement, "symbol", cfg.symbol))
		{
			return -2;
		}

		if (!this->ExtractVersionStrFromSoName(cfg.so_name, &cfg.version_str))
		{
			return -3;
		}

		cfg_vec.push_back(cfg);
		dataElement = dataElement->NextSiblingElement();
	}

	return 0;
}

bool HotfixConfig::ExtractVersionStrFromSoName(const std::string& so_name, std::string* version_str)
{
	std::vector<std::string> split_strs = ParseStringByDelimiter(so_name.c_str(), "_");
	for (size_t i = 0; i < split_strs.size(); ++i)
	{
		// 版本号的格式必是10个数字
		if (split_strs[i].size() != 10) continue;
		if (!IsStringAllNumber(split_strs[i])) continue;
		if (!IsStringMatchVersionStr(split_strs[i])) continue;

		if (NULL != version_str) *version_str = split_strs[i];
		
		return true;
	}

	return false;
}

