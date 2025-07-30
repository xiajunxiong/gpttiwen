#ifndef __RAND_ACTIVITY_JOY_SEEKS_FAVORS_CONFIG_HPP__
#define __RAND_ACTIVITY_JOY_SEEKS_FAVORS_CONFIG_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/configcommon.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/roleactivity/rajoyseeksfavorsparam.hpp"
#include "servercommon/struct/itemlistparam.h"
#include <map>

struct JoySeeksFavorsCfg
{
	JoySeeksFavorsCfg() :seq(0), seq1(0), victory_or_defeat(0) {}

	int seq;
	int seq1;
	int victory_or_defeat;
	ItemConfigData reward;
};

struct JoySeeksFavorsWDCfg
{
	JoySeeksFavorsWDCfg() {}
	enum DW_TYPE
	{
		DW_TYPE_WIN = 0,//”Æ
		DW_TYPE_LOSE = 1,// ‰
		DW_TYPE_MAX,
	};
	int grade_start;
	int grade_end;
	JoySeeksFavorsCfg wd_list[DW_TYPE_MAX];
};

struct JoySeeksFavorsOtherCfg
{
	JoySeeksFavorsOtherCfg() :start_time1(0), end_time1(0), start_time2(0), end_time2(0), dr_time(0) {}
	const static int HOUR_PER = 100;
	int start_time1;
	int end_time1;
	int start_time2;
	int end_time2;
	int dr_time;
};

class RandActivityManager;

class RandActivityJoySeeksFavorsConfig : public RandActivityConfig
{
public:
	RandActivityJoySeeksFavorsConfig();
	virtual ~RandActivityJoySeeksFavorsConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const JoySeeksFavorsCfg * GetJoySeeksFavorsCfg(int level, bool is_win)const;
	const JoySeeksFavorsOtherCfg & GetOtherCfg() const { return m_ohter_cfg; }

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitSeekPetsCfg(TiXmlElement *RootElement);
	int GetKeyByStartAndEnd(int grade_start, int grade_end) { return grade_start * 10000 + grade_end; }

	JoySeeksFavorsOtherCfg m_ohter_cfg;
	std::map<int, JoySeeksFavorsWDCfg> m_joy_seeks_favor_cfg;
};

#endif	//__RAND_ACTIVITY_JOY_SEEKS_FAVORS_CONFIG_HPP__