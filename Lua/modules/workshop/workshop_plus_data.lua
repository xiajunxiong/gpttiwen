

function WorkshopData:InitNevealCfg()
    self.neveal_effect_list = {}
    for k,v in pairs(Config.equipment_dianhua_auto.effects) do 
        if self.neveal_effect_list[v.type] == nil then 
            self.neveal_effect_list[v.type] = {}
        end 

        self.neveal_effect_list[v.type][v.effect_level] = v
    end 

    self.neveal_skill_list = {}
    for k,v in pairs(Config.equipment_dianhua_auto.tujian) do 
        if self.neveal_skill_list[v.type] == nil then 
            self.neveal_skill_list[v.type] = {}
        end 

        self.neveal_skill_list[v.type][v.skill_level] = v
    end 
end 

function WorkshopData:GetNevealEquipList()
    local data = BagData.Instance:GetEquipList()
    local data2 = BagData.Instance:GetGridByType({ItemType.Weapon, ItemType.Armor})

    local new_list = {}
    for k, v in pairs(data) do
        table.insert(new_list, v)
    end

    table.sort(new_list, function(a, b)
        if a:EquipTypeSort() > b:EquipTypeSort() then
            return true
        else
            return false
        end
    end)

    table.sort(data2, self.CountEquipSort())
    for k, v in pairs(data2) do
        table.insert(new_list, v)
    end

    return new_list
end
function WorkshopData:GetNevealInfo()
    return self.neveal_info
end
function WorkshopData:GetNevealSelect()
    return self.neveal_info.select
end

function WorkshopData:SetNevealSelect(data)
    self.neveal_info.select = data
end

function WorkshopData:SetNevealStoneSelectStone(data)
    self.neveal_info.stone_select = data
end

function WorkshopData:GetNevealStoneSelectStone()
    return self.neveal_info.stone_select
end

function WorkshopData:GetNevealCfg(effect_id,effect_level)
    if self.neveal_effect_list[effect_id] == nil then 
        return nil
    end 
    return self.neveal_effect_list[effect_id][effect_level or 1 ]
end

function WorkshopData:GetNevealSkillCfg(effect_id,skill_level)
    if self.neveal_skill_list[effect_id] == nil then 
        return nil
    end 
    return self.neveal_skill_list[effect_id][skill_level or 1]
end

function WorkshopData:GetNevealSkillCfgByUnlockLevel(effect_id,un_level)
    if self.neveal_skill_list[effect_id] == nil then 
        return nil
    end 
    local result = nil
    for k,v in pairs(self.neveal_skill_list[effect_id]) do 
        if v.skill_unlock_level <= un_level then 
            result = v
        end 
    end 
    return result
end

function WorkshopData:GetNevealAtlasList()
    local list = {}
    for k,v in pairs(self.neveal_skill_list) do 
        table.insert(list,v[1])
    end 

    return list
end

-- 整合提取图鉴列表
function WorkshopData:GetNevealAtlasLevelList(data)
    local list = {}
    for k,v in pairs(self.neveal_skill_list[data.type]) do 
        local vo = {}
        vo.level = v.skill_level..Language.Common.Level
        vo.Desc =  v.skill_name..":"..v.set_skill_description..string.format(Language.Workshop.Neveal.LevelNeed,v.skill_unlock_level)

        table.insert(list,vo)
    end 

    return list
end

-- 提取最接近激活等级
function WorkshopData:GetNevealSkillUnlockCfg(data)
    local cur_unlock_level = BagData.Instance:GetNevealSkillLevel(data.type)
    local cfg = nil
    for k,v in pairs(self.neveal_skill_list[data.type]) do 
        if cur_unlock_level >= v.skill_unlock_level or cfg == nil then 
            cfg = v
        end 
    end 

    local next = self.neveal_skill_list[cfg.type][cfg.skill_level + 1]
    return next
end

