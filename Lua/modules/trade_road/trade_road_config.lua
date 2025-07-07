TradeRoadConfig = {
    ResPath = "trade_road/",
    PngPath = "trade_road/npc%s.png",

    BagIcon = 2,
    SupplyIcon = 1,

    --丝绸之路请求 同服务端 SILK_ROADS_REQ_OP_TYPE
    REQ_OP_TYPE = {
        TYPE_INFO = 0,                  --基础信息
        TYPE_READY = 1,                 --准备，先随机生成数据
        TYPE_START_OUTSET = 2,          --开始出发
        TYPE_KEEP_GOING = 3,            --继续出发
        TYPE_BUY_COMMODITY = 4,         --购买货物  p1:货物seq  p2:count
        TYPE_SALE_COMMODITY = 5,        --售卖货物  p1,货物seq  p2:count
        TYPE_BUY_NEWS = 6,              --购买消息
        TYPE_EXPANSION_WAREHOUSE = 7,   --扩充仓库  p1:扩充次数
        TYPE_BUY_SUPPLY = 8,            --购买补给  p1:购买次数
        TYPE_END = 9,                   --结束贸易之旅
        TYPE_RANK_ALL_INFO = 10,        --请求排行信息
        TYPE_ALL_TRADING_INFO = 11,     --请求贸易消息列表
    },
    --简易信息下发 同服务端 SILK_ROADS_SMIPLE_TYPE
    SMIPLE_TYPE = {
        TYPE_READY = 0,                 --随机技能  p1:skill_id p2:line_group_id
        TYPE_START_OUTSET = 1,          --开始出发  p1:cur_city_index
        TYPE_KEEP_GOING = 2,            --继续出发  p1:cur_city_index p2:news_pool_count
        TYPE_BUY_NEWS = 3,              --购买消息  p1:buy_news_count p2:news_pool_count
        TYPE_EXPANSION_WAREHOUSE = 4,   --扩充仓库  p1:buy_warehouse_count
        TYPE_BUY_SUPPLY = 5,            --购买补给  p1:buy_supply_count
        TYPE_END = 6,                   --结束贸易之旅  p1:have_coin_count
    },

    --贸易日志类型 同服务端 SILK_ROADS_LOG_TYPE
    LOG_TYPE = {
        TYPE_BEGIN = 0,
    },
}

--仓库变化
SILK_ROADS_NOTICE_TYPE = {
    TYPE_DEFAULT = 0,
    TYPE_BUY = 1,       --购买货物
    TYPE_SALE = 2,      --售卖货物
    TYPE_REWARD = 3,    --奖励
}

--日志原因
SILK_ROADS_LOG_TYPE = {
    TYPE_BEGIN = 0,
    TYPE_PRICE_UP = 1,  --普通涨价
    TYPE_DOUBLE_UP = 2, --大涨

}

