#include "roleactivitydef.hpp"

#include "servercommon/roleactivity/ratimelimitstoreparam.hpp"
#include "servercommon/roleactivity/ratruntableparam.hpp"
#include "servercommon/roleactivity/radiscountstoreparam.hpp"
#include "servercommon/roleactivity/raconsumecarouselparam.hpp"
#include "servercommon/roleactivity/raexpelmonsterparam.hpp"
#include "servercommon/roleactivity/raheritagetreasureparam.hpp"
#include "servercommon/roleactivity/rasevencolorpictureparam.hpp"
#include "servercommon/roleactivity/rasaleshopparam.hpp"
#include "servercommon/roleactivity/rahappyupparam.hpp"
#include "servercommon/roleactivity/raanimalraceparam.hpp"
#include "servercommon/roleactivity/raluckybagparam.hpp"
#include "servercommon/roleactivity/rareducetaxrateparam.hpp"
#include "servercommon/roleactivity/ratianjianghaoliparam.hpp"
#include "servercommon/roleactivity/rasealpromoteparam.hpp"
#include "servercommon/roleactivity/radropdoubleparam.hpp"
#include "servercommon/roleactivity/rajoyseeksfavorsparam.hpp"
#include "servercommon/roleactivity/racollectwordparam.hpp"
#include "servercommon/roleactivity/radiscountcarnivalparam.hpp"
#include "servercommon/roleactivity/rapartnerexchangeparam.hpp"
#include "servercommon/roleactivity/raluckydiscountparam.hpp"
#include "servercommon/roleactivity/rapetexchangeparam.hpp"
#include "servercommon/roleactivity/rajiandanqinxinparam.hpp"
#include "servercommon/roleactivity/raadventuregiftparam.hpp"
#include "servercommon/roleactivity/ralinkagegiftparam.hpp"
#include "servercommon/roleactivity/raluckyturntableparam.hpp"
#include "servercommon/roleactivity/ratianmingdivinationparam.hpp"
#include "servercommon/roleactivity/raquanmindiscountparam.hpp"
#include "servercommon/roleactivity/rasportsparam.hpp"
#include "servercommon/roleactivity/ratujiantreasureparam.hpp"
#include "servercommon/roleactivity/ragodtokenparam.hpp"
#include "servercommon/roleactivity/raelementcardsparam.hpp"
#include "servercommon/roleactivity/rasignincontinuityparam.hpp"
#include "servercommon/roleactivity/randactivityroleparam.hpp"
#include "servercommon/roleactivity/ragivepetdrawparam.hpp"
#include "servercommon/roleactivity/racijiuyingxinparam.hpp"
#include "servercommon/roleactivity/raniuxingdayunparam.hpp"
#include "servercommon/roleactivity/raspringfestivalstoreparam.hpp"
#include "servercommon/roleactivity/racowparam.hpp"
#include "servercommon/roleactivity/ralittleyeargiftparam.hpp"
#include "servercommon/roleactivity/ragivesecretkeyparam.hpp"
#include "servercommon/roleactivity/racumulativerechargeparam.hpp"
#include "servercommon/roleactivity/rapetlootparam.hpp"
#include "servercommon/roleactivity/ratrademarketreturngoldparam.hpp"
#include "servercommon/roleactivity/ragoodgiftturnparam.hpp"
#include "servercommon/roleactivity/rahelpothersparam.hpp"
#include "servercommon/roleactivity/rahelpothersparam2.hpp"
#include "servercommon/roleactivity/rasecrettreasureparam.hpp"
#include "servercommon/roleactivity/rafortunemazeparam.hpp"
#include "servercommon/roleactivity/raburningsummerparam.hpp"
#include "servercommon/roleactivity/raqionglouyuyanparam.hpp"
#include "servercommon/roleactivity/racoolsummerparam.hpp"
#include "servercommon/roleactivity/rayuanqiturningparam.hpp"
#include "servercommon/roleactivity/ramazesummerparam.hpp"
#include "servercommon/roleactivity/ranationaldayparam.hpp"
#include "servercommon/roleactivity/rapaimaihangparam.hpp"
#include "servercommon/roleactivity/rabackrewardparam.hpp"
#include "servercommon/roleactivity/raservercompetitionparam.hpp"
#include "servercommon/roleactivity/rahappylookparam.hpp"
#include "servercommon/roleactivity/rasignhappyparam.hpp"
#include "servercommon/roleactivity/rawanlinggongyingparam.hpp"
#include "servercommon/roleactivity/rahorcruxtianchengparam.hpp"
#include "servercommon/roleactivity/raintegraldrawparam.hpp"
#include "servercommon/roleactivity/rashanhaiquestionparam.hpp"
#include "servercommon/roleactivity/rashanhaijubaoparam.hpp"
#include "servercommon/roleactivity/rashanhaimetparam.hpp"
#include "servercommon/roleactivity/rashanhaimeetparam.hpp"
#include "servercommon/roleactivity/rafateentanglementparam.hpp"
#include "servercommon/roleactivity/razhaocaijinbaoparam.hpp"
#include "servercommon/roleactivity/rayunzeterritoryparam.hpp"
#include "servercommon/roleactivity/rayunzebloomingrecordparam.hpp"
#include "servercommon/roleactivity/razaizhanyunzeparam.hpp"
#include "servercommon/roleactivity/rayunzeslotmachineparam.hpp"
#include "servercommon/roleactivity/ramozuzainaliparam.hpp"
#include "servercommon/roleactivity/rachaozhifenxiangparam.hpp"
#include "servercommon/roleactivity/rayunzeersanshiparam.hpp"
#include "servercommon/roleactivity/racolorfuldoubleparam.hpp"
#include "servercommon/roleactivity/rashanhaihuadengparam.hpp"
#include "servercommon/roleactivity/rashanhaibaoxiangparam.hpp"
#include "servercommon/roleactivity/rashanhaibaodaiparam.hpp"
#include "servercommon/roleactivity/rashanhaibaoheparam.hpp"
#include "servercommon/roleactivity/rajixinggaozhaoparam.hpp"
#include "servercommon/roleactivity/rayunzewishparam.hpp"
#include "servercommon/roleactivity/rayunzehaoliparam.hpp"
#include "servercommon/roleactivity/rayunzecutepetparam.hpp"
#include "servercommon/roleactivity/rayunzecatchelvesparam.hpp"
#include "servercommon/roleactivity/raxianshileichongparam.hpp"
#include "servercommon/roleactivity/raweekendgiftdef.hpp"
#include "servercommon/roleactivity/rachaozhigiftdef.hpp"
#include "servercommon/roleactivity/ratigerauspicioussnowparam.hpp"
#include "servercommon/roleactivity/ratigerfisheveryyearparam.hpp"
#include "servercommon/roleactivity/rayinhuactivityparam.hpp"
#include "servercommon/roleactivity/ratigerfindspringparam.hpp"
#include "servercommon/roleactivity/ratigerteasenewyearparam.hpp"
#include "servercommon/roleactivity/ratigerluckymoneyparam.hpp"
#include "servercommon/roleactivity/rajingjiyuekaparam.hpp"
#include "servercommon/roleactivity/raqingyuanactivityparam.hpp"
#include "servercommon/roleactivity/ragodbeastadventparam.hpp"
#include "servercommon/roleactivity/rayuanqispeedparam.hpp"
#include "servercommon/roleactivity/rayuanqihelpparam.hpp"
#include "servercommon/roleactivity/raqiyuankongmingdengparam.hpp"
#include "servercommon/roleactivity/raxunmenghudieparam.hpp"
#include "servercommon/roleactivity/rabozhongyouliparam.hpp"
#include "servercommon/roleactivity/rawanlingzhuliparam.hpp"
#include "servercommon/roleactivity/rayuanqijishibuparam.hpp"
#include "servercommon/roleactivity/razongxiangparam.hpp"
#include "servercommon/roleactivity/raluckyblindboxparam.hpp"
#include "servercommon/roleactivity/raduanyangxiangnangparam.hpp"
#include "servercommon/roleactivity/rasanbaichouparam.hpp"
#include "servercommon/roleactivity/rachongjifanliparam.hpp"
#include "servercommon/roleactivity/raxinfuqianghuaparam.hpp"
#include "servercommon/roleactivity/racharismatictanabataparam.hpp"
#include "servercommon/roleactivity/raqixitehuiparam.hpp"
#include "servercommon/roleactivity/raxinfulingchongparam.hpp"
#include "servercommon/roleactivity/rahunqifanliparam.hpp"
#include "servercommon/roleactivity/randactivityshencihuafuparam.hpp"
#include "servercommon/roleactivity/randactivityzhuiyueshangdianparam.hpp"
#include "servercommon/roleactivity/rayuexidengmiparam.hpp"
#include "servercommon/roleactivity/radanbifanliparam.hpp"

