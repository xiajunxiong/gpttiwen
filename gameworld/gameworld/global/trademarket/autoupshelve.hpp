#ifndef __AUTO_UPSHELVE_HPP__
#define __AUTO_UPSHELVE_HPP__

#include "servercommon/struct/global/trademarketparam.hpp"

class AutoUpshelveInfo;
class AutoUpshelve
{
public:
	typedef std::map<long long, AutoUpshelveInfo*> AutoUpshelveInfoMap;

public:
	AutoUpshelve();
	~AutoUpshelve();

	void Update(time_t now);
	bool IsChanged() { return m_is_change; }
	void SaveDB();
	void OnInitFinish();

	bool AddAutoUpshelveInfo(const AutoUpshelveParam& param, bool is_init);
	bool RemoveAutoUpshelveInfo(long long unique_id);

	void OnItemSold(long long unique_id);
	void SetIsChange(bool is_change) { m_is_change = is_change; }
private:
	void GetChangeSaleItemList(ARG_OUT AutoUpshelveDataList* data);
	void ClearDirtyFlag(const AutoUpshelveDataList& data);
	void EraseAutoUpshelveInfo(long long unique_id);
	AutoUpshelveInfo* GetAutoUpshelveInfo(long long unique_id);
	void Release();

	AutoUpshelveInfoMap m_auto_info_map;

	bool m_init_finish;
	bool m_is_change;
	unsigned int m_next_save_time;
};

#endif