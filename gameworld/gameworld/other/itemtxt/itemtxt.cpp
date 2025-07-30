#include "itemtxt.hpp"
#include "world.h"
#include "gamelog.h"
#include "gameworld/item/itembase.h"
#include "gameworld/item/itempool.h"
#include "checkresourcecenter.hpp"
#include <iostream>
#include <cmath>
#include <fstream>
#include <iomanip>


ItemTxt & ItemTxt::Instance()
{
	static ItemTxt ins;
	return ins;
}

void ItemTxt::OnWriet(char* path)
{
	m_path = path;
	if (0 == path)return;
	this->SetItemName();
}

ItemTxt::ItemTxt()
{
}

ItemTxt::~ItemTxt()
{
}


void ItemTxt::SetItemName()
{
	memset(m_item_name, 0, sizeof(m_item_name));

	for (int i = 0; i < TXT_MAX_ITEM_ID; ++i)
	{
		const ItemBase *base = ItemPool::Instance()->GetItem(i);
		if (NULL == base) continue;

		m_item_name[i] = base->GetItemName();
	}
	this->OutPutFile();
}

void ItemTxt::OutPutFile()
{
	std::ofstream outfile;
	outfile.open(m_path);
	outfile << std::setiosflags(std::ios::fixed);

	for (int i = 0; i < TXT_MAX_ITEM_ID; ++i)
	{
		if (m_item_name[i] == 0)continue;

		outfile << i << "|" << m_item_name[i] << std::endl;
	}

	outfile.close();
}
