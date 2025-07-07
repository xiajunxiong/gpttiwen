
--[[
时间函数库
常用时间格式
%a abbreviated weekday name (e.g., Wed)
%A full weekday name (e.g., Wednesday)
%b abbreviated month name (e.g., Sep)
%B full month name (e.g., September)
%c date and time (e.g., 09/16/98 23:48:10)
%d day of the month (16) [01-31]
%H hour, using a 24-hour clock (23) [00-23]
%I hour, using a 12-hour clock (11) [01-12]
%M minute (48) [00-59]
%m month (09) [01-12]
%p either "am" or "pm" (pm)
%S second (10) [00-61]
%w weekday (3) [0-6 = Sunday-Saturday]
%x date (e.g., 09/16/98)
%X time (e.g., 23:48:10)
%Y full year (1998)
%y two-digit year (98) [00-99]
%% the character '%'
用法：
os.date("%x",os.time()) --> 07/29/2014
os.date("%Y-%m-%d", os.time()) --> 2014-07-29
]]
--时间管理器
--注意 TimeHelper 是TimeManager的一个实际对象,调用请用TimeHelper
--例 TimeHelper:AddDelayTimer(...) or TimeHelper.GetServerTime()
--一分钟
TIME_MINUTE_SECOND_NUM = 60 
--一小时
TIME_HOUR_SECOND_NUM = 3600 
--一天
TIME_DAY_SECOND_NUM = 86400 
--一周
TIME_WEEK_SECOND_NUM = 604800

TimeManager = TimeManager or BaseClass()

function TimeManager:__init()
	if TimeHelper ~= nil then
		Debuger.LogError("[TimeHelper] attempt to create singleton twice!")
		return
	end	

	self.func_list = {}
	self.update_list = {}
	self.time_scale = 1
	Runner.Instance:AddRunObj(self, 4)
end

function TimeManager:__delete()
	self.func_list = nil

	Runner.Instance:RemoveRunObj(self)
end

function TimeManager.Init()
	TimeHelper = TimeManager.New()
end

function TimeManager:Update()
	local do_list = {}
	local comp_list = {}
	local delete_list = {}
	local count = 0

	for ht, info in pairs(self.func_list) do
		if nil ~= info[4] then	--time event complete
			if nil ~= info[5] then
				if TimeHelper.RealTime() > info[5] then
					table.insert(comp_list, info[4])
					info[3] = 0
				end
			end
			if nil ~= info[6] then
				if TimeHelper.GetServerTime() > info[6] then
					table.insert(comp_list, info[4])
					info[3] = 0
				end
			end
		end
		if info[3] ~= nil then		--time event do
			if TimeHelper.RealTime() > ht.time then
				ht.time = ht.time + info[2]
				table.insert(do_list, info[1])
				if info[3] > 0 then info[3] = info[3] - 1 end
			end
			if 0 == info[3] then
				table.insert(delete_list, ht)
			end
		else		--frame event complete
			if info[5] ~= nil and info[2] - info[5] > 0 then
				info[2] = info[2] - info[5]
				table.insert(do_list, info[1])
			else
				info[2] = info[2] - 1
			end
			if info[2] <= 0 then
				table.insert(comp_list, info[1])
				table.insert(delete_list, ht)
			end
		end
	end


	for key, value in pairs(self.update_list) do
		if nil ~= value[4] then	--time event complete
			if nil ~= value[5] then --total time
				if TimeHelper.RealTime() > value[5] then
					value[3] = 0
					table.insert(comp_list, value[4])
				end
			end
		end
		if value[3] ~= nil then
			if TimeHelper.RealTime() > key.time then
				count = math.floor((TimeHelper.RealTime() - key.time) / value[2] + 0.5)
				for i = 1, count - 1, 1 do
					key.time = key.time + value[2]--time interval
					table.insert(do_list, value[1])--update event
				end
				key.time = key.time + value[2]--time interval
				table.insert(do_list, value[1])--update event
			end
			if 0 == value[3] then
				table.insert(delete_list, key)
			end
		end
	end

	for _, func in pairs(do_list) do
		if type(func) == "function" then
			func()
		elseif type(func) == "table" then
			BuglyReport("timer do_list func is table",func)
		end
	end
	
	for _, func in pairs(comp_list) do
		func()
	end

	for _, ht in pairs(delete_list) do
		self.func_list[ht] = nil
		self.update_list[ht] = nil
	end
end

local function new_ht(interval)
	return { time = TimeHelper.RealTime() + interval}
end


function TimeManager:AddDelayFrameTimer(func,frame_count)
	if frame_count == nil then
		frame_count = 0
	end
	local ht = {frame_count = Time.frameCount + frame_count}
	self.func_list[ht] = {func,frame_count}
	return ht