-- 整合提取技能展示的列表
function WorkshopData:GetNevealSkillTipList(data)
    local cur_unlock_level = BagData.Instance:GetNevealSkillLevel(data.type)
    local list = {}
    for k,v in pairs(self.neveal_skill_list[data.type]) do 
        local vo = {}
        vo.skill_level = v.skill_level
        local level_done = cur_unlock_level >= v.skill_unlock_level
        vo.description = v.set_skill_description..string.format(Language.Workshop.Neveal.LevelNeedPlus, 
            ColorStr(v.skill_unlock_level,level_done and COLORSTR.Green3 or COLORSTR.Red8),
            ColorStr(cur_unlock_level,level_done and COLORSTR.Green3 or COLORSTR.Red8))

        table.insert(list,vo)
    end 

    return list
end

function WorkshopData:SetNevealExtractItem(item)
    self.neveal_extract_item = item
end

function WorkshopData:GetNevealExtractItem()
    return self.neveal_extract_item
end

function WorkshopData:GetNevealClickSkill()
    return self.neveal_click_skill
end

function WorkshopData:SetNevealClickSkill(data)
    self.neveal_click_skill = data
end

function WorkshopData:GetNevealResolveList(data)
    local list = {}
    for k,v in pairs(data) do 
        local eff_cfg = self:GetNevealCfg(v.param.effect_id,v.param.effect_level)
        for i,j in pairs(eff_cfg.resolve_get_item) do 
            local mark_pos = -1
            for key,value in pairs(list) do 
                if value.item_id == j.item_id then 
                    mark_pos = key
                    break
                end 
            end 
            if mark_pos == -1 then 
                local vo = Item.Create(j)
                table.insert(list,vo)
            else 
                list[mark_pos].num = list[mark_pos].num + j.num
            end 
        end 
    end 
    return list
end

function WorkshopData:SetQuickForgeBuyTips(flag)
    self.quick_forge_buy_tips = flag 
end

function WorkshopData:GetQuickForgeBuyTips()
    return self.quick_forge_buy_tips 
end

-- 获取具体的购买途径信息
function WorkshopData:CatchForgeBuyWay(item)
    if item:GetWay() == nil then return nil end 
    for k,v in pairs(item:GetWay()) do 
        local info = BagData.Instance:GetWayCfg(v)
        -- 持有入手途径·市场购买
        if info.open_panel ~= "" and Mod.Mall.Market == tonumber(info.open_panel) then 
            local shop_data = MarketData.Instance:GetParam(item)
            local market_price = MarketData.Instance:GetItemPrice(item.item_id)
            if market_price > 0 and shop_data.price ~= market_price then
                shop_data.price = market_price
            end
            return shop_data,Mod.Mall.Market
        end 
        -- 持有入手途径·商会购买
        if info.open_panel ~= "" and Mod.Mall.Shop == tonumber(info.open_panel) then 
            local shop_data = CommerceData.Instance:GetMourinhoCfgById(item.item_id)

            return shop_data,Mod.Mall.Shop
        end
        -- 持有入手途径·声望商店
        if info.npc ~= "" then 
            local vil_cfg = PrestigeData.Instance:GetVillageByNpcId(tonumber(info.npc))
            if vil_cfg ~= nil then 
                local list = PrestigeData.Instance:GetPageVillageData(1,vil_cfg.village_id) 
                for i,j in pairs(list or {}) do 
                    if item.item_id == j.item_id then 
                        return j,Mod.Reputation.Main
                    end 
                end 
            end 
        end 
    end 
end

