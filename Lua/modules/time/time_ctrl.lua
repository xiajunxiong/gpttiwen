
FILE_REQ("modules/time/timer")

TimeCtrl = TimeCtrl or BaseClass(BaseCtrl)

function TimeCtrl:__init()
	if TimeCtrl.Instance ~= nil then
		Debuger.LogError("[TimeCtrl] attempt to create singleton twice!")
		return
	end	
	TimeCtrl.Instance = self

	TimeManager.Init()

	self:RegisterAllProtocols()

	self.server_time = 0		--前期会出现下发0的情况,所以在上线自动下发协议时相对时间记录建议使用RealTime
	self.last_req_time = 0

	self.delay_time = SmartData.New({time = 10})		--网络延迟时间
	self.send_time = 0 			--记录发送获取服务器时间协议的当前时间
	self.send_count = 0			--已发送获取服务器时间未返回次数
	self.check_delay_count = 0
	self.server_data = SmartData.New({server_open_day = 0})
end

function TimeCtrl:__delete()
	if TimeCtrl.Instance == self then
		TimeCtrl.Instance = nil
	end

	self:CancelTimer()
	self:UnRegisterAllProtocols()
end

function TimeCtrl:OnInit()
	self.handle_le = LoginData.Instance:BaseData():KeysCare({"login_state"}, BindTool.Bind(TimeCtrl.Instance.OnLoginEvent, TimeCtrl.Instance))
end

function TimeCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSTimeReq)
	self:RegisterProtocol(SCTimeAck, "OnTimeAck")
	self:RegisterProtocol(SCWorldStatusHiddenInfo, "OnWorldStatusHiddenInfo") 
end

function TimeCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCTimeAck)
end

function TimeCtrl:SendTimeReq()
	self.send_time = TimeHelper.RealTime()
	self.send_count = self.send_count + 1
	self.check_delay_count = self.check_delay_count + 1
	if NetworkCtrl.Instance:LogicServerIsConnected() then
		local protocol = GetProtocol(CSTimeReq)
		SendProtocol(protocol)
	end
end

function TimeCtrl:OnTimeAck(protocol)
	if protocol.is_server_driving_send == 0 then
		self.send_count = math.max(self.send_count - 1, 0)
		self:SetDelayTime()
	end
	self.server_time = protocol.server_time
	self.server_real_start_time = protocol.server_real_start_time
	self.server_real_combine_time = protocol.server_real_combine_time
	self.server_six_start_time  = protocol.server_six_start_time
	self.last_req_time = TimeHelper.RealTime()
	if self.cur_day ~= protocol.open_days then
		self.cur_day = protocol.open_days
		self.server_data.server_open_day = protocol.open_days
		EventSys:Fire(GlobalEventKeys.ServerOpenDayChange)
	end
end

function TimeCtrl:OnWorldStatusHiddenInfo(protocol)
	self.cross_open_server_time = protocol.cross_open_server_time
end

--角色登录时服务器下发一次的当前服务器时间及年月日时分秒
-- function TimeCtrl:OnTimeDateInfo(protocol)
-- 	self.server_time_data = protocol
-- end

function TimeCtrl:GetServerTime()
	return self.server_time + (TimeHelper.RealTime() - self.last_req_time)
end

function TimeCtrl:GetServerDate()
	local tb = {}
    tb.year = tonumber(os.date("%Y",self.server_time))
    tb.month =tonumber(os.date("%m",self.server_time))
    tb.day = tonumber(os.date("%d",self.server_time))
    tb.hour = tonumber(os.date("%H",self.server_time))
    tb.min = tonumber(os.date("%M",self.server_time))
    tb.second = tonumber(os.date("%S",self.server_time))
    return tb
end

function TimeCtrl:IsServerTimeReady()
	return self.server_time ~= 0;
end

function TimeCtrl:OnLoginEvent()
	if LoginData.Instance:LoginState() == LoginStateType.Logined and not self.timer_rt then
		self:SendTimeReq()
		self.timer_rt = TimeHelper:AddRunTimer(BindTool.Bind(self.SendTimeReq, self), 10)
		LoginData.Instance:BaseData():Uncare(self.handle_le)
	end
