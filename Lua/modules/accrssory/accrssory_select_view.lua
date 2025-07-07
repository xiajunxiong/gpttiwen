AccrssorySelectView = AccrssorySelectView or DeclareView("AccrssorySelectView", "accrssory/accrssory_select")
VIEW_DECLARE_MASK(AccrssorySelectView, ViewMask.BgBlock)
function AccrssorySelectView:AccrssorySelectView()
    self.Board:SetData(self:GetType(), Language.Accrssory.SelectTitle, Vector2.New(380, 540))
end

--------------AccrssorySelectPanel--------------
AccrssorySelectPanel = AccrssorySelectPanel or DeclareMono("AccrssorySelectPanel", UIWFlushPanel)
function AccrssorySelectPanel:AccrssorySelectPanel()
    self.Data = AccrssoryData.Instance
    self.language = Language.Accrssory
    self.data = self.Data.accrssory_inherit_data
    self.data_cares = {
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Accrssory), func = self.FlushSelect, init_call = false},
        {data = self.Data.accrssory_equip_data, func = self.FlushSelect, init_call = false},
    }
    self:FlushSelect()
end

function AccrssorySelectPanel:FlushSelect()
    if self.data.inherit_type == AccrssoryInheritType.Now then
        self.List:SetData(AccrssoryData.Instance:GetEquipDataInfo())
    elseif self.data.inherit_type == AccrssoryInheritType.Material then
        if TableIsEmpty(self.data.now) then
            PublicPopupCtrl.Instance:Center(self.language.InheritNowSelectEmpty)
            ViewMgr:CloseView(AccrssorySelectView)
            return
        end
        local item_list = {}
        local bag_list = self.Data:GetAccrssoryBagData(false, 0, 0, self.data.now:SuitType(), self.data.now:AccrssoryType())
        for k, v in pairs(bag_list) do
            table.insert(item_list, {item = v})
        end
        self.List:SetObjActive(not TableIsEmpty(item_list))
        self.List:SetData(item_list)
    end
end

--------------AccrssorySelectItem--------------
AccrssorySelectItem = AccrssorySelectItem or DeclareMono("AccrssorySelectItem", UIWidgetBaseItem)
function AccrssorySelectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, data.item:QuaName(true))
    self.Cell:SetData(data.item)
end

function AccrssorySelectItem:OnItemClick()
    if AccrssoryData.Instance.accrssory_inherit_data.inherit_type == AccrssoryInheritType.Now then
        AccrssoryData.Instance.accrssory_inherit_data.now = self.data.item
    elseif AccrssoryData.Instance.accrssory_inherit_data.inherit_type == AccrssoryInheritType.Material then
        AccrssoryData.Instance.accrssory_inherit_data.material = self.data.item
    end
    ViewMgr:CloseView(AccrssorySelectView)
end