end

-- DelayTimer (在delay_time之后执行一次func,执行完自动注销)
function TimeManager:AddDelayTimer(func, delay_time)
	local ht = new_ht(delay_time, false)
	self.func_list[ht] = {func, delay_time, 1}
	return ht
end

-- real_times = times + init_call
-- AddRunTimer (每隔interval时间执行一次func,interval 默认为1,times 默认为无限,init_call 为true注册时执行一次func,默认为true)
function TimeManager:AddRunTimer(func, interval, times, init_call)
	interval = interval or 1
	init_call = init_call == nil and true or init_call
	times = times or -1
	local ht = new_ht(interval, false)
	self.func_list[ht] = {func, interval, times}
	if init_call then func() end
	return ht
end
-- 帧计时器
-- 每间隔多少帧执行一次 总帧数（可能有1帧误差不用于做详细计算）
function TimeManager:AddRunFrameTimer(func, interval, times, init_call)
	interval = interval or 1
	times = times or 1
	local ht = {frame_count = Time.frameCount}
	self.func_list[ht] = {func, times, nil, nil, interval}
	if init_call then func() end
	return ht
end
-- AddCountDownTT 和 AddCountDownCT 区别是前者不受服务器时间影响,后者反之
-- last_time do update_func and complete_func
-- AddCountDownTT (每隔interval时间执行一次func,直到达到 total_time 自动注销,interval 默认为1,init_call 为true注册时执行一次update_func,默认为true)
function TimeManager:AddCountDownTT(update_func, complete_func, total_time, interval, init_call)
	if nil == update_func or nil == complete_func or nil == total_time then
		Debuger.LogErrorSD("AddCountDownTT error")
		return
	end
	interval = interval or 1
	init_call = init_call == nil and true or init_call
	local ht = new_ht(interval)
	self.func_list[ht] = {update_func, interval, -1, complete_func, total_time + self.RealTime()}
	if init_call then update_func() end
	return ht
end
function TimeManager:AddCountDownTTA(update_func, complete_func, total_time, interval, init_call)
	if nil == update_func or nil == complete_func or nil == total_time then
		Debuger.LogErrorSD("AddCountDownTT error")
		return
	end
	interval = interval or 1
	init_call = init_call == nil and true or init_call
	local ht = --[[ {time = TimeHelper.RealTime()} ]]new_ht(interval)
	self.update_list[ht] = {update_func, interval, -1, complete_func, total_time + self.RealTime()}
	if init_call then update_func() end
	return ht
end

-- AddCountDownCT (每隔interval时间执行一次func,直到达到complete_time(完成时间戳)自动注销,interval默认为1,init_call为true注册时执行一次update_func,默认为true)
function TimeManager:AddCountDownCT(update_func, complete_func, complete_time, interval, init_call)
	if nil == update_func or nil == complete_func or nil == complete_time then
		Debuger.LogErrorSD("AddCountDownCT error")
		return
	end
	interval = interval or 1
	init_call = init_call == nil and true or init_call
	local ht = new_ht(interval)
	self.func_list[ht] = {update_func, interval, -1, complete_func, nil, complete_time}
	if init_call then update_func() end
	return ht
end

--CancelTimer 传入注册前面4种Timer返回的时间句柄可以手动注销
function TimeManager:CancelTimer(ht)
	if ht == nil then return end
	self.func_list[ht] = nil
	self.update_list[ht] = nil
end

--服务器时间
function TimeManager.GetServerTime()
	return TimeCtrl.Instance:GetServerTime()
end

--游戏运行实际时间
function TimeManager.RealTime()
	return Time.realtimeSinceStartup
end

--时间格式化静态工具(没有合适的可以手动添加) 天/时/分/秒
function TimeManager.FormatDHMS(time)
    local t = {day = 0, hour = 0, min = 0, s = 0}
    if time > 0 then
        t.day = math.floor(time / 86400)
        t.hour = math.floor((time / 3600) % 24)
        t.min = math.floor((time / 60) % 60)
        t.s = math.floor(time % 60)
    end
    return t
end

function TimeManager.Ago(timestamp)
	local remain = TimeHelper.GetServerTime() - timestamp
	local t = TimeHelper.FormatDHMS(remain)
	local ago = Language.Timer.Ago
	if remain < 10 then
		return ago.gg
	elseif remain < TIME_MINUTE_SECOND_NUM then
		return string.format(ago.sec, t.s)
	elseif remain < TIME_HOUR_SECOND_NUM then
		return string.format(ago.min, t.min)
	elseif remain < TIME_DAY_SECOND_NUM then
		return string.format(ago.hour, t.hour)
	elseif remain < TIME_WEEK_SECOND_NUM then
		return string.format(ago.day, t.day)
	else
		return string.format(ago.day, 7)
	end
