#include <memory>
#include <set>

#include "global/worldstatus/worldstatus.hpp"
#include "config/sharedconfig/sharedconfig.h"
#include "globalconfig/globalconfig.h"
#include "config/logicconfigmanager.hpp"

#include "gamelog.h"
#include "randactivity.hpp"
#include "randactivitymanager.hpp"
#include "gameworld/internalcomm.h"
#include "battle/battle_manager_local.hpp"
#include "servercommon/activitydef.hpp"
#include "config/activityconfig/randactivityopencfg.hpp"
#include "servercommon/struct/global/activitydatalistparam.hpp"
#include "servercommon/internalprotocal/crossgameprotocal.h"
#include "servercommon/serverconfig/crossconfig.hpp"

#include "global/randactivity/randactivityimpl/randactivityturntable.h"
#include "global/randactivity/randactivityimpl/randactivityskydropgift.hpp"
#include "global/randactivity/randactivityimpl/randactivitydiscountstore.hpp"
#include "global/randactivity/randactivityimpl/randactivityconsumecarousel.hpp"
#include "global/randactivity/randactivityimpl/randactivityexpelmonster.hpp"
#include "global/randactivity/randactivityimpl/randactivityheritagetreasure.h"
#include "global/randactivity/randactivityimpl/randactivitysevencolorpicture.hpp"
#include "global/randactivity/randactivityimpl/randactivitysaleshop.hpp"
#include "global/randactivity/randactivityimpl/randactivityhappup.hpp"
#include "global/randactivity/randactivityimpl/randactivityanimalrace.hpp"
#include "global/randactivity/randactivityimpl/randactivityluckybag.h"
#include "global/randactivity/randactivityimpl/randactivityreducetaxrate.hpp"
#include "global/randactivity/randactivityimpl/randactivitytianjianghaoli.hpp"
#include "global/randactivity/randactivityimpl/randactivitysealpromote.hpp"
#include "global/randactivity/randactivityimpl/randactivitydropdouble.hpp"
#include "global/randactivity/randactivityimpl/randactivityjoyseeksfavors.hpp"
#include "global/randactivity/randactivityimpl/randactivitypartnerexchange.h"
#include "global/randactivity/randactivityimpl/randactivitydiscountcarnival.hpp"
#include "global/randactivity/randactivityimpl/randactivitycollectword.hpp"
#include "global/randactivity/randactivityimpl/randactivityluckydiscount.hpp"
#include "global/randactivity/randactivityimpl/randactivitypetexchange.h"
#include "global/randactivity/randactivityimpl/randactivitytianmingdivination.hpp"
#include "global/randactivity/randactivityimpl/randactivityjiandanqinxin.hpp"
#include "global/randactivity/randactivityimpl/randactivityadventuregift.hpp"
#include "global/randactivity/randactivityimpl/randactivitylinkagegift.hpp"
#include "global/randactivity/randactivityimpl/randactivityluckyturntable.hpp"
#include "global/randactivity/randactivityimpl/randactivityquanmindiscount.hpp"
#include "global/randactivity/randactivityimpl/randactivitysports.hpp"
#include "global/randactivity/randactivityimpl/randactivitytujiantreasure.hpp"
#include "global/randactivity/randactivityimpl/randactivitygodtoken.hpp"
#include "global/randactivity/randactivityimpl/randactivitycaishenjubao.h"
#include "global/randactivity/randactivityimpl/randactivitygivepetdraw.hpp"
#include "global/randactivity/randactivityimpl/randactivitycijiuyingxin.hpp"
#include "global/randactivity/randactivityimpl/randactivityniuxingdayun.hpp"
#include "global/randactivity/randactivityimpl/randactivityspringfestivalstore.hpp"
#include "global/randactivity/randactivityimpl/randactivitymonpolygift.hpp"
#include "global/randactivity/randactivityimpl/randactivitycowexchange.hpp"
#include "global/randactivity/randactivityimpl/randactivitycowstore.hpp"
#include "global/randactivity/randactivityimpl/randactivitycowluckyexchange.hpp"
#include "global/randactivity/randactivityimpl/randactivitylittleyeargift.hpp"
#include "global/randactivity/randactivityimpl/randactivitygivesecretkey.hpp"
#include "global/randactivity/randactivityimpl/randactivitycumulativerecharge.hpp"
#include "global/randactivity/randactivityimpl/randactivitypetloot.hpp"
#include "global/randactivity/randactivityimpl/randactivitytrademarketreturngold.hpp"
#include "global/randactivity/randactivityimpl/randactivityshenshoujianglin.hpp"
#include "global/randactivity/randactivityimpl/randactivitygoodgiftturn.hpp"
#include "global/randactivity/randactivityimpl/randactivityfortunemaze.hpp"
#include "global/randactivity/randactivityimpl/randactivityhelpothers.hpp"
#include "global/randactivity/randactivityimpl/randactivityhelpothers2.hpp"
#include "global/randactivity/randactivityimpl/randactivitysecrettreasure.hpp"
#include "global/randactivity/randactivityimpl/randactivitymijingqiwen.hpp"
#include "global/randactivity/randactivityimpl/randactivitymijingqiwen2.hpp"
#include "global/randactivity/randactivityimpl/randactivitylaodonghaoli.hpp"
#include "global/randactivity/randactivityimpl/randactivitysummerlilian.hpp"
#include "global/randactivity/randactivityimpl/randactivitycolorfullantern.hpp"
#include "global/randactivity/randactivityimpl/randactivitysummerstore.hpp"
#include "global/randactivity/randactivityimpl/randactivityluckysign.hpp"
#include "global/randactivity/randactivityimpl/randactivityqionglouyuyan.hpp"
#include "global/randactivity/randactivityimpl/randactivitysuperoffshop.hpp"
#include "global/randactivity/randactivityimpl/randactivityyuanqiturning.hpp"
#include "global/randactivity/randactivityimpl/randactivitycoolbreeze.hpp"
#include "global/randactivity/randactivityimpl/randactivitylotusmibao.hpp"
#include "global/randactivity/randactivityimpl/randactivitymazesummer.hpp"
#include "global/randactivity/randactivityimpl/randactivitymibaofangdian.hpp"
#include "global/randactivity/randactivityimpl/randactivitypaimaihang.hpp"
#include "global/randactivity/randactivityimpl/randactivityservercompetition.hpp"
#include "global/randactivity/randactivityimpl/randactivityhorcruxtiancheng.hpp"
#include "global/randactivity/randactivityimpl/randactivityintegraldraw.hpp"
#include "global/randactivity/randactivityimpl/randactivityshanhaiquestion.hpp"
#include "global/randactivity/randactivityimpl/randactivityshanhaimeet.hpp"
#include "global/randactivity/randactivityimpl/randactivityfateentanglement.hpp"
#include "global/randactivity/randactivityimpl/randactivityyunzeterritory.hpp"
#include "global/randactivity/randactivityimpl/randactivityyunzebloomingrecord.hpp"
#include "global/randactivity/randactivityimpl/randactivityyunzeslotmachine.hpp"
#include "global/randactivity/randactivityimpl/randactivitymozuzainali.hpp"
#include "global/randactivity/randactivityimpl/randactivityyunzeersanshi.hpp"
#include "global/randactivity/randactivityimpl/randactivityshanhaizhengfeng.hpp"
#include "global/randactivity/randactivityimpl/randactivityshanhaimibao.hpp"
#include "global/randactivity/randactivityimpl/randactivityjixinggaozhao.hpp"
#include "global/randactivity/randactivityimpl/randactivityyunzewish.hpp"
#include "global/randactivity/randactivityimpl/randactivityyunzehaoli.hpp"
#include "global/randactivity/randactivityimpl/randactivityyunzecutepet.hpp"
#include "global/randactivity/randactivityimpl/randactivityyunzecatchelves.hpp"
#include "global/randactivity/randactivityimpl/randactivityxianshileichong.hpp"
#include "global/randactivity/randactivityimpl/randactivitytigerauspicioussnow.hpp"
#include "global/randactivity/randactivityimpl/randactivitytigerfisheveryyear.hpp"
#include "global/randactivity/randactivityimpl/randactivitytigerfindspring.hpp"
#include "global/randactivity/randactivityimpl/randactivitytigerteasenewyear.hpp"
#include "global/randactivity/randactivityimpl/randactivitytigerluckymoney.hpp"
#include "global/randactivity/randactivityimpl/randactivityjingjiyueka.hpp"
#include "global/randactivity/randactivityimpl/randactivitygodbeastadvent.hpp"
#include "global/randactivity/randactivityimpl/randactivityxunmenghudie.hpp"
#include "global/randactivity/randactivityimpl/randactivitychengfengpolang.hpp"
#include "global/randactivity/randactivityimpl/randactivitysaltysweetbattle.hpp"
#include "global/randactivity/randactivityimpl/randactivitycharismatictanabata.hpp"

