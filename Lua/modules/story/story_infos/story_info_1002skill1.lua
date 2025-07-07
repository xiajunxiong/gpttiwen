
StoryInfo1002Skill1 = StoryInfo1002Skill1 or BaseClass(StoryInfo)

function StoryInfo1002Skill1:StoryInfo()
    -- self.trigger_type = StoryConfig.TriggerType.BattleStart
end

function StoryInfo1002Skill1:StoryReady()
end

function StoryInfo1002Skill1:StoryLoaded(obj)
end

function StoryInfo1002Skill1:storyCheckResourcesEnd()
end

function StoryInfo1002Skill1:Play()
    BattleData.Instance:ViewInfo().opened_sub = BattleSubView.None
    --ViewMgr:MainViewVisible(false)
    ViewMgr:CloseAllViewInBattle()
    StoryInfo.Play(self)
    -- BattleData.Instance.story_data.is_start = true
end

function StoryInfo1002Skill1:StoryEnd()
    -- ViewMgr:MainViewVisible(true)
    self:CallCustomFunc()
    -- BattleData.Instance.story_data.is_start = false
end