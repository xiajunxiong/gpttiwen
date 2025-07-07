PetResonanceData = PetResonanceData or BaseClass()
PetResonanceData.MAX_ATTRI_COUNT = 7    -- 共鸣加成最大属性条数
function PetResonanceData:__init()
    if PetResonanceData.Instance ~= nil then
        Debuger.LogError("[PetResonanceData] attempt to create singleton twice!")
        return
    end
    PetResonanceData.Instance = self
    self.cfg = Config.pet_cfg_auto.pet_strengthen_buff
    self.other_cfg = Config.pet_cfg_auto.other[1]
    self.info_data = SmartData.New()
    self.last_info_data = nil       -- 上一次的数据，用来作比较

    self.past_Level_info = {}

    -- 选择的宠物
    self.selected_pets = SmartData.New()

    Cfg.Register("PetResonancCfg",Cfg.RealTimeSearch2Key(self.cfg, "pets_quality", "resonance_stage"))

    -- 特效提醒
    self.effect_play = SmartData.New()

    self.qua_dialog = nil  -- 打开弹窗提示的品质
end

function PetResonanceData:ClearData()
    self.info_data:Set({})
    self.last_info_data = nil
    self.past_Level_info = {}
end

function PetResonanceData:SetInfoData(protocol)
    if not TableIsEmpty(self.info_data) then
        self.last_info_data = {}
        for qua = 2, 5 do
            self.last_info_data[qua] = self:GetQuaLevel(qua)
        end
    end
    self.info_data:Set(protocol.info)
    self.past_Level_info = protocol.past_Level_info

    local t_qua = self:GetDialogQua()
    if t_qua then
        self.qua_dialog = t_qua
    end
end

--获取格子信息
function PetResonanceData:GetCellInfo(qua, cell_index)
    if TableIsEmpty(self.info_data) then
        return
    end
    return self.info_data[qua][cell_index]
end

function PetResonanceData:GetPet(uid_t)
    local pet_list = PetData.Instance:GetPetList()
    for i,v in pairs(pet_list) do
        local pet_info = v.info
        if pet_info.pet_unique_id1 == uid_t.uid_1 and pet_info.pet_unique_id2 == uid_t.uid_2 then
            return v
        end
    end
    -- 仓库列表
    pet_list = PetData.Instance:GetStorageList()
    for i,v in pairs(pet_list) do
        local pet_info = v.info
        if pet_info.pet_unique_id1 == uid_t.uid_1 and pet_info.pet_unique_id2 == uid_t.uid_2 then
            return v
        end
    end
end

function PetResonanceData:GetQuaCfgList(qua)
    if not self.qua_group_cfg then
        self.qua_group_cfg = DataHelper.ConvertGroup(self.cfg, "pets_quality")
    end
    local max_strlevel = Pet.StrengthMaxLv(qua)
    local list = {}
    for i,v in pairs(self.qua_group_cfg[qua]) do
        if v.strengthening_level <= max_strlevel then
            table.insert(list, v)
        end
    end
    return list
end

-- 默认为 3
function PetResonanceData:ConditionCount()
    return self.other_cfg.pets_number or 3
end

