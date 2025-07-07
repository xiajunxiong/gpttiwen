--[[@------------------------------------------------------------------
说明: 根据外部设定的优先级在每一帧中依次执行所有托管的RunObj
作者: deadline
----------------------------------------------------------------------]]
Runner = Runner or BaseClass()
local touch_down_delay_frame = 1
local touch_helper = Game.SceneTouchHelper
UnityTouchState = {
	native_touch_down_frame = 0,
	delay_touch_up_true = false,
	is_touching = false,
	touch_ui = false,
	is_touch_up = false,
	last_touch_time = 0,
}

function UnityTouchState.IsTouchDown()
	if IS_IOS() then
		return UnityTouchState.is_touching
	else
		return touch_helper.IsTouching()
	end
end

function UnityTouchState.IsTouchOnUI()
	if IS_IOS() then
		return UnityTouchState.touch_ui
	else
		return touch_helper.IsTouchOnUI()
	end
end

function UnityTouchState.IsTouchUp()
	if IS_IOS() then
		return UnityTouchState.is_touch_up
	else
		return touch_helper.IsTouchUp()
	end
end

function UnityTouchState.LastTouchTime()
	return UnityTouchState.last_touch_time
end

function UnityTouchState.ResetLastTouchTime()
	UnityTouchState.last_touch_time = Time.time
end

function UnityTouchState.updateTouchState()
	-- if touch_helper.IsTouching() == true then
	-- 	LogError("UnityTouchState:TOUCHSTATE====IsTouching TRUE!!",Time.frameCount,UnityEngine.Time.frameCount)
	-- end
	-- if touch_helper.IsTouchOnUI() == true then
	-- 	LogError("XXX==UnityTouchState:TOUCHSTATE====IsTouchOnUIIIIII",Time.frameCount,UnityEngine.Time.frameCount)
	-- end
	-- 	if touch_helper.IsTouchUp() == true then
	-- 	LogError("UnityTouchState:TOUCHSTATE====TRUE IsTouchUp",Time.frameCount,UnityEngine.Time.frameCount)
	-- end
	local native_touch_down = touch_helper.IsTouching()
	if native_touch_down then
		UnityTouchState.last_touch_time = Time.time
	end
	if not IS_IOS() then
		return
	end
	if UnityTouchState.delay_touch_up_true then
		UnityTouchState.is_touch_up = true
		UnityTouchState.delay_touch_up_true = false
	else
		UnityTouchState.is_touch_up = false
	end
	UnityTouchState.is_touching = false
	if UnityTouchState.touch_ui == true then
		UnityTouchState.touch_ui = touch_helper.IsTouchOnUI()
	end
	if touch_helper.IsTouchUp() == true then
		if UnityTouchState.native_touch_down_frame ~= 0 then
			UnityTouchState.is_touching = true
			UnityTouchState.touch_ui = touch_helper.IsTouchOnUI() 
			UnityTouchState.is_touch_up = false
			UnityTouchState.native_touch_down_frame = 0
			UnityTouchState.delay_touch_up_true = true
		else
			UnityTouchState.is_touch_up = true
		end
		return
	end
	if native_touch_down == true then
		if touch_helper.IsTouchOnUI() == true then
			UnityTouchState.is_touching = true
			UnityTouchState.touch_ui = true
			UnityTouchState.native_touch_down_frame = 0
		else
			UnityTouchState.native_touch_down_frame = Time.frameCount
		end
		return
	end
	if UnityTouchState.native_touch_down_frame ~= 0 then
		if Time.frameCount - UnityTouchState.native_touch_down_frame <= touch_down_delay_frame then
			if touch_helper.IsTouchOnUI() then
				UnityTouchState.is_touching = true
				UnityTouchState.touch_ui = true
				UnityTouchState.native_touch_down_frame = 0
			end
		else
			UnityTouchState.is_touching = true
			UnityTouchState.touch_ui = false
			UnityTouchState.native_touch_down_frame = 0
		end
	end
end


function Runner:__init()
	if Runner.Instance ~= nil then
		error("[Runner] attempt to create singleton twice!")
		return
	end
	Runner.Instance = self


	-- 用于标记某个模块是否已经注册,避免重复性的注册
	self.all_run_obj_list = {}		
	self.id_count = 0


	self.run_until_true = {}
	self.run_until_true_for_queue = {}

	--支持1 ~ 16级优先级指定, 1为最先执行, 16为最后执行
	self.priority_run_obj_list = {}
	for i=1,16 do
		table.insert(self.priority_run_obj_list, {})
	end

	self.time_requests = {}

end

function Runner:__delete()
	self.all_run_obj_list = nil		
	self.id_count = 0
	self.run_until_true = nil
	self.run_until_true_for_queue = nil
	self.priority_run_obj_list = nil
	self.time_requests = nil

	if Runner.Instance == self then
		Runner.Instance = nil
	end
end

