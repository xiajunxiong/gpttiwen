FabaoFetters = FabaoFetters or DeclareView("FabaoFetters","fabao/fabao_fetters")
function FabaoFetters:FabaoFetters()
    
end
function FabaoFetters:LoadCallback()
    self.Board:SetData(self:GetType(),Language.Fabao.FettersTitle, Vector2.New(726,512))
end
function FabaoFetters:OnFlush()
    local data = FabaoData.Instance.fetter_group_list
    self.List:SetData(data)
end

FabaoFettersListItemParent = FabaoFettersListItemParent or DeclareMono("FabaoFettersListItemParent", UIWidgetBaseItem)
function FabaoFettersListItemParent:SetData(data)
    UH.SetText(self.Name, data[1].fetter_name)
    self.List:SetData(data)
end
FabaoFettersListItem = FabaoFettersListItem or DeclareMono("FabaoFettersListItem", UIWidgetBaseItem)
function FabaoFettersListItem:FabaoFettersListItem()
    self.fabao_data = FabaoData.Instance
end
function FabaoFettersListItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local cell = self.fabao_data:FabaoAttrInfo(data.id)
    self.FabaoCell:SetData(cell)
    UH.SetText(self.LblName, cell.name)
    self.EffectObj:SetActive(cell.corner_icon ~= "")
    UH.SetText(self.LblEffect, cell.corner_icon)
end
function FabaoFettersListItem:OnClickItem()
    local cell = self.fabao_data:FabaoAttrInfo(self.data.id)
    local info = {
		item_list = {Item.Init(cell.talisman_virtual_item)}
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end