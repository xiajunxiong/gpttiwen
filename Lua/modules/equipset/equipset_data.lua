EquipSetData = EquipSetData or BaseClass()
EquipSetData.ColorSet = {
    Act = {
        title = COLORSTR.Yellow7,
        attr = COLORSTR.Green5,
        sp = COLORSTR.Red3,
    },
    Lock = {        
        title = COLORSTR.Gray4,
        attr = COLORSTR.Gray3,
        sp = COLORSTR.Gray3,
    },
}
function EquipSetData:__init()
    if EquipSetData.Instance ~= nil then
        Debuger.LogError("[EquipSetData] attempt to create singleton twice!")
        return
    end
    EquipSetData.Instance = self

    self:InitConfig()

    self.bag_release = SmartData.New({val = false})
end 

function EquipSetData:CheckRelease()
    self.bag_release.val = not self.bag_release.val
end

function EquipSetData:InitConfig()
    self.suit_cfg_list = {}
    for k,v in pairs(Config.suit_list_auto.suit_attr) do 
        if self.suit_cfg_list[v.id] == nil then 
            self.suit_cfg_list[v.id] = {}
        end 

        if v.color == "" then 
            self.suit_cfg_list[v.id][v.suit_compose_1] = v
        else
            if self.suit_cfg_list[v.id][v.suit_compose_1] == nil then 
                self.suit_cfg_list[v.id][v.suit_compose_1] = {}
                self.suit_cfg_list[v.id][v.suit_compose_1].with_color = true
                self.suit_cfg_list[v.id][v.suit_compose_1].split = {}
            end 
            self.suit_cfg_list[v.id][v.suit_compose_1].split[v.color] = v
        end 
    end 
    self.empty_suit_ids = {}
    local list = Split(Config.suit_list_auto.other[1].empty_suit_id,"|")
    for k,v in pairs(list) do 
        self.empty_suit_ids[tonumber(v)] = tonumber(v)
    end 
end 

function EquipSetData:CheckIsEmptySuit(suit_id)
    return self.empty_suit_ids[suit_id] == nil
end

function EquipSetData:CatchEquipSetConfig(suit_id)
    return self.suit_cfg_list[suit_id]
end

function EquipSetData:QuickEquipSetConfig(suit_id)
    for k,v in pairs(Config.suit_list_auto.suit_attr) do 
        if suit_id == v.id then 
            return v
        end 
    end 
end

-- 以套装id检查当前有多少件
function EquipSetData:EquipSetLinkCheck(suit_id,color)
    local suit_cfg = self:QuickEquipSetConfig(suit_id)
    if suit_cfg == nil then return 0 end 
    local equip_list = suit_cfg.suit_type == 1 and BagData.Instance:GetEquipList() or BagData.Instance:GetJewelryList()
    local link_num = 0
    for k,v in pairs(equip_list) do 
        if color == nil or color < ItemColor.Star then 
            if v.param.new_suit_effect_id == suit_id then 
                link_num = link_num + 1
            end 
        else 
            if v.param.new_suit_effect_id == suit_id and v:Color() >= color then 
                link_num = link_num + 1
            end 
        end 
    end 
    return link_num
end

-- 本方法提取玩家信息中的装备信息来检查套装
function EquipSetData:EquipSetLinkCheckOther(suit_id,color)
    local suit_cfg = self:QuickEquipSetConfig(suit_id)
    if suit_cfg == nil then return 0 end 
    local data = RoleInfoCtrl.Instance:Data()
    local equip_list = suit_cfg.suit_type == 1 and data:GetRoleEquips().equipment_list or data:GetRoleEquips().jewelry_list
    local link_num = 0
    for k,v in pairs(equip_list) do 
        if color == nil or color < ItemColor.Star then 
            if v.param.new_suit_effect_id == suit_id then 
                link_num = link_num + 1
            end 
        else 
            if v.param.new_suit_effect_id == suit_id and v:Color() >= color then 
                link_num = link_num + 1
            end 
        end 
    end 
    return link_num
