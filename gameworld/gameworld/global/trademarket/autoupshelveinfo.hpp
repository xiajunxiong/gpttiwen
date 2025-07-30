#ifndef __AUTO_UPSHELVE_INFO_HPP__
#define __AUTO_UPSHELVE_INFO_HPP__

#include "servercommon/struct/global/trademarketparam.hpp"

class AutoUpshelveInfo
{
public:
	AutoUpshelveInfo();
	~AutoUpshelveInfo();

	void* operator new(size_t c);
	void operator delete(void* m);

	bool Init(const AutoUpshelveParam& param, bool is_from_init);
	bool GetInitParam(ARG_OUT AutoUpshelveParam* param);
	void Update(unsigned int now);

	inline bool IsDirty() const { return m_is_dirty; }
	inline void SetDirty(bool is_dirty) { m_is_dirty = is_dirty; }
	inline bool GetOldState() const { return m_old_state; }
	inline void SetOldState(bool old_state) { m_old_state = old_state; }
	inline bool IsValid() const { return !m_invalid_flag; }
	inline void SetInvalidFlag(bool invalid_flag) { m_invalid_flag = invalid_flag; }
	inline ItemID GetItemID() const { return m_param.item_id; }
	inline long long GetUniqueID() const { return m_param.unique_id; }
	bool GetUpshelvePrice(ARG_OUT int* price, ARG_OUT SaleItemParam* param);
	void OnSold();

private:
	void DoSystemUpshelve();

	AutoUpshelveParam m_param;
	bool m_is_dirty;
	bool m_old_state;
	bool m_invalid_flag;
	bool m_has_sold;
	bool m_firsttime_upshelve;	//!< 是否首次上架（首次上架用原价，其后再价格波动）
	
	short m_cur_float_percent;
	unsigned int m_next_upshelve_timestamp;
};


#endif