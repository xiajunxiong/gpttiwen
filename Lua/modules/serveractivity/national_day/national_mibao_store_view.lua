MiBaoStoreView = MiBaoStoreView or DeclareView("MiBaoStoreView", "serveractivity/national_day/mibao_store_view", Mod.NationalDay.MiBao)

function MiBaoStoreView:OpenCallback()
    if ServerActivityData.Instance:GetMiBaoOpenDayFlag() == 0 then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_NATIONAL_MIBAO_STORE, 2)
    end
    self.Currency:DefaultSet()
end

function MiBaoStoreView:OnClickClose()
    ViewMgr:CloseView(MiBaoStoreView)
end

----------------------------MiBaoStorePanel----------------------------
MiBaoStorePanel = MiBaoStorePanel or DeclareMono("MiBaoStorePanel", UIWFlushPanel)
function MiBaoStorePanel:MiBaoStorePanel()
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.data.national_mibao_data, func = self.OnFlushView, keys = {"is_change", "is_need_recal_phase_flag"}}
    }
    self.NextTime:SetShowCallBack(function(t)self:FlushShowView(t)end)
end

function MiBaoStorePanel:OnFlushView()
    local phase = self.data:GetMiBaoStorePhase()
    self.NextBtn:SetActive(false)
    if phase >= 1 then
        local next_list = self.data:GetMiBaoStoreDataByPhase(phase + 1)
        if #next_list > 0 then
            self.NextTime:StampTime(self.data:GetMiBaoStoreStampTime(phase))
            self.NextList:SetData(next_list)
            self.NextBtn:SetActive(true)
        end
    end
    self.LastBtn:SetActive(phase >= 2)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_NATIONAL_MIBAO_STORE, 
    TimeType.Type_Special_4, Language.NationalMiBao.ActivityTime)
    self.CurList:SetData(self.data:GetMiBaoStoreList())
end

function MiBaoStorePanel:FlushShowView(t)
    local day = tonumber(t.day) or 0
    if day == 0 then
        self.DayShow:SetData(t.hour)
        self.HourShow:SetData(t.min)
        self.MinShow:SetData(t.s)
    else
        self.DayShow:SetData(t.day)
        self.HourShow:SetData(t.hour)
        self.MinShow:SetData(t.min)
    end
    local TimeShow = Language.NationalMiBao.TimeShow
    UH.SetText(self.TimeShow,TimeShow[day == 0 and 2 or 1])
end

function MiBaoStorePanel:OnClickItem()
    PublicPopupCtrl.Instance:Center(Language.NationalMiBao.ActivityEnd)
end

----------------------------MiBaoItem----------------------------
MiBaoItem = MiBaoItem or DeclareMono("MiBaoItem", UIWidgetBaseItem)

function MiBaoItem:SetData(data)
    data.item = Item.New(data.reward_item)
    data.times = ServerActivityData.Instance:GetMiBaoStoreHasTimes(data)
    local times = data.limit_type == 1 and data.buy_times or data.today_times
    self.Cell:SetData(data.item)
    UH.SetText(self.BuyVal, data.price)
    UH.SetText(self.PropName, data.item:Name())
    UH.SetGold(self.Icon, data.price_type == 1 and 16 or data.price_type)
    UH.SetText(self.PropVal, Format(Language.NationalMiBao.Value, data.value))
    UH.SetText(self.BuyLimit, Format(Language.NationalMiBao.Limit[data.limit_type], data.times, times))
    UIWidgetBaseItem.SetData(self, data)
    self:FlushBtnView()
end

function MiBaoItem:FlushBtnView()
    self.Buy:SetActive(self.data.times > 0)
    self.BtnInter.Interactable = self.data.times > 0
    UH.SetText(self.SellQing, self.data.times <= 0 
    and Language.NationalMiBao.Sell or nil)
end

function MiBaoItem:OnClickItem()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_NATIONAL_MIBAO_STORE, 1, self.data.seq)
end

----------------------------NextMiBaoItem----------------------------
NextMiBaoItem = NextMiBaoItem or DeclareMono("NextMiBaoItem", UIWidgetBaseItem)
function NextMiBaoItem:OnClick()
    PublicPopupCtrl.Instance:Center(Language.Appearance.ExpectTip)
end

----------------------------MiBaoTimeItem----------------------------
MiBaoTimeItem = MiBaoTimeItem or DeclareMono("MiBaoTimeItem", UIWidgetBaseItem)

function MiBaoTimeItem:SetData(data)
    if self.data and tonumber(data) ~= tonumber(self.data) then
        UH.SetText(self.LastNum,tonumber(self.data))
        self.TimeAnim:SetTrigger(APH("Show"))
    end
    UH.SetText(self.Num, tonumber(data))
    UIWidgetBaseItem.SetData(self, data)
end