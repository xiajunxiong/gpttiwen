AccrssoryChangeView = AccrssoryChangeView or DeclareView("AccrssoryChangeView", "accrssory/accrssory_change", Mod.Accrssory.Change)
function AccrssoryChangeView:AccrssoryChangeView()
    self.BG:SetData(self:GetType(), Language.Accrssory.ChangeTitle, Vector2.New(934, 495))
end

--------------AccrssoryChangePanel--------------
AccrssoryChangePanel = AccrssoryChangePanel or DeclareMono("AccrssoryChangePanel", UIWFlushPanel)
function AccrssoryChangePanel:AccrssoryChangePanel()
    self.Data = AccrssoryData.Instance
    self.language = Language.Accrssory
    self.data = self.Data.change_data
    self.data_cares = {
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushByItem, init_call = false},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Accrssory), func = self.ClearSelect, init_call = false},
        {data = self.data, func = self.FlushSelect, keys = {"now", "change"}},
    }
end

function AccrssoryChangePanel:Awake()
    UIWFlushPanel.Awake(self)
end

function AccrssoryChangePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    AccrssoryData.Instance:ResetChange()
end

function AccrssoryChangePanel:ClearSelect()
    PublicPopupCtrl.Instance:Center(self.language.ItemChangeReason)
    if ViewMgr:IsOpen(AccrssoryChangeSelectView) then
        ViewMgr:CloseView(AccrssoryChangeSelectView)
    end
    self.Data:ResetChange()
    self:FlushSelect()
end

function AccrssoryChangePanel:FlushByItem()
    if TableIsEmpty(self.data.now) then
        self.Cell:SetObjActive(false)
        self.Cell:SetData({})
        UH.SetText(self.TxtName, "")
        UH.SetText(self.TxtNum, Format(Language.Common.Slash, 0, 0))
    else
        local item_id, need_num = self.Data:NeedChangeItem(self.data.now)
        local now_num = Item.GetNum(item_id)
        UH.SetText(self.TxtName, Item.GetName(item_id))
        self.Cell:SetObjActive(true)
        self.Cell:SetData(Item.Init(item_id))
        UH.SetText(self.TxtNum, Format(Language.Common.Cstr, need_num > now_num and COLORSTR.Red8 or COLORSTR.Green3, now_num, need_num))
    end
end

function AccrssoryChangePanel:FlushSelect()
    self.LeftInfo:SetData({item = self.data.now})
    self.RightInfo:SetData({item = self.data.change})
    self:FlushByItem()
end

function AccrssoryChangePanel:OnNowClick()
    self.data.select_type = AccrssoryChangeType.Now
    ViewMgr:OpenView(AccrssoryChangeSelectView)
end

function AccrssoryChangePanel:OnChangeClick()
    if TableIsEmpty(self.data.now) then
        PublicPopupCtrl.Instance:Center(self.language.ChangeNowEmpty)
        return
    end
    self.data.select_type = AccrssoryChangeType.Change
    ViewMgr:OpenView(AccrssoryChangeSelectView)
end

function AccrssoryChangePanel:OnSendChangeClick()
    if TableIsEmpty(self.data.now) then
        PublicPopupCtrl.Instance:Center(self.language.ChangeNowEmpty)
        return
    end
    if TableIsEmpty(self.data.change) then
        PublicPopupCtrl.Instance:Center(self.language.ChangeChangeEmpty)
        return
    end
    local item_id, need_num = self.Data:NeedChangeItem(self.data.now)
    local now_num = Item.GetNum(item_id)
    if need_num > now_num then
        local need_num = need_num - now_num
        PublicPopupCtrl.Instance:Center(self.language.MaterialTip[2])
        ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = item_id, jump_need_num = need_num})})
        return
    end
    PublicPopupCtrl.Instance:AlertTip(Format(self.language.ChangeTip, self.data.now:QuaName(), self.data.change:QuaName()), function()
        local param_t = {}
        param_t.req_type = AccrssoryReqType.Change
        param_t.param_1 = self.data.now.in_bag and 0 or 1
        param_t.param_2 = self.data.now.in_bag and self.data.now:GridColumn() or 0
        param_t.param_3 = self.data.now.in_bag and self.data.now:GridIndex() or self.data.now:AccrssoryType()
        param_t.param_4 = self.data.change.item_id
        AccrssoryCtrl.Instance:SendAccrssoryReq(param_t)
        ViewMgr:CloseView(AccrssoryChangeView)
        MainOtherCtrl.Instance:OpenGiftView({self.data.change}, GIFT_OPEN_TYPE.CONGRATULATIONS, not self.data.now.in_bag)
    end)
