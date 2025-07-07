
StoryInfoShowBoss104 = StoryInfoShowBoss104 or BaseClass(StoryInfo)

function StoryInfoShowBoss104:StoryInfo()
    self.trigger_type = StoryConfig.TriggerType.BattleTrans
end

function StoryInfoShowBoss104:StoryReady()
    -- SE:Play(SceneEffectConfig.EffectList.HCHuoYan)
    -- BattleData.Instance:BaseData().pause = true
    -- ViewMgr:MainViewVisible(false)
    -- ViewMgr:OpenView(StoryShowBossView)
end

function StoryInfoShowBoss104:StoryLoaded(obj)
    self.story_mono = obj:GetLuaComponent(StoryMonoItem:GetTypeName())
    local boss_live, boss_pos = self:BossPos()
    if boss_live then self.story_mono.BossPos:SetTarget(boss_pos) end
end

function StoryInfoShowBoss104:storyCheckResourcesEnd()
end

function StoryInfoShowBoss104:Play()
    -- BattleData.Instance:BaseData().pause = true
    -- ViewMgr:MainViewVisible(false)
    ViewMgr:OpenView(StoryShowBossView)
    StoryInfo.Play(self)
end

function StoryInfoShowBoss104:StoryDyLoad(wait_until_get_resource, res_type, res_id)
end

function StoryInfoShowBoss104:StoryEvent(type, params)
    if StoryConfig.CustomEventType.ShowBoss == type then
        self.data.story_event:Set{id = self.info.id}
    end
end

function StoryInfoShowBoss104:StoryEnd()
    ViewMgr:CloseView(StoryShowBossView)
    -- BattleData.Instance:BaseData().pause = false
    ViewMgr:MainViewVisible(true)
    self:CallCustomFunc()
end

-- function StoryInfoShowBoss104:NeedTrigger()
--     return self:MonsterGroupIdVerify()
-- end