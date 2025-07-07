SmeltPreRangeView = SmeltPreRangeView or DeclareView("SmeltPreRangeView","workshop/smelt_pre_attr")

VIEW_DECLARE_MASK(SmeltPreRangeView, ViewMask.None)

function SmeltPreRangeView:SmeltPreRangeView()
    self.AudioOpen = CommonAudio.ItemInfoOpen
    self.AudioClose = CommonAudio.ItemInfoClose
end

function SmeltPreRangeView:LoadCallback(data)
    self:TrySetData(data)
end 

function SmeltPreRangeView:OnFlush()

end 

function SmeltPreRangeView:OnClickBlock()
    ViewMgr:CloseView(SmeltPreRangeView)
end

function SmeltPreRangeView:TrySetData(data)
    self.item = data -- self.info.item_list[data.show_type or 1]
    -- local attr_list = self.item.param.attr_list
    -- local base_range_list = {}

    -- if self.item.param ~= nil then
    --     if self.item.param.attr_list then
    --         for k, v in ipairs(self.item.param.attr_list) do
    --             if v.attr_value ~= 0 then
    --                 local info = {}
    --                 info.attr_name = Language.Common.AttrList[v.attr_type]
    --                 info.attr_value = v.attr_value
    --                 if self.item:IsEquip() then 
    --                     local sec_list = self.item:AttrSection()
    --                     for i,j in pairs(sec_list) do
    --                         if j.attr_type == v.attr_type then 
    --                             local str_format = j.max > 0 and "%s~%s" or "%s ~ %s"

    --                             info.attr_range = string.format(str_format,
    --                             DataHelper.IsPercent(j.attr_type) and Percent(j.min) or j.min,
    --                             DataHelper.IsPercent(j.attr_type) and Percent(j.max) or j.max)
    --                             break
    --                         end 
    --                     end 
    --                 elseif self.item:IsJewelry() then 
    --                     -- local _target = WorkshopData.Instance:GetJewelryShowCfg(self.item:Config().ornament_type,self.item:Config().limit_level)
    --                     -- local _target = {ornament_type = self.item:Config().ornament_type , level = self.item:Config().limit_level}
    --                     -- local jewelry_attr_info = WorkshopData.Instance:GetJewelryPreInfo(_target)
    --                     local sec_list = self.item:AttrSection()
    --                     for i,j in pairs(sec_list) do
    --                         if j.attr_type == v.attr_type then 
    --                             local str_format = j.max > 0 and "%s~%s" or "%s ~ %s"

    --                             info.attr_range = string.format(str_format,
    --                             DataHelper.IsPercent(j.attr_type) and Percent(j.min) or j.min,
    --                             DataHelper.IsPercent(j.attr_type) and Percent(j.max) or j.max)
    --                             break
    --                         end 
    --                     end 
    --                     -- for i,j in pairs(jewelry_attr_info) do 
    --                     --     if i == v.attr_type then 
    --                     --         local str_format = j.attr_max > 0 and "%s~%s" or "%s ~ %s"
                                
    --                     --         info.attr_range = string.format(str_format,
    --                     --         DataHelper.IsPercent(i) and Percent(j.attr_min) or j.attr_min,
    --                     --         DataHelper.IsPercent(i) and Percent(j.attr_max) or j.attr_max)
    --                     --         break
    --                     --     end 
    --                     -- end 
    --                 end 
    --                 table.insert( base_range_list,info)
    --             end 
    --         end 
    --     end 
    -- end 

    -- self.BaseList:SetData(base_range_list)


    -- local smelt_range_list = {}
    -- if self.item.param ~= nil then
    --     if self.item.param.attr_extra_inc then
    --         -- if not WorkshopData.Instance:GetEquipNeedUnLock(self.item) 
    --         if not WorkshopData.Instance:GetEquipIsAllLock(self.item) then 
    --             for k,v in pairs(Language.Common.AttrList) do
    --                 -- if not self:CheckIsNilAttrType(k) then 
    --                     local info = {}
    --                     local min,max = WorkshopData.Instance:GetSmeltAttrRange(self.item.item_id,k)

    --                     if max > 0 and min > 0 then 
    --                         local str_format = "%s~%s"
    --                         info.attr_range = string.format(str_format,
    --                         DataHelper.IsPercent(k) and Percent(min) or min,
    --                         DataHelper.IsPercent(k) and Percent(max) or max)

    --                         info.attr_name = v
    --                         info.attr_value = ""
    --                         table.insert( smelt_range_list,info)
    --                     end 
    --                 -- end 
    --             end 
    --         end 
    --     end 
    -- end 

    -- local test_list = {}
    -- for i = 1,3 do 
    --     local vo = {}
    --     vo.name = "i = "..i
    --     vo.color = COLORS.Red10

    --     vo.children = smelt_range_list
    --     table.insert(test_list,vo)
    -- end 


    local list = WorkshopData.Instance:GetSmeltAttrAccRange(self.item.item_id)
    self.Acclist:SetData(list,true)
    self.Acclist.first_child_data = list[self.item:Color()] ~= nil and list[self.item:Color()].children[1] or list[2].children[1]
    -- self.SmeltListObj:SetActive(#smelt_range_list >0)
    -- self.SmeltList:SetData(smelt_range_list)

    -- self.info_pos.sizeDelta = Vector2.New(414, #smelt_range_list >0 and self.data:GetCurInfoPosInfo().info_h1 or 250)
end

SmeltPreParentItem = SmeltPreParentItem or DeclareMono("SmeltPreParentItem",UIWidgetBaseItem)
function SmeltPreParentItem:SmeltPreParentItem()

end

function SmeltPreParentItem:SetData(data) 
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.title,data.name)

    UH.SetText(self.endtitle,string.format(Language.Workshop.SmeltPreAttrRange, QualityColorStr[data.quality]))
end 

function SmeltPreParentItem:OnSelect(selected)
    if self.Arrow then
        self.Arrow:SetActive(not selected)
    end

    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

SmeltPreChildItem = SmeltPreChildItem or DeclareMono("SmeltPreChildItem",UIWidgetBaseItem)
function SmeltPreChildItem:SmeltPreChildItem()

end

function SmeltPreChildItem:SetData(data) 
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.attr_type,Language.Common.AttrList[data.att_type])
    local str_format = "%s~%s"
    local attr_range = string.format(str_format,
        DataHelper.IsPercent(data.att_type) and Percent(data.min) or data.min,
        DataHelper.IsPercent(data.att_type) and Percent(data.max) or data.max)

    UH.SetText(self.attr_value,attr_range)
end 

