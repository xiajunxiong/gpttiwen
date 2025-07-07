LabourGiftView = LabourGiftView or DeclareView("LabourGiftView", "serveractivity/labour_day/labour_gift_view", Mod.LabourDay.LabourGift)
VIEW_DECLARE_LEVEL(LabourGiftView, ViewLevel.L1)
VIEW_DECLARE_MASK(LabourGiftView, ViewMask.BgBlock)

function LabourGiftView:LoadCallback()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_LABOUR_GIFT, TimeType.Type_Time_1)
    self.RewardList:SetData(self:GetRewardList())
    local config = Config.may_day_activity_auto.other[1]
    local s_time, e_time = self:GetTime(config.time_start), self:GetTime(config.time_end)
    UH.SetText(self.TxtOpenTime, Format(Language.LabourGift.TimeTip, s_time.hour, s_time.min, e_time.hour, e_time.min))
    UH.SetText(self.TxtFlush, Format(Language.LabourGift.FlushTip, config.time_the))
    UH.SetText(self.TxtNum, config.map_case_number)
end

function LabourGiftView:OnClickClose()
    ViewMgr:CloseView(LabourGiftView)
end

function LabourGiftView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[106].desc)
end

function LabourGiftView:GetTime(time)
    local hour = math.floor(time / 100)
    local min = time % 100
    return {hour = hour, min = min < 10 and tostring("0" .. min) or min}
end

function LabourGiftView:GetRewardList()
    local list = {}
    for i,v in ipairs(Config.may_day_activity_auto.case_exhibition) do
        if v.item_show == 1 then
            table.insert(list, Item.New(v.reward_item)) 
        end
    end
    return list
end


------------------------------LabourGiftPanel------------------------------
LabourGiftPanel = LabourGiftPanel or DeclareMono("LabourGiftPanel", UIWFlushPanel)
function LabourGiftPanel:LabourGiftPanel()
    self.data = ServerActivityData.Instance
	self.data_cares = {
		{data = self.data.labour_gift_data, func = self.OnRecieveCountChange, "receive_count"},
		{data = self.data.labour_gift_data, func = self.OnRefreshBoxChange, "is_refresh_box"},
	}
end

function LabourGiftPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function LabourGiftPanel:OnRecieveCountChange()
    local frequency = Config.may_day_activity_auto.other[1].reward_frequency
    UH.SetText(self.Times, Format(Language.LabourGift.TimesTip, self.data:GetLabourGiftCount(), frequency))
end

function LabourGiftPanel:OnRefreshBoxChange()
    self.RedPoint:SetNum(self.data:GetLabourGiftNum())
end

function LabourGiftPanel:OnClickEnter()
    if LoginData.Instance:BaseData().is_on_crosssever == true then
		PublicPopupCtrl.Instance:Center(Language.SpringClimb.CrossTip)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
		return
	end
    if not self.data:IsLabourGiftRefreshingBox() then
        PublicPopupCtrl.Instance:Center(Language.LabourGift.NotTimeTip)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        return
    end
    if not self.data:IsHasLabourGiftCount() then
        PublicPopupCtrl.Instance:Center(Language.LabourGift.NotTimesTip)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        return
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    self.data.labour_gift_data.remind_check = true
    if self.data:IsHasLabourGiftCount() then
        SceneLogic.Instance:AutoToPos(SceneLogic.Instance:MainCityID(), nil, function()
            local treasure_chest_id = Config.may_day_activity_auto.case_coordinate[1].treasure_chest_id
            AutoInvokerCtrl.Instance:SetCommands({AutoCommandGather.New(treasure_chest_id)}, 0)
        end, nil, true)
    else
        SceneLogic.Instance:AutoToPos(SceneLogic.Instance:MainCityID(), nil, nil, nil, true)
    end
    ActivityRandData.Instance:CheckRandChildData()
    ViewMgr:CloseView(LabourGiftView)
    ViewMgr:CloseView(LabourDayView)
end