-- 龙魂背包
FairyDragonSoulBagView = FairyDragonSoulBagView or DeclareView("FairyDragonSoulBagView", "fairy_dragon/fairy_dragon_soul_bag_view")
function FairyDragonSoulBagView:FairyDragonSoulBagView()
    
end

function FairyDragonSoulBagView:OnCloseClick()
    ViewMgr:CloseView(FairyDragonSoulBagView)
end

-- 背包
FairyDragonSoulBagPanel = FairyDragonSoulBagPanel or DeclareMono("FairyDragonSoulBagPanel", UIWFlushPanel)
function FairyDragonSoulBagPanel:FairyDragonSoulBagPanel()
    self.data = FairyDragonSoulData.Instance
    self.dragon = FairyDragon.Instance
    self.sel_type = self.data:GetBagSelType()
    self.data_cares = {
        {data = self.data.soul_item_list, func = self.FlushPanel, init_call = false},
        {data = self.dragon.base_data, func = self.FlushPanel, init_call = false}
    }
end

function FairyDragonSoulBagPanel:Awake()
    UIWFlushPanel.Awake(self)
    local type_index = self.sel_type + 2
    self.TypeList[type_index].isOn = true
    self:FlushPanel()
end

function FairyDragonSoulBagPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SetBagSel(nil)
    self.data:SetBagSelType(nil)
end

function FairyDragonSoulBagPanel:FlushPanel()
    local count = self.data:ItemListCount()
    UH.SetText(self.NumText, string.format(Language.FairyDragon.Text10, count, MAX_DRAGON_SOUL_BAG_NUM))
    self:FlushList()
end

function FairyDragonSoulBagPanel:FlushList()
    local list = self.data:GetBagItemList(self.sel_type)
    for i = #list + 1, 25 do
        table.insert(list, Item.Create({item_id = 0}))
    end
    self.List:SetData(list)
end

function FairyDragonSoulBagPanel:OnTypeClick(type)
    self.sel_type = type
    self:FlushList()
end

function FairyDragonSoulBagPanel:OnItemClick(data)
    if self.data.item_id == 0 then
        return
    end
    self.data:SetBagSel(data)
    self:FlushList()

    local soul_type = data:SoulType()
    local hole_item = self.data:GetHoleSoulByType(soul_type)
    local data_list = {data}
    local func_list = {}
    if hole_item then
        if not data:IsInUse() then
            table.insert(func_list, ItemInfoFunc.DragonSoulRep)
            table.insert(data_list, hole_item)
        end
    else
        table.insert(func_list, ItemInfoFunc.DragonSoulUse)
    end
    table.insert(func_list, ItemInfoFunc.DragonSoulUp)
    local info = {
        func = function ()
			return func_list
        end,
        item_list = data_list,
    }
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

FairyDragonSoulBagItem = FairyDragonSoulBagItem or DeclareMono("FairyDragonSoulBagItem", UIWidgetBaseItem)
function FairyDragonSoulBagItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ItemCell:SetData(data)
    self.Show:SetActive(data.item_id ~= 0)
    if data.item_id ~= 0 then
        self.UseFlag:SetActive(data:IsInUse())
        UH.SetText(self.Level, string.format(Language.FairyDragon.Level2,data:Level()))
        self:FlushSel()
    end
end

function FairyDragonSoulBagItem:FlushSel()
    local sel_data = FairyDragonSoulData.Instance:GetBagSel()
    self.Sel:SetActive(not TableIsEmpty(sel_data) and sel_data.index == self.data.index)
end


-- 分解
FairyDragonSoulResolvePanel = FairyDragonSoulResolvePanel or DeclareMono("FairyDragonSoulResolvePanel", UIWFlushPanel)
function FairyDragonSoulResolvePanel:FairyDragonSoulResolvePanel()
    self.data = FairyDragonSoulData.Instance
    self.type_list_data = {
        {qua = ItemColor.Green},
        {qua = ItemColor.Blue},
        {qua = ItemColor.Purple},
        {qua = ItemColor.Orange},
        {qua = ItemColor.Red},
        {qua = ItemColor.Gold},
        --{qua = ItemColor.Black},
    }
    self.data_cares = {
        {data = self.data.soul_item_list, func = self.FlushPanel, init_call = false},
    }
end

