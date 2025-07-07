ItemPreRangeView = ItemPreRangeView or DeclareView("ItemPreRangeView","item_info/item_pre_attr")

VIEW_DECLARE_MASK(ItemPreRangeView, ViewMask.None)

function ItemPreRangeView:ItemPreRangeView()
    self.data = ItemInfoCtrl.Instance:Data()
    self.info = self.data:ItemInfo()
end

function ItemPreRangeView:LoadCallback(data)
    self:TrySetData(data)
    self.InfoPosition.localPosition = data.pos + Vector2.New(213,5)
end 

function ItemPreRangeView:TrySetData(data)
    self.item = self.info.item_list[data.show_type or 1]
    local attr_list = self.item.param.attr_list
    local base_range_list = {}
    local is_blaze = Equip.CanBlazeStar(self.item.item_id) or (self.item.IsBlazeStar and self.item:IsBlazeStar())
    if self.item.param ~= nil then
        if self.item.param.attr_list then
            for k, v in ipairs(self.item.param.attr_list) do
                if v.attr_value ~= 0 then
                    local info = {}
                    info.attr_name = Language.Common.AttrList[v.attr_type]
                    info.attr_value = v.attr_value
                    info.attr_type = v.attr_type
                    self.TextObj:SetActive(false)
                    if self.item:IsEquip() and not is_blaze and self.item:Color() ~= ItemColor.Star and self.item:Color() ~= ItemColor.God then --神装不显示属性范围
                        local sec_list = self.item:AttrSection()
                        for i, j in pairs(sec_list) do
                            if j.attr_type == v.attr_type then
                                local str_format = j.max > 0 and "%s~%s" or "%s ~ %s"
                                info.attr_range =
                                    Format(str_format, DataHelper.IsPercent(j.attr_type) and Percent(j.min) or j.min, DataHelper.IsPercent(j.attr_type) and Percent(j.max) or j.max)
                                break
                            end
                        end
                        self.TextObj:SetActive(true)
                    elseif self.item:IsJewelry() and not is_blaze and self.item:Color() ~= ItemColor.Star and self.item:Color() ~= ItemColor.God then --神装不显示属性范围
                        local jewerly_vo = Jewelry.New(self.item)
                        local sec_list = jewerly_vo:AttrSection()
                        for i, j in pairs(sec_list) do
                            if j.attr_type == v.attr_type then
                                local str_format = j.max > 0 and "%s~%s" or "%s ~ %s"

                                info.attr_range =
                                    Format(str_format, DataHelper.IsPercent(j.attr_type) and Percent(j.min) or j.min, DataHelper.IsPercent(j.attr_type) and Percent(j.max) or j.max)
                                break
                            end
                        end
                        self.TextObj:SetActive(true)
                    elseif is_blaze then
                        local blaze_list = WorkshopData.Instance:GetBlazeList(self.item.item_id)
                        local str_format =  "%s ~ %s"

                        -- 从0星开始遍历
                        local min_attr = 0
                        local max_attr = 0
                        for i,j in pairs(blaze_list) do
                            for key = 1,6 do 
                                local at_type = j["attr_type_"..key]
                                local at_value = j["attr_min_"..key]
                                if at_type == v.attr_type and at_value > 0 then 
                                    -- 取最小值
                                    if min_attr == 0 then 
                                        min_attr = at_value
                                    elseif at_value < min_attr then 
                                        min_attr = at_value
                                    end 

                                    -- 取最大值
                                    if max_attr == 0 then 
                                        max_attr = at_value
                                    elseif at_value > max_attr then 
                                        max_attr = at_value
                                    end 
                                end 
                            end 
                        end 

                        info.attr_range =
                                    Format(str_format, DataHelper.IsPercent(v.attr_type) and Percent(min_attr) or min_attr, DataHelper.IsPercent(v.attr_type) and Percent(max_attr) or max_attr)

                        self.TextObj:SetActive(true)
                    end
                    table.insert(base_range_list, info)
                end
            end
        end
    end
    self.BaseList:SetData(base_range_list)
    local smelt_range_list = {}
    if self.item.param ~= nil then
        if self.item.param.attr_extra_inc then
            if not WorkshopData.Instance:GetEquipIsAllLock(self.item) then
                for k, v in pairs(Language.Common.AttrList) do
                    local info = {}
                    local min, max = WorkshopData.Instance:GetSmeltAttrRange(self.item.item_id, k)
                    if max > 0 and min > 0 then
                        local str_format = "%s~%s"
                        info.attr_range = Format(str_format, DataHelper.IsPercent(k) and Percent(min) or min, DataHelper.IsPercent(k) and Percent(max) or max)
                        info.attr_name = v
                        info.attr_value = ""
                        table.insert(smelt_range_list, info)
                    end
                end
            end
        end
    end
    self.SmeltListObj:SetActive(#smelt_range_list > 0)
    self.SmeltList:SetData(smelt_range_list)
    self.info_pos.sizeDelta = Vector2.New(414, #smelt_range_list > 0 and 250 + #smelt_range_list * 30 or 250)
end

function ItemPreRangeView:OnClickPressDown()
	ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.ItemInfo)
end

PreAttrRangeItem = PreAttrRangeItem or DeclareMono("PreAttrRangeItem", UIWidgetBaseItem)

function PreAttrRangeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local str = Format(Language.ItemInfo.ItemTitles.EquipAttr2,data.attr_name, DataHelper.IsPercent(data.attr_type) and Percent(data.attr_value) or data.attr_value)
    self.attr_index.text = str
    self.attr_range.text = data.attr_range
end