---- 熔铸升星
function WorkshopData:InitBlazeSatrCfg()
    self.screen_list = {prof = {},level = {}}
    self.blaze_list = {}
    for k,v in pairs(Config.equip_star_auto.star_equip) do 
        local e_cfg = Item.GetConfig(v.id)
        local prof_type = RoleData.ProfIdToProfType(e_cfg.limit_prof)
        -- 全职业
        if self.screen_list.prof[prof_type] == nil then 
            local vo = {}
            vo.name = Language.Common.Prof[prof_type]
            vo.key = prof_type
            vo.type = "select_prof"
            self.screen_list.prof[prof_type] = vo
        end 
        -- 职业区分
        if self.blaze_list[prof_type] == nil then 
            self.blaze_list[prof_type] = {}
        end 

        -- 全等级
        -- if e_cfg.limit_level < 60 then 
        --     LogError("?铜哥你看你表有问题")
        --     LogError("?这个装备",Item.GetName(v.id),"是一件",Language.Partner.ColorLevel[Item.GetColor(v.id)],"装备")
        --     LogError("?他怎么会出现在铸星表里呢")
        -- end 
        if self.screen_list.level[e_cfg.limit_level] == nil then 
            local vo = {}
            vo.name = e_cfg.limit_level..Language.Common.Level
            vo.key = e_cfg.limit_level
            vo.type = "select_level"
            self.screen_list.level[e_cfg.limit_level] = vo
        end 

        -- 等级区分
        if self.blaze_list[prof_type][e_cfg.limit_level] == nil then 
            self.blaze_list[prof_type][e_cfg.limit_level] = {}
        end 

        -- 装备id区分
        if self.blaze_list[prof_type][e_cfg.limit_level][v.id] == nil then 
            self.blaze_list[prof_type][e_cfg.limit_level][v.id] = {}
        end 

        -- 双配置连携
        local vo = {equip = e_cfg,blaze = v,item_id = e_cfg.id,limit_level =e_cfg.limit_level }
        -- 星级区分
        if v.item_star > 1 then 
            self.blaze_list[prof_type][e_cfg.limit_level][v.id][v.item_star] = vo
        end 
    end 

    self.simple_blaze_list = Config.equip_star_auto.star_equip
end 

function WorkshopData:GetBlazeCfg(item_id,star)
    for k,v in pairs(self.simple_blaze_list) do 
        if v.item_star == star and v.id == item_id then 
            return v
        end 
    end 
    return nil
end 

-- 用统一装备表来寻找对应的合成表
function WorkshopData:ChangeBlazeCfg(equip_blaze)
    for k,v in pairs(Config.equip_star_auto.equip_star_up) do 
        if v.compose_id == equip_blaze.id and v.item_star == equip_blaze.item_star then 
            return v
        end 
    end 
    return nil
end

function WorkshopData:GetBlazeList(item_id)
    local list = {} 
    for k,v in pairs(self.simple_blaze_list) do 
        if v.id == item_id then 
           table.insert(list,v) 
        end 
    end 
    return list
end

function WorkshopData:GetBlazeData(item_id,star,prof,level)
    local oper_prof = prof == nil and self.blazestar_info.select_prof or prof
    local oper_level = level == nil and self.blazestar_info.select_level or level

    if self.blaze_list[oper_prof] == nil or self.blaze_list[oper_prof][oper_level] == nil then 
        return nil 
    end 

    local cfg_list = self.blaze_list[oper_prof][oper_level]

    if cfg_list[item_id] ~= nil and cfg_list[item_id][star] then 
        return cfg_list[item_id][star]
    else 
        return nil
    end 
end

function WorkshopData:GetBlazeStarList(item_id,prof,level)
    local oper_prof = prof == nil and self.blazestar_info.select_prof or prof
    local oper_level = level == nil and self.blazestar_info.select_level or level

    if self.blaze_list[oper_prof] == nil or self.blaze_list[oper_prof][oper_level] == nil then 
        return nil 
    end 

    local cfg_list = self.blaze_list[oper_prof][oper_level]

    if cfg_list[item_id] ~= nil then 
        return cfg_list[item_id]
    else 
        return nil
    end 
end 

