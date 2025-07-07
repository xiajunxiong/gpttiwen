----------------------------LingBaoItemBaseAttr----------------------------
LingBaoItemBaseAttr = LingBaoItemBaseAttr or DeclareMono("LingBaoItemBaseAttr", ItemInfoViewItem)
function LingBaoItemBaseAttr:OnEnable()
	ItemInfoViewItem.OnEnable(self)
    UH.SetText(self.Title, self.language.ItemTitles.LingBaoItemBaseAttr)
    self.item = self.info.item_list[self.ShowType]
	self.List:SetData(self:GetAttrData())
end

function LingBaoItemBaseAttr:GetAttrData()
    local item_2 = self.info.item_list[2]
    local attr_list1 = self.item:BaseAttr()
    local t = {}
    local attr_list2
    if self.ShowType == 1 and item_2 then
        attr_list2 = item_2:BaseAttr()
    end
    for k, v in pairs(attr_list1) do
        -- 如果有装备对比
        if attr_list2 and attr_list2[k] then
            if v.attr_type == attr_list2[k].attr_type and v.attr_value ~= attr_list2[k].attr_value then
                v.show_arrow = true
                v.is_up = tonumber(v.attr_value) > tonumber(attr_list2[k].attr_value)
            end
        end
    end
    return attr_list1
end

--------------------------LingBaoAttrItemInfo--------------------------
LingBaoAttrItemInfo = LingBaoAttrItemInfo or DeclareMono("LingBaoAttrItemInfo", UIWidgetBaseItem)
function LingBaoAttrItemInfo:SetData(data)
    UH.SetText(self.TxtAttrName, data.attr_name)
    UH.SetText(self.TxtAttrValue, data.attr_percent_value)
    if data.show_arrow then
        self.ImgSetArrow:SetObjActive(true)
        UH.SpriteName(self.ImgSetArrow, data.is_up and "up_s" or "down_s")		
    else
        self.ImgSetArrow:SetObjActive(false)
    end
end