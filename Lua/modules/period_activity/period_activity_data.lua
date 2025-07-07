PeriodActivityData = PeriodActivityData or BaseClass()

PERIOD_ACTIVITY_WAIT_DAY_NUM = 3 --剩余兑换时间
PERIOD_ACTIVITY_SPECIAL_TYPE = 5 --特殊玩法处理

function PeriodActivityData:__init()
    if PeriodActivityData.Instance ~= nil then
        Debuger.LogError("[PeriodActivityData] attempt to create singleton twice!")
        return
    end
    PeriodActivityData.Instance = self
    -- period_type为0就是不开
    self.info_data = SmartData.New({period_type = 0,begin_time = 0,end_time = 0})
    self.activity_create_list = {}
    self.activity_delete_list = {}
    self.activity_open_list = {}
    self.activity_times_list = {}
    self.activity_help_list = {}
    self.remind_list = {}
    self.remind_data = SmartData.New({val = false})
end

function PeriodActivityData:ClearData()
    self.remind_list = {}
end

function PeriodActivityData:SetPeriodActivityInfo(protocol)
    self.info_data.period_type = protocol.period_type
    self.info_data.begin_time = protocol.begin_time
    self.info_data.end_time = protocol.end_time
end

--获取结束时间戳
function PeriodActivityData:GetActivityEndTime()
    return self.info_data.end_time
end

--获取时间戳
function PeriodActivityData:GetActivityTime()
    return self.info_data.begin_time,self:GetActivityEndTime()
end

--获取当前周期类型开启的玩法
function PeriodActivityData:GetItemList()
    local list = {}
    for i,v in ipairs(Config.period_activity_auto.play_list) do
        if v.period_type == self:GetPeriodType() then
            if v.show_type == PERIOD_ACTIVITY_SPECIAL_TYPE then 
                if self:CheckSpecial(v)then
                    table.insert(list,v)
                end
            else
                table.insert(list,v)
            end
        end
    end
    table.sort(list,DataHelper.SortFunc([[sort]]))
    return list
end

function PeriodActivityData:CheckSpecial(data)
    return self:GetDataDayNum()%2 == data.mod%2
end

--玩法倒计时 时间戳 是否是玩法时间内 
function PeriodActivityData:GetActivityStampTime()
    local exchange_time = PERIOD_ACTIVITY_WAIT_DAY_NUM * TIME_DAY_SECOND_NUM
    if self:GetActivityEndTime() - TimeHelper.GetServerTime() > exchange_time then
        return self:GetActivityEndTime() - exchange_time,true
    end
    return self:GetActivityEndTime(),false
end

--主界面图标开启
function PeriodActivityData:GetPeriodActivityEnd()
    if not FunOpen.Instance:IsFunOpen(Mod.PeriodActivity.Main) then
        return false
    end
    if not self:IsOpenConfig() then
        return false
    end
    return self.info_data.period_type > 0 and self:GetActivityEndTime() ~= 0 and TimeHelper:GetServerTime() <= self:GetActivityEndTime()
end

--是否在配置开启范围之内
function PeriodActivityData:IsOpenConfig()
    local sever_time = TimeHelper:GetServerTime()
    local curr_time = TimeCtrl.Instance:GetCrossOpenServerTime()
    for i,v in ipairs(DataHelper.ReverseList(Config.period_activity_auto.time_level)) do
        if TimeManager:GetServerTime() >= v.time_stamp and RoleData.Instance:GetRoleLevel() >= v.level and sever_time >= (curr_time + (v.start_time-1)*TIME_DAY_SECOND_NUM) then
            return true
        else
            return false
        end
    end
    return false
end

function PeriodActivityData:OnCreateHandle()
	for mod,func in pairs(self.activity_create_list) do
        local is_open = self:IsOpen(mod)
        if not self.activity_open_list[mod] and is_open == true then
            self.activity_open_list[mod] = is_open
            Call(func)
        end
        if self.activity_open_list[mod] == true and is_open == false then
            if self.activity_delete_list[mod] then
                Call(self.activity_delete_list[mod])
            end
            self.activity_open_list[mod] = nil
        end
    end
end

function PeriodActivityData:OnDeleteHandle()
    for mod,func in pairs(self.activity_delete_list) do
        self.activity_open_list[mod] = nil
        Call(func)
    end
end

function PeriodActivityData:GetTimesHandle(mod)
	return self.activity_times_list[mod]
end
function PeriodActivityData:GetHelpHandle(mod)
	return self.activity_help_list[mod]
end

function PeriodActivityData:SetRemindData(mod)
    self.remind_list[mod] = true
    self.remind_data.val = not self.remind_data.val
end

function PeriodActivityData:GetRemindData(mod)
    return self.remind_list[mod] --暂时屏蔽
end
--=======================外部接口===============================

--根据mod_key 获取单个玩法配置
function PeriodActivityData:GetConfig(mod)
    for k,v in pairs(Config.period_activity_auto.play_list) do
        if v.mod == mod then
            return v
        end
    end
end

--获取当前周期类型
function PeriodActivityData:GetPeriodType()
    return self.info_data.period_type
end

--获取周期活动到第几天
function PeriodActivityData:GetDataDayNum()
    return TimeCtrl.Instance:GetDataDayNum(TimeHelper:GetServerTime(),self:GetActivityTime())
end

--判断某个玩法是否开启 mod_key
function PeriodActivityData:IsOpen(mod)
    if not self:GetPeriodActivityEnd() then return false end    --周期活动开启是否满足（等级 跨服天数 剩余时间）
    if not FunOpen.Instance:IsFunOpen(mod) then return false end--该玩法等级是否满足 默认为true
    local stamp_time,is_show = self:GetActivityStampTime()      --该玩法的剩余时间 是否满足
    local data = tonumber(mod) and self:GetConfig(mod) or mod
    if type(data) == "number" then return false end
    is_show = data.show_type == 7 and true or is_show
    if not is_show then return false end
    if data.period_type ~= self:GetPeriodType() then return false end --当前类型是否一致
    if data.show_type == PERIOD_ACTIVITY_SPECIAL_TYPE then
        if not self:CheckSpecial(data)then return false end --三仙归洞
    end
    local curr_time,day_num = ActivityData.Instance:GetCurrTime(),self:GetDataDayNum()
    if not IsRange(day_num,data.open_opint,data.open_days) then return false end --是否在开启的天数范围内
    return IsRange(curr_time,data.start_time,data.end_time) or IsRange(curr_time,data.begin_time1,data.end_time1) --是否在开启时间范围内
end

-- 活动玩法开启创建监听
function PeriodActivityData:CustomCreateHandle(mod, func)
	self.activity_create_list[mod] = func
end
-- 活动玩法结束销毁监听
function PeriodActivityData:CustomDeleteHandle(mod, func)
	self.activity_delete_list[mod] = func
end
-- 活动玩法剩余次数和总次数注册
function PeriodActivityData:CustomTimesHandle(mod, func)
	self.activity_times_list[mod] = func
end
-- 活动玩法说明
function PeriodActivityData:CustomHelpHandle(mod, func)
    self.activity_help_list[mod] = func
end