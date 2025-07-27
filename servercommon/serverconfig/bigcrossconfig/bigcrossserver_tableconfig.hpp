#ifndef __BIG_CROSS_SERVER_TABLE_CONFIG_H__
#define __BIG_CROSS_SERVER_TABLE_CONFIG_H__

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include <string>
#include <string.h>
#include <vector>

struct BigCrossTableCfgNoCaChe									// NoCacheTableÅäÖÃ
{
	std::vector<std::string> no_cache_vec;
};

struct BigCrossTableCfgNoDelay									// NoDelayTableÅäÖÃ
{
	std::vector<std::string> no_delay_vec;
};

class BigCrossTableConfig
{
public:
	static BigCrossTableConfig & Instance();

	bool Init(const std::string &path, std::string *err);

	const BigCrossTableCfgNoCaChe & GetNoCaCheConfig() { return m_no_cache_config; }
	const BigCrossTableCfgNoDelay & GetNoDelayConfig() { return m_no_delay_config; }

private:
	int InitNoCaCheConfig(TiXmlElement *RootElement, std::string& errormsg);
	int InitNoDelayConfig(TiXmlElement *RootElement, std::string& errormsg);


	BigCrossTableConfig();
	~BigCrossTableConfig();

	BigCrossTableCfgNoCaChe			m_no_cache_config;		// big_cross ÅäÖÃ
	BigCrossTableCfgNoDelay			m_no_delay_config;		// big_cross dataaccess ÅäÖÃ

};

#endif	// __BIG_CROSS_SERVER_TABLE_CONFIG_H__

