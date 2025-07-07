PetRefiningData = PetRefiningData or BaseClass()
function PetRefiningData:__init()
    if PetRefiningData.Instance ~= nil then
        Debuger.LogError("[PetRefiningData] attempt to create singleton twice!")
        return
    end
    PetRefiningData.Instance = self
    self.all_info = Config.pet_lianyao_auto
    self.other_info = self.all_info.other[1]
    self.luzi_list = self.all_info.furnace
    self.pet_unlock_info = SmartData.New()
    self.view_data = SmartData.New({
        stove_idx = 0,
        pet_1 = nil,
        pet_2 = nil,
        select_left = false,
        select_right = false,
        refining_ing = false,
        has_result = false,
    })
    self.result_pet = nil
end
function PetRefiningData:__delete()
    
end
function PetRefiningData:Clear()
    self.view_data.stove_idx = 0
    self.view_data.pet_1 = nil
    self.view_data.pet_2 = nil
end
function PetRefiningData:SetStove(idx)
    self.view_data.stove_idx = idx
end
function PetRefiningData:SetPet1(pet)
    self.view_data.pet_1 = pet
end
function PetRefiningData:SetPet2(pet)
   self.view_data.pet_2 = pet
end
function PetRefiningData:Stove()
    return self.view_data.stove_idx
end
function PetRefiningData:Pet1()
    return self.view_data.pet_1
end
function PetRefiningData:Pet2()
    return self.view_data.pet_2
end
function PetRefiningData:ViewData()
    return self.view_data
end
function PetRefiningData:LuZiList()
    return self.luzi_list
end
--爐子解鎖數量
function PetRefiningData:UnlockCount()
    local list = self:LuZiList()
    local total = table.nums(list)
    local unlock = 0
    for _, value in ipairs(list) do
        if self:IsUnlock(value) then
            unlock = unlock + 1
        end
    end
    --当第4个没有到等级显示X/3
    --RoleData.Instance:GetRoleAvatarQuality()
    local role_level = RoleData.Instance:GetRoleLevel()
    if role_level < list[4].open_level then
        total = total - 1
    end
    return unlock, total
end
function PetRefiningData:IsUnlock(info)
    local role_level = RoleData.Instance:GetRoleLevel()
    local fly_flag = RoleData.Instance:GetRoleAvatarQuality()
    if info.feisheng_level == 0 and role_level >= info.open_level then
        if info.pet_quality > 0  then
            return self:GetUnlockInfo(info.furnace_id) == 1
        end
        return true
    end
    if info.feisheng_level ~= 0
    and role_level >= info.open_level
    and fly_flag >= info.feisheng_level then
        return true
    end
    return false
end

function PetRefiningData:MainPetList()
    --根据炉子id条件显示可选宠物列表
    local id = self.view_data.stove_idx
    if id > 0 then
        local pets = PetData.Instance:GetPetList()
        local pet2 = self:Pet2()
        local list = {}
        local info = self:LuZiList()[id]
        for _, pet in pairs(pets) do
            if pet:Quality() >= PetData.Quality.Jin and pet:Quality() <= info.main_max_quality
            and pet:StrLevel() >= info.main_min_strengthen then
                --假设pet2存在那么剔除
                table.insert(list, pet)
            end
        end
        if pet2 ~= nil then
            local idx = 0
            for index, pet in ipairs(list) do
                if pet:Index() == pet2:Index() then
                    idx = index
                end
            end
            if idx > 0 then
                table.remove(list, idx)
            end
        end
        table.sort(list, function (a, b)
            return a:StrLevel() > b:StrLevel()
        end)
        return list
    end
    return nil
end

function PetRefiningData:SecPetList()
    --根据炉子id条件显示可选宠物列表
    local id = self.view_data.stove_idx
    if id > 0 then
        local pets = PetData.Instance:GetPetList()
        local pet1 = self:Pet1()
        local list = {}
        local info = self:LuZiList()[id]
        for _, pet in pairs(pets) do
            if pet:Quality() >= PetData.Quality.Jin and pet:Quality() <= info.second_max_quality
            and pet:StrLevel() <= info.second_max_strengthen then--注意这里是最高强化即不能超过
                --假设pet2存在那么剔除
                table.insert(list, pet)
            end
        end
        if pet1 ~= nil then
            local idx = 0
            for index, pet in ipairs(list) do
                if pet:Index() == pet1:Index() then
                    idx = index
                end
            end
            if idx > 0 then
                table.remove(list, idx)
            end
        end
        table.sort(list, function (a, b)
            return a:StrLevel() > b:StrLevel()
        end)
        return list
    end
    return nil
end
function PetRefiningData:SetResultPet(pet)
    self.result_pet = pet
end
function PetRefiningData:ResultPet()
    return self.result_pet
end
function PetRefiningData:SetUnlockInfo(info)
    self.pet_unlock_info:Set(info)
end
function PetRefiningData:GetUnlockInfo(id)
    return self.pet_unlock_info:Val()[id] or 0
end