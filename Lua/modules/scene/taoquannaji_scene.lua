TQNJ_Scene = TQNJ_Scene or BaseClass(BaseScene)
function TQNJ_Scene:__init()
    
end
function TQNJ_Scene:StartLoading()
    self.scene_loaded = false
    SceneMgr:LoadScene("TQNJ_1",function(error)
		if self:Disposed() then
			return
		end
		if not StringIsEmpty(error) then
			Debuger.LogError("GameScene Load Scene failed===TQNJ_1")
		end
        ViewMgr:MainViewVisible(false)
        TaoQuanNaJiBattleShow.Instance:StartScene(function()
            self.scene_loaded = true
            ViewMgr:OpenView(TaoQuanNaJiBattleView)
        end)
	end)
end

function TQNJ_Scene:LoadingComplete()
    local complete = self.scene_loaded and ViewMgr:IsOpen(TaoQuanNaJiBattleView)
    if complete then
        PreloadViewStateMachine.Instance:StopAll()
    end
    return complete
end

function TQNJ_Scene:Dispose()
    BaseScene.Dispose(self)
    TaoQuanNaJiBattleShow.Instance:ExitScene()
end