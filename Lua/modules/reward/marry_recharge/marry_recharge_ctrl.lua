FILE_REQ("modules/reward/marry_recharge/marry_recharge_data")
VIEW_REQ("modules/reward/marry_recharge/marry_recharge_view")

MarryRechargeCtrl = MarryRechargeCtrl or BaseClass(BaseCtrl)
function MarryRechargeCtrl:__init()
    if MarryRechargeCtrl.Instance ~= nil then
        Debuger.LogError("[MarryRechargeCtrl] attempt to create singleton twice!")
        return
    end
    MarryRechargeCtrl.Instance = self
    self.data = MarryRechargeData.New()
    self:RegisterAllProtocols()
end

function MarryRechargeCtrl:__delete()
    MarryRechargeCtrl.Instance = nil
    Delete(self, "data")
end

function MarryRechargeCtrl:OnInit()
    ActivityData.Instance:CareEvent(BindTool.Bind1(self.OnActEvent, self))
    RemindCtrl.Instance:Remind():Register(Mod.Reward.MarryRecharge, self.data.act_info, function()
        return ActivityData.IsOpen(ACTIVITY_TYPE.RAND_MARRY_RECHARGE) and self.data:GetRemindNum() or 0
	end)
    ActivityRandData.Instance:Register(Mod.Reward.MarryRecharge, function()
		return self.data:GetFunIsOpened()
    end)
end

function MarryRechargeCtrl:OnUnloadGameLogic()
    self.data:ClearData()
end

function MarryRechargeCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAQingYuanLeiChongInfo, "OnSCRAQingYuanLeiChongInfo")
end

function MarryRechargeCtrl:OnActEvent()
    if ActivityData.Instance:GetActivityEventType() ~= ACTIVITY_TYPE.RAND_MARRY_RECHARGE then
        return
    end
    local act_state = ActivityData.Instance:GetActivityStatus(ACTIVITY_TYPE.RAND_MARRY_RECHARGE)
    if act_state == ActStatusType.Close then
        self.data:OnActOver()
        RewardData.Instance.reward_list_data:Notify()
    end
end

function MarryRechargeCtrl:SendAllInfo()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_MARRY_RECHARGE, 0)
end

function MarryRechargeCtrl:SendFetchReward(seq)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_MARRY_RECHARGE, 1, seq)
end

function MarryRechargeCtrl:OnSCRAQingYuanLeiChongInfo(protocol)
    self.data:SetAllInfo(protocol)
end

-----考虑情缘信息改变，
function MarryRechargeCtrl:OnMarryChange()
    
end
