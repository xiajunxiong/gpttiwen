UIWFlushPanel = UIWFlushPanel or  DeclareMono("UIWFlushPanel")

-----Key==========String------缺省的话默认为类名
-----FlushEvents==Sring[]-----自动刷新监听的事件
-----LoadWait=====UIWSubstitute[](nil)----加载wait对象组

function UIWFlushPanel:UIWFlushPanel()
	self.need_flush = false
	self.flush_events = nil

	self.data_care_funcs = {}

	self.wait_flush_funcs = {}

	self.onflush_func = BindTool.Bind(self.onFlush,self)

	self.event_handles = nil

	self.data_care_handles = nil
end

-- function UIWFlushPanel:initFlush()
-- 	self.wait_flush_funcs[self.onflush_func] = true
-- end

function UIWFlushPanel:GetKey()
	if self.Key == nil then
		self.Key = self:GetTypeName()
	end
	return self.Key
end

function UIWFlushPanel:OnEnable()
	-- if #self.wait_flush_funcs > 0 then
		local cache_funcs = self.wait_flush_funcs
		self.wait_flush_funcs= {}
		for k,keys in pairs(cache_funcs) do
			self:performFlush(k,keys)
		end
	-- end
end
function UIWFlushPanel:IsLoaded()
	return true
end
function UIWFlushPanel:Flush()
	if self:IsActiveAndEnabled() == false then
		-- self.need_flush = true
		self.wait_flush_funcs[self.onflush_func] = true
	else
		self:performFlush(self.onflush_func)
	end
end

function UIWFlushPanel:performFlush(func, keys)
	-- if self:IsLoaded() then
		func(keys == true and nil or keys)
	-- else   --wait load
	-- 		Runner.Instance:RunUntilTrue(function ()
	-- 			local is_loaded = self:IsLoaded()
	-- 			if is_loaded then
	-- 				func(keys == true and nil or keys)
	-- 			end
	-- 			return is_loaded
	-- 		end)
	-- end
end

-- function UIWFlushPanel:IsLoaded()
	-- if self.LoadWait ~= nil then
	-- 	for i=1,self.LoadWait:Length() do
	-- 		local wait = self.LoadWait[i]
	-- 		if wait:GetObjActiveInScene() and wait:IsEnabled() and not wait:IsLoaded() then
	-- 			return false
	-- 		end
	-- 	end
	-- end
-- 	return true
-- end

--override
function UIWFlushPanel:onFlush()

end

function UIWFlushPanel:Awake()
	if self.FlushEvents ~= nil then
		self.event_handles = {}
		for i=1,self.FlushEvents:Length() do
			local handle = EventSys:Bind(self.FlushEvents[i],BindTool.Bind(self.Flush,self))
			table.insert(self.event_handles,handle)
		end
	end
	if self.flush_events ~= nil then
		if self.event_handles == nil then self.event_handles = {} end
		for _,eve_key in pairs(self.flush_events) do
			local handle = EventSys:Bind(eve_key,BindTool.Bind(self.Flush,self))
			table.insert(self.event_handles,handle)
		end
	end

	local repeat_call_check = {}
	if self.data_cares ~= nil then
		self.data_care_handles = {}
		for k,v in pairs(self.data_cares) do
			local fl_func = self:getDataCareFunc(v.func)
			if v.data.KeysCare then
				local handle = v.data:KeysCare(v.keys,fl_func)
				self.data_care_handles[handle] = v.data
				if not (v.init_call == false) and (repeat_call_check[fl_func] ~= true) then
					repeat_call_check[fl_func] = true
					fl_func()
				end
			elseif not IS_EDITOR then
				BuglyReport(self:GetKey(),"k",k,"data not a smartData, not has KeysCare")
			else
				LogError(self:GetKey(),"k",k,"data not a smartData, not has KeysCare")
			end
		end
	end
end

function UIWFlushPanel:OnDestroy()
	LuaMonoBehaviour.OnDestroy(self)
	if self.event_handles ~= nil then
		for _,v in pairs(self.event_handles) do
			EventSys:UnBind(v)
		end
		self.event_handles = nil
	end

	if self.data_care_handles ~= nil then
		for k,v in pairs(self.data_care_handles) do
			v:Uncare(k)
		end
		self.data_care_handles = nil
	end
end
function UIWFlushPanel:uncareAllData()
	if self.data_care_handles ~= nil then
		for k,v in pairs(self.data_care_handles) do
			v:Uncare(k)
		end
		self.data_care_handles = {}
	end
end

function UIWFlushPanel:getDataCareFunc(func_name)
	local cache_func = self.data_care_funcs[func_name]
	if cache_func == nil then
		local flush_func = BindTool.Bind(func_name,self)
		cache_func = function(keys)
			if not self:IsActiveAndEnabled() then
				if keys == nil then		---all change
					self.wait_flush_funcs[flush_func] = true
				else
					local cache_wait_flush = self.wait_flush_funcs[flush_func]
					if cache_wait_flush == nil then
						self.wait_flush_funcs[flush_func] = keys
					elseif type(cache_wait_flush) == "table" then 
						for k,v in pairs(keys) do
							if cache_wait_flush[k] == nil then
								cache_wait_flush[k] = v
							end
						end
					end
				end
			else
				self:performFlush(flush_func,keys)
			end
		end
		self.data_care_funcs[func_name] = cache_func
	end
	return cache_func
end

function UIWFlushPanel:FlushCare(func)
	local exe_func = self.data_care_funcs[func]
	if exe_func ~= nil then
		exe_func()
	end
end

function UIWFlushPanel:CareData(handle,data,func,keys,init_call)
	self:UncareData(handle)
	if data ~= nil then
		local fl_func = self:getDataCareFunc(func)
		if data.KeysCare then
			handle = data:KeysCare(keys,fl_func)
			self.data_care_handles[handle] = data
			if not (init_call == false) then
				fl_func()
			end	
			return handle
		elseif not IS_EDITOR then
			BuglyReport(self:GetKey(),"data is not a smartData, not has KeysCare")
		else
			LogError(self:GetKey(),"data is not a smartData, not has KeysCare")
		end
	else
		return nil
	end
end

function UIWFlushPanel:UncareData(handle)
	if handle ~= nil then
		local data = self.data_care_handles[handle]
		if data ~= nil then
			data:Uncare(handle)
		end
		self.data_care_handles[handle] = nil
	end
	return nil
end
function UIWFlushPanel:OnTriggerEnter()
	-- body
end
function UIWFlushPanel:OnTriggerExit()
	-- body
end