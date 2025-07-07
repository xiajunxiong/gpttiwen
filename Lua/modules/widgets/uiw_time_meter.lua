UIWTimeMeter = UIWTimeMeter or DeclareMono("UIWTimeMeter")

----LblTime========Text 传入需要显示倒计时的文本

----StampTime------创建时间 传入时间戳类型   服务端时间戳倒计时 支持刷新回调和结束回调
----TotalTime------创建时间 传入时间长度类型 从多少秒开始倒计时 支持刷新回调和结束回调
----CreateRunTime--创建累加时间 计时器       从0开始计时       支持刷新回调和结束回调
----CreateActTime--创建活动时间 传入活动类型 活动剩余时间倒计时 支持刷新回调和结束回调
----SetCallBack----设置结束回调 和 刷新回调（可不传）
----SetTime--------外部设置文字
----CloseCountDownTime 关闭计时器
----SetData--------传入一个param_t创建时间
----SetShowCallBack 自定义处理时间格式的函数 回调 t 时间表

----NoneZero===Bool 可以为空，默认加零处理 true为不加零
----DayZero=== Bool 可以为空，天数加零处理
----ShowType===int  可以为空，默认无时间单位  00:00:00

--时间类型
TimeType = {
	Type_Time_0 = 0,  --无时间单位  00:00:00
	Type_Time_1 = 1,  --基本时间单位 时分秒
	Type_Time_2 = 2,  --天时分秒 时分秒
	Type_Time_3 = 3,  --天时分
	Type_Time_4 = 4,  --基本日期单位 年月日
	Type_Time_5 = 5,  --天时分秒
	Type_Time_6 = 6,  --天时
	Type_Time_7 = 7,  --天时分 时分秒 00:00:00
	Type_Time_8 = 8,  --天时分 时分秒
	Type_Special_0 = 10, --特殊单位 00:00 分秒
	Type_Special_1 = 11, --特殊单位 00:00 时分
	Type_Special_2 = 12, --特殊单位 00 秒
	Type_Special_3 = 13, --特殊单位 天 00:00:00
	Type_Special_4 = 14, --特殊单位 天时 -> 时分
	Type_Special_5 = 15, --特殊单位 秒
	Type_Special_6 = 16, --特殊单位 分 秒
	Type_Special_7 = 17, --竞技场用
	Type_Special_8 = 18, --大于1小时用 时分，不足一小时用分秒
	Type_Special_9 = 19, --特殊单位 天时 -> 时分 -> 分秒
	Type_Special_10 = 20, --特殊单位 小时 分钟
	Type_Special_11 = 21, --特殊单位 00:00 大于1小时用 时分秒，不足一小时用分秒
	Type_Special_12 = 22, --特殊单位 文字 大于1小时用 时分秒，不足一小时用分秒
}

function UIWTimeMeter:UIWTimeMeter()
	if self.IsRunTime then return end
	self.text = nil     --需要显示的文字
	self.realtime = 0   --当前执行的时间
	self.remain_time = 0--当前剩余的时间
	self.time_func = nil--时间回调
	self.end_text = nil --结束状态显示文字
	self.update_fun = nil--计时回调
	self.complete_fun = nil--结束回调
	self.show_type = self.ShowType or TimeType.Type_Time_0 --时间显示类型
	self.none_zero = nil
	self.close_stamp_time = 0 --关闭时的时间戳OnDisable
	self.start_time = nil --计时开始时间
	self.end_time = nil   --计时关闭时间
	self.init_call = nil
	self.remain_event = nil
	self.remain_event_time = nil
end

--一般可关闭页面勾选OnDestroy
function UIWTimeMeter:OnDestroy()
	self:CloseCountDownTime()
	self:UIWTimeMeter()
end

--需要后台关闭还需恢复勾选OnDisable
function UIWTimeMeter:OnDisable()
	self:CloseCountDownTime()
	self.close_stamp_time = TimeHelper.GetServerTime()
end

--需要后台关闭还需恢复勾选OnEnable
function UIWTimeMeter:OnEnable()
	local exit_time = TimeHelper.GetServerTime() - self.close_stamp_time
	if self.start_time == nil then
		self.realtime = 0
		self:CreateTime(self.remain_time - exit_time)
	else
		self.start_time = self.start_time + exit_time
		if self.end_time and self.start_time >= self.end_time then
			self:CompleteCallBack()
		else
			self:CreateRunTime(self.start_time,self.end_time,self.show_type,self.text)
		end
	end
end

