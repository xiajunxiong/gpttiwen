
StoryInfoShowBoss3 = StoryInfoShowBoss3 or BaseClass(StoryInfo)

function StoryInfoShowBoss3:StoryInfo()
    self.trigger_type = StoryConfig.TriggerType.BattleStart
    self.ready_time = 1
end

function StoryInfoShowBoss3:StoryReady()
    -- BattleData.Instance:BaseData().pause = true
    ViewMgr:MainViewVisible(false)
    ViewMgr:OpenView(StoryShowBossView)
end

function StoryInfoShowBoss3:StoryLoaded(obj)
    self.story_mono = obj:GetLuaComponent(StoryMonoItem:GetTypeName())
    local boss_live, boss_pos = self:BossPos()
    if boss_live then self.story_mono.BossPos:SetTarget(boss_pos) end
end

function StoryInfoShowBoss3:StoryDyLoad(wait_until_get_resource, res_type, res_id)
    -- if res_type == "MonsterPos" then
    --     wait_until_get_resource(
    --         function()
    --             -- return BattleCtrl.Instance.displayer:getCh(13):GetRoot()
    --             return MainPlayer.root_node
    --         end
    --     )
    -- end
end

function StoryInfoShowBoss3:StoryEvent(type, params)
    if StoryConfig.CustomEventType.ShowBoss == type then
        self.data.story_event:Set{id = self.info.id}
    end
end

function StoryInfoShowBoss3:StoryEnd()
    ViewMgr:CloseView(StoryShowBossView)
    -- BattleData.Instance:BaseData().pause = false
    ViewMgr:MainViewVisible(true)
    BattleCtrl.SendPrepareFinish()
end

function StoryInfoShowBoss3:NeedTrigger()
    return self:MonsterGroupIdVerify()
end
