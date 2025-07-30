#include "roleactivitymanager.hpp"

#include "roleactivity.hpp"
#include "servercommon/servercommon.h"
#include "servercommon/uniqueidgenerator.hpp"
#include "servercommon/serverconfig/crossconfig.hpp"
#include "servercommon/string/gameworldstr.h"

#include "config/logicconfigmanager.hpp"
#include "config/randactivityconfig/randactivitybonusconfig.hpp"
#include "config/randactivityconfig/randactivityconfigmanager.hpp"
#include "config/activityconfig/teamfight/activityteamfightconfig.hpp"
#include "item/knapsack.h"

#include "global/randactivity/randactivityadapter.hpp"
#include "global/randactivity/randactivitymanager.hpp"
#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activityteamfight.hpp"
#include "global/cmdbuyrecordmanager/cmdbuyrecordmanager.hpp"
#include "global/worldteamarenamanager/worldteamarenasignupmanager.hpp"
#include "obj/character/role.h"
#include "other/route/mailroute.hpp"
#include "other/rolemodulemanager.hpp"
#include "other/rolecross/rolecross.hpp"
#include "global/team/team.hpp"
#include "global/team/teammanager.hpp"
#include "other/capability/capability.hpp"
#include "other/funopen/funopenconfig.hpp"
#include "other/funopen/funopen.hpp"
#include "other/event/eventhandler.hpp"
#include "other/rolecommonsave/rolecommonsavemanager.hpp"
#include "global/cross/crossmanager.h"
#include "global/guild/guild.hpp"

#include "global/activity/activitymanager.hpp"
#include "global/activity/activityimpl/activityonlyfight.hpp"
#include "global/activity/activityimpl/activityguildfight.hpp"
#include "global/activity/activityimpl/activityguildanswer.hpp"
#include "global/activity/activityimpl/activityworldteamarena.hpp"

#include "config/activityconfig/activityconfig.hpp"
#include "config/activityconfig/monthcardconfig.hpp"
#include "config/activityconfig/onlyfight/activityonlyfightconfig.hpp"
#include "config/activityconfig/guildfight/activityguildfightconfig.hpp"
#include "config/activityconfig/worldteamarena/activityworldteamarenaconfig.hpp"

#include "other/roleactivity/impl/roleactivitytimelimitstore.hpp"
#include "other/roleactivity/impl/roleactivitytruntable.hpp"
#include "other/roleactivity/impl/roleactivitydiscountstore.hpp"
#include "other/roleactivity/impl/roleactivityconsumecarousel.hpp"
#include "other/roleactivity/impl/roleactivityexpelmonster.hpp"
#include "other/roleactivity/impl/roleactivityheritagetreasure.hpp"
#include "other/roleactivity/impl/roleactivitysevencolorpicture.hpp"
#include "other/roleactivity/impl/roleactivitysaleshop.hpp"
#include "other/roleactivity/impl/roleactivityhappyup.hpp"
#include "other/roleactivity/impl/roleactivityanimalrace.hpp"
#include "other/roleactivity/impl/roleactivityluckybag.hpp"
#include "other/roleactivity/impl/roleactivityreducetaxrate.hpp"
#include "other/roleactivity/impl/roleactivitytianjianghaoli.hpp"
#include "other/roleactivity/impl/roleactivitysealpromote.hpp"
#include "other/roleactivity/impl/roleactivitydropdouble.hpp"
#include "other/roleactivity/impl/roleactivityjoyseeksfavors.hpp"
#include "other/roleactivity/impl/roleactivitycollectword.hpp"
#include "other/roleactivity/impl/roleactivitydiscountcarnival.hpp"
#include "other/roleactivity/impl/roleactivitypartnerexchange.hpp"
#include "other/roleactivity/impl/roleactivityluckydiscount.hpp"
#include "other/roleactivity/impl/roleactivitypetexchange.hpp"
#include "other/roleactivity/impl/roleactivityjiandanqinxin.hpp"
#include "other/roleactivity/impl/roleactivityadventuregift.hpp"
#include "other/roleactivity/impl/roleactivitylinkagegift.hpp"
#include "other/roleactivity/impl/roleactivityluckyturntable.hpp"
#include "other/roleactivity/impl/roleactivitytianmingdivination.hpp"
#include "other/roleactivity/impl/roleactivityquanmindiscount.hpp"
#include "other/roleactivity/impl/roleactivitysports.hpp"
#include "other/roleactivity/impl/roleactivitytujiantreasure.hpp"
#include "other/roleactivity/impl/roleactivitygodtoken.hpp"
#include "other/roleactivity/impl/roleactivityelementcards.hpp"
#include "other/roleactivity/impl/roleactivitysignincontinuity.hpp"
#include "other/roleactivity/impl/roleactivitycaishenjubao.hpp"
#include "other/roleactivity/impl/roleactivitydingzhibaoxiang.hpp"
#include "other/roleactivity/impl/roleactivitydingzhibaoxiang2.hpp"
#include "other/roleactivity/impl/roleactivitydingzhibaoxiang3.hpp"
#include "other/roleactivity/impl/roleactivityzhuanshulibao.hpp"
#include "other/roleactivity/impl/roleactivityjingyanbiaosheng.hpp"
#include "other/roleactivity/impl/roleactivityxianshibaohe.hpp"
#include "other/roleactivity/impl/roleactivityxianshibaohe2.hpp"
#include "other/roleactivity/impl/roleactivityxianshibaohe3.hpp"
#include "other/roleactivity/impl/roleactivityxianshibaohe4.hpp"
#include "other/roleactivity/impl/roleactivityxianshibaodai.hpp"
#include "other/roleactivity/impl/roleactivityxianshibaodai2.hpp"
#include "other/roleactivity/impl/roleactivitygivepetdraw.hpp"
#include "other/roleactivity/impl/roleactivitycijiuyingxin.hpp"
#include "other/roleactivity/impl/roleactivityniuxingdayun.hpp"
#include "other/roleactivity/impl/roleactivityspringfestivalstore.hpp"
#include "other/roleactivity/impl/roleactivitymonpolygift.hpp"
#include "other/roleactivity/impl/roleactivitycowexchange.hpp"
#include "other/roleactivity/impl/roleactivitycowstore.hpp"
#include "other/roleactivity/impl/roleactivitycowluckyexchange.hpp"
#include "other/roleactivity/impl/roleactivitylittleyeargift.hpp"
#include "other/roleactivity/impl/roleactivitygivesecretkey.hpp"
#include "other/roleactivity/impl/roleactivitycumulativerecharge.hpp"
#include "other/roleactivity/impl/roleactivitypetloot.hpp"
#include "other/roleactivity/impl/roleactivitytrademarketreturngold.hpp"
#include "other/roleactivity/impl/roleactivityshenshoujianglin.hpp"
#include "other/roleactivity/impl/roleactivitygoodgiftturn.hpp"
#include "other/roleactivity/impl/roleactivityfortunemaze.hpp"
#include "other/roleactivity/impl/roleactivityhelpothers.hpp"
#include "other/roleactivity/impl/roleactivityhelpothers2.hpp"
#include "other/roleactivity/impl/roleactivitysecrettreasure.hpp"
#include "other/roleactivity/impl/roleactivitymijingqiwen.hpp"
#include "other/roleactivity/impl/roleactivitymijingqiwen2.hpp"
#include "other/roleactivity/impl/roleactivitychunridengfeng.hpp"
#include "other/roleactivity/impl/roleactivitylaodonghaoli.hpp"
#include "other/roleactivity/impl/roleactivityzuheheli.hpp"
#include "other/roleactivity/impl/roleactivitylingpaitianci.hpp"
#include "other/roleactivity/impl/roleactivityfashionshop.hpp"
#include "other/roleactivity/impl/roleactivityoneyuangou.hpp"
#include "other/roleactivity/impl/roleactivitylingyujijin.hpp"
#include "other/roleactivity/impl/roleactivitylingpaitianci2.hpp"
#include "other/roleactivity/impl/roleactivitypaimaihang.hpp"

#include "other/roleactivity/impl/roleactivitysummerlilian.hpp"
#include "other/roleactivity/impl/roleactivitycolorfullantern.hpp"
#include "other/roleactivity/impl/roleactivitysummerstore.hpp"
#include "other/roleactivity/impl/roleactivityluckysign.hpp"
#include "other/roleactivity/impl/roleactivityqionglouyuyan.hpp"
#include "other/roleactivity/impl/roleactivitysuperoffshop.hpp"
#include "other/roleactivity/impl/roleactivityyuanqiturning.hpp"
#include "other/roleactivity/impl/roleactivitymazesummer.hpp"
#include "other/roleactivity/impl/roleactivitycoolbreeze.hpp"
#include "other/roleactivity/impl/roleactivitylotusmibao.hpp"
#include "other/roleactivity/impl/roleactivitymibaofangdian.hpp"
#include "other/roleactivity/impl/roleactivityzhenpinshangpu.hpp"
#include "other/roleactivity/impl/roleactivitylingpaitianci3.hpp"
#include "other/roleactivity/impl/roleactivitybackreward.hpp"
#include "other/roleactivity/impl/roleactivityservercompetition.hpp"
#include "other/roleactivity/impl/roleactivityhappylook.hpp"
#include "other/roleactivity/impl/roleactivitysignhappy.hpp"
#include "other/roleactivity/impl/roleactivitywanlinggongying.hpp"
#include "other/roleactivity/impl/roleactivityhorcruxtiancheng.hpp"
#include "other/roleactivity/impl/roleactivityintegraldraw.hpp"
#include "other/roleactivity/impl/roleactivityshanhaiquestion.hpp"
#include "other/roleactivity/impl/roleactivityshanhaijubao.hpp"
#include "other/roleactivity/impl/roleactivityshanhaimet.hpp"
#include "other/roleactivity/impl/roleactivityshanhaimeet.hpp"
#include "other/roleactivity/impl/roleactivityfateentanglement.hpp"
#include "other/roleactivity/impl/roleactivityzhaocaijinbao.hpp"
#include "other/roleactivity/impl/roleactivityyunzeterritory.hpp"
#include "other/roleactivity/impl/roleactivityyunzebloomingrecord.hpp"
#include "other/roleactivity/impl/roleactivityzaizhanyunze.hpp"
#include "other/roleactivity/impl/roleactivityyunzeslotmachine.hpp"
#include "other/roleactivity/impl/roleactivitymozuzainali.hpp"
#include "other/roleactivity/impl/roleactivitychaozhifenxiang.hpp"
#include "other/roleactivity/impl/roleactivityyunzeersanshi.hpp"
#include "other/roleactivity/impl/roleactivityshanhaizhengfeng.hpp"
#include "other/roleactivity/impl/roleactivitycolorfuldouble.hpp"
#include "other/roleactivity/impl/roleactivityshanhaimibao.hpp"
#include "other/roleactivity/impl/roleactivityshanhaihuadeng.hpp"
#include "other/roleactivity/impl/roleactivityshanhaibaoxiang.hpp"
#include "other/roleactivity/impl/roleactivityshanhaibaohe.hpp"
#include "other/roleactivity/impl/roleactivityshanhaibaodai.hpp"
#include "other/roleactivity/impl/roleactivityjixinggaozhao.hpp"
#include "other/roleactivity/impl/roleactivityyunzewish.hpp"
#include "other/roleactivity/impl/roleactivityyunzehaoli.hpp"
#include "other/roleactivity/impl/roleactivityyunzecutepet.hpp"
#include "other/roleactivity/impl/roleactivityyunzecatchelves.hpp"
#include "other/roleactivity/impl/roleactivityxianshileichong.hpp"
#include "other/roleactivity/impl/roleactweekendgift.hpp"
#include "other/roleactivity/impl/roleactchaozhigift.hpp"
#include "other/roleactivity/impl/roleactivitytigerauspicioussnow.hpp"
#include "other/roleactivity/impl/roleactivitytigerfisheveryyear.hpp"
#include "other/roleactivity/impl/roleactivityyinhuwenshi.hpp"
#include "other/roleactivity/impl/roleactivitytigerfindspring.hpp"
#include "other/roleactivity/impl/roleactivitytigerteasenewyear.hpp"
#include "other/roleactivity/impl/roleactivityciniuyingxinhu.hpp"
#include "other/roleactivity/impl/roleactivitytigerluckymoney.hpp"
#include "other/roleactivity/impl/roleactivityhuxingdayun.hpp"
#include "other/roleactivity/impl/roleactivityyinhubaisui.hpp"
#include "other/roleactivity/impl/roleactivityjingjiyueka.hpp"
#include "other/roleactivity/impl/roleactivityronglianyouli.hpp"
#include "other/roleactivity/impl/roleactivityqingyuanshizhuang.hpp"
#include "other/roleactivity/impl/roleactivityqingyuanleichong.hpp"
#include "other/roleactivity/impl/roleactivitygodbeastadvent.hpp"
#include "other/roleactivity/impl/roleactivityyuanqispeed.hpp"
#include "other/roleactivity/impl/roleactivityyuanqihelp.hpp"
#include "other/roleactivity/impl/roleactivityqiyuankongmingdeng.hpp"
#include "other/roleactivity/impl/roleactivityxunmenghudie.hpp"
#include "other/roleactivity/impl/roleactivitybozhongyouli.hpp"
#include "other/roleactivity/impl/roleactivityyuanqifanpai.hpp"
#include "other/roleactivity/impl/roleactivitywanlingzhuli.hpp"
#include "other/roleactivity/impl/roleactivitybaozilaile.hpp"
#include "other/roleactivity/impl/roleactivityjinyitianxiang.hpp"
#include "other/roleactivity/impl/roleactivitymiaobishenghua.hpp"
#include "other/roleactivity/impl/roleactivityyuanqiyanhuo.hpp"
#include "other/roleactivity/impl/roleactivityyuanqijishibu.hpp"
#include "other/roleactivity/impl/roleactivitychengfengpolang.hpp"
#include "other/roleactivity/impl/roleactivityzongxiangmancheng.hpp"
#include "other/roleactivity/impl/roleactivitysaltysweetbattle.hpp"
#include "other/roleactivity/impl/roleactivityluckyblindbox.hpp"
#include "other/roleactivity/impl/roleactivityduanyangxiangnang.hpp"
#include "other/roleactivity/impl/roleactivityduanyangzixuan.hpp"
#include "other/roleactivity/impl/roleactivityduanyangfuli.hpp"
#include "other/roleactivity/impl/roleactivitycanglongcifu.hpp"
#include "other/roleactivity/impl/roleactivitylantangchumu.hpp"
#include "other/roleactivity/impl/roleactivitysanbaichou.hpp"
#include "other/roleactivity/impl/roleactivityjinlongjubao.hpp"
#include "other/roleactivity/impl/roleactivitychongjifanli.hpp"
#include "other/roleactivity/impl/roleactivityshenshouenze.hpp"
#include "other/roleactivity/impl/roleactivityxinfuqianghua.hpp"
#include "other/roleactivity/impl/roleactivityqixitehui.hpp"
#include "other/roleactivity/impl/roleactivitycharismatictanabata.hpp"
#include "other/roleactivity/impl/roleactivityxinfulingchong.hpp"
#include "other/roleactivity/impl/roleactivityhunqifanli.hpp"
#include "other/roleactivity/impl/roleactivityshencihuafu.hpp"
#include "other/roleactivity/impl/roleactivityzhuiyueshangdian.hpp"
#include "other/roleactivity/impl/roleactivitytianyiyaoshi.hpp"
#include "other/roleactivity/impl/roleactivityyuexidengmi.hpp"
#include "other/roleactivity/impl/roleactivityyuexileichong.hpp"
#include "other/roleactivity/impl/roleactivitydanbifanli.hpp"

#include "servercommon/string/gameworldstr.h"
#include "servercommon/errornum.h"
#include "servercommon/noticenum.h"
#include "gamelog.h"
#include "global/worldstatus/worldstatus.hpp"
#include "config/randactivityconfig/randactivitycompensationconfig.hpp"