--[[param_t = {
	stamp_time 时间戳
	total_time 时间长度（秒）
	act_type   活动类型
	show_type  显示类型
	show_func  自定义处理时间格式的函数 回调 t 时间表
	text       需要插入到 某个文字的%s中替换
	end_text   倒计时结束的时候需要传
	none_zero = false ，取消序列字段NoreZero逻辑
}]]
--注意：传stamp_time、total_time、act_type 只能存在一个
--目前仅支持 StampTime TotalTime CreateActTime
function UIWTimeMeter:SetData(param_t)
	param_t = param_t or {}
	if not next(param_t) then
		self:CloseCountDownTime()
		return
	end
	local show_type = param_t.show_type

	local text = param_t.text

	local end_text = param_t.end_text
	
	if param_t.show_func ~= nil then
		self.show_func = param_t.show_func
	end
	if param_t.none_zero ~= nil then
		self.none_zero = param_t.none_zero
	end
	if param_t.stamp_time then
		self:StampTime(param_t.stamp_time,show_type,text,end_text)
	elseif param_t.total_time then
		self:TotalTime(param_t.total_time,show_type,text,end_text)
	else
		self:CreateActTime(param_t.act_type,show_type,text,end_text)
	end
end

--时间戳类型 倒计时
--[[
    @desc: 时间戳类型 倒计时
    author:{author}
    time:2019-05-29 20:04:20
    --@stamp_time:时间戳
	--@show_type:时间显示类型 可不传
	--@text:需要传递的文字 支持%s 可不传
	--@end_text:结束状态需要显示的文字 可不传
    @return:
]]
function UIWTimeMeter:StampTime(stamp_time,show_type,text,end_text)
	if text ~= nil then
		self.text = text
	end
	if show_type ~= nil then
		self.show_type = show_type
	end
	if end_text ~= nil then
		self.end_text = end_text
	end
	if stamp_time ~= nil then
		local total_time = stamp_time - TimeHelper.GetServerTime()
		self:CreateTime(total_time)
	end
end

--非时间戳类型 时间长度 倒计时
--[[
    @desc: 非时间戳类型 时间长度 倒计时
    author:{author}
    time:2019-05-29 20:04:20
    --@total_time:时间长度
	--@show_type:时间显示类型 可不传
	--@text:需要传递的文字 支持%s 可不传
	--@end_text:结束状态需要显示的文字 可不传
    @return:
]]
function UIWTimeMeter:TotalTime(total_time,show_type,text,end_text)
	if text ~= nil then
		self.text = text
	end
	if show_type ~= nil then
		self.show_type = show_type
	end
	if end_text ~= nil then
		self.end_text = end_text
	end
	self:CreateTime(total_time)
end

--累加时间的计时器
--[[
    @desc: 累加时间的计时器
    author:{author}
    time:2019-06-01 09:51:15
    --@start_time:开始时间 默认为0    可以为时间戳
	--@end_time:  结束时间 不传为无限 可以为时间戳
	--@show_type:时间显示类型 可不传
	--@text:需要传递的文字 支持%s 可不传
    @return:
]]
function UIWTimeMeter:CreateRunTime(start_time,end_time,show_type,text)
	if text ~= nil then
		self.text = text
	end
	self.show_type = show_type or TimeType.Type_Special_0

	self.start_time = start_time or 0

	self.end_time = end_time

	self:CloseCountDownTime()
	self.time_func = TimeHelper:AddRunTimer(function()
		self:SetTimeTransform(self.start_time)
		if self.update_fun ~= nil then
			self.update_fun(self.start_time)
		end
		if self.end_time and self.start_time >= self.end_time then
			self:CompleteCallBack()
		end
		self.start_time = self.start_time + 1
	end,1,self.end_time)

	self.IsRunTime = true
end

function UIWTimeMeter:CreateRunStampTime(start_time,end_time,show_type,text)
	if start_time ~= nil then
		start_time = self:ChangeStampTime(start_time)
	end
	if end_time ~= nil then
		end_time = self:ChangeStampTime(end_time)
	end
	self:CreateRunTime(start_time,end_time,show_type,text)
end

function UIWTimeMeter:ChangeStampTime(time)
	if time - TimeHelper.GetServerTime() < 0 then
		return TimeHelper.GetServerTime() - time
	else
		return time - TimeHelper.GetServerTime()
	end
end

--创建活动时间倒计时
--[[
    @desc: 创建活动时间倒计时
    author:{author}
    time:2019-05-29 20:04:20
    --@act_type:活动act_type
	--@show_type:时间显示类型 可不传
	--@text:需要传递的文字 支持%s 可不传
	--@end_text:结束状态需要显示的文字 可不传
    @return:
]]
function UIWTimeMeter:CreateActTime(act_type,show_type,text,end_text)
	if act_type == nil then
		return
	end

	if text ~= nil then
		self.text = text
	end
	
	self.show_type = show_type or TimeType.Type_Time_2

	self.end_text = end_text or Language.Activity.NotOpenTip

	local activity_info = ActivityData.Instance:GetActivityStatusInfo(act_type)
	if activity_info ~= nil and activity_info.status ~= ActStatusType.Close then
		local total_time = activity_info.next_status_switch_time - TimeHelper.GetServerTime()
		self:CreateTime(total_time)
	else
		self:SetTime(Language.Activity.NotOpenTip)
	end
