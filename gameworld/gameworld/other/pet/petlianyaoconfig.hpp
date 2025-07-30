#ifndef __PET_LIAN_YAO_CONFIG_HPP__
#define __PET_LIAN_YAO_CONFIG_HPP__

#include "servercommon/struct/itemlistparam.h"
#include "servercommon/serverdef.h"
#include "gamedef.h"
#include "servercommon/configcommon.h"
#include <map>
#include <vector>
#include <set>
#include "gameworld/item/itembase.h"

struct PetFurnaceCfg
{
	int furnace_id;
	int lian_yao_unlock_need_role_level;
	int lian_yao_unlock_need_role_fei_sheng_times;
	int lian_yao_unlock_need_pet_quality;
	int lian_yao_unlock_need_pet_strengthen_level;;

	int main_pet_max_quality;
	int main_pet_min_strengthen;
	int second_pet_max_quality;
	int second_pet_max_strengthen;
	int need_coin;
};

struct PetFurnaceOtherCfg
{
	int lian_yao_add_passive_rate;
	int lian_yao_add_passive_max_num;
	int lian_yao_comprehend_passive_skill_need_item_id;
	std::vector<int> lian_yao_comprehend_passive_skill_need_num;
	std::vector<int> lian_yao_comprehend_passive_skill_pool;
};

class PetLianYaoConfig : public ILogicConfig
{
public:
	PetLianYaoConfig();
	virtual ~PetLianYaoConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const PetFurnaceCfg * GetPetFurnaceCfg(int furnace_id);
	const PetFurnaceOtherCfg & GetOtherCfg();

private:
	int InitPetFurnaceCfg(TiXmlElement * RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);

	PetFurnaceOtherCfg m_other_cfg;
	std::map<int , PetFurnaceCfg > m_pet_furnace_cfg;
};

#endif 