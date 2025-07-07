
StoryInfoTaskHCShow = StoryInfoTaskHCShow or BaseClass(StoryInfo)

function StoryInfoTaskHCShow:StoryInfo()
    -- self.trigger_type = StoryConfig.TriggerType.BattleStart
    -- self.ready_time = 1
end

function StoryInfoTaskHCShow:StoryReady()
    -- ViewMgr:MainViewVisible(false)
    -- SceneCtrl.Instance:Objs():Shielding(true)
    self:GetPostEffect()
    self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
    self.mainCanvas:SetActive(false)
    -- self.mainCanvas:SetActive(false)
end

function StoryInfoTaskHCShow:StoryLoaded(obj)
end

function StoryInfoTaskHCShow:Play()
    -- ViewMgr:MainViewVisible(false)
    self.post_effect_state = self:SetPostEffectEnable(false)
    SceneCtrl.Instance:Objs():ShieldingWithoutTypes(true, {[SceneObjType.Soldier] = true})
    StoryInfo.Play(self)
end

function StoryInfoTaskHCShow:StoryEnd()
    -- ViewMgr:MainViewVisible(true)
    SceneCtrl.Instance:Objs():ShieldingWithoutTypes(false, {[SceneObjType.Soldier] = true})
    if self.mainCanvas then
        self.mainCanvas:SetActive(true)
        self.mainCanvas = nil
    end
    ViewMgr:ViewOverrideSortingOpen()
    self:SetPostEffectEnable(self.post_effect_state or false)

    local task = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.mainline)
    if task then
        task:GoTask()
    end
end

function StoryInfoTaskHCShow:NeedTrigger()
    return StoryInfo.SceneIdVerify(108)
end

function StoryInfoTaskHCShow:GetPostEffect()
    local main_camera = UnityGameObj.FindGameObjectWithTag("MainCamera")
    self.post_effect = main_camera:GetComponent(typeof(Crystal.PostEffectGaussianBlur))
end