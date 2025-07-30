#ifndef __SALE_ITEM_BASE_HPP__
#define __SALE_ITEM_BASE_HPP__

#include "servercommon/struct/global/trademarketparam.hpp"

class SaleItemBase
{
public:
	SaleItemBase();
	virtual ~SaleItemBase();

	void * operator new(size_t c);
	void operator delete(void *m);

	void Init(int seller_uid, const char* seller_name, const SaleItemParam& param, bool is_from_focus);
	inline SaleItemParam& GetSaleItemParam() { return m_param; }
	inline const SaleItemParam& GetSaleItemParamConst() const { return m_param; }
	inline const char* GetSellerName() const { return m_seller_name; }
	inline int GetSellerUid() const { return m_seller_uid; }

	bool IsOnShelve() const { return SALE_ITEM_STATUS_ON_SHELVE == m_param.cur_status; }
	bool IsOnSale() const { return SALE_ITEM_STATUS_ON_SALE == m_param.cur_status; }
	bool IsAnnouncing() const { return SALE_ITEM_STATUS_ANNOUNCEMENT == m_param.cur_status; }
	bool IsOffShelve() const { return SALE_ITEM_STATUS_OFF_SHELVE == m_param.cur_status; }

	const char* GetUniqueKeyStr() const { return m_unique_key_str; }
protected:
	bool m_is_from_focus;
	int m_seller_uid;
	GameName m_seller_name;
	SaleItemParam m_param;

	UniqueKeyStr m_unique_key_str;
};

#endif