RoleActivityManager::RoleActivityManager()
	: m_role_module_mgr(NULL)
{
	memset(m_role_rand_activity_map, 0, sizeof(m_role_rand_activity_map));


	// 随机活动 [如果该活动有玩家独立数据的就在这里添加]
	// 这里添加后需要在gamemempool.cpp 里做对应的添加
	// 加活动数据也需要修改 GetRandActivityRoleDataLengthByType 这个函数内的switch , 文件名 roleactivitydef.hpp

	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_TIME_LIMIT_STORE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityTimeLimitStore(RAND_ACTIVITY_TYPE_TIME_LIMIT_STORE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_TRUN_TABLE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityTrunTable(RAND_ACTIVITY_TYPE_TRUN_TABLE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_DISCOUNT_STORE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityDiscountStore(RAND_ACTIVITY_TYPE_DISCOUNT_STORE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityConsumeCarousel(RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_EXPEL_MONSTER - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityExpelMonster(RAND_ACTIVITY_TYPE_EXPEL_MONSTER);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_HERITAGE_TREASURE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityHeritageTreasure(RAND_ACTIVITY_TYPE_HERITAGE_TREASURE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivitySevenColorPicture(RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SALE_SHOP - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivitySaleShop(RAND_ACTIVITY_TYPE_SALE_SHOP);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_HAPPY_UP - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityHappyUp(RAND_ACTIVITY_TYPE_HAPPY_UP);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_ANIMAL_RACE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityAnimalRace(RAND_ACTIVITY_TYPE_ANIMAL_RACE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_LUCKY_BAG - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityLuckyBag(RAND_ACTIVITY_TYPE_LUCKY_BAG);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_REDUCE_TAX_RATE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityReduceTaxRate(RAND_ACTIVITY_TYPE_REDUCE_TAX_RATE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_TIAN_JIANG_HAO_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityTianJiangHaoLi(RAND_ACTIVITY_TYPE_TIAN_JIANG_HAO_LI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SEAL_PROMOTE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivitySealPromote(RAND_ACTIVITY_TYPE_SEAL_PROMOTE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_DROP_DOUBLE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityDropDouble(RAND_ACTIVITY_TYPE_DROP_DOUBLE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_JOYSEEKSFAVORS - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityJoySeeksFavors(RAND_ACTIVITY_TYPE_JOYSEEKSFAVORS);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_COLLECT_WORD - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityCollectWord(RAND_ACTIVITY_TYPE_COLLECT_WORD);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_DISCOUNT_CARNIVAL - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityDiscountCarnival(RAND_ACTIVITY_TYPE_DISCOUNT_CARNIVAL);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_PARTNER_EXCHANGE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityPartnerExchange(RAND_ACTIVITY_TYPE_PARTNER_EXCHANGE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_LUCKY_DISCOUNT - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityLuckyDiscount(RAND_ACTIVITY_TYPE_LUCKY_DISCOUNT);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_PET_EXCHANGE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityPetExchange(RAND_ACTIVITY_TYPE_PET_EXCHANGE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityJianDanQinXin(RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_ADVENTURE_GIFT - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityAdventureGift(RAND_ACTIVITY_TYPE_ADVENTURE_GIFT);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_LINKAGE_GIFT - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityLinkageGift(RAND_ACTIVITY_TYPE_LINKAGE_GIFT);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityLuckyTurnTable(RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityTianMingDivination(RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityQuanMinDiscount(RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SPORTS - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivitySports(RAND_ACTIVITY_TYPE_SPORTS);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityTuJianTreasure(RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_GOD_TOKEN - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityGodToken(RAND_ACTIVITY_TYPE_GOD_TOKEN);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_ELEMENT_CARDS - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityElementCards(RAND_ACTIVITY_TYPE_ELEMENT_CARDS);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SIGN_IN_CONTINUITY - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivitySignInContinuity(RAND_ACTIVITY_TYPE_SIGN_IN_CONTINUITY);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_CAI_SHEN_JU_BAO - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityCaiShenJuBao(RAND_ACTIVITY_TYPE_CAI_SHEN_JU_BAO);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityDingZhiBaoXiang(RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_ZHUAN_SHU_LI_BAO - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityZhuanShuLiBao(RAND_ACTIVITY_TYPE_ZHUAN_SHU_LI_BAO);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_JING_YAN_BIAO_SHENG - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityJingYanBiaoSheng(RAND_ACTIVITY_TYPE_JING_YAN_BIAO_SHENG);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityXianShiBaoHe(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityXianShiBaoDai(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_GIVE_PET_DRAW - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityGivePetDraw(RAND_ACTIVITY_TYPE_GIVE_PET_DRAW);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_CI_JIU_YING_XIN - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityCiJiuYingXin(RAND_ACTIVITY_TYPE_CI_JIU_YING_XIN);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_NIU_XING_DA_YUN - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityNiuXingDaYun(RAND_ACTIVITY_TYPE_NIU_XING_DA_YUN);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivitySpringFestivalStore(RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_MONPOLY_GIFT - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityMonpolyGift(RAND_ACTIVITY_TYPE_MONPOLY_GIFT);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_COW_EXCHANGE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityCowExchange(RAND_ACTIVITY_TYPE_COW_EXCHANGE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_COW_STORE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityCowStore(RAND_ACTIVITY_TYPE_COW_STORE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityCowLuckyExchange(RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_LITTLE_YEAR_GIFT - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityLittleYearGift(RAND_ACTIVITY_TYPE_LITTLE_YEAR_GIFT);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_GIVE_SECRET_KEY - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityGiveSecretKey(RAND_ACTIVITY_TYPE_GIVE_SECRET_KEY);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityCumulativeRecharge(RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_PET_LOOT - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityPetLoot(RAND_ACTIVITY_TYPE_PET_LOOT);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_TRADEMARKET_GOLD_RETURN - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityTradeMarketReturnGold(RAND_ACTIVITY_TYPE_TRADEMARKET_GOLD_RETURN);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityShenShouJiangLin(RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityGoodGiftTurn(RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivitySecretTreasure(RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_HELP_OTHERS- RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityHelpOthers(RAND_ACTIVITY_TYPE_HELP_OTHERS);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_FORTUNE_MAZE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityFortuneMaze(RAND_ACTIVITY_TYPE_FORTUNE_MAZE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_CHUN_RI_DENG_FENG - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityChunRiDengFeng(RAND_ACTIVITY_TYPE_CHUN_RI_DENG_FENG);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_ANECDOTE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityMiJingQiWen(RAND_ACTIVITY_TYPE_ANECDOTE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityLaoDongHaoLi(RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_ZU_HE_HE_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityZuHeHeLi(RAND_ACTIVITY_TYPE_ZU_HE_HE_LI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityLingPaiTianCi(RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_FASHION_SHOP - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityFashionShop(RAND_ACTIVITY_TYPE_FASHION_SHOP);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_ONE_YUAN_GOU - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityOneYuanGou(RAND_ACTIVITY_TYPE_ONE_YUAN_GOU);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_LING_YU_JI_JIN - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityLingYuJiJin(RAND_ACTIVITY_TYPE_LING_YU_JI_JIN);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI_2 - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityLingPaiTianCi2(RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI_2);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SUMMER_LI_LIAN - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivitySummerLiLian(RAND_ACTIVITY_TYPE_SUMMER_LI_LIAN);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_COLORFUL_LANTERN - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityColorfulLantern(RAND_ACTIVITY_TYPE_COLORFUL_LANTERN);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SUMMER_STORE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivitySummerStore(RAND_ACTIVITY_TYPE_SUMMER_STORE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_LUCKY_SIGN - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityLuckySign(RAND_ACTIVITY_TYPE_LUCKY_SIGN);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityQiongLouYuYan(RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivitySuperOffShop(RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_YUAN_QI_TURNING - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityYuanQiTurning(RAND_ACTIVITY_TYPE_YUAN_QI_TURNING);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_COOL_BREEZE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityCoolBreeze(RAND_ACTIVITY_TYPE_COOL_BREEZE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_LOTUS_MI_BAO - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityLotusMiBao(RAND_ACTIVITY_TYPE_LOTUS_MI_BAO);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_MAZE_SUMMER - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityMazeSummer(RAND_ACTIVITY_TYPE_MAZE_SUMMER);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityMiBaoFangDian(RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_ZHEN_PIN_SHANG_PU - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityZhenPinShangPu(RAND_ACTIVITY_TYPE_ZHEN_PIN_SHANG_PU);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_PAI_MAI_HANG - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityPaiMaiHang(RAND_ACTIVITY_TYPE_PAI_MAI_HANG);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI_3 - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityLingPaiTianCi3(RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI_3);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_BACK_REWARD - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityBackReward(RAND_ACTIVITY_TYPE_BACK_REWARD);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SERVER_COMPETITION - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityServerCompetition(RAND_ACTIVITY_TYPE_SERVER_COMPETITION);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_HAPPY_LOOK - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityHappyLook(RAND_ACTIVITY_TYPE_HAPPY_LOOK);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SIGN_HAPPY - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivitySignHappy(RAND_ACTIVITY_TYPE_SIGN_HAPPY);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityWanLingGongYing(RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_HELP_OTHERS_2 - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityHelpOthers2(RAND_ACTIVITY_TYPE_HELP_OTHERS_2);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_ANECDOTE_2 - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityMiJingQiWen2(RAND_ACTIVITY_TYPE_ANECDOTE_2);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityHorcruxTianCheng(RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_INTEGRAL_DRAW- RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityIntegralDraw(RAND_ACTIVITY_TYPE_INTEGRAL_DRAW);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SHANHAI_QUESTION - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityShanHaiQuestion(RAND_ACTIVITY_TYPE_SHANHAI_QUESTION);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SHAN_HAI_JU_BAO - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityShanHaiJuBao(RAND_ACTIVITY_TYPE_SHAN_HAI_JU_BAO);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SHAN_HAI_MET - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityShanHaiMet(RAND_ACTIVITY_TYPE_SHAN_HAI_MET);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SHAN_HAI_MEET - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityShanHaiMeet(RAND_ACTIVITY_TYPE_SHAN_HAI_MEET);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_FATE_ENTANGLEMENT - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityFateEntanglement(RAND_ACTIVITY_TYPE_FATE_ENTANGLEMENT);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityZhaoCaiJinBao(RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_YUN_ZE_TERRITORY - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityYunZeTerritory(RAND_ACTIVITY_TYPE_YUN_ZE_TERRITORY);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityYunZeBloomingRecord(RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityYunZeSlotMachine(RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_ZAI_ZHAN_YUN_ZE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityZaiZhanYunZe(RAND_ACTIVITY_TYPE_ZAI_ZHAN_YUN_ZE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_MO_ZU_ZAI_NA_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityMoZuZaiNaLi(RAND_ACTIVITY_TYPE_MO_ZU_ZAI_NA_LI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityChaoZhiFenXiang(RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_YUN_ZE_ER_SAN_SHI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityYunZeErSanShi(RAND_ACTIVITY_TYPE_YUN_ZE_ER_SAN_SHI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityShanHaiZhengFeng(RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_COLORFUL_DOUBLE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityColorfulDouble(RAND_ACTIVITY_TYPE_COLORFUL_DOUBLE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityShanHaiMiBao(RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SHANHAI_HUADENG - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityShanHaiHuaDeng(RAND_ACTIVITY_TYPE_SHANHAI_HUADENG);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_XIANG - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityShanHaiBaoXiang(RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_XIANG);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_HE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityShanHaiBaoHe(RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_HE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_DAI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityShanHaiBaoDai(RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_DAI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_2 - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityDingZhiBaoXiang2(RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_2);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_2 - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityXianShiBaoHe2(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_2);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI_2 - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityXianShiBaoDai2(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI_2);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_JIXING_GAOZHAO - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityJiXingGaoZhao(RAND_ACTIVITY_TYPE_JIXING_GAOZHAO);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_YUN_ZE_WISH - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityYunZeWish(RAND_ACTIVITY_TYPE_YUN_ZE_WISH);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityYunZeHaoLi(RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityYunZeCutePet(RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_YUN_ZE_CATCH_ELVES - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityYunZeCatchElves(RAND_ACTIVITY_TYPE_YUN_ZE_CATCH_ELVES);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG- RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityXianShiLeiChong(RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_WEEKEND_GIFT - RAND_ACTIVITY_TYPE_BEGIN] = new RoleACTWeekendGift(RAND_ACTIVITY_TYPE_WEEKEND_GIFT);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_CHAO_ZHI_GIFT - RAND_ACTIVITY_TYPE_BEGIN] = new RoleACTChaoZhiGift(RAND_ACTIVITY_TYPE_CHAO_ZHI_GIFT);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityTigerAuspiciousSnow(RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityTigerFishEveryYear(RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityTigerFindSpring(RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_TIGER_TEASE_NEWYEAR - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityTigerTeaseNewYear(RAND_ACTIVITY_TYPE_TIGER_TEASE_NEWYEAR);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityTigerLuckyMoney(RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_YINHU_WENSHI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityYinHuWenShi(RAND_ACTIVITY_TYPE_YINHU_WENSHI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_CINIU_YINGXINHU - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityCiNiuYingXinHu(RAND_ACTIVITY_TYPE_CINIU_YINGXINHU);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_HU_XING_DA_YUN - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityHuXingDaYun(RAND_ACTIVITY_TYPE_HU_XING_DA_YUN);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_YIN_HU_BAI_SUI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityYinHuBaiSui(RAND_ACTIVITY_TYPE_YIN_HU_BAI_SUI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_JING_JI_YUE_KA - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityJingJiYueKa(RAND_ACTIVITY_TYPE_JING_JI_YUE_KA);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_RONG_LIAN_YOU_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityRongLianYouLi(RAND_ACTIVITY_TYPE_RONG_LIAN_YOU_LI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_QING_YUAN_SHI_ZHUANG - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityQingYuanShiZhuang(RAND_ACTIVITY_TYPE_QING_YUAN_SHI_ZHUANG);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_QING_YUAN_LEI_CHONG - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityQingYuanLeiChong(RAND_ACTIVITY_TYPE_QING_YUAN_LEI_CHONG);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityGodBeastAdvent(RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_QIYUAN_KONGMINGDENG - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityQiYuanKongMingDeng(RAND_ACTIVITY_TYPE_QIYUAN_KONGMINGDENG);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_YUAN_QI_SPEED - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityYuanQiSpeed(RAND_ACTIVITY_TYPE_YUAN_QI_SPEED);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_YUAN_QI_HELP - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityYuanQiHelp(RAND_ACTIVITY_TYPE_YUAN_QI_HELP);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_XUNMENG_HUDIE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityXunMengHuDie(RAND_ACTIVITY_TYPE_XUNMENG_HUDIE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_BO_ZHONG_YOU_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityBoZhongYouLi(RAND_ACTIVITY_TYPE_BO_ZHONG_YOU_LI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_YUNA_QI_FAN_PAI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityYuanQiFanPai(RAND_ACTIVITY_TYPE_YUNA_QI_FAN_PAI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_WANLING_ZHULI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityWanLingZhuLi(RAND_ACTIVITY_TYPE_WANLING_ZHULI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_BAO_ZI_LAI_LE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityBaoZiLaiLe(RAND_ACTIVITY_TYPE_BAO_ZI_LAI_LE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_JIN_YI_TIAN_XIANG - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityJinYiTianXiang(RAND_ACTIVITY_TYPE_JIN_YI_TIAN_XIANG);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_MIAO_BI_SHENG_HUA - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityMiaoBiShengHua(RAND_ACTIVITY_TYPE_MIAO_BI_SHENG_HUA);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_YUAN_QI_YAN_HUO - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityYuanQiYanHuo(RAND_ACTIVITY_TYPE_YUAN_QI_YAN_HUO);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityYuanQiJiShiBu(RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_CHENG_FENG_PO_LANG - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityChengFengPoLang(RAND_ACTIVITY_TYPE_CHENG_FENG_PO_LANG);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_ZONG_XIANG_MAN_CHENG - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityZongXiangManCheng(RAND_ACTIVITY_TYPE_ZONG_XIANG_MAN_CHENG);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivitySaltySweetBattle(RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityDuanYangXiangNang(RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_DUAN_YANG_ZI_XUAN - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityDuanYangZiXuan(RAND_ACTIVITY_TYPE_DUAN_YANG_ZI_XUAN);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_CANG_LONG_CI_FU - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityCangLongCiFu(RAND_ACTIVITY_TYPE_CANG_LONG_CI_FU);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_LUCKY_BLIND_BOX - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityLuckyBlindBox(RAND_ACTIVITY_TYPE_LUCKY_BLIND_BOX);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityLanTangChuMu(RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_300_CHOU - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivitySanBaiChou(RAND_ACTIVITY_TYPE_300_CHOU);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_DUAN_YANG_FU_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityDuanYangFuLi(RAND_ACTIVITY_TYPE_DUAN_YANG_FU_LI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_JIN_LONG_JU_BAO - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityJinLongJuBao(RAND_ACTIVITY_TYPE_JIN_LONG_JU_BAO);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_CHONG_JI_FAN_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityChongJiFanLi(RAND_ACTIVITY_TYPE_CHONG_JI_FAN_LI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SHEN_SHOU_EN_ZE - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityShenShouEnZe(RAND_ACTIVITY_TYPE_SHEN_SHOU_EN_ZE);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_XIN_FU_QIANG_HUA - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityXinFuQiangHua(RAND_ACTIVITY_TYPE_XIN_FU_QIANG_HUA);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_QI_XI_TE_HUI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityQiXiTeHui(RAND_ACTIVITY_TYPE_QI_XI_TE_HUI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_CHARISMATIC_TANABATA - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityCharismaticTanabata(RAND_ACTIVITY_TYPE_CHARISMATIC_TANABATA);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_3 - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityXianShiBaoHe3(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_3);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_4 - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityXianShiBaoHe4(RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_4);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_3 - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityDingZhiBaoXiang3(RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_3);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_XIN_FU_LING_CHONG - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityXinFuLingChong(RAND_ACTIVITY_TYPE_XIN_FU_LING_CHONG);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_HUN_QI_FAN_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityHunQiFanLi(RAND_ACTIVITY_TYPE_HUN_QI_FAN_LI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_SHEN_CI_HUA_FU - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityShenCiHuaFu(RAND_ACTIVITY_TYPE_SHEN_CI_HUA_FU);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_TIAN_YI_YAO_SHI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityTianYiYaoShi(RAND_ACTIVITY_TYPE_TIAN_YI_YAO_SHI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_YUE_XI_DENG_MI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityYueXiDengMi(RAND_ACTIVITY_TYPE_YUE_XI_DENG_MI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_YUE_XI_LEI_CHONG - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityYueXiLeiChong(RAND_ACTIVITY_TYPE_YUE_XI_LEI_CHONG);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_DAN_BI_FAN_LI - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityDanBiFanLi(RAND_ACTIVITY_TYPE_DAN_BI_FAN_LI);
	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_ZHUI_YUE_SHANG_DIAN - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivityZhuiYueShangDian(RAND_ACTIVITY_TYPE_ZHUI_YUE_SHANG_DIAN);

	m_role_rand_activity_map[RAND_ACTIVITY_TYPE_ZENG_100_CHOU - RAND_ACTIVITY_TYPE_BEGIN] = new RoleActivity(RAND_ACTIVITY_TYPE_ZENG_100_CHOU);

	UNSTD_STATIC_CHECK(178 == RAND_ACTIVITY_TYPE_END - RAND_ACTIVITY_TYPE_BEGIN);
	
	// 这里添加后需要在gamemempool.cpp 里做对应的添加
	// 加活动数据也需要修改 GetRandActivityRoleDataLengthByType 这个函数内的switch , 文件名 roleactivitydef.hpp
}

RoleActivityManager::~RoleActivityManager()
{
	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; ++i)
	{
		if (NULL != m_role_rand_activity_map[i])
		{
			delete m_role_rand_activity_map[i];
			m_role_rand_activity_map[i] = NULL;
		}
	}
}

void RoleActivityManager::SetRole(RoleModuleManager *mgr)
{
	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; ++i)
	{
		if (NULL != m_role_rand_activity_map[i])
		{
			m_role_rand_activity_map[i]->SetRole(mgr);
		}
	}
}

void RoleActivityManager::Init(RoleModuleManager *mgr, const RoleActivityCommonParam &p, const RandActivityRoleParamList & data_list)
{
	m_role_module_mgr = mgr;
	m_common_param = p;

	for (int i = 0; i < data_list.count && i < MAX_RAND_ACTIVITY_ROLE_PARAM_COUNT; ++i)
	{
		int rand_type = data_list.data_list[i].activity_type - RAND_ACTIVITY_TYPE_BEGIN;
		if (rand_type >= 0 && rand_type < RAND_ACTIVITY_TYPE_MAX)
		{
			if (NULL != m_role_rand_activity_map[rand_type])
			{
			// 初始化角色活动数据 ,能从数据库中拿到数据 , 说明这个活动数据肯定是有效且非新创的 , 那么设置一下
				m_role_rand_activity_map[rand_type]->SetInvalid(false);
				m_role_rand_activity_map[rand_type]->SetNewCreate(false);

				m_role_rand_activity_map[rand_type]->Init(mgr, data_list.data_list[i].data);
		}
	}
	}
}

void RoleActivityManager::GetInitParam(RoleActivityCommonParam *p, RandActivityRoleParamList * data_list, bool is_logout)
{
	*p = m_common_param;

	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX&& data_list->count < MAX_RAND_ACTIVITY_ROLE_PARAM_COUNT; ++i)
	{
		if (NULL != m_role_rand_activity_map[i] && (m_role_rand_activity_map[i]->IsDataChange() || is_logout))
		{
			data_list->data_list[data_list->count].activity_type = i + RAND_ACTIVITY_TYPE_BEGIN;
			data_list->data_list[data_list->count].change_state = !m_role_rand_activity_map[i]->IsNewCreate() ? structcommon::CS_UPDATE : structcommon::CS_INSERT;
			m_role_rand_activity_map[i]->GetInitParam(&data_list->data_list[data_list->count].data);
			++data_list->count;
		}
	}
}

void RoleActivityManager::OnLogin()
{
	//屏蔽第二天和第三天奖励
	//unsigned char reward_first_flag = m_role_module_mgr->GetCommonData().reward_first_flag;
	//int first_yi_xian_er_index = FRIST_RECHARGE_TYPE_YIXIANER - 1;
	//第一天奖励领取没设置过,但却领取过首充,则为老号(老号直接发放天数奖励)
	/*if (!(m_common_param.first_recharge_day_flag[first_yi_xian_er_index] & (1 << 0)) && (reward_first_flag & (1 << first_yi_xian_er_index)))
	{
		const FristRechargeCfg * first_recharge_cfg = LOGIC_CONFIG->GetChongzhiConfig()->GetFristRechargeCfg(FRIST_RECHARGE_TYPE_YIXIANER);
		if (NULL != first_recharge_cfg)
		{
			for (int i = 1; i < MAX_FIRST_RECHARGE_DAY_NUM; i++)
			{
				int day_index = i - 1;
				static MailContentParam contentparam; contentparam.Reset();
				FillMailContentParam(contentparam, first_recharge_cfg->day_reward_list[day_index]);

				SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_first_recharge_subject);
				SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_first_recharge_content, i + 1);

				MailRoute::MailToUser(m_role_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
				
				m_common_param.first_recharge_day_flag[first_yi_xian_er_index] |= (1 << i);
				gamelog::g_log_first_recharge.printf(LL_INFO, "%s role[%d, %s] first_recharge_type[%d] day[%d]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), 
					FRIST_RECHARGE_TYPE_YIXIANER, i + 1);
			}
		}
		m_common_param.first_recharge_day_flag[first_yi_xian_er_index] |= (1 << 0);
		m_common_param.first_recharge_grow_end_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time() + SECOND_PER_DAY * LOGIC_CONFIG->GetChongzhiConfig()->GetOtherCfg().first_recharge_grow_day);
		this->SendFristChongzhiReward();
	}
	*/

	unsigned char reward_first_flag = m_role_module_mgr->GetCommonData().reward_first_flag;
	int first_yi_xian_er_index = FRIST_RECHARGE_TYPE_YIXIANER - 1;
	//老号领取过首充的，需要设置一次成长礼包持续时间
	if (0 == m_common_param.first_recharge_grow_end_timestamp && !(m_common_param.first_recharge_day_flag[first_yi_xian_er_index] & (1 << 0)) && (reward_first_flag & (1 << first_yi_xian_er_index)))
	{
		m_common_param.first_recharge_grow_end_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time() + SECOND_PER_DAY * LOGIC_CONFIG->GetChongzhiConfig()->GetOtherCfg().first_recharge_grow_day);
		this->SendFristChongzhiReward();
	}

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		gamelog::g_log_role_activity.printf(LL_INFO, "RoleActivityManager::OnLogin ramgr error  user[%s %d]  "
			" uniques_server_id[%d %d]",
			 m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetUID(),
			m_role_module_mgr->GetRole()->GetUniqueServerID().plat_type, m_role_module_mgr->GetRole()->GetUniqueServerID().server_id);

		return;
	}
	//检查月卡

	if (m_common_param.week_card_end_timestamp > 0 && m_common_param.month_card_level == 0)
	{
		m_common_param.month_card_level = m_role_module_mgr->GetRole()->GetLevel();
		this->OnSendMonthCardInfo();
	}

	if (m_common_param.month_card_end_timestamp > 0 && m_common_param.month_card_level == 0)
	{
		m_common_param.month_card_level = m_role_module_mgr->GetRole()->GetLevel();
		this->OnSendMonthCardInfo();
	}

	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; ++i)
	{
		if (NULL != m_role_rand_activity_map[i])
		{

			if (m_role_rand_activity_map[i]->IsInvalid() && !m_role_rand_activity_map[i]->IsNewCreate())
			{
				// 因为玩家建立角色的时候不知道当前有哪些活动,
				// 所以就该为在登录的时候判断一次
				m_role_rand_activity_map[i]->SetInvalid(false);
				m_role_rand_activity_map[i]->SetNewCreate(true);
				m_role_rand_activity_map[i]->SetRoleDataChange();
			}

			// 如果需要在检查重置数据前做处理 , 请自行添加接口
			m_role_rand_activity_map[i]->OnLoginCheck();

			// 	
			int rank_type = i + RAND_ACTIVITY_TYPE_BEGIN;
			if (!ramgr->IsRandActivityOpen(rank_type))
			{
				continue;
			}

			m_role_rand_activity_map[i]->CheckInitRandActivity(rank_type);
			m_role_rand_activity_map[i]->OnLogin();
			m_role_rand_activity_map[i]->OnLoginSendInfo();
		}
	}

	this->CheckRACompensation();
}

void RoleActivityManager::Update(time_t now_second)
{
	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; ++i)
	{
		if (NULL != m_role_rand_activity_map[i])
		{
			m_role_rand_activity_map[i]->Update(now_second);
		}
	}
}

void RoleActivityManager::OnResetModuleData(unsigned int old_dayid, unsigned int now_dayid)
{
	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; ++i)
	{
		if (NULL != m_role_rand_activity_map[i])
		{
			m_role_rand_activity_map[i]->OnResetModuleData(old_dayid, now_dayid);
		}
	}
}

void RoleActivityManager::OnDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		gamelog::g_log_role_activity.printf(LL_WARNING, "RoleActivityManager::OnDayChange ramgr NULL,  user[%s %d]  "
			" uniques_server_id[%d %d]",
			 m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetUID(),
			m_role_module_mgr->GetRole()->GetUniqueServerID().plat_type, m_role_module_mgr->GetRole()->GetUniqueServerID().server_id);
	}

	{
		//充值返利每日重置,保留重置时的等级
		m_common_param.ChongZhiFanLiDayReset();
		m_common_param.level = m_role_module_mgr->GetRole()->GetLevel();
		this->SendChongZhiInfo();
	}

	//月卡周卡
	this->OnMonthCardDayChange();

	//首充
	this->OnFirstChongzhiDayChange();

	// 月度投资每日刷新
	this->OnMonthInvestmentDayChange(old_dayid, now_dayid);

	// 世界BOSS参与次数
	m_common_param.world_boss_play_times = 0;

	//限时活动报名重置
	m_common_param.SignUpDataReset();
	this->SendActivitySignUpInfo();

	// 下面是有活动号的活动数据  上面是没有活动号的活动数据或者系统数据
	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; ++i)
	{
		if (NULL != m_role_rand_activity_map[i])
		{
			m_role_rand_activity_map[i]->OnDayChange(old_dayid, now_dayid);
		}
	}
}

void RoleActivityManager::OnWeekChange()
{
	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; ++i)
	{
		if (NULL != m_role_rand_activity_map[i])
		{
			m_role_rand_activity_map[i]->OnWeekChange();
		}
	}
	//月卡周卡
	this->OnMonthCardWeekChange();
}

void RoleActivityManager::OnMonthChange()
{
	if (m_common_param.team_fight_score >= 0)
	{
		// 小队竞技 每月初重置积分为 等级*10
		m_common_param.team_fight_score = m_role_module_mgr->GetRole()->GetLevel() * 10;
		m_common_param.team_fight_victory_count = 0;
		m_common_param.team_fight_defeat_count = 0;
		m_common_param.team_fight_box_fetch_flag = 0;
		ActivityTeamFight* activity = (ActivityTeamFight*)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_TEAM_FIGHT);
		if (NULL != activity)
		{
			activity->SyncRoleScoreToActivityData(m_role_module_mgr->GetRole());
		}
	}

	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; ++i)
	{
		if (NULL != m_role_rand_activity_map[i])
		{
			m_role_rand_activity_map[i]->OnMonthChange();
		}
	}
}

void RoleActivityManager::OnRoleLevelUp(int old_level, int to_level)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr)
	{
		return;
	}

	if (m_common_param.team_fight_score < 0)
	{
		int limit_level = LOGIC_CONFIG->GetActivityTeamFightConfig()->GetOtherCfg()->level;
		if (to_level >= limit_level && old_level < limit_level)
		{
			m_common_param.team_fight_score = to_level * 10;

			ActivityTeamFight* activity = (ActivityTeamFight*)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_TEAM_FIGHT);
			if (NULL != activity)
			{
				activity->SyncRoleScoreToActivityData(m_role_module_mgr->GetRole());
			}
		}
	}


	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; ++i)
	{
		if (NULL != m_role_rand_activity_map[i])
		{
			m_role_rand_activity_map[i]->OnRoleLevelUp(old_level, to_level);
		}
	}
}

void RoleActivityManager::OnAddChongZhi(long long chongzhi)
{
	if (chongzhi <= 0) return;
	
	m_common_param.history_chongzhi_num += chongzhi;
	this->OnNewBuyGiftAddChongZhi(chongzhi);

	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; ++i)
	{
		if (NULL != m_role_rand_activity_map[i])
		{
			m_role_rand_activity_map[i]->OnAddChongZhi(chongzhi);
		}
	}
}

void RoleActivityManager::OnNewBuyGiftAddChongZhi(long long chongzhi)
{
	if (chongzhi <= 0) return;

	m_common_param.chongzhi_num += chongzhi;
	this->SendChongZhiInfo();

	gamelog::g_log_role_activity.printf(LL_INFO, "RoleActivityManager::OnAddChongZhi user[%s %d]  "
		" addchongzhi:%lld   history_chongzhi_num:%lld  day_chongzhi_num:%lld",
		m_role_module_mgr->GetRole()->GetName(), m_role_module_mgr->GetRole()->GetUID(),
		chongzhi, m_common_param.history_chongzhi_num, m_common_param.chongzhi_num);
}

void RoleActivityManager::OnConsumeGold(long long consume_gold)
{
	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; ++i)
	{
		if (NULL != m_role_rand_activity_map[i])
		{
			m_role_rand_activity_map[i]->OnConsumeGold(consume_gold);
		}
	}
}

void RoleActivityManager::CheckInitRandActivity(int rand_act_type, bool is_force /*= false*/)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(rand_act_type))
	{
		return;
	}

	// 防止误启动活动，不在跨服启动的活动会导致重置
	if (CrossConfig::Instance().IsHiddenServer())
	{
		RandActivity *activity = ramgr->ForceGetRandActivity(rand_act_type);
		if (NULL != activity && !activity->CanSyncActivityDataToCross())
		{
			return;
		}
	}

	int ra_type = rand_act_type - RAND_ACTIVITY_TYPE_BEGIN;
	if (ra_type >= 0 && ra_type < RAND_ACTIVITY_TYPE_MAX)
	{
		if (NULL != m_role_rand_activity_map[ra_type])
		{
			m_role_rand_activity_map[ra_type]->CheckInitRandActivity(rand_act_type, is_force);
		}
	}
}

void RoleActivityManager::OnActivityEnterReq(Protocol::CSActivityEnterReq *aer)
{
	if (NULL == aer) return;

	bool is_goto_cross = false;
	int to_scene = 0;
	int to_scene_key = 0;
	Posi to_scene_pos(0, 0);

	int count = 0;
	bool is_pass = true;
	Role* role_list[MAX_TEAM_MEMBER_NUM] = { NULL };
	switch (aer->activity_type)
	{
	case ACTIVITY_TYPE_ONLY_FIGHT:
		{
			is_pass = this->OnActivityEnterOnlyFight(aer, to_scene_key, to_scene, to_scene_pos, &is_goto_cross);
		}
		break;

	case ACTIVITY_TYPE_TEAM_FIGHT:
		{
			is_pass = this->OnActivityEnterTeamFight(aer, to_scene_key, to_scene, to_scene_pos, &is_goto_cross);
		}
		break;

	case ACTIVITY_TYPE_GUILD_FIGHT:
		{
			is_pass = this->OnActivityEnterGuildFight(aer, to_scene_key, to_scene, to_scene_pos, &is_goto_cross);
		}
		break;

	case ACTIVITY_TYPE_GUILD_ANSWER:
		{
			is_pass = this->OnActivityEnterGuildAnswer(aer, to_scene_key, to_scene, to_scene_pos, &is_goto_cross);
		}
		break;
	case ACTIVITY_TYPE_WORLD_TEAM_ARENA:
		{
			is_pass = this->OnActivityEnterWorldTeamArena(aer, to_scene_key, to_scene, to_scene_pos, &is_goto_cross);
		}
		break;
	default:
		break;
	}

	if (is_goto_cross || !is_pass)
	{
		return;
	}

	if (0 == to_scene || 0 == to_scene_key)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	Role* role = m_role_module_mgr->GetRole();
	Team* team = role->GetMyTeam();
	if (NULL == team)
	{
		WorldStatus::Instance().LogFunctionStats(role, "RoleActivityManagerEnter");
		World::GetInstWorld()->GetSceneManager()->GoTo(role, to_scene, to_scene_key, to_scene_pos);
	}
	else
	{
		for (int member_idx = 0; member_idx < team->GetMemberCount(); member_idx++)
		{
			Role * member_role = team->GetMemberRoleByIndex(member_idx);
			if (NULL == member_role)continue;
			WorldStatus::Instance().LogFunctionStats(member_role, "RoleActivityManagerEnter");
		}
		World::GetInstWorld()->GetSceneManager()->TeamGoTo(team, to_scene, to_scene_key, to_scene_pos);
	}

	int ra_type = aer->activity_type - RAND_ACTIVITY_TYPE_BEGIN;
	if (ra_type >= 0 && ra_type < RAND_ACTIVITY_TYPE_MAX)
	{
		if (NULL != m_role_rand_activity_map[ra_type])
		{
			m_role_rand_activity_map[ra_type]->OnActivityEnterReq(aer);
		}
	}
}

void RoleActivityManager::OnActivityEnterReq2(int activity_type, int param1 /*= 0*/)
{
	Protocol::CSActivityEnterReq req;
	req.activity_type = activity_type;
	req.param1 = param1;

	this->OnActivityEnterReq(&req);
}

void RoleActivityManager::OnRandActivityOpReq(Protocol::CSRandActivityOperaReq* req)
{
	if (NULL == req) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(req->rand_activity_type))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}
	this->CheckInitRandActivity(req->rand_activity_type);

	int ra_type = req->rand_activity_type - RAND_ACTIVITY_TYPE_BEGIN;
	if (ra_type >= 0 && ra_type < RAND_ACTIVITY_TYPE_MAX)
	{
		if (NULL != m_role_rand_activity_map[ra_type])
		{
			const RandActivityConfig * config = static_cast<const RandActivityConfig *>
				(LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityConfig(req->rand_activity_type));
			if (config && config->GetConfigVer() && config->GetConfigVer() != req->cfg_ver)
			{
				Protocol::SCCmdBuyItemCheckRet protocol;
				protocol.unique_id = 0;
				protocol.cfg_ver = config->GetConfigVer();
				m_role_module_mgr->NetSend(&protocol, sizeof(protocol));
				
				m_role_module_mgr->NoticeNum(errornum::EN_NEED_UPDATE_CFG_VER);

				return;
			}

			m_role_rand_activity_map[ra_type]->OnRandActivityOpReq(req);
		}
	}
}

void RoleActivityManager::OnChongzhiOperateReq(Protocol::CSChongzhiOperateReq* req)
{
	if (NULL == req) return;

	switch (req->oper_type)
	{
	case Protocol::CSChongzhiOperateReq::OPER_TYPE_SEND_CHONGZHI_INFO:
		{
			this->SendChongZhiInfo();
		}
		break;

	case Protocol::CSChongzhiOperateReq::OPER_TYPE_FANLI_REWARD_FETCH:
		{
			this->FetchFanliReward(req->param1);
		}
		break;
	}
}

void RoleActivityManager::OnRandActivityBonusOpReq(Protocol::CSRandActivityBonusOperaReq * req)
{
	if(NULL == req) return;

	switch (req->opera_type)
	{
		case Protocol::RA_BONUS_OP_TYPE_INFO:
		{
			this->SendBonusInfo();
		}
		break;
		case Protocol::RA_BONUS_OP_TYPE_FETCH:
		{
			this->OnFetchBonus(req);
		}
		break;
	default:
		break;
	}
}

void RoleActivityManager::OnFetchBonus(Protocol::CSRandActivityBonusOperaReq * req)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(req->rand_activity_type))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}
	this->CheckInitRandActivity(req->rand_activity_type);
	const RABonusCfg * bonus_cfg = LOGIC_CONFIG->GetRandActivityBonusConfig()->GetBonusCfgBySeq(req->param_1);
	if (NULL == bonus_cfg || 0 == bonus_cfg->type_item || req->rand_activity_type != bonus_cfg->activity_id)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}
	int act_open_from_server_start = LOGIC_CONFIG->GetRandActivityConfigManager()->GetActRealOpenDay(ramgr, req->rand_activity_type);
	if (bonus_cfg->section_start > act_open_from_server_start || bonus_cfg->section_end < act_open_from_server_start)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return;
	}

	if (!m_common_param.bonus_flag.IsBitSet(req->param_1))
	{
		int ra_type = req->rand_activity_type - RAND_ACTIVITY_TYPE_BEGIN;
		bool is_finish = false;
		if (ra_type >= 0 && ra_type < RAND_ACTIVITY_TYPE_MAX)
		{
			if (NULL != m_role_rand_activity_map[ra_type])
			{
				if (m_role_rand_activity_map[ra_type]->IsFinishBonusCond(bonus_cfg->type))
				{
					is_finish = true;
				}
			}
		}
		if (!is_finish)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_NOT_ELIGIBLE);
			return;
		}
		bool is_put_finish = false;
		if (m_role_module_mgr->GetKnapsack()->CheckForPut(bonus_cfg->reward.item_id, bonus_cfg->reward.num))
		{
			if (m_role_module_mgr->GetKnapsack()->Put(bonus_cfg->reward, PUT_REASON_RA_BONUS))
			{
				is_put_finish = true;
			}
		}
		
		if (!is_put_finish)
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&bonus_cfg->reward, 1, SNED_MAIL_TYPE_DEFAULT, true);
		}
		gamelog::g_log_rand_activity.printf(LL_INFO, "%s user[%d, %s], level[%d], type[%d] item_list_num[%d]", __FUNCTION__,
			m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
			m_role_module_mgr->GetRole()->GetLevel(), req->rand_activity_type, 1);
		this->SetBonusFlag(req->param_1, true);
		this->SendBonusInfo();
	}
	else
	{
		m_role_module_mgr->NoticeNum(errornum::EN_BOSS_CHAPTER_REWARD_CLAIMED);
		return;
	}
}

void RoleActivityManager::OnCheckBonusConfig(int version_id)
{
	/*if(version_id == m_common_param.bonus_version_flag) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; ++i)
	{
		int rank_type = i + RAND_ACTIVITY_TYPE_BEGIN;
		if (!ramgr->IsRandActivityOpen(rank_type))
		{
			continue;
		}
		m_role_rand_activity_map[i]->OnCheckBonusConfig();
	}
	m_common_param.bonus_version_flag = version_id;
	this->SendBonusInfo();*/
}

void RoleActivityManager::SetBonusFlag(int seq, bool is_set)
{
	if (seq < 0 || seq >= MAX_BONUS_COUNT_NUM) return;
	
	if (is_set)
	{
		m_common_param.bonus_flag.SetBit(seq);
	}
	else
	{
		m_common_param.bonus_flag.UnSetBit(seq);
	}
}

void RoleActivityManager::OnRandActivityMakeUpReq(Protocol::CSRandActivityMakeUpReq * req)
{
	if(NULL == req || req->make_up_type < 0 || req->make_up_type >= RAND_ACTIVITY_MAKE_UP_TYPE_MAX) return;

	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	if (!ramgr->IsRandActivityOpen(req->rand_activity_type))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return;
	}
	this->CheckInitRandActivity(req->rand_activity_type);

	int ra_type = req->rand_activity_type - RAND_ACTIVITY_TYPE_BEGIN;
	if (ra_type >= 0 && ra_type < RAND_ACTIVITY_TYPE_MAX)
	{
		if (NULL != m_role_rand_activity_map[ra_type])
		{
			m_role_rand_activity_map[ra_type]->OnRandActivityMakeUpReq(req);
		}
	}
}

bool RoleActivityManager::JoinActivityInCross(int activity_type, int limit_level, bool is_limit_guild /*= false*/)
{
	if (is_limit_guild && 0 == m_role_module_mgr->GetRole()->GetGuildID())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GUILD_NOT_IN_GUILD);
		return false;
	}

	if (m_role_module_mgr->GetRole()->GetLevel() < limit_level)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
		return false;
	}

	Team* team = m_role_module_mgr->GetRole()->GetMyTeam();
	if (NULL != team)
	{
		Role * leader_role = team->GetLeaderRole();
		if (NULL != leader_role)
		{
			if (leader_role->GetUID() != m_role_module_mgr->GetRole()->GetUID())
			{
				m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
				return false;
			}
		}

		if (team->HasDismissMember())
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
			return false;
		}

		for (int i = 0; i < team->GetMemberCount(); i++)
		{
			Role* member_role = team->GetMemberRoleByIndex(i);
			if (NULL != member_role)
			{
				if (member_role->GetLevel() < limit_level)
				{
					m_role_module_mgr->NoticeNum(errornum::EN_TEAM_MEMBER_LEVEL_LIMIT);
					return false;
				}

				if (is_limit_guild && member_role->GetGuildID() != m_role_module_mgr->GetRole()->GetGuildID())
				{
					m_role_module_mgr->NoticeNum(errornum::EN_TEAM_MEMBER_NOT_GUILD_MEMBER);
					return false;
				}
			}
		}

		TeamManager::Instance().OnTeamGotoHidden(m_role_module_mgr->GetRole(), activity_type);
	}
	else
	{
		CrossData_Business business_data;
		business_data.auto_join_type = CROSS_AUTO_JION_TYPE_ACTIVITY;
		business_data.auto_join_type_param1 = activity_type;
		business_data.cross_business_type = CrossData_Business::BUSINESS_TYPE_ROLE_GO_TO_HIDDEN;
		m_role_module_mgr->GetRoleCross()->OnStartCrossReq(business_data);
	}

	return true;
}

void RoleActivityManager::OnSuccsGather(int gather_id, int seq_id, const Posi & gather_pos)
{
	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; ++i)
	{
		if (NULL != m_role_rand_activity_map[i])
		{
			m_role_rand_activity_map[i]->OnSuccsGather(gather_id, seq_id, gather_pos);
		}
	}
}

int RoleActivityManager::OnBuyGiftItemChongZhi(int chongzhi_num, int ARG_OUT * price)
{
	if (chongzhi_num <= 0 || chongzhi_num > 1000000)   // 安全检测，不可以负数，也不能太大（防止策划配错超大的数）
	{
		return 1;
	}

	if (NULL != price)
	{
		*price = chongzhi_num;
		return 0;
	}

	if (!m_role_module_mgr->GetMoney()->AddGold(chongzhi_num, __FUNCTION__))
	{
		return 1;
	}

	this->CheckChongzhiExtraRewardByNum(chongzhi_num);
	return 0;
}

void RoleActivityManager::CheckChongzhiExtraRewardByNum(int chongzhi_num)
{
	// 每种充值金额额外奖励
	UNSTD_STATIC_CHECK(CHONGZHI_REWARD_CONFIG_MAX_NUM <= (int)sizeof(m_common_param.chongzhi_reward_fetch_flag) * 8);

	int spid = m_role_module_mgr->GetRole()->GetClientShowSpid();
	if (0 == spid)
	{
		std::string plat_user_name = m_role_module_mgr->GetRole()->GetPlatName();
		spid = globalconfig::SpidConfig::GetSpidByStrPlatName(plat_user_name);
		m_role_module_mgr->GetRole()->SetClientShowSpid(spid);
	}
	//int double_spid = LOGIC_CONFIG->GetChongzhiCfg().GetDoubleSpid(spid);//这啥

	gamelog::g_log_chongzhi.printf(LL_INFO, "CheckChongzhiExtraRewardByNum start, user[%d,%s] chongzhi_num[%d] spid[%d] currency[%d] chong_zhi_flag[%u]", m_role_module_mgr->GetRole()->GetUID(), 
		m_role_module_mgr->GetRole()->GetName(), chongzhi_num, spid, m_role_module_mgr->GetCommonData().show_currency_type, m_common_param.chong_zhi_flag);

	const ChongzhiReward *reward_cfg = LOGIC_CONFIG->GetChongzhiConfig()->GetExtraRewardByNum(chongzhi_num, m_role_module_mgr->GetCommonData().show_currency_type, spid);
	if (NULL != reward_cfg)
	{
		int chongzhi_flag = (1 << reward_cfg->seq);
		if (!(m_common_param.chong_zhi_flag & (1 << reward_cfg->seq)))
		{
			m_common_param.chong_zhi_flag |= (1 << reward_cfg->seq);
			this->OnSendNewChongZhiReloadNotice();
		}
		this->CheckChongzhiExtraRewardByFlag(chongzhi_flag, spid);
	}
	else
	{
		// 		const ChongzhiReward *reward_cfg = LOGIC_CONFIG->GetChongzhiConfig().GetExtraRewardByNum(chongzhi_num, m_role_module_mgr->GetRole()->GetCurrencyType(), double_spid);
		// 		if (NULL != reward_cfg)
		// 		{
		// 			int chongzhi_flag = (1 << reward_cfg->seq);
		// 			this->CheckChongzhiExtraRewardByFlag(chongzhi_flag, double_spid);
		// 		}
		// 		else
		{
			gamelog::g_log_chongzhi.printf(LL_INFO, "CheckChongzhiExtraRewardByNum null reward_cfg, user[%d,%s] chongzhi_num[%d] spid[%d] currency[%d] chong_zhi_flag[%u]", m_role_module_mgr->GetRole()->GetUID(), 
				m_role_module_mgr->GetRole()->GetName(), chongzhi_num, spid, m_role_module_mgr->GetCommonData().show_currency_type, m_common_param.chong_zhi_flag);
		}
	}
}

void RoleActivityManager::CheckChongzhiExtraRewardByFlag(int chongzhi_flag, int spid)
{
	bool need_send = false;

	int count = LOGIC_CONFIG->GetChongzhiConfig()->GetExtraRewardCfgCount(m_role_module_mgr->GetRole()->GetCurrencyType(), spid);
	gamelog::g_log_chongzhi.printf(LL_INFO, "CheckChongzhiExtraRewardByFlag start, user[%d,%s] chongzhi_flag[%d] spid[%d] currency[%d] cfg_count[%d] chong_zhi_flag[%u]", m_role_module_mgr->GetRole()->GetUID(), 
		m_role_module_mgr->GetRole()->GetName(), chongzhi_flag, spid, m_role_module_mgr->GetCommonData().show_currency_type, count, m_common_param.chong_zhi_flag);

	for (int seq = 0; seq < count; ++seq)
	{
		if (0 == (chongzhi_flag & (1 << seq)))
		{
			continue;
		}

		const ChongzhiReward *reward_cfg = LOGIC_CONFIG->GetChongzhiConfig()->GetExtraRewardBySeq(seq, m_role_module_mgr->GetCommonData().show_currency_type, spid);
		if (NULL == reward_cfg)
		{
			continue;
		}

		gamelog::g_log_chongzhi.printf(LL_INFO, "CheckChongzhiExtraRewardByFlag get reward_cfg ok, user[%d,%s] chongzhi_flag[%d] spid[%d] currency[%d] seq[%d]", m_role_module_mgr->GetRole()->GetUID(), 
			m_role_module_mgr->GetRole()->GetName(), chongzhi_flag, spid, m_role_module_mgr->GetCommonData().show_currency_type, seq);

		//应策划要求,改为由每笔充值都反馈
		/*if (0 != (m_activity_info.chongzhi_reward_fetch_flag & (1 << seq)))
		{
		gamelog::g_log_chongzhi.printf(LL_INFO, "CheckChongzhiExtraRewardByFlag already get reward before, user[%d,%s] chongzhi_flag[%d] spid[%d] currency[%d] seq[%d]", m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), chongzhi_flag, spid, m_role_module_mgr->GetCommonData().show_currency_type, seq);
		continue;
		}*/

		bool can_get_chongzhi_extra_reward = true;
		{
			if (CHONGZHI_EXTRA_REWARD_TYPE_MONTH_CARD != reward_cfg->reward_type && CHONGZHI_EXTRA_REWARD_TYPE_NEW_MONTH_CARD != reward_cfg->reward_type) // 超过了制定开服天数，不再有额外奖励，月卡除外
			{
				time_t openserver_zero_timestamp = World::GetInstWorld()->GetServerOpenZeroTime(m_role_module_mgr->GetRole()->GetUniqueServerID());
				int day = GetDayIndex(openserver_zero_timestamp, EngineAdapter::Instance().Time());
				const ChongzhiOtherConfig &other_cfg = LOGIC_CONFIG->GetChongzhiConfig()->GetOtherCfg();

				if (day > other_cfg.extra_reward_effective_day)
				{
					can_get_chongzhi_extra_reward = false;
				}
			}
		}

		if (can_get_chongzhi_extra_reward)
		{
			if (CHONGZHI_EXTRA_REWARD_TYPE_COIN_BIND == reward_cfg->reward_type)
			{
				if (reward_cfg->reward_coin > 0)
				{
					// 发额外绑定铜币奖励
					static MailContentParam contentparam; contentparam.Reset();
					contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN] = reward_cfg->reward_coin;

					int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chongzhi_reward_subject);
					int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chongzhi_reward_coin_content, reward_cfg->chongzhi, reward_cfg->reward_coin);

					if (length1 > 0 && length2 > 0)
					{
						//m_activity_info.chongzhi_reward_fetch_flag |= (1 << reward_cfg->seq);

						MailRoute::MailToUser(m_role_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
					}
				}
			}
			else if (CHONGZHI_EXTRA_REWARD_TYPE_GOLD == reward_cfg->reward_type)
			{
				if (reward_cfg->reward_coin > 0)
				{
					// 发额外元宝奖励
					static MailContentParam contentparam; contentparam.Reset();
					contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] = reward_cfg->reward_coin;

					int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chongzhi_reward_subject);
					int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chongzhi_reward_content, reward_cfg->chongzhi, reward_cfg->reward_coin);

					if (length1 > 0 && length2 > 0)
					{
						//m_activity_info.chongzhi_reward_fetch_flag |= (1 << reward_cfg->seq);

						MailRoute::MailToUser(m_role_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
					}
				}
			}
			else if (CHONGZHI_EXTRA_REWARD_TYPE_GOLD_BIND == reward_cfg->reward_type)
			{
				if (reward_cfg->reward_coin > 0)
				{
					// 发额外绑定元宝奖励
					static MailContentParam contentparam; contentparam.Reset();
					contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_IMMORTAL_COIN] = reward_cfg->reward_coin;

					int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_chongzhi_reward_subject);
					int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_chongzhi_reward_content, reward_cfg->chongzhi, reward_cfg->reward_coin);

					if (length1 > 0 && length2 > 0)
					{
						//m_activity_info.chongzhi_reward_fetch_flag |= (1 << reward_cfg->seq);

						MailRoute::MailToUser(m_role_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
					}
				}
			}
			// G6暂时没有月卡
			// 			else if (CHONGZHI_EXTRA_REWARD_TYPE_MONTH_CARD == reward_cfg->reward_type)
			// 			{
			// 
			// 				m_activity_info.chongzhi_reward_fetch_flag |= (1 << reward_cfg->seq);
			// 				EventHandler::Instance().OnBuyMonthCard(m_role_module_mgr->GetRole());
			// 			}
			// 			else if (CHONGZHI_EXTRA_REWARD_TYPE_NEW_MONTH_CARD == reward_cfg->reward_type)
			// 			{
			// 				// 新月卡期限30天，原则上可以无限买，客户端控制60天内才可以购买， 所以不执行fetch_flag赋值
			// 				EventHandler::Instance().OnBuyNewMonthCard(m_role_module_mgr->GetRole());
			// 			}

			need_send = true;
			gamelog::g_log_chongzhi.printf(LL_INFO, "CheckChongzhiExtraRewardByFlag fetch reward ok, user[%d,%s] chongzhi_flag[%d] spid[%d] currency[%d] seq[%d] reward_flag[%d] chong_zhi_flag[%u]",
				m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), chongzhi_flag, spid, m_role_module_mgr->GetRole()->GetCurrencyType(), seq, 
				m_common_param.chongzhi_reward_fetch_flag, m_common_param.chong_zhi_flag);
		}
	}

	if (need_send)
	{
		this->SendChongZhiInfo();
	}
}

void RoleActivityManager::OnSendNewChongZhiReloadNotice()
{
	Protocol::SCNewChongzhiConfigReloadNotice info;
	
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityManager::OnRAOpenOnlineSend(int activity_type)
{
	int ra_type = activity_type - RAND_ACTIVITY_TYPE_BEGIN;
	if (ra_type >= 0 && ra_type < RAND_ACTIVITY_TYPE_MAX)
	{
		if (NULL != m_role_rand_activity_map[ra_type])
		{
			m_role_rand_activity_map[ra_type]->CheckInitRandActivity(activity_type);
			m_role_rand_activity_map[ra_type]->OnLoginSendInfo();
			m_role_rand_activity_map[ra_type]->OnRAOpenOnlineSend();
		}
	}
}

void RoleActivityManager::OnRAClose(int activity_type)
{
	int ra_type = activity_type - RAND_ACTIVITY_TYPE_BEGIN;
	if (ra_type >= 0 && ra_type < RAND_ACTIVITY_TYPE_MAX)
	{
		if (NULL != m_role_rand_activity_map[ra_type])
		{
			m_role_rand_activity_map[ra_type]->OnRAClose();
		}
	}
}

void RoleActivityManager::SendAllInfo()
{
	// 这里是那些没有活动号的
	this->SendRAMonthInvestmentInfo();
	this->SendFristChongzhiReward();
	this->CalcRoleCap();
	this->SendActivitySignUpInfo();
	this->OnSendMonthCardInfo();
}

void RoleActivityManager::ClearDirtyMark()
{
	for (int i = 0; i < RAND_ACTIVITY_TYPE_MAX; ++i)
	{
		if (NULL != m_role_rand_activity_map[i])
		{
			m_role_rand_activity_map[i]->ClearDirtyMark();
		}
	}
}

RoleActivity * RoleActivityManager::GetRoleActivity(int rand_activity_type)
{
	int ra_type = rand_activity_type - RAND_ACTIVITY_TYPE_BEGIN;
	if (ra_type >= 0 && ra_type < RAND_ACTIVITY_TYPE_MAX)
	{
		return m_role_rand_activity_map[ra_type];
	}

	return NULL;
}

void RoleActivityManager::OnGiveRoleActivityItem(int type, int boss_id, std::vector<ItemConfigData>* drop_list)
{
	RandActivityManager* ramgr = RandActivityAdapter::Instance()->GetRAManager(m_role_module_mgr->GetRole()->GetUniqueServerID());
	if (NULL == ramgr) return;

	std::vector<ItemConfigData> item_list;
	switch (type)
	{
	case ACTIVITY_GIVE_ITEM_TYPE_PATROL_MINING:
		{
			LOGIC_CONFIG->GetRandActivityConfigManager()->GetPatrolMiningCfg(ramgr, &item_list);		//单组
			LOGIC_CONFIG->GetRandActivityConfigManager()->GetPatrolMiningMoreCfg(ramgr, &item_list);	//多组 运营说会几率出现多组与单组都有奖励的情况,所以2个抽取逻辑都跑一遍
		}
		break;
	case ACTIVITY_GIVE_ITEM_TYPE_FIELD_BOSS:
		{
			LOGIC_CONFIG->GetRandActivityConfigManager()->GetFieldBossCfgById(ramgr, boss_id, &item_list);
			LOGIC_CONFIG->GetRandActivityConfigManager()->GetFieldCrystalFBMoreCfgById(ramgr, boss_id, &item_list);	//多组

		}
		break;
	case ACTIVITY_GIVE_ITEM_TYPE_CRYSTAL_BOSS:
		{
			LOGIC_CONFIG->GetRandActivityConfigManager()->GetCrystalBossCfgById(ramgr, boss_id, &item_list);
			LOGIC_CONFIG->GetRandActivityConfigManager()->GetFieldCrystalFBMoreCfgById(ramgr, boss_id, &item_list);	//多组

																												//水晶副本需要展示,所以不在本函数给予
			if (NULL == drop_list || item_list.empty()) return;

			drop_list->insert(drop_list->end(), item_list.begin(), item_list.end());
			return;
		}
		break;
	default:
		break;
	}

	if ((int)item_list.size() <= 0) return;

	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList((short)item_list.size(), &item_list[0]))
	{
		return;
	}
	if (!m_role_module_mgr->GetKnapsack()->PutList((short)item_list.size(), &item_list[0], PUT_REASON_RELATED_TO_THE_EVENT))
	{
		return;
	}
}

int RoleActivityManager::OnFetchScoreReward(int seq)
{
	if (seq < 0 || seq >= SCORE_REWARD_MAX_NUM) return -1;
	if (m_common_param.score_reward_flag.IsBitSet(seq))
	{
		return -2;
	}
	const ScoreReward * cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetScoreReward(seq);
	if (NULL == cfg) return -3;

	if (m_common_param.capability < cfg->score)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_NOT_ELIGIBLE);
		return -4;
	}
	if (!m_role_module_mgr->GetKnapsack()->CheckForPutList(cfg->reward_count, cfg->reward))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return -5;
	}
	if (!m_role_module_mgr->GetKnapsack()->PutList(cfg->reward_count, cfg->reward, PUT_REASON_DAY_REWARD))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return -6;
	}

	m_common_param.score_reward_flag.SetBit(seq);
	this->SendScoreRewardInfo();
	return 0;
}

void RoleActivityManager::CalcRoleCap()
{
	m_common_param.capability = m_role_module_mgr->GetCapability()->GetRoleCapability();
	this->SendScoreRewardInfo();
}

int RoleActivityManager::OnMonthInvestmentOp(int type, int seq)
{
	int ret = true;
	switch (type)
	{
	case 0:
		{
			ret = this->OnRAMonthInvestmentFetch(seq);		//签到领取奖励
		}
		break;
	case 1:
		{
			ret = this->OnRABuyMonthInvestment();			//购买月度投资
		}
		break;
	default:
		ret = false;
		break;
	}
	return ret;
}

void RoleActivityManager::OnMonthInvestmentDayChange(unsigned int old_dayid, unsigned int now_dayid)
{
	if (old_dayid == now_dayid) return;
	if (m_common_param.is_buy_month_investment == 0) return;

	m_common_param.ra_month_investment_buy_day = GetDayIndex(m_common_param.ra_month_buy_time_timestamp, EngineAdapter::Instance().Time()) + 1; //当前时间到购买月度投资的天数1~N
	if (m_common_param.ra_month_investment_buy_day <= 0)
	{
		m_common_param.ra_month_investment_buy_day = 1;
	}
	this->SendRAMonthInvestmentInfo();
}

void RoleActivityManager::OnCmdBuyMonthInvestmentCheck()
{
	if (m_common_param.is_buy_month_investment || m_common_param.ra_month_buy_time_timestamp != 0)
	{
		return;
	}
	const RandActivityOtherCfg & other_cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityOtherCfg();
	const std::vector<MonthPurchaseRewardCfg> * cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetMonthPurchaseReward();
	if (NULL == cfg)
	{
		return ;
	}
	if (other_cfg.monthly_top_up_ingot <= 0)
	{
		return;
	}
	RoleCommonSave * rcs = m_role_module_mgr->GetRoleCommonSaveManager()->GetRoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_MONTH_INVESTMENT);
	if (rcs == NULL)
	{
		return;
	}
	RoleCommonSaveCmdBuyItem * buy_item = rcs->GetTypeData<RoleCommonSaveCmdBuyItem>(0);
	if (buy_item == NULL)
	{
		return;
	}
	time_t now = EngineAdapter::Instance().Time();
	if (now < (time_t)buy_item->create_time + CMD_BUY_ITEM_WAIT_TIME)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_HAS_ORDER);

		return;
	}
	long long unique_key = 0;
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		return;
	}
	if (buy_item->create_time != 0)
	{
		std::string log_str1 = ::GetString(*buy_item);
		gamelog::g_log_month_investment.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str1.c_str());
	}
	std::vector<ItemConfigData> reward_list;
	for (int i = 0; i < (int)cfg->size(); i++)
	{
		reward_list.push_back((*cfg)[i].item);
	}
	
	buy_item->cfg_vesion = LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityOtherCfg().month_investment_cfg_ver;
	buy_item->need_gold = other_cfg.monthly_top_up_ingot;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = 0;
	buy_item->unique_id = unique_key;
	buy_item->param1 = 0;
	::ItemConfigDataVecToBuyItem(reward_list, buy_item->rewards_item, &buy_item->rewards_num);
	rcs->SetDataChange(0);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_month_investment.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	Protocol::SCCmdBuyItemCheckRet info;
	info.buy_type = BUY_TIME_TYPE_MONTH_PURCHASE_NEW;
	info.cfg_ver = buy_item->cfg_vesion;
	info.unique_id = unique_key;

	m_role_module_mgr->NetSend(&info, sizeof(info));
	return;
}

int RoleActivityManager::OnCmdBuyMonthInvestment(int chongzhi_gold, long long unique_id)
{
	RoleCommonSave * rcs = m_role_module_mgr->GetRoleCommonSaveManager()->GetRoleCommonSave(ROLE_COMMON_SAVE_DATA_TYPE_MONTH_INVESTMENT);
	if (rcs == NULL)
	{
		return -__LINE__;
	}

	const RoleCommonSaveCmdBuyItem * buy_item = rcs->GetTypeData<RoleCommonSaveCmdBuyItem>(0, false);
	if (buy_item == NULL)
	{
		return -__LINE__;
	}
	gamelog::g_log_month_investment.printf(LL_INFO, "%s line:%d | role[%d %s] chongzhi_gold[%d] unique_id[%lld] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
		chongzhi_gold, unique_id, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chongzhi_gold != buy_item->need_gold)
	{
		return  -__LINE__;
	}
	std::vector<ItemConfigData> rewards = ::BuyItemToItemConfigDataVec(buy_item->rewards_num, buy_item->rewards_item);
	if (rewards.empty())
	{
		return  -__LINE__;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)rewards.size(), &rewards[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_RA_MYSTERIOUS_TRIAL);
	}
	else
	{
		// 发邮件
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_knapsack_full_mail_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_knapsack_full_send_mail);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
	m_role_module_mgr->GetMoney()->AddGold(buy_item->need_gold, __FUNCTION__);
	m_common_param.ra_month_buy_time_timestamp = EngineAdapter::Instance().Time();
	m_common_param.is_buy_month_investment = 1;
	m_common_param.ra_month_investment_buy_day = 1;
	m_common_param.ra_month_investment_last_num = 10;
	m_common_param.month_investment_fetch_flag.Reset();

	gamelog::g_log_month_investment.printf(LL_INFO, "role[%d,%s] item_id[%d] num[%d] succ reason[%s]", m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		rewards[0].item_id, rewards[0].num, __FUNCTION__);

	rcs->RemoveData(0);
	m_role_module_mgr->GetRole()->Save();
	return 0;
}

void RoleActivityManager::OnActivitySignUpReq(int seq)
{
	const ActivitySignUpCfg* cfg = LOGIC_CONFIG->GetActivityConfig()->GetSignUpCfg(seq);
	if (NULL == cfg) return;

	if (m_common_param.sign_up_flag.IsBitSet(seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_SIGN_UP_FAILD);
		return;
	}
	m_role_module_mgr->NoticeNum(noticenum::NT_ACTIVITY_SIGN_UP_SUCC);
	m_common_param.sign_up_flag.SetBit(seq);
	this->SendActivitySignUpInfo();
}

void RoleActivityManager::OnPutOutSignUpReward(int activity_id)
{
	const ActivitySignUpCfg* cfg = LOGIC_CONFIG->GetActivityConfig()->GetSignUpCfgById(activity_id);
	if (NULL == cfg) return;

	//1.判断是否开启报名奖励 2.判断玩家有没有报名 3.判断玩家是否已经领取报名奖励
	if (cfg->open == 0) return;
	if (!m_common_param.sign_up_flag.IsBitSet(cfg->seq)) return;
	if (m_common_param.sign_up_reward_flag.IsBitSet(cfg->seq)) return;

	m_role_module_mgr->GetKnapsack()->SendMail(&cfg->item_list[0], (int)cfg->item_list.size(), SEND_MAIL_TYPE_SIGN_UP, false, activity_id);
	m_common_param.sign_up_reward_flag.SetBit(cfg->seq);

	gamelog::g_log_activity.printf(LL_INFO, "RoleActivity::OnPutOutSignUpReward by mail, role[%d,%s] activity_id[%d]",
		m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), activity_id);

}

void RoleActivityManager::GetFirstChongzhiReward(int first_recharge_type)
{
	int first_recharge_type_index  = first_recharge_type - 1;
	const FristRechargeCfg *cfg = LOGIC_CONFIG->GetChongzhiConfig()->GetFristRechargeCfg(first_recharge_type);
	if (NULL == cfg || m_common_param.history_chongzhi_num < cfg->need_gold || m_role_module_mgr->GetCommonData().reward_first_flag & (1 << first_recharge_type_index))
	{
		return;
	}
	bool is_put = false;
	if (m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->reward.item_id, cfg->reward.num, false))
	{
		if (m_role_module_mgr->GetKnapsack()->Put(cfg->reward, PUT_REASON_RA_FIRST_CHONGZHI_YIXIANER))
		{
			is_put = true;
		}
	}

	if (!is_put)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	m_role_module_mgr->GetCommonData().reward_first_flag |= (1 << first_recharge_type_index);
	m_common_param.first_recharge_day_flag[first_recharge_type_index] |= (1 << 0);	//领取时默认设置第一天
	m_common_param.first_recharge_grow_end_timestamp = static_cast<unsigned int>(EngineAdapter::Instance().Time() + SECOND_PER_DAY * LOGIC_CONFIG->GetChongzhiConfig()->GetOtherCfg().first_recharge_grow_day);
	this->SendFristChongzhiReward();
	gamelog::g_log_first_recharge.printf(LL_INFO, "%s role[%d, %s] first_recharge_type[%d] end_timestamp[%u]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), 
		first_recharge_type, m_common_param.first_recharge_grow_end_timestamp);
}

void RoleActivityManager::OnFetchExclusiveReward(int type)
{
	int first_recharge_type_index = FRIST_RECHARGE_TYPE_YIXIANER - 1;
	if (!(m_role_module_mgr->GetCommonData().reward_first_flag & (1 << first_recharge_type_index)))
	{
		return;
	}

	const FirstRechargeExclusiveCfg * exclusive_cfg = LOGIC_CONFIG->GetChongzhiConfig()->GetFirstRechargeExclusiveCfg(type);
	if(NULL == exclusive_cfg) return;

	int cond_param = 0;
	switch (exclusive_cfg->activity_type)
	{
		case FIRST_RECHARGE_EXCLUSIVE_TYPE_ZHAO_CAI_XIAN_ZI:
		{
			cond_param = m_common_param.first_recharge_zhao_cai_progress;
		}
		break;
		case FIRST_RECHARGE_EXCLUSIVE_TYPE_CHENG_ZHANG_ZHU_LI:
		{
			if (!(m_common_param.first_cmd_buy_flag & (1 << FIRST_RECHARGE_CMD_TYPE_GROW)))
			{
				return;
			}
			cond_param = m_role_module_mgr->GetRole()->GetLevel();
		}
		break;
	default:
		return;
	}

	if (m_common_param.first_recharge_exclusive_flag.IsBitSet(type))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_REWARD_ALREADY_FETCHED);
		return;
	}

	if (exclusive_cfg->param1 > cond_param)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_NOT_ELIGIBLE);
		return;
	}
	if (!exclusive_cfg->reward_list.empty())
	{
		bool is_put_succ = false;
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)exclusive_cfg->reward_list.size(), &exclusive_cfg->reward_list[0]))
		{
			if (m_role_module_mgr->GetKnapsack()->PutList((short)exclusive_cfg->reward_list.size(), &exclusive_cfg->reward_list[0], PUT_REASON_FIRST_RECHARGE_EXCLUSIVE))
			{
				is_put_succ = true;
			}
		}
		if (!is_put_succ)
		{
			m_role_module_mgr->GetKnapsack()->SendMail(&exclusive_cfg->reward_list[0], (int)exclusive_cfg->reward_list.size(), SEND_MAIL_TYPE_FIRST_RECHARGE_EXCLUSIVE, true);
		}
	}
	m_common_param.first_recharge_exclusive_flag.SetBit(type);
	this->SendFristChongzhiReward();
	gamelog::g_log_first_recharge.printf(LL_INFO, "%s role[%d, %s] type[%d] first_recharge_exclusive_type[%d]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), 
		type, exclusive_cfg->activity_type);
}

void RoleActivityManager::OnParticipateLimitActivity(int activity_type)
{
	int first_recharge_type_index = FRIST_RECHARGE_TYPE_YIXIANER - 1;
	if (!(m_role_module_mgr->GetCommonData().reward_first_flag & (1 << first_recharge_type_index)))
	{
		return;
	}
	//每天积分限制增加一次
	if (0 != m_common_param.is_add_first_recharge_zhao_cai_day_flag)
	{
		return;
	}
	if (!LOGIC_CONFIG->GetChongzhiConfig()->IsFirstRechargeActivity(activity_type))
	{
		return;
	}

	m_common_param.first_recharge_zhao_cai_progress += 1;
	m_common_param.is_add_first_recharge_zhao_cai_day_flag = 1;
	this->SendFristChongzhiReward();
	gamelog::g_log_first_recharge.printf(LL_INFO, "%s role[%d, %s] activity_type[%d] zhao_cai_progress[%d]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), 
		activity_type, m_common_param.first_recharge_zhao_cai_progress);
}

void RoleActivityManager::OnFirstBuyItemCheck(int first_recharge_cmd_type, int cfg_ver)
{	
	const ChongzhiOtherConfig & other_cfg = LOGIC_CONFIG->GetChongzhiConfig()->GetOtherCfg();
	if (0 != other_cfg.cfg_ver && cfg_ver != other_cfg.cfg_ver)
	{
		Protocol::SCCmdBuyItemCheckRet protocol;
		protocol.unique_id = 0;
		protocol.cfg_ver = other_cfg.cfg_ver;
		m_role_module_mgr->NetSend(&protocol, sizeof(protocol));

		m_role_module_mgr->NoticeNum(errornum::EN_NEED_UPDATE_CFG_VER);
		return;
	}
	if (first_recharge_cmd_type < 0 || first_recharge_cmd_type >= FIRST_RECHARGE_CMD_TYPE_MAX)
	{
		return;
	}
	if (m_common_param.first_cmd_buy_flag & (1 << first_recharge_cmd_type))
	{
		return;
	}
	if (other_cfg.buy_money <= 0)
	{
		return;
	}
	if (other_cfg.buy_reward_list.empty())
	{
		return;
	}

	long long unique_key = 0;
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		return;
	}

	time_t now = EngineAdapter::Instance().Time();
	UNSTD_STATIC_CHECK(FIRST_RECHARGE_CMD_TYPE_MAX == 1);
	//默认使用通用结构存储,新增首充直购类型,需再新加common_save类型
	RoleCommonSaveCmdBuyItem * buy_item = NULL;
	int common_save_type = ROLE_COMMON_SAVE_DATA_TYPE_INVALID;
	int buy_type = 0;
	switch (first_recharge_cmd_type)
	{
		case FIRST_RECHARGE_CMD_TYPE_GROW:
		{
			//首充专属-成长助力已超过时限
			if ((unsigned int)now > m_common_param.first_recharge_grow_end_timestamp)
			{
				return;
			}
			buy_type = BUY_TIME_TYPE_FIRST_RECHARGE_EXCLUSIVE;
			common_save_type = ROLE_COMMON_SAVE_DATA_TYPE_FIRST_RECHARGE_EXCLUSIVE;
			buy_item = CmdBuyRecordManager::Instance().GetNewCmdBuyRecord<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), ROLE_COMMON_SAVE_DATA_TYPE_FIRST_RECHARGE_EXCLUSIVE);
		}
		break;
		default:
		break;
	}
	if (NULL == buy_item || ROLE_COMMON_SAVE_DATA_TYPE_INVALID == common_save_type || 0 == buy_type)
	{
		return;
	}


	buy_item->cfg_vesion = other_cfg.cfg_ver;
	buy_item->need_gold = other_cfg.buy_money;
	buy_item->create_time = (unsigned int)now;
	buy_item->ra_begin_timestamp = 0;
	buy_item->unique_id = unique_key;
	buy_item->param1 = first_recharge_cmd_type;
	::ItemConfigDataVecToBuyItem(other_cfg.buy_reward_list, buy_item->rewards_item, &buy_item->rewards_num);
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), common_save_type, gamelog::g_log_first_recharge, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_first_recharge.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	Protocol::SCCmdBuyItemCheckRet protocol;
	protocol.buy_type = buy_type;
	protocol.unique_id = unique_key;
	protocol.cfg_ver = other_cfg.cfg_ver;
	m_role_module_mgr->NetSend(&protocol, sizeof(protocol));

	return;
}

int RoleActivityManager::OnFirstCmdBuyItem(int chong_zhi_gold, long long unique_id, int first_recharge_cmd_type)
{
	UNSTD_STATIC_CHECK(FIRST_RECHARGE_CMD_TYPE_MAX == 1);
	//默认使用通用结构存储,新增首充直购类型,需再新加common_save类型
	RoleCommonSaveCmdBuyItem * buy_item = NULL;
	int common_save_type = ROLE_COMMON_SAVE_DATA_TYPE_INVALID;
	switch (first_recharge_cmd_type)
	{
		case FIRST_RECHARGE_CMD_TYPE_GROW:
		{
			common_save_type = ROLE_COMMON_SAVE_DATA_TYPE_FIRST_RECHARGE_EXCLUSIVE;
			buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), ROLE_COMMON_SAVE_DATA_TYPE_FIRST_RECHARGE_EXCLUSIVE, unique_id);
		}
		break;
		default:
		break;
	}
	if (NULL == buy_item || ROLE_COMMON_SAVE_DATA_TYPE_INVALID == common_save_type)
	{
		return -__LINE__;
	}
	gamelog::g_log_first_recharge.printf(LL_INFO, "%s line:%d | role[%d %s] first_cmd_type[%d] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		first_recharge_cmd_type, ::GetString(*buy_item).c_str());

	if (buy_item->unique_id != unique_id)
	{
		return -__LINE__;
	}

	if (chong_zhi_gold != buy_item->need_gold)
	{
		return  -__LINE__;
	}

	if (buy_item->param1 != first_recharge_cmd_type)
	{
		return -__LINE__;
	}

	std::vector<ItemConfigData> rewards = ::BuyItemToItemConfigDataVec(buy_item->rewards_num, buy_item->rewards_item);
	if (rewards.empty())
	{
		return -__LINE__;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)rewards.size(), &rewards[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_FIRST_RECHARGE_EXCLUSIVE);
	}
	else
	{	
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, rewards);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_first_recharge_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_first_recharge_exclusive);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}
	m_common_param.first_cmd_buy_flag |= (1 << first_recharge_cmd_type);
	this->SendFristChongzhiReward();
	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), common_save_type, unique_id, gamelog::g_log_first_recharge, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	return 0;

}

void RoleActivityManager::OnFirstChongzhiDayChange()
{
	//屏蔽第二天第三天奖励
/*	unsigned char reward_first_flag = m_role_module_mgr->GetCommonData().reward_first_flag;
	for (int i = FRIST_RECHARGE_TYPE_YIXIANER; i < FRIST_RECHARGE_TYPE_MAX; ++i)
	{
		int first_recharge_index = i - 1;
		if(!(reward_first_flag & (1 << first_recharge_index))) continue;

		const FristRechargeCfg * first_recharge_cfg = LOGIC_CONFIG->GetChongzhiConfig()->GetFristRechargeCfg(i);
		if(NULL == first_recharge_cfg) continue;

		for (int day = 1; day < MAX_FIRST_RECHARGE_DAY_NUM; day++)
		{
			int day_index = day - 1;
			if (m_common_param.first_recharge_day_flag[first_recharge_index] & (1 << day)) continue;

			static MailContentParam contentparam; contentparam.Reset();
			FillMailContentParam(contentparam, first_recharge_cfg->day_reward_list[day_index]);

			SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_first_recharge_subject);
			SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_first_recharge_content, day + 1);

			MailRoute::MailToUser(m_role_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);

			m_common_param.first_recharge_day_flag[first_recharge_index] |= (1 << day);
			gamelog::g_log_first_recharge.printf(LL_INFO, "%s role[%d, %s] first_recharge_type[%d] day[%d]", __FUNCTION__, m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(),
				i, day + 1);
			break;
		}
	}*/
	m_common_param.is_add_first_recharge_zhao_cai_day_flag = 0;
}

void RoleActivityManager::SendFristChongzhiReward()
{
	Protocol::SCRAFirstChongZhiInfo msg;
	msg.reward_first_flag = m_role_module_mgr->GetCommonData().reward_first_flag;
	msg.first_cmd_buy_flag = m_common_param.first_cmd_buy_flag;
	msg.first_recharge_zhao_cai_progress = m_common_param.first_recharge_zhao_cai_progress;
	msg.first_recharge_grow_end_timestamp = m_common_param.first_recharge_grow_end_timestamp;
	msg.first_recharge_exclusive_flag = m_common_param.first_recharge_exclusive_flag;
	msg.guild_create_consume_coin_bind = m_role_module_mgr->GetCommonData().guild_create_consume_coin_bind;

	m_role_module_mgr->NetSend(&msg, sizeof(msg));
}

int RoleActivityManager::OnFirstCmdBuyGiftItem(int first_recharge_cmd_type, ARG_OUT int * price)
{
	if (first_recharge_cmd_type < 0 || first_recharge_cmd_type >= FIRST_RECHARGE_CMD_TYPE_MAX)
	{
		return 1;
	}
	if (m_common_param.first_cmd_buy_flag & (1 << first_recharge_cmd_type))
	{
		return 2;
	}
	const ChongzhiOtherConfig & other_cfg = LOGIC_CONFIG->GetChongzhiConfig()->GetOtherCfg();
	if (other_cfg.buy_money <= 0)
	{
		return 1;
	}
	if (other_cfg.buy_reward_list.empty())
	{
		return 1;
	}

	if(NULL != price)
	{
		*price = other_cfg.buy_money;	
		return 0;
	}

	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)other_cfg.buy_reward_list.size(), &other_cfg.buy_reward_list[0]))
	{
		m_role_module_mgr->GetKnapsack()->PutList((short)other_cfg.buy_reward_list.size(), &other_cfg.buy_reward_list[0], PUT_REASON_FIRST_RECHARGE_EXCLUSIVE);
	}
	else
	{
		static MailContentParam contentparam; contentparam.Reset();
		FillMailContentParam(contentparam, other_cfg.buy_reward_list);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_first_recharge_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_first_recharge_exclusive);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	m_common_param.first_cmd_buy_flag |= (1 << first_recharge_cmd_type);
	this->SendFristChongzhiReward();

	return 0;
}

