require("preload/game_state")

GameStateMachine = GameStateMachine or BaseClass()
local play_state_name = "core/game_state_play"
local state_list = {
"preload/game_state_init",
"preload/game_state_update",
"preload/game_state_extlua_update",
"preload/game_state_update_luabundle",
"core/game_state_lua_require",
play_state_name,
}

function GameStateMachine:__init()
	self.current_state = nil
	self.runing_state_index = 0
end

function GameStateMachine:__delete()

end

function GameStateMachine:Run()
	self.runing_state_index = 0
	self:moveToNextState()
end

function GameStateMachine:moveToNextState()
	self.runing_state_index = self.runing_state_index + 1
	if self.runing_state_index <= #state_list then
		local state_name = state_list[self.runing_state_index]
		if state_name ~= play_state_name or PreloadViewStateMachine.Instance:Finished() then	--跳转到play状态前需要等待view层执行完成
			local state_class = require(state_name)
			self.current_state = state_class.New()
			self.current_state:Start()
		else
			self.runing_state_index = self.runing_state_index - 1
		end
	else
		self.current_state = nil
	end
end

function GameStateMachine:Update()
	if self.current_state ~= nil then
		local st = self.current_state:State()
		if st == GameStateState.Running then
			self.current_state:Update()
		elseif st == GameStateState.Stoped then
			self:moveToNextState()
		else
			print("st====" .. tostring(st))
		end
	end
	if ChatFilterLpeg ~= nil and ChatFilterLpeg.Update~= nil then
		ChatFilterLpeg.Update()
	end
end




