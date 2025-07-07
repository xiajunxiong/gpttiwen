MUSICS_Scene = MUSICS_Scene or BaseClass(BaseScene)
function MUSICS_Scene:__init()
    
end
function MUSICS_Scene:StartLoading()
    self.scene_loaded = false
    SceneMgr:LoadScene("MUSICSTATION_1",function(error)
		if self:Disposed() then
			return
		end
		if not StringIsEmpty(error) then
			Debuger.LogError("GameScene Load Scene failed===MUSICSTATION_1")
		end
        self.scene_loaded = true

        ViewMgr:MainViewVisible(false)
        -- 括号里是多少就是多少关
        MusicStationCtrl.Instance:MusicStation(5)--MusicStationData.Instance:LoadProgress())
	end)
end

function MUSICS_Scene:LoadingComplete()
    local complete = self.scene_loaded and ViewMgr:IsOpen(MusicStationView)
    if complete then
        PreloadViewStateMachine.Instance:StopAll()
    end
    return complete
end

function MUSICS_Scene:Dispose()
    BaseScene.Dispose(self)
end