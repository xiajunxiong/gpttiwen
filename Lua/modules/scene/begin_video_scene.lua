BeginVideoScene = BeginVideoScene or BaseClass(BaseScene)
if IS_EDITOR then
    BeginVideoScene.TEST = false
end

function BeginVideoScene:__init()
    self.cg_com = false
    self.load_com = false
end

function BeginVideoScene:__delete()

end

function BeginVideoScene:StartLoading()
    VideoCtrl.Instance:StartVideoById(5,function() 
        self.cg_com = true
        if self.load_com == true then
            MainProber:Step(MainProber.STEP_BEGINCG_END)
            SceneCtrl.Instance:GotoScene(LoginScene)
        end
    end)
end

function BeginVideoScene:LoadingComplete()
	return ViewMgr:IsOpen(VideoView) or self.cg_com
end
 
function BeginVideoScene:OnLoadingComplete()
    self.load_com = true
    UnityPlayerPrefs.SetInt("HasPlayedBeginCG",1)
    PreloadViewStateMachine.Instance:StopAll() --进入CG界面后删除加载页
    if self.cg_com == true then
        MainProber:Step(MainProber.STEP_BEGINCG_END)
        SceneCtrl.Instance:GotoScene(LoginScene)
    end    
end

function BeginVideoScene:Dispose()
	BaseScene.Dispose(self)
end

function BeginVideoScene.NeedPlayCG()
    if BeginVideoScene.TEST then
        return true
    else
        return UnityPlayerPrefs.GetInt("HasPlayedBeginCG",0) == 0
    end
end