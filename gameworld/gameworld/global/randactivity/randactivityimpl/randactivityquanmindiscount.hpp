#ifndef __RAND_ACTIVITY_QUAN_MIN_DISCOUNT_HPP__
#define __RAND_ACTIVITY_QUAN_MIN_DISCOUNT_HPP__

#include "global/randactivity/randactivity.hpp"
#include "servercommon/roleactivity/raquanmindiscountparam.hpp"
#include <map>
#include <vector>

class RandActivityQuanMinDiscount : public RandActivity
{
public:
	RandActivityQuanMinDiscount(RandActivityManager *activity_manager);
	~RandActivityQuanMinDiscount();

	virtual void Init(const ActivityData &arg_data);
	virtual void GetInitParam(ActivityData & arg_data);

	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);

	int GetBuyCount(int seq) { return m_param.buy_count[seq]; }
	unsigned int GetNextAddTime(int seq) { return m_param.next_add_timestamp[seq]; }

private:
	void SetData();

	struct QuanMinDisocuntParam
	{
		QuanMinDisocuntParam() { this->Reset(); }

		void Reset()
		{
			memset(buy_count, 0, sizeof(buy_count));
			memset(next_add_timestamp, 0, sizeof(next_add_timestamp));
			memset(reserve_ll, 0, sizeof(reserve_ll));
			is_set_data_flag = 0;
			reserve_ch = 0;
			reserve_sh = 0;
		}

		int buy_count[QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM];
		unsigned int next_add_timestamp[QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM];
		long long reserve_ll[4];
		char is_set_data_flag;		//是否对错乱数据进行处理	0:未设置
		char reserve_ch;
		short reserve_sh;
	};

	QuanMinDisocuntParam m_param;									//需要保存到数据库中的数据

};
#endif