#include "global/datasynccheck/datasynccheck.hpp"

#include "world.h"
#include "radelaysettimemgr.hpp"

RandActivityManager::RandActivityManager() : m_is_first(true)
{
	memset(m_rand_activity_list, 0, sizeof(m_rand_activity_list));

	m_rand_activity_list[RAND_ACTIVITY_TYPE_TIME_LIMIT_STORE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_TIME_LIMIT_STORE);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_TRUN_TABLE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityTurnTable(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_DISCOUNT_STORE - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityDiscountStore(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SKY_DROP_LUXURY_GIFT - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivitySkyDropGift(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityConsumeCarousel(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_EXPEL_MONSTER - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityExpelMonster(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_HERITAGE_TREASURE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityHeritageTreasure(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivitySevenColorPicture(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SALE_SHOP - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivitySaleShop(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_HAPPY_UP - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityHappyUp(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_ANIMAL_RACE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityAnimalRace(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_LUCKY_BAG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityLuckyBag(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_REDUCE_TAX_RATE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityReduceTaxRate(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_TIAN_JIANG_HAO_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityTianJiangHaoLi(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SEAL_PROMOTE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivitySealPromote(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_DROP_DOUBLE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityDropDouble(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_JOYSEEKSFAVORS - RAND_ACTIVITY_TYPE_BEGIN] = new RandactivityJoyseeksfavors(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_COLLECT_WORD - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityCollectWord(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_DISCOUNT_CARNIVAL - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityDiscountCarnival(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_PARTNER_EXCHANGE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityPartnerExchange(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_LUCKY_DISCOUNT - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityLuckyDiscount(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_PET_EXCHANGE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityPetExchange(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityJianDanQinXin(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_ADVENTURE_GIFT - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityAdventureGift(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_LINKAGE_GIFT - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityLinkageGift(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityLuckyTable(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityTianMingDivination(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityQuanMinDiscount(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SPORTS - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivitySports(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityTuJianTreasure(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_GOD_TOKEN - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityGodToken(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_ELEMENT_CARDS - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_ELEMENT_CARDS);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SIGN_IN_CONTINUITY - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_SIGN_IN_CONTINUITY);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_CAI_SHEN_JU_BAO - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityCaiShenJuBao(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_ZHUAN_SHU_LI_BAO - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_ZHUAN_SHU_LI_BAO);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_JING_YAN_BIAO_SHENG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_JING_YAN_BIAO_SHENG);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_GIVE_PET_DRAW - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityGivePetDraw(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_CI_JIU_YING_XIN - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityCiJiuYingXin(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_NIU_XING_DA_YUN - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityNiuXingDaYun(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivitySpringFestivalStore(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_MONPOLY_GIFT - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityMonpolyGift(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_COW_EXCHANGE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityCowExchange(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_COW_STORE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityCowStore(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityCowLuckyExchange(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_LITTLE_YEAR_GIFT - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityLittleYearGift(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_GIVE_SECRET_KEY - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityGiveSecretKey(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityCumulativeCharge(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_PET_LOOT - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityPetLoot(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_TRADEMARKET_GOLD_RETURN - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityTradeMarketReturnGold(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityShenShouJiangLin(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityGoodGiftTurn(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivitySecretTreasure(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_HELP_OTHERS - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityHelpOthers(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_FORTUNE_MAZE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityFortuneMaze(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_CHUN_RI_DENG_FENG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_CHUN_RI_DENG_FENG);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_ANECDOTE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityMiJingQiWen(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityLaoDongHaoLi(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_ZU_HE_HE_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_ZU_HE_HE_LI);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_FASHION_SHOP - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_FASHION_SHOP);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_ONE_YUAN_GOU - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_ONE_YUAN_GOU);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_LING_YU_JI_JIN - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_LING_YU_JI_JIN);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI_2 - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI_2);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SUMMER_LI_LIAN - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivitySummerLiLian(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_COLORFUL_LANTERN - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityColorfulLantern(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SUMMER_STORE - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivitySummerStore(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_LUCKY_SIGN - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityLuckySign(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityQiongLouYuYan(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivitySuperOffShop(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_YUAN_QI_TURNING - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityYuanQiTurning(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_COOL_BREEZE - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityCoolBreeze(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_LOTUS_MI_BAO - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityLotusMiBao(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_MAZE_SUMMER - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityMazeSummer(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityMiBaoFangDian(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_ZHEN_PIN_SHANG_PU - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_ZHEN_PIN_SHANG_PU);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_PAI_MAI_HANG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityPaiMaiHang(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI_3 - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI_3);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_DUI_HUAN_LING_YU - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_DUI_HUAN_LING_YU);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_BACK_REWARD - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_BACK_REWARD);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SERVER_COMPETITION - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityServerCompetition(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_HAPPY_LOOK - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_HAPPY_LOOK);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SIGN_HAPPY - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_SIGN_HAPPY);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_HELP_OTHERS_2 - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityHelpOthers2(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_ANECDOTE_2 - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityMiJingQiWen2(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityHorcruxTianCheng(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_INTEGRAL_DRAW - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityIntegralDraw(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SHANHAI_QUESTION - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityShanHaiQuestion(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SHAN_HAI_JU_BAO - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_SHAN_HAI_JU_BAO);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SHAN_HAI_MET - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_SHAN_HAI_MET);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SHAN_HAI_MEET - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityShanHaiMeet(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_FATE_ENTANGLEMENT - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_FATE_ENTANGLEMENT);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_YUN_ZE_TERRITORY - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityYunZeTerritory(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityYunZeBloomingRecord(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE -RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityYunZeSlotMachine(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_ZAI_ZHAN_YUN_ZE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_ZAI_ZHAN_YUN_ZE);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_MO_ZU_ZAI_NA_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityMoZuZaiNaLi(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_YUN_ZE_ER_SAN_SHI - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityYunZeErSanShi(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityShanHaiZhengFeng(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityShanHaiMiBao(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_COLORFUL_DOUBLE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_COLORFUL_DOUBLE);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SHANHAI_HUADENG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_SHANHAI_HUADENG);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_XIANG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_XIANG);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_HE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_HE);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_DAI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_DAI);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_2 - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_2);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_2 - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_2);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI_2 - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI_2);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_JIXING_GAOZHAO - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityJiXingGaoZhao(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_YUN_ZE_WISH - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityYunZeWish(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_YUN_ZE_CATCH_ELVES - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_YUN_ZE_CATCH_ELVES);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityXianShiLeiChong(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_WEEKEND_GIFT - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_WEEKEND_GIFT);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_CHAO_ZHI_GIFT - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_CHAO_ZHI_GIFT);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_TIGER_TEASE_NEWYEAR - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_TIGER_TEASE_NEWYEAR);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_YINHU_WENSHI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_YINHU_WENSHI);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_CINIU_YINGXINHU - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_CINIU_YINGXINHU);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_HU_XING_DA_YUN - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_HU_XING_DA_YUN);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_YIN_HU_BAI_SUI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_YIN_HU_BAI_SUI);	
	m_rand_activity_list[RAND_ACTIVITY_TYPE_QIYUAN_KONGMINGDENG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_QIYUAN_KONGMINGDENG);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_YUAN_QI_SPEED - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_YUAN_QI_SPEED);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_YUAN_QI_HELP - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_YUAN_QI_HELP);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_XUNMENG_HUDIE - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityXunMengHuDie(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityGodBeastAdvent(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_BO_ZHONG_YOU_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_BO_ZHONG_YOU_LI);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_JING_JI_YUE_KA - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityJingJiYueKa(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_RONG_LIAN_YOU_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_RONG_LIAN_YOU_LI);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_QING_YUAN_SHI_ZHUANG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_QING_YUAN_SHI_ZHUANG);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_QING_YUAN_LEI_CHONG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_QING_YUAN_LEI_CHONG);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_YUNA_QI_FAN_PAI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_YUNA_QI_FAN_PAI);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_BAO_ZI_LAI_LE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_BAO_ZI_LAI_LE);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_WANLING_ZHULI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_WANLING_ZHULI);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_JIN_YI_TIAN_XIANG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_JIN_YI_TIAN_XIANG);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_MIAO_BI_SHENG_HUA - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_MIAO_BI_SHENG_HUA);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_YUAN_QI_YAN_HUO - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_YUAN_QI_YAN_HUO);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_CHENG_FENG_PO_LANG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityChengFengPoLang(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_JING_JI_YUE_KA - RAND_ACTIVITY_TYPE_BEGIN] = new RandAcitivityJingJiYueKa(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_RONG_LIAN_YOU_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_RONG_LIAN_YOU_LI);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_QING_YUAN_SHI_ZHUANG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_QING_YUAN_SHI_ZHUANG);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_QING_YUAN_LEI_CHONG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_QING_YUAN_LEI_CHONG);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_ZONG_XIANG_MAN_CHENG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_ZONG_XIANG_MAN_CHENG);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivitySaltySweetBattle(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_DUAN_YANG_ZI_XUAN - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_DUAN_YANG_ZI_XUAN);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_CANG_LONG_CI_FU - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_CANG_LONG_CI_FU);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_LUCKY_BLIND_BOX - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_LUCKY_BLIND_BOX);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_300_CHOU - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_300_CHOU);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_DUAN_YANG_FU_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_DUAN_YANG_FU_LI);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_JIN_LONG_JU_BAO - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_JIN_LONG_JU_BAO);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_CHONG_JI_FAN_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_CHONG_JI_FAN_LI);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SHEN_SHOU_EN_ZE - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_SHEN_SHOU_EN_ZE);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_XIN_FU_QIANG_HUA - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_XIN_FU_QIANG_HUA);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_QI_XI_TE_HUI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_QI_XI_TE_HUI);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_CHARISMATIC_TANABATA - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivityCharismaticTanabata(this);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_3 - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_3);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_4 - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_4);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_3 - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_3);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_XIN_FU_LING_CHONG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_XIN_FU_LING_CHONG);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_HUN_QI_FAN_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_HUN_QI_FAN_LI);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_SHEN_CI_HUA_FU - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_SHEN_CI_HUA_FU);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_TIAN_YI_YAO_SHI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_TIAN_YI_YAO_SHI);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_YUE_XI_DENG_MI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_YUE_XI_DENG_MI);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_YUE_XI_LEI_CHONG - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_YUE_XI_LEI_CHONG);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_DAN_BI_FAN_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_DAN_BI_FAN_LI);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_ZHUI_YUE_SHANG_DIAN - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_ZHUI_YUE_SHANG_DIAN);
	m_rand_activity_list[RAND_ACTIVITY_TYPE_ZENG_100_CHOU - RAND_ACTIVITY_TYPE_BEGIN] = new RandActivity(this, RAND_ACTIVITY_TYPE_ZENG_100_CHOU);

	UNSTD_STATIC_CHECK(178 == RAND_ACTIVITY_TYPE_END - RAND_ACTIVITY_TYPE_BEGIN);

#ifdef _DEBUG

	for (int i = 1; i < ARRAY_LENGTH(m_rand_activity_list); ++i)
	{
		if (m_rand_activity_list[i] != NULL && m_rand_activity_list[i]->GetActivityType() - RAND_ACTIVITY_TYPE_BEGIN != i)
		{
			assert(0);
		}
	}

#endif 


	m_usid = LocalConfig::Instance().GetUniqueServerID();

	m_delay_set_time_mgr = new RADelaySetTimeMgr(this);
}

RandActivityManager::~RandActivityManager()
{
	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; i++)
	{
		if (NULL != m_rand_activity_list[i])
		{
			delete m_rand_activity_list[i];
		}
	}

	memset(m_rand_activity_list, 0, sizeof(m_rand_activity_list));

	delete m_delay_set_time_mgr;
	m_delay_set_time_mgr = NULL;
}

RandActivityManager* g_RandActivityManager = NULL;
RandActivityManager* RandActivityManager::Instance()
{
	if (NULL == g_RandActivityManager)
	{
		g_RandActivityManager = new RandActivityManager();
	}

	return g_RandActivityManager;
}

void RandActivityManager::InitActivity(int activity_type, const ActivityData& data)
{
	if (activity_type < RAND_ACTIVITY_TYPE_BEGIN || activity_type >= RAND_ACTIVITY_TYPE_END) return;

	if (NULL == m_rand_activity_list[activity_type - RAND_ACTIVITY_TYPE_BEGIN]) return;

	m_rand_activity_list[activity_type - RAND_ACTIVITY_TYPE_BEGIN]->Init(data);
}

void RandActivityManager::GetChangedActivityData(ActivityDataListParam* list_param)
{
	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX && list_param->count < ActivityDataListParam::LIST_COUNT; ++i)
	{
		if (NULL != m_rand_activity_list[i] && m_rand_activity_list[i]->IsDirty())
		{
			list_param->data_list[list_param->count].Reset(RAND_ACTIVITY_TYPE_BEGIN + i);
			list_param->data_list[list_param->count].activity_type = RAND_ACTIVITY_TYPE_BEGIN + i;
			m_rand_activity_list[i]->GetInitParam(list_param->data_list[list_param->count].activity_data);
			list_param->count++;
		}
	}
}

void RandActivityManager::GetActivityData(ActivityDataListParam* list_param)
{
	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX && list_param->count < ActivityDataListParam::LIST_COUNT; ++i)
	{
		if (NULL != m_rand_activity_list[i])
		{
			list_param->data_list[list_param->count].Reset(RAND_ACTIVITY_TYPE_BEGIN + i);
			list_param->data_list[list_param->count].activity_type = RAND_ACTIVITY_TYPE_BEGIN + i;
			m_rand_activity_list[i]->GetInitParam(list_param->data_list[list_param->count].activity_data);
			list_param->count++;
		}
	}
}

void RandActivityManager::ClearDirtyMark()
{
	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; ++i)
	{
		if (NULL != m_rand_activity_list[i])
		{
			m_rand_activity_list[i]->ClearDirtyMark();
		}
	}
}

void RandActivityManager::Update(unsigned long interval, time_t now_second)
{
	if (!WorldStatus::Instance().IsLoadFinish()) return;

	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; i++)
	{
		if (NULL != m_rand_activity_list[i])
		{
			m_rand_activity_list[i]->Update(interval, now_second);
		}
	}

	if (m_is_first)
	{
		m_is_first = false;

		this->CalActivityConfigOpenTime(true, __FUNCTION__);
	}

	m_delay_set_time_mgr->CheckDelaySetTime(static_cast<unsigned int>(now_second));
}

void RandActivityManager::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; i++)
	{
		if (NULL != m_rand_activity_list[i])
		{
			m_rand_activity_list[i]->OnDayChange(old_dayid, now_dayid);
		}
	}
}

void RandActivityManager::OnWeekChange()
{
	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; i++)
	{
		if (NULL != m_rand_activity_list[i])
		{
			m_rand_activity_list[i]->OnWeekChange();
		}
	}
}

void RandActivityManager::OnMonthChange()
{
	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; i++)
	{
		if (NULL != m_rand_activity_list[i])
		{
			m_rand_activity_list[i]->OnMonthChange();
		}
	}
}

void RandActivityManager::OnUserLogin(Role *user)
{
	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; i++)
	{
		if (NULL != m_rand_activity_list[i])
		{
			m_rand_activity_list[i]->OnUserLogin(user);
		}
	}
}

void RandActivityManager::OnUserLogout(Role *user)
{
	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; i++)
	{
		if (NULL != m_rand_activity_list[i])
		{
			m_rand_activity_list[i]->OnUserLogout(user);
		}
	}
}

