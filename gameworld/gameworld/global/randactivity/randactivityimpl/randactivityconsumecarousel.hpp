#ifndef __RAND_ACTIVITY_CONSUME_CAROUSEL_HPP__
#define __RAND_ACTIVITY_CONSUME_CAROUSEL_HPP__

#include "global/randactivity/randactivity.hpp"
#include "servercommon/roleactivity/raconsumecarouselparam.hpp"

class RandActivityConsumeCarousel : public RandActivity
{
public:
	RandActivityConsumeCarousel(RandActivityManager *activity_manager);
	~RandActivityConsumeCarousel();

	virtual void Init(const ActivityData &data);
	virtual void GetInitParam(ActivityData & data);
	virtual void SyncActivityDataFromCross(CrossRandActivityDataParam &param);

	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);
	virtual bool CanSyncActivityDataToCross();

	void OnAddShowListInfo(RAShowInfoParam::RAShowInfoDataParam * show_data, bool is_sync_from_cross);

	void GetRAShowInfoData(RAShowInfoParam::RAShowInfoDataParam* ra_show_data_list, int len);
	int GetRAShowInfoCount() { return m_param.count; }

private:
	RAShowInfoParam m_param;

};



#endif
