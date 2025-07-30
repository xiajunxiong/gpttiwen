#ifndef _MARRIAGE_CONFIG_HPP_
#define _MARRIAGE_CONFIG_HPP_

#include "servercommon/configcommon.h"
#include "servercommon/servercommon.h"

#include "servercommon/marrydef.hpp"

#include <map>
#include <set>

struct MarriageOtherCfg
{
	MarriageOtherCfg() :need_role_level(0), need_intimacy(0), newly_married_time(3 * 24 * 3600), force_divorce_time(0), marriage_seek_need_coin(0)
	{
	}

	int need_role_level;
	int need_intimacy;
	int newly_married_time;	//结婚后多久算新婚时间

	int can_divorce_time;		//结婚多久后才能离婚
	int force_divorce_time;	//强制离婚要的时间

	int marriage_seek_need_coin;
}; 

struct MarriageMarryCfg
{
	MarriageMarryCfg() :marry_type(0), item_id(0), need_item_num(0), title_id(0), need_coin(0)
	{
	}

	int marry_type;
	int item_id;
	int need_item_num;
	int title_id;
	int need_coin;
};

struct MarriageTaskQuestionCfg
{
	MarriageTaskQuestionCfg()
	{
		question_id = 0;
		coin = 0;
		answer = 0;
	}

	int question_id;
	int coin;	
	int answer;
};

struct MarriageTaskRewardCfg
{
	MarriageTaskRewardCfg()
	{

	}

	std::vector<ItemConfigData> reward_list;
};

struct MarriageTaskFbCfg
{
	MarriageTaskFbCfg()
	{
		monster_id = 0;
	}

	int monster_id;
	std::vector<ItemConfigData> rewards;
};

enum QUESTION_TYPE
{
	QUESTION_TYPE_SINGLE,
	QUESTION_TYPE_DUET,
};


class MarriageConfig : public ILogicConfig
{
public:
	MarriageConfig();
	~MarriageConfig();
	
	virtual bool Init(const std::string &configname, std::string *err);

	const MarriageOtherCfg & GetOtherCfg();
	const MarriageMarryCfg * GetMarriageMarryCfg(int marry_type);

	const MarriageTaskQuestionCfg * GetTaskDuetQuestionCfg(int question_id);
	const MarriageTaskQuestionCfg * GetTaskSingleQuestionCfg(int question_id);
	int GetTaskRandQuestion(int type, const std::set<int> & already_have_question);

	const MarriageTaskFbCfg * GetMarriageTaskFbCfg(int wave);
	const MarriageTaskRewardCfg * GetMarriageTaskRewardCfg(int task_type);
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitMarryCfg(TiXmlElement *RootElement);

	int InitMarryQuestionCfg(TiXmlElement *RootElement);
	int InitMarryTaskCfg(TiXmlElement *RootElement);
	int InitMarryFbCfg(TiXmlElement *RootElement);

	MarriageOtherCfg m_other_cfg;
	std::map<int, MarriageMarryCfg> m_marry_cfg;

	std::map<int, MarriageTaskRewardCfg> m_marry_task_cfg;
	std::vector<MarriageTaskFbCfg> m_task_fb_cfg;
	std::map<int, std::vector<MarriageTaskQuestionCfg> > m_question_cfg;
};

#endif