PeakRoadReward = PeakRoadReward or
                     DeclareView("PeakRoadReward", "peak_road/peak_road_reward")
function PeakRoadReward:PeakRoadReward()
    self.data = PeakRoadData.Instance
end
function PeakRoadReward:LoadCallback()
    self.handle_delay = TimeHelper:AddDelayTimer(function()
        AudioMgr:PlayEffect(AudioType.UI, "treasure_hunt_open")
    end, 2.25)
    if self.data.effect_open == false then
        self.EffectTool:Play(3165002)
        self.Anima:Play("peak_road_reward_showing_A2")
        self.data.effect_open = true
    else
        self.Anima:Play("peak_road_reward_showing_A3")
    end
end
function PeakRoadReward:CloseCallback()
    TimeHelper:CancelTimer(self.handle_delay)
end
PeakRoadRewardPanel = PeakRoadRewardPanel or
                          DeclareMono("PeakRoadRewardPanel", UIWFlushPanel)
function PeakRoadRewardPanel:PeakRoadRewardPanel()
    self.data = PeakRoadData.Instance
    self.data_cares = {
        {
            data = self.data:PeakData(),
            func = self.onFlush,
            keys = {"level", "reward_refresh", "is_open"}
        },
        {
            data = self.data:PeakData(),
            func = self.FlushAchieve,
            keys = {"achieve_refresh"}
        }
    }
end
function PeakRoadRewardPanel:FlushAchieve()
    self.RedPoint:SetNum(self.data:AchieveNum())
end
function PeakRoadRewardPanel:onFlush()
    local peak_data = self.data:PeakData()
    if not peak_data.is_open then
        ViewMgr:CloseView(PeakRoadReward)
        return
    end
    local data = self.data:GetRewardList()
    self.List:SetData(data)
    self.List:JumpVertIndex(self.data:SelectIndex())
    UH.SetText(self.Level,
               string.format(Language.PeakRoad.Level, peak_data.level))
    self.Time:StampTime(peak_data.timestamp, TimeType.Type_Time_6,
                        Language.PeakRoad.Time)
end
function PeakRoadRewardPanel:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[145].desc})
end
function PeakRoadRewardPanel:OnClickClose() ViewMgr:CloseView(PeakRoadReward) end
function PeakRoadRewardPanel:OnClickGetExp()
    ViewMgr:OpenViewByKey(Mod.Activity.Daily)
    ViewMgr:CloseView(PeakRoadReward)
end
function PeakRoadRewardPanel:OnClickAchievement()
    ViewMgr:OpenView(PeakAchieveView)
end
PeakRoadRewardItem = PeakRoadRewardItem or
                         DeclareMono("PeakRoadRewardItem", UIWidgetBaseItem)
function PeakRoadRewardItem:PeakRoadRewardItem()
    self.peak_data = PeakRoadData.Instance
end
function PeakRoadRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Level, data.peak_level)
    self.List:SetData(data.items)
    self.BtnInter.Interactable = self.peak_data:CanGet(data.peak_level)
    self.RedPoint:SetActive(self.peak_data:CanGet(data.peak_level))
    UH.SetText(self.BtnTex, self.peak_data:HadGet(data.peak_level) and Language.PeakRoad.HadGet or Language.PeakRoad.Get)
end
function PeakRoadRewardItem:OnClickGet()
    local flag = self.peak_data:CanGet(self.data.peak_level)
    if flag then
        AudioMgr:PlayEffect(AudioType.UI, "chushixingxing")
    else
        AudioMgr:PlayEffect(AudioType.UI, "click_error")
    end
    PeakRoadCtrl.CSRoleTopLevelReq(1, self.data.peak_level)
end
