#include "randactivityconfigmanager.hpp"

#include "servercommon/servercommon.h"
#include "servercommon/fabaodef.hpp"
#include "servercommon/partnerdef.h"
#include "servercommon/smartmountssystemdef.hpp"
#include "other/collection/collectionconfig.hpp"
#include "other/pet/petconfig.hpp"
#include "config/activityconfig/activitybigdipper/activitybigdipperconfig.hpp"

#include "global/randactivity/randactivitymanager.hpp"
#include "config/randactivityconfig/impl/randactivitytimelimitstoreconfig.hpp"
#include "config/randactivityconfig/impl/randactivitytruntableconfig.hpp"
#include "config/randactivityconfig/impl/randactivitydiscountstoreconfig.hpp"
#include "config/randactivityconfig/impl/randactivityskydropluxurygiftconfig.hpp"
#include "config/randactivityconfig/impl/randactivityexpelmonsterconfig.hpp"
#include "config/randactivityconfig/impl/randactivityheritagetreasureconfig.hpp"
#include "config/randactivityconfig/impl/randactivitysevencolorpictureconfig.hpp"
#include "config/randactivityconfig/impl/randactivitysaleshopconfig.hpp"
#include "config/randactivityconfig/impl/randactivityanimalraceconfig.hpp"
#include "config/randactivityconfig/impl/randactivityluckybagconfig.hpp"
#include "config/randactivityconfig/impl/randactivityreducetaxrateconfig.hpp"
#include "config/randactivityconfig/impl/randactivitytianjianghaoliconfig.hpp"
#include "config/randactivityconfig/impl/randactivitysealpromoteconfig.hpp"
#include "config/randactivityconfig/impl/randactivitydropdoubleconfig.hpp"
#include "config/randactivityconfig/impl/randactivityjoyseeksfavorsconfig.hpp"
#include "config/randactivityconfig/impl/randactivitycollectwordconfig.hpp"
#include "config/randactivityconfig/impl/randactivitydiscountcarnivalconfig.hpp"
#include "config/randactivityconfig/impl/randactivitypartnerexchangeconfig.hpp"
#include "config/randactivityconfig/impl/randactivityluckydiscountconfig.hpp"
#include "config/randactivityconfig/impl/randactivitypetexchangeconfig.hpp"
#include "config/randactivityconfig/impl/randactivityjiandanqinxinconfig.hpp"
#include "config/randactivityconfig/impl/randactivityadventuregiftconfig.hpp"
#include "config/randactivityconfig/impl/randactivitylinkagegiftconfig.hpp"
#include "config/randactivityconfig/impl/randactivitysportsconfig.hpp"
#include "config/randactivityconfig/impl/randactivityluckyturntableconfig.hpp"
#include "config/randactivityconfig/impl/randactivitytianmingdivinationconfig.hpp"
#include "config/randactivityconfig/impl/randactivityquanmindiscountconfig.hpp"
#include "config/randactivityconfig/impl/randactivitytujiantreasureconfig.hpp"
#include "config/randactivityconfig/impl/randactivitygodtokenconfig.hpp"
#include "config/randactivityconfig/impl/randactivityelementcardsconfig.hpp"
#include "config/randactivityconfig/impl/randactivitysignincontinuityconfig.hpp"
#include "config/randactivityconfig/impl/randactivityconsumecarouselconfig.hpp"
#include "config/randactivityconfig/impl/randactivityhappyupconfig.hpp"
#include "config/randactivityconfig/impl/randactivitycaishenjubaoconfig.hpp"
#include "config/randactivityconfig/impl/randactivitydingzhibaoxiangconfig.hpp"
#include "config/randactivityconfig/impl/randactivitydingzhibaoxiang2config.hpp"
#include "config/randactivityconfig/impl/randactivitydingzhibaoxiang3config.hpp"
#include "config/randactivityconfig/impl/randactivityyuanqiyanhuoconfig.hpp"
#include "config/randactivityconfig/impl/randactivityzhuanshulibaoconfig.hpp"
#include "config/randactivityconfig/impl/randactivityjingyanbiaoshengconfig.hpp"
#include "config/randactivityconfig/impl/randactivityxianshibaoheconfig.hpp"
#include "config/randactivityconfig/impl/randactivityxianshibaohe2config.hpp"
#include "config/randactivityconfig/impl/randactivityxianshibaohe3config.hpp"
#include "config/randactivityconfig/impl/randactivityxianshibaohe4config.hpp"
#include "config/randactivityconfig/impl/randactivityxianshibaodaiconfig.hpp"
#include "config/randactivityconfig/impl/randactivityxianshibaodai2config.hpp"
#include "config/randactivityconfig/impl/randactivitygivepetdrawconfig.hpp"
#include "config/randactivityconfig/impl/randactivitycijiuyingxinconfig.hpp"
#include "config/randactivityconfig/impl/randactivityniuxingdayunconfig.hpp"
#include "config/randactivityconfig/impl/randactivityspringfestivalstoreconfig.hpp"
#include "config/randactivityconfig/impl/randactivitymonpolygiftconfig.hpp"
#include "config/randactivityconfig/impl/randactivitycowexchangeconfig.hpp"
#include "config/randactivityconfig/impl/randactivitycowstoreconfig.hpp"
#include "config/randactivityconfig/impl/randactivitycowluckyexchangeconfig.hpp"
#include "config/randactivityconfig/impl/randactivitylittleyeargiftconfig.hpp"
#include "config/randactivityconfig/impl/randactivitygivesecretkeyconfig.hpp"
#include "config/randactivityconfig/impl/randactivitycumulativerechargeconfig.hpp"
#include "config/randactivityconfig/impl/randactivitypetlootconfig.hpp"
#include "config/randactivityconfig/impl/randactivitytrademarketreturngoldconfig.hpp"
#include "config/randactivityconfig/impl/randactivityshenshoujianglinconfig.hpp"
#include "config/randactivityconfig/impl/randactivitygoodgiftturnconfig.hpp"
#include "config/randactivityconfig/impl/randactivityfortunemazeconfig.hpp"
#include "config/randactivityconfig/impl/randactivityhelpothersconfig.hpp"
#include "config/randactivityconfig/impl/randactivityhelpothersconfig2.hpp"
#include "config/randactivityconfig/impl/randactivitysecrettreasureconfig.hpp"
#include "config/randactivityconfig/impl/randactivitymijingqiwenconfig.hpp"
#include "config/randactivityconfig/impl/randactivitymijingqiwenconfig2.hpp"
#include "config/randactivityconfig/impl/randactivitychunridengfengconfig.hpp"
#include "config/randactivityconfig/impl/randactivitylaodonghaoliconfig.hpp"
#include "config/randactivityconfig/impl/randactivityzuheheliconfig.hpp"
#include "config/randactivityconfig/impl/randactivitylingpaitianciconfig.hpp"
#include "config/randactivityconfig/impl/randactivityfashionshopconfig.hpp"
#include "config/randactivityconfig/impl/randactivityoneyuangouconfig.hpp"
#include "config/randactivityconfig/impl/randactivitylingyujijinconfig.hpp"
#include "config/randactivityconfig/impl/randactivitylingpaitianci2config.hpp"
#include "config/randactivityconfig/impl/randactivitypaimaihangconfig.hpp"

#include "config/randactivityconfig/impl/randactivitysummerlilianconfig.hpp"
#include "config/randactivityconfig/impl/randactivitycolorfullanternconfig.hpp"
#include "config/randactivityconfig/impl/randactivitysummerstoreconfig.hpp"
#include "config/randactivityconfig/impl/randactivityluckysignconfig.hpp"
#include "config/randactivityconfig/impl/randactivityqionglouyuyanconfig.hpp"
#include "config/randactivityconfig/impl/randactivitysuperoffshopconfig.hpp"
#include "config/randactivityconfig/impl/randactivityyuanqiturningconfig.hpp"
#include "config/randactivityconfig/impl/randactivitymazesummerconfig.hpp"
#include "config/randactivityconfig/impl/randactivitycoolbreezeconfig.hpp"
#include "config/randactivityconfig/impl/randactivitylotusmibaoconfig.hpp"
#include "config/randactivityconfig/impl/randactivitymibaofangdianconfig.hpp"
#include "config/randactivityconfig/impl/randactivityzhenpinshangpuconfig.hpp"
#include "config/randactivityconfig/impl/randactivitylingpaitianci3config.hpp"
#include "config/randactivityconfig/impl/randactivityduihuanlingyuconfig.hpp"
#include "config/randactivityconfig/impl/randactivitybackrewardconfig.hpp"
#include "config/randactivityconfig/impl/randactivityservercompetitionconfig.hpp"
#include "config/randactivityconfig/impl/randactivityhappylookconfig.hpp"
#include "config/randactivityconfig/impl/randactivitysignhappyconfig.hpp"
#include "config/randactivityconfig/impl/randactivitywanlinggongyingconfig.hpp"
#include "config/randactivityconfig/impl/randactivityhorcruxtianchengconfig.hpp"
#include "config/randactivityconfig/impl/randactivityintegraldrawconfig.hpp"
#include "config/randactivityconfig/impl/randactivityshanhaiquestionconfig.hpp"
#include "config/randactivityconfig/impl/rashanhaijubaoconfig.hpp"
#include "config/randactivityconfig/impl/rashanhaimetconfig.hpp"
#include "config/randactivityconfig/impl/randactivityshanhaimeetconfig.hpp"
#include "config/randactivityconfig/impl/randactivityfateentanglementconfig.hpp"
#include "config/randactivityconfig/impl/randactivityzhaocaijinbaoconfig.hpp"
#include "config/randactivityconfig/impl/randactivityyunzeterritoryconfig.hpp"
#include "config/randactivityconfig/impl/randactivityyunzebloomingrecordconfig.hpp"
#include "config/randactivityconfig/impl/razaizhanyunzeconfig.hpp"
#include "config/randactivityconfig/impl/randactivityyunzeslotmachineconfig.hpp"
#include "config/randactivityconfig/impl/randactivitymozuzainaliconfig.hpp"
#include "config/randactivityconfig/impl/randactivitychaozhifenxiangconfig.hpp"
#include "config/randactivityconfig/impl/randactivityyunzeersanshiconfig.hpp"
#include "config/randactivityconfig/impl/randactivityshanhaizhengfengconfig.hpp"
#include "config/randactivityconfig/impl/racolorfuldoubleconfig.hpp"
#include "config/randactivityconfig/impl/randactivityshanhaimibaoconfig.hpp"
#include "config/randactivityconfig/impl/randactivityshanhaihuadengconfig.hpp"
#include "config/randactivityconfig/impl/randactivityshanhaibaoxiangconfig.hpp"
#include "config/randactivityconfig/impl/randactivityshanhaibaoheconfig.hpp"
#include "config/randactivityconfig/impl/randactivityshanhaibaodaiconfig.hpp"
#include "config/randactivityconfig/impl/randactivityjixinggaozhaoconfig.hpp"
#include "config/randactivityconfig/impl/randactivityyunzewishconfig.hpp"
#include "config/randactivityconfig/impl/randactivityyunzehaoliconfig.hpp"
#include "config/randactivityconfig/impl/randactivityyunzecutepetconfig.hpp"
#include "config/randactivityconfig/impl/randactivityyunzecatchelvesconfig.hpp"
#include "config/randactivityconfig/impl/randactivityxianshileichongconfig.hpp"
#include "config/randactivityconfig/impl/raweekendgiftconfig.hpp"
#include "config/randactivityconfig/impl/rachaozhigiftconfig.hpp"
#include "config/randactivityconfig/impl/randactivitytigerauspicioussnowconfig.hpp"
#include "config/randactivityconfig/impl/randactivitytigerfisheveryyearconfig.hpp"
#include "config/randactivityconfig/impl/randactivityyinhuwenshiconfig.hpp"
#include "config/randactivityconfig/impl/randactivitytigerfindspringconfig.hpp"
#include "config/randactivityconfig/impl/randactivitytigerteasenewyearconfig.hpp"
#include "config/randactivityconfig/impl/randactivityciniuyingxinhuconfig.hpp"
#include "config/randactivityconfig/impl/randactivitytigerluckymoneyconfig.hpp"
#include "config/randactivityconfig/impl/randactivityhuxingdayunconfig.hpp"
#include "config/randactivityconfig/impl/randactivityyinhubaisuiconfig.hpp"
#include "config/randactivityconfig/impl/randactivityjingjiyuekaconfig.hpp"
#include "config/randactivityconfig/impl/randactivityronglianyouliconfig.hpp"
#include "config/randactivityconfig/impl/randactivityqingyuanshizhuangconfig.hpp"
#include "config/randactivityconfig/impl/randactivityqingyuanleichongconfig.hpp"

