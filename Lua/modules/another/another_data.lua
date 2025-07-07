AnotherData = AnotherData or BaseClass()

function AnotherData:__init()
	if AnotherData.Instance ~= nil then
		Debuger.LogError("[AnotherData] attempt to create singleton twice!")
		return
	end
	AnotherData.Instance = self
	self:InitSmartData()
end

function AnotherData:__delete()
	AnotherData.Instance = nil
end

function AnotherData:ClearData()
	self.pet_list:Set({})
    self.pet_other_info:Set({})
end

function AnotherData:InitSmartData()
	self.equipment_list = SmartData.New()
	self.jewelry_list = SmartData.New()
    self.item_list = SmartData.New()
    self.item_grid = SmartData.New()
    self.equip_data = SmartData.New{
        medal_info = {},
        medal_id = 0,
        element_crystal = SmartData.New(),
        medal_take_off = 0,
    }
    self.role_data = SmartData.New{
        position = 0,
        appearance = {},
        attr_list = {},
        role_level = 0,
    }
    self.point_data = SmartData.New{
        plan_list = {},
        cur_plan_index = 0,
        plan_info = {}
    }
    self.ws_operate = SmartData.New{
        is_inset = false,
        is_smelt = false,
    }
    self.pet_list = SmartData.New()
	self.pet_other_info = SmartData.New()
	self.pet_select = SmartData.New({index = -1})
    self.pet_attr_list = SmartData.New()
    self.pet_equip_select = SmartData.New({item_type = 1, skill_type = 0, item_index = -1, equip = 1})
    self.medal_take_off_str = ""
    self.role_point_data = SmartData.New{view_show = false,add_list = {},add_change = false}
    
    self.another_click_func = {
        [1] = function()
            return self:GetRoleAddPlanInfo().add_point_num ~= 0
        end,
        [2] = function()
            if self:GetPetCount() == 0 then
                return false
            end
            local num = 0
            for k,v in pairs(self:GetPetList() or {}) do
                if self:IsFightState(v.index) then
                    num = num + 1
                end
            end
            return num == 0
        end,
        [3] = function()
            if self:GetPetCount() == 0 then
                return false
            end
            for k,v in pairs(self:GetPetList() or {}) do
                if self:IsFightState(v.index) and v:PointNum() ~= 0 then
                    return true
                end
            end
        end
    }
end 

function AnotherData:InitializationRoleData()
    self.role_point_data.view_show = false
    self.role_point_data.add_change = false
    self.role_point_data.add_list = {}
end

function AnotherData:IsHasTip(type)
    if self.another_click_func[type] then
        return self.another_click_func[type]()
    end
    return false
end

function AnotherData.IsGuide()
    return BattlefieldData.GetPrefsKey() == 0
end

function AnotherData.IsGuideOpened()
    return ActivityData.IsOpen(ActType.Battlefield) == false and AnotherData.IsGuide()
end

--===================================化身人物下发===================================
function AnotherData:SetRoleInfoAck(protocol)
    self.role_data.appearance = CommonStruct.AppearanceParam(protocol.appearance)
    self.role_data.role_level = protocol.role_level
end

function AnotherData:SetRoleAttrList(protocol)
    self.role_data.attr_list = protocol.attr_list
end

function AnotherData:SetRoleAttributePlanAllInfo(protocol)
    self.point_data.plan_list = protocol.plan_list
    self.point_data.plan_info = protocol.plan_info
    self.point_data.cur_plan_index = protocol.cur_plan_index
end

function AnotherData:SetRoleAttributePlanInfo(protocol)
    self.point_data.plan_info = protocol.plan_info
    self.point_data.cur_plan_index = protocol.cur_plan_index
    self.point_data.plan_list[protocol.cur_plan_index] = protocol.plan_info
end

function AnotherData:SetBattlePosition(protocol)
    self.role_data.position = protocol.position
end

function AnotherData:GetRoleAppearance()
    local cur_appearance = self.role_data.appearance
    local item = self:GetEquipList(EquipmentIndex.IndexMainWeapon)
    cur_appearance.weapon_id = item ~= nil and item.item_id or 0
    return cur_appearance
