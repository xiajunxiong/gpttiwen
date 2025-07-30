#ifndef __WORKSHOP_CONFIG_HPP__
#define __WORKSHOP_CONFIG_HPP__

#include "gamedef.h"
#include "servercommon/configcommon.h"
#include "servercommon/workshopdef.h"
#include "equipment/equipment.h"
#include "equipment/jewelry.h"

#include <map>
#include <set>

static const int FAILURE_RETURN_NUM_MAX = 2;		// ʧ�ܷ������������
static const int COMPOUND_MATE_KIND_MAX = 5;		// �ϳɲ������������
UNSTD_STATIC_CHECK(COMPOUND_MATE_KIND_MAX < ItemNamespace::MAX_BATCH_CONSUME_ITEM_NUM);

struct MaterialCon		// �ϳɲ�������
{
	MaterialCon() : mate_id(0), can_return(WORKSHOP_RETURN_NONE), need_num(0), can_decompose(false), min_value(0), max_value(0) {}

	ItemID mate_id;			// ���� ID
	short can_return;		// ʧ���Ƿ񷵻�
	int need_num;			// �ϳ���������

	bool can_decompose;		// �ֽ��Ƿ񷵻�
	int min_value;
	int max_value;
};

struct WorkshopItemCompound		// ��Ʒ�ϳɱ� - ��ֵ�� ֵ���� �� �ϳ�ID
{
	WorkshopItemCompound(const ItemID & p_comp_id = 0) : composition(p_comp_id), fortifier_id(0),
		level_limit(0), active_sign(0), need_coin(0), craft_need(0), succ_prob(0), resolve_return_lingyu(0) {}

	ItemID composition;				// �ϳ���
	ItemID fortifier_id;			// ǿ������ʯ
	int level_limit;				// �ȼ�����
	int active_sign;				// ������
	int need_coin;					// �ϳ�����ͭ��
	int craft_need;					// ����ǿ������ʱ����Ҫ�Ĵ���ʯ����
	int succ_prob;					// �ϳɳɹ���

	ItemPairCon fail_return_item[FAILURE_RETURN_NUM_MAX];	// ʧ�ܷ�������
	MaterialCon need_mate[COMPOUND_MATE_KIND_MAX];			// �ϳ��������

	int resolve_return_lingyu;
};

struct WorkshopCraftAttrAdd		// װ��ǿ���������������� - ��ֵ�� ֵ������װ��ID
{
	WorkshopCraftAttrAdd() : effect_prob_inc(0)
	{
		memset(min_val_instead, 0, sizeof(min_val_instead));
	}

	int effect_prob_inc;									// ��Ч���ָ�������
	int min_val_instead[Equipment::EQUIP_ATTR_IDX_MAX];		// ������Сֵ�滻ֵ
};

struct WorkshopItemExchange		// ��Ʒ�һ���
{
	WorkshopItemExchange() : exchange_type(0), param1(0), param2(0), need_num(0), give_num(0), succ_rate(0), fail_return_item(0), fail_return_num(0)
	{}

	int exchange_type;		// �һ�����	- 0:������һ�	1:��ӡ���һ� 2:������Ƭ�ֽ� 3:��Կ�׺ϳ�  4:��Կ�׺ϳ� 5:����ϴ��¶�ϳ�
	int param1;				// ����1		- 0:�һ���Ʒ��	1:������ƷID
	int param2;				// ����2		- 0:�����Ʒ��	1:�����ƷID
	int need_num;			// ��������
	int give_num;			// �һ�����
	int succ_rate;			// �ɹ�����
	int need_coin;			// ����ͭ��
	int fail_return_item;	// ʧ�ܷ�������
	int fail_return_num;	// ʧ�ܷ�������
};

struct WorkshopExRate
{
	WorkshopExRate(const ItemID & _item_id = 0) : item_id(_item_id), ex_rate(0) {}

	ItemID item_id;
	UInt16 ex_rate;

