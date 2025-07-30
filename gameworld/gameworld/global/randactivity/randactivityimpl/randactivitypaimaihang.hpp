#ifndef __RAND_ACTIVITY_PAI_MAI_HANG_HPP__
#define __RAND_ACTIVITY_PAI_MAI_HANG_HPP__

#include "global/randactivity/randactivity.hpp"
#include "servercommon/roleactivity/rapaimaihangparam.hpp"

struct RAPaiMaiHangParam
{
	RAPaiMaiHangParam() { this->Reset(); }

	void Reset()
	{
		pai_mai_start_timestamp = 0;
		pai_mai_end_timestamp = 0;
		pai_mai_last_end_timestamp = 0;
		activity_open_day = 0;
		for (int i = 0; i < ARRAY_LENGTH(shop_item_list); ++i)
		{
			shop_item_list[i].Reset();
		}
	}

	unsigned int pai_mai_start_timestamp;				//竞拍开始时间戳
	unsigned int pai_mai_end_timestamp;					//竞拍结束时间戳
	unsigned int pai_mai_last_end_timestamp;			//竞拍最晚结束时间戳
	int activity_open_day;								//活动开启第几天
	
	RAPaiMaiHangSingleShopItemInfo shop_item_list[MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_NUM];	//商品信息
};

class RandActivityPaiMaiHang : public RandActivity
{
public:
	RandActivityPaiMaiHang(RandActivityManager *activity_manager);
	~RandActivityPaiMaiHang();

	virtual void Init(const ActivityData &arg_data);
	virtual void GetInitParam(ActivityData & arg_data);

	virtual void OnUserLogin(Role *user);

	virtual void Update(unsigned long interval, time_t now_second);
	virtual void OnDayChange(unsigned int old_dayid, unsigned int now_dayid);
	virtual void OnSpecialActivityStatusChange(int from_status, int to_status);

	bool IsBidTime(RAPaiMaiHangSingleShopItemInfo shop_item_info);
	RAPaiMaiHangSingleShopItemInfo * GetShopItemInfo(int seq);
	int GetActivityOpenDay() { return m_param.activity_open_day; }
	void OnSetEndTimestamp(RAPaiMaiHangSingleShopItemInfo & shop_item_info, int delay_time);
	void SendNoticeInfo(Role * user = NULL);
	void SendNoticeSingleInfo(int seq, Role * user = NULL);
private:

	void OnReCalcTime();
	void OnCheckTime(unsigned int now_second, bool is_close);

	RAPaiMaiHangParam m_param;
};


#endif