unsigned int GetRandActivityRoleDataLengthByType(int rand_activity_type)
{
	unsigned int length = 0;
	switch (rand_activity_type)
	{
	case RAND_ACTIVITY_TYPE_SHAN_HAI_ZHENG_FENG:
		{
			length = sizeof(RASummerLiLianParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_MAZE_SUMMER:
		{
			length = sizeof(RAMazeSummerParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_YUAN_QI_TURNING:
		{
			length = sizeof(RAYuanQiTurningParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_QIONG_LOU_YU_YAN:
		{
			length = sizeof(RAQiongLouYuYanParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_PET_LOOT:
		{
			length = sizeof(RAPetLootParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_CUMULATIVE_RECHARGE:
		{
			length = sizeof(RACumulativeChargeParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_GIVE_SECRET_KEY:
		{
			length = sizeof(RAGiveSecretKeyParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_LAO_DONG_HAO_LI:
		{
			length = sizeof(RALaoDongHaoLiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_FORTUNE_MAZE:
		{
			length = sizeof(RAFortuneMazeParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_LITTLE_YEAR_GIFT:
		{
			length = sizeof(RALittleYearGiftParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_COW_LUCKY_EXCHANGE:
		{
			length = sizeof(RACowLuckyExchangeParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_COW_STORE:
		{
			length = sizeof(RACowStoreParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_COW_EXCHANGE:
		{
			length = sizeof(RACowExchangeParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_MONPOLY_GIFT:
		{
			length = sizeof(RAMonpolyGiftParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SPRING_FESTIVAL_STORE:
		{
			length = sizeof(RASpringFesticalStoreParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_NIU_XING_DA_YUN:
		{
			length = sizeof(RANiuXingDaYunParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_CI_JIU_YING_XIN:
		{
			length = sizeof(RACiJiuYingXinParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_TIME_LIMIT_STORE:
		{
			length = sizeof(RATimeLimitStoreParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_TRUN_TABLE:
		{
			length = sizeof(RATrunTableParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_DISCOUNT_STORE:
		{
			length = sizeof(RADiscountStoreParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SKY_DROP_LUXURY_GIFT:
		{
			length = 0;
		}
		break;
	case RAND_ACTIVITY_TYPE_CONSUME_CAROUSEL:
		{
			length = sizeof(RAConsumerCarouselParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_EXPEL_MONSTER:
		{
			length = sizeof(RAExpelMonsterParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_HERITAGE_TREASURE:
		{
			length = sizeof(RAHeritageTreasureParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SEVEN_COLOR_PICTURE:
		{
			length = sizeof(RASevenColorPictureParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SALE_SHOP:
		{
			length = sizeof(RASaleShopParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_HAPPY_UP:
		{
			length = sizeof(RAHappyUpParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_ANIMAL_RACE:
		{
			length = sizeof(RAAnimalRaceParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_LUCKY_BAG:
		{
			length = sizeof(RALuckyBagParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_REDUCE_TAX_RATE:
		{
			length = sizeof(RAReduceTaxRateParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_TIAN_JIANG_HAO_LI:
		{
			length = sizeof(RATianJiangHaoLiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SEAL_PROMOTE:
		{
			length = sizeof(RASealPromoteParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_DROP_DOUBLE:
		{
			length = sizeof(RADropDoubleParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_JOYSEEKSFAVORS:
		{
			length = sizeof(RAJoySeeksFavorsParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_COLLECT_WORD:
		{
			length = sizeof(RACollectWordParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_DISCOUNT_CARNIVAL:
		{
			length = sizeof(RADiscountCarnivalParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_PARTNER_EXCHANGE:
		{
			length = sizeof(RAPartnerExchangeParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_LUCKY_DISCOUNT:
		{
			length = sizeof(RALuckyDiscountParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_PET_EXCHANGE:
		{
			length = sizeof(RAPetExchangeParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_JIAN_DAN_QIN_XIN:
		{
			length = sizeof(RAJianDanQinXinParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_ADVENTURE_GIFT:
		{
			length = sizeof(RAAdventureGiftParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_LINKAGE_GIFT:
		{
			length = sizeof(RALinkageGiftParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_LUCKY_TURNTABLE:
		{
			length = sizeof(RALuckyTurnTableParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_TIAN_MING_DIVINATION:
		{
			length = sizeof(RATianMingDivinationParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_QUAN_MIN_DISCOUNT:
		{
			length = sizeof(RAQuanMinDiscountParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SPORTS:
		{
			length = sizeof(RASportsParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_TU_JIAN_TREASURE:
		{
			length = sizeof(RATuJianTreasureParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_GOD_TOKEN:
		{
			length = sizeof(RAGodTokenParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_ELEMENT_CARDS:
		{
			length = sizeof(RAElementCardsParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SIGN_IN_CONTINUITY:
		{
			length = sizeof(RASignInContinuityParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_CAI_SHEN_JU_BAO:
		{
			length = sizeof(RACaiShenJuBaoParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG:
		{
			length = sizeof(RADingZhiBaoXiangParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_ZHUAN_SHU_LI_BAO:
		{
			length = sizeof(RAZhuanShuLiBaoParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_JING_YAN_BIAO_SHENG:
		{
			length = sizeof(RAJingYanBiaoShengParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE:
		{
			length = sizeof(RAXianShiBaoHeParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI:
		{
			length = sizeof(RAXianShiBaoDaiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_GIVE_PET_DRAW:
		{
			length = sizeof(RAGivePetDrawParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_TRADEMARKET_GOLD_RETURN:
		{
			length = sizeof(RATradeMarketReturnGoldParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SHEN_SHOU_JIANG_LIN:
		{
			length = sizeof(RAShenShouJiangLinParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_GOOD_GIFT_TURN:
		{
			length = sizeof(RAGoodGiftTurnParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SPECIAL_SECRET_TREASURE:
		{
			length = sizeof(RASecretTreasureParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_HELP_OTHERS:
		{
			length = sizeof(RAHelpOthersParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_CHUN_RI_DENG_FENG:
		{
			length = sizeof(RAChunRiDengFengParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_ZU_HE_HE_LI:
		{
			length = sizeof(RAZuHeHeLiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI:
		{
			length = sizeof(RALingPaiTianCiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_FASHION_SHOP:
		{
			length = sizeof(RAFashionShopParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_ONE_YUAN_GOU:
		{
			length = sizeof(RAOneYuanGouParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_LING_YU_JI_JIN:
		{
			length = sizeof(RALingYuJinJiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI_2:
		{
			length = sizeof(RALingPaiTianCi2Param);
		}
		break;
	case RAND_ACTIVITY_TYPE_SUMMER_LI_LIAN:
		{
			length = sizeof(RASummerLiLianParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_COLORFUL_LANTERN:
		{
			length = sizeof(RAColorfulLanternParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SUMMER_STORE:
		{
			length = sizeof(RASummerStoreParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_LUCKY_SIGN:	
		{
			length = sizeof(RALuckySignParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SUPER_OFF_SHOP:
		{
			length = sizeof(RASuperOffShopParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_COOL_BREEZE:
		{
			length = sizeof(RACoolBreezeParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_LOTUS_MI_BAO:
		{
			length = sizeof(RALotusMiBao);
		}
		break;
	case RAND_ACTIVITY_TYPE_SHAN_HAI_MI_BAO:
		{
			length = sizeof(RALotusMiBao);
		}
		break;
	case RAND_ACTIVITY_TYPE_MI_BAO_FANG_DIAN:
		{
			length = sizeof(RAMiBaoFangDianParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_ZHEN_PIN_SHANG_PU:
		{
			length = sizeof(RAZhenPinShangPuParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_PAI_MAI_HANG:
		{
			length = sizeof(RARolePaiMaiHangParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_LING_PAI_TIAN_CI_3:
		{
			length = sizeof(RALingPaiTianCi2Param);
		}
		break;
	case RAND_ACTIVITY_TYPE_BACK_REWARD:
		{
			length = sizeof(RABackRewardParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SERVER_COMPETITION:
		{
			length = sizeof(RARoleServerCompetitionParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_HAPPY_LOOK:
		{
			length = sizeof(RAHappyLookParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SIGN_HAPPY:
		{
			length = sizeof(RASignHappyParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_WAN_LING_GONG_YING:
		{
			length = sizeof(RAWanLingGongYingParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_HELP_OTHERS_2:
		{
			length = sizeof(RAHelpOthersParam2);
		}
		break;
	case RAND_ACTIVITY_TYPE_HORCRUX_TIANCHENG:
		{
			length = sizeof(RAHorcruxTianChengParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_INTEGRAL_DRAW:
		{
			length = sizeof(RAIntegralDrawParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SHANHAI_QUESTION:
		{
			length = sizeof(RAShanHaiQuestionParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SHAN_HAI_JU_BAO:
		{
			length = sizeof(RAShanHaiJuBaoParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SHAN_HAI_MET:
		{
			length = sizeof(RAShanHaiMetParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SHAN_HAI_MEET:
		{
			length = sizeof(RAShanHaiMeetParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_FATE_ENTANGLEMENT:
		{
			length = sizeof(RAFateEntanglementParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_ZHAOCAIJINBAO:
		{
			length = sizeof(RAZhaoCaiJinBaoParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_YUN_ZE_TERRITORY:
		{
			length = sizeof(RAYunZeTerritoryParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_YUN_ZE_BLOOMING_RECORD:
		{
			length = sizeof(RAYunZeBloomingRecordParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_ZAI_ZHAN_YUN_ZE:
		{
			length = sizeof(RAZaiZhanYunZeParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_YUN_ZE_SLOT_MACHINE:
		{
			length = sizeof(RAYunZeSlotMachineParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_MO_ZU_ZAI_NA_LI:
		{
			length = sizeof(RAMoZuZaiNaLiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_CHAOZHI_FENXIANG:
		{
			length = sizeof(RAChaoZhiFenXiangParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_YUN_ZE_ER_SAN_SHI:
		{
			length = sizeof(RAYunZeErSanShiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_COLORFUL_DOUBLE:
		{
			length = sizeof(RAColorfulDoubleParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SHANHAI_HUADENG:
		{
			length = sizeof(RAShanHaiHuaDengParam);
		}
		break;
			case RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_XIANG:
		{
			length = sizeof(RAShanHaiBaoXiangParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_HE:
		{
			length = sizeof(RAShanHaiBaoHeParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SHAN_HAI_BAO_DAI:
		{
			length = sizeof(RAShanHaiBaoDaiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_2:
		{
			length = sizeof(RADingZhiBaoXiang2Param);
		}
		break;
	case RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_2:
		{
			length = sizeof(RAXianShiBaoHe2Param);
		}
		break;
	case RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_DAI_2:
		{
			length = sizeof(RAXianShiBaoDai2Param);
		}
		break;
	case RAND_ACTIVITY_TYPE_JIXING_GAOZHAO:
		{
			length = sizeof(RAJiXingGaoZhaoParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_YUN_ZE_WISH:
		{
			length = sizeof(RAYunZeWishParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_YUN_ZE_HAO_LI:
		{
			length = sizeof(RAYunZeHaoLiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_YUN_ZE_CUTE_PET:
		{
			length = sizeof(RAYunZeCutePetParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_YUN_ZE_CATCH_ELVES:
		{
			length = sizeof(RAYunZeCatchElvesParam);
		}
	break;
	case RAND_ACTIVITY_TYPE_XIAN_SHI_LEI_CHONG:
		{
			length = sizeof(RAXianShiLeiChongParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_WEEKEND_GIFT:
		{
			length = sizeof(RoleRAWeekendGiftParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_CHAO_ZHI_GIFT:
		{
			length = sizeof(RoleRAChaoZhiGiftParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_TIGER_FISH_EVERY_YEAR:
		{
			length = sizeof(RATigerFishEveryYearParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_TIGER_FIND_SPRING:
		{
			length = sizeof(RATigerFindSpringParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_TIGER_TEASE_NEWYEAR:
		{
			length = sizeof(RATigerTeaseNewYearParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_TIGER_LUCKY_MONEY:
		{
			length = sizeof(RATigerLuckyMoneyParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_YINHU_WENSHI:
		{
			length = sizeof(RAYinHuWenShiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_CINIU_YINGXINHU:
		{
			length = sizeof(RACiNiuYingXinHuParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_HU_XING_DA_YUN:
		{
			length = sizeof(RAHuXingDaYunParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_YIN_HU_BAI_SUI:
		{
			length = sizeof(RAYinHuBaiSuiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_TIGER_AUSPICIOUS_SNOW:
		{
			length = sizeof(RATigerAuspiciousSnowParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_JING_JI_YUE_KA:
		{
			length = sizeof(RAJingJiYueKaParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_RONG_LIAN_YOU_LI:
		{
			length = sizeof(RARongLianYouLiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_QING_YUAN_SHI_ZHUANG:
		{
			length = sizeof(RAQingYuanShiZhuangParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_QING_YUAN_LEI_CHONG:
		{
			length = sizeof(RAQingYuanLeiChongParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_GOD_BEAST_ADVENT:
		{
			length = sizeof(RAGodBeastAdventParam);
		}
		break;
	case  RAND_ACTIVITY_TYPE_YUAN_QI_SPEED:
		{
			length = sizeof(RAYuanQiSpeedParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_YUAN_QI_HELP:
		{
			length = sizeof(RAYuanQiHelpParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_QIYUAN_KONGMINGDENG:
		{
			length = sizeof(RAQiYuanKongMingDengParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_XUNMENG_HUDIE:
		{
			length = sizeof(RAXunMengHuDieParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_BO_ZHONG_YOU_LI:
		{
			length = sizeof(RABoZhongYouLiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_YUNA_QI_FAN_PAI:
		{
			length = sizeof(RAYuanQiFanPaiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_WANLING_ZHULI:
		{
			length = sizeof(RAWanLingZhuLiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_BAO_ZI_LAI_LE:
		{
			length = sizeof(RABaoZiLaiLeParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_JIN_YI_TIAN_XIANG:
		{
			length = sizeof(RAJinYiTianXiangParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_MIAO_BI_SHENG_HUA:
		{
			length = sizeof(RAMiaoBiShengHuaParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_YUAN_QI_YAN_HUO:
		{
			length = sizeof(RAYuanQiYanHuoParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_YUAN_QI_JI_SHI_BU:
		{
			length = sizeof(RAYuanQiJiShiBuParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_CHENG_FENG_PO_LANG:
		{
			length = sizeof(RAChengFengPoLangParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_ZONG_XIANG_MAN_CHENG:
		{
			length = sizeof(RAZongXiangManChengParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SALTY_SWEET_BATTLE:
		{
			length = sizeof(RASaltySweetBattleParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_DUAN_YANG_XIANG_NANG:
		{
			length = sizeof(RADuanYangXiangNangParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_DUAN_YANG_ZI_XUAN:
		{
			length = sizeof(RADuanYangZiXuanParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_CANG_LONG_CI_FU:
		{
			length = sizeof(RACangLongCiFuParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_LUCKY_BLIND_BOX:
		{
			length = sizeof(RALuckyBlindBoxParam);
		}
		break;	
	case RAND_ACTIVITY_TYPE_LAN_TANG_CHU_MU:
		{
			length = sizeof(RALanTangChuMuParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_300_CHOU:
		{
			length = sizeof(RASanBaiChouParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_DUAN_YANG_FU_LI:
		{
			length = sizeof(RADuanYangFuLiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_JIN_LONG_JU_BAO:
		{
			length = sizeof(RAJinLongJuBaoParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_CHONG_JI_FAN_LI:
		{
			length = sizeof(RAChongJiFanLiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SHEN_SHOU_EN_ZE:
		{
			length = sizeof(RAShenShouEnZeParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_XIN_FU_QIANG_HUA:
		{
			length = sizeof(RAXinFuQiangHuaParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_QI_XI_TE_HUI:
		{
			length = sizeof(RAQiXiTeHuiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_CHARISMATIC_TANABATA:
		{
			length = sizeof(RACharismaticTanabataParam);
		}
		break;	
	case RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_3:
		{
			length = sizeof(RAXianShiBaoHe3Param);
		}
		break;
	case RAND_ACTIVITY_TYPE_XIAN_SHI_BAO_HE_4:
		{
			length = sizeof(RAXianShiBaoHe4Param);
		}
		break;
	case RAND_ACTIVITY_TYPE_DING_ZHI_BAO_XIANG_3:
		{
			length = sizeof(RADingZhiBaoXiang3Param);
		}
		break;
	case RAND_ACTIVITY_TYPE_XIN_FU_LING_CHONG:
		{
			length = sizeof(RAXinFuLingChongParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_HUN_QI_FAN_LI:
		{
			length = sizeof(RAHunQiFanLiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_SHEN_CI_HUA_FU:
		{
			length = sizeof(RAShenCiHuaFuParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_TIAN_YI_YAO_SHI:
		{
			length = sizeof(RATianYiYaoShiParam);
		}
		break;	
	case RAND_ACTIVITY_TYPE_YUE_XI_DENG_MI:
		{
			length = sizeof(RAYueXiDengMiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_YUE_XI_LEI_CHONG:
		{
			length = sizeof(RAYueXiLeiChongParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_DAN_BI_FAN_LI:
		{
			length = sizeof(RADanBiFanLiParam);
		}
		break;
	case RAND_ACTIVITY_TYPE_ZHUI_YUE_SHANG_DIAN:
		{
			length = sizeof(RAZhuiYueShangDianParam);
		}
	break;
	default:
		{
			length = 0;
		}
		break;
	}

	UNSTD_STATIC_CHECK(178 == RAND_ACTIVITY_TYPE_END - RAND_ACTIVITY_TYPE_BEGIN);

	return length;
}

bool RandActivityRoleParamList::Serialize(TLVSerializer &outstream) const
{
	bool ret = outstream.Push(count);
	if (!ret)
	{
		return false;
	}

	for (int i = 0; i < count; ++i)
	{
		const DBRandActivityRoleDataItem & rank_item = data_list[i];

		ret = outstream.Push(rank_item.change_state) && outstream.Push(rank_item.activity_type) ;
		if (!ret)
		{
			return false;
		}

		if (rank_item.change_state == structcommon::CS_DELETE)
		{
			continue;
		}

		unsigned int length = GetRandActivityRoleDataLengthByType(rank_item.activity_type);
		if (length > 0)
		{
			TLVSerializer role_data;
			role_data.Reset((void *)&rank_item.data, sizeof(rank_item.data));
			role_data.MoveCurPos(length);

			ret = outstream.Push(role_data);

			if (!ret)
			{
				return false;
			}
		}
	}

	return true;
}

bool RandActivityRoleParamList::Unserialize(TLVUnserializer &instream)
{
	bool ret = instream.Pop(&count);
	if (!ret) return false;

	for (int i = 0; i < count; ++i)
	{
		DBRandActivityRoleDataItem & rank_item = data_list[i];
		ret = instream.Pop(&rank_item.change_state) && instream.Pop(&rank_item.activity_type) ;

		if (!ret)
		{
			return false;
		}

		if (rank_item.change_state == structcommon::CS_DELETE)
		{
			continue;
		}

		unsigned int length = GetRandActivityRoleDataLengthByType(rank_item.activity_type);
		if (length > 0)
		{
			TLVUnserializer role_data;
			ret = instream.Pop(&role_data);

			if (role_data.Size() <= sizeof(rank_item.data))
			{
				memcpy(&rank_item.data, role_data.Ptr(), role_data.Size());
			}

			if (!ret)
			{
				return false;
			}
		}
	}

	return true;
}