function WorkshopData:BlazeSelectLevelByChange()
    local role_level = RoleData.Instance:GetRoleLevel() 
    local select = -1 
    for k,v in pairs( self.screen_list.level ) do 
        if select == -1 then 
            select = v.key
        elseif v.key > select and v.key <= role_level then 
            select = v.key
        end 
    end 

    self.blazestar_info.select_level = select

    self.blazestar_info.remind_level = select
end

function WorkshopData:BlazeSelectProfByChange()
    local profession = RoleData.Instance:GetProfession()

    self.blazestar_info.select_prof = profession == 0 and 1 or profession
end

function WorkshopData:GetBlazeAtlasInfo()
    local screen_info = {
        prof = self.screen_list.prof[self.blazestar_info.select_prof],
        level = self.screen_list.level[self.blazestar_info.select_level],
    }

    local show_list = {}
    local cur_list = self.blaze_list[self.blazestar_info.select_prof][self.blazestar_info.select_level]

    for k,v in pairs(cur_list) do 
        local vo = {}
        local n_info = v[2]
        vo.name = Language.Common.Equip[n_info.equip.equip_type]
        vo.item_id = n_info.equip.id
        vo.children = {}
        vo.equip_type = n_info.equip.equip_type
        for i,j in pairs(v) do 
            if j.blaze.is_can_star == 1 then 
                table.insert(vo.children,j)
            end 
        end 
        if #vo.children > 0 then 
            table.insert(show_list,vo)
        end 
    end 

    table.sort(show_list,function(a,b)return a.equip_type < b.equip_type end)

    return screen_info,show_list
end

function WorkshopData:SetSelectBlazeItem(data)
    if data ~= nil and self.blazestar_info.select_data ~= data then 
        self:ClearBlazeInfo()
    end 

    self.blazestar_info.select_data = data
end

function WorkshopData:GetSelectBlazeItem()
    return self.blazestar_info.select_data
end

function WorkshopData:ClearBlazeInfo()
    self.blazestar_info.operate = nil
    self.blazestar_info.obl_right = nil
    self.blazestar_info.obl_left = nil
end

function WorkshopData:SetBlazeQuick(flag)
    local role_id = RoleData.Instance:GetBaseData().role_id
    UnityPlayerPrefs.SetInt(role_id.."blaze_star_quick",flag and 1 or 0)

    self.blazestar_info.quick_tick = flag
end

function WorkshopData:GetBlazeQuick()
    return self.blazestar_info.quick_tick 
end

function WorkshopData:BlazeSelectQuickByChange()
    local role_id = RoleData.Instance:GetBaseData().role_id
    self.blazestar_info.quick_tick = UnityPlayerPrefs.GetInt(role_id.."blaze_star_quick") == 1
end

function WorkshopData:SelectBlazeFromEquiplist()
    local data = BagData.Instance:GetEquipList()
    local data2 = BagData.Instance:GetJewelryList()

    local blaze = nil
    for k,v in pairs(self.simple_blaze_list) do 
        for i,equip in pairs(data) do 
            if equip.item_id == v.id and v.item_star == equip:BlazeStar()+1 then 
                blaze = v
                break
            end 
        end 

        for j,jewerly in pairs(data2) do 
            if jewerly.item_id == v.id and v.item_star == jewerly:BlazeStar()+1 then 
                blaze = v
                break
            end 
        end 
        if blaze ~= nil then 
            break
        end 
    end 

    return blaze
end


