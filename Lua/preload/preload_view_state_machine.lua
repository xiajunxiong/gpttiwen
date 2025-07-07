require("preload/preload_view_state")

PreloadViewStateMachine = PreloadViewStateMachine or BaseClass()
local state_list = {
	"preload/preload_view_state_health",
	"preload/preload_view_state_load",
}

local state_tab_dic = {}
for _,v in ipairs(state_list) do
	state_tab_dic[v] = require(v)
end

function PreloadViewStateMachine:__init()
    if PreloadViewStateMachine.Instance ~= nil then
		UnityEngine.Debug.LogError("[PreloadViewStateMachine] attempt to create singleton twice!")
		return
    end
    PreloadViewStateMachine.Instance = self
	self.current_state = nil
    self.runing_state_index = 0
    self.last_state = nil
end

function PreloadViewStateMachine:__delete()
	if PreloadViewStateMachine.Instance == self then
		PreloadViewStateMachine.Instance = nil
	end
end

function PreloadViewStateMachine:Run()
	-- print("PreloadViewStateMachine:Run")
	self.runing_state_index = 0
	self:moveToNextState()
end

function PreloadViewStateMachine:CurrentState()
	return self.current_state
end

function PreloadViewStateMachine:CurrentStateType()
	if self.current_state == nil then
		return nil
	else
		return self.current_state:GetClassType()
	end
end

function PreloadViewStateMachine:moveToNextState()
	self.runing_state_index = self.runing_state_index + 1
	if self.runing_state_index <= #state_list then
        local last_state = self.current_state
        local state_class = state_tab_dic[state_list[self.runing_state_index]]
		self.current_state = state_class.New()
		local func = function ()
            if last_state ~= nil then
                last_state:DestroyView()
            end
		end
		-- UnityEngine.Debug.LogError("!!!!"..tostring(func))
		self.current_state:Start(func)
    else
        self.last_state = self.current_state
		self.current_state = nil
	end
end

function PreloadViewStateMachine:Update()
	if self.current_state ~= nil then
		local st = self.current_state:State()
		if st == PreloadViewStateState.Running then
			self.current_state:Update()
		elseif st == PreloadViewStateState.Stoped then
			self:moveToNextState()
		else
			-- print("st====" .. tostring(st))
		end
	end
end

function PreloadViewStateMachine:Finished()
    return self.current_state == nil
end

function PreloadViewStateMachine:StopAll()
	for _,tab in pairs(state_tab_dic) do
		tab.ReleaseViewObj()
	end

    if self.last_state ~= nil then
        self.last_state:DestroyView()
        self.last_state = nil
    end
end