
YunZeErSanShiView = YunZeErSanShiView or DeclareView("YunZeErSanShiView","serveractivity/yunzeersanshi", Mod.YunZe.YunZeErSanShi)
VIEW_DECLARE_LEVEL(YunZeErSanShiView, ViewLevel.L1)

function YunZeErSanShiView:LoadCallback()
end

function YunZeErSanShiView:OnClickClose()
    ViewMgr:CloseView(YunZeErSanShiView)
end

YunZeErSanShiViewF = YunZeErSanShiViewF or DeclareMono("YunZeErSanShiViewF", UIWFlushPanel)

function YunZeErSanShiViewF:YunZeErSanShiViewF()
    self.data = YunZeErSanShiData.Instance
    self.act_data = ActivityData.Instance
    self.language = Language.YunZeErSanShi

    self.data_cares = {
        {data = self.data.info_sm, func = self.FlushInfo, keys = {"info"}},
        {data = self.data.info_sm, func = self.FlushDraw, keys = {"draw"}, init_call = false},
        {data = self.act_data.activity_status_event, func = self.FlushState},
    }

    YunZeErSanShiViewF.is_rolling = false
    self.rolling_index = 1

    self.SHOW_ITEM_MAX = 6
    self.INTERVAL_MIN = 0.1
    self.INTERVAL_MAX = 0.2
    self.INTERVAL_PER = 0.05
end

function YunZeErSanShiViewF:Awake()
    UIWFlushPanel.Awake(self)

    YunZeErSanShiData.Instance.login_flag = true
    YunZeErSanShiData.Instance.info_sm.info:Notify()
    
    self:FlushRewards()
    self:FlushRemainTime()
end

function YunZeErSanShiViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    TimeHelper:CancelTimer(self.timer_ct)
    TimeHelper:CancelTimer(self.timer_rt)
end

function YunZeErSanShiViewF:FlushRewards()
    local list = self.data:GetYunZeErSanShiRewardList()
    for i = 1, self.ShowItems:Length() do
        if list[i] then
            self.ShowItems[i]:SetData(list[i])
        end
    end
    self.ShowItems[self.rolling_index]:SetSelected(true)
end

function YunZeErSanShiViewF:FlushInfo()
    local info = self.data.info_sm.info
    local is_finish = info.times == self.data:ConfigFrequency()
    self.BtnGoInter.Interactable = (not is_finish) or (is_finish and 1 == info.is_turntable_reward)
    local btn_go_show = ""
    if not is_finish then
        btn_go_show = self.language.BtnGoShow.go
    elseif 1 == info.is_turntable_reward then
        btn_go_show = self.language.BtnGoShow.draw
    else
        local activity_status = self.act_data:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_YUN_ZE_ER_SAN_SHI)
        local end_time = activity_status and activity_status.param_2 or 0
        local last_time = end_time - TimeHelper.GetServerTime()
        local ft = TimeHelper.FormatDHMS(last_time)
        local last_day = ft.day
        btn_go_show = last_day > 0 and self.language.BtnGoShow.finish_today or self.language.BtnGoShow.finish_all
    end
    self.BtnGoShow.text = btn_go_show
    self.ProgressShow.text = string.format(self.language.ProgressShow, info.times, self.data:ConfigFrequency())
end


function YunZeErSanShiViewF:FlushDraw()
    -- if self.SkipToggle.isOn then
    --     for i = 1, self.ShowItems:Length() do
    --         self.ShowItems[i]:RollingEnd(self.data.info_sm.draw.seq)
    --     end
    -- else
        YunZeErSanShiViewF.is_rolling = true
        local result_index = 1
        for i = 1, self.ShowItems:Length() do
            result_index = self.ShowItems[i]:IsResultItem(self.data.info_sm.draw.seq) and i or result_index
        end
        self:Rolling(result_index, self.INTERVAL_MIN, false)
    -- end
end

