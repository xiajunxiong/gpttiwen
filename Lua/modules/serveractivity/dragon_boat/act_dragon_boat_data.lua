ActDragonBoatData = ActDragonBoatData or BaseClass()

function ActDragonBoatData:__init()
    if ActDragonBoatData.Instance ~= nil then
        Debuger.LogError("[ActDragonBoatData] attempt to create singleton twice!")
        return
    end
    ActDragonBoatData.Instance = self

    self.cfg = Config.activity_main_auto.duanwu_festival

    -- 红点刷新
    self.remind_data = SmartData.New({})
end

function ActDragonBoatData:ClearData()

end

function ActDragonBoatData:IsOpen()
    local s_time,e_time = ActivityRandData.GetBaseDayOpenStampTime(self:ActId())
    local cur_time = TimeCtrl.Instance:GetServerTime()
    if cur_time >= s_time and cur_time <= e_time then
        return true
    end
    return false
end

-- 总活动Id --写死，此活动会贯穿整个活动
function ActDragonBoatData:ActId()
    return ACTIVITY_TYPE.DRAGON_COLLECT
end

-- 总活动结束时间
function ActDragonBoatData:EndTime()
    local s_time,e_time = ActivityRandData.GetBaseDayOpenStampTime(self:ActId())
    return e_time
end

-- 总活动开启时间
function ActDragonBoatData:StartTime()
    local s_time,e_time = ActivityRandData.GetBaseDayOpenStampTime(self:ActId())
    return s_time
end

-- 总活动开启了第几天
function ActDragonBoatData:OpenDay()
    if not self:IsOpen() then
        return 0
    end
    local cur_time = TimeCtrl.Instance:GetServerTime()
    local start_time = self:StartTime()
    local s_time_info = TimeManager.FormatUnixTime2Date(start_time)
    local c_time_info = TimeManager.FormatUnixTime2Date(cur_time)
    return c_time_info.day - s_time_info.day + 1
end

function ActDragonBoatData:GetOpenActList()
    local day = self:OpenDay()
    local cfg_group = DataHelper.ConvertGroup(self.cfg, "location")
    local list = {}
    for i,v in pairs(cfg_group) do
        for j,v2 in pairs(v) do
            if not list[i] and day <= v2.end_time then
                list[i] = v2
            end
        end
    end
    for i = 0, 6 do
        if not list[i] then
            local v = cfg_group[i]
            list[i] = v[#v]
        end
    end
    return list
end

function ActDragonBoatData:GetOpenAct(pos_index)
    local list = self:GetOpenActList()
    return list[pos_index]
end