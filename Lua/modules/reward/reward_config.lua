
RewardConfig = {
    ResPath = "reward/",
    DAY_ACTIVITY_TYPE_MAX = 20,
    ANTI_FRAUD_NUM_MAX = 4,
    BackType = {coin = 1,gold = 2},
    OperType = {
        online = 0,             --在线奖励
        daily_sign_in = 1,      --签到奖励
        upgrade = 2,            --等级奖励
        score = 3,              --评分奖励
		lunchsupper = 5,        --领取午餐晚餐
    },
    --月卡类型
    MonthCardType = {
        TYPE_WEEK = 0,  --周卡
        TYPE_MONTH = 1, --月卡
    },
    MonthCardReqType = {
        TYPE_INFO = 0,  --请求信息下发
        TYPE_BUY_ITEM = 1, --购买商城商品 p1:seq(配置)
        TYPE_BUY_CMD_CHECK = 2, --购买直购道具前检测 p1:seq p2:版号
        TYPE_BUY_CARD_CHECK = 3, --购买周卡/月卡前检测 p1:card_type p2:版号
        TYPE_FETCH_DAY_REWARD = 4, --领取每日奖励 p1:card_type
    },
    MonthCardPriceType = {
        [0] = HuoBi[CurrencyType.Gold],
        [1] = HuoBi[CurrencyType.ImmortalCoin],
        [2] = HuoBi[CurrencyType.CoinBind],
        [3] = HuoBi[CurrencyType.RenMinBi],
    },
	
	LunchSupperType = {
        [0] = HuoBi[CurrencyType.CoinBind],
        [1] = HuoBi[CurrencyType.Gold],
        [2] = HuoBi[CurrencyType.ImmortalCoin],
    },

}