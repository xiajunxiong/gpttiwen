#ifndef __DREAM_NOTES_CHALLENGE_CONFIG_HPP__
#define __DREAM_NOTES_CHALLENGE_CONFIG_HPP__

#include "servercommon/configcommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "gameworld/gamedef.h"

struct DreamNotesChallengeOtherCfg
{
	DreamNotesChallengeOtherCfg() : pass_time_limit(0), extra_purchase_time(0) {}

	int pass_time_limit;
	int extra_purchase_time;
};

struct DreamNotesChallengeCfg
{
	DreamNotesChallengeCfg()
	{
		level = 0;
		recommend_level = 0;
		recommend_score = 0;
		scene_id = 0;
		boss_id = 0;
		boss_pos = Posi(0, 0);
		boss_dir = 0;
		partner_fragment_rate = 0;
		first_yuanling = 0;
		first_lucky = 0;
		coin = 0;
		lucky = 0;
		common_yuanling = 0;
		npc_1_id = 0;
		npc_1_pos = Posi(0, 0);
		npc_1_dir = 0;
		is_need_talk_1 = 0;
		npc_2_id = 0;
		npc_2_pos = Posi(0, 0);
		npc_2_dir = 0;
		is_need_talk_2 = 0;
		npc_3_id = 0;
		npc_3_pos = Posi(0, 0);
		npc_3_dir = 0;
		is_need_talk_3 = 0;
		power_expend = 0;
	}

	int level;
	int recommend_level;
	int recommend_score;
	int scene_id;
	int boss_id;
	Posi boss_pos;
	Dir boss_dir;
	std::vector<ItemConfigData> first_pass_reward;
	std::vector<ItemConfigData> common_reward;
	int partner_fragment_rate;
	std::vector<ItemConfigData> partner_fragment_reward;
	int first_yuanling;
	int first_lucky;		
	int coin;				//∆’Õ®Ω±¿¯
	int lucky;				//∆’Õ®Ω±¿¯
	int common_yuanling;	//∆’Õ®Ω±¿¯
	int npc_1_id;
	Posi npc_1_pos;
	Dir npc_1_dir;
	int is_need_talk_1;
	int npc_2_id;
	Posi npc_2_pos;
	Dir npc_2_dir;
	int is_need_talk_2;
	int npc_3_id;
	Posi npc_3_pos;
	Dir npc_3_dir;
	int is_need_talk_3;
	int power_expend;
};

struct DreamNotesChallengeBuyCfg
{
	DreamNotesChallengeBuyCfg() : price_type(0), price(0)
	{}

	int price_type;
	int price;
};

class DreamNotesChallengeConfig : public ILogicConfig
{
public:
	DreamNotesChallengeConfig();
	virtual ~DreamNotesChallengeConfig();

	virtual bool Init(const std::string &configname, std::string *err);

	const DreamNotesChallengeOtherCfg & GetOtherCfg() { return m_other_cfg; }
	const DreamNotesChallengeCfg * GetChallengeFbCfg(int level);
	int GetChallengeExpCfg(int level);
	int GetMaxLevel() { return max_level; }
	const DreamNotesChallengeBuyCfg * GetPriceByBuyTimes(int buy_times);
	int GetMaxBuyTimes();

private:
	int InitOtherCfg(TiXmlElement *RootElement);
	int InitChallengeFbCfg(TiXmlElement *RootElement);
	int InitChallengeExpCfg(TiXmlElement *RootElement);
	int InitExtraBuyCfg(TiXmlElement *RootElement);

	DreamNotesChallengeOtherCfg m_other_cfg;
	int max_level;
	std::vector<DreamNotesChallengeCfg> m_challenge_fb_cfg;
	std::map<int, int> m_challenge_exp_cfg;							//æ≠—ÈΩ±¿¯≈‰÷√

	std::map<int, DreamNotesChallengeBuyCfg> m_extra_buy_cfg;		//key:buy_times

};



#endif