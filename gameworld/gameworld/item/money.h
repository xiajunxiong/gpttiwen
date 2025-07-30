#ifndef __MONEY_H__
#define __MONEY_H__

#include "iclockmodule.h"
#include "servercommon/servercommon.h"
#include "servercommon/basedef.h"
#include "servercommon/struct/moneyotherdef.h"

class RoleModuleManager;
class Money
{
public:
	static const int BINDCOIN_FIRST = 1;						// 优先扣除绑定铜钱
	static const int NOBINDCOIN_FIRST = 2;						// 优先扣除非绑定铜钱
	static const int RAND_MONEY_MAX = 20;						// 获得金币时概率额外最多获得+20或者这-20

	Money();
	~Money();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* module_mgr, Int64 coin_bind, Int64 immortal_coin, Int64 gold);
	void InitOtherCoin(const MoneyOtherParam &param);
	void GetInitOtherCoin(MoneyOtherParam * param);

	Int64 GetCoinBind() const { return m_coin_bind; }
	Int64 GetImmortalCoin() const { return m_immortal_coin; }
	Int64 GetGold() const { return m_gold; }
	Int64 GetMoneyByType(int coin_type) const;

	bool CoinBindMoreThan(Int64 consume_coin_bind);
	bool ImmortalCoinMoreThan(Int64 consume_immortal_coin);
	bool GoldMoreThan(Int64 consume_gold);
	bool AllCoinMoreThan(Int64 consume_coin);
	bool MoneyTypeMoreThan(int type,Int64 num, bool is_noitce = true);

	bool UseGold(Int64 consume_gold, const char* type, bool is_consume_sys = true, bool is_use_in_cross = false, bool is_no_show_on_client = false);
	bool UseCoinBind(Int64 consume_coin_bind, const char* type, bool is_use_in_cross = false, bool is_no_show_on_client = false);
	bool UseImmortalCoin(Int64 consume_immortal_coin, const char* type, bool is_use_in_cross = false, bool is_no_show_on_client = false);
	bool UseAllCoin(Int64 consume_coin, const char* type, bool bind_first = true, Int64* use_bind_coin = NULL, Int64* use_immortal_coin = NULL, bool is_use_in_cross = false, bool is_no_show_on_client = false);
	bool UseMoneyType(int money_type, Int64 num, const char * type);

	bool Add(Int64 gold, Int64 immortal_coin, Int64 coin_bind, const char* type, bool is_use_in_cross = false, bool is_no_show_on_client = false);
	bool AddGold(Int64 gold, const char* type, bool is_chongzhi = false, bool is_use_in_cross = false, bool is_no_show_on_client = false);	//
	bool AddImmortalCoin(Int64 immortal_coin, const char* type, bool is_use_in_cross = false, bool is_no_show_on_client = false);		//仙币
	bool AddCoinBind(Int64 coin_bind, const char* type, bool is_use_in_cross = false, bool is_no_show_on_client = false);				//金币

	Int64 GetOtherCoin(int coin_type) const;
	bool OtherCoinMoreThan(Int64 consume_other_currency, int coin_type);
	bool UseOtherCoin(Int64 consume_other_currency, int coin_type, const char* type, bool is_use_in_cross = false, bool is_no_show_on_client = false);
	bool AddOtherCoin(Int64 add_other_currency, int coin_type, const char* type, bool is_use_in_cross = false, bool is_no_show_on_client = false);

	void GetAccountGoldFromDB(time_t now_second);
	void GetAccountGoldToRole(Int64 gold, int authority_type);

	void SysGuildContribution(int Contribution, bool is_no_show_on_client = false);

	void Send(int money_type, bool is_no_show_on_client = false);

	void ForceGetGold();
	unsigned int GetLastGetAccountTime() { return static_cast<unsigned>(m_last_account_get_time); }

	bool RetrieveMoney(Int64 gold_unbind, Int64 gold_bind, Int64 coin_bind, Int64 spirit_jade, int retrieve_reason);
	bool RetrieveMoney(MoneyOtherParam retrieve_other_currency, int retrieve_reason);

	bool OnAddMoney(int money_type, Int64 money_num, const char* type, bool is_use_in_cross = false, bool is_no_show_on_client = false);
	bool UseMoney(int money_type, Int64 money_num, const char* type, bool is_use_in_cross = false, bool is_no_show_on_client = false);

	void GmClearMoney(int money_type, const char *reason);
	void GmRetieveMoney(int role_id, int money_type, int money_num, int retiveve_type, int retrieve_reason);
private:
	RoleModuleManager* m_module_mgr;

	Int64	m_gold;
	Int64	m_coin_bind;
	Int64	m_immortal_coin;
	
	time_t m_last_account_get_time;

	// ------------------------------------------------
	// 都是不会出现在背包里的 , 但又都是兑换购买道具的货币[积分]
	MoneyOtherParam m_other_currency_param;							// 货币  \see MoneyType
};

#endif
