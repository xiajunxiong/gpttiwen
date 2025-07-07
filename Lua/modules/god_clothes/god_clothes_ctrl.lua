VIEW_REQ("modules/god_clothes/god_clothes_view")
VIEW_REQ("modules/god_clothes/god_clothes_show")
VIEW_REQ("modules/god_clothes/god_clothes_exchange")
VIEW_REQ("modules/god_clothes/god_clothes_buy")
VIEW_REQ("modules/god_clothes/god_clothes_select")
VIEW_REQ("modules/god_clothes/god_clothes_result")
-- 神赐华服
GodClothesCtrl = GodClothesCtrl or BaseClass(BaseCtrl)

function GodClothesCtrl:__init()
    if GodClothesCtrl.Instance ~= nil then
        Debuger.LogError("[GodClothesCtrl] attempt to create singleton twice!")
        return
    end
    GodClothesCtrl.Instance = self
    self.data = GodClothesData.New()
end

function GodClothesCtrl:OnInit()
    self:RegisterRemind()
    self:RegisterAllProtocol()
end

function GodClothesCtrl:RegisterRemind()
    local remind_func = BindTool.Bind(self.FlushRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.RandActivity.GodClothes,
        self.data.info_data,
        remind_func
    )
    BagData.Instance:ListenItems(self.data:ExchangeItem(), function()
		self.data.info_data:Notify()
    end)
    BagData.Instance:ListenItems(self.data:LotteryItem(), function()
		self.data.info_data:Notify()
	end)
end

function GodClothesCtrl:RegisterAllProtocol()
    self:RegisterProtocol(SCRAShenCiHuaFuInfo, "OnSCRAShenCiHuaFuInfo")
    self:RegisterProtocol(SCRAShenCiHuaFuResultInfo, "OnSCRAShenCiHuaFuResultInfo")
end

function GodClothesCtrl:__delete()
    if GodClothesCtrl.Instance == self then
        GodClothesCtrl.Instance = nil
    end
end

function GodClothesCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function GodClothesCtrl:FlushRemind()
    local num = self.data:Remind()
    return num
end

-- 发送请求
function GodClothesCtrl:SendReq(req_type, param1, param2, param3)
    self:MyLogError(ColorStr("神赐华服发送请求", COLORSTR.Red8),req_type, param1, param2, param3)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_GOD_CLOTHES, req_type, param1, param2, param3)
end

function GodClothesCtrl:OnSCRAShenCiHuaFuInfo(protocol)
    self:MyLogError(ColorStr("神赐华服活动信息下发"..protocol.msg_type, COLORSTR.Red8), protocol)
    self.data:SetDataInfo(protocol)
end

function GodClothesCtrl:OnSCRAShenCiHuaFuResultInfo(protocol)
    self:MyLogError(ColorStr("神赐华服抽奖信息下发"..protocol.msg_type, COLORSTR.Red8), protocol)
    self.data:SetResultInfo(protocol)
end

function GodClothesCtrl:MyLogError(...)
    -- LogError(...)
end

-----------------------------GodClothesData-----------------------------
GodClothesData = GodClothesData or BaseClass()

-- 操作请求类型
GodClothesReqType = {
    Info = 0,       --请求下发
    MoneyBuy = 1,   --直购，p1：seq
    Buy = 2,        --非直购，p1：seq
    Select = 3,     --设置自选，p1~p3：所选seq
    Lottery = 4,    --抽奖，p1：次数
    Exchange = 5,   --兑换奖励，p1：seq
}

-- 道具类型
GodClothesItemType = {
    Select = 0,
    Rand = 1,
}

function GodClothesData:__init()
    if GodClothesData.Instance ~= nil then
        Debuger.LogError("[GodClothesData] attempt to create singleton twice!")
        return
    end
    GodClothesData.Instance = self
    self.config = Config.shencihuafu_auto
    self.other = self.config.other[1]
    self.info_data = SmartData.New()
    self.result_data = SmartData.New()
    
    self:ResetData()
end

function GodClothesData:__delete()
    GodClothesData.Instance = nil
end