int RoleActivityManager::OnMonthBuyGiftItem(int param, int type, ARG_OUT int * price)
{
	unsigned int now_second = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	if (0 == type)
	{
		int card_type = param;
		const MonthCardCfg * card_cfg = LOGIC_CONFIG->GetMonthCardConfig()->GetCardCfg(m_role_module_mgr->GetRole()->GetLevel(), card_type);
		if (NULL == card_cfg)
		{
			return 1;
		}
		if (NULL != price)
		{
			*price = card_cfg->buy_money;
			return 0;
		}

		switch (card_type)
		{
			case MONTH_CARD_TYPE_WEEK:
			{
				if (m_common_param.week_card_end_timestamp > now_second)
				{
					m_common_param.week_card_end_timestamp += card_cfg->card_day * SECOND_PER_DAY;
				}
				else
				{
					m_common_param.week_card_end_timestamp = (unsigned int)(now_second + card_cfg->card_day * SECOND_PER_DAY);
					//新开
					m_common_param.month_card_level = m_role_module_mgr->GetRole()->GetLevel();
				}
				gamelog::g_log_month_week_card.printf(LL_INFO, "%s role[%d %s] card_type[%d] end_timestamp[%u]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
					card_type, m_common_param.week_card_end_timestamp);
			}
			break;
			case MONTH_CARD_TYPE_MONTH:
			{
				if (m_common_param.month_card_end_timestamp > now_second)
				{
					m_common_param.month_card_end_timestamp += card_cfg->card_day * SECOND_PER_DAY;
				}
				else
				{
					m_common_param.month_card_end_timestamp = (unsigned int)(now_second + card_cfg->card_day * SECOND_PER_DAY);
					//新开
					m_common_param.month_card_level = m_role_module_mgr->GetRole()->GetLevel();
				}
				gamelog::g_log_month_week_card.printf(LL_INFO, "%s role[%d %s] card_type[%d] end_timestamp[%u]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
					card_type, m_common_param.month_card_end_timestamp);
			}
			break;
			default:
			return 1;
		}
		if (card_cfg->buy_reward_gold > 0)
		{
			m_role_module_mgr->GetMoney()->AddGold(card_cfg->buy_reward_gold, __FUNCTION__);
		}
		if (card_cfg->buy_reward_immortal > 0)
		{
			m_role_module_mgr->GetMoney()->AddOtherCoin(card_cfg->buy_reward_immortal, MONEY_TYPE_SILVER_COIN, __FUNCTION__);
		}
		if (NULL != ITEMPOOL->GetItem(card_cfg->buy_reward.item_id))
		{
			bool is_need_send_mail = true;
			if (m_role_module_mgr->GetKnapsack()->CheckForPut(card_cfg->buy_reward.item_id, card_cfg->buy_reward.num, false))
			{
				if (m_role_module_mgr->GetKnapsack()->Put(card_cfg->buy_reward, PUT_REASON_MONTH_CARD_REWARD))
				{
					is_need_send_mail = false;
				}
			}
			if (is_need_send_mail)
			{
				m_role_module_mgr->GetKnapsack()->SendMail(&card_cfg->buy_reward, 1, SEND_MAIL_TYPE_MONTH_CARD_BUY_CARD_OR_DAY_REWARD, false, card_type);
			}
		}
	}
	else if (1 == type)
	{
		int seq = param;
		if (now_second > m_common_param.week_card_end_timestamp && now_second > m_common_param.month_card_end_timestamp)
		{
			return 1;
		}
		const MonthShopCfg * shop_item_cfg = LOGIC_CONFIG->GetMonthCardConfig()->GetShopCfg(m_common_param.month_card_level, seq);
		if (NULL == shop_item_cfg)
		{
			return 1;
		}

		switch (shop_item_cfg->limit_type)
		{
			case MONTH_CARD_LIMIT_TYPE_WEEK:
			{
				if (m_common_param.card_shop_buy_times[seq] >= shop_item_cfg->week_buy_times)
				{
					return 2;
				}
			}
			break;
			case MONTH_CARD_LIMIT_TYPE_DAY:
			{
				if (m_common_param.card_shop_buy_times[seq] >= shop_item_cfg->today_buy_times)
				{
					return 2;
				}
			}
			break;
			default:
			return 1;
		}

		if(shop_item_cfg->price_type != MONTH_CARD_PRICE_TYPE_MONEY || shop_item_cfg->price <= 0) return 1;

		if (NULL != price)
		{
			*price = shop_item_cfg->price;
			return 0;
		}

		if (NULL != ITEMPOOL->GetItem(shop_item_cfg->item.item_id))
		{
			bool is_need_send_mail = true;
			if (m_role_module_mgr->GetKnapsack()->CheckForPut(shop_item_cfg->item.item_id, shop_item_cfg->item.num, false))
			{
				if (m_role_module_mgr->GetKnapsack()->Put(shop_item_cfg->item, PUT_REASON_MONTH_CARD_REWARD))
				{
					is_need_send_mail = false;
				}
			}
			if (is_need_send_mail)
			{
				m_role_module_mgr->GetKnapsack()->SendMail(&shop_item_cfg->item, 1, SEND_MAIL_TYPE_MONTH_CARD_BUY_ITEM, false);
			}
		}
		switch (shop_item_cfg->limit_type)
		{
			case MONTH_CARD_LIMIT_TYPE_WEEK:
			{
				m_common_param.card_shop_buy_times[seq]++;
				gamelog::g_log_month_week_card.printf(LL_INFO, "%s role[%d %s] seq[%d] buy_times[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
					seq, m_common_param.card_shop_buy_times[seq]);
			}
			break;
			case MONTH_CARD_LIMIT_TYPE_DAY:
			{
				
				m_common_param.card_shop_buy_times[seq]++;
				gamelog::g_log_month_week_card.printf(LL_INFO, "%s role[%d %s] seq[%d] buy_times[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
					seq, m_common_param.card_shop_buy_times[seq]);
			}
			break;
			default:
			break;
		}
	}
	else
	{
		return 1;
	}


	this->OnSendMonthCardInfo();
	return 0;
}

