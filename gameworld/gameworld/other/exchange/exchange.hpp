#ifndef __EXCHANGE_HPP__
#define __EXCHANGE_HPP__

#include <map>
#include <vector>
#include "servercommon/exchangedef.h"

class RoleModuleManager;
class Exchange //!< \note 暂已砍掉  用RoleConvertShop代替
{
public:
	Exchange();
	~Exchange();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *role_module_manager, const ExchangeParam& param);
	void GetOtherInitParam(ExchangeParam* param);

	void OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid);
	void OnResetModuleDataWeek();


	void OnExchange(int seq, int num);
	
	void SendAllInfo();
	void SendSingelInfo(int limit_type, int seq);
private:
	RoleModuleManager * m_role_module_mgr;
	bool IsEnough(int score_type, int score_num);
	bool ConsumeScore(int score_type, Int64 score_num);

	std::map<int, ExchangeLimitData> m_limit_buy_count[SHOP_BUY_LIMIT_TYPE_MAX];	// key:商品seq
};


#endif // !__EXCHANGE_HPP__

