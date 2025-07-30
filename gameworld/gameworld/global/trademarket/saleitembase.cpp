#include "saleitembase.hpp"
#include "trademarketmanager.hpp"

SaleItemBase::SaleItemBase() : m_is_from_focus(false), m_seller_uid(0)
{
	memset(m_seller_name, 0, sizeof(m_seller_name));
	memset(m_unique_key_str, 0, sizeof(m_unique_key_str));
}

SaleItemBase::~SaleItemBase()
{
	if (!m_is_from_focus)
	{
/*
#ifdef _DEBUG
		assert(false == TradeMarketManager::Instance().GetSaleItemIndex().IsItemInIndex(this));

		if (SALE_ITEM_STATUS_ON_SALE == m_param.cur_status)
		{
			TradeMarketManager::Instance().GetSaleItemIndex().OnRemoveItemOnSale(this);
		}
		else if (SALE_ITEM_STATUS_ANNOUNCEMENT == m_param.cur_status)
		{
			TradeMarketManager::Instance().GetSaleItemIndex().OnRemoveItemOnAnnounce(this);
		}
#endif
*/
	}
}

void SaleItemBase::Init(int seller_uid, const char* seller_name, const SaleItemParam& param, bool is_from_focus)
{
	m_is_from_focus = is_from_focus;
	m_seller_uid = seller_uid;
	F_STRNCPY(m_seller_name, seller_name, sizeof(GameName));
	m_param = param;

	ConvertUniqueKeyNumberToStr(param.unique_key, m_unique_key_str);
}