void RoleActivityManager::OnBuyMonthShopItem(int seq)
{
	unsigned int now = (unsigned int)EngineAdapter::Instance().Time();
	if (now > m_common_param.week_card_end_timestamp && now > m_common_param.month_card_end_timestamp)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_SHOP_LOCK_NOT_CAN_BUY_ITEM);
		return;
	}
	const MonthShopCfg * shop_item_cfg = LOGIC_CONFIG->GetMonthCardConfig()->GetShopCfg(m_common_param.month_card_level, seq);
	if (NULL == shop_item_cfg)
	{
		return;
	}
	int & buy_times = m_common_param.card_shop_buy_times[seq];
	switch (shop_item_cfg->limit_type)
	{
		case MONTH_CARD_LIMIT_TYPE_WEEK:
		{
			if (buy_times >= shop_item_cfg->week_buy_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_BUY_NOT_ENOUGH);
				return;
			}
		}
		break;
		case MONTH_CARD_LIMIT_TYPE_DAY:
		{
			if (buy_times >= shop_item_cfg->today_buy_times)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_BUY_NOT_ENOUGH);
				return;
			}
		}
		break;
		default:
		return;
	}

	if (shop_item_cfg->price > 0)
	{
		switch (shop_item_cfg->price_type)
		{
			case MONTH_CARD_PRICE_TYPE_GOLD:
			{
				if (!m_role_module_mgr->GetMoney()->GoldMoreThan(shop_item_cfg->price))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_GOLD_NOT_ENOUGH);
					return;
				}
				m_role_module_mgr->GetMoney()->UseGold(shop_item_cfg->price, __FUNCTION__);
			}
			break;
			case MONTH_CARD_PRICE_TYPE_SILVER_COIN:
			{
				if (!m_role_module_mgr->GetMoney()->OtherCoinMoreThan(shop_item_cfg->price, MONEY_TYPE_SILVER_COIN))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_SILVER_COIN_NOT_ENOUGH);
					return;
				}
				m_role_module_mgr->GetMoney()->UseOtherCoin(shop_item_cfg->price, MONEY_TYPE_SILVER_COIN, __FUNCTION__);
			}
			break;
			case MONTH_CARD_PRICE_TYPE_COIN:
			{
				if (!m_role_module_mgr->GetMoney()->CoinBindMoreThan(shop_item_cfg->price))
				{
					m_role_module_mgr->NoticeNum(errornum::EN_COIN_NOT_ENOUGH);
					return;
				}
				m_role_module_mgr->GetMoney()->UseCoinBind(shop_item_cfg->price, __FUNCTION__);
			}
			break;
			case MONTH_CARD_PRICE_TYPE_MONEY:
			{
				return;
			}
			break;
		default:
			return;
		}
	}
	bool is_put_succ = false;
	if (m_role_module_mgr->GetKnapsack()->CheckForPut(shop_item_cfg->item.item_id, shop_item_cfg->item.num, false))
	{
		if (m_role_module_mgr->GetKnapsack()->Put(shop_item_cfg->item, PUT_REASON_MONTH_CARD_REWARD))
		{
			is_put_succ = true;
		}
	}
	if (!is_put_succ)
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&shop_item_cfg->item, 1, SEND_MAIL_TYPE_MONTH_CARD_BUY_ITEM, true);
	}
	
	buy_times++;
	this->OnSendMonthCardInfo();
	gamelog::g_log_month_week_card.printf(LL_INFO, "%s role[%d %s] seq[%d] buy_times[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), seq, buy_times);
}

