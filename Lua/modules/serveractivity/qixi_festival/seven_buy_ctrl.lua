VIEW_REQ("modules/serveractivity/qixi_festival/seven_buy_view")
-- 七巧特惠
SevenBuyCtrl = SevenBuyCtrl or BaseClass(BaseCtrl)

function SevenBuyCtrl:__init()
    if SevenBuyCtrl.Instance ~= nil then
        Debuger.LogError("[SevenBuyCtrl] attempt to create singleton twice!")
        return
    end
    SevenBuyCtrl.Instance = self
    self.data = SevenBuyData.New()
end

function SevenBuyCtrl:OnInit()
    self:RegisterRemind()
    self:RegisterAllProtocol()
end

function SevenBuyCtrl:RegisterRemind()
    local remind_func = BindTool.Bind(self.FlushRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.QixiFestival.SevenBuy,
        self.data.info_data,
        remind_func
    )
end

function SevenBuyCtrl:RegisterAllProtocol()
    self:RegisterProtocol(SCRAQiXiTeHuiInfo, "OnSCRAQiXiTeHuiInfo")
    self:RegisterProtocol(SCRAQiXiTeHuiSingleInfo, "OnSCRAQiXiTeHuiSingleInfo")
    self:RegisterProtocol(SCRAQiXiTeHuiOtherInfo, "OnSCRAQiXiTeHuiOtherInfo")
end

function SevenBuyCtrl:__delete()
    if SevenBuyCtrl.Instance == self then
        SevenBuyCtrl.Instance = nil
    end
end

function SevenBuyCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function SevenBuyCtrl:FlushRemind()
    local num = self.data:Remind()
    return num
end

-- 发送请求
function SevenBuyCtrl:SendReq(req_type, param1)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SEVEN_BUY, req_type or 0, param1 or 0)
end

function SevenBuyCtrl:OnSCRAQiXiTeHuiInfo(protocol)
    -- LogError(ColorStr("OnSCRAQiXiTeHuiInfo",COLORSTR.Green9),protocol)
    self.data:SetInfo(protocol)
end

function SevenBuyCtrl:OnSCRAQiXiTeHuiSingleInfo(protocol)
    -- LogError(ColorStr("OnSCRAQiXiTeHuiSingleInfo",COLORSTR.Green9),protocol)
    self.data:SetSingleInfo(protocol)
end

function SevenBuyCtrl:OnSCRAQiXiTeHuiOtherInfo(protocol)
    -- LogError(ColorStr("OnSCRAQiXiTeHuiOtherInfo",COLORSTR.Green9),protocol)
    self.data:SetOtherInfo(protocol)
end

-----------------SevenBuyData-----------------
SevenBuyData = SevenBuyData or BaseClass()

-- 操作请求类型
SevenBuyReqType = {
    Info = 0,       --请求下发
    Buy = 1,        --购买，p1：seq
    FirstOpen = 2,  --请求第一次打开界面
    PhaseReward = 3,--领取阶段奖励，p1：seq
}

function SevenBuyData:__init()
    if SevenBuyData.Instance ~= nil then
        Debuger.LogError("[SevenBuyData] attempt to create singleton twice!")
        return
    end
    SevenBuyData.Instance = self
    self.config = Config.qiqiaotehui_auto
    self.other = self.config.other[1]
    self.info_data = SmartData.New()
    -- 一行最多放几个
    self.max_show = 3
    self:ResetData()
end

function SevenBuyData:__delete()
    SevenBuyData.Instance = nil
end

function SevenBuyData:ResetData()
    local info = {}
    info.is_open_flag = 1
    info.activity_buy_count = 0
    info.phase_reward_flag = {}
    info.today_buy_times = {}
    info.activity_buy_times = {}
    self.info_data.info = info

    self.max_page = 1
    self.base_data = {}
    self.reward_show_data = {}
end

function SevenBuyData:Remind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_SEVEN_BUY) then
        return 0
    end
    if self.info_data.info.is_open_flag == 0 then
        return 1
    end
    for k, v in pairs(self:PhaseCfg()) do
        if self:IsCanGetPhaseReward(v) then
            return 1
        end
    end
    return 0
