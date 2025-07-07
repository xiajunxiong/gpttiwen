VIEW_REQ("modules/serveractivity/may_day/vitality_firework_view")
-- 元气烟火秀
VitalityFireworkCtrl = VitalityFireworkCtrl or BaseClass(BaseCtrl)

function VitalityFireworkCtrl:__init()
    if VitalityFireworkCtrl.Instance ~= nil then
        Debuger.LogError("[VitalityFireworkCtrl] attempt to create singleton twice!")
        return
    end
    VitalityFireworkCtrl.Instance = self
    self.data = VitalityFireworkData.New()
end

function VitalityFireworkCtrl:OnInit()
    self:RegisterRemind()
    self:RegisterAllProtocol()
end

function VitalityFireworkCtrl:RegisterRemind()
    -- 红点
    local remind_func = BindTool.Bind(self.FlushRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.MayDay.VitalityFirework,
        self.data.info_data,
        remind_func
    )
end

function VitalityFireworkCtrl:RegisterAllProtocol()
    self:RegisterProtocol(SCRAYuanQiYanHuo, "OnSCRAYuanQiYanHuo")
end

function VitalityFireworkCtrl:__delete()
    if VitalityFireworkCtrl.Instance == self then
        VitalityFireworkCtrl.Instance = nil
    end
end

function VitalityFireworkCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function VitalityFireworkCtrl:FlushRemind()
    local num = self.data:Remind()
    return num
end

-- 发送请求
function VitalityFireworkCtrl:SendReq(req_type, param1)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_VITALITY_FIREWORK, req_type, param1)
end

function VitalityFireworkCtrl:OnSCRAYuanQiYanHuo(protocol)
    -- LogError("OnSCRAYuanQiYanHuo",protocol)
    self.data:SetInfo(protocol)
end

-- 使用烟花
function VitalityFireworkCtrl:UseFirework()
    if ActivityData.IsOpen(ACTIVITY_TYPE.RAND_VITALITY_FIREWORK) then
        local now_time_stamp = TimeCtrl.Instance:GetServerTime()
        local t0 = TimeManager.FormatUnixTime2Date(now_time_stamp)
        local time_start = self.data:Other().time_start
        local start_hour = math.floor(time_start / 100)
        local start_sec = time_start % 100
        local time_end = self.data:Other().time_end
        local end_hour = math.floor(time_end / 100)
        local end_sec = time_end % 100
        local start_time_stamp = os.time({year = t0.year, month = t0.month, day = t0.day, hour = start_hour, min = start_sec, sec = 0})
        local end_time_stamp = os.time({year = t0.year, month = t0.month, day = t0.day, hour = end_hour, min = end_sec, sec = 0})
        if now_time_stamp > start_time_stamp and now_time_stamp < end_time_stamp then
            self:SendReq(VitalityFireworkReqType.UseFire)
        else
            PublicPopupCtrl.Instance:Center(Language.VitalityFirework.ActTip2)
        end
    else
        PublicPopupCtrl.Instance:Center(Language.VitalityFirework.ActTip)
    end
end


-----------------------VitalityFireworkData-----------------------
VitalityFireworkData = VitalityFireworkData or BaseClass()

-- 操作请求类型
VitalityFireworkReqType = {
    Info = 0,       --请求下发
    Buy = 1,        --购买
    Receive = 2,    --领取奖励，p1：seq
    Free = 3,       --领取免费礼包
    UseFire = 4,    --使用烟火
}

function VitalityFireworkData:__init()
    if VitalityFireworkData.Instance ~= nil then
        Debuger.LogError("[VitalityFireworkData] attempt to create singleton twice!")
        return
    end
    VitalityFireworkData.Instance = self
    self.config = Config.yuanqiyanhuoxiu_auto
    self.other = self.config.other[1]
    self.info_data = SmartData.New()
    self:ResetData()
end

function VitalityFireworkData:__delete()
    VitalityFireworkData.Instance = nil
end

function VitalityFireworkData:ResetData()
    local info = {}
    info.is_buy = 0
    info.is_receive_free = 0
    info.reward_flag = {}
    self.info_data.info = info
end

function VitalityFireworkData:IsBuy()
    return self.info_data.info.is_buy == 1
end

function VitalityFireworkData:IsReceiveFree()
    return self.info_data.info.is_receive_free == 1
end

function VitalityFireworkData:RewardFlag(seq)
    return self.info_data.info.reward_flag[seq] or 0 
end

-- 判断是否可以领取，这里会排除已领取和未到时间的情况
function VitalityFireworkData:IsCanReceive(seq)
    local open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_VITALITY_FIREWORK)
    return self:RewardFlag(seq) == 0 and open_day > seq
end

function VitalityFireworkData:Other()
    return self.other
end

function VitalityFireworkData:Remind()
    if ActivityData.IsOpen(ACTIVITY_TYPE.RAND_VITALITY_FIREWORK) then
        if not self:IsReceiveFree() then
            return 1
        end
        if self:IsBuy() then
            local reward_config = self:RewardConfig()
            for k, v in pairs(reward_config) do
                if self:IsCanReceive(v.seq) then
                    return 1
                end
            end
        end
    end
    return 0
end

function VitalityFireworkData:RewardConfig()
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_VITALITY_FIREWORK, self.config.reward, "type")
end

function VitalityFireworkData:SetInfo(protocol)
    self.info_data.info = protocol.info
end