void RoleActivityManager::OnFetchMonthDayReward(int card_type)
{
	const MonthCardCfg * card_cfg = LOGIC_CONFIG->GetMonthCardConfig()->GetCardCfg(m_common_param.month_card_level, card_type);
	if (NULL == card_cfg)
	{
		return;
	}
	switch (card_type)
	{
		case MONTH_CARD_TYPE_WEEK:
		{
			if ((unsigned int)EngineAdapter::Instance().Time() > m_common_param.week_card_end_timestamp)
			{
				return;
			}
		}
		break;
		case MONTH_CARD_TYPE_MONTH:
		{
			if ((unsigned int)EngineAdapter::Instance().Time() > m_common_param.month_card_end_timestamp)
			{
				return;
			}
		}
		break;
	default:
		break;
	}
	if (m_common_param.is_fetch_card_day_reward_flag & (1 << card_type))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_COLLECT_WORD_NOT_FETCH);
		return;
	}
	bool is_put_succ = false;
	if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)card_cfg->day_reward_list.size(), &card_cfg->day_reward_list[0]))
	{
		if (m_role_module_mgr->GetKnapsack()->PutList((short)card_cfg->day_reward_list.size(), &card_cfg->day_reward_list[0], PUT_REASON_MONTH_CARD_REWARD))
		{
			is_put_succ = true;
		}
	}
	if (!is_put_succ)
	{
		m_role_module_mgr->GetKnapsack()->SendMail(&card_cfg->day_reward_list[0], (int)card_cfg->day_reward_list.size(), SEND_MAIL_TYPE_MONTH_CARD_BUY_CARD_OR_DAY_REWARD, true, card_type);
	}
	m_common_param.is_fetch_card_day_reward_flag |= (1 << card_type);
	this->OnSendMonthCardInfo();
	gamelog::g_log_month_week_card.printf(LL_INFO, "%s role[%d %s] card_type[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), card_type);
}