	bool operator == (const WorkshopExRate & cobj) const
	{
		return this->item_id == cobj.item_id;
	}

	bool operator < (const WorkshopExRate & cobj) const
	{
		return this->item_id < cobj.item_id;
	}
};

struct WorkshopUltimateCfg
{
	WorkshopUltimateCfg() :equip_type(0), color(0), limit_level(0) {}

	int equip_type;
	int	color;
	int	limit_level;
	
	std::vector<ItemConfigData> common_consume_item;
	std::vector<ItemConfigData> added_consume_item;
};

struct WorkShopUltimateOtherCfg
{
	WorkShopUltimateOtherCfg() : jg_extent_min(0), jg_extent_max(0), rate(0){}

	int jg_extent_min;
	int	jg_extent_max;
	int	rate;
};

struct WorkShopUltimateOtherList
{
	WorkShopUltimateOtherList() : weight_count(0) {}

	int weight_count;
	WorkShopUltimateOtherCfg value_ultimate[5];
};

struct WorlshopGodEquipReturn
{
	WorlshopGodEquipReturn() : color(0), level(0), reset(0) {}

	int color;
	int	level;
	int	reset;
	std::vector<ItemConfigData> turn_back_reward_vec;
};

class WorkshopConfig : public ILogicConfig
{
public:
	WorkshopConfig();
	virtual ~WorkshopConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const WorkshopItemCompound * GetItemCompoundCfg(int comp_seq);
	const WorkshopItemCompound* GetItemCompoundCfgByItemId(ItemID _item_id);
	const WorkshopCraftAttrAdd * GetCraftAttrAddCfg(int equip_id);
	const WorkshopItemExchange * GetItemExchangeCfg(int exch_seq);

	const ItemPairCon * GetFastComposeGemCon(int gemstone_id);
	bool IsItemBelongsToGroup(int group_id, int item_id);
	const ItemID RandAExchangeItem(int group_id);
	const WorkshopUltimateCfg * GetUltimateCfg(int equip_type, int color, int limit_level);
	const WorkShopUltimateOtherCfg * GetRandUlitmateCfg();
	const WorlshopGodEquipReturn * GetGodEquipReturnCfg(int color, int level);
	int GetNowGodEquipReturnVersion() { return god_equip_version_id; }
private:
	int InitCompoundList(TiXmlElement * RootElement);
	int InitCraftAttrAdd(TiXmlElement * RootElement);
	int InitItemExchange(TiXmlElement * RootElement);
	int InitConvertGroup(TiXmlElement * RootElement);
	int InitUltimateConsume(TiXmlElement * RootElement);
	int InitOhterCfg(TiXmlElement * RootElement);
	int InitGodEquipReturn(TiXmlElement *RootElement);

	std::map<int, WorkshopItemCompound> m_compound_items_map;	// ��Ʒ�ϳɱ�
	std::map<int, WorkshopItemCompound> m_target_to_wiccfg;     // ͨ����Ʒid�����Ʒ�ϳɱ�
	std::map<int, WorkshopCraftAttrAdd> m_craft_attr_add_map;	// ǿ�������
	std::map<int, WorkshopItemExchange> m_exchange_props_map;	// ���߶һ���
	std::map<int, std::set<WorkshopExRate> > m_convert_group_map;	// �һ�������

	std::map<int, ItemPairCon> m_gemstone_compose_search_map;	// ��ʯ���ٺϳɲ�ѯӳ��

	WorkshopUltimateCfg m_ultimate_consume[ULTIMATE_EQUIP_TYPE_MAX][ItemBase::I_COLOR_MAX][MAX_ROLE_LEVEL + 1];//��������-װ�����ͣ�װ��Ʒ�ʣ�װ���ȼ�
	WorkShopUltimateOtherList m_ultimate_weight;//��������
	int god_equip_version_id;
	std::vector<WorlshopGodEquipReturn> m_god_equip_return_vec;//��װ����
};

#endif