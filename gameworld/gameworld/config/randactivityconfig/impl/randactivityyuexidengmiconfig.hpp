#ifndef __RAND_ACTIVITY_YUE_XI_DENG_MI_CONFIG_HPP__
#define __RAND_ACTIVITY_YUE_XI_DENG_MI_CONFIG_HPP__

#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itempool.h"
#include "servercommon/activitydef.hpp"
#include "servercommon/roleactivity/rayuexidengmiparam.hpp"

template <class T>
struct RAYueXiDengMiBaseSectionCfg
{
	RAYueXiDengMiBaseSectionCfg() :section_start(0), section_end(0), total_rate(0)
	{}

	bool IsInclude(int real_open_server_day) const
	{
		return (section_start <= real_open_server_day && real_open_server_day <= section_end);
	}

	int section_start;
	int section_end;
	int total_rate;
	T	section_contents;
};

struct RAYueXIDengMiBaseCfg
{
	RAYueXIDengMiBaseCfg(): rate(0)
	{}

	int rate;
	std::vector<ItemConfigData> reward_list;
};

struct RAYueXiDengMiQuestionCfg
{
	RAYueXiDengMiQuestionCfg():lantern_riddle_id(0), rate(0)
	{}

	int lantern_riddle_id;
	int rate;
	std::string answer;
};

struct RAYueXiDengMiOtherCfg
{
	RAYueXiDengMiOtherCfg(): day_answer_limit(0)
	{}

	int day_answer_limit;		// 每日答题次数上限
};

class RandActivityYueXiDengMiConfig : public RandActivityConfig
{
public:
	RandActivityYueXiDengMiConfig();
	virtual ~RandActivityYueXiDengMiConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	int RandQuestion() const;			// 随机题目
	bool IsCorrectAnswer(int question_id, const char* answer) const;		// 判断是否是正确答案
	const std::vector<ItemConfigData>* GetRewardCfg(RandActivityManager* ramgr)const;

	const RAYueXiDengMiOtherCfg& GetOtherCfg() const { return m_other_cfg; }

private:
	int InitBaseCfg(TiXmlElement* RootElement, std::string& err);
	int InitLanternRiddleCfg(TiXmlElement* RootElement, std::string& err);
	int InitOtherCfg(TiXmlElement* RootElement, std::string& err);


	std::vector<RAYueXiDengMiBaseSectionCfg<std::vector<RAYueXIDengMiBaseCfg> > > m_basic_section_cfg;

	int m_question_total_rate;
	std::vector<RAYueXiDengMiQuestionCfg> m_question_cfg;

	RAYueXiDengMiOtherCfg m_other_cfg;
};

#endif