--挑选材料的借口
--逻辑变更，操作位的穿戴中转个
function WorkshopData:CacheBlazeSelect(pop_mark,selected)
    local can_equip = pop_mark == "operate"

    local data = BagData.Instance:GetEquipList()
    local data2 = BagData.Instance:GetJewelryList()
    local data3 = BagData.Instance:GetGridByType({ItemType.Weapon, ItemType.Armor,ItemType.Jewelry})
    local new_list = {}

    local select_data = selected == nil and self.blazestar_info.select_data or selected

    if select_data == nil then 
        return {}
     end 
    local equip = Item.Create(select_data)

    if can_equip then 
        if equip:IsEquip() and data[select_data.equip.equip_type] ~=nil and data[select_data.equip.equip_type].item_id == select_data.item_id then 
            -- 同星级
            if data[select_data.equip.equip_type]:BlazeStar() == select_data.blaze.item_star - 1 then  -- 星级要比要升上去的配置低一个
                table.insert(new_list,data[select_data.equip.equip_type])
            end 
        elseif equip:IsJewelry() and  data2[select_data.equip.equip_type - 5] ~=nil  and data2[select_data.equip.equip_type - 5].item_id == select_data.item_id then 
            -- 同星级
            if data2[select_data.equip.equip_type - 5]:BlazeStar() == select_data.blaze.item_star - 1 then  -- 星级要比要升上去的配置低一个
                table.insert(new_list,data2[select_data.equip.equip_type-5])
            end 
        end 
    end 

    for k,v in pairs(data3) do 
        -- 检查过滤出来的装备是否已经被选中了
        if (self.blazestar_info.operate == nil or (self.blazestar_info.operate ~= nil and 
            not BagData.Instance:CheckSameEquipByUniqueId(self.blazestar_info.operate,v))) and 
            (self.blazestar_info.obl_left == nil or (self.blazestar_info.obl_left ~= nil and 
            not BagData.Instance:CheckSameEquipByUniqueId(self.blazestar_info.obl_left,v))) and 
            (self.blazestar_info.obl_right == nil or (self.blazestar_info.obl_right ~= nil and 
            not BagData.Instance:CheckSameEquipByUniqueId(self.blazestar_info.obl_right,v))) then 
                
            -- 同职业，同等级，同星级
            -- LogError("?funck!职业",select_data.equip.limit_prof,Equip.GetLimitProf(v.item_id))
            -- LogError("?funck!等级",Item.GetLimitLv(v.item_id) , select_data.equip.limit_level )
            -- LogError("?funck!星级",v:BlazeStar() , select_data.blaze.item_star - 1 )
            -- LogError("?funck!同类型",(equip:IsEquip() and v:IsEquip()) , (equip:IsJewelry() and v:IsJewelry()) )

            if Item.GetLimitLv(v.item_id) == select_data.equip.limit_level  and 
                -- select_data.equip.limit_prof == Equip.GetLimitProf(v.item_id) and 
                -- (not can_equip or (can_equip and v.item_id == select_data.item_id)) and -- 去除同id限定
                ((equip:EquipType() == v:EquipType() and can_equip) or not can_equip ) and  -- 同部位
                ((equip:IsEquip() and v:IsEquip()) or (equip:IsJewelry() and v:IsJewelry())) and  
                v:BlazeStar() == select_data.blaze.item_star - 1 then  -- 星级要比要升上去的配置低一个

                    table.insert(new_list,v)
            end 
            
        end 
    end 

    return new_list
end 

--- 注意funopen与飞升控制
function WorkshopData:CheckBlazeStarRemind()
    -- funopen 控制
    if FunOpen.Instance:GetFunIsOpened(Mod.Workshop.BlazeStar) == false then return 0 end

    -- 飞升控制 预留，暂时没有
    -- if not FlyUpData.Instance:IsFinish() then return 0 end 

    local mix_list = {}

    local data = BagData.Instance:GetEquipList()
    local data2 = BagData.Instance:GetJewelryList()
    local data3 = BagData.Instance:GetGridByType({ItemType.Weapon, ItemType.Armor,ItemType.Jewelry})

    local function checkIsBlaze(data)
        local color_flag = data:Color() == ItemColor.Orange 
        local can_flag = Equip.CanBlazeStar(data.item_id)

        return color_flag and can_flag
    end 

    for k,v in pairs(data) do 
        if checkIsBlaze(v) then table.insert( mix_list, v) end 
    end 

    for k,v in pairs(data2) do 
        if checkIsBlaze(v) then table.insert( mix_list, v) end 
    end 

    for k,v in pairs(data3) do 
        if checkIsBlaze(v) then table.insert( mix_list, v) end 
    end 

    local sort_list = {}
    for k,v in pairs(mix_list) do 
        if Equip.CanBlazeStar(v.item_id) and self.blazestar_info.remind_level == v:Level() and v:IsBlazeStar() then 
            if sort_list[v.item_id] == nil then sort_list[v.item_id] = {} end 

            table.insert( sort_list[v.item_id], v)
        end 
    end 


    for k,v in pairs(sort_list) do 
        if #v >= 3 then 
            return 1
        end 
    end 

    return 0