end

function AnotherData:GetRoleAttrList()
    return self.role_data.attr_list
end

function AnotherData:GetRolePosition()
    return self.role_data.position
end

function AnotherData:GetRoleAddPlanInfo()
    return self.point_data.plan_info or {}
end

function AnotherData:GetRoleAddPlanList()
    return self.point_data.plan_list or {}
end

function AnotherData:GetRoleAllPiontIndex()
    return self.point_data.cur_plan_index
end

function AnotherData:IsRolePointViewShow()
    return self.role_point_data.view_show
end

function AnotherData:SetRolePointViewShow(view_show)
    self.role_point_data.view_show = view_show
end

function AnotherData:SetRolePointAddNum(type,num)
    self.role_point_data.add_list[type] = num
    self.role_point_data.add_change = not self.role_point_data.add_change
end

function AnotherData:SetRolePointAddList(add_list)
    self.role_point_data.add_list = add_list or {}
    if add_list ~= nil then
        self.role_point_data.add_change = not self.role_point_data.add_change
    end
end

function AnotherData:GetRolePointAddNum()
    local num = 0
    for k,v in pairs(self.role_point_data.add_list) do
        num = num + v
    end
    return num
end

function AnotherData:GetRolePointAddList()
    return self.role_point_data.add_list
end

--===================================化身装备下发===================================
function AnotherData:SetMedalInfo(protocol)
    self.equip_data.medal_id = protocol.medal_id
    self.equip_data.medal_info = protocol.medal_list
    self:CheckIsBetter()
end

function AnotherData:SetMedalTakeOffInfo(protocol)
    self.equip_data.medal_take_off = protocol.out_index
end

function AnotherData:SetEquipmentInfoAck(protocol)
    self.equip_data.element_crystal:Set(protocol.element_crystal)
    local item_list = {}
    for k, v in pairs(protocol.equipment_list) do
        item_list[k] = Equip.New(v)
        item_list[k].in_bag = false
    end
    self.equipment_list:Set(item_list)
    self:CheckIsBetter()
end

function AnotherData:SetElementCrystalChangeNotice(protocol)
    self.equip_data.element_crystal:Set(protocol.element_crystal)
end

function AnotherData:SetEquipmentGridChange(protocol)
    if protocol.equip_info.item_id == 0 then
        self.equipment_list[protocol.index] = nil
    else
        self.equipment_list[protocol.index] = Equip.New(protocol.equip_info)
    end
    self:CheckIsBetter()
end

function AnotherData:SetJewelryAllInform(protocol)
    local item_list = {}
    for k, v in pairs(protocol.jewelry_list) do
        item_list[k] = Jewelry.New(v)
        item_list[k].in_bag = false
    end
    self.jewelry_list:Set(item_list)
    self:CheckIsBetter()
end

function AnotherData:SetJewelryInfoChange(protocol)
    if protocol.jewelry.item_id == 0 then
        self.jewelry_list[protocol.index] = nil
    else
        self.jewelry_list[protocol.index] = Jewelry.New(protocol.jewelry)
    end
    self:CheckIsBetter()
end

function AnotherData:SetKnapsackInfoAck(protocol)
    self:SetColumnSize(400)
    for i = 1, protocol.item_count do
        self:SetItemData(protocol.item_list[i], protocol.reason)
    end
    self:ClearEmptyItem(protocol.item_list)
end

function AnotherData:SetColumnSize(size)
    for i = size - 1, 0, -1 do
        if self.item_grid[i] ~= nil then
            return
        else
            local item = SmartData.New(Item.New())
            item.column_type = ItemColumnType.Start
            item.index = i
            self.item_grid[i] = item
        end
    end
end

