TradeMarketStruct = TradeMarketStruct or {}

function TradeMarketStruct.TradeMarketRead()
    local stu = {}
    stu.next_status_time = MsgAdapter.ReadUInt()--公示剩余时间戳
    stu.cur_status = MsgAdapter.ReadShort()     --当前状态 1上架 2公示 3开售 4下架
    stu.price_type = MsgAdapter.ReadShort()     --货币类型
    stu.price = MsgAdapter.ReadInt()            --价格
    stu.item_data = TradeMarketStruct.ItemDataRoad()
    stu.seller_uid = MsgAdapter.ReadInt()       --卖家uid
    stu.sale_index = MsgAdapter.ReadShort()     --商品在卖家货架上的序号
    stu.focus_index = MsgAdapter.ReadShort()    --商品在我的关注列表中的序号
    stu.upshelve_time = MsgAdapter.ReadUInt()   --物品上架时间
    stu.unique_key = MsgAdapter.ReadStrN(TRADE_UNIQUE_KEY_MAX)--唯一标识
    return stu
end

function TradeMarketStruct.TransactionItemInfo()
    local stu = {}
    stu.item_id = MsgAdapter.ReadUShort()
    stu.sold_num = MsgAdapter.ReadShort()
    stu.num = stu.sold_num
    stu.price_type = MsgAdapter.ReadShort()
    stu.param_length = MsgAdapter.ReadShort()
    local price = MsgAdapter.ReadInt()
    stu.price = price * stu.sold_num 
    if stu.param_length > 0 then
        stu.param = ItemDataParam(stu.item_id,stu.param_length)
        if stu.param == nil or type(stu.param) == "string"  then
            stu.param = {}
        end
        stu.param.item_id = stu.item_id
        stu.param.num = stu.sold_num
    else
        stu.param = {
            item_id = stu.item_id,
            num = stu.sold_num
        }
    end
    return stu
end

function TradeMarketStruct.ItemDataRoad()
	local stu = {}
	stu.item_id = MsgAdapter.ReadUShort()
	stu.num = MsgAdapter.ReadShort()
	stu.invalid_time = MsgAdapter.ReadUInt()
    stu.param_length = MsgAdapter.ReadShort()
    if stu.param_length > 0 then
        stu.param = ItemDataParam(stu.item_id,stu.param_length)
    end
    stu.is_bind = false
	return stu
end

--交易所请求 上架货物
CSAddItemOnShelve = CSAddItemOnShelve or BaseClass(BaseProtocolStruct)
function CSAddItemOnShelve:__init()
    self.msg_type = 2301
end

function CSAddItemOnShelve:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteChar(self.is_from_pet)--是否是宠物
    MsgAdapter.WriteChar(0) --保留字
    MsgAdapter.WriteShort(self.index)     --序号（当为宠物时，是货物在宠物背包的序号，否则是货物在背包的序号）
    MsgAdapter.WriteShort(self.column)    --物品栏号（仅当货物是道具时有效，表示货物在背包中的哪个物品栏）
    MsgAdapter.WriteShort(self.item_num)  --上架货物的数量
    MsgAdapter.WriteInt(self.price)       --单价
end

--交易所请求 下架货物
CSRemoveItemOnShelve = CSRemoveItemOnShelve or BaseClass(BaseProtocolStruct)
function CSRemoveItemOnShelve:__init()
    self.msg_type = 2302
end

function CSRemoveItemOnShelve:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.sale_index)--货物在货架上的序号 [0,8) 
end

--交易所请求 货物出售
CSTradeMarketCashOut = CSTradeMarketCashOut or BaseClass(BaseProtocolStruct)
function CSTradeMarketCashOut:__init()
    self.msg_type = 2303
end

function CSTradeMarketCashOut:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.sale_index)--货物在货架上的序号 [0,8) 
end

--交易所请求 玩家个人交易所数据请求
CSTradeMarketUserShelveData = CSTradeMarketUserShelveData or BaseClass(BaseProtocolStruct)
function CSTradeMarketUserShelveData:__init()
    self.msg_type = 2304
end

function CSTradeMarketUserShelveData:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

--按物品ID搜索 数据请求
CSTradeMarketSearchByItemID = CSTradeMarketSearchByItemID or BaseClass(BaseProtocolStruct)
function CSTradeMarketSearchByItemID:__init()
    self.msg_type = 2305
end

function CSTradeMarketSearchByItemID:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteUShort(self.item_id) -- 物品id
    MsgAdapter.WriteShort(self.req_page) -- 请求的页数 
    MsgAdapter.WriteShort(self.is_ammounce)
    MsgAdapter.WriteShort(self.order_type) --排序类型 0 按价格从低到高 1按价格从高到低