end 

function WorkshopData:CheckNevealRemind()
    return 0
end 

function WorkshopData:IsSelectCanQuickItem()
    local data = BagData.Instance:GetEquipList()
    local data2 = BagData.Instance:GetJewelryList()

    local select_equip = Item.Create(self.blazestar_info.select_data)

    local equip_type = Equip.GetEquipType(select_equip.item_id)
    local operate = select_equip:IsEquip() and data[equip_type] or data2[equip_type -5] 
    
    return operate ~= nil
end

function WorkshopData:SetAutoClickBlazeStar(item)
    if item == nil then self.blaze_star_auto = nil end 
    if self.blazestar_info.quick_tick then return end 
    if item == nil then 
        self.blaze_star_auto = nil
        return 
    end 
    self.blaze_star_auto = {
        item_id = item.item_id,
        item_star = item:BlazeStar(),
        item = item,
    }
end

function WorkshopData:GetAutoClickBlazeStar()
    return self.blaze_star_auto
end

function WorkshopData:SetAutoClickNeveal(item)
    if item == nil then return end 
    self:SetNevealSelect(item)
end

-- 这是一个复合的判定条件，需要分步讨论
function WorkshopData:GetNevealIsOpen()
    if not FunOpen.Instance:GetFunIsOpened(Mod.Equip.Neveal) then 
        return false
    end 
    if FlyUpData.Instance:GetStage() < FlyUpStageType.Low then 
        return false
    end 
    return true
end

-- 清空强化附魔信息中的信息
function WorkshopData:ClearNevealExtraPlus()
    self.neveal_extra_plus_info.main = nil
    self.neveal_extra_plus_info.stuff_1 = nil
    self.neveal_extra_plus_info.stuff_2 = nil
    self.neveal_extra_plus_info.is_show = false
end

-- 设置选中的主物品
function WorkshopData:SetMainNevealExtraPlus(item)
    self.neveal_extra_plus_info.main = item
end

-- 设置选中的材料1物品
function WorkshopData:SetMainNevealExtraStuff1Plus(item)
    self.neveal_extra_plus_info.stuff_1 = item
end

-- 设置选中的材料2物品
function WorkshopData:SetMainNevealExtraStuff2Plus(item)
    self.neveal_extra_plus_info.stuff_2 = item
end

-- 获取展示的下一级
function WorkshopData:GetNevealExtraMainUpgrade()
    local main_param = self.neveal_extra_plus_info.main.param
    local next_pram = self:GetNevealCfg(main_param.effect_id,main_param.effect_level+1)
    if next_pram == nil then 
        return nil
    end 
    local item = Item.Create({item_id = Config.equipment_dianhua_auto.other[1].dianhua_id,
    param = {has_param = 1,effect_id = next_pram.type ,effect_level = next_pram.effect_level}})

    return item 
end

