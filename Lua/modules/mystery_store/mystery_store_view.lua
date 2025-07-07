MysteryStoreView = MysteryStoreView or DeclareView("MysteryStoreView", "mystery_store/MysteryStoreView", Mod.MysteryStore.Main)

function MysteryStoreView:MysteryStoreView()
    self.Currency:SetCurrency(CurrencyType.LuckCoin, CurrencyType.MysteryFlush)
end

function MysteryStoreView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[105].desc)
end

function MysteryStoreView:OnClickPreview()
    ViewMgr:OpenView(MysteryStorePreview)
end

function MysteryStoreView:OnClickClose()
    MysteryStoreData.FlushAudioName = nil
    MysteryStoreCtrl.IsMysteryStoreItem = nil
    ViewMgr:CloseView(MysteryStoreView)
end

----------------------------MysteryStorePanel----------------------------
MysteryStorePanel = MysteryStorePanel or DeclareMono("MysteryStorePanel", UIWFlushPanel)

function MysteryStorePanel:MysteryStorePanel()
    self.data = MysteryStoreData.Instance
    self.data_cares = {
        {data = self.data.item_group_data,func = self.OnFlushPanel, init_call = false},
        {data = BagData.Instance.item_grid,func = self.OnUpdateItemShow},
        {data = self.data.mystery_store_data, func = self.StartCountDown, init_call = false},
    }

    self.Time:SetShowCallBack(function(t)
        if tonumber(t.hour) > 0 then
            return t.hour .. Language.MysteryStore.Hour .. t.min .. Language.MysteryStore.Min
        else
            return t.min .. Language.MysteryStore.Min .. t.s .. Language.MysteryStore.Sec
        end
    end)
    self.Time:SetCallBack(BindTool.Bind(self.TimeCompleteCallback, self))
    self:StartCountDown()
end

function MysteryStorePanel:StartCountDown()
    self.ObjBtnUpdate:SetActive(true)
    self.ObjTime:SetActive(true)
    self.Time:CloseCountDownTime()
    self.Time:StampTime(self.data.mystery_store_data.next_free_refresh_time, TimeType.Type_Time_3)
end

function MysteryStorePanel:TimeCompleteCallback()
    self.ObjBtnUpdate:SetActive(false)
    self.ObjTime:SetActive(false)
end

function MysteryStorePanel:Awake()
    UIWFlushPanel.Awake(self)
    self.List:SetData({self.data:GetItemList()})
    UH.SetText(self.TxtUseCount, Format(Language.MysteryStore.TodayFlushCount, self.data:GetRefreshNum()))
end

function MysteryStorePanel:OnFlushPanel()
    MysteryStoreData.FlushAudioName = nil
    self.List:SetData({self.data:GetItemList()})
    UH.SetText(self.TxtUseCount, Format(Language.MysteryStore.TodayFlushCount, self.data:GetRefreshNum()))
    self:OnUpdateItemShow()
end

function MysteryStorePanel:OnUpdateItemShow()
    local config = self.data:GetUpdateCfg()
    local is_enought_item = Item.GetNum(config.item.item_id) >= config.item.num
    if is_enought_item then
        self:ShowItemBuy(config)
    else
        self:ShowYuanBaoBuy(config)
    end
end

function MysteryStorePanel:ShowItemBuy(config)
    UH.SetText(self.TxtConsume, config.item.num)
    UH.SpriteName(self.ImgSet, HuoBi[CurrencyType.MysteryFlush])
end

function MysteryStorePanel:ShowYuanBaoBuy(config)
    UH.SetText(self.TxtConsume, config.manual_price)
    UH.SpriteName(self.ImgSet, HuoBi[CurrencyType.Gold])
end

function MysteryStorePanel:FreeFlush()
    MysteryStoreCtrl.Instance:SendMysteryStoreReq(0)
end

function MysteryStorePanel:OnClickOpenFlush()
    AudioMgr:PlayEffect(AudioType.UI,CommonAudio.ToggleOn)
    local config = self.data:GetUpdateCfg()
    if Item.GetNum(config.item.item_id) >= config.item.num then
        MysteryStoreCtrl.IsMysteryStoreItem = nil
        MysteryStoreCtrl.Instance:SendMysteryStoreReq(0)
    else
        local dialog_info = {
            content = Format(Language.MysteryStore.ConsumeStr, config.manual_price),
            confirm = {
                func = function()
                    if RoleData.Instance:GetCurrency(CurrencyType.Gold) >= config.manual_price then
                        MysteryStoreCtrl.Instance:SendMysteryStoreReq(0)
                        ViewMgr:CloseView(DialogTipsView)
                    else
                        MallCtrl.IsNotCurrency(CurrencyType.Gold, config.manual_price)
                    end
                    MysteryStoreCtrl.IsMysteryStoreItem = nil
                end
            },
            cancel = {
                func = function()
                    ViewMgr:CloseView(DialogTipsView)
                end
            },
            new_tnr = DTNewTodayNotRemind.MysteryStoreFlush,
        }
        PublicPopupCtrl.Instance:DialogTips(dialog_info)
    end
end

----------------------------MysteryStorePageItem----------------------------
MysteryStorePageItem = MysteryStorePageItem or DeclareMono("MysteryStorePageItem", UIWidgetBaseItem)
function MysteryStorePageItem:SetData(data)
    if not MysteryStoreCtrl.IsMysteryStoreItem then
        self.List:SetObjActive(false)
    end
    self.List:SetCompleteCallBack(function()
        if not MysteryStoreCtrl.IsMysteryStoreItem then
            self.List:SetObjActive(true)
            self.Anim:Stop()
            self.Anim:Play()
            if MysteryStoreData.FlushAudioName ~= nil then
                TimeHelper:CancelTimer(self.delay_audio_handler)
                self.delay_audio_handler = TimeHelper:AddDelayTimer(function()
                    if MysteryStoreData.FlushAudioName ~= nil then
                        AudioMgr:PlayEffect(AudioType.UI,MysteryStoreData.FlushAudioName)
                    end
                end, 0.8)
            end
        end
    end)
    self.List:SetData(data)