#include "config/randactivityconfig/impl/randactivityyuanqispeedconfig.hpp"
#include "config/randactivityconfig/impl/randactivityyuanqihelpconfig.hpp"
#include "config/randactivityconfig/impl/randactivityqiyuankongmingdengconfig.hpp"
#include "config/randactivityconfig/impl/randactivityxunmenghudieconfig.hpp"
#include "config/randactivityconfig/impl/randactivitybozhongyouliconfig.hpp"
#include "config/randactivityconfig/impl/randactivityyuanqifanpaiconfig.hpp"
#include "config/randactivityconfig/impl/randactivitywanlingzhuliconfig.hpp"
#include "config/randactivityconfig/impl/randactivitybaozilaileconfig.hpp"
#include "config/randactivityconfig/impl/randactivityjinyitianxiangconfig.hpp"
#include "config/randactivityconfig/impl/randactivitymiaobishenghuaconfig.hpp"
#include "config/randactivityconfig/impl/randactivityyuanqijishibuconfig.hpp"
#include "config/randactivityconfig/impl/randactivitychengfengpolangconfig.hpp"
#include "config/randactivityconfig/impl/randactivityduanyangxiangnangconfig.hpp"
#include "config/randactivityconfig/impl/randactivityduanyangzixuanconfig.hpp"
#include "config/randactivityconfig/impl/randactivityduanyangfuliconfig.hpp"
#include "config/randactivityconfig/impl/randactivityshenshouenzeconfig.hpp"
#include "config/randactivityconfig/impl/randactivityjinlongjubaoconfig.hpp"
#include "config/randactivityconfig/impl/randactivitycanglongcifuconfig.hpp"
#include "config/randactivityconfig/impl/randactivitylantangchumuconfig.hpp"
#include "config/randactivityconfig/impl/randactivitysanbaichouconfig.hpp"
#include "config/randactivityconfig/impl/randactivitycharismatictanabataconfig.hpp"
#include "config/randactivityconfig/impl/randactivityxinfulingchongconfig.hpp"

#include "world.h"
#include "config/randactivityconfig/impl/randactivitygodbeastadventconfig.hpp"
#include "config/randactivityconfig/impl/randactivityzongxiangmanchengconfig.hpp"
#include "config/randactivityconfig/impl/randactivitysaltysweetbattleconfig.hpp"
#include "config/randactivityconfig/impl/randactivityluckyblindboxconfig.hpp"
#include "config/randactivityconfig/impl/rachongjifanliconfig.hpp"
#include "config/randactivityconfig/impl/randactivityxinfuqianghuaconfig.hpp"
#include "config/randactivityconfig/impl/randactivityqixitehuiconfig.hpp"
#include "config/randactivityconfig/impl/randactivityhunqifanliconfig.hpp"
#include "config/randactivityconfig/impl/randactivityshencihuafuconfig.hpp"
#include "config/randactivityconfig/impl/randactivityzhuiyueshangdianconfig.hpp"
#include "config/randactivityconfig/impl/randactivitytianyiyaoshiconfig.hpp"
#include "config/randactivityconfig/impl/randactivityyuexidengmiconfig.hpp"
#include "config/randactivityconfig/impl/randactivityyuexileichongconfig.hpp"
#include "config/randactivityconfig/impl/randactivitydanbifanliconfig.hpp"

#define INIT_RAND_ACTIVITY_CFG(activity_type , class_name)							\
{																					\
	if(this->CheckRandActivity(activity_type))										\
	{																				\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: "#class_name" Init failed  RAND_ACTIVITY_TYPE[%d] ERROR", configname.c_str( ) , activity_type );\
		*err = errinfo;																\
		return false;																\
	}																			\
	class_name * class_cfg = new class_name();									\
	iRet = class_cfg->Init(RootElement , err);									\
	if (!iRet)																	\
	{																			\
		SNPRINTF(errinfo, sizeof(errinfo), " %s : "#class_name" Init failed errormsg[%s]", configname.c_str(), err->c_str());\
		*err =  errinfo;														\
		delete class_cfg;														\
		class_cfg = NULL;														\
		return false;															\
	}																			\
	m_rand_activity_cfg_arr[activity_type - RAND_ACTIVITY_TYPE_BEGIN] = class_cfg;\
}\

#define PRE_LOAD_SEOARATE_CONFIG \
	int index = configname.rfind("/");\
	std::string tmp_str = configname.substr(0, index + 1);\
	tmp_str += "randactivity/";\


#define INIT_SEOARATE_RAND_ACTIVITY_CFG(activity_type , class_name , config_path)	\
{		\
	char errinfo[1024] = { 0 }; \
	TiXmlDocument document;\
	std::string tmp_config_path = tmp_str;\
	tmp_config_path += config_path;\
	if (tmp_config_path == "" || !document.LoadFile(tmp_config_path.c_str()))\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "Load config [%s] Error.%s", tmp_config_path.c_str(), document.ErrorStr());\
		*err = errinfo;\
		return false;\
	}\
	TiXmlElement *RootElement = document.RootElement();\
	if (NULL == RootElement)\
	{\
		SNPRINTF(errinfo, sizeof(errinfo), "Load config [%s] Error.cannot find root node.", tmp_config_path.c_str());\
		*err = errinfo;\
		return false;\
	}\
	int iRet = 0;\
	if(this->CheckRandActivity(activity_type))										\
	{																				\
		SNPRINTF(errinfo, sizeof(errinfo), "%s: "#class_name" Init failed  RAND_ACTIVITY_TYPE[%d] ERROR", tmp_config_path.c_str( ) , activity_type );\
		*err = errinfo;																\
		return false;																\
	}																			\
	class_name * class_cfg = new class_name();									\
	iRet = class_cfg->Init(RootElement , err);									\
	if (!iRet)																	\
	{																			\
		SNPRINTF(errinfo, sizeof(errinfo), " %s : "#class_name" Init failed  %s", tmp_config_path.c_str() , err->c_str());\
		*err =  errinfo;\
		delete class_cfg;														\
		class_cfg = NULL;														\
		return false;															\
	}																			\
	m_rand_activity_cfg_arr[activity_type - RAND_ACTIVITY_TYPE_BEGIN] = class_cfg;\
}\



RandActivityConfigManager::RandActivityConfigManager()
{
	for (int i = 0; i < ARRAY_LENGTH(m_rand_activity_cfg_arr); ++i)
	{
		m_rand_activity_cfg_arr[i] = NULL;
	}
	
}

RandActivityConfigManager::~RandActivityConfigManager()
{
	this->Release();

	std::vector<RAPatrolMiningCfg>().swap(m_patrol_mining_cfg);
	for (std::map<int, std::vector<RAFieldBossCfg> >::iterator it = m_field_boss_map.begin(); it != m_field_boss_map.end(); it++){
		std::vector<RAFieldBossCfg>().swap(it->second);
	}

	for (std::map<int, std::vector<RACrystalBossCfg> >::iterator it = m_crystal_boss_map.begin(); it != m_crystal_boss_map.end(); it++){
		std::vector<RACrystalBossCfg>().swap(it->second);
	}

	std::vector<RAPatrolMiningMoreCfg>().swap(m_patrol_mining_more_cfg);
	for (std::map<int, std::vector<RAFieldCrystalFBMoreCfg> >::iterator it = m_field_crystal_fb_map.begin(); it != m_field_crystal_fb_map.end(); it++){
		std::vector<RAFieldCrystalFBMoreCfg>().swap(it->second);
	}
	for (std::map<int, std::vector<RAMorePrizeCfg> >::iterator it = m_more_prize_map.begin(); it != m_more_prize_map.end(); it++){
		std::vector<RAMorePrizeCfg>().swap(it->second);
	}

	std::vector<ScoreReward>().swap(m_score_reward_vec);
	std::vector<MonthPurchaseRewardCfg>().swap(m_month_purchase_reward);

	for (std::map<int, std::vector<MysteriousTrialRewardCfg> >::iterator it = m_mysterious_trial_reward_cfg.begin(); it != m_mysterious_trial_reward_cfg.end(); it++){
		std::vector<MysteriousTrialRewardCfg>().swap(it->second);
	}

	std::vector<RALoginSystemRewardGroupCfg>().swap(m_login_system_vec);
}

bool RandActivityConfigManager::Init(const std::string &configname, std::string *err)
{
	PRE_LOAD_CONFIG;

	LOAD_CONFIG("other", InitRandActivityOtherCfg);
	LOAD_CONFIG("patrol_mining", InitPatrolMiningCfg);
	LOAD_CONFIG("field_boss", InitFieldBossCfg);
	LOAD_CONFIG("crystal_copy", InitCrystalBossCfg);
	LOAD_CONFIG("duozu", InitPatrolMiningMoreCfg);
	LOAD_CONFIG("yewai", InitFieldCrystalFBCfg);
	LOAD_CONFIG("jianglichi", InitMorePrizeCfg);
	LOAD_CONFIG("character_rating", InitScoreRewardCfg);
	LOAD_CONFIG("monthly_investment", InitMonthInvestment);
	LOAD_CONFIG("monthly_purchase_reward", InitMonthPurchaseReward);
	LOAD_CONFIG("mysterious_trial_reward", InitMysteriousTrialRewardfg);
	LOAD_CONFIG("mysterious_trial", InitMysteriousTrialCfg);				//神秘试炼条件配置读取需要放在奖励之后
	LOAD_CONFIG("cumulative_trial_reward", InitMysteriousTrialFinallyCfg);
	LOAD_CONFIG("trial_buy", InitMysteriousTrialBuyCfg);
	LOAD_CONFIG("login_reward", InitLoginSystemRewardCfg);
	LOAD_CONFIG("activity_id", InitInvalidTimeActivity);

	// 这里是写在统一的活动表内的配置
	INIT_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_TIME_LIMIT_STORE, RandActivityTimeLimitStoreConfig);
	INIT_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_TRUN_TABLE, RandActivityTrunTableConfig);
	INIT_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL, RandActivityConsumeCarouselConfig);
	INIT_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_DISCOUNT_STORE, RandActivityDiscountStoreConfig);
	INIT_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SKY_DROP_LUXURY_GIFT, RandActivitySkyDropLuxuryGiftConfig);
	INIT_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SPORTS, RandActivitySportsConfig);
	INIT_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_FASHION_SHOP, RandActivityFashionShopConfig);

	// 这里是有单独配表的
	iRet = this->InitSeoarateCfg(configname, err);

	return iRet > 0;
}

