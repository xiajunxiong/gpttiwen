#ifndef __HOTFIX_CONFIG_HPP__
#define __HOTFIX_CONFIG_HPP__

#include <string>
#include <vector>
#include "servercommon/configcommon.h"

struct HotfixCfg
{
	std::string version_str;
	std::string symbol;
	std::string so_name;
};

class HotfixConfig
{
public:
	typedef std::vector<HotfixCfg> HotfixCfgVec;

public:
	HotfixConfig();
	~HotfixConfig();

	static HotfixConfig& Instance();

	bool Init(const std::string &configname, std::string *err);

	const HotfixCfgVec& GetGameWorldHotfixCfgs() const { return m_gameworld_hotfix; }
	const HotfixCfgVec& GetBattleHotfixCfgs() const { return m_battle_hotfix; }

	bool DoHotfix(const std::string& so_path, const std::string& version_str, const HotfixCfgVec& hotfix_cfgs);
private:
	int InitGameWorldHotfix(TiXmlElement *RootElement);
	int InitBattleHotfix(TiXmlElement *RootElement);
	int InitHotfixCfg(HotfixCfgVec& cfg_vec, TiXmlElement* RootElement);
	bool ExtractVersionStrFromSoName(const std::string& so_name, std::string* version_str);

	HotfixCfgVec m_gameworld_hotfix;
	HotfixCfgVec m_battle_hotfix;

};

#endif