#ifndef _TIAN_DI_DAO_HEN_CONFIG_HPP_
#define _TIAN_DI_DAO_HEN_CONFIG_HPP_

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"

#include <vector>
#include <map>

struct TianDiDaoHenCollectionSeqCfg
{
	struct ItemCfg
	{
		int need_item_id;
		int need_item_num;
		int reward_group_id;
	};

	int seq;
	int final_reward_group_id;
	int final_reward_group_id_2;
	std::vector<ItemCfg>  item_cfg;
};

struct TianDiDaoHenCollectionCfg
{
	struct ItemCfg
	{
		int need_item_id;
		int need_item_num;
		int reward_group_id;
	};

	std::vector<TianDiDaoHenCollectionSeqCfg> list_cfg;
};

struct TianDiDaoHenRewardGroupCfg
{
	TianDiDaoHenRewardGroupCfg()
	{
		rate_count = 0;
	}

	struct ItemCfg
	{
		int rate;
		std::vector<ItemConfigData> rewards;
	};

	int rate_count;
	std::vector<ItemCfg> list_cfg;
};

struct TianDiDaoHenOtherCfg
{
	int get_item_rate;
	std::vector<ItemConfigData> get_item_list;

	int begin_day;
	int open_during_day;
	int close_during_day;
};

class TianDiDaoHenConfig : public ILogicConfig
{
public:
	TianDiDaoHenConfig();
	~TianDiDaoHenConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	int GetRandSeq();
	const TianDiDaoHenCollectionSeqCfg * GetTianDiDaoHenCollectionSeqCfg(int seq);
	const TianDiDaoHenRewardGroupCfg::ItemCfg * GetRandReward(int group_id);
	ItemConfigData * GetRandDropItem();

	const TianDiDaoHenOtherCfg & GetOtherCfg()const;
private:
	int InitCollectCfg(TiXmlElement *RootElement);
	int InitOtherCfg(TiXmlElement * RootElement);
	int InitGroupRewardCfg(TiXmlElement * RootElement);
	 
	TianDiDaoHenCollectionCfg m_collection_cfg;
	TianDiDaoHenOtherCfg m_other_cfg;
	std::map<int, TianDiDaoHenRewardGroupCfg> m_reward_cfg;
};

#endif

