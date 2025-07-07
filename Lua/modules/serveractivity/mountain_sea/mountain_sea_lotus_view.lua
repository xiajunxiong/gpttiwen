-- 山海觅宝
MSLotusTreasureView = MSLotusTreasureView or DeclareView("MSLotusTreasureView", "serveractivity/mountain_sea/ms_lotus_treasure_view", Mod.MountainSea.LotusTwo)
VIEW_DECLARE_LEVEL(MSLotusTreasureView, ViewLevel.L0)

function MSLotusTreasureView:OnCloseClick()
    ViewMgr:CloseView(MSLotusTreasureView)
end

function MSLotusTreasureView:OnHelpClick()
    local cfg = Config.language_cfg_auto.textdesc[135] or {}
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = cfg.desc or ""})
end

function MSLotusTreasureView:OnJumpClick()
    local mod_key = Config.shanhaimibao_auto.other[1].jump_mod_key
    ViewMgr:OpenViewByKey(mod_key)
    ViewMgr:CloseView(MSLotusTreasureView)
end

MSLotusTreasurePanel = MSLotusTreasurePanel or DeclareMono("MSLotusTreasurePanel", LotusTreasurePanel)
function MSLotusTreasurePanel:MSLotusTreasurePanel()
    self.data = ActFlowerLampData.Instance
    self.act_data = self.data.lotus_data
    self.data_cares = {
        {data = self.act_data, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushGridList, init_call = false},
    }
    self.act_type = ACTIVITY_TYPE.RAND_LOTUS_MS
end

MSLotusTreasureGridItem = MSLotusTreasureGridItem or DeclareMono("MSLotusTreasureGridItem", UIWidgetBaseItem)

function MSLotusTreasureGridItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    local is_lighten = ActFlowerLampData.Instance:IsLotusGridLighten(data)
    self.PutBtnObj:SetActive(not is_lighten)
    UH.SetIcon(self.Icon, Item.GetIconId(data.item_id))
    local is_can_put = ActFlowerLampData.Instance:IsLotusGridPut(data)
    self.TipObj:SetActive(is_can_put and not is_lighten)
end

function MSLotusTreasureGridItem:OnClick()
    if ServerActivityData.Instance:IsLotusGridPut(self.data) then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_LOTUS_MS, LotusTreasureView.Oper.ActiveGrid, self.data.line_num - 1, self.data.seq)
    else
        local item = Item.Create({item_id = self.data.activation_item_id})
        MainOtherCtrl.Instance:GetWayView({item = item})
        PublicPopupCtrl.Instance:Error(string.format(Language.ActLotus.Error3, self.data.activation_item_num, Item.GetName(self.data.activation_item_id)))
    end
end