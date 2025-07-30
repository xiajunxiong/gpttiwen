#ifndef __RAND_ACTIVITY_OPEN_CFG_HPP__
#define __RAND_ACTIVITY_OPEN_CFG_HPP__

#include <string.h>
#include "servercommon/configcommon.h"
#include "servercommon/activitydef.hpp"

struct RandActivityOpenDetail
{
	RandActivityOpenDetail() : activity_type(0), open_type(RAND_ACTIVITY_OPEN_TYPE_END), 
		sevenday_open(false), is_from_open_server_day_cfg(false), begin_timestamp(0), end_timestamp(0) {}

	bool IsValid() { return 0 != activity_type && RAND_ACTIVITY_OPEN_TYPE_END != open_type; }

	int activity_type;
	int open_type;
	bool sevenday_open; // 开服前七天内是否开启
	bool is_from_open_server_day_cfg;

	unsigned int begin_timestamp;
	unsigned int end_timestamp;
};

class RandActivityOpenCfg : public ILogicConfig
{
public:
	RandActivityOpenCfg();
	~RandActivityOpenCfg();

	bool Init(const std::string &path, std::string *err);

	const std::vector<RandActivityOpenDetail>* GetRaOpenCfg(int activity_type);

	int GetLimitOpenDay(int activity_type);
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitBaseOnServerOpenCfg(TiXmlElement *RootElement);
	int InitBaseOnDayCfg(TiXmlElement *RootElement);
	int InitOpenDayLimitCfg(TiXmlElement *RootElement);
	bool CheckActivityTimeValid();
	bool CheckActivityTimeValid(const std::vector<RandActivityOpenDetail>& cfg_vec);
	bool IsActivityTimeConflict(const RandActivityOpenDetail& cfg_1, const RandActivityOpenDetail& cfg_2);

	int m_begin_day_idx;
	int m_end_day_idx; 
	std::vector<RandActivityOpenDetail> m_open_detail_list[RAND_ACTIVITY_TYPE_MAX];

	time_t m_rand_activity_cfg_switch_timestamp;

	std::map<int, int> m_ra_open_day_limit_map;
};

#endif

