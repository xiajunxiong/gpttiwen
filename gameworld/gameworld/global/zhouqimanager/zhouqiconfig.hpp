#ifndef _ZHOU_QI_CONFIG_HPP_
#define _ZHOU_QI_CONFIG_HPP_

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"

#include <vector>
#include <map>
#include <set>


struct ZhouQiPlayTypeCfg
{
	int play_type;
	unsigned int day_begin_time[2];
	unsigned int day_end_time[2];
	int start_day;
	int end_day;
};

struct ZhouQiCfg
{
	int zhou_num;
	std::map<int, ZhouQiPlayTypeCfg> play_cfg;
};

struct ZhouQiStartPointCfg
{
	int zhou_num;
	time_t start_time;
	time_t end_time;
};

struct ZhouQiLimitItemCfg
{
	int zhou_qi_num;
	int item_id;
	std::string zhou_zi_name;
};

struct ZhouQiOpenLimitCfg
{
	long long time;
	int need_role_level;
	int need_hidden_open_server_day;
};

class ZhouQiConfig : public ILogicConfig
{
public:
	ZhouQiConfig();
	~ZhouQiConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	
	const ZhouQiCfg * GetZhouQiCfg(int zhou_qi_num);
	bool GetZhouQiTimeInfo(time_t time, int * ARG_OUT zhou_qi_num, time_t * ARG_OUT begin_time, time_t * ARG_OUT end_time);
	const ZhouQiStartPointCfg * GetZhouQiStartPointCfg(time_t time);
	const ZhouQiLimitItemCfg * GetZhouQiLimitItemCfg(int item_id);
	const ZhouQiOpenLimitCfg * GetZhouQiOpenLimitCfg(long long time);
private:
	int InitZhouQiCfg(TiXmlElement *RootElement);
	int InitZhouQiStartPointCfg(TiXmlElement * RootElement);
	int InitZhouQiLimitItemCfg(TiXmlElement * RootElement);
	int InitZhouQiOpenLimitCfg(TiXmlElement * RootElement);

	std::map<int, ZhouQiCfg> m_zhou_qi_cfg;
	std::vector<ZhouQiStartPointCfg> m_start_point_cfg;
	std::map<int, ZhouQiLimitItemCfg> m_zhou_qi_limit_item_cfg;		//物品 周期数限制配置，不再这个周期里，自动出售
	std::map<long long, ZhouQiOpenLimitCfg> m_zhou_qi_open_limit_cfg;		//方便策划调整等级，不影响当前的，下一期才影响，跟据当前时间读对应配置。
};

#endif