end

--创建倒计时，外部不可调用
function UIWTimeMeter:CreateTime(total_time)
	self:CloseCountDownTime()
	if total_time ~= nil then 
		self.time_func = TimeHelper:AddCountDownTT(
		function()
			self:SetTimeTransform(total_time - self.realtime)
			if self.update_fun ~= nil then
				self.update_fun(self.realtime, total_time)
			end
			self.realtime = self.realtime + 1
		end,
		BindTool.Bind(self.CompleteCallBack,self),total_time,1,self.init_call)
	end
end
local function t_add(time)
	return time < 10 and tostring("0"..time) or time
end

--设置节点时间 外部不可调用
function UIWTimeMeter:SetTimeTransform(time)
	--加0处理
	local function T(t)
		t.s = t_add(t.s)
		t.min = t_add(t.min)
		t.hour = t_add(t.hour)
		if self.DayZero == true then
			t.day = t_add(t.day)
		end
		return t
	end
	if self.unityMono == nil or self.unityMono:Equals(nil) then
		return
	end
	time = time <= 0 and 0 or time
	self.remain_time = time
	if self.none_zero == false then
		self.NoneZero = nil
	end
	local t = self.NoneZero == nil and T(TimeHelper.FormatDHMS(time)) or TimeHelper.FormatDHMS(time)

	local t_str = self.show_func and self.show_func(t) or self:GetFromatTime(t,time)

	t_str = t_str or ""
	--需要替换%s的文字
	if self.text ~= nil then
		if string.find(self.text,"%%s") or string.find(self.text,"%%d") then
			t_str = string.format(self.text,t_str)
		else
			t_str = self.text .. t_str
		end
	end

	--设置时间
	if self.LblTime ~= nil and self.LblTime.text then
		UH.SetText(self.LblTime,t_str)
	end

	--结束状态的文字显示
	if math.floor(time) == 0 and self.end_text then
		UH.SetText(self.LblTime,self.end_text)
	end

	--倒计时多少秒需要的回调
	if self.remain_event_time and self.remain_event then
		if self.remain_event_time == math.floor(time)then
			self.remain_event()
			self.remain_event = nil
			self.remain_event_time = nil
		end
	end
end

function UIWTimeMeter:CompleteCallBack()
	if self.complete_fun then
		self.complete_fun(self)
	end
end

--====================================控件外部方法====================================
--销毁或者停止计时器
function UIWTimeMeter:CloseCountDownTime()
	if self.time_func then
		TimeHelper:CancelTimer(self.time_func)
		self.time_func = nil
		self.realtime = 0
	end
end

--设置回调
function UIWTimeMeter:SetCallBack(complete_fun,update_fun)
	if complete_fun then
		self.complete_fun = complete_fun
	end
	if update_fun then
		self.update_fun = update_fun
	end
end

-- 自定义时间显示格式
function UIWTimeMeter:SetShowCallBack(show_func)
	if show_func then
		self.show_func = show_func
	end
end

--剩余多少秒的需要的回调
function UIWTimeMeter:SetRemainEvent(event,time)
	if event then
		self.remain_event = event
	end
	if time then
		self.remain_event_time = time
	end
end

--获取剩余时间
function UIWTimeMeter:GetRemainTime()
	return self.remain_time
end

--设置显示的文字
function UIWTimeMeter:SetTimeText(text)
	self.text = text
end

--设置时间显示类型
function UIWTimeMeter:SetShowType(show_type)
	self.show_type = show_type
end

--设置结束状态的文字显示
function UIWTimeMeter:SetEndText(text)
	self.end_text = text
end

--外部设置Text
function UIWTimeMeter:SetTime(text)
	self:CloseCountDownTime()
	UH.SetText(self.LblTime,text or "")
end

--设置init_call
function UIWTimeMeter:SetInitCall(flag)
	self.init_call = flag
end

function UIWTimeMeter:SetCloseStampTime(stamp_time)
	self.close_stamp_time = stamp_time
end