bool RoleActivityManager::OnBuyMonthCheck(int req_type, int param1, int cfg_ver, long long ARG_OUT & unique_key)
{
	const MonthCardOtherCfg & other_cfg = LOGIC_CONFIG->GetMonthCardConfig()->GetOtherCfg();
	if (cfg_ver != other_cfg.cfg_ver)
	{	
		Protocol::SCCmdBuyItemCheckRet protocol;
		protocol.unique_id = 0;
		protocol.cfg_ver = other_cfg.cfg_ver;
		m_role_module_mgr->NetSend(&protocol, sizeof(protocol));

		m_role_module_mgr->NoticeNum(errornum::EN_NEED_UPDATE_CFG_VER);
		return false;
	}

	unsigned int now = static_cast<unsigned int>(EngineAdapter::Instance().Time());
	unique_key = 0;
	int need_gold = 0, common_param1 = 0, common_param2 = 0, common_param3 = 0, common_param4 = 0, common_param5 = 0;
	std::vector<ItemConfigData> item_list;
	switch (req_type)
	{
		case Protocol::CSMonthCardReq::MONTH_CARD_REQ_TYPE_BUY_CMD_CHECK:
		{
			if (now > m_common_param.week_card_end_timestamp && now > m_common_param.month_card_end_timestamp)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_SHOP_LOCK_NOT_CAN_BUY_ITEM);
				return false;
			}
			const MonthShopCfg * shop_item_cfg = LOGIC_CONFIG->GetMonthCardConfig()->GetShopCfg(m_common_param.month_card_level, param1);
			if (NULL == shop_item_cfg)
			{
				return false;
			}
			switch (shop_item_cfg->limit_type)
			{
			case MONTH_CARD_LIMIT_TYPE_WEEK:
			{
				if (m_common_param.card_shop_buy_times[param1] >= shop_item_cfg->week_buy_times)
				{
					m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_BUY_NOT_ENOUGH);
					return false;
				}
			}
			break;
			case MONTH_CARD_LIMIT_TYPE_DAY:
			{
				if (m_common_param.card_shop_buy_times[param1] >= shop_item_cfg->today_buy_times)
				{
					m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_BUY_NOT_ENOUGH);
					return false;
				}
			}
			break;
			default:
				return false;
			}
			if(shop_item_cfg->price <= 0) return false;

			if(shop_item_cfg->price_type != MONTH_CARD_PRICE_TYPE_MONEY) return false;

			common_param1 = MONTH_CARD_CMD_TYPE_BUY_ITEM;
			common_param2 = param1;
			common_param3 = shop_item_cfg->limit_type;
			need_gold = shop_item_cfg->price;
			if (NULL != ITEMPOOL->GetItem(shop_item_cfg->item.item_id))
			{
				item_list.push_back(shop_item_cfg->item);
			}
		}
		break;
		case Protocol::CSMonthCardReq::MONTH_CARD_REQ_TYPE_BUY_CARD_CHECK:
		{
			const MonthCardCfg * card_cfg = LOGIC_CONFIG->GetMonthCardConfig()->GetCardCfg(m_role_module_mgr->GetRole()->GetLevel(), param1);
			if (NULL == card_cfg)
			{
				return false;
			}
			if(card_cfg->buy_money <= 0) return false;

			common_param1 = MONTH_CARD_CMD_TYPE_CARD;
			common_param2 = param1;
			common_param3 = card_cfg->buy_reward_gold;
			common_param4 = card_cfg->buy_reward_immortal;
			common_param5 = card_cfg->card_day;
			need_gold = card_cfg->buy_money;
			if (NULL != ITEMPOOL->GetItem(card_cfg->buy_reward.item_id))
			{
				item_list.push_back(card_cfg->buy_reward);
			}
		}
		break;
	default:
		return false;
	}
	if (!UniqueIDGenerator::Instance()->GenUniqueID(&unique_key))
	{
		return false;
	}

	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetNewCmdBuyRecord<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), ROLE_COMMON_SAVE_DATA_TYPE_MONTH_CARD);
	if (NULL == buy_item)
	{
		return false;
	}

	buy_item->cfg_vesion = other_cfg.cfg_ver;
	buy_item->need_gold = need_gold;
	buy_item->create_time = now;
	buy_item->ra_begin_timestamp = 0;
	buy_item->unique_id = unique_key;
	buy_item->param1 = common_param1;
	buy_item->param2 = common_param2;
	buy_item->param3 = common_param3;
	buy_item->param4 = common_param4;
	buy_item->param5 = common_param5;
	::ItemConfigDataVecToBuyItem(item_list, buy_item->rewards_item, &buy_item->rewards_num);
	CmdBuyRecordManager::Instance().DelRedundantCmdBuyRecord< RoleCommonSaveCmdBuyItem >(m_role_module_mgr->GetRole(), ROLE_COMMON_SAVE_DATA_TYPE_MONTH_CARD, gamelog::g_log_month_week_card, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();

	std::string log_str2 = ::GetString(*buy_item);
	gamelog::g_log_month_week_card.printf(LL_INFO, "%s line:%d |  role[%d %s] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(), log_str2.c_str());

	return true;
}

int RoleActivityManager::OnBuyCmdMonthCard(int chong_zhi_gold, long long unique_id)
{
	RoleCommonSaveCmdBuyItem * buy_item = CmdBuyRecordManager::Instance().GetCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), ROLE_COMMON_SAVE_DATA_TYPE_MONTH_CARD, unique_id);
	if (NULL == buy_item)
	{
		return -__LINE__;
	}
	gamelog::g_log_month_week_card.printf(LL_INFO, "%s line:%d | role[%d %s] cmd_unique_id[%lld] %s", __FUNCTION__, __LINE__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
		unique_id, ::GetString(*buy_item).c_str());

	if (unique_id != buy_item->unique_id)
	{
		return -__LINE__;
	}
	if (chong_zhi_gold != buy_item->need_gold)
	{
		return -__LINE__;
	}
	std::vector<ItemConfigData> rewards = ::BuyItemToItemConfigDataVec(buy_item->rewards_num, buy_item->rewards_item);
	bool is_need_mail_send = false;
	if (!rewards.empty())
	{
		is_need_mail_send = true;
		if (m_role_module_mgr->GetKnapsack()->CheckForPutListWithoutTemporary((short)rewards.size(), &rewards[0]))
		{
			if (m_role_module_mgr->GetKnapsack()->PutList((short)rewards.size(), &rewards[0], PUT_REASON_MONTH_CARD_REWARD))
			{
				is_need_mail_send = false;
			}
		}
	}
	time_t now_second = EngineAdapter::Instance().Time();
	switch (buy_item->param1)
	{
		case MONTH_CARD_CMD_TYPE_BUY_ITEM:
		{
			if (buy_item->param2 >= 0 && buy_item->param2 < MAX_MONTH_CARD_SHOP_ITEM_NUM)
			{
				switch (buy_item->param3)
				{
					case MONTH_CARD_LIMIT_TYPE_WEEK:
					{
						if (GetWeekZeroTime(buy_item->create_time) == GetWeekZeroTime(now_second))
						{
							m_common_param.card_shop_buy_times[buy_item->param2]++;
							gamelog::g_log_month_week_card.printf(LL_INFO, "%s role[%d %s] seq[%d] buy_times[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
								buy_item->param2, m_common_param.card_shop_buy_times[buy_item->param2]);
						}
					}
					break;
					case MONTH_CARD_LIMIT_TYPE_DAY:
					{
						if (GetZeroTime(buy_item->create_time) == GetZeroTime(now_second))
						{
							m_common_param.card_shop_buy_times[buy_item->param2]++;
							gamelog::g_log_month_week_card.printf(LL_INFO, "%s role[%d %s] seq[%d] buy_times[%d]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
								buy_item->param2, m_common_param.card_shop_buy_times[buy_item->param2]);
						}
					}
					break;
					default:
					break;
				}
				if (is_need_mail_send)
				{
					m_role_module_mgr->GetKnapsack()->SendMail(&rewards[0], (int)rewards.size(), SEND_MAIL_TYPE_MONTH_CARD_BUY_ITEM, true);
				}
			}
		}
		break;
		case MONTH_CARD_CMD_TYPE_CARD:
		{
			switch (buy_item->param2)
			{
				case MONTH_CARD_TYPE_WEEK:
				{
					if (m_common_param.week_card_end_timestamp > now_second)
					{
						m_common_param.week_card_end_timestamp += buy_item->param5 * SECOND_PER_DAY;
					}
					else
					{	
						m_common_param.week_card_end_timestamp = (unsigned int)(now_second + buy_item->param5 * SECOND_PER_DAY);
						//新开
						m_common_param.month_card_level = m_role_module_mgr->GetRole()->GetLevel();
					}
					gamelog::g_log_month_week_card.printf(LL_INFO, "%s role[%d %s] card_type[%d] end_timestamp[%u]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
						buy_item->param2, m_common_param.week_card_end_timestamp);
				}
				break;
				case MONTH_CARD_TYPE_MONTH:
				{
					if (m_common_param.month_card_end_timestamp > now_second)
					{
						m_common_param.month_card_end_timestamp += buy_item->param5 * SECOND_PER_DAY;
					}
					else
					{
						m_common_param.month_card_end_timestamp = (unsigned int)(now_second + buy_item->param5 * SECOND_PER_DAY);
						//新开
						m_common_param.month_card_level = m_role_module_mgr->GetRole()->GetLevel();
					}
					gamelog::g_log_month_week_card.printf(LL_INFO, "%s role[%d %s] card_type[%d] end_timestamp[%u]", __FUNCTION__, m_role_module_mgr->GetRole()->GetUID(), m_role_module_mgr->GetRole()->GetName(),
						buy_item->param2, m_common_param.month_card_end_timestamp);
				}
				break;
				default:
				break;
			}
			if (is_need_mail_send)
			{
				static MailContentParam contentparam; contentparam.Reset();
				FillMailContentParam(contentparam, rewards);
				if (buy_item->param3 > 0)
				{
					contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_ITEM_GOLD] += buy_item->param3;
				}
				if (buy_item->param4 > 0)
				{
					contentparam.virtual_item_list.virtual_item[MAIL_VIRTUAL_SILVER_COIN] += buy_item->param4;
				}

				int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_month_week_card_reward_subject);
				int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_buy_month_week_card_reward_content, buy_item->param2);
				if (lensub > 0 && length > 0)
				{
					MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);
				}
			}
			else
			{
				if (buy_item->param3 > 0)
				{
					m_role_module_mgr->GetMoney()->AddGold(buy_item->param3, __FUNCTION__);
				}
				if (buy_item->param4 > 0)
				{
					m_role_module_mgr->GetMoney()->AddOtherCoin(buy_item->param4, MONEY_TYPE_SILVER_COIN, __FUNCTION__);
				}
			}
		}
		break;
	default:
		break;
	}

	this->OnSendMonthCardInfo();
	CmdBuyRecordManager::Instance().DelCmdBuyRecordByUniqueId<RoleCommonSaveCmdBuyItem>(m_role_module_mgr->GetRole(), ROLE_COMMON_SAVE_DATA_TYPE_MONTH_CARD, unique_id, gamelog::g_log_month_week_card, __FUNCTION__);
	m_role_module_mgr->GetRole()->Save();
	return 0;
}

