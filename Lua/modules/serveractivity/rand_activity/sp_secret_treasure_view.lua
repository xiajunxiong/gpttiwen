SpSecretTreasureView = SpSecretTreasureView or DeclareView("SpSecretTreasureView", "serveractivity/sp_secret_treasure_view", Mod.RandActivity.SpSecretTreasure)

function SpSecretTreasureView:LoadCallback(param_t)
    self.Tabs[1]:Trigger()
end

function SpSecretTreasureView:OnClickClose()
    ViewMgr:CloseView(SpSecretTreasureView)
end

SpSecretTreasurePanel = SpSecretTreasurePanel or DeclareMono("SpSecretTreasurePanel", UIWFlushPanel)
function SpSecretTreasurePanel:SpSecretTreasurePanel()
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.data.secret_treasure_data, func = self.onFlush}
    }
    self.Time:SetShowCallBack(function(t)
        return t.day > 0 and string.format(Language.SecretTreasure.LongDay, t.day, t.hour, t.min, t.s) or string.format(Language.SecretTreasure.DayLong, t.hour, t.min, t.s)
    end)
    self.index = 0
    self.need_show_list = true
end

function SpSecretTreasurePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.end_ht)
end

function SpSecretTreasurePanel:onFlush()
    TimeHelper:CancelTimer(self.end_ht)
    local main_cfg = self.data:GetSecrectTreasureCfg()
    if self.data.secret_treasure_data.phase > 1 then
        self.Tabs[3]:SetActive(true)
        self.Line:SetActive(true)
        self.Tail:SetActive(false)
    else
        self.Tabs[3]:SetActive(false)
        self.Line:SetActive(false)
        self.Tail:SetActive(true)
    end
    if not self.data:GetSecrectWithNextPhase() then
        self.Tabs[2]:SetActive(false)
        self.Line:SetActive(false)
        self.Tail:SetActive(true)
    end
    if self.need_show_list then
        self.ListTotal:SetActive(false)
    end
    if self.index == 0 then
        self.list_num = #main_cfg.cur_list
        self.List:SetData(main_cfg.cur_list)
        self.end_ht = TimeHelper:AddRunTimer(BindTool.Bind(self.FlushPageJump, self), 0.2)
    elseif self.index == 2 then
        self.list_num = #main_cfg.last_list
        self.List:SetData(main_cfg.last_list)
        self.end_ht = TimeHelper:AddRunTimer(BindTool.Bind(self.FlushPageJump, self), 0.2)
    elseif self.index == 1 then
        self:FlushPhaseTime()
    end
    self.List:SetCompleteCallBack(function()
        if self.need_show_list then
            self.PagePart:SetLocalPosition(Vector3.zero)
            self.ListTotal:SetActive(true)
            self.SellShow:Play()
            self.need_show_list = false
        end        
    end)
end

function SpSecretTreasurePanel:FlushPhaseTime()
    self.Time:StampTime(self.data:GetSecretTreasureStampTime(), TimeType.Type_Time_2, Language.SecretTreasure.NextPhase, Language.SecretTreasure.QuickNext)
end

function SpSecretTreasurePanel:FlushPageJump()
    local cur_page_offset = self.PagePart:GetLocalPosition()
    self.BtnLeft:SetActive(not (cur_page_offset.x > -220))
    self.BtnRight:SetActive(not (cur_page_offset.x < -(self.list_num - 4) * 220))
end

function SpSecretTreasurePanel:OnClickPage(index)
    local cur_page_offset = self.PagePart:GetLocalPosition()
    local width = 229
    if index == 0 then
        if cur_page_offset.x <= -width * 4 then
            cur_page_offset.x = cur_page_offset.x + width * 4
        else
            cur_page_offset.x = 0
        end
    else
        if self.list_num - 8 > 0 and cur_page_offset.x >= -(self.list_num - 8) * width then
            cur_page_offset.x = cur_page_offset.x - width * 4
        else
            cur_page_offset.x = -(self.list_num - 4) * width
        end
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
    self.PagePart:SetLocalPosition(cur_page_offset)
end

function SpSecretTreasurePanel:OnClickTab(index)
    self.need_show_list = true
    self.index = index
    local flag = index == 0 or index == 2
    self.CenterPart:SetActive(flag)
    self.NextPart:SetActive(not flag)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
    self:onFlush()
end

function SpSecretTreasurePanel:OnClickBuy(data)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    if data.price_type == 3 then
        if ChannelAgent ~= nil then
            ChannelAgent:RequestPay(data.price, BUY_ITEM_TYPE.TYPE_SECRECT_TREASURE, data.seq)
        end
    else
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SECRET_TREASURE, 1, data.type, data.seq)
    end
end

----------------------------SpseTreasureItem----------------------------
SpseTreasureItem = SpseTreasureItem or DeclareMono("SpseTreasureItem", UIWidgetBaseItem)

function SpseTreasureItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.gameObject.name = "sell_item_" .. data.panel_index
    local item = Item.Create(data.reward_item)
    self.Cell:SetData(item)
    UH.SetText(self.Name, item:Name())
    UH.SetText(self.OriginPrice, Language.SecretTreasure.Origin .. (data.price_type == 3 and data.value / 10 or data.value))
    self.PriceImg:SetActive(data.price_type == 3)
    self.PriceType:SetObjActive(data.price_type < 3)
    if data.price_type < 3 then
        UH.SpriteName(self.PriceType, data.price_type == 0 and "YuanBao" or "coin")
    end
    UH.SetText(self.Price, data.price_type == 3 and data.price / 10 or data.price)
    local is_cur = ServerActivityData.Instance.secret_treasure_data.phase == data.phase
    self.Pass:SetActive(not is_cur)
    self.Cur:SetActive(is_cur)
    if is_cur then
        local times = ServerActivityData.Instance:GetSecrectTreasureBuyTime(data.limit_type == 2 and data.seq + 1 or data.type + 1, data.limit_type)
        local limited_str = data.limit_type == 1 and Language.SecretTreasure.ActLimited or Language.SecretTreasure.DayLimited
        local limited_time = data.limit_type == 1 and data.buy_times or data.today_times
        UH.SetText(self.Limited, limited_str .. " " .. ((limited_time - times) > 0 and (limited_time - times) or 0) .. "/" .. limited_time)
        self.Mask:SetActive(limited_time == times)
    else
        local times, all_times = ServerActivityData.Instance:GetLastPhaseBuy(data.type)
        local limited_str = Language.SecretTreasure.LastTimeBuy .. " " .. times .. "/" .. all_times
        UH.SetText(self.LastTime, limited_str)
        if times == all_times then
            UH.SetText(self.LastTips, Language.SecretTreasure.BuyComplte)
        else
            UH.SetText(self.LastTips, times == 0 and Language.SecretTreasure.BuyNone or Language.SecretTreasure.BuySome)
        end
        self.Mask:SetActive(false)
    end
end