end

--按物品子类型搜索 数据请求
CSTradeMarketSearchByItemSubType = CSTradeMarketSearchByItemSubType or BaseClass(BaseProtocolStruct)
function CSTradeMarketSearchByItemSubType:__init()
    self.msg_type = 2306
end

function CSTradeMarketSearchByItemSubType:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.item_sub_type)  -- 物品子类型
    MsgAdapter.WriteShort(self.req_page)       -- 请求的页数 
    MsgAdapter.WriteShort(self.is_ammounce)
    MsgAdapter.WriteShort(self.order_type) --排序类型 0 按价格从低到高 1按价格从高到低
    MsgAdapter.WriteShort(self.item_type)
    MsgAdapter.WriteShort(self.level_param)
end

--购买物品
CSTradeMarketBuyItem = CSTradeMarketBuyItem or BaseClass(BaseProtocolStruct)
function CSTradeMarketBuyItem:__init()
    self.msg_type = 2307
end

function CSTradeMarketBuyItem:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.seller_uid)
    MsgAdapter.WriteShort(self.sale_index)
    MsgAdapter.WriteShort(self.buy_num)
    MsgAdapter.WriteUInt(self.upshelve_time)
    MsgAdapter.WriteUShort(self.buy_item_id)
    MsgAdapter.WriteShort(0)
    MsgAdapter.WriteInt(self.expect_price)
    MsgAdapter.WriteStrN(self.unique_key,TRADE_UNIQUE_KEY_MAX)
end

-- 玩家交易所个人数据
SCTradeMarketUserShelveData = SCTradeMarketUserShelveData or BaseClass(BaseProtocolStruct)
function SCTradeMarketUserShelveData:__init()
    self.msg_type = 2351
end

function SCTradeMarketUserShelveData:Decode()
    self.item_list = {}
    self.item_info = TradeData.Instance:GetSellList()
    self.item_index = 0
    local reason = MsgAdapter.ReadShort()--原因 0 整个货架信息下发 1 状态改变通知 2 新增 3 删除
    local item_count = MsgAdapter.ReadShort()
    for i=1, item_count do
        local info = {}
        info.sale_index = MsgAdapter.ReadShort()
        info.cur_status = MsgAdapter.ReadShort() --当前状态 1上架 2公示 3开售 4下架
        info.next_status_time = MsgAdapter.ReadUInt()--公示剩余时间戳
        info.sold_num = MsgAdapter.ReadShort()
        info.price_type = MsgAdapter.ReadShort()
        info.price = MsgAdapter.ReadInt()
        info.item_data = TradeMarketStruct.ItemDataRoad()
        self.item_info[info.sale_index] = reason ~= 3 and info or nil
    end
    TradeData.Instance:SetSellList(self.item_info)
    for k,v in pairs(self.item_info) do
        if v ~= nil then
            table.insert(self.item_list,v)
        end
    end
    if #self.item_list >= 2 then
        table.sort(self.item_list,DataSorter.KeyLowerSorter("sale_index"))
    end
end

-- 玩家交易所个人数据
SCTradeMarketSearchResult = SCTradeMarketSearchResult or BaseClass(BaseProtocolStruct)
function SCTradeMarketSearchResult:__init()
    self.msg_type = 2352
end

function SCTradeMarketSearchResult:Decode()
    self.cursor_index = MsgAdapter.ReadShort()
    self.announce = MsgAdapter.ReadShort() + 1
    self.cur_page = MsgAdapter.ReadShort()
    self.total_page = MsgAdapter.ReadShort()
    self.order_type = MsgAdapter.ReadShort()
    local item_count = MsgAdapter.ReadShort()
    self.item_list = {}
    for i=1,item_count do
        local info = {}
        info.next_status_time = MsgAdapter.ReadUInt()--公示剩余时间戳
        info.cur_status = MsgAdapter.ReadShort()     --当前状态 1上架 2公示 3开售 4下架
        info.price_type = MsgAdapter.ReadShort()     --货币类型
        info.price = MsgAdapter.ReadInt()
        info.item_data = TradeMarketStruct.ItemDataRoad()
        info.seller_uid = MsgAdapter.ReadInt()    --卖家uid
        info.sale_index = MsgAdapter.ReadShort()  --商品在卖家货架上的序号
        info.upshelve_time = MsgAdapter.ReadUInt()--物品上架时间
        info.unique_key = MsgAdapter.ReadStrN(TRADE_UNIQUE_KEY_MAX) --唯一标识
        info.flush_index = MallConfig.TradeShop.Shop
        self.item_list[i] = info
    end