end

-- 以套装id及换装检索换装后有多少件
function EquipSetData:EquipSetLinkCheckExe(suit_id,change_data,color)
    local suit_cfg = self:QuickEquipSetConfig(suit_id)
    if suit_cfg == nil then return 0 end 
    local equip_list = suit_cfg.suit_type == 1 and BagData.Instance:GetEquipList() or BagData.Instance:GetJewelryList()
    local link_num = 0
    for k,v in pairs(equip_list) do 
        if color == nil or color < ItemColor.Star then 
            if v.param.new_suit_effect_id == suit_id and change_data:EquipType() ~= k then 
                link_num = link_num + 1
            elseif v.param.new_suit_effect_id ~= suit_id and change_data:EquipType() == k 
                and change_data.param.new_suit_effect_id == suit_id then 
                link_num = link_num + 1
            end 
        else 
            if v.param.new_suit_effect_id == suit_id and change_data:EquipType() ~= k and v:Color() >= color then 
                link_num = link_num + 1
            elseif v.param.new_suit_effect_id ~= suit_id and change_data:EquipType() == k 
                and change_data.param.new_suit_effect_id == suit_id and v:Color() >= color then 
                link_num = link_num + 1
            end 
        end 
    end
    return link_num
end

function EquipSetData:FixSuitConfig(list)
    local operate_list = {}
    for k,v in pairs(list) do 
        if v.with_color == nil then 
            table.insert(operate_list,v)
        else
            for i , j in pairs(v.split) do
                table.insert(operate_list,j)
            end 
        end  
    end 
    table.sort(operate_list, function(a, b)
		local itemA = (a.color~="" and a.color or 0) *100 + a.suit_compose_1
		local itemB = (b.color~="" and b.color or 0) *100 + b.suit_compose_1
        if itemB > itemA then
            return true
        else
            return false
        end
    end)
    return operate_list
end

function EquipSetData:GetItemInfoEquipSetShow(suit_id,show_link,color)
    local suit_cfg = self:CatchEquipSetConfig(suit_id)
    local link_num = self:EquipSetLinkCheck(suit_id,color) -- 同级颜色
    local link_plus_num = self:EquipSetLinkCheck(suit_id,color + 1) -- 上级颜色

    local operate_list = self:FixSuitConfig(suit_cfg or {})

    -- LogError("?suit_cfg!",suit_cfg)
    -- LogError("?operate_list!",operate_list)
    -- 套装成立条件改为 当前颜色连接数大于等于同级颜色套装连接数 且 同级颜色套装连接数 大于 上级颜色连接数
    local desc_list = {}
    for k,v in pairs(operate_list) do 
        if v.color == "" or v.color == color then 
            local vo = {}
            local colors = ((link_num >= v.suit_compose_1 and ((link_plus_num < v.suit_compose_1 and color >= ItemColor.Star ) or color < ItemColor.Star ) and show_link) 
                or (not show_link)) and EquipSetData.ColorSet.Act or EquipSetData.ColorSet.Lock
    
            local link_show = (link_num > v.suit_compose_1 and link_plus_num < v.suit_compose_1 ) and v.suit_compose_1 or link_num
            local link_info = show_link and "  【"..link_show.."/"..v.suit_compose_1.."】" or ""
            vo.desc_title = ColorStr(v.suit_name .. link_info,colors.title)
    
            vo.attr_desc = ""
            for i = 1,2 do 
                if v["attr_add_"..i] ~= 0 then 
                    vo.attr_desc = vo.attr_desc .. Language.Common.AttrList[v["attr_type_"..i]].."  +"
                    ..(DataHelper.IsPercent(v["attr_type_"..i]) and Percent(v["attr_add_"..i]) or v["attr_add_"..i]) .."    "
                end 
            end 
            vo.attr_desc = ColorStr(vo.attr_desc,colors.attr)
    
            vo.sp_effect = ""
            vo.show_sp = false
            if v.sp_id > 0 then 
                local cfg = Cfg.SpecialEffect(v.sp_id)
                vo.sp_effect = ColorStr(Language.ItemInfo.ItemTitles.SuitSp..cfg.description,colors.sp) 
                vo.show_sp = true
            elseif v.attr_add_3 > 0 then 
                local is_show_attr = k <= 2
                vo.sp_effect = ColorStr((is_show_attr and "" or Language.ItemInfo.ItemTitles.SuitSp)..Language.Common.AttrList[v.attr_type_3].."  +"
                ..(DataHelper.IsPercent(v.attr_type_3) and Percent(v.attr_add_3) or v.attr_add_3),is_show_attr and colors.attr or colors.sp)
                vo.show_sp = true
            end 
    
            table.insert(desc_list, vo)
        end
    end 

    return desc_list
