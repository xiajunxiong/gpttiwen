
StoryInfoHarmonyHeavensShow = StoryInfoHarmonyHeavensShow or BaseClass(StoryInfo)

function StoryInfoHarmonyHeavensShow:StoryInfo()
    -- self.trigger_type = StoryConfig.TriggerType.BattleStart
    -- self.ready_time = 1
end

function StoryInfoHarmonyHeavensShow:StoryReady()
    -- ViewMgr:MainViewVisible(false)
    -- SceneCtrl.Instance:Objs():Shielding(true)
    self:GetPostEffect()
    -- self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
    -- self.mainCanvas:SetActive(false)
    -- self.mainCanvas:SetActive(false)
    ViewMgr:MainViewVisible(false)
end

function StoryInfoHarmonyHeavensShow:StoryLoaded(obj)
end

function StoryInfoHarmonyHeavensShow:Play()
    -- ViewMgr:MainViewVisible(false)
    self.post_effect_state = self:SetPostEffectEnable(false)
    SceneCtrl.Instance:Objs():ShieldingWithoutTypes(true, {[SceneObjType.Soldier] = true})
    StoryInfo.Play(self)
end

function StoryInfoHarmonyHeavensShow:StoryEnd()
    -- ViewMgr:MainViewVisible(true)
    SceneCtrl.Instance:Objs():ShieldingWithoutTypes(false, {[SceneObjType.Soldier] = true})
    -- if self.mainCanvas then
    --     self.mainCanvas:SetActive(true)
    --     self.mainCanvas = nil
    -- end
    ViewMgr:MainViewVisible(true)
    ViewMgr:ViewOverrideSortingOpen()
    self:SetPostEffectEnable(self.post_effect_state or false)

    HarmonyHeavenData.Instance:AutoToNpc()
end

function StoryInfoHarmonyHeavensShow:NeedTrigger()
    return StoryInfo.SceneIdVerify(self.info.param1)
end

function StoryInfoHarmonyHeavensShow:GetPostEffect()
    local main_camera = UnityGameObj.FindGameObjectWithTag("MainCamera")
    self.post_effect = main_camera:GetComponent(typeof(Crystal.PostEffectGaussianBlur))
end