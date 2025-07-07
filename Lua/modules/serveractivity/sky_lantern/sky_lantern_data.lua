SkyLanternData = SkyLanternData or BaseClass()
function SkyLanternData:__init()
    if SkyLanternData.Instance ~= nil then
        Debuger.LogError("[SkyLanternData] attempt to create singleton twice!")
        return
    end
    SkyLanternData.Instance = self

    self.act_info = SmartData.New({huozhong = 0, task_list = {}, complete_flag = {}})
    self.remind_info = SmartData.New({value = 0})
end

function SkyLanternData:__delete()
    SkyLanternData.Instance = nil
end

function SkyLanternData:Config()
    return Config.qiyuankongmingdeng_auto
end
function SkyLanternData:OtherConfig()
    return self:Config().other[1]
end

function SkyLanternData:SetActInfo(protocol)
    self.act_info.huozhong = protocol.huozhong
    self.act_info.complete_flag = protocol.complete_flag
    self.act_info.task_list = protocol.task_list
    self:CheckRemind()
end

-----任务信息
function SkyLanternData:GetTaskList()
    local show_list = {}
    local act_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_SKY_LANTERN)
    for i, v in pairs(self:Config().task) do
        if v.section == act_day then
            table.insert(show_list, v)
        end
    end
    table.sort(show_list, DataHelper.WeightSort("seq", function(data)
        if self:IsTaskOver(data.seq) then
            return 1000
        end
    end))
    return show_list
end

function SkyLanternData:IsTaskOver(seq)
    return self.act_info.complete_flag[seq] == 1
end
function SkyLanternData:GetTaskProg(seq)
    return self.act_info.task_list[seq] or 0
end

-----奖励配置
function SkyLanternData:GetRewardList()
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_SKY_LANTERN, self:Config().reward, nil)[1]
end

-----时间信息
function SkyLanternData:GetTimeData(time)
    -- return ActivityData.Instance:GetTimeData(time)
    local hour = math.floor(time / 100)
    return {hour = hour, min = time - hour * 100}
end

--获取开启时间
function SkyLanternData:GetFlyTime()
    return self:GetTimeData(self:OtherConfig().time_start), self:GetTimeData(self:OtherConfig().time_end)
end
--是否开启时间段
function SkyLanternData:IsFlyTime()
    if self:OtherConfig().time_start <= ActivityData.Instance:GetCurrTime() and ActivityData.Instance:GetCurrTime() < self:OtherConfig().time_end then
        return true
    end
    return false
end
--获取下次开启时间戳
function SkyLanternData:GetNextStartTime()
    local fly_time1, fly_time2 = self:GetFlyTime()
    local time_t = os.date("*t", TimeHelper.GetServerTime())
    time_t.hour = fly_time1.hour
    time_t.min = fly_time1.sec
    time_t.sec = 0
    if self:OtherConfig().time_end <= ActivityData.Instance:GetCurrTime() then
        time_t.day = time_t.day + 1
    end
    return os.time(time_t)
end
--获取本次结束时间戳
function SkyLanternData:GetOverTime()
    local fly_time1, fly_time2 = self:GetFlyTime()
    local time_t = os.date("*t", TimeHelper.GetServerTime())
    time_t.hour = fly_time2.hour
    time_t.min = fly_time2.sec
    time_t.sec = 0
    return os.time(time_t)
end

--物品数量和消耗数量
function SkyLanternData:GetItemNum()
    return self.act_info.huozhong
end
function SkyLanternData:GetConcuseNum()
    return self:OtherConfig().reward_consume
end

-----红点
function SkyLanternData:CheckRemind()
    self.remind_info.value = self:GetRemindNum()
    MayDayData.Instance:MainBtnState(ACTIVITY_TYPE.RAND_SKY_LANTERN, self:GetBtnState())
end

function SkyLanternData:GetRemindNum()
    if PeriodActivityData.Instance:GetRemindData(Mod.MayDay.SkyLantern) then
        return 0
    end
    for i, v in ipairs(self:GetTaskList()) do
        if not self:IsTaskOver(v.seq) then
            return 1
        end
    end
    if ActivityData.Instance:IsHasSign(ACTIVITY_TYPE.RAND_SKY_LANTERN) then
        return 1
    end
    if self:IsFlyTime() then
        return self:GetItemNum() >= self:GetConcuseNum() and 1 or 0
    end
    return 0
end

function SkyLanternData:GetBtnState()
    --活动最后一天，已完结
    if self:IsActOver() then
        return MayDayConfig.MainBtnState.ActEnd
    end
    --任务做完了且过了点燃时间->明日再来
    for i, v in ipairs(self:GetTaskList()) do
        if not self:IsTaskOver(v.seq) then
            return MayDayConfig.MainBtnState.Go
        end
    end
    if self:GetItemNum() < self:GetConcuseNum() then
        return MayDayConfig.MainBtnState.NextDay
    end
    --过了点燃时间
    if self:OtherConfig().time_end <= ActivityData.Instance:GetCurrTime() then
        return MayDayConfig.MainBtnState.NextDay
    end
    return MayDayConfig.MainBtnState.Go
end

function SkyLanternData:IsActOver()
    local next_time = self:GetNextStartTime()
    return next_time >= ActivityData.GetSwitchStampTime(ACTIVITY_TYPE.RAND_SKY_LANTERN)
end

-----奖励展示
function SkyLanternData:SaveNeedDelayItem(item)
    table.insert(self.need_delay_items, item)
end
function SkyLanternData:LoadNeedDelayItems()
    return self.need_delay_items
end
function SkyLanternData:ClearNeedDelayItems()
    self.need_delay_items = {}
end
