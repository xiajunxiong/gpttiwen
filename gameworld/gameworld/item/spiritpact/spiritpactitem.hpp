#ifndef __SPIRIT_PACT_ITEM_HPP__
#define __SPIRIT_PACT_ITEM_HPP__
//伙伴--元神契约

#include "item/itembase.h"
//要求不放入物品背包  新起字段做仓库保存
//不愿意做被动消耗类  数据要通过物品id取
//要求仓库储存无上限  伙伴系统表里配置映射 save_index <-> item_id

//I_TYPE_PARTNER_PACT
//ITEM_SUB_TYPE_SPIRIT_PACT
//仓库保存	SERIALIZE_OTHER_PARAM(partner_pact_data, partner_pact_param);

class SpiritPactItem : public ItemBase
{
public:
	SpiritPactItem();
	virtual ~SpiritPactItem() {}

	virtual bool Init(const char* path, std::string *err);
	virtual int Use(Role *role, const ItemGridData &item_data, short &num, short param1, short param2, short param3, bool & out_need_set_dirty, int param_type = ITEM_PARAM_TYPE_INVAILD, void * param = NULL) const;

	//对应伙伴id
	int GetPartnerID()const { return m_partner_id; };
	//星级
	int GetStar()const { return m_star; };
	//升星消耗(无法升星 填0)
	ItemID GetUpStarCostID()const { return m_up_star_cost_id; };
	int GetUpStarCostNum()const { return m_up_star_cost_num; };

	//升星产物
	int GetUpStarOutID()const { return m_up_star_out_id; };

	//增加属性千分比
	int GetAttrUpPerMil()const { return m_attr_up_per_mil; };

private:
	int m_partner_id;	//对应伙伴id
	int m_star;		//星级

	//升星消耗(无法升星 填0)
	ItemID m_up_star_cost_id;
	int m_up_star_cost_num;

	//升星产物
	ItemID m_up_star_out_id;

	//增加属性千分比
	int m_attr_up_per_mil;
};

#endif 