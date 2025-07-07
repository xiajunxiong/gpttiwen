SowRewardView = SowRewardView or DeclareView("SowRewardView", "serveractivity/sow_reward/sow_reward", Mod.MayDay.SowReward)
function SowRewardView:OnClickClose()
    ViewMgr:CloseView(SowRewardView)
end

SowRewardPanel = SowRewardPanel or DeclareMono("SowRewardPanel", UIWFlushPanel)
function SowRewardPanel:SowRewardPanel()
    self.data_cares = {
        {data = SowRewardData.Instance.act_info, func = self.FlushView},
    }
end

function SowRewardPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:ShowActTime()
    self.ItemList1:SetData(SowRewardData.Instance:GetGatherRewardList())
    self.ItemList2:SetData(SowRewardData.Instance:GetWaterRewardList())

    local other_cfg = SowRewardData.Instance:OtherConfig()
    UH.SetText(self.WaterTimesDesc, Format(Language.SowReward.WaterTimesDesc, self:ConverTime(SowRewardData.Instance:GetWaterWait())))
    UH.SetText(self.SowTimesDesc, Format(Language.SowReward.SowTimesDesc, other_cfg.frequency1))
end

function SowRewardPanel:ConverTime(_time)
    local hour = math.floor(_time / TIME_HOUR_SECOND_NUM)
    if hour == 0 then
        return Format(Language.SowReward.WaterTimeT[2], math.floor(_time / TIME_MINUTE_SECOND_NUM))
    end
    return Format(Language.SowReward.WaterTimeT[1], hour)
end

function SowRewardPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.water_timer)
    self.water_timer = nil
    TimeHelper:CancelTimer(self.do_water_timer)
    self.do_water_timer = nil
end

function SowRewardPanel:FlushView()
    local act_info = SowRewardData.Instance.act_info
    UH.SetText(self.WaterTimes, Format(Language.SowReward.WaterTimes, SowRewardData.Instance:GetWaterTimes()))
    UH.SetText(self.SowTimes, Format(Language.SowReward.SowTimes, SowRewardData.Instance:GetSowTimes()))
    self.WaterRedNum:SetNum(SowRewardData.Instance:GetWaterTimes())
    self.SowRedNum:SetNum(SowRewardData.Instance:GetSowTimes())

    for i = 1, 4 do
        local data = {index = i - 1}
        data.timestamp = SowRewardData.Instance:GetSowTimestamp(data.index)
        self.GrassList[i]:SetData(data)
    end
    self:FlushWaterTimes()
end

function SowRewardPanel:DoWater()
    for i = 1, 4 do
        self.GrassList[i]:DoWater()
    end
end
function SowRewardPanel:StopWather()
    for i = 1, 4 do
        self.GrassList[i]:StopWater()
    end
end

function SowRewardPanel:ShowActTime()
    local open_time = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_SOW_REWARD)
    if open_time > 0 then
        local open_t = os.date("*t", open_time)
        local end_t = os.date("*t", ActivityData.GetSwitchStampTime(ACTIVITY_TYPE.RAND_SOW_REWARD) - 1)
        UH.SetText(self.ActTime, Format(Language.SowReward.ActTime, open_t.month, open_t.day, end_t.month, end_t.day))
    else
        UH.SetText(self.ActTime, Format(Language.SowReward.ActTime, 1, 1, 1, 1))
    end
end

function SowRewardPanel:OnClickWater()
    local can_val = SowRewardData.Instance:IsCanWater()
    if can_val == -1 then
        PublicPopupCtrl.Instance:Center(Language.SowReward.WaterError3)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
    elseif can_val == 0 then
        PublicPopupCtrl.Instance:Center(Language.SowReward.WaterError1)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
    elseif can_val == 2 then
        PublicPopupCtrl.Instance:Center(Language.SowReward.WaterError2)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
    else
        if self.do_water_timer ~= nil then
            return
        end
        self:DoWater()
        local function func_delay()
            self.do_water_timer = nil
            self:StopWather()
            SowRewardCtrl.Instance:SendWater()
        end
        self.do_water_timer = Invoke(func_delay, 3)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.EquipEquipment)
    end
end

function SowRewardPanel:OnClickSow()
    local can_val = SowRewardData.Instance:IsCanSow()
    if can_val == -1 then
        PublicPopupCtrl.Instance:Center(Language.SowReward.SowError2)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
    elseif can_val == 0 then
        PublicPopupCtrl.Instance:Center(Language.SowReward.SowError1)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
    else
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoOpen)
        SowRewardCtrl.Instance:SendSow()
    end
