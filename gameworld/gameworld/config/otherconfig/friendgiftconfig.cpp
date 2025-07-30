#include "friendgiftconfig.hpp"

#include "gameworld/item/itembase.h"

FriendGiftConfig::FriendGiftConfig()
{

}

FriendGiftConfig::~FriendGiftConfig()
{

}

bool FriendGiftConfig::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG_2("other", InitOtherCfg);
	LOAD_CONFIG_2("send_flowers", InitSendFlowersCfg);
	LOAD_CONFIG_2("exclude_fight_type", InitExcludeFightTypeCfg);

	return true;
}

const FriendGiftSendFlowerCfg * FriendGiftConfig::GetSendFlowerCfg(int _item_id) const
{
	std::map<int, FriendGiftSendFlowerCfg>::const_iterator it = m_send_flowers_cfg_map.find(_item_id);
	if (m_send_flowers_cfg_map.end() == it)
		return NULL;

	return &it->second;
}

bool FriendGiftConfig::IsExcludeFightType(int _type)
{
	std::set<int >::iterator it = m_exclude_fight_type_set.find(_type);
	if (m_exclude_fight_type_set.end() != it)
	{
		return true;
	}
	return false;
}

int FriendGiftConfig::InitOtherCfg(TiXmlElement* RootElement, std::string& errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	if (!GetSubNodeValue(root_element, "hook_add", m_other_cfg.hook_add) || m_other_cfg.hook_add < 0)
	{
		errormsg = STRING_SPRINTF("[hook_add=%d] error, hook_add < 0", m_other_cfg.hook_add);
		return -1;
	}

	if (!GetSubNodeValue(root_element, "first_talk_add", m_other_cfg.first_talk_add) || m_other_cfg.first_talk_add < 0)
	{
		errormsg = STRING_SPRINTF("[first_talk_add=%d] error, first_talk_add < 0", m_other_cfg.first_talk_add);
		return -2;
	}

	if (!GetSubNodeValue(root_element, "intimacy_max", m_other_cfg.intimacy_max) || m_other_cfg.intimacy_max <= 0)
	{
		errormsg = STRING_SPRINTF("[intimacy_max=%d] error, intimacy_max <= 0", m_other_cfg.intimacy_max);
		return -3;
	}

	return 0;
}

int FriendGiftConfig::InitSendFlowersCfg(TiXmlElement* RootElement, std::string& errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int item_id = 0;
		if (!GetSubNodeValue(root_element, "item_id", item_id) || NULL == ITEMPOOL->GetItem(item_id))
		{
			errormsg = STRING_SPRINTF("[item_id=%d] error, NULL == ITEMPOOL->GetItem(item_id)", item_id);
			return -1;
		}

		FriendGiftSendFlowerCfg & temp_cfg = m_send_flowers_cfg_map[item_id];

		if (!GetSubNodeValue(root_element, "intimacy_add", temp_cfg.intimacy_add) || temp_cfg.intimacy_add < 0)
		{
			errormsg = STRING_SPRINTF("[intimacy_add=%d] error, temp_cfg.intimacy_add < 0 ", temp_cfg.intimacy_add);
			return -2;
		}

		if (!GetSubNodeValue(root_element, "type", temp_cfg.type) || temp_cfg.type <= SEND_FLOWER_TYPE_INVALID || temp_cfg.type >= SEND_FLOWER_TYPE_MAX)
		{
			errormsg = STRING_SPRINTF("[type=%d] error, The range of values of type(%d - %d) ", temp_cfg.type , SEND_FLOWER_TYPE_INVALID , SEND_FLOWER_TYPE_MAX);
			return -3;
		}

		if (!GetSubNodeValue(root_element, "fast_buy", temp_cfg.is_fast_buy))
		{
			errormsg = STRING_SPRINTF("error, [fast_buy] Node does not exist");
			return -4;
		}

		if (!GetSubNodeValue(root_element, "batch", temp_cfg.is_batch))
		{
			errormsg = STRING_SPRINTF("error, [batch] Node does not exist");
			return -5;
		}

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

int FriendGiftConfig::InitExcludeFightTypeCfg(TiXmlElement* RootElement, std::string& errormsg)
{
	TiXmlElement * root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		int battle_type = 0;
		if (!GetSubNodeValue(root_element, "type", battle_type) || battle_type <= BATTLE_MODE_INVALID || battle_type >= BATTLE_MODE_MAX)
		{
			errormsg = STRING_SPRINTF("[type=%d] error,  The value range should be  battle_type > %d && battle_type < %d ", battle_type , BATTLE_MODE_INVALID , BATTLE_MODE_MAX);
			return -1;
		}

		m_exclude_fight_type_set.insert(battle_type);

		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

