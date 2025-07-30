#ifndef __SYSTEM_RECYCLE_RULE_HPP__
#define __SYSTEM_RECYCLE_RULE_HPP__

#include "servercommon/attributedef.hpp" 
#include "item/itembase.h"

struct RecycleRuleDetailPet
{
	unsigned short growth[ADD_POINT_TYPE_MAX];
	short growth_total;
	short skill_num;
	short reserve_sh;
	UNSTD_STATIC_CHECK(5 == ADD_POINT_TYPE_MAX);
};

struct RecycleRuleDetailEquipment
{
	short attr_num;
	short extra_attr_num;
	AttrInfo attr_list[EquipmentParam::ATTR_NUM_MAX];
	AttrInfo extra_attr_list[EquipmentParam::ATTR_NUM_MAX];
};

union RecycleRuleDetail
{
	RecycleRuleDetail() // union可带有构造函数，但其成员不能有自定义的构造函数
	{
		this->Reset();
	}

	~RecycleRuleDetail() {}

	void GetData(RecycleRuleDetailData out_data) const
	{
		if (NULL == out_data) return;

		memcpy(out_data, data, sizeof(RecycleRuleDetailData));
	}

	void SetData(const RecycleRuleDetailData in_data)
	{
		if (NULL == in_data) return;

		memcpy(data, in_data, sizeof(RecycleRuleDetailData));
	}

	void Reset()
	{
		memset(data, 0, sizeof(data));
	}

	RecycleRuleDetailData data;
	RecycleRuleDetailPet pet;
	RecycleRuleDetailEquipment equipment;
};
UNSTD_STATIC_CHECK(sizeof(RecycleRuleDetailData) > sizeof(RecycleRuleDetailPet));
UNSTD_STATIC_CHECK(sizeof(RecycleRuleDetailData) > sizeof(RecycleRuleDetailEquipment));

struct RecycleRuleParam;
union NetValueItemParam;
class RecycleRuleInfo
{
public:
	RecycleRuleInfo();
	~RecycleRuleInfo();

	void* operator new(size_t c);
	void operator delete(void* m);

	inline bool IsDirty() const { return m_is_dirty; }
	inline void SetDirty(bool is_dirty) { m_is_dirty = is_dirty; }
	inline bool GetOldState() const { return m_old_state; }
	inline void SetOldState(bool old_state) { m_old_state = old_state; }
	inline bool IsValid() const { return !m_invalid_flag; }
	inline void SetInvalidFlag(bool invalid_flag) { m_invalid_flag = invalid_flag; }
	inline long long GetRuleID() const { return m_rule_id; }
	inline ItemID GetItemID() const { return m_item_id; }

	bool Init(const RecycleRuleParam& param, bool is_from_init);
	bool GetInitParam(ARG_OUT RecycleRuleParam* param);

	bool IsMatchRule(ItemID item_id, int price, const NetValueItemParam& item_param);
	bool IsMatchRulePet(const NetValueItemParam& item_param);
	bool IsMatchRuleEquipment(const NetValueItemParam& item_param);
	void OnRecycle();
	bool IsLapse(unsigned int now);	// 是否失效
private:
	bool m_is_dirty;
	bool m_old_state;
	bool m_invalid_flag;

	ItemID m_item_id;
	int m_recycle_num_limit;
	int m_recycle_price_limit;
	int m_cur_recycle_num;
	unsigned int m_begin_timestamp;
	unsigned int m_end_timestamp;
	long long m_rule_id;
	
	RecycleRuleDetail m_rule_detail;
};

#endif