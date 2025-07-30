#ifndef __FUN_OPEN_CONFIG_HPP__
#define __FUN_OPEN_CONFIG_HPP__

#include "funopendef.hpp"
#include "servercommon/configcommon.h"

#include <map>

struct FunOpenCfg
{
	FunOpenCfg() : fun_id(0), limit_level(0), limit_task(0), limit_open_day(0) {}

	int fun_id;
	int limit_level;
	int limit_task;
	int limit_open_day;
};

class FunOpenConfig : public ILogicConfig
{
public:
	FunOpenConfig();
	~FunOpenConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const FunOpenCfg* GetFunOpenCfg(int fun_type);

private:
	int InitFunOpenConfig(TiXmlElement* RootElement);

	FunOpenCfg m_fun_cfg_list[FUN_TYPE_MAX];

};

#endif