#ifndef __RAND_ACTIVITY_CHENG_FENG_PO_LANG_CONFIG_HPP__
#define __RAND_ACTIVITY_CHENG_FENG_PO_LANG_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "servercommon/struct/itemlistparam.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "servercommon/activitydef.hpp"

struct CFPLRewardSectionCfg
{
	CFPLRewardSectionCfg() :section_start(0), section_end(0)
	{}

	int section_start;
	int section_end;
	std::map<int, std::vector<ItemConfigData> > reward_list;		// key-奖励组
};

struct CFPLOtherCfg
{
	CFPLOtherCfg():frequency(0),time(0)
	{}

	int frequency;
	int time;			//开启时间, 单位:分钟
};

class RandActivityManager;
class RandActivityChengFengPoLangConfig : public RandActivityConfig
{
public:
	RandActivityChengFengPoLangConfig();
	virtual ~RandActivityChengFengPoLangConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	int RandQuestionCfg(std::vector<int>& used_question)const;
	const std::vector<ItemConfigData>* GetResultCfg(RandActivityManager* ramgr, int question, int answer, ARG_OUT bool& is_true)const;
	const CFPLOtherCfg& GetOtherCfg()const { return m_other_cfg; }

private:
	int InitBaseCfg(TiXmlElement *RootElement);
	int InitRewardCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement *RootElement);

	std::vector<int> m_question_cfg;	// 题目组
	std::map<int, std::pair<int, int> > m_answer_cfg;	// key-题目,value-[first-正确答案,secend-奖励组]
	std::vector<CFPLRewardSectionCfg> m_reward_section_cfg;
	CFPLOtherCfg m_other_cfg;
};

#endif