function AnotherData:SetItemData(item_protc, reason, put_reason)
    local grid_item = self.item_grid[item_protc.index]
    if grid_item == nil then
        return
    end
    grid_item.item_id = item_protc.item_id
    grid_item.num = item_protc.num
    grid_item.is_bind = item_protc.is_bind
    grid_item.item_type = grid_item.GetType(grid_item.item_id) or ItemType.Normal

    local item = SmartData.New(Item.Create(item_protc))
    item.column_type = grid_item.column_type
    item.item_type = grid_item.item_type
    self.item_grid[item_protc.index] = item

    if grid_item.item_id ~= 0 then
        self.item_list[grid_item.index] = item
    else
        self.item_list[grid_item.index] = nil
    end
    self:CheckIsBetter()
end

function AnotherData:GetItemByIndex(index)
    if self.item_list[index] then
        return self.item_list[index]
    end
end

function AnotherData:ClearEmptyItem(new_item_list)
    local item_map = {}
    for k, item in pairs(new_item_list) do
        if item_map == nil then
            item_map = {}
        end
        item_map[item.index] = item
    end
    if self.item_list then
        for index, v in pairs(self.item_list) do
            if item_map[index] == nil then
                local item = SmartData.New(Item.New())
                item.index = index
                self.item_grid[index] = item
                self.item_list[index] = nil
            end
        end
    end
end

function AnotherData:SetKnapsackItemChange(protocol)
    self:SetItemData(protocol, protocol.reason, protocol.put_reason)
end

function AnotherData:GetCrystalList()
    local crystal_list = {}
    local id = self:GetElementCrystal().element_crystal_id
    if id > 0 then
        local item = Item.Create({item_id = id})
        item.index = -1
        item.param.used_times = self:GetElementCrystal().used_times
        table.insert(crystal_list, item)
    end
    for k, v in pairs(self.item_grid) do
        if v.item_type == ItemType.EleCrystal then
            table.insert(crystal_list, v)
        end
    end
    return crystal_list
end

-- 需求已变更
-- 现在化身和本尊会同步更新，两者的装备是一致的
function AnotherData:CheckEquipmentUniqueId(equip, is_equip)
    -- local equal_flag = false
    -- local operate_list = is_equip and self.equipment_list or self.jewelry_list
    -- for k, v in pairs(operate_list) do
    --     if v.vo.item_unique_id_1 == equip.item_unique_id_1 and v.vo.item_unique_id_2 == equip.item_unique_id_2 then
    --         equal_flag = true
    --     end
    -- end
    -- return not equal_flag

    return true
end

function AnotherData:GetMedalEmpty()
    local pos = 2
    local is_full = true
    for k, v in pairs(self.equip_data.medal_info) do
        if v.item_id == 0 then
            pos = v.grid_index
            is_full = false
            break
        end
    end
    return pos, is_full
end

