ComposeSucc = ComposeSucc or DeclareView("ComposeSucc", "workshop/compose_succ")
VIEW_DECLARE_MASK(ComposeSucc,ViewMask.BgBlockClose)
function ComposeSucc:ComposeSucc()
    self.data = WorkshopData.Instance
end
function ComposeSucc:OnFlush()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ComposeSucc)
    
    self.data.forged_flag.flag = false

    local info = self.data:ComposeResult()
    local item = Item.Create(info.item)
    self.Cell:SetData(item)
    UH.SetText(self.Desc, item:QuaName(true))
    UH.SetText(self.LimitedDesc, item:LimitProfDesc())
    UH.SetText(self.EquipLevel, item:Level()..Language.Common.Level)
    UH.SetText(self.BlazeLevel, item:Level()..Language.Common.Level)
    self.BlazeLevelShow:SetActive(false)--item:IsBlazeStar())
    self.EquipLevelShow:SetActive(not item:IsBlazeStar())
    local operate_pos = info.item.param
    local has_inc, _inc_list = item:IncAttr()
    self.IsTresure:SetActive(operate_pos.is_treasure == 3)
    -- local temp1 = ""
	-- local temp2 = ""
    -- if operate_pos.attr_list then
    --     local attr_index = 0
    --     for k, v in ipairs(operate_pos.attr_list) do
    --         if v.attr_value > 0 then
    --             attr_index = attr_index + 1
    --             local str = string.format(Language.ItemInfo.ItemTitles.EquipAttr2, Language.Common.AttrList[v.attr_type], v.attr_value)
    --             if attr_index % 2 == 0 then 
    --                 temp2 = temp2 == "" and str or temp2.."\n" ..str
    --             elseif attr_index % 2 == 1 then  
    --                 temp1 = temp1 == "" and str or temp1.."\n" ..str
    --             end 
    --         end 
    --     end 
    -- end

    -- UH.SetText(self.AttrBaseStr, temp1)
    -- UH.SetText(self.AttrBaseLeftStr, temp2)
    
    -- temp1 = ""
    -- temp2 = ""
    -- local attr_index = 0
    -- for i = 1,operate_pos.inc_num or 0 do
    --     if operate_pos.attr_extra_inc[i].attr_value > 0 then
    --         attr_index = attr_index + 1

    --         local str = string.format(Language.ItemInfo.ItemTitles.EquipAttr3, 
    --         QualityColorStr[operate_pos.attr_extra_color[i]],
    --         Language.Common.AttrList[operate_pos.attr_extra_inc[i].attr_type],
    --         operate_pos.attr_extra_inc[i].attr_value)

    --         if attr_index % 2 == 0 then 
    --             temp2 = temp2 == "" and str or temp2.."\n" ..str
    --         elseif attr_index % 2 == 1 then  
    --             temp1 = temp1 == "" and str or temp1.."\n" ..str
    --         end 
    --     end 
    -- end 

    -- UH.SetText(self.IncAttrStr, temp1)
    -- UH.SetText(self.IncAttrLeftStr, temp2)

    -- if operate_pos.sp_id ~= nil and operate_pos.sp_id > 0 then 
    --     local str = string.format(Language.ItemInfo.ItemTitles.EffectOtE, COLORSTR.Purple1, Equip.GetEffectDesc(operate_pos.sp_id))

    --     UH.SetText(self.SpecialStr, str)
    -- end 

    -- if operate_pos == nil then self.AttrBg:SetActive(false) return end 
    -- self.AttrBg:SetActive(operate_pos.inc_num ~= nil and operate_pos.inc_num > 0)
    -- if operate_pos.inc_num == nil then return end 
    -- UH.SizeDelta(self.Bg,Vector2.New(447,500))
    -- UH.SizeDelta(self.Bg,operate_pos.inc_num > 0 and Vector2.New(447,500) or Vector2.New(447,374))

    local base_list = {}
    local is_stren = operate_pos.is_strong_create==1
    self.BaseObj:SetActive(not is_stren)
    self.BaseStrenObj:SetActive(is_stren)
    if is_stren then 
        for k, v in ipairs(item:UltimateAttrList()) do
            if v.attr_value ~= 0 then
                local str = ""
                if v.ultimate_value > 0 then 
                    str = string.format(Language.ItemInfo.ItemTitles.EquipAttrU, 
                        Language.Common.AttrList[v.attr_type], 
                        DataHelper.IsPercent(v.attr_type) and Percent(v.attr_value) or v.attr_value,
                        v.ultimate_value)
                else 
                    str = string.format(Language.ItemInfo.ItemTitles.EquipAttr2, 
                    Language.Common.AttrList[v.attr_type], 
                    DataHelper.IsPercent(v.attr_type) and Percent(v.attr_value) or v.attr_value)
                end 

                table.insert( base_list, {str,v.attr_type} ) 
            end
        end 
    else 
        for k, v in ipairs(item:BaseAttr()) do
            if v.attr_value ~= 0 then
                local str = string.format(Language.ItemInfo.ItemTitles.EquipAttr2, 
                    Language.Common.AttrList[v.attr_type], 
                    DataHelper.IsPercent(v.attr_type) and Percent(v.attr_value) or v.attr_value)
                table.insert( base_list, {str,v.attr_type} ) 
            end
        end
    end 

    -- if #base_list == 0 and operate_pos.attr_pair.attr_value > 0 then 
    --     local str = string.format(Language.ItemInfo.ItemTitles.EquipAttr2, 
    --         Language.Common.AttrList[operate_pos.attr_pair.attr_type], 
    --         DataHelper.IsPercent(operate_pos.attr_pair.attr_type) and or operate_pos.attr_pair.attr_value)
    --     table.insert( base_list, str ) 
    -- end 

    if #base_list % 2 > 0 and not is_stren then 
        local str = " "
        table.insert( base_list, str ) 
    end 
    
    local inc_list = {}
    for i = 1,6 do
        if _inc_list[i].attr_value ~= 0 then
            local str = string.format(Language.ItemInfo.ItemTitles.EquipAttr3, 
                QualityColorStr[_inc_list[i].color],
                Language.Common.AttrList[_inc_list[i].attr_type],
                DataHelper.IsPercent(_inc_list[i].attr_type) and Percent(_inc_list[i].attr_value) or _inc_list[i].attr_value)
            table.insert( inc_list, {str,_inc_list[i].attr_type} ) 
        end
    end

    if #inc_list % 2 > 0 then 
        local str = " "
        table.insert( inc_list, str ) 
    end 
    self.IncObj:SetActive( #inc_list > 0)
    

	local height = 130
    if operate_pos.new_suit_effect_id > 0 then 
        local list = EquipSetData.Instance:GetItemInfoEquipSetShow(operate_pos.new_suit_effect_id,true,item:Color())
        self.SuitList:SetData(list)
        height = #list > 1 and height or 80
    end 
    self.SuitHeight.preferredHeight = height
    self.HideSuit:SetActive( operate_pos.new_suit_effect_id == 0)
    self.ShowSuit:SetActive( operate_pos.new_suit_effect_id > 0)
    self.SuitObj:SetActive( operate_pos.new_suit_effect_id > 0)

    local special_list = {}
    if operate_pos.sp_id ~= nil and operate_pos.sp_id > 0 then 
        local str = string.format(Language.ItemInfo.ItemTitles.EffectOtE, COLORSTR.Purple1, Equip.GetEffectDesc(operate_pos.sp_id))
        table.insert( special_list, {str} ) 
    end 

    local item_desc_list = {}
    local desc_str = Item.GetDesc(item.item_id)
    table.insert( item_desc_list, {desc_str} ) 

    local creater_list = {}
    local creater_str = string.format(Language.ItemInfo.ItemTitles.CreateName,operate_pos.creater_name)
    table.insert( creater_list, {creater_str} ) 

    self.BaseList:SetData(base_list)
    self.BaseStrenList:SetData(base_list)
    self.IncList:SetData(inc_list)
    self.SpecialList:SetData(special_list)
    self.ItemDescList:SetData(item_desc_list)
    self.CreaterList:SetData(creater_list)

    self.NoInc:SetActive(#inc_list==0)

    self.SpecialObj:SetActive(operate_pos.sp_id ~= nil and operate_pos.sp_id > 0)

    self.AttrPart:SetActive(false)
    TimeHelper:AddDelayFrameTimer(function ()
        if self.AttrPart then 
            self.AttrPart:SetActive(true)
        end 
    end,1)

    -- 这个项隐藏
    local flag = item:IsBlazeStar()
    self.BlazeAttrlist.gameObject:SetActive(false)
    if flag then 
        UH.SetText(self.BlazeTitle, DataHelper.GetDaXie(item:BlazeStar()-1)..Language.ItemInfo.ItemTitles.BlazeStar)

        local blaze_cfg = WorkshopData.Instance:GetBlazeCfg(item.item_id,item:BlazeStar())
        local list = {}
        for i = 1,6 do 
            local at_type = blaze_cfg["attr_type_"..i]
            if at_type ~= nil and at_type > 0 then 
                local at_value = blaze_cfg["attr_min_"..i]
                local str = Language.Common.AttrList[at_type].." "..
			    ColorStr((DataHelper.IsPercent(at_type) and Percent(at_value) or at_value),COLORSTR.Green10) 

                table.insert( list, {str,at_type} )   
            end 
        end 

        self.BlazeAttrlist:SetData(list)
    end 
end

SuccTipsAttrItem = SuccTipsAttrItem or DeclareMono("SuccTipsAttrItem", UIWidgetBaseItem)

function SuccTipsAttrItem:RoleProfessionItem()

end 

function SuccTipsAttrItem:SetData(data)
    self.attr_str.text = data[1]
    if data[2] and self.PointObj then
        self.PointObj:SetActive(DataHelper.IsPoint(data[2]))
    end
end 