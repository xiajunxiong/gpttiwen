VIEW_REQ("modules/pet/pet_inborn/pet_inborn_view")
PetInbornCtrl = PetInbornCtrl or BaseClass(BaseCtrl)

-- ================= 宠物天赋 =================

function PetInbornCtrl:__init()
    if PetInbornCtrl.Instance ~= nil then
		Debuger.LogError("[PetInbornCtrl] attempt to create singleton twice!")
		return
	end
    PetInbornCtrl.Instance = self
    self.data = PetInbornData.New()
    BagData.Instance:ListenItems({self.data:StuffItemId()},function ()
        PetInbornData.Instance.remind_data:Notify()
    end)
    RemindCtrl.Instance:Remind():Register(
        Mod.Pet.PetAttr,
        self.data.remind_data,
        function ()
            return PetInbornData.Instance:GetRemindNum()
        end
    )
end

function PetInbornCtrl:__delete()
    PetInbornCtrl.Instance = nil
    self.data = nil
end

function PetInbornCtrl:OnUnloadGameLogic()
    self.data:Reset()
end

function PetInbornCtrl:HandelPetChange(new_pet, old_pet, notie_type)
    local state = new_pet:State()
    if state ~= PetState.Fight and state ~= PetState.StandBy then
        return
    end
    if not old_pet then
        return
    end
    local old_str_level = old_pet:StrLevel()
    local new_str_level = new_pet:StrLevel()
    if new_str_level > old_str_level or notie_type == PetNotieType.PET_NOTIE_TYPE_STATUS then
        local flag = self.data:GetPetInbornFlag(new_pet, true)
        if flag > 0 then
            self.data:SetRemindStuffNum(0)
        end
    end
end



PetInbornData = PetInbornData or BaseClass()

function PetInbornData:__init()
    if PetInbornData.Instance ~= nil then
		Debuger.LogError("[PetInbornData] attempt to create singleton twice!")
		return
	end
    PetInbornData.Instance = self
    self:InitCfg()

    -- 宠物天赋升级信息
    self.inborn_up_data = SmartData.New()
    -- 红点提醒
    self.remind_data = SmartData.New({stuff_num = 0})
    -- 解锁信息
    self.inborn_unlock_data = SmartData.New({inborn_group_type = 0, grid_index = 0})
end

function PetInbornData:Reset()
    self.remind_data.stuff_num = 0
end

function PetInbornData:InitCfg()
    self.cfg = Config.pet_inborn_auto
    -- 天赋类型组
    self.inborn_type_group = DataHelper.ConvertGroup(self.cfg.pet_inborn_base, "inborn_type")
    -- 天赋id组
    self.inborn_id_group = DataHelper.ConvertGroup(self.cfg.pet_inborn_level, "inborn_id")
end

function PetInbornData:GetInbornTypeGroup(_type)
    if _type then
        return self.inborn_type_group[_type] or {}
    end
    return self.inborn_type_group
end

function PetInbornData:GetInbornIdGroup(id)
    if id then
        return self.inborn_id_group[id] or  {}
    end
    return self.inborn_id_group
end

-- 获取天赋id最大等级
function PetInbornData:GetInbornMaxLevel(id)
    local group = self:GetInbornIdGroup(id)
    return #group
end

function PetInbornData:GetInbornIdCfg(id, level)
    local cfg_group = self:GetInbornIdGroup(id)
    for i,v in pairs(cfg_group) do
        if v.inborn_id == id and level == v.inborn_level then
            return v
        end
    end
end

-- 显示的消耗材料
function PetInbornData:StuffItemId()
    return self.cfg.pet_inborn_level[1].strengthen_consume_item[0].item_id
end

function PetInbornData:SetInboenUpData(data)
    self.inborn_up_data:Set(data)
end

function PetInbornData:GetInboenUpData()
    return self.inborn_up_data
end

