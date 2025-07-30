#ifndef __SYSTEM_SALE_ITEM_HPP__
#define __SYSTEM_SALE_ITEM_HPP__

#include "servercommon/struct/global/trademarketparam.hpp"
#include "saleitembase.hpp"

class SystemSaleItem : public SaleItemBase
{
public:
	SystemSaleItem();
	virtual ~SystemSaleItem();

	void * operator new(size_t c);
	void operator delete(void *m);
	
private:
	
};

#endif