end

--倒计时，刷新下次浇水时间
function SowRewardPanel:FlushWaterTimes()
    if SowRewardData.Instance:GetWaterTimes() > 0 then
        return
    end
    local next_time = SowRewardData.Instance.act_info.next_refresh_timestamp
    if next_time > TimeHelper.GetServerTime() then
        local function func_delay()
            self.water_timer = nil
            self:FlushView()
        end
        TimeHelper:CancelTimer(self.water_timer)
        self.water_timer = Invoke(func_delay, next_time - TimeHelper.GetServerTime())
    end
end

SowRewardGrassItem = SowRewardGrassItem or DeclareMono("SowRewardGrassItem", UIWidgetBaseItem)
function SowRewardGrassItem:SowRewardGrassItem()
    self.State1Timer:SetCallBack(BindTool.Bind1(self.RipeTimeOver, self))
    self.State1Timer:SetShowCallBack(BindTool.Bind1(self.RipeTimeShow, self))
    self.open_time = TimeHelper.GetServerTime()
end

function SowRewardGrassItem:OnDestroy()
    TimeHelper:CancelTimer(self.flush_timer)
    self.flush_timer = nil
    TimeHelper:CancelTimer(self.delay_effect)
    self.delay_effect = nil
end

function SowRewardGrassItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if self.data.timestamp == 0 then
        self.IconImg:SetObjActive(false)
        self.State1Timer:SetObjActive(false)
        self.State2:SetActive(false)
        TimeHelper:CancelTimer(self.flush_timer)
        self.flush_timer = nil
        return
    else
        self.IconImg:SetObjActive(true)
        local state = SowRewardData.Instance:GetGrassImage(data.timestamp)
        UH.SpriteName(self.IconImg, "Grass" .. state)
        if SowRewardData.Instance:IsGrassRipe(data.timestamp) then
            self:ShowRipe(true)
            self.State1Timer:CloseCountDownTime()
            self.State1Timer:SetObjActive(false)
            self.State2:SetActive(true)
            TimeHelper:CancelTimer(self.flush_timer)
            self.flush_timer = nil
        else
            self:ShowRipe(false)
            self.State2:SetActive(false)
            local ripe_time = SowRewardData.Instance:GetRipeTimestamp(data.timestamp)
            self.State1Timer:SetObjActive(true)
            self.State1Timer:StampTime(ripe_time)
            TimeHelper:CancelTimer(self.flush_timer)
            local next_time = SowRewardData.Instance:GetNextStageTimestamp(data.timestamp)
            if next_time ~= nil then
                self.flush_timer = Invoke(BindTool.Bind1(self.RipeTimeOver, self), next_time - TimeHelper.GetServerTime())
            end
        end
    end
end

function SowRewardGrassItem:OnClickGather()
    SowRewardCtrl.Instance:SendGather(self.data.index)
end

function SowRewardGrassItem:DoWater()
    if self.data.timestamp == 0 or SowRewardData.Instance:IsGrassRipe(self.data.timestamp) then
        return
    end
    self.WaterEffect:SetActive(true)
end
function SowRewardGrassItem:StopWater()
    self.WaterEffect:SetActive(false)
end

function SowRewardGrassItem:ShowRipe(is_ripe)
    if is_ripe then
        if self.open_time < TimeHelper.GetServerTime() then
            TimeHelper:CancelTimer(self.delay_effect)
            self.delay_effect = nil
            self.RipeEffect:SetActive(true)
        elseif self.delay_effect == nil then
            local function func_delay()
                self.delay_effect = nil
                self.RipeEffect:SetActive(true)
            end
            self.delay_effect = Invoke(func_delay, 0.5)
        end
    else
        TimeHelper:CancelTimer(self.delay_effect)
        self.delay_effect = nil
        self.RipeEffect:SetActive(false)
    end
end

function SowRewardGrassItem:RipeTimeShow(time_t)
    if tonumber(time_t.hour) == 0 then
        return Format(Language.SowReward.RipeTime2, time_t.min, time_t.s)
    end
    return Format(Language.SowReward.RipeTime1, time_t.hour, time_t.min)
end
function SowRewardGrassItem:RipeTimeOver()
    self:SetData(self:GetData())
end

SowRewardShowItem = SowRewardShowItem or DeclareMono("SowRewardShowItem", UIWidgetBaseItem)
function SowRewardShowItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ItemCell:SetData(data)
end
