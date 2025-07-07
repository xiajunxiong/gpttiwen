ItemPreRangeSingleView = ItemPreRangeSingleView or DeclareView("ItemPreRangeSingleView","item_info/item_pre_attr_single")

VIEW_DECLARE_MASK(ItemPreRangeSingleView, ViewMask.None)

function ItemPreRangeSingleView:ItemPreRangeSingleView()

end

-- data = {item = item,pos = {x = int,y = int},height = int}
function ItemPreRangeSingleView:LoadCallback(data)
    self.InfoPosition.localPosition = data.pos ~= nil and Vector2.New(data.pos.x, data.pos.y) or  Vector2.New(-212,195)

    local auto_set = ItemInfoConfig.AutoPosSetting
    self.info_h = (data.height ~=nil and  data.height > auto_set.max_plus_height) and data.height or auto_set.max_plus_height
    
    self:TrySetData(data)
end 

function ItemPreRangeSingleView:OnFlush()

end 

function ItemPreRangeSingleView:OnClickClose()
    ViewMgr:CloseView(ItemPreRangeSingleView)
end

function ItemPreRangeSingleView:TrySetData(data)
    self.item = data.item
    local attr_list = self.item.param.attr_list
    local base_range_list = {}

    if self.item.param ~= nil then
        if self.item.param.attr_list then
            for k, v in ipairs(self.item.param.attr_list) do
                if v.attr_value ~= 0 then
                    local info = {}
                    info.attr_name = Language.Common.AttrList[v.attr_type]
                    info.attr_value = v.attr_value
					self.TextObj:SetActive(false)
                    if self.item:IsEquip() and self.item:Color() ~= ItemColor.Star and self.item:Color() ~= ItemColor.God then --神装不显示属性范围
                        local sec_list = self.item:AttrSection()
                        for i,j in pairs(sec_list) do
                            if j.attr_type == v.attr_type then 
                                local str_format = j.max > 0 and "%s~%s" or "%s ~ %s"

                                info.attr_range = string.format(str_format,
                                DataHelper.IsPercent(j.attr_type) and Percent(j.min) or j.min,
                                DataHelper.IsPercent(j.attr_type) and Percent(j.max) or j.max)
                                break
                            end 
                        end 
						self.TextObj:SetActive(true)
                    elseif self.item:IsJewelry() and self.item:Color() ~= ItemColor.Star and self.item:Color() ~= ItemColor.God then --神装不显示属性范围
                        local jewerly_vo = Jewelry.New(self.item)
                        local sec_list = jewerly_vo:AttrSection()
                        for i,j in pairs(sec_list) do
                            if j.attr_type == v.attr_type then 
                                local str_format = j.max > 0 and "%s~%s" or "%s ~ %s"

                                info.attr_range = string.format(str_format,
                                DataHelper.IsPercent(j.attr_type) and Percent(j.min) or j.min,
                                DataHelper.IsPercent(j.attr_type) and Percent(j.max) or j.max)
                                break
                            end 
                        end 
						self.TextObj:SetActive(true)
                    end 
                    table.insert( base_range_list,info)
                end 
            end 
        end 
    end 

    self.BaseList:SetData(base_range_list)


    local smelt_range_list = {}
    if self.item.param ~= nil then
        if self.item.param.attr_extra_inc then
            if not WorkshopData.Instance:GetEquipIsAllLock(self.item) then 
                for k,v in pairs(Language.Common.AttrList) do
                    local info = {}
                    local min,max = WorkshopData.Instance:GetSmeltAttrRange(self.item.item_id,k)
                    if max > 0 and min > 0 then 
                        local str_format = "%s~%s"
                        info.attr_range = string.format(str_format,
                        DataHelper.IsPercent(k) and Percent(min) or min,
                        DataHelper.IsPercent(k) and Percent(max) or max)

                        info.attr_name = v
                        info.attr_value = ""
                        table.insert( smelt_range_list,info)                        
                    end 
                end 
            end 
        end 
    end 

    self.SmeltListObj:SetActive(#smelt_range_list >0)
    self.SmeltList:SetData(smelt_range_list)

    self.info_pos.sizeDelta = Vector2.New(414, #smelt_range_list >0 and self.info_h  or 250)
end

function ItemPreRangeSingleView:CheckIsNilAttrType(attr_type)
    local t = {
        [5] = 5,
        [7] = 7,
        [9] = 9,
        [10] = 10,
        [11] = 11,
        [12] = 12,
        [30] = 30,
        [31] = 31,
        [32] = 32,
        [33] = 33,
        [34] = 34,
        [100] = 100,
    }

    for k,v in pairs(t) do 
        if attr_type == k then 
            return true
        end 
    end 
    return false
end