function GodClothesData.Decode(item_type, index)
    local t = {}
    t.index = index
    t.item_type = item_type
    t.is_get = MsgAdapter.ReadUChar()   --是否获得
    t.is_select = MsgAdapter.ReadUChar()--是否选择
    MsgAdapter.ReadUShort()
    t.group = MsgAdapter.ReadInt()  --奖励组
    t.seq = MsgAdapter.ReadInt()    --奖励组里的seq
    return t
end

function GodClothesData:GetResetItem(item_type, index)
    local t = {}
    t.index = index
    t.item_type = item_type
    t.is_get = 0
    t.is_select = 0
    t.group = 0
    t.seq = 0
    return t
end

function GodClothesData:ResetData()
    local info = {}
    info.day_buy_times = {} --每天限购
    info.act_buy_times = {} --总限购
    info.day_exchange_times = {} --每天限兑
    info.act_exchange_times = {} --总限兑
    local item_select = {}	--自选奖励
	for i = 0, 2 do
		item_select[i] = self:GetResetItem(GodClothesItemType.Select, i)
	end
	info.item_select = item_select
	local item_rand = {}	--随机奖励
	for i = 0, 5 do
		item_rand[i] = self:GetResetItem(GodClothesItemType.Rand, i)
	end
	info.item_rand = item_rand
    self.info_data.info = info

    self.result_data.item_list = {}

    self.is_select = false
    self.can_lottery_times = 0
end

function GodClothesData:DayBuyTimes(seq)
    return self.info_data.info.day_buy_times[seq] or 0
end

function GodClothesData:ActBuyTimes(seq)
    return self.info_data.info.act_buy_times[seq] or 0
end

function GodClothesData:DayExchangeTimes(seq)
    return self.info_data.info.day_exchange_times[seq] or 0
end

function GodClothesData:ActExchangeTimes(seq)
    return self.info_data.info.act_exchange_times[seq] or 0
end

function GodClothesData:LotteryItem()
    return {self.other.consume_item}
end

function GodClothesData:LotteryItemId()
    return self.other.consume_item
end

function GodClothesData:ExchangeItem()
    local cfg = self:ExchangeCfg()
    return {cfg[1].recharge_item}
end

function GodClothesData:ExchangeItemId()
    local cfg = self:ExchangeCfg()
    return cfg[1].recharge_item
end

function GodClothesData:Remind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_GOD_CLOTHES) then
        return 0
    end
    -- 活动红点
    local act_start = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_GOD_CLOTHES)
	if tostring(act_start) ~= PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_GOD_CLOTHES)) then
		-- 活动首次红点，换设备重新提示
		return 1
    end
    -- 可兑换红点
    if self:AllExchangeRemind() > 0 then
        return 1
    end
    -- 可抽奖红点
    if self:LotteryRemind() > 0 then
        return 1
    end
    return 1
end

function GodClothesData:ClearFirstRemind()
    local act_start = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_GOD_CLOTHES)
	if tostring(act_start) ~= PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_GOD_CLOTHES)) then
        PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_GOD_CLOTHES), act_start)
        self.info_data:Notify()
	end
end

function GodClothesData:AllExchangeRemind()
    for k, v in pairs(self:ExchangeCfg()) do
        if self:ExchangeRemind(v) > 0 then
            return 1
        end
    end
    return 0
end

function GodClothesData:LotteryRemind(times)
    times = times or 1
    if self:IsSelect() then
        if self:CanLotteryTimes() >= times then
            local has_num = Item.GetNum(self:LotteryItemId())
            return has_num >= times and 1 or 0
        end
    end
    return 0
end

function GodClothesData:SetDataInfo(protocol)
    self.info_data.info = protocol.info
    self:Calculate()
end

function GodClothesData:SetResultInfo(protocol)
    self.result_data.item_list = protocol.item_list
    local list = {}
    for k, v in pairs(protocol.item_list) do
        if v.m_type == GodClothesItemType.Select then
            self.info_data.info.item_select[v.index].is_get = 1
            table.insert(list, self.info_data.info.item_select[v.index])
        else
            self.info_data.info.item_rand[v.index].is_get = 1
            table.insert(list, self.info_data.info.item_rand[v.index])
        end
    end
    ViewMgr:OpenView(GodClothesResultView, {data = list})
    self:Calculate()
    self.info_data:Notify()
