MiniGameScene = MiniGameScene or BaseClass(BaseScene)

function MiniGameScene:__init()
	self.select_view = nil
end

function MiniGameScene:__delete()
	
end

function MiniGameScene:StartLoading()
	self.scene_loaded = false
	local mini_game = LoginData.Instance:GetMiniGame()
    if mini_game > 0 then
		-- 区分选关界面
		if mini_game == MINIGAME_TYPE.TOWER_ADVENTURE then
			self.select_view = TowerAdventureLevelView
			TowerAdventureData.Instance:ResetData()
		elseif mini_game == MINIGAME_TYPE.COW_BOY then
			self.select_view = CowBoyLevelView
			CowBoyData.Instance:ResetData()
		end
		ViewMgr:OpenView(self.select_view)
	else
		SceneCtrl.Instance:GotoScene(RoleCreateScene)
	end
    
    SceneMgr:LoadScene("MINIGAME_1",function(error)
		if self:Disposed() then
			return
		end
		if not StringIsEmpty(error) then
			Debuger.LogError("GameScene Load Scene failed===MINIGAME_1")
		end
		self.scene_loaded = true
	end)
end

function MiniGameScene:LoadingComplete()
    if ViewMgr:IsOpen(self.select_view) and self.scene_loaded then
        return true
    end
    return false
end

function MiniGameScene:Dispose()
    MiniGameCtrl.Instance:IsOpenMiniGameView(true)
	BaseScene.Dispose(self)
end