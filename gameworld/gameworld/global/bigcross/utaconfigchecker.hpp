#ifndef __UTA_CONFIG_CHECKER_HPP__
#define __UTA_CONFIG_CHECKER_HPP__

#include <string>
#include "servercommon/struct/itemlistparam.h"

class UTAConfig;
class UTAConfigChecker
{
public:
	bool Check(std::string& errmsg);

private:
	bool CheckItem(const std::vector<ItemConfigData>& item_list, std::string& errmsg);
	bool CheckWinBoxReward(const UTAConfig& uta_cfg, std::string& errmsg);
	bool CheckRankReward(const UTAConfig& uta_cfg, std::string& errmsg);
	bool CheckAdvanceReward(const UTAConfig& uta_cfg, std::string& errmsg);
	bool CheckTitleReward(ItemID title_item_id, std::string& errmsg);
	bool CheckOtherCfg(const UTAConfig& uta_cfg, std::string& errmsg);
};

#endif