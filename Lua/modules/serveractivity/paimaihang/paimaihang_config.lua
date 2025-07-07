PaiMaiHangConfig = {
    MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_NUM = 10,    --每日最多商品数量
    MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_CUR_NUM = 4, --当前每天最多商品数量
    MAX_PAI_MAI_HANG_RECORD_NUM = 30,           --竞拍记录数量

    RA_PAI_MAI_HANG_BID_STATUS = {
        TYPE_NOT_START = 0, --未开始
        TYPE_BIDDING = 1,   --进行中
        TYPE_END = 2,       --已结束
    },

    --请求3000协议
    RA_PAI_MAI_HANG_REQ_TYPE = {
        TYPE_INFO = 0,          --请求角色信息
        TYPE_BID = 1,           --请求竞拍  p1:seq  p2:bid_price
        TYPE_AUTO_BID = 2,      --请求自动竞拍  p1:seq p2:bid_price p3:add_price(此处固定是自动竞价)
        TYPE_CANCEL_AUTO = 3,   --请求取消自动竞拍  p1:seq
        TYPE_RETRIEVE = 4,      --请求取回寄存元宝  p1:seq p2:retrieve_gold p3:add_price
        TYPE_SET_SHOW_FLAG = 5, --请求设置信息展示状态 p1:seq p2: 0-不展示
        TYPE_RECORD_INFO = 6,   --请求记录信息
        TYPE_CHANGE_ADD_PRICE = 7, --请求修改自动竞价元宝 p1:seq p2:add_price
    },

    --记录类型
    RA_PAI_MAI_HANG_RECORD_TYPE = {
        TYPE_BID = 0,           --出价竞拍  p1:bid_price
        TYPE_BID_FAILD = 1,     --竞拍失败
        TYPE_AUTO_BID = 2,      --自动竞拍  p1:bid_price    p2:remain_deposit_gold
        TYPE_DEPOSIT_SUCC = 3,  --寄存成功  p1:add_deposit_gold
        TYPE_FETCH_DEPOSIT = 4, --取回寄存  p1:retrieve_gold
    }
}