bool RandActivityManager::IsRandActivityOpen(int rand_activity_type)
{
	rand_activity_type -= RAND_ACTIVITY_TYPE_BEGIN;
	if (rand_activity_type < 0 || rand_activity_type >= RAND_ACTIVITY_TYPE_MAX)
	{
		return false;
	}

	bool is_open = false;
	if (NULL != m_rand_activity_list[rand_activity_type])
	{
		is_open = (ACTIVITY_STATUS_OPEN == m_rand_activity_list[rand_activity_type]->GetStatus());
	}

	return is_open;
}

unsigned int RandActivityManager::GetRandActivityBeginTime(int rand_activity_type)
{
	rand_activity_type -= RAND_ACTIVITY_TYPE_BEGIN;
	if (rand_activity_type < 0 || rand_activity_type >= RAND_ACTIVITY_TYPE_MAX)
	{
		return 0;
	}

	if (NULL == m_rand_activity_list[rand_activity_type])
	{
		return 0;
	}

	return  m_rand_activity_list[rand_activity_type]->GetBeginTime();
}

unsigned int RandActivityManager::GetRandActivityEndTime(int rand_activity_type)
{
	rand_activity_type -= RAND_ACTIVITY_TYPE_BEGIN;
	if (rand_activity_type < 0 || rand_activity_type >= RAND_ACTIVITY_TYPE_MAX)
	{
		return 0;
	}

	if (NULL == m_rand_activity_list[rand_activity_type])
	{
		return 0;
	}

	return  m_rand_activity_list[rand_activity_type]->GetEndTime();
}

