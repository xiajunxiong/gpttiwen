#ifndef __RAND_ACTIVITY_XIAN_SHI_LEI_CHONG_HPP__
#define __RAND_ACTIVITY_XIAN_SHI_LEI_CHONG_HPP__

#include "global/randactivity/randactivity.hpp"

class RandActivityXianShiLeiChong : public RandActivity
{
public:
	RandActivityXianShiLeiChong(RandActivityManager * activity_manager);
	~RandActivityXianShiLeiChong();

	virtual void Init(const ActivityData &arg_data);
	virtual void GetInitParam(ActivityData & arg_data);
	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);
	virtual void SyncActivityDataFromCross(CrossRandActivityDataParam &param);

	int GetOpenDay();
private:
	struct RAServerXianShiLeiChongParam
	{
		RAServerXianShiLeiChongParam() { this->Reset(); }

		void Reset()
		{
			open_day = 1;
		}
		int open_day;												//活动开启时的开服时间
	};

	RAServerXianShiLeiChongParam m_param;
};


#endif