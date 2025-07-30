#include "historytopprice.hpp"
#include "item/itempool.h"
#include "item/itembase.h"
#include "servercommon/struct/itemlistparam.h"

HistoryTopPrice::HistoryTopPrice() : m_is_change(false), m_info_num(0)
{
	memset(m_info_list, 0, sizeof(m_info_list));
}

HistoryTopPrice::~HistoryTopPrice()
{

}

void HistoryTopPrice::Init(const HistoryTopPriceParam& param)
{
	m_info_num = 0;
	for (int i = 0; i < ARRAY_LENGTH(param.price_list) && m_info_num < ARRAY_LENGTH(m_info_list); ++i)
	{
		m_info_list[m_info_num++] = param.price_list[i];
	}
}

void HistoryTopPrice::GetInitParam(ARG_OUT HistoryTopPriceParam& param)
{
	for (int i = 0; i < m_info_num && i < ARRAY_LENGTH(param.price_list); ++i)
	{
		param.price_list[i] = m_info_list[i];
	}
}

void HistoryTopPrice::OnItemSold(int price, const ItemDataWrapper& wrapper)
{
	const ItemBase* item = ITEMPOOL->GetItem(wrapper.item_id);
	if (NULL == item) return;

	if (!item->IsNetValueItem())
	{
		for (int i = 0; i < m_info_num; ++i)
		{
			if (m_info_list[i].wrapper.item_id == wrapper.item_id)
			{
				if (m_info_list[i].price < price)
				{
					m_info_list[i].price = price;
					this->SetDirty();
				}
				return; // 非净值数据，若找到更高价格，就更新价格，然后退出
			}
		}
	}

	if (!this->IsListFull())
	{
		m_info_list[m_info_num].price = price;
		m_info_list[m_info_num].wrapper = wrapper;
		++m_info_num;
		this->SetDirty();
	}
	else
	{
		if (this->ReplaceLowestPriceItem(price, wrapper))
		{
			this->SetDirty();
		}
	}
}

void HistoryTopPrice::GetTopPriceItemList(int max_list_size, ARG_OUT int* num, ARG_OUT ItemDataWrapper* item_list)
{
	if (NULL == num || NULL == item_list) return;

	*num = 0;
	for (int i = 0; i < m_info_num && *num < max_list_size; ++i)
	{
		item_list[*num] = m_info_list[i].wrapper;
		++*num;
	}
}

bool HistoryTopPrice::IsListFull()
{
	return m_info_num >= ARRAY_LENGTH(m_info_list);
}

bool HistoryTopPrice::ReplaceLowestPriceItem(int price, const ItemDataWrapper& wrapper)
{
	int lowest_idx = -1;
	int lowest_price = MAX_INT;
	for (int i = 0; i < m_info_num; ++i)
	{
		if (m_info_list[i].price < lowest_price)
		{
			lowest_price = m_info_list[i].price;
			lowest_idx = i;
		}
	}

	if (-1 == lowest_idx)
	{
		return false;
	}

	m_info_list[lowest_idx].price = price;
	m_info_list[lowest_idx].wrapper = wrapper;

	return true;
}

void HistoryTopPrice::SetDirty()
{
	m_is_change = true;
}