function FairyDragonSoulResolvePanel:Awake()
    UIWFlushPanel.Awake(self)
    self.TypeList:SetData(self.type_list_data)
    self:FlushPanel()
end

function FairyDragonSoulResolvePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.down_time_ht)
    TimeHelper:CancelTimer(self.time_ht_2)
    self.data:ClearResolveSel()
end

function FairyDragonSoulResolvePanel:FlushPanel()
    self:FlushItemList()
end

function FairyDragonSoulResolvePanel:FlushItemList()
    local list = self.data:GetResolveItemList()
    for i = #list + 1, 25 do
        table.insert(list, Item.Create({item_id = 0}))
    end
    self.list_data = list
    self.ItemList:SetData(list)

    self:FlushResolveResult()
end

function FairyDragonSoulResolvePanel:FlushResolveResult()
    local sel_list = self.data.soul_resolve_sel_list or {}
    local num = 0
    local soul_list = {}
    local list = {}
    for i,v in pairs(sel_list) do
        if v then
            local item = self.data:GetItem(i)
            if item:Level() == 1 then
                num = num + item:ResolveClipCount()
            else
                if not soul_list[item.item_id] then
                    soul_list[item.item_id] = 0
                end
                soul_list[item.item_id] = soul_list[item.item_id] + item:ResolveSoulCount()
            end
        end
    end
    if num > 0 then
        local item_id = Config.god_dragon_auto.other[1].dragon_soul_chips
        table.insert(list,Item.Create({item_id = item_id, num = num}))
    end
    for i,v in pairs(soul_list) do
        if v > 0 then
            table.insert(list, Item.Create({item_id = i, num = v}))
        end
    end
    for i = #list + 1, 12 do
        table.insert(list, Item.Create({item_id = 0}))
    end
    self.ResultList:SetData(list)
end

function FairyDragonSoulResolvePanel:OnTypeClick(data, value)
    local qua = data.qua
    for i,v in pairs(self.list_data or {}) do
        if v.item_id ~= 0 then
            if v:Color() == qua then
                self.data:SetResolveSel(v.index, value)
            end
        end
    end
    self:FlushItemList()
end

function FairyDragonSoulResolvePanel:OnItemClick(data)
    if self.is_downed then
        self.is_downed = nil
        return
    end
    local is_sel = self.data:GetResolveSel(data.index)
    if not is_sel then
        if data:Color() >= 5 then
            PublicPopupCtrl.Instance:AlertTip(Language.FairyDragon.Tip3, function ()
                self.data:SetResolveSel(data.index, true)
                self:FlushItemList()
            end)
        else
            self.data:SetResolveSel(data.index, true)
        end
    else
        self.data:SetResolveSel(data.index, false)
    end
    self:FlushItemList()
end

function FairyDragonSoulResolvePanel:OnDown(data)
    TimeHelper:CancelTimer(self.down_time_ht)
    self.down_time_ht = Invoke(function ()
        local info = {
            item_list = {data},
        }
        ItemInfoCtrl.Instance:ItemInfoView(info)
        self.is_downed = true
        TimeHelper:CancelTimer(self.time_ht_2)
        self.time_ht_2 = Invoke(function ()
            self.is_downed = nil
        end)
    end, 1)
end

function FairyDragonSoulResolvePanel:OnUp()
    TimeHelper:CancelTimer(self.down_time_ht)
end

function FairyDragonSoulResolvePanel:OnResolveClick()
    FairyDragonCtrl.Instance:SendResolveSoulReq()
    self.data:ClearResolveSel()
end

function FairyDragonSoulResolvePanel:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(385)
end

FairyDragonSoulResolveType = FairyDragonSoulResolveType or DeclareMono("FairyDragonSoulResolveType", UIWidgetBaseItem)
function FairyDragonSoulResolveType:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local qua = data.qua
    local text = ColorStr(Language.FairyDragon.ColorLevel2[qua], QualityColorStr[qua])
    UH.SetText(self.Text, text)
end


FairyDragonResolveBagItem = FairyDragonResolveBagItem or DeclareMono("FairyDragonResolveBagItem", FairyDragonSoulBagItem)
function FairyDragonResolveBagItem:FlushSel()
    if self.data.item_id == 0 then
        self.Sel:SetActive(false)
        return
    end
    local is_sel = FairyDragonSoulData.Instance:GetResolveSel(self.data.index)
    self.Sel:SetActive(is_sel)
end