function WorkshopData:GetNevealExtraSelectList()
    local item_list = BagData.Instance:GetGridByType({ItemType.Neveal})
    local main_index = self.neveal_extra_plus_info.main == nil and -1 or self.neveal_extra_plus_info.main:GridIndex()
    local stuff_1_index = self.neveal_extra_plus_info.stuff_1 == nil and -1 or self.neveal_extra_plus_info.stuff_1:GridIndex()
    local stuff_2_index = self.neveal_extra_plus_info.stuff_2 == nil and -1 or self.neveal_extra_plus_info.stuff_2:GridIndex()
    local ready_list = {}
    for k,v in pairs(item_list) do 
        if v:GridIndex() ~= main_index and v:GridIndex() ~= stuff_1_index and v:GridIndex() ~= stuff_2_index then 
            if v.param.has_param == 1 and v.param.effect_id > 0 and v.param.effect_level == self.neveal_extra_plus_info.main.param.effect_level then  
                table.insert(ready_list,v)
            end 
        end 
    end 

    return ready_list
end

function WorkshopData:OneKeyNevealExtraSelect()
    local list = self:GetNevealExtraSelectList()
    if self.neveal_extra_plus_info.stuff_1 == nil then 
        self:SetMainNevealExtraStuff1Plus(list[1])
    end 
    if self.neveal_extra_plus_info.stuff_2 == nil then 
        self:SetMainNevealExtraStuff2Plus(list[2])
    end 

    if #list == 0 then
        PublicPopupCtrl.Instance:Center(Language.NevealExtralPlus.ErrorOneKeyFail)
    end 
end

-- function WorkshopData()
    
-- end

-- 校准当前的附魔信息
function WorkshopData:NevealExtralHandleItemChange()
    -- main
    local flag_1 = self.neveal_extra_plus_info.main:GridIndex() == BagData.Instance.item_grid[ItemColumnType.Item][self.neveal_extra_plus_info.main:GridIndex()]:GridIndex()
    local flag_2 = self.neveal_extra_plus_info.main.param.has_param == 1 and self.neveal_extra_plus_info.main.param.effect_level ~= nil
    local flag_3 = BagData.Instance.item_grid[ItemColumnType.Item][self.neveal_extra_plus_info.main:GridIndex()].param.has_param == 1 and BagData.Instance.item_grid[ItemColumnType.Item][self.neveal_extra_plus_info.main:GridIndex()].param.effect_level ~= nil
    local flag_4 = (flag_2 and flag_3) and self.neveal_extra_plus_info.main.param.effect_level < BagData.Instance.item_grid[ItemColumnType.Item][self.neveal_extra_plus_info.main:GridIndex()].param.effect_level

    self.neveal_extra_plus_info.main = 
        BagData.Instance.item_list[ItemColumnType.Item][self.neveal_extra_plus_info.main:GridIndex()]

    if self.neveal_extra_plus_info.main ~= nil and self.neveal_extra_plus_info.main.param.has_param == 1 and self.neveal_extra_plus_info.main.param.effect_level > 20 then 
        self.neveal_extra_plus_info.main = nil
    end 

    self.neveal_extra_plus_info.is_show = flag_1 and flag_4

    if self.neveal_extra_plus_info.stuff_1 ~= nil then 
        self.neveal_extra_plus_info.stuff_1 = 
            BagData.Instance.item_list[ItemColumnType.Item][self.neveal_extra_plus_info.stuff_1:GridIndex()]
    else 
        self.neveal_extra_plus_info.stuff_1 = nil
    end 

    if self.neveal_extra_plus_info.stuff_2 ~= nil then 
        self.neveal_extra_plus_info.stuff_2 = 
            BagData.Instance.item_list[ItemColumnType.Item][self.neveal_extra_plus_info.stuff_2:GridIndex()]
    else 
        self.neveal_extra_plus_info.stuff_2 = nil
    end 
end

function WorkshopData:GetNevealExtralIsMax()
    return self.neveal_extra_plus_info.main.param.has_param == 1 and self.neveal_extra_plus_info.main.param.effect_level >= 20
end

function WorkshopData:SetBehaviorSelect(id)
    self.behavior_select = id
end

function WorkshopData:GetBehaviorSelect(id)
    return self.behavior_select
end

function WorkshopData:NevealUnlockItem()
    return Config.equipment_dianhua_auto.other[1].plan_cost_item
end