bool RandActivityManager::IsInRandActivityOpenTime(int rand_activity_type, unsigned int timestamp)
{
	rand_activity_type = rand_activity_type - RAND_ACTIVITY_TYPE_BEGIN;
	if (rand_activity_type < 0 || rand_activity_type >= RAND_ACTIVITY_TYPE_MAX) return false;

	if (m_rand_activity_list[rand_activity_type]->GetEndTime() < EngineAdapter::Instance().Time())
	{
		return false;
	}

	if (m_rand_activity_list[rand_activity_type]->GetBeginTime() <= timestamp && timestamp < m_rand_activity_list[rand_activity_type]->GetEndTime())
	{
		return true;
	}
	return false;
}

void RandActivityManager::OnSetActivityTime(int rand_activity_type, unsigned int begin_time, unsigned int end_time, int limit_day, int open_type)
{
	/*
		1.关闭活动：limit_day = ?, now_time > end_time > begin_time
		2.改变活动时间: limit_day = 0, end_time_> now_time, end_time > begin_time
		3.开服前n天不开:limit_day = n, end_time > begin_time, end_time ? now_time, begin_time ? now_time
	*/

	unsigned int now_time = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	{
		// 开服前N天内，设置活动开启时间不可以也在前N天内，除了关闭
		int limit_open_days = LOGIC_CONFIG->GetRandActivityOpenCfg()
			->GetLimitOpenDay(rand_activity_type);

		unsigned int allow_set_begin_time = 0u;
		if (limit_open_days > 0)
		{
			unsigned int server_open_zero_time = World::GetInstWorld()
				->GetServerOpenZeroTime(this->GetUniqueServerID());
			
			allow_set_begin_time = server_open_zero_time +
				(limit_open_days - 1) * SECOND_PER_DAY;
		}
		if (begin_time < allow_set_begin_time)
		{
			if (begin_time > now_time || end_time > now_time) // 不是关闭活动，一律返回
			{	
				gamelog::g_log_rand_activity.printf(LL_ERROR,"%s  not close the randactivity, limit_open_days[%d], begin_time[%d] now_time[%d] end_time[%d]", __FUNCTION__, limit_open_days, begin_time, now_time, end_time);
				return;
			}
		}
	}

	// 判断开服前n天不开活动，调整开始时间
	// 注意：结束时间小于当前时间为关闭活动，不判断limit_day
	if (limit_day > 0 && end_time > now_time)	
	{
		time_t server_open_six_time = World::GetInstWorld()->GetServerOpenZeroTime(this->GetUniqueServerID());
		if (GetDayIndex(server_open_six_time, begin_time) < limit_day)		// 修正活动开启时间
		{
			begin_time = (unsigned int)(server_open_six_time + limit_day * 24 * 3600);
		}
	}

	if (begin_time > end_time)
	{
		gamelog::g_log_rand_activity.printf(LL_ERROR, "%s begin_time cannot be greater than end_time, begin_time[%d] now_time[%d] end_time[%d]", __FUNCTION__, begin_time, now_time, end_time);
		return;
	}

	int rand_activity_idx = rand_activity_type - RAND_ACTIVITY_TYPE_BEGIN;
	if (rand_activity_idx < 0 || rand_activity_idx >= RAND_ACTIVITY_TYPE_MAX) return;

	if (NULL == m_rand_activity_list[rand_activity_idx])
	{
		gamelog::g_log_rand_activity.printf(LL_ERROR, "%s randactivity not NULL, begin_time[%d] now_time[%d] end_time[%d]", __FUNCTION__, begin_time, now_time, end_time);
		return;
	}

	// 当前活动正在开启中
	if (this->IsRandActivityOpen(rand_activity_type))
	{
		// 活动结束后要加开活动，加入活动延迟设置管理器中
		if (begin_time >= this->GetRandActivityEndTime(rand_activity_type))
		{
			// 设置到当前活动结束后再设置一次，防止结算跟重开活动冲突，推迟几秒
			unsigned int delay_to_time = this->GetRandActivityEndTime(rand_activity_type) + 3;
			m_delay_set_time_mgr->AddDelaySetTime(rand_activity_type, begin_time, end_time, open_type, delay_to_time, __FUNCTION__, false);		
			return;
		}
		else if (begin_time > now_time) // 在当前时间之后并且活动未关闭前要重新开启，说明会导致当前活动不正常关闭，直接忽略请求
		{
			gamelog::g_log_rand_activity.printf(LL_ERROR, "%s begin_time cannot be greater than now_time, begin_time[%d] now_time[%d] end_time[%d]", __FUNCTION__, begin_time, now_time, end_time);
			return;
		}

		// 其他情况一般是设置时间关活动，要允许！
	}

	m_rand_activity_list[rand_activity_idx]->SetRandActivityTime(begin_time, end_time, open_type, __FUNCTION__);
}