--公用时间格式处理
function UIWTimeMeter:GetFromatTime(t,time)
	local t_str = ""
	if self.show_type == TimeType.Type_Time_0 then
		t_str = string.format(Language.UiTimeMeter.TimeStr, t.hour, t.min ,t.s)
	elseif self.show_type == TimeType.Type_Time_1 then
		t_str = string.format(Language.UiTimeMeter.TimeStr1, t.hour, t.min ,t.s)
	elseif self.show_type == TimeType.Type_Time_2 then
		if t.day > 0 then
			t_str = string.format(Language.UiTimeMeter.TimeStr2,t.day, t.hour, t.min,t.s)
		else
			t_str = string.format(Language.UiTimeMeter.TimeStr1, t.hour, t.min ,t.s)
		end
	elseif self.show_type == TimeType.Type_Time_3 then
		if tonumber(t.day) < 1 then
			t_str = string.format(Language.UiTimeMeter.Special4, t.hour ,t.min)
		else
			t_str = string.format(Language.UiTimeMeter.TimeStr3, t.day, t.hour ,t.min)
		end
	elseif self.show_type == TimeType.Type_Time_4 then
		t = os.date("*t",time)
		t_str = string.format(Language.UiTimeMeter.TimeStr4, t.year, t.month ,t.day)
		
	elseif self.show_type == TimeType.Type_Special_0 then
		t_str = string.format(Language.UiTimeMeter.Special, t.min ,t.s)

	elseif self.show_type == TimeType.Type_Special_1 then
		t_str = string.format(Language.UiTimeMeter.Special, t.hour ,t.min)

	elseif self.show_type == TimeType.Type_Special_2 then
		t_str = t.s
	elseif self.show_type == TimeType.Type_Special_3 then
		if t.day > 0 then
			t_str = string.format(Language.UiTimeMeter.Special2,t.day, t.hour, t.min,t.s)
		else
			t_str = string.format(Language.UiTimeMeter.TimeStr, t.hour, t.min ,t.s)
		end
	elseif self.show_type == TimeType.Type_Special_4 then
		if t.day > 0 then
			t_str = string.format(Language.UiTimeMeter.Special3,t.day, t.hour)
		else
			t_str = string.format(Language.UiTimeMeter.Special4, t.hour, t.min)
		end
	elseif self.show_type == TimeType.Type_Special_5 then
		t_str = string.format(Language.UiTimeMeter.Special5, t.s)

	elseif self.show_type == TimeType.Type_Special_6 then
		t_str = string.format(Language.UiTimeMeter.Specia2, t.min ,t.s)

	elseif self.show_type == TimeType.Type_Time_5 then
		t_str = string.format(Language.UiTimeMeter.TimeStr2,t.day, t.hour, t.min,t.s)
	elseif self.show_type == TimeType.Type_Time_6 then
		t_str = string.format(Language.UiTimeMeter.TimeStr6,t.day, t.hour)
	elseif self.show_type == TimeType.Type_Special_7 then
		if t.day <= 0 then
			t_str = string.format(Language.Arena.NewSer3,t.hour, t.min, t.s)
		else
			t_str = string.format(Language.Arena.NewSer2,t.day, t.hour, t.min)
		end
	elseif self.show_type == TimeType.Type_Special_8 then
		if tonumber(t.hour) < 1 then --小心加0处理变string
			t_str = string.format(Language.UiTimeMeter.Specia2, t.min, t.s)
		else
			t_str = string.format(Language.UiTimeMeter.Special4, t.hour, t.min)
		end
	elseif self.show_type == TimeType.Type_Special_9 then
		if tonumber(t.day) > 0 then
			t_str = string.format(Language.UiTimeMeter.Special3, t.day, t.hour)
		elseif tonumber(t.hour) > 0 then
			t_str = string.format(Language.UiTimeMeter.Special4, t.hour, t.min)
		else
			t_str = string.format(Language.UiTimeMeter.Specia2, t.min, t.s)
		end
	elseif self.show_type == TimeType.Type_Time_7 then
		t_str = string.format(Language.UiTimeMeter.TimeStr, t.hour + t.day * 24, t.min,t.s)
	elseif self.show_type == TimeType.Type_Time_8 then
		if t.day > 0 then
			t_str = string.format(Language.UiTimeMeter.TimeStr3,t.day, t.hour, t.min)
		else
			t_str = string.format(Language.UiTimeMeter.TimeStr1, t.hour, t.min ,t.s)
		end
	elseif self.show_type == TimeType.Type_Special_10 then
		t_str = string.format(Language.UiTimeMeter.Special10, t.hour + t.day * 24, t.min)
	elseif self.show_type == TimeType.Type_Special_11 then
		if tonumber(t.hour) > 0 then
			t_str = string.format(Language.UiTimeMeter.TimeStr, t.hour, t.min, t.s)
		else
			t_str = string.format(Language.UiTimeMeter.Special, t.min, t.s)
		end
	elseif self.show_type == TimeType.Type_Special_12 then
		if tonumber(t.hour) > 0 then
			t_str = string.format(Language.UiTimeMeter.TimeStr1, t.hour, t.min, t.s)
		else
			t_str = string.format(Language.UiTimeMeter.Specia2, t.min, t.s)
		end
	end
	return t_str
end