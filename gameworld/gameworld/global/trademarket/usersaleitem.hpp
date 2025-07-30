#ifndef __SALE_ITEM_HPP__
#define __SALE_ITEM_HPP__

#include "servercommon/struct/global/trademarketparam.hpp"
#include "saleitembase.hpp"

class UserSaleItem : public SaleItemBase
{
public:
	UserSaleItem();
	virtual ~UserSaleItem();

	void * operator new(size_t c);
	void operator delete(void *m);
	
	void OnUpdateFocus(unsigned int now) { m_last_update_focus_time = now; }
	unsigned int GetLastUpdateFocusTime() const { return m_last_update_focus_time; }

private:
	unsigned int m_last_update_focus_time;
};

#endif