ActEquipSmeltShowView = ActEquipSmeltShowView or DeclareView("ActEquipSmeltShowView", "serveractivity/act_equip_smelt/act_equip_smelt_show")
function ActEquipSmeltShowView:LoadCallback()
    self.Alert:SetData(self:GetType(), Vector2.New(630, 410))
end

function ActEquipSmeltShowView:OpenCallback()
    local show_info, is_over = ActEquipSmeltData.Instance:GetShowInfo()
    self.List1:SetData(DataHelper.FormatItemList({show_info.show_regular_item}))
    self.List2:SetData(DataHelper.FormatItemList(show_info.show_rand_item))
end

ActEquipSmeltShowItem = ActEquipSmeltShowItem or DeclareMono("ActEquipSmeltShowItem", UIWidgetBaseItem)
function ActEquipSmeltShowItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.ItemCell:SetData(data)
    UH.SetText(self.ItemName, data:QuaName())
end