void RoleActivityManager::OnMonthCardDayChange()
{
	m_common_param.is_fetch_card_day_reward_flag = 0;
	LOGIC_CONFIG->GetMonthCardConfig()->OnRefreshBuyTimes(m_common_param.month_card_level, m_common_param.card_shop_buy_times, MAX_MONTH_CARD_SHOP_ITEM_NUM, MONTH_CARD_LIMIT_TYPE_DAY);
	this->OnSendMonthCardInfo();
}

void RoleActivityManager::OnMonthCardWeekChange()
{
	m_common_param.month_card_level = m_role_module_mgr->GetRole()->GetLevel();
	LOGIC_CONFIG->GetMonthCardConfig()->OnRefreshBuyTimes(m_common_param.month_card_level ,m_common_param.card_shop_buy_times, MAX_MONTH_CARD_SHOP_ITEM_NUM, MONTH_CARD_LIMIT_TYPE_WEEK);
	this->OnSendMonthCardInfo();
}

void RoleActivityManager::OnSendMonthCardInfo()
{
	Protocol::SCMonthCardInfo info;
	info.is_fetch_card_day_reward_flag = m_common_param.is_fetch_card_day_reward_flag;
	info.reserve_ch = m_common_param.reserve_ch;
	info.month_card_level = m_common_param.month_card_level;
	info.week_card_end_timestamp = m_common_param.week_card_end_timestamp;
	info.month_card_end_timestamp = m_common_param.month_card_end_timestamp;
	for (int i = 0; i < ARRAY_LENGTH(info.card_shop_buy_times) && i < ARRAY_LENGTH(m_common_param.card_shop_buy_times); i++)
	{
		info.card_shop_buy_times[i] = m_common_param.card_shop_buy_times[i];
	}

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

bool RoleActivityManager::HoldMonthCard()
{
	return m_common_param.week_card_end_timestamp >= EngineAdapter::Instance().Time() || m_common_param.month_card_end_timestamp >= EngineAdapter::Instance().Time();
}

void RoleActivityManager::SendChongZhiInfo()
{
	static Protocol::SCChongzhiInfo info;
	info.history_chongzhi_num = m_common_param.history_chongzhi_num;
	info.today_chongzhi_num = m_common_param.chongzhi_num;
	info.chongzhi_fanli_reward_fetch_flag = m_common_param.fanli_reward_fetch_flag;
	info.reset_level_num = m_common_param.level;

	EngineAdapter::Instance().NetSend(m_role_module_mgr->GetRole()->GetNetId(), (const char*)&info, sizeof(info));
}

void RoleActivityManager::FetchFanliReward(int reward_seq)
{
	const ChongzhiFanliCfg* cfg = LOGIC_CONFIG->GetChongzhiConfig()->GetFanliCfg(reward_seq, m_common_param.level);
	if (NULL == cfg) return; // 找不到该seq

	if (IsSetBit(m_common_param.fanli_reward_fetch_flag, reward_seq)) return; // 奖励已领取

	if(m_common_param.chongzhi_num < cfg->gold) return;

	Knapsack* knapsack = m_role_module_mgr->GetKnapsack();
	if (!knapsack->CheckForPutList(cfg->count, cfg->reward_list))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	SetBit(m_common_param.fanli_reward_fetch_flag, reward_seq);

	if (!knapsack->PutList(cfg->count, cfg->reward_list, PUT_REASON_CHONGZHI_FANLI))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return;
	}

	this->SendChongZhiInfo();
}

void RoleActivityManager::SendScoreRewardInfo()
{
	Protocol::SCScoreRewardInfo info;
	info.capability = m_common_param.capability;
	info.flag = m_common_param.score_reward_flag;
	m_role_module_mgr->NetSend(&info, sizeof(info));
}

int RoleActivityManager::OnRAMonthInvestmentFetch(int seq)
{
	if (m_common_param.is_buy_month_investment == 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_NOT_BOUGHT);
		return false;
	}
	if (m_common_param.ra_month_investment_buy_day > RA_MONTH_INVESTMENT_MAX_NUM)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_EXPIRED);
		return false;
	}
	int day = seq + 1;
	if (seq < 0 || seq >= RA_MONTH_INVESTMENT_MAX_NUM || day > m_common_param.ra_month_investment_buy_day)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}
	const MonthInvestmentCfg * cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetMonthInvestment(seq);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}
	if (m_common_param.month_investment_fetch_flag.IsBitSet(seq))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_NOT_ELIGIBLE);
		return false;
	}
	if (day < m_common_param.ra_month_investment_buy_day && m_common_param.ra_month_investment_last_num <= 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_MAKE_NOT_ENOUGH);
		return false;
	}
	if (!m_role_module_mgr->GetKnapsack()->CheckForPut(cfg->item.item_id, cfg->item.num))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}
	if (!m_role_module_mgr->GetKnapsack()->Put(cfg->item, PUT_REASON_DAY_REWARD))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_KNAPSACK_NO_SPACE);
		return false;
	}
	if (day < m_common_param.ra_month_investment_buy_day)
	{
		m_common_param.ra_month_investment_last_num--;
	}

	m_common_param.month_investment_fetch_flag.SetBit(seq);

	this->SendRAMonthInvestmentInfo();
	return true;
}

void RoleActivityManager::SendRAMonthInvestmentInfo()
{
	Protocol::SCMonthInvestmentInfo info;

	info.is_buy = m_common_param.is_buy_month_investment;
	info.last_num = m_common_param.ra_month_investment_last_num;
	info.day = m_common_param.ra_month_investment_buy_day;
	info.month_investment_fetch_flag = m_common_param.month_investment_fetch_flag;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

int RoleActivityManager::OnRABuyMonthInvestment()
{
	if (m_common_param.is_buy_month_investment || m_common_param.ra_month_buy_time_timestamp != 0)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_RAND_ACTIVITY_BOUGHT);
		return false;
	}
	m_common_param.ra_month_buy_time_timestamp = EngineAdapter::Instance().Time();
	m_common_param.is_buy_month_investment = 1;
	m_common_param.ra_month_investment_buy_day = 1;
	m_common_param.ra_month_investment_last_num = 10;
	m_common_param.month_investment_fetch_flag.Reset();
	const RandActivityOtherCfg & other_cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetRandActivityOtherCfg();

	const std::vector<MonthPurchaseRewardCfg> * cfg = LOGIC_CONFIG->GetRandActivityConfigManager()->GetMonthPurchaseReward();
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}
	m_role_module_mgr->GetMoney()->AddGold(other_cfg.monthly_top_up_ingot, __FUNCTION__);
	ItemDataWrapper item[RA_MONTH_PURCHASE_REWARD];
	int count = RA_MONTH_PURCHASE_REWARD > (int)(*cfg).size() ? (int)(*cfg).size() : RA_MONTH_PURCHASE_REWARD;
	for (int i = 0; i < count; i++)
	{
		item[i].item_id = (*cfg)[i].item.item_id;
		item[i].is_bind = (*cfg)[i].item.is_bind;
		item[i].num = (*cfg)[i].item.num;
	}
	bool is_ret = true;
	if (count > 0)
	{
		if (m_role_module_mgr->GetKnapsack()->CheckForPutList(count, item))
		{
			if (m_role_module_mgr->GetKnapsack()->PutList(count, item, PUT_REASON_DAY_REWARD))
				is_ret = false;
		}
	}

	static MailContentParam contentparam;
	if (is_ret && count > 0)
	{
		UNSTD_STATIC_CHECK(RA_MONTH_PURCHASE_REWARD <= MAX_ATTACHMENT_ITEM_NUM);

		contentparam.Reset();
		if (count >= MAX_ATTACHMENT_ITEM_NUM)
		{
			memcpy(contentparam.item_list, item, sizeof(contentparam.item_list));
		}
		else
		{
			memcpy(contentparam.item_list, item, sizeof(ItemDataWrapper) * count);
		}

		int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_knapsack_put_subject);
		int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_knapsack_put_content);
		if (length1 > 0 && length2 > 0)
		{
			m_role_module_mgr->NoticeNum(noticenum::NT_KNAPSACKFULL_SEND_MAIL);
			MailRoute::MailToUser(m_role_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
		}
	}

	contentparam.Reset();

	int length1 = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_month_investment_subject);
	int length2 = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_month_investment_content);
	if (length1 > 0 && length2 > 0)
	{
		MailRoute::MailToUser(m_role_module_mgr->GetRole()->GetUserId(), MAIL_REASON_DEFAULT, contentparam);
	}


	this->SendRAMonthInvestmentInfo();
	return true;
}

void RoleActivityManager::SendActivitySignUpInfo()
{
	Protocol::SCActivitySignUpInfo info;
	info.m_sign_up_flag = m_common_param.sign_up_flag;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}

void RoleActivityManager::SendBonusInfo()
{
	Protocol::SCRandActivityBonusInfo info;
	info.bonus_flag = m_common_param.bonus_flag;

	m_role_module_mgr->NetSend(&info, sizeof(info));
}


bool RoleActivityManager::OnActivityEnterOnlyFight(Protocol::CSActivityEnterReq * aer, int & to_scene_key, int & to_scene_id, Posi & to_scene_pos, ARG_OUT bool* is_go_to_cross)
{
	if (NULL == aer)
	{
		return false;
	}

	ActivityOnlyFight * activity = (ActivityOnlyFight *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_ONLY_FIGHT);
	if (NULL == activity) return false;

	if (!activity->IsActivityOpen())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return false;
	}
	if (CrossConfig::Instance().IsHiddenServer())
	{
		if (!activity->IsActivityEnterTime())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_ENTER_TIME_LATER_NOT_ENTER);
			return false;
		}
	}

	const FunOpenCfg* cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(FUN_TYPE_ONLY_FIGHT);
	if (NULL == cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
		return false;
	}
	if (m_role_module_mgr->GetRole()->GetLevel() < cfg->limit_level)
	{
		Protocol::SCSystemMsg sm;
		int sendlen = 0;
		int length = SNPRINTF(gamestring::GAMESTRING_BUF, sizeof(gamestring::GAMESTRING_BUF), gamestring::g_scene_unlock_notice, cfg->limit_level);
		if (length > 0)
		{
			if (SceneManager::CreateSystemMsg(&sm, &sendlen, gamestring::GAMESTRING_BUF, length, GetMsgType(SYS_MSG_FLOAT)))
			{
				m_role_module_mgr->NetSend((const char*)&sm, sendlen);
			}
		}
		return false;
	}
	if (m_role_module_mgr->GetRole()->InTeam())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ROLE_IN_TEAM);
		return false;
	}
	const OnlyFightSceneCfg * scene_cfg = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetOnlyFightSceneCfgByLevel(m_role_module_mgr->GetRole()->GetLevel());
	if (NULL == scene_cfg)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_CURR_LEVEL_NOT_ENTER_SCENE);
		return false;
	}

	if (!m_role_module_mgr->GetRole()->IsInCross())
	{
		if (!CrossManager::Instance().IsCrossServerConnected())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_CROSS_SERVER_NOT_OPEN);
			return false;
		}

		CrossData_Business business_data;
		business_data.auto_join_type = CROSS_AUTO_JION_TYPE_ACTIVITY;
		business_data.auto_join_type_param1 = aer->activity_type;
		business_data.cross_business_type = CrossData_Business::BUSINESS_TYPE_ROLE_GO_TO_HIDDEN;
		m_role_module_mgr->GetRoleCross()->OnStartCrossReq(business_data);

		*is_go_to_cross = true;
		return false;
	}

	to_scene_id = scene_cfg->scene_id;
	to_scene_pos = LOGIC_CONFIG->GetActivityOnlyFightConfig()->GetRandBornPosi();
	to_scene_key = COMMON_FB_KEY;
	return true;
}

