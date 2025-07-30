#ifndef __SYSTEM_NOTICE_CONFIG_HPP__
#define __SYSTEM_NOTICE_CONFIG_HPP__

#include "gamedef.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"

#include <map>

const static int SYSTEM_NOTICE_NUM_MAX = 5;		// 奖励最大数量

struct SystemNoticeAwardCon
{
	SystemNoticeAwardCon() : need_level(0), award_num(0) {}

	int need_level;

	int award_num;
	ItemConfigData award_list[SYSTEM_NOTICE_NUM_MAX];
};

class SystemNoticeConfig : public ILogicConfig
{
public:
	SystemNoticeConfig();
	virtual ~SystemNoticeConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const SystemNoticeAwardCon * GetSystemNoticeAwardCfg(int seq);

private:
	int InitAwardList(TiXmlElement * RootElement);

	std::map<int, SystemNoticeAwardCon> m_award_cfg;
};



#endif