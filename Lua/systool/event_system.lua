require("systool/event_keys")

EventSystem = EventSystem or BaseClass()

EventSys = nil

function EventSystem:__init()
	if EventSys ~= nil then
		Debuger.LogError("[EventSystem] attempt to create singleton twice!")
		return
	end
	EventSys = self

	self.event_list = {}	---event_tab_list{funcs{key=handle,value=func}}
end

function EventSystem:__delete()
	if EventSys == self then
		EventSys = nil
	end
end

--return event handle = {event_key}
function EventSystem:Bind(key,func)
	-- Debuger.LogError("Bind key===%s",key)
	local event = self.event_list[key]
	if event == nil then
		event = {func_map = {}}
		self.event_list[key] = event
	end
	local re_handle = {event_key = key}
	event.func_map[re_handle] = func
	return re_handle
end

function EventSystem:UnBind(handle)
	if handle == nil then return end
	local event = self.event_list[handle.event_key]
	if event ~= nil then
		event.func_map[handle] = nil
	end
end

function EventSystem:Fire(key, ...)
	-- Debuger.LogError("Fire key===%s",key)
	local event = self.event_list[key]
	if event ~= nil then
		for _,func in pairs(event.func_map) do
			func(...)
		end
	end
end