bool RandActivityConfigManager::InitSeoarateCfg(const std::string &configname, std::string *err)
{
	PRE_LOAD_SEOARATE_CONFIG;

	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_EXPEL_MONSTER, RandActivityExpelMonsterConfig, "repel_the_monster.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_HERITAGE_TREASURE, RandActivityHeritageTreasireConfig, "treasure_activity.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE, RandActivitySevenColorPictureConfig, "colorful_illustrations.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SALE_SHOP, RandActivitySaleShopConfig, "sale_shop_activity.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_HAPPY_UP, RandActivityHappyUpConfig, "happy_up_activity.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_ANIMAL_RACE, RandActivityAnimalRaceConfig, "animal_race.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_LUCKY_BAG, RandActivityLuckyBagConfig, "fortune_bag.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_REDUCE_TAX_RATE, RandActivityReduceTaxRateConfig, "tax_discount.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_TIAN_JIANG_HAO_LI, RandActivityTianJiangHaoLiConfig, "tianjianghaoli.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SEAL_PROMOTE, RandActivitySealPromoteConfig, "seal_promote.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_DROP_DOUBLE, RandActivityDropDoubleConfig, "drop_double.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_JOYSEEKSFAVORS, RandActivityJoySeeksFavorsConfig, "joy_seeks_favors.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_COLLECT_WORD, RandActivityCollectWordConfig, "word_gift_activity.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_DISCOUNT_CARNIVAL, RandActivityDiscountCarnivalConfig, "discount_carnival.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_PARTNER_EXCHANGE, RandActivityPartnerExchangeConfig, "partner_activity.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_LUCKY_DISCOUNT, RandActivityLuckyDiscountConfig, "lucky_discount.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_PET_EXCHANGE, RandActivityPetExchangeConfig, "pet_exchange_activity.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN, RandActivityJianDanQinXinConfig, "jiandanqinxin.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_ADVENTURE_GIFT, RandActivityAdventureGiftConfig, "adventure_gift_activity.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_LINKAGE_GIFT, RandActivityLinkageGiftConfig, "linkage_gift.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE, RandActivityLuckyTurnTableConfig, "lucky_turntable.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION, RandActivityTianMingDivinationConfig, "divination_activity.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT, RandActivityQuanMinDiscountConfig, "qm_discount.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE, RandActivityTuJianTreasureConfig, "tujian_treasure.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_GOD_TOKEN, RandActivityGodTokenConfig, "fengshenzhanling.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_ELEMENT_CARDS, RandActivityElementCardsConfig, "element_cards.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SIGN_IN_CONTINUITY, RandActivitySignInContinuityConfig, "sign_in_continuity.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_CAI_SHEN_JU_BAO, RandActivityCaiShenJuBaoConfig, "wealth_cornucopia.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG, RandActivityDingZhiBaoXiangConfig, "ustomized_box.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_ZHUAN_SHU_LI_BAO, RandActivityZhuanShuLiBaoConfig, "exclusive_gift.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_JING_YAN_BIAO_SHENG, RandActivityJingYanBiaoShengConfig, "exp_purchase.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE, RandActivityXianShiBaoHeConfig, "gift_box.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI, RandActivityXianShiBaoDaiConfig, "chain_box.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_GIVE_PET_DRAW, RandActivityGivePetDrawConfig, "pet_search.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_CI_JIU_YING_XIN, RandActivityCiJiuYingXinConfig, "looks_new.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_NIU_XING_DA_YUN, RandActivityNiuXingDaYunConfig, "customs_gift.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE, RandActivitySpringFestivalStoreConfig, "spring_festival_store.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_MONPOLY_GIFT, RandActivityMonpolyGiftConfig, "monopoly_gift.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_COW_EXCHANGE, RandActivityCowExchangeConfig, "cow_exchange.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_COW_STORE, RandActivityCowStoreConfig, "cow_store.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE, RandActivityCowLuckyExchangeConfig, "cow_lucky_exchange.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_LITTLE_YEAR_GIFT, RandActivityLittleYearGiftConfig, "little_year_gift.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_GIVE_SECRET_KEY, RandActivityGiveSecretKeyConfig, "give_secret_key.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE, RandActivityCumulativeRechargeConfig, "acc_recharge.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_PET_LOOT, RandActivityPetLootConfig, "pet_loot.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_TRADEMARKET_GOLD_RETURN, RandActivityTradeMarketReturnGoldConfig, "trade_discount.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN, RandActivityShenShouJiangLinConfig, "pet_befall.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN, RandActivityGoodGiftTurnConfig, "gift_rotation.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_FORTUNE_MAZE, RandActivityFortuneMazeConfig, "lucky_maze.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE, RandActivitySecretTreasureConfig, "may_day_activity.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_HELP_OTHERS, RandActivityHelpOthersConfig, "may_day_activity.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_HELP_OTHERS_2, RandActivityHelpOthersConfig2, "taoyuanyisanshi.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_CHUN_RI_DENG_FENG, RandActivityChunRiDengFengConfig, "may_day_activity.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_ANECDOTE, RandActivityMiJingQiWenConfig, "may_day_activity.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI, RandActivityLaoDongHaoLiConfig, "may_day_activity.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_ZU_HE_HE_LI, RandActivityZuHeHeLiConfig, "may_day_activity.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI, RandActivityLingPaiTianCiConfig, "lingpai_divine.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_ONE_YUAN_GOU, RandActivityOneYuanGouConfig, "buy_one_gold.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_LING_YU_JI_JIN, RandActivityLingYuJiJinConfig, "immortal_coin_fund.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI_2, RandActivityLingPaiTianCi2Config, "xinfawuliang.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_PAI_MAI_HANG, RandActivityPaiMaiHangConfig, "pai_mai_hang.xml");

	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SUMMER_LI_LIAN, RandActivitySummerLiLianConfig, "burning_summer.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_COLORFUL_LANTERN, RandActivityColorfulLanternConfig, "burning_summer.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SUMMER_STORE, RandActivitySummerStoreConfig, "burning_summer.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_LUCKY_SIGN, RandActivityLuckySignConfig, "burning_summer.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN, RandActivityQiongLouYuYanConfig, "burning_summer.xml");

	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP, RandActivitySuperOffShopConfig, "cool_summer.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_YUAN_QI_TURNING, RandActivityYuanQiTurningConfig, "cool_summer.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_COOL_BREEZE, RandActivityCoolBreezeConfig, "cool_summer.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_LOTUS_MI_BAO, RandActivityLotusMiBaoConfig, "cool_summer.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_MAZE_SUMMER, RandActivityMazeSummerConfig, "cool_summer.xml");

	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN, RandActivityMiBaoFangDianConfig, "national_day.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_ZHEN_PIN_SHANG_PU, RandActivityZhenPinShangPuConfig, "national_day.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI_3, RandActivityLingPaiTianCi3Config, "assign_horcrux.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_DUI_HUAN_LING_YU, RandActivityDuiHuanLingYuConfig, "exchange_jade.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_BACK_REWARD, RandActivityBackRewardConfig, "back_reward.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SERVER_COMPETITION, RandActivityServerCompetitionConfig, "server_competition.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_HAPPY_LOOK, RandActivityHappyLookConfig, "happy_look.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SIGN_HAPPY, RandActivitySignHappyConfig, "sign_happy.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING, RandActivityWanLingGongYingConfig, "wanlinggongying.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_ANECDOTE_2, RandActivityMiJingQiWenConfig2, "secret_area.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG, RandActivityHorcruxTianChengConfig, "horcrux_tiancheng.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_INTEGRAL_DRAW, RandActivityIntegralDrawConfig, "integral_point_choujiang.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SHANHAI_QUESTION, RandActivityShanHaiQuestionConfig,"time_limit_answer.xml")
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SHAN_HAI_JU_BAO, RAShanHaiJuBaoConfig, "shanhaijubao.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SHAN_HAI_MET, RAShanHaiMetConfig, "shanhaichuyu.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SHAN_HAI_MEET, RandActivityShanHaiMeetConfig, "shanhaixiangfeng.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_FATE_ENTANGLEMENT, RandActivityFateEntanglementConfig, "shanhai_origin.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO, RandActivityZhaoCaiJinBaoConfig, "zhaocaijinbao.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_YUN_ZE_TERRITORY, RandActivityYunZeTerritoryConfig, "yunzehuajing.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD, RandActivityYunZeBloomingRecordConfig,"yunzefanhualu.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE, RandActivityYunZeSlotMachineConfig,"fenxiangtonghuali.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_ZAI_ZHAN_YUN_ZE, RAZaiZhanYunZeConfig, "zaizhanyunze.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_MO_ZU_ZAI_NA_LI, RandActivityMoZuZaiNaLiConfig, "mozuzainali.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG, RandActivityChaoZhiFenXiangConfig, "chaozhifenxiang.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_YUN_ZE_ER_SAN_SHI, RandActivityYunZeErSanShiConfig, "yunzeersanshi.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG, RandActivityShanHaiZhengFengConfig, "shanhaizhengfeng.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_COLORFUL_DOUBLE, RAColorfulDoubleConfig, "fenxiangjiabei.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO, RandActivityShanHaiMiBaoConfig, "shanhaimibao.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SHANHAI_HUADENG, RandActivityShanHaiHuaDengConfig, "shanhaihuadeng.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_XIANG, RandActivityShanHaiBaoXiangConfig, "shanhai_ustomized_box.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_HE, RandActivityShanHaiBaoHeConfig, "shanhai_box.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_DAI, RandActivityShanHaiBaoDaiConfig, "shanhai_chain_box.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_2, RandActivityDingZhiBaoXiang2Config, "yunzebaoxiang.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_2, RandActivityXianShiBaoHe2Config, "yunzebaohe.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI_2, RandActivityXianShiBaoDai2Config, "yunzebaodai.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_JIXING_GAOZHAO, RandActivityJiXingGaoZhaoConfig, "jixinggaozhao.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_YUN_ZE_WISH, RandActivityYunZeWishConfig, "yunzeqiyuan.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI, RandActivityYunZeHaoLiConfig, "yunzehaoli.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET, RandActivityYunZeCutePetConfig, "mengchongjiadao.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_YUN_ZE_CATCH_ELVES, RandActivityYunZeCatchElvesConfig, "jinglingdazhuabu.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG, RandActivityXianShiLeiChongConfig, "xianshileichong.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_WEEKEND_GIFT, RAWeekendGiftConfig, "exclusive_gift.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_CHAO_ZHI_GIFT, RAChaoZhiGiftConfig, "chaozhixianli.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW, RandActivityTigerAuspiciousSnowConfig, "ruixuefengnian.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR, RandActivityTigerFishEveryYearConfig, "niannianyouyu.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING, RandActivityTigerFindSpringConfig, "yinhutanchun.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_TIGER_TEASE_NEWYEAR, RandActivityTigerTeaseNewYearConfig, "yinhudouxinchun.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY, RandActivityTigerLuckyMoneyConfig, "yinhuyasui.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_YINHU_WENSHI, RandActivityYinHuWenShiConfig, "yinhuwenshi.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_CINIU_YINGXINHU, RandActivityCiNiuYingXinHuConfig, "ciniuyingxinhu.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_HU_XING_DA_YUN, RandActivityHuXingDaYunConfig, "huxingdayun.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_YIN_HU_BAI_SUI, RandActivityYinHuBaiSuiConfig, "yinhubaisui.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA, RandActivityJingJiYueKaConfig, "jingjiyueka.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_RONG_LIAN_YOU_LI, RandActivityRongLianYouLiConfig, "ronglianyouli.xml");  
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_QING_YUAN_SHI_ZHUANG, RandActivityQingYuanShiZhuangConfig, "qingyuanshizhuang.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_QING_YUAN_LEI_CHONG, RandActivityQingYuanLeiChongConfig, "qingyuanleichong.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT, RandActivityYiShouLinShiConfig, "yishoulinshi.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_YUAN_QI_SPEED, RandActivityYuanQiSpeedConfig, "yuanqijingsu.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_YUAN_QI_HELP, RandActivityYuanQiHelpConfig, "yuanqizhuren.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_QIYUAN_KONGMINGDENG, RandActivityQiYuanKongMingDengConfig, "qiyuankongmingdeng.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_XUNMENG_HUDIE, RandActivityXunMengHuDieConfig, "xunmenghudie.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_BO_ZHONG_YOU_LI, RandActivityBoZhongYouLiConfig, "bozhongyouli.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_YUNA_QI_FAN_PAI, RandActivityYuanQiFanPaiConfig, "yuanqifanpai.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_WANLING_ZHULI, RandActivityWanLingZhuLiConfig, "wanlingzhuli.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_BAO_ZI_LAI_LE, RandActivityBaoZiLaiLeConfig, "baozilaile.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_JIN_YI_TIAN_XIANG, RandActivityJinYiTianXiangConfig, "jinyitianxiang.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_MIAO_BI_SHENG_HUA, RandActivityMiaoBiShengHuaConfig, "miaobishenghua.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_YUAN_QI_YAN_HUO, RandActivityYuanQiYanHuoConfig, "yuanqiyanhuoxiu.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU, RandActivityYuanQiJiShiBuConfig, "yuanqijishibu.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_CHENG_FENG_PO_LANG, RandActivityChengFengPoLangConfig, "chengfengpolang.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_ZONG_XIANG_MAN_CHENG, RandActivityZongXiangManChengConfig, "zongxiangmancheng.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE, RandActivitySaltySweetBattleConfig, "tianxianzhizheng.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG, RandActivityDuanYangXiangNangConfig, "duanyangxiangnang.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_DUAN_YANG_ZI_XUAN, RandActivityDuanYangZiXuanConfig, "duanyangzixuanli.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_CANG_LONG_CI_FU, RandActivityCangLongCiFuConfig, "canglongcifu.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_LUCKY_BLIND_BOX, RandActivityLuckyBlindBoxConfig, "xingyunmanghe.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU, RandActivityLanTangChuMuConfig, "lantangchumu.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_300_CHOU, RandActivitySanBaiChouConfig, "sanbaichoupifu.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_DUAN_YANG_FU_LI, RandActivityDuanYangFuLiConfig, "duanyangfuli.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_JIN_LONG_JU_BAO, RandActivityJinLongJuBaoConfig, "jinlongjubao.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_CHONG_JI_FAN_LI, RandActivityChongJiFanLiConfig, "chongjifanli.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SHEN_SHOU_EN_ZE, RandActivityShenShouEnZeConfig, "shenshouenze.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_XIN_FU_QIANG_HUA, RandActivityXinFuQiangHuaConfig, "xinfuqianghua.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_QI_XI_TE_HUI, RandActivityQiXiTeHuiConfig, "qiqiaotehui.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_CHARISMATIC_TANABATA, RandActivityCharismaticTanabataConfig, "meiliqixi.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_3, RandActivityXianShiBaoHe3Config, "sijihuapai.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_4, RandActivityXianShiBaoHe4Config, "sijibaohe.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_3, RandActivityDingZhiBaoXiang3Config, "sijibaoxiang.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_XIN_FU_LING_CHONG, RandActivityXinFuLingChongConfig, "xinfulingchong.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_HUN_QI_FAN_LI, RandActivityHunQiFanLiConfig, "hunqifanli.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_SHEN_CI_HUA_FU, RandActivityShenCiHuaFuConfig, "shencihuafu.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_TIAN_YI_YAO_SHI, RandActivityTianYiYaoShiConfig, "tianyiyaoshi.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_YUE_XI_DENG_MI, RandActivityYueXiDengMiConfig, "yuexidengmi.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_YUE_XI_LEI_CHONG, RandActivityYueXiLeiChongConfig, "yuexileichong.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_DAN_BI_FAN_LI, RandActivityDanBiFanLiConfig, "danbifanli.xml");
	INIT_SEOARATE_RAND_ACTIVITY_CFG(RAND_ACTIVITY_TYPE_ZHUI_YUE_SHANG_DIAN, RandActivityZhuiYueShangDianConfig, "zhuiyueshangcheng.xml");

	UNSTD_STATIC_CHECK(178 == RAND_ACTIVITY_TYPE_END - RAND_ACTIVITY_TYPE_BEGIN);
	
	return true;
}