--宠物是否能升级天赋 2,可解锁，1可升级，0材料不足，-1，强化等级不足，-2已满级, -3没有天赋, -4 上一个未解锁
function PetInbornData:GetPetInbornUpFlag(inborn_index, pet)
    pet = pet or PetData.Instance:GetCurSelect()
    local inborn_type = pet:PetCfg().inborn_type
    if not inborn_type or inborn_type == 0 then
        return -3
    end
    local info = pet:InbornInfo(inborn_index)
    local inborn_type_cfg = self:GetInbornTypeGroup(inborn_type)
    local inborn_index_cfg = inborn_type_cfg[inborn_index]
    if not inborn_index_cfg then
        return -3
    end
    if info.inborn_level == 0 then
        local str_level = pet:StrLevel()
        if str_level < inborn_index_cfg.inborn_group_lock then
            return -1
        end
        -- 上一个解锁了才能解锁
        if inborn_index > 1 then
            local last_inborn_info = pet:InbornInfo(inborn_index - 1)
            if last_inborn_info.inborn_level == 0 then
                return -4
            end
        end
        local inborn_id = inborn_index_cfg.inborn_1
        local inborn_id_cfg = self:GetInbornIdCfg(inborn_id, 1)
        local item_id = inborn_id_cfg.strengthen_consume_item[0].item_id
        local need_num = inborn_id_cfg.strengthen_consume_item[0].num
        local has_num = Item.GetNum(item_id)
        if has_num >= need_num then
            return 2
        else
            return 0
        end
    else
        local grid_pos = info.inborn_grid
        local inborn_id = inborn_index_cfg["inborn_" .. grid_pos]
        local max_level = self:GetInbornMaxLevel(inborn_id)
        if info.inborn_level >= max_level then
            return -2
        end
        local inborn_id_cfg = self:GetInbornIdCfg(inborn_id, info.inborn_level + 1)
        local item_id = inborn_id_cfg.strengthen_consume_item[0].item_id
        local need_num = inborn_id_cfg.strengthen_consume_item[0].num
        local has_num = Item.GetNum(item_id)
        if has_num >= need_num then
            return 1
        else
            return 0
        end
    end
end

function PetInbornData:GetPetInbornFlag(pet, is_not_remind)
    if not is_not_remind then
        if not self:IsCanRemind() then
            return 0
        end
        local state = pet:State()
        if state ~= PetState.Fight and state ~= PetState.StandBy then
            return 0
        end
    end
    local inborn_type = pet:PetCfg().inborn_type
    if not inborn_type or inborn_type <= 0 then
        return 0
    end
    local group = self:GetInbornTypeGroup(inborn_type)
    for i,v in pairs(group) do
        local flag = self:GetPetInbornUpFlag(v.inborn_group_type,pet)
        if flag > 0 then
            return 1
        end
    end
    return 0
end


-- 红点逻辑
-- 1.材料足够解锁触发解锁红点和解锁特效
-- 红点路径：主界面宠物按钮-属性页签-宠物头像区域-宠物天赋功能按钮-解锁按钮（解锁按钮区域还需新增一个解锁提示特效）
-- 2.材料足够升级触发天赋升级红点
-- 红点路径：主界面宠物按钮-属性页签-宠物头像区域-宠物天赋功能按钮-升级按钮
-- 重点：以上红点逻辑，只针对上阵宠物才出现；
-- 红点消失逻辑：进入宠物天赋界面后，无论玩家是否进行操作。退出该界面后红点消失，本次登录期间解锁和升级材料发生变化则再次触发红点，否则下次登录后，才会再次出现红点
function PetInbornData:GetRemindNum()
    if not self:IsCanRemind() then
        return 0
    end

    local pet_list = PetData.Instance:GetPetList()
    for _,pet in pairs(pet_list) do
        local flag = self:GetPetInbornFlag(pet)
        if flag > 0 then
            return 1
        end
    end
    return 0
end

function PetInbornData:IsCanRemind()
    local item_id = self:StuffItemId()
    local stuff_num = Item.GetNum(item_id)
    local remind_num = self.remind_data.stuff_num
    if stuff_num <= remind_num then
        self.remind_data.stuff_num = stuff_num
        return false
    end
    return true
end

function PetInbornData:SetRemindStuffNum(num)
    num = num or 0
    self.remind_data.stuff_num = num
end