end

function TimeCtrl:CancelTimer()
	TimeHelper:CancelTimer(self.timer_rt)
	self.timer_rt = nil
end

-- 设置网络延迟（10秒检查一次）
function TimeCtrl:SetDelayTime()
	if 0 == self.send_count then
		self.delay_time.time = TimeHelper.RealTime() - self.send_time
	elseif self.send_count > 0 and self.check_delay_count >= 30 then --5分钟青一次
	 	self.send_count = 0
	 	self.check_delay_count = 0
	end
end

function TimeCtrl:GetDelayTime()
	return self.delay_time.time
end
function TimeCtrl:ClearDelayTime()
	self.send_count = 0
	self.check_delay_count = 0
	self.delay_time.time = 0
	self.send_time = TimeHelper.RealTime()
end

function TimeCtrl:ServerStartTS()
	return self.server_real_start_time
end

--获取当前服务器当前开启天数
function TimeCtrl:GetCurOpenServerDay()
	return self.cur_day
end

--获取当前跨服到了第几天
function TimeCtrl:GetCrossOpenServerDay()
	return self:GetDataDayNum(TimeHelper:GetServerTime(),self.cross_open_server_time)
end

function TimeCtrl:GetCrossOpenServerTime()
	return self.cross_open_server_time or 0
end

--废弃 用ChangDataTime 转换为日期传入时间的当天晚上24点
function TimeCtrl:ChangDataTime24(time_stamp)
	local t = os.date("*t",time_stamp)
	return os.time{year=t.year,month=t.month,day=t.day+1,hour=0,min=0,sec=0} or 0
end

--废弃 直接用os.date("*t",time_stamp)
function TimeCtrl:ChangDataTimeDetail(time_stamp)
	local t = os.date("*t",time_stamp)
	return {year=t.year,month=t.month,day=t.day,hour=t.hour,min=t.min,sec=t.sec} 
end

--废弃 建议使用GetActOpenDay
function TimeCtrl:ActOpenDay(act_type)
	local time = self:GetServerTime() - ActivityData.Instance:GetActivityStatusInfo(act_type).param_1
	return math.ceil(time / TIME_DAY_SECOND_NUM)
end

--=======================================================================================================================================
--@G根据传入的时间表转换为 当前日期0点时间戳 t = {year,month,day}
function TimeCtrl:ChangDataStampTime(t)
	return os.time{year=t.year,month=t.month,day=t.day,hour=0,min=0,sec=0} or 0
end

--@G根据传入的时间戳转换为 day天后时间戳 默认不传day为 当前日期0点时间戳
function TimeCtrl:ChangDataTime(time_stamp,day)
	local t = os.date("*t",time_stamp)
	return os.time{year=t.year,month=t.month,day=t.day + (day or 0),hour=0,min=0,sec=0} or 0
end

--@G获取两个时间戳的天数
function TimeCtrl:GetDataDayNum(time_stamp,start_time)
	local day_num = math.floor((self:ChangDataTime(time_stamp) - self:ChangDataTime(start_time)) / TIME_DAY_SECOND_NUM) + 1
	return day_num > 0 and day_num or 1
end

--@G获取当前活动对应的服务器开启天数
function TimeCtrl:GetActOpenServerDay(act_type)
	return self:GetDataDayNum(ActivityData.GetStartStampTime(act_type),self.server_real_start_time)
end

--@G获取当前活动对应的服务器开启天数·六点开服
function TimeCtrl:GetActOpenServerDaySixStart(act_type)
	return self:GetDataDayNum(ActivityData.GetStartStampTime(act_type),self.server_six_start_time)
end

--@G当前活动开启到了第几天
function TimeCtrl:GetActOpenDay(act_type)
	return self:GetDataDayNum(self:GetServerTime(),ActivityData.GetStartStampTime(act_type))
end
--=======================================================================================================================================