end

--交易记录
CSRecentTransactionRecords = CSRecentTransactionRecords or BaseClass(BaseProtocolStruct)
function CSRecentTransactionRecords:__init()
    self.msg_type = 2308
end

function CSRecentTransactionRecords:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteUShort(self.item_id)
    MsgAdapter.WriteShort(0)
end

--返回交易记录
SCRecentTransactionRecords = SCRecentTransactionRecords or BaseClass(BaseProtocolStruct)
function SCRecentTransactionRecords:__init()
    self.msg_type = 2353
end

function SCRecentTransactionRecords:Decode()
    self.record_list = {}
    for i=1,MsgAdapter.ReadInt() do
        self.record_list[i] = TradeMarketStruct.TransactionItemInfo()
    end
end

--查询关注列表
CSTradeMarketGetFocusList = CSTradeMarketGetFocusList or BaseClass(BaseProtocolStruct)
function CSTradeMarketGetFocusList:__init()
    self.msg_type = 2309
end

function CSTradeMarketGetFocusList:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

--获取关注列表
SCTradeMarketGetFocusList = SCTradeMarketGetFocusList or BaseClass(BaseProtocolStruct)
function SCTradeMarketGetFocusList:__init()
    self.msg_type = 2354
end

function SCTradeMarketGetFocusList:Decode()
    self.focus_list = {{},{}}
    local announce_num = MsgAdapter.ReadShort()
    local sale_num = MsgAdapter.ReadShort()
    for i=1,announce_num do
        local info = TradeMarketStruct.TradeMarketRead()
        info.flush_index = MallConfig.TradeShop.Shop
        table.insert(self.focus_list[2],info)
    end
    for i=1,sale_num do
        local info = TradeMarketStruct.TradeMarketRead()
        info.flush_index = MallConfig.TradeShop.Shop
        table.insert(self.focus_list[1],info)
    end
end

--添加关注
CSTradeMarketAddFocus = CSTradeMarketAddFocus or BaseClass(BaseProtocolStruct)
function CSTradeMarketAddFocus:__init()
    self.msg_type = 2310
end

function CSTradeMarketAddFocus:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.seller_uid)    --卖家uid 
    MsgAdapter.WriteShort(self.sale_index)  --在卖家货架上的序号 
    MsgAdapter.WriteShort(0)
    MsgAdapter.WriteUInt(self.upshelve_time)--商品上架时间 （用于核对物品）
end

--取消关注
CSTradeMarketRemoveFocus = CSTradeMarketRemoveFocus or BaseClass(BaseProtocolStruct)
function CSTradeMarketRemoveFocus:__init()
    self.msg_type = 2311
end

function CSTradeMarketRemoveFocus:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.focus_index)
    MsgAdapter.WriteShort(self.cur_page)
    MsgAdapter.WriteShort(self.is_ammounce)
end

--关注
CSTradeMarketGetFocusItemInfo = CSTradeMarketGetFocusItemInfo or BaseClass(BaseProtocolStruct)
function CSTradeMarketGetFocusItemInfo:__init()
    self.msg_type = 2312
end

function CSTradeMarketGetFocusItemInfo:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.focus_index)
    MsgAdapter.WriteShort(self.cur_page)
    MsgAdapter.WriteShort(self.is_ammounce)
end

--关注单个物品下发
SCTradeMarketGetFocusItemInfo = SCTradeMarketGetFocusItemInfo or BaseClass(BaseProtocolStruct)
function SCTradeMarketGetFocusItemInfo:__init()
    self.msg_type = 2355
end

function SCTradeMarketGetFocusItemInfo:Decode()
    self.focus_info = TradeMarketStruct.TradeMarketRead()  
end

--发送广告
CSTradeMarketAdvertise = CSTradeMarketAdvertise or BaseClass(BaseProtocolStruct)
function CSTradeMarketAdvertise:__init()
    self.msg_type = 2313
end

function CSTradeMarketAdvertise:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.sale_index)
    MsgAdapter.WriteShort(0)
end

--查找广告上的商品
CSTradeMarketSearchAdvertise = CSTradeMarketSearchAdvertise or BaseClass(BaseProtocolStruct)
function CSTradeMarketSearchAdvertise:__init()
    self.msg_type = 2314
end

function CSTradeMarketSearchAdvertise:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.seller_uid)
    MsgAdapter.WriteUInt(self.put_on_shelve_time)
    MsgAdapter.WriteInt(self.price)
    MsgAdapter.WriteShort(self.sale_index)
    MsgAdapter.WriteUShort(self.item_id)
    MsgAdapter.WriteStrN(self.unique_key,TRADE_UNIQUE_KEY_MAX)
