#ifndef MSGPRESTIGE_H
#define MSGPRESTIGE_H

#include "gamedef.h"
#include "servercommon/prestigedef.hpp"
#include "servercommon/commondata.hpp"
#include "servercommon/userprotocal/msgheader.h"

#pragma pack(push) 
#pragma pack(4)

namespace Protocol
{
	struct PrestigeVillageInfo
	{
		PrestigeVillageInfo(){}

		PrestigeVillageInfo& operator=(const PrestigeVillageNode & param)
		{
			this->prestige = param.prestige;
			this->level = param.level;
			return *this;
		}

		int prestige;
		int level;
	};

	class SCPrestigeAllVillageInfo			//!< 1800 全部村子的信息
	{
	public:
		SCPrestigeAllVillageInfo();
		MessageHeader		header;

		int count;						//!< 有多少个村子
		PrestigeVillageInfo prestige_village_list[PRESTIGE_VILLAGE_MAX_NUM];
	};

	class SCPrestigeSingleVillageInfo			//!< 1802  单个村子的信息
	{
	public:
		SCPrestigeSingleVillageInfo();
		MessageHeader		header;

		int index;						//!< 村子的id
		PrestigeVillageInfo prestige_village;
	};

	class SCPrestigeAllShopInfo			//!< 1801 全部商品的信息
	{
	public:
		SCPrestigeAllShopInfo();
		MessageHeader		header;

		// 限购次数
		unsigned short buy_count[NEW_PRESTIGE_VILLAGE_GOODS_MAX_NUM];		//!< 购买次数
	};

	class SCPrestigeSingleShopInfo			//!< 1803 单个商品的信息
	{
	public:
		SCPrestigeSingleShopInfo();
		MessageHeader		header;

		// 限购次数
		short index;				//!< 商品的index
		short buy_count;			//!< 购买次数
	};

	class CSPrestigeShopingReq			//!< 1804 购买请求
	{
	public:
		CSPrestigeShopingReq();
		MessageHeader		header;

		int index;				//!< 商品的index
		int num;					//!< 购买的数量
	};

	class SCPrestigeDonateInfo			//!< 1805	捐赠信息
	{
	public:
		SCPrestigeDonateInfo();
		MessageHeader		header;

		int today_donate_count;			//!< 今日捐赠了多少件
		int today_add_value;			//!< 今日获得了多少声望
	};

	class SCPrestigeDonateNotice			//!< 1807	捐赠通知
	{
	public:
		SCPrestigeDonateNotice();
		MessageHeader		header;

		int today_donate_count;
	};


	class CSPrestigeDonateOp			//!< 1806	捐赠  
	{
	public:
		CSPrestigeDonateOp();
		MessageHeader		header;

		int village_id;		//!<村子id
		int column;			//!<背包类型
		int index;			//!<背包格子
	};

	class SCPrestigeChangeNotice			//!< 1808	声望变动通知
	{
	public:
		SCPrestigeChangeNotice();
		MessageHeader		header;

		int village_id;		//!<村子id
		int param;			//!<参数（声望增加值/声望等级）
	};

	
}
#pragma pack(pop)

#endif // !MSGPRESTIGE_H
