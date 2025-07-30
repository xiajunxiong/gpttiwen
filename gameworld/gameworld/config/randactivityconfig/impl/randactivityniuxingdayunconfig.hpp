#ifndef __RAND_ACTIVITY_NIU_XING_DA_YUN_CONFIG_HPP__
#define __RAND_ACTIVITY_NIU_XING_DA_YUN_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itemextern.hpp"
#include "item/itembase.h"
#include <vector>
#include <map>
#include "servercommon/roleactivity/racowparam.hpp"

struct RANiuXingDaYunConfig
{
	RANiuXingDaYunConfig() : type(0), section_start(0), section_end(0), seq(0), customs_introduce_id(0) {}

	int type;
	int section_start;
	int section_end;
	int seq;
	int customs_introduce_id;

	std::vector<ItemConfigData> reward_vec;
};

class RandActivityManager;

class RandActivityNiuXingDaYunConfig : public RandActivityConfig
{
public:
	RandActivityNiuXingDaYunConfig();
	virtual ~RandActivityNiuXingDaYunConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);

	const RANiuXingDaYunConfig * RandSubject(int randactivity_openday) const;
	const RANiuXingDaYunConfig * GetNXDYSubjectCfg(int id) const;
private:
	int InitNiuXingDaYunCfg(TiXmlElement *RootElement);
	long long GetKeyBySection(int section_start, int section_end) const;
	void GetSectionByKey(long long key, int* section_start, int* section_end) const;
	std::map<int, RANiuXingDaYunConfig> m_type_subject_map;
	std::map<long long, std::vector<RANiuXingDaYunConfig> > m_subject_group;
};

#endif	//__RAND_ACTIVITY_GIVE_PET_DRAW_CONFIG_HPP__