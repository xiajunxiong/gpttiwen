GameStateLuaRequire = GameStateLuaRequire or BaseClass(GameState)
-- GameStateLuaRequire.ViewObj = nil
-- GameStateLuaRequire.ViewCanvasGroup = nil

-- GameStateLuaRequire.SubState = {
-- 	None = -1,
-- 	LoadView = 0,
-- 	OpenView = 1,
-- 	RequireScript = 2,
-- 	-- CloseView = 3,
-- }

-- GameStateLuaRequire.open_time = 0.75
-- GameStateLuaRequire.close_time = 1
-- GameStateLuaRequire.require_time = 1.5
GameStateLuaRequire.FastRequireCount = 200
GameStateLuaRequire.SlowRequireCount = 5



function GameStateLuaRequire:__init()
	-- self.cur_st = GameStateLuaRequire.SubState.None
	-- self.timer = 0
	self.cur_list_idx = 1
	self.cur_file_idx = 1
	self.total_count = 0
	self.all_req_list = nil
	self.required_count = 0
	self.view_require_list = nil



end

function GameStateLuaRequire:__delete()

end

local u_time = UnityEngine.Time

function GameStateLuaRequire:Update()
	local begin_t = os.clock()
	
			-- if PreloadViewStateLoadInst == nil or 
			-- 	PreloadViewStateMachine.Instance:CurrentState() ~= PreloadViewStateLoadInst then
			-- 	return
			-- end

	-- local cur_req_count = 0
	-- local frame_req_count = (PreloadData.Instance:GetProgSpeed() == PreloadData.ProgSpeed.Slow) and GameStateLuaRequire.SlowRequireCount or GameStateLuaRequire.FastRequireCount
	-- UnityEngine.Debug.LogError("FC====" .. tostring(frame_req_count))
	-- for i=self.cur_list_idx,#self.all_req_list do
	-- 	local file_list = self.all_req_list[i]
	-- 	for j = self.cur_file_idx,#file_list do
	-- 		-- print("require====" .. tostring(file_list[j]))
	-- 		require(file_list[j])
	-- 		self.required_count = self.required_count + 1
	-- 		cur_req_count = cur_req_count + 1
	-- 		if self.required_count >= (self.total_count + #self.view_require_list) then
	-- 			PreloadData.Instance:SetScriptRequirePer(1) --总加载结束
	-- 			-- UnityEngine.Debug.LogError("TOT COUNT====" .. tostring(self.total_count) .."," .. tostring(#self.view_require_list))
	-- 			self:Stop()
	-- 			return
	-- 		elseif cur_req_count >= frame_req_count then		--本帧的加载结束
	-- 			self.cur_list_idx = i
	-- 			self.cur_file_idx = j + 1
	-- 			local cur_per = self.required_count/(self.total_count + #self.view_require_list)
	-- 			if cur_per > PreloadData.Instance:GetScriptRequirePer() then	--可能中途添加总加载数量，防止进度倒退
	-- 				PreloadData.Instance:SetScriptRequirePer(cur_per)
	-- 			end
	-- 			return
	-- 		end
	-- 	end
	-- 	self.cur_file_idx = 1
	-- end
	
	local update_t = (PreloadData.Instance:GetProgSpeed() == PreloadData.ProgSpeed.Slow) and 0.02 or 0.1
	local cur_per = 0
	-- for i=1,frame_req_count do
	while true do
		self.require_handle:RequireOne()
		cur_per = self.require_handle:RequiredPercent()
		if cur_per >= 1 then
			self:Stop()
			break
		else 
			local past_t = os.clock() - begin_t
			-- UnityEngine.Debug.LogError("past_t===" .. past_t)
			if past_t >= update_t then
				-- if past_t > 0.03 then
				-- 	UnityEngine.Debug.LogError("LargeFile===" .. self.require_handle:GetCurReqedFile() .. ",t=" .. past_t)
				-- end
				break
			end
		end
	end
	PreloadData.Instance:SetScriptRequirePer(cur_per)

end

function GameStateLuaRequire:Start()
	GameState.Start(self)
	-- if not IS_IOS() then
		print("GameStateLuaRequire:Start")
	-- end
	PreloadData.Instance:SetState(PreloadData.State.Require)
	MainProber:Step(MainProber.STEP_LOAD_CONFIG_BEG)
	self:RequireAllScript()

	if SIMULATE_ASSETBUNDLE == false and IS_MICRO_PLAYER() then
		bundle_preloader = require("preload/ab_preload")
		bundle_preloader:Begin()
	end
end

function GameStateLuaRequire:RequireAllScript()
	self.require_handle = require("core/require_list")
	self.require_handle:Reset()
	-- local require_data = require("core/require_list")
	-- local require_files = require_data[1]
	-- local require_lists = require_data[2]
	-- self.all_req_list = {}
	-- self.total_count = #require_files
	-- table.insert(self.all_req_list,require_files)
	
	-- for _,fs in ipairs(require_lists) do
	-- 	self.total_count = #fs + self.total_count
	-- 	table.insert(self.all_req_list,fs)
	-- end
	-- self.view_require_list = self.all_req_list[#self.all_req_list]
	-- self.cur_list_idx = 1
	-- self.cur_file_idx = 1
	-- self.required_count = 0

end

function GameStateLuaRequire:Stop()
	collectgarbage("setpause", 100)
	collectgarbage("setstepmul", 5000)
	-- self.cur_st = GameStateLuaRequire.SubState.None
	MainProber:Step(MainProber.STEP_LOAD_CONFIG_END)
	GameState.Stop(self)
	-- if not IS_IOS() then
		print("GameStateLuaRequire:Stop")
	-- end
end


function GameStateLuaRequire.CloseView()
	-- if GameStateLuaRequire.ViewObj ~= nil then
	-- 	UnityGameObj.Destroy(GameStateLuaRequire.ViewObj)
	-- 	GameStateLuaRequire.ViewObj = nil
	-- 	GameStateLuaRequire.ViewCanvasGroup = nil
	-- end
end

return GameStateLuaRequire