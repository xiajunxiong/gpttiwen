VIEW_REQ("modules/serveractivity/soup_bath/soup_bath_view")
-- 兰汤沐浴
SoupBathCtrl = SoupBathCtrl or BaseClass(BaseCtrl)

function SoupBathCtrl:__init()
    if SoupBathCtrl.Instance ~= nil then
        Debuger.LogError("[SoupBathCtrl] attempt to create singleton twice!")
        return
    end
    SoupBathCtrl.Instance = self
    self.data = SoupBathData.New()
end

function SoupBathCtrl:OnInit()
    self:RegisterRemind()
    self:RegisterAllProtocol()
end

function SoupBathCtrl:RegisterRemind()
    local remind_func = BindTool.Bind(self.FlushRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.DragonBoat.SoupBath,
        self.data.info_data,
        remind_func
    )
    BagData.Instance:ListenItems(self.data:ConsumeItem(), function()
        self.data.info_data:Notify()
    end)
    FunOpen.Instance:SelfCondition(Mod.DragonBoat.SoupBath,function ()
        return ActivityData.IsOpen(ACTIVITY_TYPE.RAND_SOUP_BATH), Language.Activity.NotOpenTip
    end)
end

function SoupBathCtrl:RegisterAllProtocol()
    self:RegisterProtocol(SCRALanTangChuMuInfo, "OnSCRALanTangChuMuInfo")
end

function SoupBathCtrl:__delete()
    if SoupBathCtrl.Instance == self then
        SoupBathCtrl.Instance = nil
    end
end

function SoupBathCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function SoupBathCtrl:FlushRemind()
    local num = self.data:Remind()
    return num
end

-- 发送请求
function SoupBathCtrl:SendReq(req_type, param1, param2)
    -- LogError(ColorStr("SendReq", COLORSTR.Red1), req_type, param1, param2)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SOUP_BATH, req_type, param1, param2)
end

function SoupBathCtrl:OnSCRALanTangChuMuInfo(protocol)
    -- LogError(ColorStr("OnSCRALanTangChuMuInfo", COLORSTR.Red1), protocol)
    self.data:SetInfo(protocol)
end

-------------------------SoupBathData-------------------------
SoupBathData = SoupBathData or BaseClass()

SoupBathReq = {
    Info = 0,
    Task = 1,       --领取任务奖励，p1，seq
    Medicine = 2,   --加入药材，p1，seq p2，数量
    Reward = 3,     --开始沐浴
}

function SoupBathData:__init()
    if SoupBathData.Instance ~= nil then
        Debuger.LogError("[SoupBathData] attempt to create singleton twice!")
        return
    end
    SoupBathData.Instance = self
    self.config = Config.lantangchumu_auto
    self.info_data = SmartData.New()
    
    self:ResetData()
    self:InitData()
end

function SoupBathData:__delete()
    SoupBathData.Instance = nil
end

function SoupBathData:InitData()
    
end

function SoupBathData:ResetData()
    local info = {}
    info.reward_flag = {}
    info.task_param = {}
    info.last_reward_flag = 0   --最终奖励标志，0不可领取，1可领取，2已领取
    info.medicine_num = {}
    self.info_data.info = info
end

function SoupBathData:RewardFlag(seq)
    return self.info_data.info.reward_flag[seq] or 0
end

function SoupBathData:TaskParam(seq)
    return self.info_data.info.task_param[seq] or 0
end

function SoupBathData:MedicineNum(seq)
    return self.info_data.info.medicine_num[seq] or 0
end

function SoupBathData:IsCanReceiveTask(cfg)
    return self:TaskParam(cfg.seq) >= cfg.parameter and self:RewardFlag(cfg.seq) == 0
end

function SoupBathData:Remind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_SOUP_BATH) then
        return 0
    end
    -- 任务红点
    local cfg = self:TaskConfig()
    for k, v in pairs(cfg) do
        if self:IsCanReceiveTask(v) then
            return 1
        end
    end
    return self:LeftRemind()
end

-- 左侧可洗澡红点和可放材料红点
function SoupBathData:LeftRemind()
    -- 可洗澡红点
    if self.info_data.info.last_reward_flag == 1 then
        return 1
    end
    -- 可放材料红点
    for k, v in pairs(self:MedicineCfg()) do
        local put_num = self:MedicineNum(v.seq)
        if v.submit_num > put_num then
            if Item.GetNum(v.submit_item_id) > 0 then
                return 1
            end
        end
    end
    return 0
end

function SoupBathData:SetInfo(protocol)
    self.info_data.info = protocol.info
end

function SoupBathData:ConsumeItem()
    if not self.consume_list then
        self.consume_list = {}
        for k, v in pairs(self.config.herbs_configuration) do
            table.insert(self.consume_list, v.submit_item_id)
        end
    end
    return self.consume_list
end

function SoupBathData:MedicineCfg()
    return self.config.herbs_configuration
end

function SoupBathData:TaskConfig()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SOUP_BATH, self.config.basis_configuration, "type") or {}
    table.sort(cfg, DataHelper.WeightSort("seq", function(data)
        if self:IsCanReceiveTask(data) then
            return -100
        elseif self:RewardFlag(data.seq) == 1 then
            return 100
        end
        return 0
    end))
return cfg
end

function SoupBathData:RewardShowCfg()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SOUP_BATH, self.config.show_reward)
    return cfg[1]
end

function SoupBathData:SetLastReward(item_list)
    self.last_reward_list = item_list
end

function SoupBathData:GetLastReward()
    return self.last_reward_list
end