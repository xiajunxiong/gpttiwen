#ifndef __RAND_ACTIVITY_LITTLE_YEAR_GIFT_CONFIG_HPP__
#define __RAND_ACTIVITY_LITTLE_YEAR_GIFT_CONFIG_HPP__

#include "gameworld/gamedef.h"
#include "config/randactivityconfig/randactivityconfig.hpp"
#include "item/itemextern.hpp"
#include "item/itembase.h"
#include <map>
#include <vector>


struct RALittleYearGiftCfg
{
	RALittleYearGiftCfg() : type(0), section_start(0), section_end(0), seq(0), talk_id(0), thread_id(0), npc_id(0){}

	int type;
	int	section_start;
	int	section_end;
	int	seq;
	int	talk_id;
	int	thread_id;
	int npc_id;
	std::vector<ItemConfigData>	reward_item_vec;
};

struct RALittleYearGiftThreadBankCfg
{
	RALittleYearGiftThreadBankCfg() : type(0), thread_id(0), thread(0), rate(0){}

	int type;
	int	thread_id;
	int	thread;
	int	rate;
};

class RandActivityManager;

class RandActivityLittleYearGiftConfig : public RandActivityConfig
{
public:
	RandActivityLittleYearGiftConfig();
	virtual ~RandActivityLittleYearGiftConfig();

	virtual bool Init(TiXmlElement *RootElement, std::string *err);
	const RALittleYearGiftCfg * GetLittleYearGiftCfgByActivityDay(int open_day, int id) const;
	const RALittleYearGiftThreadBankCfg * RandThreadCfg() const;
private:
	int InitLittleYearGiftCfg(TiXmlElement *RootElement);
	int InitThreadBankCfg(TiXmlElement *RootElement);

	union KeyChangeUn
	{
		int section[2];
		long long key;
	};

	long long GetKeyBySection(int section_start, int section_end) const;
	void GetSectionByKey(long long key, int* section_start, int* section_end) const;

	std::map<long long,std::vector<RALittleYearGiftCfg> > m_section_map;
	std::map<int, RALittleYearGiftCfg> m_index_map;
	int bank_weight_count;
	std::vector<RALittleYearGiftThreadBankCfg> m_thread_bank_vec;
};

#endif	//__RAND_ACTIVITY_GIVE_PET_DRAW_CONFIG_HPP__