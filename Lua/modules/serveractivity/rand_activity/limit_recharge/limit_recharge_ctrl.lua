VIEW_REQ("modules/serveractivity/rand_activity/limit_recharge/limit_recharge_view")

LimitRechargeCtrl = LimitRechargeCtrl or BaseClass(BaseCtrl)

function LimitRechargeCtrl:__init()
	if LimitRechargeCtrl.Instance ~= nil then
		Debuger.LogError("[LimitRechargeCtrl] attempt to create singleton twice!")
		return
	end	
	LimitRechargeCtrl.Instance = self
	self.data = LimitRechargeData.New()
	self:RegisterAllProtocols()
end

function LimitRechargeCtrl:__delete()
	self:UnRegisterAllProtocols()
	LimitRechargeCtrl.Instance = nil
	Delete(self, "data")
end

function LimitRechargeCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAXianShiLeiChongInfo, "OnLimitRechargeInfo")
end

function LimitRechargeCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCRAXianShiLeiChongInfo)
end

function LimitRechargeCtrl:OnInit()
    self:OnRemind()
end

function LimitRechargeCtrl:OnRemind()
    Remind.Instance:Register(Mod.RandActivity.LimitRecharge, self.data.limit_recharge_data, function()
        if not ActivityData.IsOpen(ACTIVITY_TYPE.RANK_LIMIT_RECHARGE) then
			return 0
		end
		return self.data:GetRemind()
	end)
end

function LimitRechargeCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function LimitRechargeCtrl:OnLimitRechargeInfo(protocol)
    self.data:SetLimitRechargeInfo(protocol)
end


--------------------LimitRechargeData--------------------
LimitRechargeData = LimitRechargeData or BaseClass()

function LimitRechargeData:__init()
	if LimitRechargeData.Instance ~= nil then
		Debuger.LogError("[LimitRechargeData] attempt to create singleton twice!")
		return
	end
	LimitRechargeData.Instance = self
    self.limit_recharge_data = SmartData.New()
    self:ResetLimitRechargeData()
end

function LimitRechargeData:ResetData()
    self:ResetLimitRechargeData()
end

function LimitRechargeData:ResetLimitRechargeData()
    self.limit_recharge_data:Set({
        open_day = 0,
        day_recharge_num = 0,
        can_fetch_flag = {},
        fetched_flag = {}
    })
end

function LimitRechargeData:SetLimitRechargeInfo(protocol)
    self.limit_recharge_data.open_day = protocol.open_day
    self.limit_recharge_data.day_recharge_num = protocol.day_recharge_num
    self.limit_recharge_data.can_fetch_flag = protocol.can_fetch_flag
    self.limit_recharge_data.fetched_flag = protocol.fetched_flag
end

function LimitRechargeData:GetRemind()
    for k, v in pairs(self:GetRewardConfig()) do
        if self:IsCanFetch(v.seq) then
            return 1
        end
    end
    return 0
end

function LimitRechargeData:GetCanFetchFlag(seq)
    return self.limit_recharge_data.can_fetch_flag[seq] or 0
end

function LimitRechargeData:GetFetchedFlag(seq)
    return self.limit_recharge_data.fetched_flag[seq] or 0
end

function LimitRechargeData:IsCanFetch(seq)
    return self:GetCanFetchFlag(seq) > 0 and self:GetFetchedFlag(seq) == 0
end

function LimitRechargeData:GetRewardConfig()
    local t = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RANK_LIMIT_RECHARGE, Config.xianshileichong_auto.gift_configure, "seq")
    table.sort(t, DataHelper.WeightSort("seq", function(data)
        return self:GetFetchedFlag(data.seq) > 0 and 1000 or 0
    end))
    return t
end