const RandActivityConfig * RandActivityConfigManager::GetRandActivityConfig(int rand_activity_type)
{
	if (this->CheckRandActivity(rand_activity_type))
	{
		return NULL;
	}

	RandActivityConfig * rand_activity_cfg = m_rand_activity_cfg_arr[rand_activity_type - RAND_ACTIVITY_TYPE_BEGIN];
	if (NULL != rand_activity_cfg)
	{
		return rand_activity_cfg;
	}

	return NULL;
}

int RandActivityConfigManager::GetActOpenDay(RandActivityManager* ramgr, int activity_type) const
{
	time_t activity_begin_time = ramgr->GetRandActivityBeginTime(activity_type);
	time_t cur_time = EngineAdapter::Instance().Time(); //取当前时间做计算
	int act_real_open_day = GetDayIndex(activity_begin_time, cur_time) + 1; // 活动开启天数，1~N

	if (act_real_open_day <= 0)
	{
		act_real_open_day = 1;
	}

	return act_real_open_day;
}

int RandActivityConfigManager::GetActOpenDayByTime(RandActivityManager * ramgr, int activity_type, time_t time) const
{
	time_t activity_begin_time = ramgr->GetRandActivityBeginTime(activity_type);
	int act_real_open_day = GetDayIndex(activity_begin_time, time) + 1; // 活动开启天数 1~N

	if (act_real_open_day <= 0)
	{
		act_real_open_day = 1;
	}

	return act_real_open_day;
}

