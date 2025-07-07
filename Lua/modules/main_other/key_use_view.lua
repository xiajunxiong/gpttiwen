KeyUseView = KeyUseView or DeclareView("KeyUseView", "main_other/key_use")

VIEW_DECLARE_LEVEL(KeyUseView, ViewLevel.Lt)
VIEW_DECLARE_MASK(KeyUseView, ViewMask.None)

KeyUsePanel = KeyUsePanel or DeclareMono("KeyUsePanel", UIWFlushPanel)

function KeyUsePanel:KeyUsePanel()
    self.data = KeyUseData.Instance
    self.data_cares = {
        {data = self.data.key_data, func = self.FlushShowItem,init_call = true}
    }
    GuideManager.Instance:RegisterGetGuideUi("KeyUseBtn",
	function ()
		return self.BtnUse
    end)
    if self.data.open_time then
        self.open_time_ht = Invoke(function ()
            ViewMgr:CloseView(KeyUseView)
        end,self.data.open_time)
        self.data:SetOpenTime(nil)
    end
end

function KeyUsePanel:FlushShowItem()
    local item = self.data:GetItem()
    if item == nil or item.item_id == 0 then
        ViewMgr:CloseView(KeyUseView)
        return
    else
        self.item = item
    end
    if self.role_pos_event then
    EventSys:UnBind(self.role_pos_event)
    end
    if self.item:IsTreasure() then
        self.role_pos_event = EventSys:Bind(GlobalEventKeys.RolePosChange,
        BindTool.Bind(self.OnMainRolePosChange, self))
    end
    self.BtnClose:SetActive(self.item:IsEquip())
    self:FlushKeyUse()
end

function KeyUsePanel:FlushKeyUse()
    self.ItemCell:SetDataByItemId(self.item.item_id)
    UH.SetText(self.ItemName, Item.GetName(self.item.item_id) or "")
    
    if self.item:IsEquip() or self.item:IsJewelry() then
        self.BtnParam:SetCallBack(function()self:OnClickUse()end)
        self.BtnParam:TotalTime(3,TimeType.Type_Special_2,Language.MainOther.EquipUse)
    elseif self.item:IsTreasure() then
        if self.item:IsStarTreasureMap() then
            UH.SetText(self.BtnName, Language.StarTreasureMap.MapBtnName)
        elseif self.item:IsSuperMap() and not GuideManager.Instance:IsGuide() then
            self.BtnParam:SetCallBack(function()self:OnClickUse()end)
            self.BtnParam:TotalTime(5,TimeType.Type_Special_2,Language.MainOther.MapUse)
        else
            UH.SetText(self.BtnName, TreasureMapData.Instance:GetNewFlag() and Language.MainOther.MapBtnUseName or Language.MainOther.MapBtnName)
        end
    elseif self.item.item_id == 28506 then
        if self.item.vo.level and self.item.vo.level ~= 0 then
            UH.SetText(self.ItemName,Format(Language.Reward.RewardUpgrade.GiftName,self.item.vo.level))
            UH.SetText(self.BtnName,Language.FirstCharge.BtnGo[2])
        else
            ViewMgr:CloseView(KeyUseView)
        end
    elseif self.data.btn_name ~= nil then
        UH.SetText(self.BtnName, self.data.btn_name)
        self.data:SetBtnName(nil)
    else
        UH.SetText(self.BtnName, Language.Bag.Use)
    end
end

function KeyUsePanel:OnMainRolePosChange()
    if self.item == nil then return end
    if self.item.param and self.item.param.x == nil then return end
    local pos = MainPlayer:GetPos()
    local target = SceneCtrl.Instance:CurScene():PosToClint(self.item.param.x, self.item.param.y)
    local dis = GameMath.GetDistance(pos.x, pos.z, target.x, target.z)
    if dis > 4 then
        MainOtherCtrl.Instance:CloseKeyUse()
        EventSys:UnBind(self.role_pos_event)
    end
end

function KeyUsePanel:OnClickUse()
    if self.item ~= nil and self.item.item_id ~= 0 then
        self:SendUseItem()
    else
        ViewMgr:CloseView(KeyUseView)
    end
    self.data:RemoveItem()
end

function KeyUsePanel:SendUseItem()
    if self.item:IsTreasure() then
        StarTreasureMapData.Instance:SetAutoFlag(false)
        --self.item:IsStarTreasureMap() and 
        if self.item.key_use_func then
            self.item.key_use_func()
            self.item.key_use_func = nil
            return
        end
        TreasureMapCtrl.Instance:KeyUseItem(self.item:MapType(),TableCopy(self.item))
    elseif self.item:IsEquip() then
        ItemInfoCtrl.Instance:ItemInfoUseFunc(ItemInfoUseParam[ItemType.Weapon](self.item))
    elseif self.item.item_id == 28506 then
        ViewMgr:OpenViewByKey(Mod.Reward.Upgrade)
    elseif self.item:Type() == ItemType.Gift then
        local param_t = {item_id = self.item.item_id,column = self.item.column_type,index = self.item.index,num = self.item.num}
        BagCtrl.Instance:SendUseGiftReq(param_t)
    elseif self.item:Type() == ItemType.CrystalEquip then 
        ItemInfoCtrl.Instance:Data().crystal_equip = Item.Init(self.item.item_id)
        BagCtrl.Instance:UseItemByIndex(self.item.column_type or 0,self.item.index or 0)
    else
        if ShowType.PartnerPieces == self.item:ShowType() then
            PartnerData.Instance:PartnerShowListJump(true)
            PartnerData.Instance.guide_partner = PartnerData.Instance:GetPartnerActivationInfoByItemId(self.item.item_id)
        elseif ShowType.BicycleAct == self.item:ShowType() then
            -- 灵骑激活道具特殊处理
            BagData.Instance:LingQiFastUse(self.item)
            return
        end
        
        local mod_key = self.item:ModKey()
        if not StringIsEmpty(mod_key) then
            ViewMgr:OpenViewByKey(mod_key)
            ViewMgr:CloseView(KeyUseView)
            return
        end
        BagCtrl.Instance:UseItemByIndex(self.item.column_type or 0,self.item.index or 0)
    end
end

function KeyUsePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.BtnParam:CloseCountDownTime()
    EventSys:UnBind(self.role_pos_event)
    -- MainOtherCtrl.Instance:CloseKeyUse()
    -- KeyUseData.Instance:ClearData()
    TimeHelper:CancelTimer(self.open_time_ht)
end

function KeyUsePanel:OnClickClose()
    RewardData.CanUpgradeData = true
    ViewMgr:CloseView(KeyUseView)
    KeyUseData.Instance:ClearData()
end