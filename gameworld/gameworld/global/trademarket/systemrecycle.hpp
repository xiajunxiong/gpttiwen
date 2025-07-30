#ifndef __SYSTEM_RECYCLE_HPP__
#define __SYSTEM_RECYCLE_HPP__

#include "servercommon/struct/global/trademarketparam.hpp"

union NetValueItemParam;
class RecycleRuleInfo;
class SystemRecycle
{
public:
	typedef std::vector<RecycleRuleInfo*> RuleVec;
	typedef std::map<ItemID, RuleVec> ItemRuleHash;
	typedef std::map<long long, RecycleRuleInfo*> RuleMap;

public:
	SystemRecycle();
	~SystemRecycle();

	void Update(time_t now);
	bool IsChanged() { return m_is_change; }
	void SaveDB();
	void OnInitFinish();

	bool AddRecycleRule(const RecycleRuleParam& param, bool is_init);
	bool RemoveRecycleRule(long long rule_id);
	bool IsMatchRule(ItemID item_id, int price, const NetValueItemParam& item_param, ARG_OUT long long* rule_id);
	
	void OnRecycle(long long rule_id);
private:
	void GetChangeSaleItemList(ARG_OUT SystemRecycleRuleList* data);
	void ClearDirtyFlag(const SystemRecycleRuleList& data);
	void EraseRule(ItemID item_id, long long rule_id);
	RecycleRuleInfo* GetRuleInfo(long long rule_id);
	void Release();
	bool CheckRuleLapse();	// 检查是否有规则已失效，有则删除

	RuleMap m_rule_map;
	ItemRuleHash m_item_rule_hash;
	bool m_is_change;
	unsigned int m_next_save_time;
	unsigned int m_next_check_lapse_time;
};

#endif