end

function SevenBuyData:MaxShow()
    return self.max_show
end

function SevenBuyData:BaseCfg()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SEVEN_BUY, self.config.basis_configuration, "type")
    return cfg
end

-- 返回分好页的数据和最大页数
function SevenBuyData:BaseCfgData()
    local cfg = self:BaseCfg()
    local max_page = 1
    local base_data = {}
    table.sort(cfg, DataHelper.WeightSort("seq", function(data)
        return self:IsBuyAll(data) and data.seq + 1000 or data.seq
    end))
    for i, v in ipairs(cfg) do
        local index1, index2 = math.floor((i - 1) / self.max_show) + 1, (i - 1) % self.max_show + 1
        if not base_data[index1] then
            base_data[index1] = {}
        end
        base_data[index1][index2] = v
        max_page = index1
    end
    return base_data, max_page
end

function SevenBuyData:RewardShowCfg(seq)
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SEVEN_BUY, self.config.show_reward_item, "type")
    return cfg
end

-- 返回分好页的数据和最大页数
function SevenBuyData:RewardShowCfgData(activity_days)
    if TableIsEmpty(self.reward_show_data) then
        local cfg = self:RewardShowCfg()
        self.reward_show_data = {}
        for k, v in pairs(cfg) do
            local index1, index2 = math.floor(v.seq / self.max_show) + 1, v.seq % self.max_show + 1
            if not self.reward_show_data[v.activity_days] then
                self.reward_show_data[v.activity_days] = {}
            end
            if not self.reward_show_data[v.activity_days][index1] then
                self.reward_show_data[v.activity_days][index1] = {}
            end
            self.reward_show_data[v.activity_days][index1][index2] = v
        end
    end
    return self.reward_show_data[activity_days], #self.reward_show_data[activity_days]
end

function SevenBuyData:PhaseCfg()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SEVEN_BUY, self.config.phase_reward, "type")
    return cfg
end

-- 返回当前限购类型的已购买次数
function SevenBuyData:LimitTimes(cfg)
    if cfg.buy_limit == 1 then
        return self:ActivityBuyTimes(cfg.seq)
    else
        return self:TodayBuyTimes(cfg.seq)
    end
end

-- 该商品是否买完了
function SevenBuyData:IsBuyAll(cfg)
    local times = self:LimitTimes(cfg)
    return times >= cfg.limit_times
end

-- 判断该阶段奖励是否可领取
function SevenBuyData:IsCanGetPhaseReward(cfg)
    return self:PhaseRewardFlag(cfg.seq) == 0 and self.info_data.info.activity_buy_count >= cfg.buy_times
end

function SevenBuyData:PhaseRewardFlag(seq)
    return self.info_data.info.phase_reward_flag[seq] or 0
end

function SevenBuyData:TodayBuyTimes(seq, value)
    if not value then
        return self.info_data.info.today_buy_times[seq] or 0
    end
    self.info_data.info.today_buy_times[seq] = value
end

function SevenBuyData:ActivityBuyTimes(seq, value)
    if not value then
        return self.info_data.info.activity_buy_times[seq] or 0
    end
    self.info_data.info.activity_buy_times[seq] = value
end

function SevenBuyData:SetInfo(protocol)
    self.info_data.info = protocol.info
end

function SevenBuyData:SetSingleInfo(protocol)
    self.info_data.info.activity_buy_count = protocol.activity_buy_count
    self:TodayBuyTimes(protocol.seq, protocol.today_buy_time)
    self:ActivityBuyTimes(protocol.seq, protocol.activity_buy_time)
    self.info_data:Notify()
end

function SevenBuyData:SetOtherInfo(protocol)
    self.info_data.info.is_open_flag = protocol.is_open_flag
    self.info_data.info.phase_reward_flag = protocol.phase_reward_flag
    self.info_data:Notify()
end

function SevenBuyData:Desc(seq)
    local cfg = self:BaseCfg()
    for k, v in pairs(cfg) do
        if v.seq == seq then
            return v.desc
        end
    end
    return cfg[1].desc
end