StoryInfoPoetry = StoryInfoPoetry or BaseClass(StoryInfo)

function StoryInfoPoetry:StoryInfo()
    
end

function StoryInfoPoetry:StoryReady()
    self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
end

function StoryInfoPoetry:StoryLoaded(obj)

end

function StoryInfoPoetry:Play()
    self.mainCanvas:SetActive(false)
    -- 用于是否显示npc和角色模型，当前隐藏
    SceneCtrl.Instance:Objs():ShieldingWithoutTypes(true, {[SceneObjType.Soldier] = true})
    StoryInfo.Play(self)
end

function StoryInfoPoetry:StoryEnd()
	if self.mainCanvas then
        self.mainCanvas:SetActive(true)
        self.mainCanvas = nil
    end
    -- 用于是否显示npc和角色模型，当前隐藏
    SceneCtrl.Instance:Objs():ShieldingWithoutTypes(false, {[SceneObjType.Soldier] = true})
    ViewMgr:ViewOverrideSortingOpen()
    ViewMgr:CloseView(PoetryView)
end

function StoryInfoPoetry:NeedTrigger()
    return StoryInfo.SceneIdVerify(131)
end