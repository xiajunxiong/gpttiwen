#ifndef __NOTICE_CONFIG_HPP__
#define __NOTICE_CONFIG_HPP__

#include <string>
#include <map>
#include <vector>
#include "servercommon/configcommon.h"
#include "servercommon/servercommon.h"

struct NoticeCfg
{
	NoticeCfg() : notice_id(0), version(0), is_show(0) {}

	int notice_id; 
	int version;
	bool is_show;
	std::string title;
	std::string subject;
	std::string content;
};

class NoticeConfig
{
public:
	typedef std::map<int, NoticeCfg> NoticeCfgMap;

	static NoticeConfig* Instance();

	bool Init(const std::string &configname, std::string *err);
	static bool Reload(const std::string &configname, std::string *err);

	void GetOldConfig(const NoticeConfig* nc);
	std::vector<NoticeCfg*> GetChangedNotice();

private:
	int InitNoticeCfg(TiXmlElement * RootElement);
	bool IsSameCfg(NoticeCfg* a, NoticeCfg* b);

	INSTANCE_FORBID_COPY_AND_EXTERIAL_CREATE(NoticeConfig);

	NoticeCfgMap m_cfg_map;
	NoticeCfgMap m_old_cfg_map;
};

#endif