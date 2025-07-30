#ifndef _JIE_YI_CONFIG_HPP_
#define _JIE_YI_CONFIG_HPP_

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/platformbattledef.hpp"

#include <vector>
#include <map>


struct JieYiOtherCfg 
{
	int intimacy_request;
	int jie_yi_name_consume_item_id;
	int jie_yi_name_consume_num;
	int jie_yi_member_name_consume_item_id;
	int jie_yi_member_name_consume_num;
	int jie_yi_zu_leave_wait_time;

	int jie_yi_tan_he_vote_time;
	int jie_yi_tan_he_offline_time;
};

struct JieYiSkillCfg
{
	int need_intimacy;
	int need_day;
	int skill_id;
	int skill_level;
	int skill_type;
};

class JieYiConfig : public ILogicConfig
{
public:
	JieYiConfig();
	~JieYiConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const JieYiOtherCfg & GetOtherCfg() const;
	std::vector<JieYiSkillCfg> GetJieYiSkill(int intimacy, int day);
private:
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitSkillCfg(TiXmlElement * RootElement);

	JieYiOtherCfg m_other_cfg;
	std::vector<JieYiSkillCfg> m_skill_cfg;
};

#endif