void RandActivityManager::OnForceSetActivityTime(int rand_activity_type, unsigned int begin_time, unsigned int end_time, int open_type)
{
	// 强制设置随机活动，只做参数检查，确定插命令设置时，参考一下OnSetActivityTime里面的检查条件

	int rand_activity_idx = rand_activity_type - RAND_ACTIVITY_TYPE_BEGIN;
	if (rand_activity_idx < 0 || rand_activity_idx >= RAND_ACTIVITY_TYPE_MAX)
	{
		gamelog::g_log_rand_activity.printf(LL_ERROR, "%s rand_activity_idx[%d] is error, rand_activity_type[%d] begin_time[%d] end_time[%d] open_type[%d]", 
			__FUNCTION__, rand_activity_idx, rand_activity_type, begin_time, end_time, open_type);
		return;
	}

	if (open_type < RAND_ACTIVITY_OPEN_TYPE_BEGIN || open_type >= RAND_ACTIVITY_OPEN_TYPE_END)
	{
		gamelog::g_log_rand_activity.printf(LL_ERROR, "%s open_type[%d] is error, rand_activity_type[%d] begin_time[%d] end_time[%d] rand_activity_idx[%d]",
			__FUNCTION__, open_type, rand_activity_type, begin_time, end_time, rand_activity_idx);
		return;
	}

	if (begin_time > end_time)
	{
		gamelog::g_log_rand_activity.printf(LL_ERROR, "%s begin_time cannot be greater than end_time, begin_time[%d] end_time[%d]",
			 __FUNCTION__, begin_time, end_time);
		return;
	}

	// 活动结束后要加开活动，加入缓存里延迟执行
	if (this->IsRandActivityOpen(rand_activity_type) && begin_time >= this->GetRandActivityEndTime(rand_activity_type))
	{
		unsigned int delay_to_time = this->GetRandActivityEndTime(rand_activity_type) + 3;
		m_delay_set_time_mgr->AddDelayForceSetTime(rand_activity_type, begin_time, end_time, open_type, delay_to_time, __FUNCTION__, false);
		return;
	}

	if (NULL != m_rand_activity_list[rand_activity_idx])
	{
		m_rand_activity_list[rand_activity_idx]->SetRandActivityTime(begin_time, end_time, open_type, __FUNCTION__);
	}
}

