
IllusionViewSuitAttr = IllusionViewSuitAttr or DeclareView("IllusionViewSuitAttr", IllusionConfig.ResPath .. "illusion_suit_attr")

function IllusionViewSuitAttr:IllusionViewSuitAttr()
	self.language = Language.Illusion
end

function IllusionViewSuitAttr:LoadCallback()
    self.Board:SetData(self:GetType(), self.language.IllusionSuitAttr, Vector2.New(830, 499))
end

IllusionViewSuitAttrList = IllusionViewSuitAttrList or DeclareMono("IllusionViewSuitAttrList", UIWFlushPanel)

function IllusionViewSuitAttrList:IllusionViewSuitAttrList()
	self.data = IllusionCtrl.Instance:Data()
end

function IllusionViewSuitAttrList:onFlush()
    local suit_list = self.data:GetSuitList()
    self.SuitList:SetData(suit_list)
end

IllusionViewSuitAttrItem = DeclareMono("IllusionViewSuitAttrItem", UIWidgetBaseItem)

function IllusionViewSuitAttrItem:SetData(data)
	self.Name.text = data.suit_name
	self.SuitList:SetData(data.suit_state)
	self.AttrGrid:SetData(data.attr_show)
end

IllusionViewSuitAttrListItem = DeclareMono("IllusionViewSuitAttrListItem", UIWidgetBaseItem)

function IllusionViewSuitAttrListItem:SetData(data)
	self.Name.text = data.name
	self.State.text = "<color=#" .. (data.actived and COLORSTR.Green3 or COLORSTR.Red6) .. ">" .. (data.actived and 1 or 0) .. "/1</color>"
end

IllusionViewSuitAttrGridItem = DeclareMono("IllusionViewSuitAttrGridItem", UIWidgetBaseItem)

function IllusionViewSuitAttrGridItem:IllusionViewSuitAttrGridItem()
	self.illusion_data = IllusionCtrl.Instance:Data()
end

function IllusionViewSuitAttrGridItem:SetData(data)
	local suit_info = self.illusion_data:GetSuitInfoWithSuitId(data.suit_id)
	self.Desc.text = "<color=#" .. (suit_info.suit_state_actived and COLORSTR.Yellow13 or COLORSTR.Gray2) .. ">" .. data.content .. "</color>"
end
