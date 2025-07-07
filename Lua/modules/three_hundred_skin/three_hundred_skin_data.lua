ThreeHundredSkinData = ThreeHundredSkinData or BaseClass()

function ThreeHundredSkinData:__init()
    if ThreeHundredSkinData.Instance ~= nil then
        Debuger.LogError("[ThreeHundredSkinData] attempt to create singleton twice!")
        return
    end
    ThreeHundredSkinData.Instance = self
    self.all_config = Config.sanbaichoupifu_auto
    self.config = self.all_config.basis_configure
    self.reward_group = self.all_config.reward_group
    self.other = self.all_config.other[1]
    self.info = SmartData.New({group = 0, total = 0, reward_item_seq = -1})
    self.select_list = SmartData.New()
    self:initConfig()
    self.index = -1
end
function ThreeHundredSkinData:initConfig()
    self.reward_group_list = {}
    for i, v in ipairs(self.reward_group) do
        self.reward_group_list[v.reward_group] = self.reward_group_list[v.reward_group] or {}
        table.insert(self.reward_group_list[v.reward_group], v)
    end
end
function ThreeHundredSkinData:Info()
    return self.info
end
function ThreeHundredSkinData:SetGroup(group)
    self.info.group = group
end
function ThreeHundredSkinData:RewardGroup(group)
    return self.reward_group_list[group]
end
function ThreeHundredSkinData:RamdomReward()
    if self.info.group == 0 then
        return
    end
    local list = self:RewardGroup(self.info.group)
    local indexs = DataHelper.GetRamdonList(1, table.nums(list))
    local result = {}
    for i = 1, 10 do
        result[i] = list[indexs[i]]
    end
    return result
end
function ThreeHundredSkinData:ClearData()
end

function ThreeHundredSkinData:SetTemplateInfo(protocol)
end

function ThreeHundredSkinData:Config()
    return self.config
end
function ThreeHundredSkinData:SelectList()
    return self.select_list
end
function ThreeHundredSkinData:SetSelectInfo(list)
    self.select_list:Set(list)
    self.select_list:Notify()
end
function ThreeHundredSkinData:SetSingleInfo(protocol)
    local type = protocol.type or self.info.group
    self.select_list:Val()[type].pool = protocol.pool
    self.select_list:Val()[type].times = protocol.type_times
    self.select_list:Val()[type].select_seq = protocol.select_seq
    self.select_list:Notify()
end
function ThreeHundredSkinData:SelectReward(type)
    local reward = self:SelectInfo(type)
    if reward then
       return reward.reward_item
    end
    return nil
end
--服务端信息
function ThreeHundredSkinData:SerInfo(type)
    return self.select_list:Val()[type]
end
--奖励信息
function ThreeHundredSkinData:SelectInfo(type)
    local info = self:SerInfo(type)
    if not info then
        LogDG("没用收到协议下发的数据")
        return nil
    end 
    local reward = self.reward_group[info.select_seq + 1]
    if reward then
        reward.info = info
    end
    return reward
end
function ThreeHundredSkinData:IsOpenLevel(info)
    local level = RoleData.Instance:GetRoleLevel()
    if info then
        return level >= info.level
    end
    return false
end
function ThreeHundredSkinData:IsCloseTimes(info)
    local ser_info = self:SerInfo(info.type + 1)
    if ser_info then
        return ser_info.times == info.extract_times and ser_info.select_seq ~= -1 and ser_info.pool[1].value == -1
    end
    return false
end
function ThreeHundredSkinData:IsCloseTimesCanSelect(info)
    local ser_info = self:SerInfo(info.type + 1)
    if ser_info then
        return ser_info.times == info.extract_times and ser_info.select_seq ~= -1 and ser_info.pool[1].value ~= -1
    end
    return false
end
function ThreeHundredSkinData:IsCloseTimesButNoSelect(info)
    local ser_info = self:SerInfo(info.type + 1)
    if ser_info then
        return ser_info.times == info.extract_times and ser_info.select_seq == -1 and ser_info.pool[1].value ~= -1
    end
    return false
end
function ThreeHundredSkinData:IsMaxTime(info)
    local ser_info = self:SerInfo(info.type + 1)
    if ser_info then
        return ser_info.times == info.extract_times
    end
    return false
end
--奖励内容
function ThreeHundredSkinData:RewardInfo(type)
    return self.reward_group[type]
end

function ThreeHundredSkinData:UpLevel()
    local data = self:Config()
    local level = RoleData.Instance:GetRoleLevel()
    local uplevel = 0
    for i, v in ipairs(data) do
        if v.level > level then
            uplevel = v.level
            break
        end
    end
    return uplevel - level
end
function ThreeHundredSkinData:SetIndex(index)
    self.index = index
end
function ThreeHundredSkinData:Index()
    return self.index
end
function ThreeHundredSkinData:GetRemindNum()
    local config = self:Config()
    local level = RoleData.Instance:GetRoleLevel()
    local ser_info
    for i, v in ipairs(config) do
        if level >= v.level then
            if self:TypeRedPoint(v) == 1 then
                return 1
            end
        end
    end
    if self:CanGet() == 1 then
        return 1
    end
    return 0
end
function ThreeHundredSkinData:FinalCanGet()
    local config = self:Config()
    local level = RoleData.Instance:GetRoleLevel()
    local ser_info
    for i, v in ipairs(config) do
        if level >= v.level then
            if self:TypeRedPoint(v) == 1 then
                return false
            end
        end
    end
    if self:CanGet() == 1 then
        return true
    end
    return false
end
function ThreeHundredSkinData:CanGet()
    if self.info.total == self.other.reward_extract_times
    and self.info.reward_item_seq == -1 then
        --可领取
        return 1
    end
    return 0
end
function ThreeHundredSkinData:TypeRedPoint(data)
    local ser_info = self:SerInfo(data.type + 1)
    if ser_info and data.extract_times - ser_info.times > 0 then
        --有次数
        return 1
    else
        --没次数也没有预设
        if ser_info and ser_info.select_seq == -1 then
            return 1
        end
        if ser_info and ser_info.pool[1].value ~= -1 then
            return 1
        end
    end
    return 0
end