void RandActivityManager::OnForceToNextState(int rand_activity_type)
{
	rand_activity_type -= RAND_ACTIVITY_TYPE_BEGIN;
	if (rand_activity_type < 0 || rand_activity_type >= RAND_ACTIVITY_TYPE_MAX) return;

	if (NULL != m_rand_activity_list[rand_activity_type])
	{
		m_rand_activity_list[rand_activity_type]->OnForceToNextState();
	}
}

RandActivityConsumeCarousel * RandActivityManager::GetRAconsumeCarousel()
{
	RandActivity *activity = m_rand_activity_list[RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL - RAND_ACTIVITY_TYPE_BEGIN];
	if (NULL != activity && RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL == activity->GetActivityType())
	{
		return (RandActivityConsumeCarousel *)activity;
	}

	return NULL;
}

RandAcitivityExpelMonster * RandActivityManager::GetRAExpelMonster()
{
	RandActivity *activity = m_rand_activity_list[RAND_ACTIVITY_TYPE_EXPEL_MONSTER - RAND_ACTIVITY_TYPE_BEGIN];
	if (NULL != activity && RAND_ACTIVITY_TYPE_EXPEL_MONSTER == activity->GetActivityType())
	{
		return (RandAcitivityExpelMonster *)activity;
	}

	return NULL;
}