end

-- 本方法独立使用， 用来看别人的套装生效
function EquipSetData:GetItemInfoEquipSetShowOther(suit_id,show_link,color)
    local suit_cfg = self:CatchEquipSetConfig(suit_id)
    local link_num = self:EquipSetLinkCheckOther(suit_id,color) -- 同级颜色
    local link_plus_num = self:EquipSetLinkCheckOther(suit_id,color + 1) -- 上级颜色

    local operate_list = self:FixSuitConfig(suit_cfg or {})

    -- 套装成立条件改为 当前颜色连接数大于等于同级颜色套装连接数 且 同级颜色套装连接数 大于 上级颜色连接数
    local desc_list = {}
    for k,v in pairs(operate_list) do 
        if v.color == "" or v.color == color then 
            local vo = {}
            local colors = ((link_num >= v.suit_compose_1 and ((link_plus_num < v.suit_compose_1 and color >= ItemColor.Star ) or color < ItemColor.Star ) and show_link) 
                or (not show_link)) and EquipSetData.ColorSet.Act or EquipSetData.ColorSet.Lock
    
            local link_show = (link_num > v.suit_compose_1 and link_plus_num < v.suit_compose_1 ) and v.suit_compose_1 or link_num
            local link_info = show_link and "  【"..link_show.."/"..v.suit_compose_1.."】" or ""
            vo.desc_title = ColorStr(v.suit_name .. link_info,colors.title)
    
            vo.attr_desc = ""
            for i = 1,2 do 
                if v["attr_add_"..i] ~= 0 then 
                    vo.attr_desc = vo.attr_desc .. Language.Common.AttrList[v["attr_type_"..i]].."  +"
                    ..(DataHelper.IsPercent(v["attr_type_"..i]) and Percent(v["attr_add_"..i]) or v["attr_add_"..i]) .."    "
                end 
            end 
            vo.attr_desc = ColorStr(vo.attr_desc,colors.attr)
    
            vo.sp_effect = ""
            vo.show_sp = false
            if v.sp_id > 0 then 
                local cfg = Cfg.SpecialEffect(v.sp_id)
                vo.sp_effect = ColorStr(Language.ItemInfo.ItemTitles.SuitSp..cfg.description,colors.sp) 
                vo.show_sp = true
            elseif v.attr_add_3 > 0 then 
                local is_show_attr = k <= 2
                vo.sp_effect = ColorStr((is_show_attr and "" or Language.ItemInfo.ItemTitles.SuitSp)..Language.Common.AttrList[v.attr_type_3].."  +"
                ..(DataHelper.IsPercent(v.attr_type_3) and Percent(v.attr_add_3) or v.attr_add_3),is_show_attr and colors.attr or colors.sp)
                vo.show_sp = true
            end 
    
            table.insert(desc_list, vo)
        end
    end 

    return desc_list
end 

