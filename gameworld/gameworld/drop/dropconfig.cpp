#include "dropconfig.hpp"

#include "servercommon/configcommon.h"

#include "item/itempool.h"
#include "item/itemgriddata.h"
#include "item/itembase.h"
#include "globalconfig/globalconfig.h"

DropConfig::DropConfig()
	: m_drop_id(0), m_drop_item_total_count(0), m_drop_item_prob_count(0)
{

}

DropConfig::~DropConfig()
{

}

bool DropConfig::LoadConfig(const char *path, std::string *err)
{
	char errinfo[1024] = {0};

	TiXmlDocument document;

	if (NULL == path || !document.LoadFile(path))
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: Load Drop Config Error, %s.", path, document.ErrorDesc());
		*err = errinfo;
		return false;
	}

	TiXmlElement *RootElement = document.RootElement();
	if (NULL == RootElement)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in root.", path);
		*err = errinfo;
		return false;
	}

	if (!GetSubNodeValue(RootElement, "drop_id", m_drop_id) || m_drop_id <= 0)
	{
		SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [droper->drop_id(%d)]", path, m_drop_id);
		*err = errinfo;
		return false;
	}

	{
		TiXmlElement *drop_item_prob_list_Element = RootElement->FirstChildElement("drop_item_prob_list");
		if (!drop_item_prob_list_Element)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [drop_item_prob_list].", path);
			*err = errinfo;
			return false;
		}

		int total_prob = 0;

		TiXmlElement *drop_item_prob_Element = drop_item_prob_list_Element->FirstChildElement("drop_item_prob");
		while (NULL != drop_item_prob_Element)
		{
			if (m_drop_item_total_count >= DROP_ITEM_PROB_MAX)
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [droper->drop_item_prob_list->drop_item_prob_count(%d)]", path, m_drop_item_prob_count);
				*err = errinfo;
				return false;
			}

			DropItemCfg *drop_item_prob = &m_drop_item_prob_list[m_drop_item_total_count];
			if (!GetSubNodeValue(drop_item_prob_Element, "item_id", drop_item_prob->item_config_data.item_id))
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [droper->drop_item_prob_list->drop_item_prob->item_id(%d)]", path, drop_item_prob->item_config_data.item_id);
				*err = errinfo;
				return false;
			}

			if (!GetSubNodeValue(drop_item_prob_Element, "num", drop_item_prob->item_config_data.num) || drop_item_prob->item_config_data.num <= 0)
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [droper->drop_item_prob_list->drop_item_prob->num(%d),id(%d)]", path, (int)drop_item_prob->item_config_data.num, drop_item_prob->item_config_data.item_id);
				*err = errinfo;
				return false;
			}

			if (COIN_ITEM_ID != drop_item_prob->item_config_data.item_id)
			{
				const ItemBase *itembase = ITEMPOOL->GetItem(drop_item_prob->item_config_data.item_id);
				if (NULL == itembase)
				{
					SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [droper->drop_item_prob_list->drop_item_prob->item_id(%d) not exist]", path, (int)drop_item_prob->item_config_data.item_id);
					*err = errinfo;
					return false;
				}

				if (drop_item_prob->item_config_data.num > itembase->GetPileLimit())
				{
					SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [droper->drop_item_prob_list->drop_item_prob->num(%d),id(%d)]", path, (int)drop_item_prob->item_config_data.num, drop_item_prob->item_config_data.item_id);
					*err = errinfo;
					return false;
				}

				if (!itembase->CanMerge() && drop_item_prob->item_config_data.num > 1)
				{
					SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [droper->drop_item_prob_list->drop_item_prob->num(%d)]", path, (int)drop_item_prob->item_config_data.num);
					*err = errinfo;
					return false;
				}
			}
			
			if (!GetSubNodeValue(drop_item_prob_Element, "is_bind", drop_item_prob->item_config_data.is_bind))
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [droper->drop_item_prob_list->drop_item_prob->is_bind(%d)]", path, (int)drop_item_prob->item_config_data.is_bind);
				*err = errinfo;
				return false;
			}

			if (!GetSubNodeValue(drop_item_prob_Element, "prob", drop_item_prob->prob))
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [droper->drop_item_prob_list->drop_item_prob->prob(%d)]", path, drop_item_prob->prob);
				*err = errinfo;
				return false;
			}

			int is_broadcast = 0;
			if (!GetSubNodeValue(drop_item_prob_Element, "broadcast", is_broadcast))
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [droper->drop_item_prob_list->drop_item_prob->broadcast(%d)]", path, is_broadcast);
				*err = errinfo;
				return false;
			}

			drop_item_prob->broadcast = (1 == is_broadcast);

			int is_active_limit = 0;
			if (!GetSubNodeValue(drop_item_prob_Element, "is_active_limit", is_active_limit))
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [droper->drop_item_prob_list->drop_item_prob->is_active_limit(%d)]", 
					path, is_active_limit);
				*err = errinfo;
				return false;
			}

			drop_item_prob->is_active_limit = (1 == is_active_limit);

			if (DROP_CERTAIN_PROB != drop_item_prob->prob)
			{
				drop_item_prob->prob += total_prob;
				total_prob = drop_item_prob->prob;

				++ m_drop_item_prob_count;
			}

			++ m_drop_item_total_count;
			if (m_drop_item_total_count != m_drop_item_prob_count && DROP_CERTAIN_PROB != drop_item_prob->prob)
			{
				SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [droper->drop_item_prob_list->drop_item_prob-> (m_drop_item_total_count != m_drop_item_prob_count && DROP_CERTAIN_PROB != drop_item_prob->prob)]", path);
				*err = errinfo;
				return false;
			}

			drop_item_prob_Element = drop_item_prob_Element->NextSiblingElement();
		}

		if (total_prob > 10000)
		{
			SNPRINTF(errinfo, sizeof(errinfo), "%s: xml node error in node [droper->drop_item_prob_list->total_prob(%d) more than 10000]", path, total_prob);
			*err = errinfo;
			return false;
		}
	}

	return true;
}

int DropConfig::RandDropItem(DropItemCfg item_list[DROP_ITEM_PROB_MAX])
{
	int count = 0;

	{
		int item_rand_i = 0, item_rand_prob = rand() % 10000;

		for (item_rand_i = 0; item_rand_i < m_drop_item_prob_count; ++ item_rand_i)
		{
			if (m_drop_item_prob_list[item_rand_i].prob > item_rand_prob)
			{
				break;
			}
		}
		if (item_rand_i < 0 || item_rand_i >= m_drop_item_prob_count) return 0;

		item_list[count ++] = m_drop_item_prob_list[item_rand_i];
	}

	return count;
}

int DropConfig::GetFixDropItem(int rand_count, DropItemCfg item_list[DROP_ITEM_PROB_MAX])
{
	if (NULL == item_list || rand_count < 0) return 0;
	if (rand_count >= DROP_ITEM_PROB_MAX) return rand_count;

	int count = rand_count;

	for (int i = m_drop_item_prob_count; i < m_drop_item_total_count; i++)
	{
		item_list[count++] = m_drop_item_prob_list[i];
		if (count >= DROP_ITEM_PROB_MAX)
		{
			break;
		}
	}

	return count;
}