int RandActivityConfigManager::GetActRealOpenDay(RandActivityManager* ramgr, int activity_type) const
{
	time_t activity_begin_time = ramgr->GetRandActivityBeginTime(activity_type);
	time_t open_server_timestamp = World::GetInstWorld()->GetServerOpenZeroTime(ramgr->GetUniqueServerID());
	int act_real_open_day = GetDayIndex(open_server_timestamp, activity_begin_time) + 1; // 活动开启时的开服天数，1~N

	if (act_real_open_day <= 0)
	{
		act_real_open_day = 1;
	}

	return act_real_open_day;
}

bool RandActivityConfigManager::GetPatrolMiningCfg(RandActivityManager* ramgr, std::vector<ItemConfigData>* list)
{
	if (NULL == ramgr) return false;

	int rand_num = 0;
	for (int i = 0; i < (int)m_patrol_mining_cfg.size(); i++)
	{
		if (ramgr->IsRandActivityOpen(m_patrol_mining_cfg[i].activity_type))
		{
			rand_num = rand() % MAX_RATE_COUNT_NUM;
			if (rand_num < m_patrol_mining_cfg[i].rate)
			{
				(*list).push_back(m_patrol_mining_cfg[i].item);
			}
		}
	}
	if ((int)(*list).size() >= 1) return true;

	return false;
}

bool RandActivityConfigManager::GetFieldBossCfgById(RandActivityManager* ramgr, int monster_id, std::vector<ItemConfigData>* list)
{
	if (NULL == ramgr) return false;

	std::map<int, std::vector<RAFieldBossCfg> >::iterator it = m_field_boss_map.find(monster_id);
	if (it == m_field_boss_map.end()) return false;

	int rand_num = 0;
	for (int i = 0; i < (int)it->second.size(); i++)
	{
		if (ramgr->IsRandActivityOpen(it->second[i].activity_type))
		{
			rand_num = rand() % MAX_RATE_COUNT_NUM;
			if (rand_num < it->second[i].rate)
			{
				(*list).push_back(it->second[i].item);
			}
		}
	}
	if ((int)(*list).size() >= 1) return true;

	return false;
}

bool RandActivityConfigManager::GetCrystalBossCfgById(RandActivityManager* ramgr, int monster_id, std::vector<ItemConfigData>* list)
{
	if (NULL == ramgr) return false;

	std::map<int, std::vector<RACrystalBossCfg> >::iterator it = m_crystal_boss_map.find(monster_id);
	if (it == m_crystal_boss_map.end()) return false;

	int rand_num = 0;
	for (int i = 0; i < (int)it->second.size(); i++)
	{
		if (ramgr->IsRandActivityOpen(it->second[i].activity_type))
		{
			rand_num = rand() % MAX_RATE_COUNT_NUM;
			if (rand_num < it->second[i].rate)
			{
				(*list).push_back(it->second[i].item);
			}
		}
	}
	if ((int)(*list).size() >= 1) return true;

	return false;
}

bool RandActivityConfigManager::GetPatrolMiningMoreCfg(RandActivityManager* ramgr, std::vector<ItemConfigData>* list)
{
	if (NULL == ramgr) return false;

	int rand_num = 0;
	for (std::vector<RAPatrolMiningMoreCfg>::const_iterator it = m_patrol_mining_more_cfg.begin(); it != m_patrol_mining_more_cfg.end(); ++it)
	{
		if (ramgr->IsRandActivityOpen(it->activity_type))
		{
			rand_num = rand() % MAX_RATE_COUNT_NUM;
			if (rand_num < it->weight)
			{
				const RAMorePrizeCfg* cfg = this->GetMorePrizeCfg(it->prize_id);
				if (NULL == cfg)continue;

				(*list).push_back(cfg->item);
			}
		}
	}
	if ((int)(*list).size() >= 1) return true;

	return false;
}