function EquipSetData:GetMixRoleSuitActiveList()
    local equip_list = BagData.Instance:GetEquipList()
    local jewerly_list = BagData.Instance:GetJewelryList()

    local mix_list = {}
    for k,v in pairs(equip_list) do 
        if v.param and v.param.new_suit_effect_id > 0 and self:CheckIsEmptySuit(v.param.new_suit_effect_id) then 
            if mix_list[v.param.new_suit_effect_id] == nil then 
                mix_list[v.param.new_suit_effect_id] = {
                    link = 0,
                    suit_id = v.param.new_suit_effect_id,
                }
            end 

            mix_list[v.param.new_suit_effect_id].link = mix_list[v.param.new_suit_effect_id].link + 1

            if v:Color() >= ItemColor.Star then 
                if mix_list[v.param.new_suit_effect_id].split == nil then 
                    mix_list[v.param.new_suit_effect_id].split = {}
                end 
                mix_list[v.param.new_suit_effect_id].split[v:Color()] = 1 +
                (mix_list[v.param.new_suit_effect_id].split[v:Color()] or 0 ) 

                if v:Color() == ItemColor.God then 
                    mix_list[v.param.new_suit_effect_id].split[v:Color()-1] = 1 +
                    (mix_list[v.param.new_suit_effect_id].split[v:Color()-1] or 0 ) 
                end 
            end 
        end 
    end 

    
    for k,v in pairs(jewerly_list) do 
        if v.param and v.param.new_suit_effect_id > 0 and self:CheckIsEmptySuit(v.param.new_suit_effect_id) then 
            if mix_list[v.param.new_suit_effect_id] == nil then 
                mix_list[v.param.new_suit_effect_id] = {
                    link = 0,
                    suit_id = v.param.new_suit_effect_id,
                }
            end 

            mix_list[v.param.new_suit_effect_id].link = mix_list[v.param.new_suit_effect_id].link + 1

            if v:Color() >= ItemColor.Star then 
                if mix_list[v.param.new_suit_effect_id].split == nil then 
                    mix_list[v.param.new_suit_effect_id].split = {}
                end 
                mix_list[v.param.new_suit_effect_id].split[v:Color()] = 1 +
                (mix_list[v.param.new_suit_effect_id].split[v:Color()] or 0 ) 

                if v:Color() == ItemColor.God then 
                    mix_list[v.param.new_suit_effect_id].split[v:Color()-1] = 1 +
                    (mix_list[v.param.new_suit_effect_id].split[v:Color()-1] or 0 ) 
                end 
            end 

        end 
    end 

    return mix_list
end