end

--查询正在售卖的某物品
CSTradeMarketGetSellingList = CSTradeMarketGetSellingList or BaseClass(BaseProtocolStruct)
function CSTradeMarketGetSellingList:__init()
    self.msg_type = 2315
end

function CSTradeMarketGetSellingList:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteUShort(self.item_id)
    MsgAdapter.WriteShort(self.special_type)
end

--返回正在售卖的某物品
SCTradeMarketGetSellingList = SCTradeMarketGetSellingList or BaseClass(BaseProtocolStruct)
function SCTradeMarketGetSellingList:__init()
    self.msg_type = 2356
end

function SCTradeMarketGetSellingList:Decode()
    self.record_list = {}
    for i=1,MsgAdapter.ReadInt() do
        self.record_list[i] = TradeMarketStruct.TransactionItemInfo()
    end
end

--请求玩家交易记录
CSTradeMarketGetRoleTransactionRecord = CSTradeMarketGetRoleTransactionRecord or BaseClass(BaseProtocolStruct)
function CSTradeMarketGetRoleTransactionRecord:__init()
    self.msg_type = 2316
end

function CSTradeMarketGetRoleTransactionRecord:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

--返回玩家交易记录
SCTradeMarketGetRoleTransactionRecord = SCTradeMarketGetRoleTransactionRecord or BaseClass(BaseProtocolStruct)
function SCTradeMarketGetRoleTransactionRecord:__init()
    self.msg_type = 2357
end

function SCTradeMarketGetRoleTransactionRecord:Decode()
    self.sold_list,self.buy_list = {},{}
    local sold_num = MsgAdapter.ReadShort()
    local buy_num = MsgAdapter.ReadShort()
    for i=1,sold_num do
        self.sold_list[i] = TradeMarketStruct.TransactionItemInfo()
    end
    for i=1,buy_num do
        self.buy_list[i] = TradeMarketStruct.TransactionItemInfo()
    end
    self.gold_list = {}
    for i=1,2 do
        self.gold_list[i] = MsgAdapter.ReadLL()
    end
end

--重新上架
CSTradeMarketRedoAddItemOnShelve = CSTradeMarketRedoAddItemOnShelve or BaseClass(BaseProtocolStruct)
function CSTradeMarketRedoAddItemOnShelve:__init()
    self.msg_type = 2317
end

function CSTradeMarketRedoAddItemOnShelve:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.sale_index)
    MsgAdapter.WriteShort(0)
    MsgAdapter.WriteInt(self.price)
end

--宠物快捷购买搜索请求
CSTradeMarketSearchPetForQuickBuy = CSTradeMarketSearchPetForQuickBuy or BaseClass(BaseProtocolStruct)
function CSTradeMarketSearchPetForQuickBuy:__init()
    self.msg_type = 2318
end

function CSTradeMarketSearchPetForQuickBuy:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.order_type)   --筛选顺序 TRADE_PET_INDEX_ORDER
    MsgAdapter.WriteUShort(self.item_id)     --要搜的宠物物品id
    MsgAdapter.WriteInt(self.last_seller_uid)--上次搜到的最后一件商品的卖家UID （第一次搜索或更换筛选顺序时发0）
    MsgAdapter.WriteInt(self.last_price)     --上次搜到的最后一件商品的价格	 （第一次搜索或更换筛选顺序时发0）
    MsgAdapter.WriteUInt(self.last_put_on_shelve_time)--上次搜到的最后一件商品的上架时间（第一次搜索或更换筛选顺序时发0）
    MsgAdapter.WriteShort(self.last_str_level)--上次搜到的最后一件商品的强化等级（第一次搜索或更换筛选顺序时发0）
    MsgAdapter.WriteShort(self.quality)
end

local function SaleItemInfo()
    return {
        next_status_time = MsgAdapter.ReadUInt(),
        cur_status = MsgAdapter.ReadShort(),
        price_type = MsgAdapter.ReadShort(),
        price = MsgAdapter.ReadInt(),
        item_data = TradeMarketStruct.ItemDataRoad(),
        seller_uid = MsgAdapter.ReadInt(),
        sale_index = MsgAdapter.ReadShort(),
        upshelve_time = MsgAdapter.ReadUInt(),
        unique_key = MsgAdapter.ReadStrN(TRADE_UNIQUE_KEY_MAX)--唯一标识
    }
end

SCTradeMarketSearchPetForQuickBuy = SCTradeMarketSearchPetForQuickBuy or BaseClass(BaseProtocolStruct)
function SCTradeMarketSearchPetForQuickBuy:__init()
    self.msg_type = 2358