end

function GodClothesData:Calculate()
    self.is_select = self.info_data.info.item_select[1].is_select == 1
    if self.is_select then
        local times = 0
        for k, v in pairs(self.info_data.info.item_select) do
            if v.is_get == 1 then
                times = times + 1
            end
        end
        for k, v in pairs(self.info_data.info.item_rand) do
            if v.is_get == 1 then
                times = times + 1
            end
        end
        self.can_lottery_times = 9 - times
    else
        self.can_lottery_times = 0
    end
end

-- 是否已经选了道具
function GodClothesData:IsSelect()
    return self.is_select
end

-- 当前剩余多少抽
function GodClothesData:CanLotteryTimes()
    return self.can_lottery_times
end

function GodClothesData:BaseCfg()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_GOD_CLOTHES, self.config.basis_configuration)
    return cfg[1]
end

function GodClothesData:RewardCfg(group)
    if not self.reward_cfg then
        self.reward_cfg = DataHelper.Subdivision(self.config.reward_group, {"section_start", "seq"})
    end
    return self.reward_cfg[group]
end

function GodClothesData:GetReward(group, seq)
    local cfg = self:RewardCfg(group)
    return cfg[seq]
end

function GodClothesData:ShopCfg()
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_GOD_CLOTHES, self.config.shop_configuration, "type")
end

function GodClothesData:ExchangeCfg()
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_GOD_CLOTHES, self.config.recharge_shop, "type")
end

function GodClothesData:ModelShowCfg()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_GOD_CLOTHES, self.config.show_item)
    return DataHelper.FormatItemList(cfg[1].reward_item), table.nums(cfg[1].reward_item)
end

function GodClothesData:SelectShow()
    local cfg = self:BaseCfg()
    return self:RewardCfg(cfg.rare_reward_group)
end

function GodClothesData:RandShow()
    local cfg = self:BaseCfg()
    return self:RewardCfg(cfg.common_reward_group)
end

function GodClothesData:ExchangeShowData()
    local list = {}
    for k, v in pairs(self:ExchangeCfg()) do
        if v.is_show > 0 then
            table.insert(list, v)
        end
    end
    return list
end

function GodClothesData:ExchangeData()
    local cfg = self:ExchangeCfg()
    local list = {}
    for k, v in pairs(cfg) do
        table.insert(list, v)
    end
    table.sort(list, DataHelper.WeightSort("seq", function(data)
        return self:CanExchangeTimes(data) > 0 and 0 or 1000
    end))
    return list
end

function GodClothesData:GetTotalTimes(cfg)
    return cfg.limit_type == 1 and cfg.buy_times or cfg.today_times
end

function GodClothesData:NowExchangeTimes(cfg)
    if cfg.limit_type == 1 then
        return self:ActExchangeTimes(cfg.seq)
    else
        return self:DayExchangeTimes(cfg.seq)
    end
end

function GodClothesData:CanExchangeTimes(cfg)
    local times = self:GetTotalTimes(cfg) - self:NowExchangeTimes(cfg)
    return times < 0 and 0 or times
end

function GodClothesData:ExchangeRemind(cfg)
    if cfg.is_remind > 0 then
        if self:CanExchangeTimes(cfg) > 0 then
            local has = Item.GetNum(self:ExchangeItemId())
            return has >= cfg.num and 1 or 0
        end
    end
    return 0
end

function GodClothesData:ShopData()
    local cfg = self:ShopCfg()
    local list = {}
    for k, v in pairs(cfg) do
        table.insert(list, v)
    end
    table.sort(list, DataHelper.WeightSort("seq", function(data)
        return self:CanBuyTimes(data) > 0 and 0 or 1000
    end))
    return list
end

function GodClothesData:NowBuyTimes(cfg)
    if cfg.limit_type == 1 then
        return self:ActBuyTimes(cfg.seq)
    else
        return self:DayBuyTimes(cfg.seq)
    end
end

function GodClothesData:CanBuyTimes(cfg)
    local times = self:GetTotalTimes(cfg) - self:NowBuyTimes(cfg)
    return times < 0 and 0 or times
end