-- 获取格子数量
function PetResonanceData:GetMaxCell(qua)
    local cfg = self:GetQuaCfgList(qua)
    return cfg[#cfg].unlock_number
end

-- state == 1 有宠物，0可添加，-1锁住
function PetResonanceData:GetCellData(qua, cell_index)
    local cell_info = self:GetCellInfo(qua, cell_index)
    if not TableIsEmpty(cell_info) and cell_info.uid_1 ~= 0 then
        local pet = self:GetPet(cell_info)
        if not pet then
            LOG("宠物强化异常!! 未找到宠物", cell_info)
            return {pet = nil, state = 0}
        end
        return {pet = pet, state = 1}
    end

    local info = {}
    local unlock_num = self:GetUnlockNum(qua)
    local state = 0
    if cell_index > unlock_num then
        state = -1
    else
        state = 0
    end
    info.state = state
    return info
end

-- 获取可添加的孔
function PetResonanceData:GetCanAddList(qua)
    local max_cell = self:GetMaxCell(qua)
    local list = {}
    for i = 1, max_cell do
        local state = self:GetCellData(qua, i).state
        if state == 0 then
            local is_sel = false
            for pet, v in pairs(self.selected_pets) do
                if i == v then
                    is_sel = true
                    break
                end
            end
            if not is_sel then
                table.insert(list, i)
            end
        end
    end
    return list
end

-- 获取这个品质解锁了多少数量
function PetResonanceData:GetUnlockNum(qua)
    local cur_level = self:GetPastQuaLevel(qua)     -- 历史解锁的最高共鸣等级
    if cur_level < 1 then
        return self.other_cfg.initial_lattice
    end
    local cfg = Cfg.PetResonancCfg(qua, cur_level)
    if not cfg then
        return self.other_cfg.initial_lattice
    end
    return cfg.unlock_number
end

-- 获取解锁的历史最高共鸣等级
function PetResonanceData:GetPastQuaLevel(qua)
    if TableIsEmpty(self.past_Level_info) then
        return 0
    end
    return self.past_Level_info[qua] or 0
end

-- 获取解锁的共鸣等级
-- 宠物强化共鸣的阶数，根据选择进行强化共鸣的宠物中，强化等级最高的三只中，等级最低的那只宠物决定。
function PetResonanceData:GetQuaLevel(qua)
    if TableIsEmpty(self.info_data) then
        return 0
    end
    local source_list = self:GetResonanceSource(qua)
    local condition_count = self:ConditionCount()
    if #source_list < condition_count then
        return 0
    end
    local str_level = source_list[3]:StrLevel()
    local cfg_group = self:GetQuaCfgList(qua)
    local target_level = 0
    for i,v in pairs(cfg_group) do
        if str_level >= v.strengthening_level then
            target_level = v.resonance_stage
        else
            break
        end
    end
    return target_level
end

-- 获取触发共鸣的对象列表 ： 强化等级最高的三只
function PetResonanceData:GetResonanceSource(qua)
    if TableIsEmpty(self.info_data) then
        return {}
    end
    local cell_info_list = self.info_data[qua]
    local pet_list = {}
    for i,v in pairs(cell_info_list) do
        local pet = self:GetPet(v)
        if not TableIsEmpty(pet) then
            table.insert(pet_list, pet)
        end
    end
    local condition_count = self:ConditionCount()
    table.sort(pet_list, function (a,b)
        return a:StrLevel() > b:StrLevel()
    end)
    if #pet_list <= condition_count then
        return pet_list
    end
    local t = {}
    for i = 1, condition_count do
        table.insert(t, pet_list[i])
    end
    return t
end

-- 获取解锁共鸣等级进度
function PetResonanceData:GetProgress(qua, level)
    if TableIsEmpty(self.info_data) then
        return 0, self:ConditionCount()
    end
    local qua_level = self:GetQuaLevel(qua)
    if qua_level >= level then
        return self:ConditionCount(),self:ConditionCount()
    end

    local cfg = Cfg.PetResonancCfg(qua, level)
    local target_level = cfg.strengthening_level

    local cell_info_list = self.info_data[qua]
    local num = 0
    for i,v in pairs(cell_info_list) do
        local pet = self:GetPet(v)
        if not TableIsEmpty(pet) then
            if pet:StrLevel() >= target_level then
                num = num + 1
            end
        end
    end

    return num,self:ConditionCount()
end

-- 提取属性list 属性值为0的剔除
function PetResonanceData:GetAttriList(cfg)
    local list = {}
    for i = 1, PetResonanceData.MAX_ATTRI_COUNT do
        local attri_type = cfg["attr_type_" .. i]
        local attri_value = cfg["attr_add_" .. i]
        if attri_value and attri_value > 0 then
            local info = CommonStruct.AttrStruct(attri_type, attri_value)
            info.value_text = info.attr_percent_value
            info.name = info.attr_name
            table.insert(list, info)
        end
    end
    return list
end

-- 获取累计加了多少属性
function PetResonanceData:GetAddAttriList(qua)
    local cur_level = self:GetQuaLevel(qua)     -- 当前共鸣等级
    if cur_level < 1 then
        return
    end
    -- >>>>>> 不累加
    -- local cfg_list = self:GetQuaCfgList(qua)
    -- local attri_list = {}
    -- for i,v in pairs(cfg_list) do
    --     if cur_level >= v.resonance_stage then
    --         for n = 1, PetResonanceData.MAX_ATTRI_COUNT do
    --             local attri_type = v["attr_type_" .. n]
    --             local attri_value = v["attr_add_" .. n]
    --             if not attri_list[attri_type] then
    --                 attri_list[attri_type] = {attri_type = attri_type, attri_value = 0}
    --             end
    --             attri_list[attri_type].attri_value = attri_list[attri_type].attri_value + attri_value
    --         end
    --     end
    -- end

    -- local list = {}
    -- for i,v in pairs(attri_list) do
    --     if v.attri_value > 0 then
    --         local info = CommonStruct.AttrStruct(v.attri_type, v.attri_value)
    --         info.value_text = info.attr_percent_value
    --         info.name = info.attr_name
    --         table.insert(list, info)
    --     end
    -- end
    
    local cfg = Cfg.PetResonancCfg(qua, cur_level)
    local attri_list = self:GetAttriList(cfg)

    
    return attri_list
end



-- 设置选中的品质
function PetResonanceData:SetSelCell(data)
    self.sel_qua_data = data
end
function PetResonanceData:GetSelCell()
    return self.sel_qua_data or {qua = 2, cell_index = 1}
end

-- 获取可以被选择的宠物列表
function PetResonanceData:GetPetList(qua)
    local list = {}
    local pet_list = PetData.Instance:GetPetList()
    for i,v in pairs(pet_list) do
        if v:Quality() == qua and not v:IsReson() then
            table.insert(list, v)
        end
    end
    return list
end

function PetResonanceData:GetDialogQua()
    if TableIsEmpty(self.last_info_data) then
        return
    end
    local t_qua
    for qua = 2, 5 do
        local last_level = self.last_info_data[qua]
        local cur_level = self:GetQuaLevel(qua)
        if cur_level > last_level then
            t_qua = qua
            break
        end
    end
    return t_qua
end


-- 检查此次数据变化触发了哪个共鸣
function PetResonanceData:CheckResonance(is_check)
    local t_qua = self.qua_dialog

    if is_check then
        t_qua = self:GetDialogQua()
    end

    if t_qua then
        local last_level = self.last_info_data[t_qua] or 0
        ViewMgr:OpenView(PetResonanceTip, {qua = t_qua, last_level = last_level})
        self.qua_dialog = nil
    end
end


function PetResonanceData:SelPet(qua, pet)
    if self.selected_pets[pet] then
        self.selected_pets[pet] = nil
        return true
    end
    local list = self:GetCanAddList(qua)
    if TableIsEmpty(list) then
        return false
    end
    self.selected_pets[pet] = list[1]
    return true
end

function PetResonanceData:IsSel(pet)
    return self.selected_pets[pet] ~= nil
end

function PetResonanceData:ClearSel()
    self.selected_pets:Set({})
end

function PetResonanceData:PetIsReson(pet)
    local pet_info = pet.info
    local qua = pet:Quality()
    local cell_info_list = self.info_data[qua]
    for i,v in pairs(cell_info_list) do
        if pet_info.pet_unique_id1 == v.uid_1 and pet_info.pet_unique_id2 == v.uid_2 then
            return true
        end
    end
    return false
end


-- 获取格子解锁等级
function PetResonanceData:GetCellUnlockLevel(qua, cell_index)
    local cfg = self:GetQuaCfgList(qua)
    for i,v in pairs(cfg) do
        if v.unlock_number >= cell_index then
            return v.resonance_stage
        end
    end
    return 1
end


-- 获取特效预览列表
function PetResonanceData:GetPreviewEffectList(qua, level)
    local cfg = self:GetQuaCfgList(qua)
    level = level or self:GetQuaLevel(qua)
    local list = {}
    for i,v in pairs(cfg) do
        if v.sp_id > 0 then
            local info = {}
            info.buff_id = v.sp_id
            if level < v.strengthening_level then
                info.active_level = v.strengthening_level
            end
            table.insert(list, info)
        end
    end
    return list
end