end

function SCTradeMarketSearchPetForQuickBuy:Decode()
    self.order_type = MsgAdapter.ReadShort()
    self.item_count = MsgAdapter.ReadShort()
    self.item_list = {}
    for index = 1, self.item_count do
        self.item_list[index] = SaleItemInfo()
    end
end

SCTradeMarketRefreshNotice = SCTradeMarketRefreshNotice or BaseClass(BaseProtocolStruct)
function SCTradeMarketRefreshNotice:__init()
    self.msg_type = 2359
end

function SCTradeMarketRefreshNotice:Decode()
end

--请求每个大类中所有小类目前有多少商品
CSTradeMarketGetMerchandiseNum = CSTradeMarketGetMerchandiseNum or BaseClass(BaseProtocolStruct)
function CSTradeMarketGetMerchandiseNum:__init()
    self.msg_type = 2319
end

function CSTradeMarketGetMerchandiseNum:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.is_announce)    --0:发售期   1:公示期
    MsgAdapter.WriteShort(self.big_search_type)--大类  1：武器	2：防具  3：宠物  10：灵饰
end

--返回每个大类中所有小类目前有多少商品
SCTradeMarketGetMerchandiseNum = SCTradeMarketGetMerchandiseNum or BaseClass(BaseProtocolStruct)
function SCTradeMarketGetMerchandiseNum:__init()
    self.msg_type = 2360
end

function SCTradeMarketGetMerchandiseNum:Decode()
    self.is_announce = MsgAdapter.ReadShort()    --0:发售期  1:公示期
    self.big_search_type = MsgAdapter.ReadShort()--大类  原样返回
    self.info_list = {}
    for i=1,MsgAdapter.ReadInt()do
        self.info_list[MsgAdapter.ReadInt()] = MsgAdapter.ReadInt()--小类 商品数量
    end
end

--交易信用
SCTradeMarketCreditInfo = SCTradeMarketCreditInfo or BaseClass(BaseProtocolStruct)
function SCTradeMarketCreditInfo:__init()
    self.msg_type = 2361
end

function SCTradeMarketCreditInfo:Decode()
    self.total_credit = MsgAdapter.ReadLL() --玩家当前拥有的信用值
end

--令牌搜索
CSTradeMarketSearchByMedalEffectType = CSTradeMarketSearchByMedalEffectType or BaseClass(BaseProtocolStruct)
function CSTradeMarketSearchByMedalEffectType:__init()
    self.msg_type = 2320
end

function CSTradeMarketSearchByMedalEffectType:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.special_type)
    MsgAdapter.WriteShort(self.color)
    MsgAdapter.WriteShort(0)
    MsgAdapter.WriteShort(self.req_page)   -- 请求的页数 
    MsgAdapter.WriteShort(self.is_ammounce)
    MsgAdapter.WriteShort(self.order_type) --排序类型 0 按价格从低到高 1按价格从高到低
end

--查询物品的推荐价格
CSQueryItemRecommendPrice = CSQueryItemRecommendPrice or BaseClass(BaseProtocolStruct)
function CSQueryItemRecommendPrice:__init()
    self.msg_type = 2321
end

function CSQueryItemRecommendPrice:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteChar(self.is_from_pet)--是否是宠物
    MsgAdapter.WriteChar(0) --保留字
    MsgAdapter.WriteShort(0)
    MsgAdapter.WriteShort(self.index)     --序号（当为宠物时，是货物在宠物背包的序号，否则是货物在背包的序号）
    MsgAdapter.WriteShort(self.column)    --物品栏号（仅当货物是道具时有效，表示货物在背包中的哪个物品栏）
    LogError(self)
end

--物品推荐价格下发
SCQueryItemRecommendPriceResp = SCQueryItemRecommendPriceResp or BaseClass(BaseProtocolStruct)
function SCQueryItemRecommendPriceResp:__init()
    self.msg_type = 2362
end

function SCQueryItemRecommendPriceResp:Decode()
    local info = {}
    info.price = MsgAdapter.ReadInt()    --推荐价格
    info.min_price = MsgAdapter.ReadInt()--最低价格
    info.max_price = MsgAdapter.ReadInt()--最高价格
    info.is_from_pet = MsgAdapter.ReadChar()--是否宠物 0 否 1 是
    MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()
    info.index = MsgAdapter.ReadShort()    --序号（当为宠物时，是货物在宠物背包的序号，否则是货物在背包的序号） 
    info.column = MsgAdapter.ReadShort()   --物品栏号（仅当货物是道具时有效，表示货物在背包中的哪个物品栏）
    self.info = info
end