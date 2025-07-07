VIEW_REQ("modules/single_rebate/single_rebate_view")
VIEW_REQ("modules/single_rebate/single_rebate_buy")

-- 单笔返利
SingleRebateCtrl = SingleRebateCtrl or BaseClass(BaseCtrl)

function SingleRebateCtrl:__init()
    if SingleRebateCtrl.Instance ~= nil then
        Debuger.LogError("[SingleRebateCtrl] attempt to create singleton twice!")
        return
    end
    SingleRebateCtrl.Instance = self
    self.data = SingleRebateData.New()
end

function SingleRebateCtrl:OnInit()
    self:RegisterRemind()
    self:RegisterAllProtocol()
end

function SingleRebateCtrl:RegisterRemind()
    local remind_func = BindTool.Bind(self.FlushRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.RandActivity.SingleRebate,
        self.data.info_data,
        remind_func
    )
end

function SingleRebateCtrl:RegisterAllProtocol()
    self:RegisterProtocol(SCRADanBiFanLiInfo, "OnSCRADanBiFanLiInfo")
    self:RegisterProtocol(SCRADanBiFanLiSingleInfo, "OnSCRADanBiFanLiSingleInfo")
end

function SingleRebateCtrl:__delete()
    if SingleRebateCtrl.Instance == self then
        SingleRebateCtrl.Instance = nil
    end
end

function SingleRebateCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function SingleRebateCtrl:FlushRemind()
    local num = self.data:Remind()
    return num
end

-- 发送请求
function SingleRebateCtrl:SendReq(req_type, param1, param2, param3)
    -- LogError(ColorStr("单笔返利发送请求", COLORSTR.Red8), req_type, param1, param2, param3)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SINGLE_REBATE, req_type, param1, param2, param3)
end

function SingleRebateCtrl:OnSCRADanBiFanLiInfo(protocol)
    -- LogError(ColorStr("单笔返利活动信息下发"..protocol.msg_type, COLORSTR.Red8), protocol)
    self.data:SetDataInfo(protocol)
end

function SingleRebateCtrl:OnSCRADanBiFanLiSingleInfo(protocol)
    -- LogError(ColorStr("单笔返利抽奖信息下发"..protocol.msg_type, COLORSTR.Red8), protocol)
    self.data:SetSingleInfo(protocol)
end

-----------------------------SingleRebateData-----------------------------
SingleRebateData = SingleRebateData or BaseClass()

-- 操作请求类型
SingleRebateReqType = {
    Info = 0,       --请求下发
    MoneyBuy = 1,   --直购，p1：pool_seq，p2：seq
}

SingleRebateLineColor = {
    [1] = COLORS.Green2,
    [2] = COLORS.Yellow14,
    [3] = COLORS.Red21,
}

function SingleRebateData:__init()
    if SingleRebateData.Instance ~= nil then
        Debuger.LogError("[SingleRebateData] attempt to create singleton twice!")
        return
    end
    SingleRebateData.Instance = self
    self.config = Config.danbifanli_auto
    self.other = self.config.other[1]
    self.info_data = SmartData.New()
    
    self:ResetData()
end

function SingleRebateData:__delete()
    SingleRebateData.Instance = nil
end

function SingleRebateData:ResetData()
    local info = {}
    -- 每日购买次数
    info.today_buy_times = {}
    for i = 0, 9 do
        info.today_buy_times[i] = 0
    end
    -- 活动购买次数
    info.act_buy_times = {}
    for i = 0, 9 do
        info.act_buy_times[i] = 0
    end
    self.info_data.info = info
end

function SingleRebateData:TodayBuyTimes(seq)
    return self.info_data.info.today_buy_times[seq] or 0
end

function SingleRebateData:ActBuyTimes(seq)
    return self.info_data.info.act_buy_times[seq] or 0
end

function SingleRebateData:TitleShow()
    return self.other.percentage
end

function SingleRebateData:Remind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_SINGLE_REBATE) then
        return 0
    end
    -- 活动红点
    local act_start = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_SINGLE_REBATE)
	if tostring(act_start) ~= PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_SINGLE_REBATE)) then
		-- 活动首次红点，换设备重新提示
		return 1
    end
    return 0
end

function SingleRebateData:ClearFirstRemind()
    local act_start = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_SINGLE_REBATE)
	if tostring(act_start) ~= PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_SINGLE_REBATE)) then
        PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_SINGLE_REBATE), act_start)
        self.info_data:Notify()
	end
end


function SingleRebateData:SetDataInfo(protocol)
    self.info_data.info = protocol.info
end

function SingleRebateData:SetSingleInfo(protocol)
    self.info_data.info.today_buy_times[protocol.pool_seq] = protocol.today_buy_times
    self.info_data.info.act_buy_times[protocol.pool_seq] = protocol.act_buy_times
    self.info_data:SetDirty("info")
end

function SingleRebateData:PoolCfg()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SINGLE_REBATE, self.config.gold_type)
    return cfg
end

function SingleRebateData:GetPool(pool)
    local cfg = self:PoolCfg()
    for k, v in pairs(cfg) do
        if v.gold_seq == pool then
            return v
        end
    end
end

function SingleRebateData:RewardCfg()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SINGLE_REBATE, self.config.reward)
    local reward = {}
    for k, v in pairs(cfg) do
        if not reward[v.gold_seq] then
            reward[v.gold_seq] = {}
        end
        reward[v.gold_seq][v.seq] = v
    end
    return reward
end

function SingleRebateData:GetReward(pool, seq)
    local cfg = self:RewardCfg()
    if pool then
        if seq then
            return cfg[pool][seq]
        else
            return cfg[pool]
        end
    end
    return cfg
end

function SingleRebateData:NowBuyTimes(cfg)
    if cfg.limit_type == 1 then
        return self:ActBuyTimes(cfg.gold_seq)
    else
        return self:TodayBuyTimes(cfg.gold_seq)
    end
end

function SingleRebateData:CanBuyTimes(cfg)
    local times = cfg.times - self:NowBuyTimes(cfg)
    return times < 0 and 0 or times
end

function SingleRebateData:NowShowPoolData(value)
    if value then
        self.now_show_pool_data = value
    else
        return self.now_show_pool_data
    end
end

function SingleRebateData:NowShowPoolRewardData(pool)
    if pool then
        self.now_show_pool_reward_data = self:GetReward(pool)
    else
        return self.now_show_pool_reward_data
    end
end