FILE_REQ("modules/serveractivity/sow_reward/sow_reward_data")
VIEW_REQ("modules/serveractivity/sow_reward/sow_reward_view")

SowRewardCtrl = SowRewardCtrl or BaseClass(BaseCtrl)
function SowRewardCtrl:__init()
    if SowRewardCtrl.Instance ~= nil then
        Debuger.LogError("[SowRewardCtrl] attempt to create singleton twice!")
        return
    end
    SowRewardCtrl.Instance = self
    self.data = SowRewardData.New()
    self:RegisterAllProtocols()
end

function SowRewardCtrl:__delete()
    SowRewardCtrl.Instance = nil
    Delete(self, "data")
end

function SowRewardCtrl:OnInit()
    PeriodActivityCtrl.Instance:RegisterRemind(Mod.MayDay.SowReward, self.data.remind_info, function()
    	return ActivityData.IsOpen(ACTIVITY_TYPE.RAND_SOW_REWARD) and self.data:GetRemindNum() or 0
    end)
    EventSys:Bind(GlobalEventKeys.TimeMinuteChange, BindTool.Bind(self.OnMinuteChange, self))
end

function SowRewardCtrl:OnUnloadGameLogic()
end

function SowRewardCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRABoZhongYouLiInfo, "OnSCRABoZhongYouLiInfo")
end

function SowRewardCtrl:OnMinuteChange()
    self.data:CheckRemind()
end

function SowRewardCtrl:OnSCRABoZhongYouLiInfo(protocol)
    self.data:SetActInfo(protocol)
end

--0：信息   1：播种   2：灌溉   3：领取奖励
function SowRewardCtrl:SendInfo()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SOW_REWARD, 0)
end

function SowRewardCtrl:SendSow()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SOW_REWARD, 1)
end

function SowRewardCtrl:SendWater()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SOW_REWARD, 2)
end

function SowRewardCtrl:SendGather(index)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SOW_REWARD, 3, index)
end
