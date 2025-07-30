#include "utaconfigchecker.hpp"
#include "servercommon/serverconfig/bigcrossconfig/utaconfig.hpp"
#include "item/itempool.h"
#include "item/itembase.h"
#include "item/expense/norexitem.hpp"
#include "config/logicconfigmanager.hpp"
#include "other/title/titleconfig.hpp"

bool UTAConfigChecker::Check(std::string& errmsg)
{
	const UTAConfig& uta_cfg = UTAConfig::Instance();
	if (!this->CheckOtherCfg(uta_cfg, errmsg))
	{
		return false;
	}

	if (!this->CheckWinBoxReward(uta_cfg, errmsg))
	{
		return false;
	}

	if (!this->CheckRankReward(uta_cfg, errmsg))
	{
		return false;
	}

	if (!this->CheckAdvanceReward(uta_cfg, errmsg))
	{
		return false;
	}

	return true;
}

bool UTAConfigChecker::CheckItem(const std::vector<ItemConfigData>& item_list, std::string& errmsg)
{
	for (size_t i = 0; i < item_list.size(); ++i)
	{
		if (!item_list[i].CheckConfig(errmsg))
		{
			return false;
		}
	}

	return true;
}

bool UTAConfigChecker::CheckWinBoxReward(const UTAConfig& uta_cfg, std::string& errmsg)
{
	for (size_t i = 0; i < uta_cfg.m_win_box_reward_cfg_list.size(); ++i)
	{
		std::string temperrormsg;
		if (!this->CheckItem(uta_cfg.m_win_box_reward_cfg_list[i].reward_list, temperrormsg))
		{
			temperrormsg = STRING_SPRINTF("UTAConfigChecker::CheckWinBoxReward Error[%s]", temperrormsg.c_str());
			return false;
		}
	}

	return true;
}

bool UTAConfigChecker::CheckRankReward(const UTAConfig& uta_cfg, std::string& errmsg)
{
	for (size_t i = 0; i < uta_cfg.m_rank_reward_cfg_list.size(); ++i)
	{
		std::string temperrormsg;
		if (!this->CheckItem(uta_cfg.m_rank_reward_cfg_list[i].reward_list, temperrormsg))
		{
			errmsg = STRING_SPRINTF("UTAConfigChecker::CheckRankReward Error[%s]", temperrormsg.c_str());
			return false;
		}

		if (!this->CheckTitleReward(uta_cfg.m_rank_reward_cfg_list[i].title_item_id, temperrormsg))
		{
			errmsg = STRING_SPRINTF("UTAConfigChecker::CheckRankReward_TitleItemError Error[%s]", temperrormsg.c_str());
			return false;
		}
	}

	if (uta_cfg.m_rank_reward_cfg_list.empty())
	{
		errmsg = STRING_SPRINTF("UTAConfigChecker::CheckRankReward Error[%s]", "rank_reward_cfg_list empty");
		return false;
	}

	if (uta_cfg.m_rank_reward_cfg_list[0].min_rank != 1)
	{
		errmsg = STRING_SPRINTF("UTAConfigChecker::CheckRankReward Error[%s]", "first min_rank != 1");
		return false;
	}

	for (size_t i = 1; i < uta_cfg.m_rank_reward_cfg_list.size(); ++i)
	{
		const UTARankCfg& cfg = uta_cfg.m_rank_reward_cfg_list[i];
		const UTARankCfg& last_cfg = uta_cfg.m_rank_reward_cfg_list[i - 1];
		if (cfg.type != last_cfg.type)
		{
			if (cfg.type < last_cfg.type)
			{
				errmsg = STRING_SPRINTF("UTAConfigChecker::CheckRankReward Error[%s]", "cfg.type < last_cfg.type");
				return false;
			}

			if (cfg.min_rank != 1)
			{
				errmsg = STRING_SPRINTF("UTAConfigChecker::CheckRankReward Error[%s]", "first min_rank != 1");
				return false;
			}

			continue;
		}

		if (cfg.min_rank != last_cfg.max_rank + 1)
		{
			errmsg = STRING_SPRINTF("UTAConfigChecker::CheckRankReward Error[%s]", "min_rank[%d] != last_max_rank[%d]", cfg.min_rank, last_cfg.max_rank);
			return false;
		}

		return true;
	}

	return true;
}

bool UTAConfigChecker::CheckAdvanceReward(const UTAConfig& uta_cfg, std::string& errmsg)
{
	for (size_t i = 0; i < uta_cfg.m_advance_reward_cfg_list.size(); ++i)
	{
		std::string temperrormsg;
		if (!this->CheckItem(uta_cfg.m_advance_reward_cfg_list[i].reward_list, temperrormsg))
		{
			temperrormsg = STRING_SPRINTF("UTAConfigChecker::CheckAdvanceReward Error[%s]", temperrormsg.c_str());
			return false;
		}
	}

	return true;
}

bool UTAConfigChecker::CheckTitleReward(ItemID title_item_id, std::string& errmsg)
{
	if (0 == title_item_id) return true;

	const ItemBase* item = ITEMPOOL->GetItem(title_item_id);
	if (NULL == item)
	{
		errmsg = STRING_SPRINTF("CheckTitleReward title_item[%d] not found", title_item_id);
		return false;
	}

	if (item->GetItemType() != I_TYPE_EXPENSE)
	{
		errmsg = STRING_SPRINTF("CheckTitleReward title_item[%d] not NorexItem", title_item_id);
		return false;
	}

	const NorexItem* norex_item = (const NorexItem*)item;
	if (norex_item->GetUseType() != NorexItem::I_NOREX_ITEM_LIMITED_TIME_TITLE_CARD)
	{
		errmsg = STRING_SPRINTF("CheckTitleReward title_item[%d] use_type muse be I_NOREX_ITEM_LIMITED_TIME_TITLE_CARD[%d]", 
			title_item_id, NorexItem::I_NOREX_ITEM_LIMITED_TIME_TITLE_CARD);
		return false;
	}

	int title_id = norex_item->GetParam2();
	const TitleCfg * title_cfg = LogicConfigManager::Instance()->GetTitleConfig()->GetTitleCfg(title_id);
	if (NULL == title_cfg)
	{
		errmsg = STRING_SPRINTF("CheckTitleReward title_item[%d] title_id[%d] TITLE_NOT_EXIST",
			title_item_id, title_id);
		return false;
	}

	if (title_cfg->time < 20 * SECOND_PER_DAY)
	{
		errmsg = STRING_SPRINTF("CheckTitleReward title_item[%d] title_id[%d] ¡®time' too small",
			title_item_id, title_id);
		return false;
	}

	return true;
}

bool UTAConfigChecker::CheckOtherCfg(const UTAConfig& uta_cfg, std::string& errmsg)
{
	for (size_t i = 0; i < uta_cfg.m_other_cfg.qualification_join_rewards.size(); ++i)
	{
		std::string temperrormsg;
		if (!this->CheckItem(uta_cfg.m_other_cfg.qualification_join_rewards, temperrormsg))
		{
			temperrormsg = STRING_SPRINTF("UTAConfigChecker::CheckOtherCfg Error[%s]", temperrormsg.c_str());
			return false;
		}
	}

	for (size_t i = 0; i < uta_cfg.m_other_cfg.active_rewards.size(); ++i)
	{
		std::string temperrormsg;
		if (!this->CheckItem(uta_cfg.m_other_cfg.active_rewards, temperrormsg))
		{
			temperrormsg = STRING_SPRINTF("UTAConfigChecker::CheckOtherCfg Error[%s]", temperrormsg.c_str());
			return false;
		}
	}

	return true;
}