RandActivityAnimalRace * RandActivityManager::GetRAAnimalRace()
{
	RandActivity *activity = m_rand_activity_list[RAND_ACTIVITY_TYPE_ANIMAL_RACE - RAND_ACTIVITY_TYPE_BEGIN];
	if (NULL != activity && RAND_ACTIVITY_TYPE_ANIMAL_RACE == activity->GetActivityType())
	{
		return (RandActivityAnimalRace *)activity;
	}

	return NULL;
}

RandActivity * RandActivityManager::ForceGetRandActivity(int activity_type)
{
	if (activity_type <= RAND_ACTIVITY_TYPE_BEGIN || activity_type >= RAND_ACTIVITY_TYPE_END) return NULL;

	return m_rand_activity_list[activity_type - RAND_ACTIVITY_TYPE_BEGIN];
}


void RandActivityManager::CalActivityConfigOpenTime(bool is_from_init, const char* call_func)
{
	// 隐藏服自己的RAMgr不需要计算活动开启时间，而是靠同步
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; i++)
	{
		if (NULL != m_rand_activity_list[i])
		{
			const std::vector<RandActivityOpenDetail>* ra_open_cfg = LOGIC_CONFIG->GetRandActivityOpenCfg()->GetRaOpenCfg(m_rand_activity_list[i]->GetActivityType());
			if (NULL != ra_open_cfg && !ra_open_cfg->empty())
			{
				for (size_t k = 0; k < ra_open_cfg->size(); ++k)
				{
					m_delay_set_time_mgr->AddOpenCfg((*ra_open_cfg)[k], __FUNCTION__); // 统一用延迟管理器来开启活动（因为配置中可能存在相同活动不同时间）
				}	
			}
		}
	}
}