end

function TimeManager.TimeFormat1(time_t)
	local format1 = Language.Timer.Format1
	if time_t.day > 0 then
		return string.format(format1.day, time_t.day, time_t.hour, time_t.min, time_t.s)
	elseif time_t.hour > 0 then
		return string.format(format1.hour, time_t.hour, time_t.min, time_t.s)
	elseif time_t.min > 0 then
		return string.format(format1.min, time_t.min, time_t.s)
	elseif time_t.s >= 0 then
		return string.format(format1.sec, time_t.s)
	end
	return ""
end

function TimeManager.TimeFormat2(time_t)
	local format2 = Language.Timer.Format2
	if time_t.day > 0 then
		return string.format(format2.day, time_t.day, time_t.hour)
	elseif time_t.hour > 0 then
		return string.format(format2.hour, time_t.hour, time_t.min)
	elseif time_t.min > 0 then
		return string.format(format2.min, time_t.min, time_t.s)
	elseif time_t.s >= 0 then
		return string.format(format2.sec, time_t.s)
	end
	return ""
end

function TimeManager.FormatUnixTime2Date(unixTime)
	if unixTime and unixTime >= 0 then
        local tb = {}
        tb.year = tonumber(os.date("%Y",unixTime))
        tb.month =tonumber(os.date("%m",unixTime))
        tb.day = tonumber(os.date("%d",unixTime))
        tb.hour = tonumber(os.date("%H",unixTime))
        tb.minute = tonumber(os.date("%M",unixTime))
        tb.second = tonumber(os.date("%S",unixTime))
        return tb
    end
end

--t1的第ofd天结束的时间戳
function TimeManager.TimeDayEnd(t1, ofd)
	t1 = t1 or RoleData.Instance:GetBaseData().create_time
	local t0 = TimeManager.FormatUnixTime2Date(t1)
	t0.day = t0.day + ofd
	t0.hour = 0
	t0.minute = 0
	t0.second = 0
	return os.time(t0)
end
--这一天开始的时间戳 0 点
function TimeManager.DayStart()
	local t0 = TimeManager.FormatUnixTime2Date(os.time())
    t0.hour = 0
	t0.minute = 0
    t0.second = 0
   	return os.time(t0)
end

--这一月开始的时间戳 0 点
function TimeManager.MonthStart(time)
	local time = time or os.time()
	local t0 = TimeManager.FormatUnixTime2Date(time)
	t0.day = 1
    t0.hour = 0
	t0.minute = 0
    t0.second = 0
   	return os.time(t0)
end

function TimeManager.DayStartAny(timestamp)
	local t0 = TimeManager.FormatUnixTime2Date(timestamp)
    t0.hour = 0
	t0.minute = 0
    t0.second = 0
   	return os.time(t0)
end

--这一周结束的时间戳
function TimeManager.WeekEndAny(timestamp)
	timestamp = timestamp or TimeCtrl.Instance:GetServerTime()
	local week_num = tonumber(os.date("%w", server_time)) or 0
	week_num = week_num == 0 and 7 or week_num
	local cur_day_timestamp = TimeManager.DayStartAny(timestamp)
	local time = cur_day_timestamp + (8 - week_num) * 86400 - 1
   	return time
end
--这一天的结束时间戳
function TimeManager.DayEnd(timestamp)
	timestamp = timestamp or TimeCtrl.Instance:GetServerTime()
	local t0 = TimeManager.FormatUnixTime2Date(timestamp)
    t0.hour = 24
	t0.minute = 0
    t0.second = 0
   	return os.time(t0)
end

--获取开启时间方便计算 21:00 >> 2100
function TimeManager.OpenTime(time)
    local split = Split(time,[[:]])
    return tonumber(split[1]) * 100 + tonumber(split[2])
end

-- 时间区间检测 如果在区间内返回true(传了tips飘字)
-- start_time、end_time格式 比如2021年10月1日0时0分0秒
-- 	os.time({year = 2021, month = 10, day = 1, hour = 0, minute = 0, sceond = 0})
function TimeManager.IsInTimeInterval(start_time, end_time, tips)
    start_time = start_time or 0
    end_time = end_time or 0
	local server_time = TimeHelper.GetServerTime()
	local value = server_time >= start_time and server_time <= end_time
	if value and nil ~= tips then
		PublicPopupCtrl.Instance:Center(tips)
	end
	return value
end

function TimeManager:TimeScale()
	return self.time_scale
end

function TimeManager:SetTimeScale(ts)
	self.time_scale = ts
end