end

function MysteryStorePageItem:OnDestroy()
    TimeHelper:CancelTimer(self.delay_audio_handler)
end

----------------------------MysteryStoreItem----------------------------
MysteryStoreItem = MysteryStoreItem or DeclareMono("MysteryStoreItem", UIWidgetBaseItem)
function MysteryStoreItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if self.effect_handle then
        self.EffectTool:Stop(self.effect_handle)
    end
    self.gameObject.name = "StoreItem" .. data.index
    data.cfg.item.param = {}
    data.cfg.item.param.attr_list = {}
    for i, v in ipairs(data.attr_list) do
        table.insert(data.cfg.item.param.attr_list, v)
    end
    data.item = Item.Create(data.cfg.item)
    self.Cell:SetData(data.item)
    UH.SetText(self.Name, data.item:QuaName(true))
    UH.SetText(self.Price, data.cfg.price)
    UH.SetText(self.RemainNum, data.RemainNum)
    self.BtnInter.Interactable = data.RemainNum > 0
    if data.cfg.is_special == 1 and data.RemainNum > 0 then
        MysteryStoreData.FlushAudioName = CommonAudio.PartnerLockOpen
    end
    if not MysteryStoreCtrl.IsMysteryStoreItem then
        TimeHelper:CancelTimer(self.delay_effect_handler)
        self.delay_effect_handler = TimeHelper:AddDelayTimer(function()
            if data.cfg.is_special == 1 and data.RemainNum > 0 then
                if self.EffectTool ~= nil then
                    self.effect_handle = self.EffectTool:Play(100108206)
                end
            end
        end, 0.8)
    end
end

function MysteryStoreItem:OnClick()
    local convert_item = Item.Init(HuoBi[self.data.cfg.price_type])
    if RoleData.Instance:GetCurrency(CurrencyType.LuckCoin) < self.data.cfg.price then
        ViewMgr:OpenView(GetWayView, {item = convert_item})
        return
    end
    local dialog_info = {
        content = Format(Language.MysteryStore.ConvertTip,self.data.cfg.price,convert_item:Name(),self.data.item:QuaName()),
        confirm = {
            func = function()
                MysteryStoreCtrl.IsMysteryStoreItem = true
                MysteryStoreCtrl.Instance:SendMysteryStoreReq(1, self.data.buy_index)
                ViewMgr:CloseView(DialogTipsView)
            end
        },
        new_tnr = DTNewTodayNotRemind.MysteryStoreBuy,
    }
    PublicPopupCtrl.Instance:DialogTips(dialog_info)
end

function MysteryStoreItem:OnDestroy()
    TimeHelper:CancelTimer(self.delay_effect_handler)
end

MysteryStorePreview = MysteryStorePreview or DeclareView("MysteryStorePreview", "mystery_store/MysteryStorePreview")

function MysteryStorePreview:LoadCallback(param_t)
    local list = MysteryStoreData.Instance:GetItemUnlockList()
    local index = self:GetIndex(list)
    if index and index > 8 then
        self.List:SetCompleteCallBack(function()
            self.List:ForceJumpVertical(223 * math.floor(index/4))
        end)
    end
    self.List:SetData(list)
end

function MysteryStorePreview:GetIndex(list)
    local pass_level = DreamNotesData.Instance:GetPassLevel()
    for i,v in ipairs(list) do
        if v.dream_notes_level > pass_level then
            return i
        end
    end
end

function MysteryStorePreview:OnClickClose()
    ViewMgr:CloseView(MysteryStorePreview)
end

----------------------------MysteryStorePreItem----------------------------
MysteryStorePreItem = MysteryStorePreItem or DeclareMono("MysteryStorePreItem", UIWidgetBaseItem)

function MysteryStorePreItem:SetData(data)
    local pass_level = DreamNotesData.Instance:GetPassLevel()
    UH.SetInter(self.Inter,pass_level >= data.dream_notes_level)
    UH.SetText(self.Desc,Format(Language.DreamNotes.MysteryStoreTip,data.dream_notes_level))
    self.Desc:SetObjActive(pass_level < data.dream_notes_level)
    local item = Item.Init(data.item_id)
    item.is_grey = pass_level < data.dream_notes_level
    UH.SetText(self.Name,item:QuaName(true))
    self.Call:SetData(item)
end

--===============================MysteryStoreRewardView===============================
MysteryStoreRewardView = MysteryStoreRewardView or DeclareMono("MysteryStoreRewardView", UIWFlushPanel)

function MysteryStoreRewardView:MysteryStoreRewardView()
    local config = DreamNotesCtrl.Instance.WaitLockFunc
    if config then
        local item = Item.Init(config.item_id)
        self.Call:SetData(item)
        UH.SetText(self.Name,item:QuaName(true))
    end
    DreamNotesCtrl.Instance.WaitLockFunc = nil
end

function MysteryStoreRewardView:Awake()
    UIWFlushPanel.Awake(self)
    self.WaitLockTimer = Invoke(function()
        self.RewardAnim:Play([[MysteryStoreReward]])
    end,1.5)
end

function MysteryStoreRewardView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.WaitLockTimer)
end

function MysteryStoreRewardView:OnClickBlock()
    self.RewardAnim:Play([[MysteryStoreReward_unlock]])
    MysteryStoreData.Instance.anim_data.val = not MysteryStoreData.Instance.anim_data.val
end