#ifndef __RA_QUAN_MIN_DISCOUNT_MANAGER_HPP__
#define __RA_QUAN_MIN_DISCOUNT_MANAGER_HPP__

#include "servercommon/servercommon.h"
#include "global/randactivity/randactivity.hpp"
#include "servercommon/roleactivity/raquanmindiscountparam.hpp"

static const int QM_DISCOUNT_MAX_UID = QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM * QUAN_MIN_DISCOUNT_DISCOUNT_MAX_NUM;

struct BuyListInfo
{
	BuyListInfo() { this->Reset(); }

	void Reset()
	{
		uid = 0;
		buy_count = 0;
	}

	int uid;
	int buy_count;
};
struct CommodityInfo
{
	CommodityInfo() { this->Reset(); }

	void Reset()
	{
		cur_discount = 10;
		next_add_timestamp = 0;
		buy_count = 0;
		memset(buy_list, 0, sizeof(buy_list));
	}

	double cur_discount;											//当前折扣
	unsigned int next_add_timestamp;								//下次增加人数的时间戳
	int buy_count;													//每件商品购买的人数
	BuyListInfo buy_list[QUAN_MIN_DISCOUNT_DISCOUNT_MAX_NUM];		//每件商品 *最低折扣* 前购买的uid
};

class RAQuanMinDiscountManager
{
public:
	static RAQuanMinDiscountManager& Instance();

	void Init(int ret, const QuanMinDiscountParam &param_list);
	void Save();

	bool OnServerStart();
	void OnServerStop();
	void Update(unsigned long interval, time_t now_second);
	void OnSpecialActivityStatusChange(int from_status, int to_status);

	void OnBuyCommodity(int seq, int uid, bool is_max_discount);

	int GetBuyCount(int seq);
	unsigned int GetNextAddTime(int seq);
	double GetCommodityDiscout(int seq);
	void SetInfoCountAndTime();
	bool CheckHasUid(int seq, int uid);

	bool IsLoadFinish() { return m_is_load_finish; }

	void OnSyncInfoFromGame(int seq, double discount, int buy_count);

private:
	RAQuanMinDiscountManager();
	~RAQuanMinDiscountManager();

	void CalcDiscount(int seq, bool isnt_init = false);
	void OnDiscountChange(int seq, double original_discount);

	bool LoadData(long long id_from);

	bool m_is_load_finish;
	bool m_is_change;
	time_t m_last_save_time;
	int m_status;
	
	CommodityInfo m_info[QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM];
	QuanMinDiscountParam::RoleData m_uid_list[QM_DISCOUNT_MAX_UID];				//save()时 暂存用
};

#endif