end

--------------AccrssoryChangeItem--------------
AccrssoryChangeItem = AccrssoryChangeItem or DeclareMono("AccrssoryChangeItem", UIWidgetBaseItem)
function AccrssoryChangeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if TableIsEmpty(data.item) then
        self.InfoObj:SetActive(false)
        UH.SetText(self.TxtName, "")
        self.Cell:SetData({})
        self.BaseList:SetData({})
        self.ExtraList:SetData({})
    else
        self.InfoObj:SetActive(true)
        UH.SetText(self.TxtName, data.item:QuaName(true))
        self.Cell:SetData(data.item)
        self.BaseList:SetData(data.item:GetBaseAttr(true))
        self.ExtraList:SetData(self:GetExtraData())
    end
end

function AccrssoryChangeItem:GetExtraData()
    local t = {}
    local pos_list, num = self.data.item:GetEffectiveAttr()
    local extra_list = self.data.item:GetAttrList()
    for k, v in pairs(pos_list) do
        local t2 = CommonStruct.AttrStruct(extra_list[v.pos].attr_type, extra_list[v.pos].attr_value)
        t2.attr_value = t2.attr_percent_value
        t2.is_last = k == num
        table.insert(t, t2)
    end
    return t
end


-----------------------------------------------------
--------------AccrssoryChangeSelectView--------------
-----------------------------------------------------
AccrssoryChangeSelectView = AccrssoryChangeSelectView or DeclareView("AccrssoryChangeSelectView", "accrssory/accrssory_change_select")
VIEW_DECLARE_MASK(AccrssoryChangeSelectView, ViewMask.BgBlock)
function AccrssoryChangeSelectView:AccrssoryChangeSelectView()
    self.Board:SetData(self:GetType(), Language.Accrssory.SelectTitle, Vector2.New(380, 540))
end

--------------AccrssoryChangeSelectPanel--------------
AccrssoryChangeSelectPanel = AccrssoryChangeSelectPanel or DeclareMono("AccrssoryChangeSelectPanel", UIWFlushPanel)
function AccrssoryChangeSelectPanel:AccrssoryChangeSelectPanel()
    self.Data = AccrssoryData.Instance
    self.language = Language.Accrssory
    self.data = self.Data.change_data
    self.data_cares = {
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Accrssory), func = self.FlushSelect, init_call = false},
        {data = self.data, func = self.FlushSelect},
    }
end


function AccrssoryChangeSelectPanel:FlushSelect()
    local item_list = {}
    if self.data.select_type == AccrssoryChangeType.Now then
        item_list = self.Data:GetAllEquipDataInfo()
    elseif self.data.select_type == AccrssoryChangeType.Change then
        if TableIsEmpty(self.data.now) then
            PublicPopupCtrl.Instance:Center(self.language.ChangeNowEmpty)
            ViewMgr:CloseView(AccrssorySelectView)
            return
        end
        item_list = self.Data:GetCanChangeItem(self.data.now)
    end
    self.List:SetObjActive(not TableIsEmpty(item_list))
    self.List:SetData(item_list)
end

--------------AccrssoryChangeSelectItem--------------
AccrssoryChangeSelectItem = AccrssoryChangeSelectItem or DeclareMono("AccrssoryChangeSelectItem", UIWidgetBaseItem)
function AccrssoryChangeSelectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, data.item:QuaName(true))
    self.Cell:SetData(data.item)
    self.ObjWare:SetActive(not data.item.in_bag and not data.item:IsFake())
end

function AccrssoryChangeSelectItem:OnItemClick()
    if AccrssoryData.Instance.change_data.select_type == AccrssoryChangeType.Now then
        AccrssoryData.Instance.change_data.now = self.data.item
        AccrssoryData.Instance.change_data.change = {}
    elseif AccrssoryData.Instance.change_data.select_type == AccrssoryChangeType.Change then
        AccrssoryData.Instance.change_data.change = self.data.item
    end
    ViewMgr:CloseView(AccrssoryChangeSelectView)
end