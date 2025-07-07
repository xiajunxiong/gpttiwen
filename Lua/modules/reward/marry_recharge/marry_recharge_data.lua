MarryRechargeData = MarryRechargeData or BaseClass()
function MarryRechargeData:__init()
    if MarryRechargeData.Instance ~= nil then
        Debuger.LogError("[MarryRechargeData] attempt to create singleton twice!")
        return
    end
    MarryRechargeData.Instance = self

    self.act_info = SmartData.New({})
end

function MarryRechargeData:__delete()
    MarryRechargeData.Instance = nil
end

function MarryRechargeData:Config()
    return Config.qingyuanleichong_auto
end

function MarryRechargeData:GetFunIsOpened()
    if self.act_info.join_begin_timestamp ~= nil then
        if self.act_info.join_begin_timestamp < TimeHelper.GetServerTime() and TimeHelper.GetServerTime() < self.act_info.join_end_timestamp then
            return true
        end
    end
    return false
end

function MarryRechargeData:ClearData()
    self.act_info:Clear()
    TimeHelper:CancelTimer(self.act_end_timer)
end

function MarryRechargeData:GetRechargeNum()
    return self.act_info.recharge_num or 0
end

function MarryRechargeData:IsFetchedReward(seq)
    return self:GetFetchMark(seq) == 1
end

function MarryRechargeData:GetRemindNum()
    if not MarryRechargeData.Instance:GetFunIsOpened() then
        return 0
    end
    if self:GetNewFlag() then
        return 1
    end
    local recharge_num = self:GetRechargeNum()
    for i, v in pairs(self:GetRewardList()) do
        if not self:IsFetchedReward(v.seq) and recharge_num >= v.acc_price then
            return 1
        end
    end
    return 0
end

function MarryRechargeData:SetAllInfo(protocol)
    self.act_info.join_begin_timestamp = protocol.join_begin_timestamp
    self.act_info.join_end_timestamp = protocol.join_end_timestamp
    self.act_info.recharge_num = protocol.chongzhi_gold
    self.act_info.fetch_mark = protocol.fetch_mark

    TimeHelper:CancelTimer(self.act_end_timer)
    if self.act_info.join_end_timestamp > 0 then
        self.act_end_timer = TimeHelper:AddCountDownCT(BindTool.Bind1(self.UpdateCall, self), BindTool.Bind1(self.OnCompleteCall, self), self.act_info.join_end_timestamp)
    end
end

function MarryRechargeData:UpdateCall()
end
function MarryRechargeData:OnCompleteCall()
    self.act_end_timer = nil
    self.act_info:Notify()
end

function MarryRechargeData:OnActOver()
    TimeHelper:CancelTimer(self.act_end_timer)
    self.act_end_timer = nil
    self.act_info:Clear()
    self.act_info:Notify()
end

function MarryRechargeData:GetRewardList()
    local begin_time = self.act_info.join_begin_timestamp or TimeCtrl.Instance:ServerStartTS()
    local open_day = TimeCtrl.Instance:GetDataDayNum(begin_time, TimeCtrl.Instance:ServerStartTS())
    return ServerActivityData.Instance:GetSectionList(self:Config().gift_configure, "seq", open_day)
end

function MarryRechargeData:GetFetchMark(seq)
    if self.act_info.fetch_mark ~= nil then
        return self.act_info.fetch_mark[seq] or 0
    end
    return 0
end

function MarryRechargeData:SetNewFlag()
    UnityPlayerPrefs.SetInt(PrefKeys.ActMarryRecharge(), math.floor(self.act_info.join_end_timestamp or 0))
    self.act_info:Notify()
end

function MarryRechargeData:GetNewFlag()
    local end_timestamp = math.floor(self.act_info.join_end_timestamp or 0)
    local save_val = UnityPlayerPrefs.GetInt(PrefKeys.ActMarryRecharge())
    if save_val == 0 or save_val < end_timestamp then
        return true
    end
    return false
end

function MarryRechargeData:GetActEndTimestamp()
    return self.act_info.join_end_timestamp or 0
end