void RandActivityManager::SendActivityDataToCross()
{
	crossgameprotocal::GameCrossRandActivityInfoSync sync;
	sync.is_all_sync = 1;
	sync.count = 0;
	sync.plat_type = LocalConfig::Instance().GetPlatType();
	sync.server_id = *LocalConfig::Instance().GetAllowServerIdSet().begin();
	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; ++i)
	{
		if (NULL != m_rand_activity_list[i] && m_rand_activity_list[i]->CanSyncActivityDataToCross())
		{
			static ActivityData  act_data; act_data.Reset(m_rand_activity_list[i]->GetActivityType());
			m_rand_activity_list[i]->GetInitParam(act_data);
			RandActivityData & data = act_data.rand_activity_data;
			sync.param_list[sync.count].activity_type = m_rand_activity_list[i]->GetActivityType();
			sync.param_list[sync.count].open_time = data.begin_time;
			sync.param_list[sync.count].close_time = data.end_time;
			memcpy(sync.param_list[sync.count].data, data.data, sizeof(data.data));
			++sync.count;
		}
	}

	InternalComm::Instance().SendToCross((char *)&sync, sizeof(sync));
	DataSyncCheck::Instance().AddCheck(crossgameprotocal::DATA_SYNC_CHECK_TYPE_RAND_ACTIVITY);
}

void RandActivityManager::OnSyncActivityInfoFromCross(int count, CrossRandActivityDataParam * param_list)
{
	if (!CrossConfig::Instance().IsHiddenServer()) return;

	if (count <= 0)
	{
		return;
	}

	for (int i = 0; i < count && i < RAND_ACTIVITY_TYPE_MAX; ++i)
	{
		int activity_type = param_list[i].activity_type - RAND_ACTIVITY_TYPE_BEGIN;
		if (activity_type < 0 || activity_type >= RAND_ACTIVITY_TYPE_MAX) continue;

		m_rand_activity_list[activity_type]->SyncActivityDataFromCross(param_list[i]);
		m_rand_activity_list[activity_type]->SetDirty();
	}
}

void RandActivityManager::OnReloadActivityConfig()
{
	this->CalActivityConfigOpenTime(false, __FUNCTION__);
}
