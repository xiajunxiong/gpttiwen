VIEW_REQ("modules/serveractivity/reed_incense_city/reed_incense_city_view")
-- 粽香满城
ReedIncenseCityCtrl = ReedIncenseCityCtrl or BaseClass(BaseCtrl)

function ReedIncenseCityCtrl:__init()
    if ReedIncenseCityCtrl.Instance ~= nil then
        Debuger.LogError("[ReedIncenseCityCtrl] attempt to create singleton twice!")
        return
    end
    ReedIncenseCityCtrl.Instance = self
    self.data = ReedIncenseCityData.New()
end

function ReedIncenseCityCtrl:OnInit()
    self:RegisterRemind()
    self:RegisterAllProtocol()
end

function ReedIncenseCityCtrl:RegisterRemind()
    local remind_func = BindTool.Bind(self.FlushRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.DragonBoat.ReedIncenseCity,
        self.data.info_data,
        remind_func
    )
    RemindCtrl.Instance:Remind():Register(
        Mod.DragonBoat.ReedIncenseCity,
        ActivityData.Instance.active_data,
        remind_func,
        "active_num"
    )
    FunOpen.Instance:SelfCondition(Mod.DragonBoat.ReedIncenseCity,function ()
        return ActivityData.IsOpen(ACTIVITY_TYPE.RAND_REED_INCENSE_CITY), Language.Activity.NotOpenTip
    end)
end

function ReedIncenseCityCtrl:RegisterAllProtocol()
    self:RegisterProtocol(SCRAZongXiangManChengInfo, "OnSCRAZongXiangManChengInfo")
end

function ReedIncenseCityCtrl:__delete()
    if ReedIncenseCityCtrl.Instance == self then
        ReedIncenseCityCtrl.Instance = nil
    end
end

function ReedIncenseCityCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function ReedIncenseCityCtrl:FlushRemind()
    local num = self.data:Remind()
    return num
end

-- 发送请求
function ReedIncenseCityCtrl:SendReq(param1)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_REED_INCENSE_CITY, 1, param1)
end

function ReedIncenseCityCtrl:OnSCRAZongXiangManChengInfo(protocol)
    self.data:SetInfo(protocol)
end

-------------------------ReedIncenseCityData-------------------------
ReedIncenseCityData = ReedIncenseCityData or BaseClass()

function ReedIncenseCityData:__init()
    if ReedIncenseCityData.Instance ~= nil then
        Debuger.LogError("[ReedIncenseCityData] attempt to create singleton twice!")
        return
    end
    ReedIncenseCityData.Instance = self
    self.config = Config.zongxiangmancheng_auto
    self.info_data = SmartData.New()
    
    self:ResetData()
    self:InitData()
end

function ReedIncenseCityData:__delete()
    ReedIncenseCityData.Instance = nil
end

function ReedIncenseCityData:InitData()
    
end

function ReedIncenseCityData:ResetData()
    local info = {}
    info.select_type = 0
    info.flag = {}
    self.info_data.info = info
end

function ReedIncenseCityData:Remind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_REED_INCENSE_CITY) then
        return 0
    end
    for k, v in pairs(self:BaseCfg()) do
        if self:SingleRemind(v.seq) > 0 then
            return 1
        end
    end
    return 0
end

-- 这里传的seq对应表里的0~8，第二个参数为需要飘字的类型，第三个参根据第二个参而定
-- p2: 1：没开活动 2：已领取 3：已点亮 4：已点亮其他路线 5：需要点亮上一个 6：活跃度不足
-- p3: 6：所需活跃度
function ReedIncenseCityData:SingleRemind(seq)
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_REED_INCENSE_CITY) then
        return 0, 1
    end
    local cfg = self:BaseCfg(seq)
    -- 获取当前活跃度
    local active_num = ActivityData.Instance:GetActiveIntegralNum()
    if self.info_data.info.select_type == 0 then
        if seq % 4 == 0 then
            if active_num >= cfg.parameter then
                return 1
            else
                return 0, 6, cfg.parameter
            end
        else
            return 0, 5
        end
    else
        if self.info_data.info.select_type == cfg.light_type then
            if self:RewardFlag(seq) == 1 then
                if seq % 4 == 3 then
                    return 0, 2
                else
                    return 0, 3
                end
            else
                if self:RewardFlag(seq - 1) == 1 then
                    if active_num >= cfg.parameter then
                        return 1
                    else
                        return 0, 6, cfg.parameter
                    end
                else
                    return 0, 5
                end
            end
        else
            return 0, 4
        end
    end
end

function ReedIncenseCityData:RewardFlag(seq)
    return self.info_data.info.flag[seq] or 0
end

function ReedIncenseCityData:SetInfo(protocol)
    self.info_data.info = protocol.info
end

-- 这里传的seq对应表里的0~8，不传返回三个值
function ReedIncenseCityData:GetNeedActive(seq)
    local cfg = self:BaseCfg()
    if seq then
        return cfg[seq + 1].parameter
    else
        return cfg[1].parameter, cfg[2].parameter, cfg[3].parameter
    end
end

function ReedIncenseCityData:BaseCfg(seq)
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_REED_INCENSE_CITY, self.config.basis_configuration, "type")
    return seq and cfg[seq + 1] or cfg
end

function ReedIncenseCityData:RewardShowCfg()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_REED_INCENSE_CITY, self.config.show_reward)
    return cfg[1]
end