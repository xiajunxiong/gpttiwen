VIEW_REQ("modules/period_activity_act/sky_treasure_hunt/sky_treasure_hunt_view")
-- 天宫寻宝
SkyTreasureHuntCtrl = SkyTreasureHuntCtrl or BaseClass(BaseCtrl)

function SkyTreasureHuntCtrl:__init()
    if SkyTreasureHuntCtrl.Instance ~= nil then
        Debuger.LogError("[SkyTreasureHuntCtrl] attempt to create singleton twice!")
        return
    end
    SkyTreasureHuntCtrl.Instance = self
    self.data = SkyTreasureHuntData.New()
    self:RegisterProtocol(CSSkyTreasureHuntReq)
    self:RegisterProtocol(SCSkyTreasureHuntInfo, "OnSCSkyTreasureHuntInfo")
    
end

function SkyTreasureHuntCtrl:OnInit()
    -- 活动结束监听
    PeriodActivityData.Instance:CustomDeleteHandle(Mod.PeriodActivity.SkyTreasureHunt, function()
        if not self.data:IsOpen() and ViewMgr:IsOpen(SkyTreasureHuntView) then
            ViewMgr:CloseView(SkyTreasureHuntView)
            ViewMgr:CloseView(SkyTreasureHuntSelectView)
            PublicPopupCtrl.Instance:Center(Language.MSLantern.ActEnd)
        end
        self.data.info_data:Notify()
        return true
    end)
    -- 红点
    local remind_func = BindTool.Bind(self.FlushRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.PeriodActivity.SkyTreasureHunt,
        self.data.info_data,
        remind_func
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.PeriodActivity.SkyTreasureHunt,
        BagData.Instance:GetColumnGrids(ItemColumnType.Item),
        remind_func
    )
end

function SkyTreasureHuntCtrl:__delete()
    if SkyTreasureHuntCtrl.Instance == self then
        SkyTreasureHuntCtrl.Instance = nil
    end
end

function SkyTreasureHuntCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function SkyTreasureHuntCtrl:FlushRemind()
    local num = self.data:Remind()
    return num
end

-- 发送请求
function SkyTreasureHuntCtrl:SendReq(req_type)
    local protocol = GetProtocol(CSSkyTreasureHuntReq)
    protocol.req_type = req_type or SkyTreasureReqType.Start
    SendProtocol(protocol)
end

function SkyTreasureHuntCtrl:OnSCSkyTreasureHuntInfo(protocol)
    self.data:SetPos(protocol)
end


-----------------------SkyTreasureHuntData-----------------------
SkyTreasureHuntData = SkyTreasureHuntData or BaseClass()

-- 天宫寻宝操作请求类型
SkyTreasureReqType = {
    Info = 0,       --请求下发
    Start = 1,      --抽奖
}

-- 骰子状态触发
SkyTreasureDiceState = {
    [1] = "Point1",
    [2] = "Point2",
    [3] = "Point3",
    [4] = "Point4",
    [5] = "Point5",
    [6] = "Point6",
}

function SkyTreasureHuntData:__init()
    if SkyTreasureHuntData.Instance ~= nil then
        Debuger.LogError("[SkyTreasureHuntData] attempt to create singleton twice!")
        return
    end
    SkyTreasureHuntData.Instance = self
    self.config = Config.tiangongxunbao_auto
    self.info_data = SmartData.New({pos = 0, is_free = 0, step = 0})
    self:ResetData()
end

function SkyTreasureHuntData:__delete()
    SkyTreasureHuntData.Instance = nil
end

function SkyTreasureHuntData:ResetData()
    self.info_data.pos = 0
end

function SkyTreasureHuntData:SetPos(protocol)
    local now_pos = protocol.pos + 1
    if now_pos < self.info_data.pos then
        now_pos = now_pos + self:MaxNum()
    end
    self.info_data.step = now_pos - self.info_data.pos
    self.info_data.pos = protocol.pos + 1
    -- 这里用0免费，1不免费，服务器下发的是相反的
    self.info_data.is_free = protocol.use_free == 0 and 1 or 0
    self.info_data:Notify()
end

function SkyTreasureHuntData:MaxNum()
    if not self.max_num then
        self.max_num = #self.config.reward
    end
    return self.max_num
end

function SkyTreasureHuntData:ConsumeId()
    return self.config.cost[1].cost
end

-- 当前抽奖需要消耗多少
function SkyTreasureHuntData:NowConsumeNum()
    return self.info_data.is_free == 1 and 0 or self.config.cost[1].consumption_quantity
end

function SkyTreasureHuntData:GetRollerConfig()
    if not self.roller_reward_config then
        self.roller_reward_config = {}
        for k, v in pairs(self.config.reward) do
            self.roller_reward_config[v.index] = v
        end
    end
    return self.roller_reward_config
end

-- 是否开启
function SkyTreasureHuntData:IsOpen()
    return PeriodActivityData.Instance:IsOpen(Mod.PeriodActivity.SkyTreasureHunt)
end

function SkyTreasureHuntData:Remind()
    if self:IsOpen() then
        local has_num = Item.GetNum(self.item_id)
        if Item.GetNum(self:ConsumeId()) >= self:NowConsumeNum() then
            return 1
        end
    end
    return 0
end

-- 获取获取途径的数据
function SkyTreasureHuntData:GetActivityConfig()
    local t = {}
    for k, v in pairs(self.config.activity_name) do
        if PeriodActivityData.Instance:IsOpen(v.client_id) then
            table.insert(t, TableCopy(v, 2))
        end
    end
    for k, v in pairs(t) do
        v.progress, v.total_num = 0, 0
        local times_func = PeriodActivityData.Instance:GetTimesHandle(v.client_id)
        if times_func ~= nil then
            v.progress, v.total_num = times_func()
        else
            LogError("模块："..data.client_id.."未监听次数")
        end
    end
    table.sort(t, DataHelper.WeightSort("seq", function(data)
        return data.progress ~= 0 and 0 or 1000
    end))
    return t
end