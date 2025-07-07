
ExternalLuaUpdater = require("preload/external_lua_updater")
ExternalLuaLoader = require("preload/external_lua_loader")


GameStateExtluaUpdate = GameStateExtluaUpdate or BaseClass(GameState)


function GameStateExtluaUpdate:Start()
	GameState.Start(self)
	PreloadData.Instance:SetState(PreloadData.State.ExtluaUpdate)

	if not EditorSettings:UpdateExternalLua() then
		-- UnityEngine.Debug.LogError("@@@@@@@@@@@@@")
		self:Stop()
		return
	end


	ExternalLuaUpdater:PerformUpdate(function ()
		local err = ExternalLuaUpdater:GetError()
		if err == nil then
			self:Stop()
		else
			PreloadData.Instance:SetUpdateError(true)
			print("GameStateExtluaUpdate ExternalLuaUpdater PerformUpdate err:" .. tostring(err))
		end
	end)
end

function GameStateExtluaUpdate:Update()
	ExternalLuaUpdater:Update()
end

function GameStateExtluaUpdate:Stop()
	GameState.Stop(self)
end

function GameStateExtluaUpdate:GetProgress()
	return ExternalLuaUpdater:GetProgressData().total
end

-- function GameStateExtluaUpdate:GetError()
-- 	return ExternalLuaUpdater:GetError()
-- end

return GameStateExtluaUpdate
