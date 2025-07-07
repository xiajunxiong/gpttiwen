
StoryInfoTrans1002 = StoryInfoTrans1002 or BaseClass(StoryInfo)

function StoryInfoTrans1002:StoryInfo()
    -- self.trigger_type = StoryConfig.TriggerType.BattleStart
end

function StoryInfoTrans1002:StoryReady()
    ViewMgr:OpenView(StoryShowBossView)
end

function StoryInfoTrans1002:StoryLoaded(obj)
end

function StoryInfoTrans1002:storyCheckResourcesEnd()
end

function StoryInfoTrans1002:Play()
    BattleData.Instance:ViewInfo().opened_sub = BattleSubView.None
    ViewMgr:MainViewVisible(false)
    ViewMgr:CloseAllViewInBattle()
    StoryInfo.Play(self)
end

function StoryInfoTrans1002:StoryEnd()
    ViewMgr:CloseView(StoryShowBossView)
    ViewMgr:MainViewVisible(true)
    self:CallCustomFunc()
    -- local story_info = StoryCtrl.Instance:PlayStoryByStoryId(104)
    -- story_info:SetCustomFunc(self.custom_func)
    -- self.custom_func = nil
end

function StoryInfoTrans1002:StoryEvent(type, params)
    if StoryConfig.CustomEventType.Pause == type then
        self:Pause()
    end
    if StoryConfig.CustomEventType.ShowBoss == type then
        self.data.story_event:Set{id = self.info.id}
    end
end