-- 拷贝来自背包
function AnotherData:CheckBetterEqiup(item_datas)
    local items = {}
    local mark_items = {equip = {},jewelry = {}}
    for k, v in pairs(item_datas) do
        local vo = v
        local prof_type = RoleData.Instance:ProfType()
        if vo:IsEquip() then
            local limit_prof = Equip.GetLimitProf(vo.item_id)
            if limit_prof == RoleData.Instance:GetRoleProfType() then
                local compare = vo:AnotherCompare()
                local rate_t = ROLE_SCORE_RATE[prof_type] or {}
                local item_score = 0
                local compare_score = 0
                if vo.param and vo.param.attr_list and rate_t then
                    for i, j in pairs(vo.param.attr_list) do
                        local rare_ = rate_t[j.attr_type] ~= nil and rate_t[j.attr_type] or 0
                        item_score = item_score + rare_ * j.attr_value
                    end
                end
                if compare and compare.param and compare.param.attr_list and rate_t then
                    for i, j in pairs(compare.param.attr_list) do
                        local rare_ = rate_t[j.attr_type] ~= nil and rate_t[j.attr_type] or 0
                        compare_score = compare_score + rare_ * j.attr_value
                    end
                end
                if item_score > compare_score and v:Level() <= RoleData.Instance:GetRoleLevel() then
                    local vo_mark = {}
                    vo_mark.pos = k
                    vo_mark.item_score = item_score
                    if mark_items.equip[vo:EquipType()] == nil then
                        if vo.is_better ~= true then vo.is_better = true end 
                        mark_items.equip[vo:EquipType()] = vo_mark
                    elseif item_score > mark_items.equip[vo:EquipType()].item_score then
                        local operate = item_datas[mark_items.equip[vo:EquipType()].pos]
                        if operate.is_better ~= false then operate.is_better = false end 
                        mark_items.equip[vo:EquipType()] = vo_mark
                        if vo.is_better ~= true then vo.is_better = true end 
                    end
                else
                    if vo.is_better ~= false then vo.is_better = false end 
                end
            end
        elseif vo:IsJewelry() then
            local compare = vo:AnotherCompare()
            local rate_t = ROLE_SCORE_RATE[prof_type] or {}
            local item_score = 0
            local compare_score = 0

            if vo.param and vo.param.attr_list and rate_t then
                for i, j in pairs(vo.param.attr_list) do
                    local rare_ = rate_t[j.attr_type] ~= nil and rate_t[j.attr_type] or 0
                    item_score = item_score + rare_ * j.attr_value
                end
            end
            if compare and compare.param and compare.param.attr_list and rate_t then
                for i, j in pairs(compare.param.attr_list) do
                    local rare_ = rate_t[j.attr_type] ~= nil and rate_t[j.attr_type] or 0
                    compare_score = compare_score + rare_ * j.attr_value
                end
            end
            if item_score > compare_score and v:Level() <= RoleData.Instance:GetRoleLevel() then
                local vo_mark = {}
                vo_mark.pos = k
                vo_mark.item_score = item_score
                if mark_items.jewelry[vo:JewelryType()] == nil then
                    if vo.is_better ~= true then vo.is_better = true end 
                    mark_items.jewelry[vo:JewelryType()] = vo_mark
                elseif item_score > mark_items.jewelry[vo:JewelryType()].item_score then
                    local operate = item_datas[mark_items.jewelry[vo:JewelryType()].pos]
                    if operate.is_better ~= false then operate.is_better = false end 
                    mark_items.jewelry[vo:JewelryType()] = vo_mark
                    if vo.is_better ~= true then vo.is_better = true end 
                end
            else
                if vo.is_better ~= false then vo.is_better = false end 
            end
        elseif vo:IsMedal() then
            local is_full = true
            for k, v in pairs(self.equip_data.medal_info) do
                if v.item_id == 0 then
                    is_full = false
                    break
                end
            end
            if not is_full then
                if vo.is_better ~= true then vo.is_better = true end 
            else
                if vo.is_better ~= false then vo.is_better = false end 
            end
        end
        table.insert(items, vo)
    end
    return items
end

-- 对比体系(简化版)
function AnotherData:CheckIsBetter()
    local total_marks = {}
    for k,v in pairs(self.item_grid) do
        if v:IsEquip() or v:IsJewelry() or v:IsMedal() then 
            if v.is_better then v.is_better = false end 
            local better_mark = BagData.Instance:ItemBetterCheck(v,{},true)
            if better_mark.is_medal ~= nil then 
                table.insert( total_marks,better_mark)
            end 
        end 
    end 

    local try_marks = {}

    for k,v in pairs(total_marks) do 
        if not v.is_medal then 
            if try_marks[v.equip_type] == nil then 
                local vo = {index = v.pos , value =v.value }
                try_marks[v.equip_type] = vo
            elseif try_marks[v.equip_type].value < v.value then 
                local vo = {index = v.pos , value =v.value }
                try_marks[v.equip_type] = vo
            end 
        else 
            self.item_grid[v.pos].is_better = true
        end 
    end 

    local str_ = ""
    for i = 0,7 do 
        local pos_ =  try_marks[i] == nil and -1 or try_marks[i].index
        str_ = i == 7 and str_..pos_ or str_ ..pos_.."|"

        if pos_ > -1 then 
            self.item_grid[pos_ ].is_better = true
        end 
    end 
end 

--化身灵饰列表
function AnotherData:GetJewelryList(index)
    if index == nil then 
        return self.jewelry_list
    else 
        return self.jewelry_list[index]
    end