bool RoleActivityManager::OnActivityEnterTeamFight(Protocol::CSActivityEnterReq * aer, int & to_scene_key, int & to_scene_id, Posi & to_scene_pos, ARG_OUT bool* is_go_to_cross)
{
	if (ActivityManager::Instance().IsActivityStandy(ACTIVITY_TYPE_TEAM_FIGHT))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_IS_STANDY_NOT_ENTER);
		return false;
	}
	else if (!ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_TEAM_FIGHT))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return false;
	}

	int count = 0;
	Role* role_list[MAX_TEAM_MEMBER_NUM] = { NULL };

	//--------------------检查条件--------------------
	Team * team = m_role_module_mgr->GetRole()->GetMyTeam();
	if (NULL != team)
	{
		if (team->HasDismissMember())
		{
			m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
			return false;
		}

		if (team->GetMemberCount() > TUAN_DUI_JING_JI_MAX_MEMBER_NUM)
		{
			team->SendLeaderIncompatibleNotice(0, Protocol::TEAM_MEMBER_INCOMPATIBLE_OVER_MEMBER_NUM, TUAN_DUI_JING_JI_MAX_MEMBER_NUM);
			return false;
		}


		for (int i = 0; i < team->GetMemberCount(); i++)
		{
			TeamMember * member_info = team->GetMemberInfo(i);
			if (NULL != member_info)
			{
				if (member_info->level < LOGIC_CONFIG->GetActivityTeamFightConfig()->GetOtherCfg()->level)
				{
					m_role_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
					return false;
				}

				Role * role = team->GetMemberRoleByIndex(i);
				if (NULL != role)
				{
					role_list[count++] = role;
				}
			}
		}
	}
	else
	{
		if (m_role_module_mgr->GetRole()->GetLevel() < LOGIC_CONFIG->GetActivityTeamFightConfig()->GetOtherCfg()->level)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
			return false;
		}
		role_list[count++] = m_role_module_mgr->GetRole();
	}
	//-------------------------------------------------------------

	if (!m_role_module_mgr->GetRole()->IsInCross())
	{
		if (!CrossManager::Instance().IsCrossServerConnected())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_CROSS_SERVER_NOT_OPEN);
			return false;
		}

		Team * team = m_role_module_mgr->GetRole()->GetMyTeam();
		if (NULL != team)
		{
			TeamManager::Instance().OnTeamGotoHidden(m_role_module_mgr->GetRole(), ACTIVITY_TYPE_TEAM_FIGHT);
		}
		else
		{
			CrossData_Business business_data;
			business_data.auto_join_type = CROSS_AUTO_JION_TYPE_ACTIVITY;
			business_data.auto_join_type_param1 = ACTIVITY_TYPE_TEAM_FIGHT;
			business_data.cross_business_type = CrossData_Business::BUSINESS_TYPE_ROLE_GO_TO_HIDDEN;
			m_role_module_mgr->GetRoleCross()->OnStartCrossReq(business_data);
		}

		*is_go_to_cross = true;
		return false;
	}
	else
	{
		to_scene_id = LOGIC_CONFIG->GetActivityTeamFightConfig()->GetOtherCfg()->scene_id;
		to_scene_pos = LOGIC_CONFIG->GetActivityTeamFightConfig()->GetOtherCfg()->pos;
		to_scene_key = COMMON_FB_KEY;
	}
	return true;
}

bool RoleActivityManager::OnActivityEnterGuildFight(Protocol::CSActivityEnterReq * aer, int & to_scene_key, int & to_scene_id, Posi & to_scene_pos, ARG_OUT bool* is_go_to_cross)
{
	ActivityGuildFight * activity = (ActivityGuildFight *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_GUILD_FIGHT);
	if (NULL == activity) return false;

	if (!activity->IsActivityOpen())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return false;
	}
	if (CrossConfig::Instance().IsHiddenServer())
	{
		if (!activity->IsActivityEnterTime())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_ENTER_TIME_LATER_NOT_ENTER);
			return false;
		}
	}

	int count = 0;
	Role* role_list[MAX_TEAM_MEMBER_NUM] = { NULL };

	Guild* guild = m_role_module_mgr->GetRole()->GetGuild();
	if (NULL == guild)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GUILD_NOT_IN_GUILD);
		return false;
	}

	int limit_level = 0;
	const FunOpenCfg* cfg = LOGIC_CONFIG->GetFunOpenConfig()->GetFunOpenCfg(FUN_TYPE_GUILD_LUANDOU);
	if (NULL != cfg)
	{
		limit_level = cfg->limit_level;
	}

	if (!m_role_module_mgr->GetRole()->IsInCross())
	{
		if (!CrossManager::Instance().IsCrossServerConnected())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_CROSS_SERVER_NOT_OPEN);
			return false;
		}

		this->JoinActivityInCross(ACTIVITY_TYPE_GUILD_FIGHT, limit_level, true);
		*is_go_to_cross = true;
		return false;
	}
	else
	{
		Team* team = m_role_module_mgr->GetRole()->GetMyTeam();
		if (NULL != team)
		{
			if (team->HasDismissMember())
			{
				m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
				return false;
			}

			for (int i = 0; i < team->GetMemberCount(); i++)
			{
				Role* member_role = team->GetMemberRoleByIndex(i);
				if (NULL != member_role)
				{
					Guild* member_guild = member_role->GetGuild();
					if (NULL == member_guild || member_guild->GetGuildID() != guild->GetGuildID())
					{
						m_role_module_mgr->NoticeNum(errornum::EN_TEAM_MEMBER_NOT_GUILD_MEMBER);
						return false;
					}

					if (member_role->GetLevel() < limit_level)
					{
						m_role_module_mgr->NoticeNum(errornum::EN_TEAM_MEMBER_LEVEL_LIMIT);
						return false;
					}
					role_list[count++] = member_role;
				}
			}
		}
		else
		{
			if (m_role_module_mgr->GetRole()->GetLevel() < limit_level)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
				return false;
			}
			role_list[count++] = m_role_module_mgr->GetRole();
		}

		to_scene_id = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetGuildFightOtherCfg().scene_id;
		to_scene_pos = LOGIC_CONFIG->GetActivityGuildFightConfig()->GetRandBornPosi();
		to_scene_key = COMMON_FB_KEY;
	}
	return true;
}

bool RoleActivityManager::OnActivityEnterGuildAnswer(Protocol::CSActivityEnterReq * aer, int & to_scene_key, int & to_scene_id, Posi & to_scene_pos, ARG_OUT bool* is_go_to_cross)
{
	if (!ActivityManager::Instance().IsActivityOpen(ACTIVITY_TYPE_GUILD_ANSWER))
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return false;
	}
	if (!FunOpen::IsFunOpen(m_role_module_mgr->GetRole(), FUN_TYPE_GUILD_ANSWER)) return false;

	int count = 0;
	Role* role_list[MAX_TEAM_MEMBER_NUM] = { NULL };

	Guild* guild = m_role_module_mgr->GetRole()->GetGuild();
	if (NULL == guild)
	{
		m_role_module_mgr->NoticeNum(errornum::EN_GUILD_NOT_IN_GUILD);
		return false;
	}

	int limit_level = LOGIC_CONFIG->GetActivityGuildAnswerConfig()->GetGuildAnswerOtherConfig().min_level;
	if (!m_role_module_mgr->GetRole()->IsInCross())
	{
		if (!CrossManager::Instance().IsCrossServerConnected())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_CROSS_SERVER_NOT_OPEN);
			return false;
		}

		this->JoinActivityInCross(ACTIVITY_TYPE_GUILD_ANSWER, limit_level, true);
		*is_go_to_cross = true;
		return false;
	}
	else
	{
		Team* team = m_role_module_mgr->GetRole()->GetMyTeam();
		if (NULL != team)
		{
			if (team->HasDismissMember())
			{
				m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
				return false;
			}

			for (int i = 0; i < team->GetMemberCount(); i++)
			{
				Role* member_role = team->GetMemberRoleByIndex(i);
				if (NULL != member_role)
				{
					Guild* member_guild = member_role->GetGuild();
					if (NULL == member_guild || member_guild->GetGuildID() != guild->GetGuildID())
					{
						m_role_module_mgr->NoticeNum(errornum::EN_TEAM_MEMBER_NOT_GUILD_MEMBER);
						return false;
					}

					if (member_role->GetLevel() < limit_level)
					{
						m_role_module_mgr->NoticeNum(errornum::EN_TEAM_MEMBER_LEVEL_LIMIT);
						return false;
					}
					role_list[count++] = member_role;
				}
			}
		}
		else
		{
			if (m_role_module_mgr->GetRole()->GetLevel() < limit_level)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
				return false;
			}
			role_list[count++] = m_role_module_mgr->GetRole();
		}

		const GuildAnswerOthers & ga_cfg = LOGIC_CONFIG->GetActivityGuildAnswerConfig()->GetGuildAnswerOtherConfig();
		to_scene_id = ga_cfg.scene_id;
		to_scene_pos = Posi(0, 0);
		to_scene_key = COMMON_FB_KEY;

		ActivityGuildAnswer* activity = (ActivityGuildAnswer*)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_GUILD_ANSWER);
		if (NULL != activity)
		{
			int least_role_num = MAX_INT;

			// 哪个房间少人就分配到哪个房间
			const std::vector<int>& key_vec = activity->GetRoomKeyVec();
			for (size_t key_idx = 0; key_idx < key_vec.size(); ++key_idx)
			{
				int room_key = key_vec[key_idx];
#ifdef _DEBUG
				room_key = key_vec[0];
#endif
				SpecialLogicGuildAnswer* ga = World::GetInstWorld()->GetSceneManager()->GetSpecialLogicGuildAnswer(to_scene_id, room_key);
				if (NULL != ga)
				{
					if (least_role_num > ga->GetScene()->RoleNum())
					{
						to_scene_key = room_key;
						least_role_num = ga->GetScene()->RoleNum();
					}
				}
			}
		}
	}
	return true;
}

bool RoleActivityManager::OnActivityEnterWorldTeamArena(Protocol::CSActivityEnterReq * aer, int & to_scene_key, int & to_scene_id, Posi & to_scene_pos, ARG_OUT bool * is_go_to_cross)
{
	ActivityWorldTeamArena * activity = (ActivityWorldTeamArena *)ActivityManager::Instance().ForceGetActivity(ACTIVITY_TYPE_WORLD_TEAM_ARENA);
	if (NULL == activity) return false;

	if (!activity->IsActivityOpen())
	{
		m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_NOT_OPEN);
		return false;
	}
	if (CrossConfig::Instance().IsHiddenServer())
	{
		if (!activity->IsActivityEnterTime())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_ENTER_TIME_LATER_NOT_ENTER);
			return false;
		}
	}

	int limit_level = LOGIC_CONFIG->GetActivityWorldTeamArenaConfig()->GetOtherCfg().need_level;
	if (!m_role_module_mgr->GetRole()->IsInCross())
	{
		if (!CrossManager::Instance().IsCrossServerConnected())
		{
			m_role_module_mgr->NoticeNum(errornum::EN_ACTIVITY_CROSS_SERVER_NOT_OPEN);
			return false;
		}

		this->JoinActivityInCross(ACTIVITY_TYPE_WORLD_TEAM_ARENA, limit_level);
		*is_go_to_cross = true;
		return false;
	}
	else
	{
		to_scene_id = LOGIC_CONFIG->GetActivityWorldTeamArenaConfig()->GetOtherCfg().scene_id;
		to_scene_pos = LOGIC_CONFIG->GetActivityWorldTeamArenaConfig()->GetRandBornPosi();
		to_scene_key = COMMON_FB_KEY;

		//先获取比赛场景1:场景中有这个战队,那么我就离队进场景2:场景中没有这个战队,那就整队进入
		SpecialLogicWorldTeamArena * special_world_team_arena = (SpecialLogicWorldTeamArena *)World::GetInstWorld()->GetSceneManager()->GetSpecialLogicWorldTeamArena(to_scene_id, to_scene_key);
		if (NULL == special_world_team_arena)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_INPUT_PARAM_ERROR);
			return false;
		}
		long long team_unqiue_id = WorldTeamArenaSignUpManager::Instance().GetTeamUniqueId(m_role_module_mgr->GetUid());
		if (team_unqiue_id <= 0)
		{
			m_role_module_mgr->NoticeNum(errornum::EN_WORLD_TEAM_ARENA_NOT_HAS_TEAM);
			return false;
		}
		//获取比赛场景中是否有该战队的队伍
		int team_index = special_world_team_arena->GetTeamIndexByUnqiueId(team_unqiue_id);
		Team * target_team = TeamManager::Instance().GetTeamByTeamIndex(team_index);

		int count = 0;
		Role* role_list[MAX_TEAM_MEMBER_NUM] = { NULL };
		Team* team = m_role_module_mgr->GetRole()->GetMyTeam();
		if (NULL != team)
		{
			if (!team->IsLeader(m_role_module_mgr->GetRole()))
			{
				m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_ROLE_NOT_LEADER);
				return false;
			}
			if (team->HasDismissMember())
			{
				m_role_module_mgr->GetRole()->NoticeNum(errornum::EN_HAS_DISMISS_MEMBER_IN_TEAM);
				return false;
			}
			
			for (int i = 0; i < team->GetMemberCount(); i++)
			{
				Role* member_role = team->GetMemberRoleByIndex(i);
				if (NULL != member_role)
				{
					if (member_role->GetLevel() < limit_level)
					{
						m_role_module_mgr->NoticeNum(errornum::EN_TEAM_MEMBER_LEVEL_LIMIT);
						return false;
					}
					role_list[count++] = member_role;
				}
			}
			//进去前需要同战队成员(让玩家先知道情况,然后要不要离队再自己操作)
			if (!WorldTeamArenaSignUpManager::Instance().IsSameTeamByTeam(team))
			{
				return false;
			}
		}
		else
		{
			if (m_role_module_mgr->GetRole()->GetLevel() < limit_level)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_ROLE_LEVEL_NOT_ENOUTH);
				return false;
			}
			role_list[count++] = m_role_module_mgr->GetRole();
		}
		if (NULL != target_team)
		{
			if (NULL != team)
			{
				team->DismissTeamHelper();
			}
			//场景中已有该战队队伍则解散队伍,加入该战队队伍
			for (int i = 0; i < count && i < ARRAY_ITEM_COUNT(role_list); i++)
			{
				WorldStatus::Instance().LogFunctionStats(role_list[i], "RoleActivityManagerEnter::OnActivityEnterWorldTeamArena");
				TeamManager::Instance().JoinTeamDirectly(role_list[i], team_index);
			}
		}
		else
		{
			//单人则创建队伍
			if(NULL == team)
			{
				team = m_role_module_mgr->GetRole()->GetMyTeam(true);
			}
			if (NULL == team)
			{
				m_role_module_mgr->NoticeNum(errornum::EN_ROLE_NOT_IN_TEAM);
				return false;
			}
			WorldStatus::Instance().LogFunctionStats(m_role_module_mgr->GetRole(), "RoleActivityManagerEnter::OnActivityEnterWorldTeamArena");
			World::GetInstWorld()->GetSceneManager()->TeamGoTo(team, to_scene_id, to_scene_key, to_scene_pos);
		}
	}

	return false;
}

void RoleActivityManager::CheckRACompensation()
{
	if (CrossConfig::Instance().IsHiddenServer())
	{
		return;
	}

	int role_reset_flag = m_role_module_mgr->GetCommonData().randactivity_compensation_flag;
	if (role_reset_flag >= LOGIC_CONFIG->GetRACompensationConfig()->GetOtherCfg().reset_flag)
	{
		return;
	}

	std::string spid_name = LocalConfig::Instance().GetPlatName();
	int server_id = IntToUid(m_role_module_mgr->GetUid()).db_index;
	if (LOGIC_CONFIG->GetRACompensationConfig()->IsNoCompensate(spid_name, server_id))
	{
		gamelog::g_log_compensation.printf(LL_WARNING, "CheckRACompensation NoCompensate spid[%s] server_id[%d]",
			spid_name.c_str(), server_id);
		return;
	}

	const RACompensationItemGroupCfg* item_group_cfg = LOGIC_CONFIG->GetRACompensationConfig()->GetItemGroupCfgByServerId(spid_name, server_id);
	if (NULL == item_group_cfg)
	{
		gamelog::g_log_compensation.printf(LL_WARNING, "CheckRACompensation item_group_cfg NULL, spid[%s] server_id[%d]",
			spid_name.c_str(), server_id);
		return;
	}

	const RACompensationConfig::SeqList* seq_list = LOGIC_CONFIG->GetRACompensationConfig()->GetUserSeqList(m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName());
	if (NULL == seq_list)
	{
		const RACompensationConfig::SeqList* seq_list_on_error_log = LOGIC_CONFIG->GetRACompensationConfig()->GetUserSeqListOnErrorLog(m_role_module_mgr->GetUid());
		if (NULL != seq_list_on_error_log)
		{
			gamelog::g_log_compensation.printf(LL_WARNING, "CheckRACompensation role[%d,%s] seq_list NULL but seq_list_on_error_log is not NULL",
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName());
		}
		else
		{
			// 两个都为NULL，则不在补偿列表中，无必要记日志
		}

		return;
	}

	for (size_t i = 0; i < seq_list->size(); ++i)
	{
		int seq = seq_list->at(i);
		const RACompensationSeqCfg* seq_cfg = item_group_cfg->GetSeqCfg(seq);
		if (NULL == seq_cfg)
		{
			gamelog::g_log_compensation.printf(LL_WARNING, "CheckRACompensation role[%d,%s] seq_cfg NULL, server_id[%d] item_group[%d] seq[%d]",
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), server_id, item_group_cfg->item_group, seq);
			continue;
		}

		static MailContentParam contentparam;
		contentparam.Reset();
		FillMailContentParam(contentparam, seq_cfg->item_list);

		int lensub = SNPRINTF(contentparam.subject, sizeof(contentparam.subject), gamestring::g_ra_compensation_subject);
		int length = SNPRINTF(contentparam.contenttxt, sizeof(contentparam.contenttxt), gamestring::g_ra_compensation_content);
		if (lensub > 0 && length > 0)
		{
			MailRoute::MailToUser(m_role_module_mgr->GetUid(), MAIL_REASON_DEFAULT, contentparam);

			gamelog::g_log_compensation.printf(LL_INFO, "CheckRACompensation role[%d,%s] Send Mail Succ, server_id[%d] item_group[%d] seq[%d]",
				m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), server_id, item_group_cfg->item_group, seq);
		}
	}

	m_role_module_mgr->GetCommonData().randactivity_compensation_flag = LOGIC_CONFIG->GetRACompensationConfig()->GetOtherCfg().reset_flag;
	gamelog::g_log_compensation.printf(LL_INFO, "CheckRACompensation role[%d,%s] Give Compensation SUCC, server_id[%d] item_group[%d] flag[%d]",
		m_role_module_mgr->GetUid(), m_role_module_mgr->GetRoleName(), server_id, item_group_cfg->item_group, m_role_module_mgr->GetCommonData().randactivity_compensation_flag);
}