function EquipSetData:GetMixRoleSuitDescList()
    local list = self:GetMixRoleSuitActiveList()

    local desc_list = {}
    for k,v in pairs(list) do 
        local operate_list = self:FixSuitConfig(self:CatchEquipSetConfig(v.suit_id))

        for i,j in pairs(operate_list) do 
            if v.split == nil then 
                if v.link >= j.suit_compose_1 then 
                    local vo = {}
                    local colors = EquipSetData.ColorSet.Act
                    local link_info = "  【"..(v.link > j.suit_compose_1 and  j.suit_compose_1 or v.link ).."/"..j.suit_compose_1.."】" or ""
                    vo.desc_title = ColorStr(j.suit_name .. link_info,colors.title)
    
                    vo.attr_desc = ""
                    for u = 1,2 do 
                        if j["attr_add_"..u] ~= 0 then 
                            vo.attr_desc = vo.attr_desc .. Language.Common.AttrList[j["attr_type_"..u]].."  +"
                            ..(DataHelper.IsPercent(j["attr_type_"..u]) and Percent(j["attr_add_"..u]) or j["attr_add_"..u]) .."    "
                        end 
                    end 
                    vo.attr_desc = ColorStr(vo.attr_desc,colors.attr)
    
                    vo.sp_effect = ""
                    vo.show_sp = false
                    if j.sp_id > 0 then 
                        local cfg = Cfg.SpecialEffect(j.sp_id)
                        vo.sp_effect = ColorStr(Language.ItemInfo.ItemTitles.SuitSp..cfg.description,colors.sp) 
                        vo.show_sp = true
                    elseif j.attr_add_3 >0 then 
                        local is_show_attr = i <= 2
                        vo.sp_effect = ColorStr((is_show_attr and "" or Language.ItemInfo.ItemTitles.SuitSp).. Language.Common.AttrList[j.attr_type_3].."  +"
                        ..(DataHelper.IsPercent(j.attr_type_3) and Percent(j.attr_add_3) or j.attr_add_3),is_show_attr and colors.attr or colors.sp)
                        vo.show_sp = true
                    end 
    
                    table.insert(desc_list, vo)
                end 
            else
                local plus_info = {}
                -- 兼容策略
                for color,color_link in pairs(v.split) do 
                    local vo = {}
                    vo.link = color_link
                    vo.plus_link = v.split[color + 1] == nil and 0 or v.split[color + 1]
                    plus_info[color] = vo
                end

                for color,color_link in pairs(plus_info) do 
                    if color == j.color and color_link.link >= j.suit_compose_1 and color_link.plus_link < j.suit_compose_1 then 
                        local vo = {}
                        local colors = EquipSetData.ColorSet.Act
                        local link_info = "  【"..(color_link.link > j.suit_compose_1 and j.suit_compose_1 or color_link.link ).."/"..j.suit_compose_1.."】" or ""
                        vo.desc_title = ColorStr(j.suit_name .. link_info,colors.title)
        
                        vo.attr_desc = ""
                        for u = 1,2 do 
                            if j["attr_add_"..u] ~= 0 then 
                                vo.attr_desc = vo.attr_desc .. Language.Common.AttrList[j["attr_type_"..u]].."  +"
                                ..(DataHelper.IsPercent(j["attr_type_"..u]) and Percent(j["attr_add_"..u]) or j["attr_add_"..u]) .."    "
                            end 
                        end 
                        vo.attr_desc = ColorStr(vo.attr_desc,colors.attr)
        
                        vo.sp_effect = ""
                        vo.show_sp = false
                        if j.sp_id > 0 then 
                            local cfg = Cfg.SpecialEffect(j.sp_id)
                            vo.sp_effect = ColorStr(Language.ItemInfo.ItemTitles.SuitSp..cfg.description,colors.sp) 
                            vo.show_sp = true
                        elseif j.attr_add_3 >0 then 
                            local is_show_attr = i <= 2
                            vo.sp_effect = ColorStr((is_show_attr and "" or Language.ItemInfo.ItemTitles.SuitSp).. Language.Common.AttrList[j.attr_type_3].."  +"
                            ..(DataHelper.IsPercent(j.attr_type_3) and Percent(j.attr_add_3) or j.attr_add_3),is_show_attr and colors.attr or colors.sp)
                            vo.show_sp = true
                        end 
        
                        table.insert(desc_list, vo)
                    end 
                end 
            end             
        end 
    end 

    return desc_list
end

--
function EquipSetData:GetRoleSuitPlus(change_data)
    local suit_id = change_data:GetEquipSet()
    local link_num = self:EquipSetLinkCheck(suit_id,change_data:Color())

    local flag_link = suit_id > 0 and self.suit_cfg_list[suit_id][link_num] ~= nil

    local link_list = {}
    if flag_link then 
        for k,v in pairs( change_data:IsEquip() and BagData.Instance:GetEquipList() or BagData.Instance:GetJewelryList()) do 
            if v:GetEquipSet() == suit_id then 
                table.insert(link_list, k)
            end 
        end 
    end 

    return flag_link,link_list
end 

function EquipSetData:GetRoleSuitPlusReady(change_data)
    local suit_id = change_data:GetEquipSet()
    local link_num = self:EquipSetLinkCheckExe(suit_id,change_data,change_data:Color())

    local flag_link = suit_id > 0 and self.suit_cfg_list[suit_id][link_num] ~= nil

    local link_list = {}
    if flag_link then 
        for k,v in pairs( change_data:IsEquip() and BagData.Instance:GetEquipList() or BagData.Instance:GetJewelryList()) do 
            if v:GetEquipSet() == suit_id then 
                table.insert(link_list, k)
            end 
        end 
    end 

    return flag_link,link_list
end