end

--化身装备列表
function AnotherData:GetEquipList(index)
    if index == nil then 
        return self.equipment_list
    else 
        return self.equipment_list[index]
    end 
end

--化身勋章
function AnotherData:GetMedalId()
    return self.equip_data.medal_id
end

--化身勋章
function AnotherData:GetMedalInfo()
    return self.equip_data.medal_info
end

--化身元素水晶
function AnotherData:GetElementCrystal()
    return self.equip_data.element_crystal
end

--背包列表
function AnotherData:GetColumnGrids()
    return self.item_grid
end 

--===================================化身宠物下发===================================
function AnotherData:SetPetAllInfo(protocol)
    self.pet_list:Set(protocol.pet_list)
    self.pet_count = protocol.pet_count
end

function AnotherData:GetPetList()
    return self.pet_list
end

function AnotherData:GetPetCount()
    return self.pet_count or 0
end

function AnotherData:SetSelect(index)
    self.pet_select.index = index
end

-- 返回当前选择
function AnotherData:GetCurSelect()
    local pet = self:GetPetInfo(self.pet_select.index)
    return pet
end

function AnotherData:GetPetInfo(index)
    return self.pet_list[index]
end

-- 处于 战斗 待机 还是休息  附带 是否是跟随
function AnotherData:GetPetState(index)
    local fight = self:GetFightIndex()
    local flag = PetState.Relax
    if index == fight then
        flag = PetState.Fight
    end
    local standby = self:GetIsStandBy(index)
    if standby then
        flag = PetState.StandBy
    end
    return flag
end

function AnotherData:GetIsStandBy(index)
    for k,v in pairs(self.pet_other_info.standby_list) do
        if v == index then
            return true
        end
    end
    return false
end

function AnotherData:GetFightIndex()
	return self.pet_other_info.fight_index or -1
end

function AnotherData:GetStandbyList()
    return self.pet_other_info.standby_list
end
function AnotherData:IsPetFightIndex(index)
    if not ActivityData.IsOpen(ActType.Battlefield) then
        return false
    end
    if self:GetFightIndex() == -1 then
        return false
    end
    return self:GetFightIndex() == index
end
function AnotherData:IsPetFightIndex(index)
    if not ActivityData.IsOpen(ActType.Battlefield) then
        return false
    end
    if self:GetFightIndex() == -1 then
        return false
    end
    return self:GetFightIndex() == index
end

function AnotherData:SetPetSingleInfo(info)
	self.pet_list[info.index] = info
end

function AnotherData:SetPetOtherInfo(info)
    self.pet_other_info:Set(info)
end

function AnotherData:SetPetAttrList(index, list)
    self.pet_attr_list[index] = list
end

--天赋增长属性(服务端属性 加点属性)
function AnotherData:GiftAttr(pet, attrs, add_attrs)
    return {},{},{},{}
end

function AnotherData:GetIsArena()
	return false
end

-- 存在可待机位置
function AnotherData:GetStandbyCanUse()
    local canuse = true
    local standby = 0
    for k,v in pairs(self.pet_other_info.standby_list) do
        if v == -1 then
            standby = standby + 1
        end
    end
    if standby == 1 and self.pet_other_info.fight_index ~= -1 then
        canuse = false
    end
    if standby == 0 then
        canuse = false
    end
    return canuse
end

--属性
function AnotherData:RealAttr(index, attr_list)
    local t = self:GetPetAttrInfo(index or self:GetSelectPetIndex())
    if index == -1 then
        t = attr_list
    end
    if t == nil then
        return
    end
    local res = {}
    res.maxhp = t[BattleAttriType.MaxHp]
    res.maxmp = t[BattleAttriType.MaxMp]
    res.gongji = t[BattleAttriType.Attack]
    res.fangyu = t[BattleAttriType.Defense]
    res.minjie = t[BattleAttriType.Agility]
    res.jingshen = t[BattleAttriType.Mental]
    res.huifu = t[BattleAttriType.Recovery]
    return res
end

