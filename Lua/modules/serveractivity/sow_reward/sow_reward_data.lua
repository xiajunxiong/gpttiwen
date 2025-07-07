SowRewardData = SowRewardData or BaseClass()
function SowRewardData:__init()
    if SowRewardData.Instance ~= nil then
        Debuger.LogError("[SowRewardData] attempt to create singleton twice!")
        return
    end
    SowRewardData.Instance = self

    self.act_info = SmartData.New({sowing_count = 0, next_refresh_timestamp = 0, planting_timestamp = {}})
    self.remind_info = SmartData.New({value = 0})
end

function SowRewardData:__delete()
    SowRewardData.Instance = nil
end

function SowRewardData:Config()
    return Config.bozhongyouli_auto
end
function SowRewardData:OtherConfig()
    return self:Config().other[1]
end

function SowRewardData:SetActInfo(protocol)
    self.act_info.sowing_count = protocol.sowing_count
    self.act_info.next_refresh_timestamp = protocol.next_refresh_timestamp
    self.act_info.planting_timestamp = protocol.planting_timestamp
    self:CheckRemind()
end

--可灌溉次数
function SowRewardData:GetWaterTimes()
    return self.act_info.next_refresh_timestamp < TimeHelper.GetServerTime() and 1 or 0
end

--播种次数
function SowRewardData:GetSowTimes()
    return self.act_info.sowing_count
end

function SowRewardData:GetSowTimestamp(index)
    return self.act_info.planting_timestamp[index] or 0
end

--获取灌溉间隔(单位：秒)
function SowRewardData:GetWaterWait()
    return self:OtherConfig().frequency2
end

--获取花朵阶段(单位：秒)
function SowRewardData:GetStageCfg(index)
    return self:OtherConfig()["state" .. index]
end

--获取熟成时间戳
function SowRewardData:GetRipeTimestamp(timestamp)
    return timestamp + self:GetStageCfg(3)
end
--是否成熟
function SowRewardData:IsGrassRipe(timestamp)
    return math.floor(TimeHelper.GetServerTime() - timestamp) >= self:GetStageCfg(3)
end

function SowRewardData:GetNextStageTimestamp(timestamp)
    local spend_time = math.floor(TimeHelper.GetServerTime() - timestamp)
    for i = 1, 3 do
        local state_time = self:GetStageCfg(i)
        if spend_time < state_time then
            return timestamp + state_time
        end
    end
end

function SowRewardData:GetGrassImage(timestamp)
    local spend_time = math.floor(TimeHelper.GetServerTime() - timestamp)
    for i = 1, 3 do
        if spend_time < self:GetStageCfg(i) then
            return i
        end
    end
    return 3
end

--获取展示奖励
function SowRewardData:GetGatherRewardList()
    local base_cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SOW_REWARD, self:Config().reward, nil)[1]
    return DataHelper.FormatItemList(base_cfg.reward_item)
end
function SowRewardData:GetWaterRewardList()
    local base_list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SOW_REWARD, self:Config().reward_rate, "seq")
    local show_list = {}
    for i, v in ipairs(base_list) do
        table.insert(show_list, Item.Create(v.reward_item))
    end
    return show_list
end

--是否可灌溉，-1没次数, 0未种植，1可以，2都已成熟
function SowRewardData:IsCanWater()
    if self:GetWaterTimes() == 0 then
        return -1 
    end
    local is_sow = false
    for i = 0, 3 do
        local timestamp = self:GetSowTimestamp(i)
        if timestamp > 0 then
            is_sow = true
            if not self:IsGrassRipe(timestamp) then
                return 1
            end
        end
    end
    return is_sow and 2 or 0
end
--是否可以播种, -1没次数，0没土地，1可以
function SowRewardData:IsCanSow()
    if self:GetSowTimes() == 0 then
        return -1
    end
    for i = 0, 3 do
        if self:GetSowTimestamp(i) == 0 then
            return 1 
        end
    end
    return 0
end
--是否可收获
function SowRewardData:IsCanGather()
    for i = 0, 3 do
        local timestamp = self:GetSowTimestamp(i)
        if timestamp > 0 and self:IsGrassRipe(timestamp) then
            return 1 
        end
    end
    return 0
end

-----红点
function SowRewardData:CheckRemind()
    self.remind_info.value = self:GetRemindNum()
    MayDayData.Instance:MainBtnState(ACTIVITY_TYPE.RAND_SOW_REWARD, self:GetBtnState())
end

function SowRewardData:GetRemindNum()
    if PeriodActivityData.Instance:GetRemindData(Mod.MayDay.SowReward) then
        return 0
    end
    if self:IsCanGather() == 1 or self:IsCanSow() == 1 or self:IsCanWater() == 1 then
        return 1
    end
    return 0
end

function SowRewardData:GetBtnState()
    --没种植，没次数 才算明天再来
    if self:GetSowTimes() > 0 then
        return MayDayConfig.MainBtnState.Go
    end
    for i = 0, 3 do
        if self:GetSowTimestamp(i) > 0 then
            return MayDayConfig.MainBtnState.Go
        end
    end
    return MayDayConfig.MainBtnState.NextDay
end
