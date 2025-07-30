#ifndef __SPIDCONFIG_H__
#define __SPIDCONFIG_H__

#include <string>
#include <map>
#include "servercommon/serverconfig/localconfig.hpp"
#include "servercommon/configcommon.h"

static const int SECTION_MAX_COUNT = 20;
namespace globalconfig
{
	struct SpidConfigItem
	{
		SpidConfigItem() : spid(0), talk_level(0), talk_level_xianmeng(0), talk_level_siliao(0), 
			talk_level_cross(0), speaker_level(0), cross_speaker_level(0), create_group_level(0),
			single_talk_num_lv(0), single_talk_num_time(0), single_talk_num_num(0), forbidden_tips_level(0),
			day_trading_up(0), is_sell(0)
		{
		}

		int spid;
		int talk_level;
		int talk_level_xianmeng;
		int talk_level_siliao;
		int talk_level_cross;
		int speaker_level;
		int cross_speaker_level;
		int create_group_level;
		int single_talk_num_lv;
		int single_talk_num_time;
		int single_talk_num_num;
		int forbidden_tips_level;
		int day_trading_up;
		int is_sell;
		std::string plat_name;
		std::vector<ItemConfigData> real_name_rewards;
	};

	struct ChatLimitCfg
	{
		ChatLimitCfg() : spid(0), day_start(0), day_end(0), talk_level(0),talk_level_xianmeng(0),talk_level_siliao(0){}

		int spid;
		int day_start;
		int day_end;
		int talk_level;
		int talk_level_xianmeng;
		int talk_level_siliao;
	};
	class SpidConfig
	{
	public:
		SpidConfig();
		~SpidConfig();

		bool Init(const std::string& path, std::string* err);

		int GetTalkLevel();
		int GetTalkLevelXianMeng();
		int GetTalkLevelSiLiao();
		int GetTalkLevelCross();
		int GetSpeakerLevel();
		int GetCrossSpeakerLevel();
		int GetCreateChatGroupLevel();
		int GetSingleChatNumLevel();
		int GetSingleChatNumTime();
		int GetSingleChatNumNum();
		int GetBuyLimit();
		int GetForbidTalkTipsLevel();
		bool IsCanSell();
		const std::vector<ItemConfigData>* GetRealNameReward();

		static int GetSpidByPlatName(PlatName plat_name);
		static int GetSpidByStrPlatName(const std::string& str_plat_name);
		static bool GetPlatNameBySpid(int spid, ARG_OUT char platname[sizeof(int)]);

	private:
		int InitSpidConfig(TiXmlElement *RootElement);
		int InitCharLimitByOpenServer(TiXmlElement *RootElement);
		int GetServerOpenDays();

		typedef std::map<int, SpidConfigItem> SpidConfigItemMap;
		SpidConfigItemMap m_spid_config_item_map;

		int new_server_open_days;
		int old_server_open_days;
		int talk_level;
		int talk_level_xianmeng;
		int talk_level_siliao;
		int m_is_open_chat_auto_forbid;
		int m_dev_spid;
		std::map<int,ChatLimitCfg> m_chat_limit_section_cfg;
	};

};

#endif