function YunZeErSanShiViewF:Rolling(result_index, interval, init_call)
    TimeHelper:CancelTimer(self.timer_rt)
    self.timer_rt = TimeHelper:AddRunTimer(function ()
        if self.rolling_index == result_index then
            TimeHelper:CancelTimer(self.timer_rt)
            if interval > self.INTERVAL_MAX then
                self.ShowItems[self.rolling_index]:SetSelected(true)
                self.ShowItems[self.rolling_index]:ShowSelEffect()
                YunZeErSanShiViewF.is_rolling = false
                AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemGet)
                return
            end
            interval = interval + self.INTERVAL_PER
            local to_index = self.rolling_index + 1
            self.rolling_index = to_index > self.SHOW_ITEM_MAX and 1 or to_index
            self.ShowItems[self.rolling_index]:SetSelected(true)
            self:Rolling(result_index, interval, false)
        else
            local to_index = self.rolling_index + 1
            self.rolling_index = to_index > self.SHOW_ITEM_MAX and 1 or to_index
            self.ShowItems[self.rolling_index]:SetSelected(true)
        end
    end, interval, nil, init_call)
end

function YunZeErSanShiViewF:FlushState()
    if self.act_data.activity_status_event.act_type ~= ACTIVITY_TYPE.RAND_YUN_ZE_ER_SAN_SHI then return end
    local activity_status = self.act_data:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_YUN_ZE_ER_SAN_SHI)
    local status = activity_status and activity_status.status or ActStatusType.Close
    if ActStatusType.Open == status then
        self:FlushRemainTime()
    else
        ViewMgr:CloseView(YunZeErSanShiView)
    end
end

function YunZeErSanShiViewF:FlushRemainTime()
    TimeHelper:CancelTimer(self.timer_ct)
    local activity_status = self.act_data:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_YUN_ZE_ER_SAN_SHI)
    local end_time = activity_status and activity_status.param_2 or 0
    if end_time - TimeHelper.GetServerTime() <= 0 then
        self.RemainTime.text = string.format(self.language.RemainTime, 0, 0)
        return
    end
    self.timer_ct = TimeHelper:AddCountDownCT(function ()
        local last_time = end_time - TimeHelper.GetServerTime()
        local ft = TimeHelper.FormatDHMS(last_time)
        self.RemainTime.text = string.format(self.language.RemainTime, ft.day, ft.hour)
    end, function ()
        self.RemainTime.text = string.format(self.language.RemainTime, 0, 0)
    end, end_time, 1)
end

function YunZeErSanShiViewF:OnClickGo()
    local info = self.data.info_sm.info
    if 1 == info.is_turntable_reward then
        if YunZeErSanShiViewF.is_rolling then return end
        ServerActivityCtrl.Instance:SendActivityYunZeErSanShiReqRolling()
    else
        self.data:AutoToNpc()
        ViewMgr:CloseView(YunZeErSanShiView)
        if ViewMgr:IsOpen(YunZeView) then
            ViewMgr:CloseView(YunZeView)
        end
    end
end

-- function YunZeErSanShiViewF:OnTogSkip()
--     UnityPlayerPrefs.SetInt(PrefKeys.YunZeErSanShiSkip(), self.SkipToggle.isOn and 1 or 0)
-- end

YunZeErSanShiViewRewardItem = DeclareMono("YunZeErSanShiViewRewardItem", UIWidgetBaseItem)

function YunZeErSanShiViewRewardItem:YunZeErSanShiViewRewardItem()
    self.act_data = ServerActivityCtrl.Instance:Data()
end

function YunZeErSanShiViewRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(Item.Create(data.reward_item))
end

function YunZeErSanShiViewRewardItem:RollingEnd(seq)
    if self.data and self.data.seq == seq then
        self:SetSelected(true)
    end
end

function YunZeErSanShiViewRewardItem:IsResultItem(seq)
    return self.data and self.data.seq == seq
end

function YunZeErSanShiViewRewardItem:ShowSelEffect()
    self.EffectTool:Play(6161015)
end