bool RandActivityConfigManager::GetFieldCrystalFBMoreCfgById(RandActivityManager* ramgr, int monster_id, std::vector<ItemConfigData>* list)
{
	if (NULL == ramgr) return false;

	std::map<int, std::vector<RAFieldCrystalFBMoreCfg> >::iterator it = m_field_crystal_fb_map.find(monster_id);
	if (it == m_field_crystal_fb_map.end()) return false;

	int rand_num = 0;
	for (std::vector<RAFieldCrystalFBMoreCfg>::const_iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
	{
		if (ramgr->IsRandActivityOpen(vit->activity_type))
		{
			rand_num = rand() % MAX_RATE_COUNT_NUM;

			if (rand_num < vit->weight)
			{
				const RAMorePrizeCfg* cfg = this->GetMorePrizeCfg(vit->prize_id);
				if (NULL == cfg)continue;

				(*list).push_back(cfg->item);
			}
		}
	}
	if ((int)(*list).size() >= 1) return true;

	return false;
}

const RAMorePrizeCfg* RandActivityConfigManager::GetMorePrizeCfg(int prize_id)
{
	std::map<int, std::vector<RAMorePrizeCfg> >::const_iterator it = m_more_prize_map.find(prize_id);
	if (it == m_more_prize_map.end())return NULL;

	int all_weight = 0;
	for (std::vector<RAMorePrizeCfg>::const_iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
	{
		all_weight += vit->weight;
	}
	if (0 == all_weight)return NULL;

	int rand_weight = rand() % all_weight;

	for (std::vector<RAMorePrizeCfg>::const_iterator vit = it->second.begin(); vit != it->second.end(); ++vit)
	{
		if (rand_weight < vit->weight)
		{
			return &(*vit);
		}
		rand_weight -= vit->weight;
	}
	return NULL;
}

const MonthInvestmentCfg * RandActivityConfigManager::GetMonthInvestment(int seq) const
{
	if (seq < 0 || seq >= RA_MONTH_INVESTMENT_MAX_NUM) return NULL;

	return &m_month_investment_config[seq];
}

const ScoreReward * RandActivityConfigManager::GetScoreReward(int seq)
{
	if (seq < 0 || seq >= (int)m_score_reward_vec.size()) return NULL;

	return &m_score_reward_vec[seq];
}

const MysteriousTrialCfg* RandActivityConfigManager::GetMysteriousTrialCfg(int type, int day, int seq) const
{
	if (seq < 0 || seq >= RA_NEW_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM) return NULL;

	std::map<int, std::map<int, MysteriousTrialConfig> >::const_iterator it = m_mysterious_cfg.find(type);
	if(it == m_mysterious_cfg.end()) return NULL;

	std::map<int, MysteriousTrialConfig>::const_iterator day_it = it->second.find(day);
	if(day_it == it->second.end()) return NULL;

	return &day_it->second.node_cfg[seq];
}

const std::map<int, MysteriousTrialConfig>* RandActivityConfigManager::GetMysteriousTrialCfg(int type) const
{
	std::map<int, std::map<int, MysteriousTrialConfig> >::const_iterator it = m_mysterious_cfg.find(type);
	if (it == m_mysterious_cfg.end()) return NULL;

	return &it->second;
}

const MysteriousTrialRewardCfg * RandActivityConfigManager::GetMysteriousTrialRewardCfg(int reward_id, unsigned int create_timestamp)const
{
	std::map<int, std::vector<MysteriousTrialRewardCfg> >::const_iterator it = m_mysterious_trial_reward_cfg.find(reward_id);
	if(it == m_mysterious_trial_reward_cfg.end() || it->second.empty()) return NULL;

	//配置读取时已经有创角时间排序了
	for (int i = (int)it->second.size() - 1; i >= 0; i--)
	{
		if (create_timestamp >= it->second[i].create_timestamp)
		{
			return &it->second[i];
		}
	}

	return NULL;
}

const MysteriousTrialFinallyCfg * RandActivityConfigManager::GetMysteriousTrialFinallyReward(int type, int seq) const
{
	if (seq < 0 || seq >= RA_MYSTERIOUS_TRIAL_FINALLY_TASK_MAX_NUM) return NULL;

	std::map<int, std::map<int, MysteriousTrialFinallyCfg> >::const_iterator it = m_mysterious_trial_finally_cfg.find(type);
	if(it == m_mysterious_trial_finally_cfg.end()) return NULL;

	std::map<int, MysteriousTrialFinallyCfg>::const_iterator finally_it = it->second.find(seq);
	if (finally_it == it->second.end()) return NULL;

	return &finally_it->second;
}

const MysteriousTraialBuyCfg * RandActivityConfigManager::GetMysteriousTrialBuyCfg(int type, int day) const
{
	std::map<int, std::map<int, MysteriousTraialBuyCfg> >::const_iterator type_it = m_mysterious_trial_buy_cfg.find(type);
	if(type_it == m_mysterious_trial_buy_cfg.end()) return NULL;

	std::map<int, MysteriousTraialBuyCfg>::const_iterator day_it = type_it->second.find(day);
	if(day_it == type_it->second.end()) return NULL;
 
	return &day_it->second;
}

const RALoginSystemRewardCfg* RandActivityConfigManager::GetLoginSystemRewardBySeq(int login_day, const UniqueServerID & usid) const
{
	if(login_day <= 0 ||login_day > MAX_LOGIN_SYSTEM_NUM) return NULL;
	
	unsigned int open_server_timestamp = World::GetInstWorld()->GetServerOpenZeroTime(usid);
	for (int i = (int)m_login_system_vec.size() - 1; i >= 0; i--)
	{
		if (open_server_timestamp >= m_login_system_vec[i].open_server_timestamp)
		{
			std::map<int, RALoginSystemRewardCfg> ::const_iterator it = m_login_system_vec[i].login_reward.find(login_day);
			if (it != m_login_system_vec[i].login_reward.end())
			{
				return &it->second;
			}
		}
	}

	return NULL;
}

const InvalidTimeActivityCfg* RandActivityConfigManager::GetInvalidTimeActivity(int _seq)
{
	std::map<int, InvalidTimeActivityCfg>::iterator it = m_seq_to_id_info.find(_seq);
	if (it != m_seq_to_id_info.end()) return &it->second;

	return NULL;
}

bool RandActivityConfigManager::IsItemInvalid(ItemID _item_id)
{
	std::map<ItemID, bool>::iterator it = m_id_to_exsit.find(_item_id);
	if (it != m_id_to_exsit.end()) return it->second;

	return false;
}

int RandActivityConfigManager::GetActivityId(ItemID _item_id)
{
	std::map<ItemID, int>::iterator it = m_itemid_to_actiid.find(_item_id);
	if (it != m_itemid_to_actiid.end()) return it->second;

	return 0;
}

const InvalidTimeItemCfg * RandActivityConfigManager::GetGroupListActivityId(ItemID _item_id)
{
	if (_item_id < 0 || _item_id >= ARRAY_ITEM_COUNT(item_activity)) return 0;

	return &item_activity[_item_id];
}

int RandActivityConfigManager::InitRandActivityOtherCfg(TiXmlElement *RootElement)
{
	if (NULL == RootElement)
	{
		return -10000;
	}

	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -20000;
	}
	
		if (!GetSubNodeValue(data_element, "extra_turntable_time", m_rand_activity_other_cfg.extra_turntable_time) || m_rand_activity_other_cfg.extra_turntable_time <= 0)
		{
			return -1;
		}
		if (!GetSubNodeValue(data_element, "kill_boss_num", m_rand_activity_other_cfg.kill_boss_num) || m_rand_activity_other_cfg.kill_boss_num <= 0)
		{
			return -2;
		}
		if (!GetSubNodeValue(data_element, "monthly_top_up_ingot", m_rand_activity_other_cfg.monthly_top_up_ingot) || m_rand_activity_other_cfg.monthly_top_up_ingot <= 0)
		{
			return -3;
		}
		if (!GetSubNodeValue(data_element, "spend_ingots_once", m_rand_activity_other_cfg.spend_ingots_once) || m_rand_activity_other_cfg.spend_ingots_once <= 0)
		{
			return -4;
		}
		if (!GetSubNodeValue(data_element, "mysterious_cfg_ver", m_rand_activity_other_cfg.mysterious_cfg_ver) || m_rand_activity_other_cfg.mysterious_cfg_ver < 0)
		{
			return -5;
		}
		if (!GetSubNodeValue(data_element, "monthly_cfg_ver", m_rand_activity_other_cfg.month_investment_cfg_ver) || m_rand_activity_other_cfg.month_investment_cfg_ver < 0)
		{
			return -6;
		}
	if (!GetSubNodeValue(data_element, "universal_card", m_rand_activity_other_cfg.universal_card_item_id) || NULL == ITEMPOOL->GetItem(m_rand_activity_other_cfg.universal_card_item_id))
	{
		return -7;
	}
	
	return 0;
}

int RandActivityConfigManager::InitPatrolMiningCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	while (NULL != data_element)
	{
		RAPatrolMiningCfg node_cfg;
		if (!GetSubNodeValue(data_element, "activity_type", node_cfg.activity_type) || node_cfg.activity_type <= RAND_ACTIVITY_TYPE_BEGIN ||
			node_cfg.activity_type >= RAND_ACTIVITY_TYPE_END)
		{
			return -1;
		}

		if (!GetSubNodeValue(data_element, "rate", node_cfg.rate) || node_cfg.rate < 0)return -2;

		TiXmlElement * item_node = data_element->FirstChildElement("reward_item");
		if (NULL != item_node)
		{
			if (!node_cfg.item.ReadConfig(item_node)) return -3;
		}

		m_patrol_mining_cfg.push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityConfigManager::InitFieldBossCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	while (NULL != data_element)
	{
		int monster_id = 0;
		if (!GetSubNodeValue(data_element, "monster_id", monster_id) || monster_id <= 0)
		{
			return -1;
		}

		RAFieldBossCfg node_cfg;
		if (!GetSubNodeValue(data_element, "activity_type", node_cfg.activity_type) || node_cfg.activity_type <= RAND_ACTIVITY_TYPE_BEGIN ||
			node_cfg.activity_type >= RAND_ACTIVITY_TYPE_END)
		{
			return -2;
		}

		if (!GetSubNodeValue(data_element, "rate", node_cfg.rate) || node_cfg.rate < 0)return -3;

		TiXmlElement * item_node = data_element->FirstChildElement("reward_item");
		if (NULL != item_node)
		{
			if (!node_cfg.item.ReadConfig(item_node)) return -4;
		}

		m_field_boss_map[monster_id].push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityConfigManager::InitCrystalBossCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	while (NULL != data_element)
	{
		int monster_id = 0;
		if (!GetSubNodeValue(data_element, "monster_id", monster_id) || monster_id <= 0)
		{
			return -1;
		}

		RACrystalBossCfg node_cfg;
		if (!GetSubNodeValue(data_element, "activity_type", node_cfg.activity_type) || node_cfg.activity_type <= RAND_ACTIVITY_TYPE_BEGIN ||
			node_cfg.activity_type >= RAND_ACTIVITY_TYPE_END)
		{
			return -2;
		}

		if (!GetSubNodeValue(data_element, "rate", node_cfg.rate) || node_cfg.rate < 0)return -3;

		TiXmlElement * item_node = data_element->FirstChildElement("reward_item");
		if (NULL != item_node)
		{
			if (!node_cfg.item.ReadConfig(item_node)) return -4;
		}

		m_crystal_boss_map[monster_id].push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityConfigManager::InitPatrolMiningMoreCfg(TiXmlElement *RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	while (NULL != data_element)
	{
		RAPatrolMiningMoreCfg node_cfg;
		if (!GetSubNodeValue(data_element, "activity_type", node_cfg.activity_type) || node_cfg.activity_type <= RAND_ACTIVITY_TYPE_BEGIN || node_cfg.activity_type >= RAND_ACTIVITY_TYPE_END)
		{
			return -1;
		}
		if (!GetSubNodeValue(data_element, "prize_id", node_cfg.prize_id) || node_cfg.prize_id < 0)
		{
			return -2;
		}
		if (!GetSubNodeValue(data_element, "rate", node_cfg.weight) || node_cfg.weight <= 0)
		{
			return -3;
		}

		m_patrol_mining_more_cfg.push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityConfigManager::InitFieldCrystalFBCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	while (NULL != data_element)
	{
		int monster_id = 0;
		if (!GetSubNodeValue(data_element, "monster_id", monster_id) || monster_id <= 0)
		{
			return -1;
		}

		RAFieldCrystalFBMoreCfg node_cfg;

		if (!GetSubNodeValue(data_element, "activity_type", node_cfg.activity_type) || node_cfg.activity_type <= RAND_ACTIVITY_TYPE_BEGIN || node_cfg.activity_type >= RAND_ACTIVITY_TYPE_END)
		{
			return -2;
		}
		if (!GetSubNodeValue(data_element, "prize_id", node_cfg.prize_id) || node_cfg.prize_id < 0)
		{
			return -3;
		}
		if (!GetSubNodeValue(data_element, "rate", node_cfg.weight) || node_cfg.weight <= 0)
		{
			return -4;
		}

		m_field_crystal_fb_map[monster_id].push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityConfigManager::InitMorePrizeCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	while (NULL != data_element)
	{
		int prize_id = 0;
		if (!GetSubNodeValue(data_element, "prize_id", prize_id) || prize_id < 0)
		{
			return -1;
		}

		RAMorePrizeCfg node_cfg;
		if (!GetSubNodeValue(data_element, "weight", node_cfg.weight) || node_cfg.weight <= 0)
		{
			return -2;
		}

		TiXmlElement * item_node = data_element->FirstChildElement("reward_item");
		if (NULL != item_node)
		{
			if (!node_cfg.item.ReadConfig(item_node)) return -3;
		}

		m_more_prize_map[prize_id].push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityConfigManager::InitScoreRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element)
	{
		return -999;
	}

	while (NULL != root_element)
	{
		ScoreReward node_cfg;
		if (!GetSubNodeValue(root_element, "seq", node_cfg.seq) || node_cfg.seq < 0 || node_cfg.seq >= SCORE_REWARD_MAX_NUM)return -1;
		if (!GetSubNodeValue(root_element, "score", node_cfg.score) || node_cfg.score < 0)return -2;

		TiXmlElement *reward_list_element = root_element->FirstChildElement("reward_item_list");
		if (NULL == reward_list_element) return -999;

		TiXmlElement *reward_element = reward_list_element->FirstChildElement("reward_item");
		node_cfg.reward_count = 0;
		while (NULL != reward_element)
		{
			if (node_cfg.reward_count >= 10) return -3;
			if (!node_cfg.reward[node_cfg.reward_count].ReadConfig(reward_element)) return -4;

			node_cfg.reward_count++;
			reward_element = reward_element->NextSiblingElement();
		}

		m_score_reward_vec.push_back(node_cfg);
		root_element = root_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityConfigManager::InitMonthInvestment(TiXmlElement * RootElement)
{
	if (NULL == RootElement)
	{
		return -10000;
	}

	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -20000;
	}
	int last_day = 0;
	while (NULL != data_element)
	{
		MonthInvestmentCfg node_cfg;
		if (!GetSubNodeValue(data_element, "days", node_cfg.days) || node_cfg.days <= 0 || node_cfg.seq > RA_MONTH_INVESTMENT_MAX_NUM) return -1;
		if (last_day + 1 != node_cfg.days)
		{
			return -111;
		}
		if (!GetSubNodeValue(data_element, "seq", node_cfg.seq) || node_cfg.seq < 0 || node_cfg.seq >= RA_MONTH_INVESTMENT_MAX_NUM) return -2;

		TiXmlElement * item_node = data_element->FirstChildElement("reward_item");
		if (NULL != item_node)
		{
			if (!node_cfg.item.ReadConfig(item_node)) return -100;
		}

		last_day = node_cfg.days;
		m_month_investment_config[node_cfg.seq] = node_cfg;
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityConfigManager::InitMonthPurchaseReward(TiXmlElement * RootElement)
{
	if (NULL == RootElement)
	{
		return -10000;
	}

	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -20000;
	}
	while (NULL != data_element)
	{
		MonthPurchaseRewardCfg node_cfg;
		if (!GetSubNodeValue(data_element, "seq", node_cfg.seq) || node_cfg.seq < 0 || node_cfg.seq >= RA_MONTH_PURCHASE_REWARD) return -1;

		TiXmlElement * list_node = data_element->FirstChildElement("reward_item_list");
		if (NULL != list_node)
		{
			TiXmlElement * item_node = list_node->FirstChildElement("reward_item");
			if (NULL != item_node)
			{
				if (!node_cfg.item.ReadConfig(item_node)) return -100;

				item_node = item_node->NextSiblingElement();
			}
		}
		m_month_purchase_reward.push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}

	if (m_month_purchase_reward.empty())
	{
		return -88;
	}

	return 0;
}

int RandActivityConfigManager::InitMysteriousTrialRewardfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -20000;
	}
	while (NULL != data_element)
	{
		int reward_id = 0;
		if (!GetSubNodeValue(data_element, "reward_id", reward_id) || reward_id <= 0) return -1;

		MysteriousTrialRewardCfg node_cfg;
		long long create_timestamp = 0;
		if (!GetSubNodeValue(data_element, "time", create_timestamp) || create_timestamp <= 0 || create_timestamp >= MAX_UINT32) return -2;
		node_cfg.create_timestamp = (unsigned int)create_timestamp;

		TiXmlElement * list_node = data_element->FirstChildElement("reward_item_list");
		if (NULL == list_node)
		{
			return -100;
		}
		TiXmlElement * item_node = list_node->FirstChildElement("reward_item");
		while (NULL != item_node)
		{
			ItemConfigData item;
			if (!item.ReadConfig(item_node)) return -101;

			node_cfg.reward_list.push_back(item);
			item_node = item_node->NextSiblingElement();
		}
		if(node_cfg.reward_list.empty() || (int)node_cfg.reward_list.size() > MAX_ATTACHMENT_ITEM_NUM) return -102;

		std::map<int, std::vector<MysteriousTrialRewardCfg> >::iterator it = m_mysterious_trial_reward_cfg.find(reward_id);
		if (it != m_mysterious_trial_reward_cfg.end() && !it->second.empty())
		{
			if (it->second.rbegin()->create_timestamp >= node_cfg.create_timestamp)
			{
				return -103;
			}
		}

		m_mysterious_trial_reward_cfg[reward_id].push_back(node_cfg);
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityConfigManager::InitMysteriousTrialCfg(TiXmlElement* RootElement)
{
	if (NULL == RootElement)
	{
		return -10000;
	}

	TiXmlElement* data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -20000;
	}
	while (NULL != data_element)
	{
		int type = 0;
		if (!GetSubNodeValue(data_element, "type", type) || type < MYSTERIOUS_TRIAL_TYPE_YUSHEN || type >= MYSTERIOUS_TRIAL_TYPE_MAX)
		{
			return -11;
		}

		MysteriousTrialCfg node_cfg;
		if (!GetSubNodeValue(data_element, "start_time", node_cfg.start_time) || node_cfg.start_time <= 0 || node_cfg.start_time > RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM)
		{
			return -10;
		}

		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0 || seq >= RA_NEW_MYSTERIOUS_TRIAL_DAY_TASK_MAX_NUM) return -1;

		node_cfg.seq = seq;
		if (!GetSubNodeValue(data_element, "activity_type", node_cfg.activity_type) || node_cfg.activity_type < 0 || node_cfg.activity_type >= MSC_CON_MAX) return -2;
		if (!GetSubNodeValue(data_element, "parameter1", node_cfg.parameter1)) return -3;
		if (!GetSubNodeValue(data_element, "parameter2", node_cfg.parameter2)) return -4;
		if (!GetSubNodeValue(data_element, "parameter3", node_cfg.parameter3)) return -5;
		if (!GetSubNodeValue(data_element, "parameter4", node_cfg.parameter4)) return -6;

		UNSTD_STATIC_CHECK(62 == MSC_CON_MAX)
			switch (node_cfg.activity_type)
			{
			case MSC_CON_ROLE_LEVEL:
			case MSC_CON_PROF_PRO:
			case MSC_CON_PET_INTEN:
			case MSC_CON_COUR_CHAG:
			case MSC_CON_PARTNER_NUM:
			case MSC_CON_PET_CAPABIL:
			case MSC_CON_GEMS_MOSAIC:
			case MSC_CON_PARTNER_PRO:
			case MSC_CON_JOIN_FAMILY:
			case MSC_CON_USER_CAPABIL:
			case MSC_CON_COURAGE_CHALLENGE:
			case MSC_CON_COLOSSEUM:
			case MSC_CON_ROLE_SKILL_LEVEL:
			case MSC_CON_GET_MEDAL:
			case MSC_CON_PARTNER_FETTERS:
			case MSC_CON_CATCH_PET:
			case MSC_CON_SPECIAL_PET_INTEN:
			case MSC_CON_MI_YI:
			case MSC_CON_PRESTIG_LEVEL:
			case MSC_CON_PET_SKILL_NUM:
			case MSC_CON_GET_FA_BAO:
			case MSC_CON_UNLOCK_SMART_MOUNTS:
			case MSC_CON_ACT_COLLECT_QUAILTY:
			case MSC_CON_DREAM_NOTES_CHALLENGE:
			case MSC_CON_LUN_HUI_WANG_CHUAN:
			case MSC_CON_CLOUD_ARENA:
			case MSC_CON_BRAVE_GROUD_PASS:
			case MSC_CON_HUAN_JIE_ZHAN_CHANG:
			case MSC_CON_PARTNER_EQUIPMENT:
			case MSC_CON_PARTNER_QUALITY:
			case MSC_CON_EQUIP_FULL_SET_STR:
			case MSC_CON_PET_MONOPOLY:
			case NSC_CON_REALM_BREAK_FIGHT:
			case MSC_CON_PASS_MI_YI_BOSS:
				if (node_cfg.parameter1 != 0) return -1001;
				if (node_cfg.parameter2 <= 0) return -1002;
				break;
			case MSC_CON_LOGIN_DAY:
			case MSC_CON_EQPM_FORG:
			case MSC_CON_COMP_TREA:
			case MSC_CON_COMPLETE_FB:
			case MSC_CON_ONLINE_TIME:
			case MSC_CON_TEMP_COMP_FB:
			case MSC_CON_PET_USE_BOOK:
			case MSC_CON_TRIAL_BOSS:
			case MSC_CON_TRA_MAR_PUR:
			case MSC_CON_KILL_YW_BOSS:
			case MSC_CON_ACT_COLLECT:
			case MSC_CON_SHOP_CONSUME:
			case MSC_CON_JOIN_XIYOUFB:
			case MSC_CON_KILL_SHAN_HAI_BOSS:
			case MSC_CON_NTHER_WORLD_PURGATORY:
			case MSC_CON_NTHER_WORLD_PURGATORY_ALL:
			case MSC_CON_SPECIAL_PET_ABANDON:
			case MSC_CON_PET_REFINED:
			case MSC_CON_PARTNER_BREAK:
			case MSC_CON_FAST_PASS_DREAM_NOTES:
			case MSC_CON_PAR_LIMIT_ACTIVITY:
			case MSC_CON_PET_INTEN_NUMBER:
			case MSC_CON_MEDAL_UP_GRADE:
			case MSC_CON_MARKET_SHOPPING:
			case MSC_CON_PAR_CLOUD_ARENA:
			case MSC_CON_PET_LIAN_YAO:
				if (node_cfg.parameter1 != 1) return -1003;
				if (node_cfg.parameter2 <= 0) return -1004;
				break;
			case MSC_CON_KILL_BOSS:
				if (node_cfg.parameter1 != 2) return -1005;
				if (node_cfg.parameter2 <= 0) return -1006;
				break;
			case MSC_CON_JOIN_XIYOUFB_LEVEL:
				if (node_cfg.parameter1 != 3) return -1009;
				if (node_cfg.parameter2 < 0) return -1010;
				break;
			}
		UNSTD_STATIC_CHECK(62 == MSC_CON_MAX)	//需要特殊处理,需要使用参数3,4的判断一下
			switch (node_cfg.activity_type)
			{
			case MSC_CON_GEMS_MOSAIC:
			case MSC_CON_COUR_CHAG:
			{
				if (node_cfg.parameter3 <= 0) return -2000;
			}
			break;
			case MSC_CON_MI_YI:
			{
				if (node_cfg.parameter2 != 1) return -2001;
				if (node_cfg.parameter3 < 0 || node_cfg.parameter3 >= MI_YI_MAX_LEVEL) return -2002;
			}
			break;
			case MSC_CON_GET_MEDAL:
			{
				if (node_cfg.parameter2 > MAX_MEDAL_WEAR_GRID) return -2003;
				if (node_cfg.parameter3 <= ItemBase::I_COLOR_INVALID || node_cfg.parameter3 > ItemBase::I_COLOR_RED) return -2004;
			}
			break;
			case MSC_CON_UNLOCK_SMART_MOUNTS:
			{
				if (node_cfg.parameter3 == 0)
				{
					if (node_cfg.parameter2 > MAX_SMART_MOUNTS_SYSTEM_NUM) return -2005;
				}
				else if (node_cfg.parameter3 == 1)
				{
					if (node_cfg.parameter2 > MAX_SMART_MOUNTS_SYSTEM_JEWELRY_LEVEL_NUM) return -2006;
				}
				else
				{
					return -2007;
				}
			}
			break;
			case MSC_CON_ACT_COLLECT_QUAILTY:
			{
				if (node_cfg.parameter3 < COLLECTION_COLOR_TYPE_INVALID || node_cfg.parameter3 >= COLLECTION_COLOR_TYPE_MAX) return -2008;
			}
			break;
			case MSC_CON_KILL_SHAN_HAI_BOSS:
			{
				if (node_cfg.parameter3 < SHAN_HAI_BOSS_GRADE_TYPE_INVAILD || node_cfg.parameter3 >= SHAN_HAI_BOSS_GRADE_TYPE_MAX) return -2009;
			}
			break;
			case MSC_CON_GET_FA_BAO:
			{
				if (node_cfg.parameter2 > MAX_FABAO_COUNT) return -2010;

				if (node_cfg.parameter4 < ItemBase::I_COLOR_INVALID || node_cfg.parameter4 > ItemBase::I_COLOR_RED) return -2011;
			}
			break;
			case MSC_CON_PARTNER_PRO:
			{
				if (node_cfg.parameter2 > PARTNER_NUM_MAX) return -2012;
				if (node_cfg.parameter3 <= 0 || node_cfg.parameter3 >= MAX_UINT8) return -2013;
			}
			break;
			case MSC_CON_PARTNER_QUALITY:
			{
				if (node_cfg.parameter2 > PARTNER_NUM_MAX) return -2014;
			}
			break;
			case MSC_CON_SPECIAL_PET_INTEN:
			{
				if (node_cfg.parameter2 >= PET_QUALITY_MAX) return -2015;
				if (node_cfg.parameter3 <= 0 || node_cfg.parameter3 > PET_STRENGTHEN_MAX_LEVEL) return -2016;
			}
			break;
			case MSC_CON_SPECIAL_PET_ABANDON:
				{
					if (node_cfg.parameter3 < 0) return -2017;
				}
				break;
			case MSC_CON_PET_REFINED:
			case MSC_CON_PET_INTEN_NUMBER:
			{
				if (node_cfg.parameter3 < 0 || node_cfg.parameter3 >= PET_QUALITY_MAX) return -2018;
			}
			break;
			case MSC_CON_PARTNER_BREAK:
			{
				if (node_cfg.parameter3 < 0 || node_cfg.parameter3 >= PQT_MAX) return -2019;
			}
			break;
			case MSC_CON_PET_USE_BOOK:
			{
				if (node_cfg.parameter3 < 0 || node_cfg.parameter3 >= PET_QUALITY_MAX) return -2020;
				if (node_cfg.parameter4 < 0 || node_cfg.parameter4 >= PET_SKILL_PRIORITY_TYPE_MAX) return -2021;
			}
			break;
			case MSC_CON_PET_MONOPOLY:
			{
				//初始就是1层
				if (node_cfg.parameter2 <= 1) return -2022;
			}
			break;
			case MSC_CON_MEDAL_UP_GRADE:
			{
				if (node_cfg.parameter3 < 0 || node_cfg.parameter3 >= ItemBase::I_COLOR_GOD_COLOR_BEGAIN) return -2023;
			}
			break;
			case MSC_CON_PASS_MI_YI_BOSS:
			{
				if (node_cfg.parameter2 != 1) return -2024;
				if (node_cfg.parameter3 < 0 || node_cfg.parameter3 >= MI_YI_MAX_LEVEL) return -2025;
				if (node_cfg.parameter4 <= 0 || node_cfg.parameter4 > MI_YI_MAX_MONSTER_NUM) return -2026;
			}
			break;
			case MSC_CON_EQPM_FORG:
			{
				if (node_cfg.parameter3 < ItemBase::I_COLOR_INVALID || node_cfg.parameter3 >= ItemBase::I_COLOR_MAX) return -2027;
				if (node_cfg.parameter4 < 0 || node_cfg.parameter4 > MAX_ROLE_LEVEL) return -2028;
			}
			break;
			case MSC_CON_PET_LIAN_YAO:
			{
				if (node_cfg.parameter3 < 0 || node_cfg.parameter3 >= PET_QUALITY_MAX) return -2029;
			}
			break;
			case MSC_CON_PET_INTEN:
			{
				if (node_cfg.parameter2 <= 0 || node_cfg.parameter2 > PET_STRENGTHEN_MAX_LEVEL) return -2030;
				if (node_cfg.parameter3 < 0 || node_cfg.parameter3 >= PET_QUALITY_MAX) return -2031;
			}
			break;
			case MSC_CON_NTHER_WORLD_PURGATORY_ALL:
			{
				if (node_cfg.parameter3 < 0 || node_cfg.parameter3 >= 3) return -2032;
			}
			break;
			default:
				break;
			}

		if (!GetSubNodeValue(data_element, "reward_id", node_cfg.reward_id) || m_mysterious_trial_reward_cfg.end() == m_mysterious_trial_reward_cfg.find(node_cfg.reward_id)) return -7;

		m_mysterious_cfg[type][node_cfg.start_time].node_cfg[node_cfg.seq] = node_cfg;
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityConfigManager::InitMysteriousTrialFinallyCfg(TiXmlElement *RootElement)
{
	if (NULL == RootElement)
	{
		return -10000;
	}

	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -20000;
	}
	while (NULL != data_element)
	{
		int type = 0;
		if (!GetSubNodeValue(data_element, "type", type) || type < MYSTERIOUS_TRIAL_TYPE_YUSHEN || type >= MYSTERIOUS_TRIAL_TYPE_MAX)
		{
			return -1;
		}

		int seq = 0;
		if (!GetSubNodeValue(data_element, "seq", seq) || seq < 0 || seq >= RA_MYSTERIOUS_TRIAL_FINALLY_TASK_MAX_NUM) return -2;

		MysteriousTrialFinallyCfg node_cfg;
		node_cfg.seq = seq;
		if (!GetSubNodeValue(data_element, "request_score", node_cfg.request_score)) return -3;

		TiXmlElement * list_node = data_element->FirstChildElement("reward_item_list");
		if (NULL != list_node)
		{
			TiXmlElement * item_node = list_node->FirstChildElement("reward_item");
			if (NULL != item_node)
			{
				if (!node_cfg.item.ReadConfig(item_node)) return -100;

				item_node = item_node->NextSiblingElement();
			}
		}
		m_mysterious_trial_finally_cfg[type][node_cfg.seq] = node_cfg;
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int RandActivityConfigManager::InitMysteriousTrialBuyCfg(TiXmlElement *RootElement)
{
	if (NULL == RootElement)
	{
		return -10000;
	}

	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -20000;
	}
	while (NULL != data_element)
	{
		int type = 0;
		if (!GetSubNodeValue(data_element, "type", type) || type < MYSTERIOUS_TRIAL_TYPE_YUSHEN || type >= MYSTERIOUS_TRIAL_TYPE_MAX)
		{
			return -1;
		}

		MysteriousTraialBuyCfg node;
		if (!GetSubNodeValue(data_element, "start_time", node.start_time) || node.start_time <= 0 || node.start_time > RA_MYSTERIOUS_TRIAL_MAX_DAY_NUM) return -2;
		if (!GetSubNodeValue(data_element, "price", node.price) || node.price < 0) return -3;

		TiXmlElement * item_node = data_element->FirstChildElement("item");
		if (item_node != NULL)
		{
			if (!node.item.ReadConfig(item_node)) return -3;
		}

		m_mysterious_trial_buy_cfg[type][node.start_time] = node;
		data_element = data_element->NextSiblingElement();
	}

	return 0;
}

int RandActivityConfigManager::InitLoginSystemRewardCfg(TiXmlElement * RootElement)
{
	TiXmlElement *data_element = RootElement->FirstChildElement("data");
	if (NULL == data_element)
	{
		return -999;
	}
	unsigned int last_open_sever_timestamp = 0;
	while (NULL != data_element)
	{
		unsigned int open_server_timestamp = 0;
		if (!GetSubNodeValue(data_element, "time", open_server_timestamp) || open_server_timestamp <= 0)
		{
			return -1;
		}
		if(last_open_sever_timestamp > open_server_timestamp) return -2;

		if (last_open_sever_timestamp != open_server_timestamp)
		{
			RALoginSystemRewardGroupCfg cfg; cfg.open_server_timestamp = open_server_timestamp;
			m_login_system_vec.push_back(cfg);
			last_open_sever_timestamp = open_server_timestamp;
		} 
		RALoginSystemRewardGroupCfg& node_group_cfg = m_login_system_vec[m_login_system_vec.size() - 1];
		 
		int login_days = 0;
		if (!GetSubNodeValue(data_element, "login_days", login_days) || login_days <= 0 || login_days > MAX_LOGIN_SYSTEM_NUM)return -2;

		if (node_group_cfg.login_reward.find(login_days) != node_group_cfg.login_reward.end())
		{
			return -3;
		}

		RALoginSystemRewardCfg & node_cfg = node_group_cfg.login_reward[login_days];
		node_cfg.login_days = login_days;

		if (!GetSubNodeValue(data_element, "seq", node_cfg.seq))
		{
			return -4;
		}

		TiXmlElement * list_node = data_element->FirstChildElement("reward_item_list");
		if (NULL != list_node)
		{
			TiXmlElement * item_node = list_node->FirstChildElement("reward_item");
			if (NULL != item_node)
			{
				ItemConfigData node;
				if (!node.ReadConfig(item_node)) return -100;
				node_cfg.reward_list.push_back(node);
				item_node = item_node->NextSiblingElement();
			}
		}
		data_element = data_element->NextSiblingElement();
	}
	return 0;
}

int InvalidTimeActivityCfg::count = 0;
int RandActivityConfigManager::InitInvalidTimeActivity(TiXmlElement * RootElement)
{
	TiXmlElement *root_element = RootElement->FirstChildElement("data");
	if (NULL == root_element) return -1000;

	memset(item_activity, 0, sizeof(item_activity));
	while (NULL != root_element)
	{
		InvalidTimeActivityCfg ita_cfg;
		if (!GetSubNodeValue(root_element, "seq", ita_cfg.seq) || ita_cfg.seq < 0) return -1;
		if (!GetSubNodeValue(root_element, "activity_id", ita_cfg.activity_id) || ita_cfg.activity_id < RAND_ACTIVITY_TYPE_BEGIN || ita_cfg.activity_id > RAND_ACTIVITY_TYPE_END) return -2;
		if (!GetSubNodeValue(root_element, "item_id", ita_cfg.item_id) || ita_cfg.item_id <= 0) return -3;
		int local_activity = 0;
		if (!GetSubNodeValue(root_element, "local_activity", local_activity)) return -4;

		if (m_seq_to_id_info.find(ita_cfg.seq) != m_seq_to_id_info.end()) return -200;
		m_seq_to_id_info[ita_cfg.seq] = ita_cfg;
		m_id_to_exsit[ita_cfg.item_id] = true;
		m_itemid_to_actiid[ita_cfg.item_id] = ita_cfg.activity_id;
		item_activity[ita_cfg.item_id].activity_id = ita_cfg.activity_id;
		item_activity[ita_cfg.item_id].local_activity = local_activity == 1;

		++InvalidTimeActivityCfg::count;
		root_element = root_element->NextSiblingElement();
	}

	return 0;
}

void RandActivityConfigManager::Release()
{
	for (int i = 0; i < ARRAY_ITEM_COUNT(m_rand_activity_cfg_arr); ++i)
	{
		if (NULL != m_rand_activity_cfg_arr[i])
		{
			delete m_rand_activity_cfg_arr[i];
			m_rand_activity_cfg_arr[i] = NULL;
		}
	}
}
