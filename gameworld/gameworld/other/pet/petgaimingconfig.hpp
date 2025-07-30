#ifndef __PET_GAI_MING_CONFIG_HPP__
#define __PET_GAI_MING_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/serverdef.h"
#include "gamedef.h"
#include "servercommon/configcommon.h"
#include <map>
#include <vector>
#include <set>
#include "gameworld/item/itembase.h"

struct PetGaiMingAttrCfg
{
	int seq;
	int is_pet_attr;
	std::vector<AttrItemCfg> level_attr;
};

struct PetGaiMingLevelCfg
{
	int level;
	int max_exp;
	bool need_break;
	int break_item_id;
	int break_item_num;
	int reset_item_id;
	int reset_item_num;
};

struct PetGaiMingOtherCfg
{
	int up_level_item_id;
	int add_exp;
	int attr_max_level;
	int return_percent;
};

class PetGaiMingConfig : public ILogicConfig
{
public:
	PetGaiMingConfig();
	virtual ~PetGaiMingConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const PetGaiMingLevelCfg * GetPetGaiMingLevelCfg(int level);
	const AttrItemCfg * GetAttrItemCfg(int seq, int level);
	const PetGaiMingOtherCfg & GetOtherCfg() const;
private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitPetGaiMingAttrCfg(TiXmlElement * RootElement);
	int InitPetGaiMingLevelCfg(TiXmlElement * RootElement);

	PetGaiMingOtherCfg m_other_cfg;
	std::map<int, PetGaiMingAttrCfg> m_attr_cfg;
	std::map<int, PetGaiMingLevelCfg> m_level_cfg;
};

#endif // __PETCONFIG_HPP__