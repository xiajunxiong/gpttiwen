PeakRoadData = PeakRoadData or BaseClass()

function PeakRoadData:__init()
    if PeakRoadData.Instance ~= nil then
        Debuger.LogError("[PeakRoadData] attempt to create singleton twice!")
        return
    end
    PeakRoadData.Instance = self
    self.config = Config.roleexp_auto

    self.peak_data = SmartData.New({
        level = 0,
        timestamp = 0,
        reward_fetch_flag = nil,
        is_open = nil,
        reward_refresh = false,
        achieve_flag = nil,
        achieve_refresh = false,
    })
    self.achieve_list = {}
    self.effect_open = false
    self:init_config()
end
function PeakRoadData:init_config()
    for _, value in ipairs(self.config.top_achieve) do
        value.rewards = {Item.Create(value.item[0])}
        table.insert(self.achieve_list, value)
    end
end
function PeakRoadData:FlushRewardList()
    self.reward_list = {}
    local config = self.config.top_level
    local o
    local items
    local data
    for i = 1, 999 do
        if i <= 400 then
            o = i
        else
            --复用201-400
            o = i - (math.floor(i / 200) - 1) * 200
            --[[ if math.floor(i / 200) <= 1 then
                o = i - (math.floor(i / 200) - 1) * 200
            else
                o = i - (math.floor(i / 200) - 1) * 200 + 1
            end ]]
            if i % 200 == 0 then
                o = i - (math.floor(i / 200) - 2) * 200
            end
        end
        items = {}
        for _, value in pairs(config[o].item_list) do
            table.insert(items, Item.Create(value))
        end
        -- if not self:HadGet(i) then
            data = DataHelper.TableCopy(config[o])
            --[[ config[o].items = items
            table.insert(self.reward_list, config[o]) ]]
            self.reward_list[i] = data
            self.reward_list[i].items = items
            self.reward_list[i].peak_level = i
        -- end
    end
end
function PeakRoadData:Clear() 
    self.peak_data.is_open = nil 
    self.effect_open = false
end
function PeakRoadData:GetRewardList()
    self:FlushRewardList()
    return self.reward_list
end
function PeakRoadData:PeakData() return self.peak_data end
function PeakRoadData:CanGet(level)
    if self.peak_data.reward_fetch_flag then
        return self.peak_data.reward_fetch_flag[level] == 0 and
                   self.peak_data.level >= level
    end
    return false
end
function PeakRoadData:HadGet(level)
    if self.peak_data.reward_fetch_flag then
        return self.peak_data.reward_fetch_flag[level] == 1
    end
    return true
end
function PeakRoadData:IsPeakAndOpen()
    local level = RoleData.Instance:GetRoleLevel()
    if level == 120 and self:PeakData().is_open == true then return true end
    return false
end
function PeakRoadData:IsPeakButNoOpen()
    local level = RoleData.Instance:GetRoleLevel()
    if level == 120 and self:PeakData().is_open == false then return true end
    return false
end
function PeakRoadData:RewardCanGet()
    for i = 1, 999 do
        if self:CanGet(i) then
            return 1
        end
    end
    return self:AchieveNum()
end
function PeakRoadData:SelectIndex()
    for i = 1, 999, 1 do
        if self:CanGet(i) or not self:HadGet(i) then
            return i
        end
    end
end
function PeakRoadData:GetAchieveCfg()
    for index, value in ipairs(self.achieve_list) do
        value.sort_value = 1 + value.seq
        if self:AchiHadGet(value.seq + 1) then
            value.sort_value = 999 + value.seq
        end
    end
    table.sort(self.achieve_list, DataSorter.KeyLowerSorter("sort_value"))
    return self.achieve_list
end
function PeakRoadData:AchiCanGet(seq, level)
   return self.peak_data.achieve_flag[seq] == 0 and self.peak_data.level >= level
end
function PeakRoadData:AchiHadGet(seq)
    return self.peak_data.achieve_flag[seq] == 1
end
function PeakRoadData:AchieveNum()
    local can_get = false
    for index, value in ipairs(self.achieve_list) do
        can_get = self:AchiCanGet(value.seq + 1, value.param)
        if can_get == true then
            break
        end
    end
    return can_get and 1 or 0
end