function AnotherData:GetSelectPetIndex()
    return self.pet_select.index
end

function AnotherData:GetPetAttrInfo(index)
    return self.pet_attr_list[index]
end

function AnotherData:GetPetOtherInfo()
	return self.pet_other_info
end

--家族是否训宠
function AnotherData:IsCanTrain(pet_info)
    if ActivityData.Instance:IsNoEnoughLevel(ActType.Battlefield,true) then
		return false
	end
    for k,v in pairs(self:GetPetList() or {}) do
        if v.info.pet_unique_id1 == pet_info.pet_unique_id1 and v.info.pet_unique_id2 == pet_info.pet_unique_id2 then
            return self:IsFightState(v.index)
        end
    end
    return false
end

--是否在出战 待机
function AnotherData:IsFightState(index)
    for k,v in pairs(self:GetPetOtherInfo()) do
        if index == v then
            return true
        end
    end
    return false
end

function AnotherData:GetPetEquipmentList()
    local list = {}
    for k,v in pairs(self.item_grid) do
        if Item.GetBigType(v.item_id) == ITEM_BIG_TYPE.PET_EQUIPMENT then
            table.insert(list,v)
        end
    end
    table.sort(list, function(a, b)
        return a.item_id > b.item_id
    end)
    return list
end

function AnotherData:GetEquipShowList()
    local equip_list = self:GetPetEquipmentList()
    if #equip_list == 0 then
        return nil
    end

    local item_id = self.pet_equip_select.item_type == 1 and 0 or self:GetPetItemTypeId()

    if item_id == 0 and self.pet_equip_select.skill_type == 0 then
        return equip_list
    end
    local item_type_list = {}
    for k, v in pairs(equip_list) do
        if v.item_id == item_id or item_id == 0 then
            table.insert(item_type_list, v)
        end
    end
    if self.pet_equip_select.skill_type == 0 then
        return item_type_list
    end
    local skill_type_list = {}
    for k, v in pairs(item_type_list) do
        for i = 1, MAX_PET_EQUIPMENT_SKILL_NUM do
            if v.param.equipment_skill_list[i] ~= 0 then
                if Cfg.PetSkillPageById(v.param.equipment_skill_list[i]).page == self.pet_equip_select.skill_type then
                    table.insert(skill_type_list, v)
                    break
                end
            end
        end
    end
    return skill_type_list
end

function AnotherData:GetPetItemTypeId()
    local type_list = PetData.Instance:GetEquipItemTypeList()
    for k, v in pairs(type_list) do
        if self.pet_equip_select.item_type == v.item_type then
            return v.item_id
        end
    end
end

--获取化身对应属性
function AnotherData:GetAttrTypeData(attr_data,config,prof,main_attribute)
    local list = {}
    for i,v in ipairs(config) do
        local attr_type = AttrTypeForName[v]
        local attr_value = self:GetAttrValue(attr_data,attr_type)
        table.insert(list, {
            attr_type = attr_type,
            attr_value = attr_value,
            attr_name = Language.Common.AttrList[attr_type] .. [[:]],
            attr_limit = self:GetAttrLimitNum(attr_type,prof,main_attribute) or 0
        })
    end
    return list
end

function AnotherData:GetAttrValue(data,attr_type)
    if attr_type == BattleAttriType.MaxHp then
        return data[BattleAttriType.CurHp] or 0
    end
    if attr_type == BattleAttriType.MaxMp then
        return data[BattleAttriType.CurMp] or 0
    end
	return data[attr_type] or 0
end

--获取化身对应属性最大值
function AnotherData:GetAttrLimitNum(attr_type,prof,main_attribute)
    local num = 0
    local key = [[max_]] .. AttrNameForType[attr_type]
    for k,v in pairs(Config.illusory_battlefield_auto.equipment_limit) do
        if v.job_type == (prof or 0) and v.main_attribute == (main_attribute or 0) then
            if v[key] then
                num = num + v[key]
            end
        end
    end
	return num
end

--获取人物等级
function AnotherData:GetRoleLevel()
    return self.role_data.role_level
end