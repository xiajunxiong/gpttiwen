#ifndef __PARTNER_EQUIPMENT_CONFIG_HPP__
#define __PARTNER_EQUIPMENT_CONFIG_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/partnerdef.h"
#include "gameworld/item/itembase.h"
#include "item/itempool.h"

#include <vector>
#include <map>

struct PartnerEqBasicInfor
{
	PartnerEqBasicInfor() : suit_index(0) 
	{
		memset(rand_group, 0, sizeof(rand_group));
	}

	int rand_group[MAX_PARTNER_NATAL_CARD_VOC_NUM];
	int suit_index;
};

struct PartnerEqSuitInform
{
	PartnerEqSuitInform() : suit_id(0), suit_level(0), suit_color(0) {}

	int suit_id;
	int suit_level;
	int suit_color;
};

struct PartnerEqRandGroups
{
	PartnerEqRandGroups() : attr_type(0), prob_rate(0) {}

	int attr_type;
	int prob_rate;
};

struct PartnerEqSuitEffect
{
	PartnerEqSuitEffect()
	{
		memset(basic_attr, 0, sizeof(basic_attr));
		memset(resis_attr, 0, sizeof(resis_attr));
		memset(amend_attr, 0, sizeof(amend_attr));
	}

	int basic_attr[PARTNER_ATTR_MAX];
	int resis_attr[PARTNER_RESI_MAX];
	int amend_attr[PARTNER_AMEN_MAX];
};

struct PartnerEqLimitCfg
{
	PartnerEqLimitCfg()
	{
		hole_level = 0;
		hole_need = 0;
		hole_num = 0;
	}
	int hole_level;
	int hole_need;
	int hole_num;
};

struct NewPartnerEqLimitCfg
{
	NewPartnerEqLimitCfg()
	{
		hole_level = 0;
		hole_need = 0;
		hole_num = 0;
		price_type = 0;
		price_num = 0;
	}
	int hole_level;
	int hole_need;
	int hole_num;
	int price_type;
	int price_num;
};


// -----------------------------------------------------------------------------------
class PartnerEquipmentConfig : public ILogicConfig
{
public:
	PartnerEquipmentConfig() {}
	virtual ~PartnerEquipmentConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	bool RandAttrTypeAndValue(int equip_id, BattleAttrPair out_attr[MAX_PARTNER_NATAL_CARD_VOC_NUM]);
	bool CheckAttrTypeAndValue(int equip_id, int attr_idx, int attr_type, int attr_value);
	const PartnerEqSuitInform * GetEquipmentSuitID(int equip_id);
	const PartnerEqSuitEffect * GetPartnerEquipmentAttrAdditionCfg(int suit_id, int level, int color);

	const PartnerEqLimitCfg * GetPEUnlockLimitByUnlockLevel(int partner_id,int slot_idx ,int unlock_level);
	const PartnerEqLimitCfg * GetPEUnlockLimitBegin(int partner_id, int slot_idx);
	const std::vector<PartnerEqLimitCfg> * GetPEUlockLimitVec(int partner_id, int slot_idx);

	const NewPartnerEqLimitCfg * GetNewPEUnlockLimitBegin(int partner_id, int slot_idx);

	const int GetPEResolveValue(int PE_level, int PE_color);
private:
	int InitPartnerEquipmentCfg(TiXmlElement * RootElement);
	int InitAttrRandGroupCfg(TiXmlElement * RootElement);
	int InitEquipmentAttrCfg(TiXmlElement * RootElement);
	int InitSuitAttributeCfg(TiXmlElement * RootElement);
	int InitEquipMentHoleCfg(TiXmlElement * RootElement);
	int InitEquipResolveCfg(TiXmlElement * RootElement);
	int InitEquipMentNewHoleCfg(TiXmlElement * RootElement);

	std::map<ItemID, PartnerEqBasicInfor> m_equipment_info_map;						// 伙伴装备基本信息；键：装备ID
	std::map<int, std::vector<PartnerEqRandGroups> > m_attr_type_rand_map;			// 属性类型随机列表；键：随机组ID
	std::map<ParamSection, std::map<int,std::map<int, ParamSection> > > m_attr_value_rand_map;		// 属性数值随机列表；键1：装备等级区间，键2：品质类型，属性类型
	std::map<int, std::map<int, std::map<int, PartnerEqSuitEffect> > > m_suit_attr_map;				// 伙伴装备套装属性；键：套装ID、套装等级

	std::map<int, PartnerEqSuitInform> m_index_to_suit_map;							// 套装属性映射；键 - index；
	std::map<int, std::map<int, std::vector<PartnerEqLimitCfg> > > m_pe_limit_map;	// key1 - 伙伴id  key2 - 命格index

	//std::map<int, int> m_resolve_map;												//命格卡分解配置 key-命格卡等级 value-分解所得气运值
	int m_resolve_2[MAX_ROLE_LEVEL][ItemBase::I_COLOR_MAX];							//命格卡分解配置 分解所得气运值

	NewPartnerEqLimitCfg m_new_pe_limit[PARTNER_NUM_MAX][PE_TYPE_MAX];				// 新命格解锁
};

#endif // __PARTNER_EQUIPMENT_CONFIG_HPP__