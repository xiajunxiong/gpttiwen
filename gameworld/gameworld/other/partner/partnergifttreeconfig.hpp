#ifndef __PARTNER_GIFT_TREE_CONFIG_HPP__
#define __PARTNER_GIFT_TREE_CONFIG_HPP__

#include "servercommon/serverdef.h"
#include "servercommon/configcommon.h"
#include "servercommon/struct/battle/battle_def.hpp"
#include "servercommon/partnerdef.h"
#include "item/itempool.h"

#include <vector>
#include <map>
#include <set>

struct GiftTreeNodeCfg
{
	GiftTreeNodeCfg() : hole(0), level_limit(0) {}

	int hole;
	int level_limit;
};

struct PartnerGiftTreeNodeCfg
{
	PartnerGiftTreeNodeCfg():inborn_type(0),inborn_hole_id(0),pre_hole_num(0),pos_hole_num(0),reset_consume_coin(0)
	{
		memset(pos_hole, 0, sizeof(pos_hole));
	}
	const static int MAX_PRE_HOLE_NUM = 2;
	const static int MAX_POS_HOLE_NUM = 3;
	int inborn_type;
	int	inborn_hole_id;

	int	pre_hole_num;
	GiftTreeNodeCfg	pre_hole[MAX_PRE_HOLE_NUM];

	int pos_hole_num;
	int pos_hole[MAX_POS_HOLE_NUM];
	int reset_consume_coin;

};

struct PartnerGiftTreeAttr
{
	PartnerGiftTreeAttr() : attr_type(0), attr_value(0) {}
	int attr_type;
	int attr_value;
};

struct PartnerGiftUplevelCfg
{
	PartnerGiftUplevelCfg() :inborn_type(0), inborn_hole_id(0), inborn_hole_level(0), sp_id(0) {}
	const static int MAX_PARTNER_TREE_MAX_ATTR_NUM = 3;
	int inborn_type;
	int inborn_hole_id;
	int	inborn_hole_level;
	PartnerGiftTreeAttr attr_list[MAX_PARTNER_TREE_MAX_ATTR_NUM];
	int	sp_id;

	std::vector<ItemConfigData> consume_item_list;
};

struct PartnerGiftSpCfg
{
	PartnerGiftSpCfg() : inborn_type(0), sp_hole(0), sp_id(0), inborn_level_ask(0) {}
	int inborn_type;
	int sp_hole;
	int	sp_id;
	int	inborn_level_ask;
};

struct PartnerGiftSpListCfg
{
	PartnerGiftSpListCfg() 
	{
		memset(sp_list, NULL, sizeof(sp_list));
	}
	const static int MAX_SP_NUM = 3;
	PartnerGiftSpCfg* sp_list[MAX_SP_NUM];
};

struct PartnerGiftTreeReturnCfg
{
	PartnerGiftTreeReturnCfg() : inborn_hole_id(0), inborn_hole_level(0), return_item(0), return_num(0) {}

	int inborn_hole_id;
	int inborn_hole_level;
	int return_item;
	int return_num;
};

// -----------------------------------------------------------------------------------
class PartnerGiftTreeConfig : public ILogicConfig
{
public:
	PartnerGiftTreeConfig();
	virtual ~PartnerGiftTreeConfig();

	virtual bool Init(const std::string &configname, std::string *err);
	const PartnerGiftTreeNodeCfg* GetPartnerGiftTreeByPartnerId(int partner_id, int tree_point);
	const PartnerGiftUplevelCfg * GetPartnerGiftTreeUplevelByPartnerId(int partner_id, int tree_point, int level);
	const PartnerGiftSpListCfg * GetPartnerGiftSpCfg(int partner_id);
	const PartnerGiftTreeReturnCfg * GetPartnerGiftTreeReturnCfg(int tree_point, int level);
private:
	int InitPartnerGiftTreeCfg(TiXmlElement * RootElement);
	int InitPartnerGiftUpLevelCfg(TiXmlElement * RootElement);
	int InitPartnerGiftSPCfg(TiXmlElement * RootElement);
	int InitPartnerGiftReturnCfg(TiXmlElement * RootElement);

	PartnerGiftTreeNodeCfg* partner_tree_list[PARTNER_NUM_MAX][MAX_PARTNER_GIFT_TREE_SKILL_NUM];
	PartnerGiftUplevelCfg * partner_gift_up_level[PARTNER_NUM_MAX][MAX_PARTNER_GIFT_TREE_SKILL_NUM][MAX_PARTNER_GIFT_TREE_LEVEL];
	PartnerGiftSpListCfg partner_gift_sp_vec[PARTNER_NUM_MAX];
	PartnerGiftTreeReturnCfg * partner_skill_return[MAX_PARTNER_GIFT_TREE_SKILL_NUM][MAX_PARTNER_GIFT_TREE_LEVEL];

	std::vector<PartnerGiftTreeNodeCfg> m_partner_gift_list;
	std::vector<PartnerGiftUplevelCfg> m_partner_uplevel_vec;
	std::vector<PartnerGiftSpCfg> m_partner_gift_sp_cfg;
	std::vector<PartnerGiftTreeReturnCfg> m_partner_skill_return_vec;
};

#endif // __PARTNER_CONFIG_HPP__