--[[@
功能:	主Update中调用该方法,触发托管对象的Update
参数:	
		无
返回值:
		无
其它:	无
作者:	deadline
]]
function Runner:Update()
	UnityTouchState.updateTouchState()
	local cpy_tbl_list = nil
	-- if IS_EDITOR and IS_IOS then
	-- 	cpy_tbl_list = {}
	-- 	for i=1, 16 do
	-- 		local priority_tbl = self.priority_run_obj_list[i]
	-- 		cpy_tbl_list[i] = {}
	-- 		for k, v in pairs(priority_tbl) do
	-- 			cpy_tbl_list[i][k] = v
	-- 		end
	-- 	end
	-- else
		cpy_tbl_list = self.priority_run_obj_list
	-- end
	for i=1,16 do
		local priority_tbl = cpy_tbl_list[i]
		for _, v in pairs(priority_tbl) do
			v:Update()
		end
	end
	local remove_wait, until_true_call
	local need_remove
	-- if table.nums(self.run_until_true) == 0 then
	-- 	goto FLAG
	-- end
	remove_wait = {}
	until_true_call = DataHelper.TableCopy(self.run_until_true)
	for k,v in pairs(until_true_call) do
		need_remove = (v() == true)
		if need_remove then
			table.insert(remove_wait,k)
		end
	end

	if #remove_wait ~= 0 then
		for i,v in pairs(remove_wait) do
			self.run_until_true[v] = nil
		end
	end

	for i, v in ipairs(self.run_until_true_for_queue) do
		need_remove = (v() == true)
		if need_remove then
			table.remove(self.run_until_true_for_queue, i)
		end
	end
	
	-- ::FLAG::

	local now_time = Time.realtimeSinceStartup
	local callback_list = {}
	local delete_list = {}		
	for k, v in pairs(self.time_requests) do
		if v[3] <= now_time then
			callback_list[#callback_list + 1] = v
			if 1 == v[4] then
				delete_list[#delete_list + 1] = k
			else
				v[3] = v[3] + v[2]
				if v[4] > 1 then
					v[4] = v[4] - 1
				end
			end
		end
	end
	for k, v in pairs(callback_list) do
		v[1]()
	end
	for k, v in pairs(delete_list) do
		self.time_requests[v] = nil
	end
end

--[[@
功能:	向Runner添加一个RunObj, RunObj必须存在Update方法
参数:	
		run_obj 要添加的run_obj对象 any 必须实现Update方法
		priority_level Update优先级 1-16,数字越小越先执行
返回值:
		是否添加run_obj成功 bool run_obj如果已经存在于Runner中, 则不能再添加
其它:	无
作者:	deadline
]]
function Runner:AddRunObj(run_obj , priority_level)
	local obj = self.all_run_obj_list[run_obj]
	if obj ~= nil then
		--已经存在该对象, 不重复添加
		return false
	else
		if run_obj["Update"] == nil then
			error("Runner:AddRunObj try to add a obj not have Update method!")
		end

		--对象不存在,正常添加
		self.id_count = self.id_count + 1
		priority_level = priority_level or 16
		self.all_run_obj_list[run_obj] = {priority_level, self.id_count}
		self.priority_run_obj_list[priority_level][self.id_count] = run_obj
	end
end

--[[@
功能:	从Runner中删除一个run_obj
参数:	
		run_obj
返回值:
		无
其它:	无
作者:	deadline
]]
function Runner:RemoveRunObj( run_obj )
	local key_info = self.all_run_obj_list[run_obj]
	if key_info ~= nil then
		self.all_run_obj_list[run_obj] = nil
		self.priority_run_obj_list[key_info[1]][key_info[2]] = nil
	end
end


--func中返回false表示keppWait下一帧会继续执行,直到func返回true,不再执行注册 函数并自动注销
function Runner:RunUntilTrue(func, init_call)
	if init_call then
		if func() then
			return
		end
	end
	local t = {}
	self.run_until_true[t] = func
	return t
end
--func中返回false表示keppWait下一帧会继续执行,直到func返回true,不再执行注册 函数并自动注销
function Runner:RunUntilTrueForQueue(func, init_call)
	if init_call then
		if func() then
			return
		end
	end
	table.insert(self.run_until_true_for_queue, func)
	return tostring(func)
end

function Runner:RemoveRunUntilTrue(handle)
	if handle == nil then return end
	self.run_until_true[handle] = nil
end
function Runner:RemoveRunUntilTrueForQueue(handle)
	if handle == nil then return end
	for i, v in pairs(self.run_until_true_for_queue) do
		if handle == tostring(v) then
			table.remove(self.run_until_true_for_queue, i)
		end
	end
end

--间隔指定时间调用函数
--func 需要调用的函数
--interval 间隔时间
--times 调用次数，默认nil为无限次数
--return 事件handle，用于注销
function Runner:RepeatCall(func,interval,times)
	local t = {}
	if times == nil then times = -1 end
	self.time_requests[t] = {func, interval, Time.realtimeSinceStartup + interval, times}
	return t
end

--注销RepeateCall
function Runner:CancelRepeat(handle)
	if handle == nil then
		return 
	end
	self.time_requests[handle] = nil
end