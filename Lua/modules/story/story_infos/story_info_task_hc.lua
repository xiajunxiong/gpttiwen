
StoryInfoTaskHC = StoryInfoTaskHC or BaseClass(StoryInfo)

function StoryInfoTaskHC:StoryInfo()
    -- self.trigger_type = StoryConfig.TriggerType.BattleStart
    self.ready_time = 1
end

function StoryInfoTaskHC:StoryReady()
    ViewMgr:MainViewVisible(false)
end

function StoryInfoTaskHC:StoryLoaded(obj)
end

function StoryInfoTaskHC:StoryDyLoad(wait_until_get_resource, res_type, res_id)
    if res_type == "MainRoleAnim" then
        wait_until_get_resource(
            function()
                return MainPlayer.body.main_body.obj
            end
        )
        -- tbt:SetTarget(self.root_node:FindChild("TopPoint").transform)
    elseif res_type == "MainRolePos" then
        wait_until_get_resource(
            function()
                return MainPlayer.root_node
            end
        )
    end
end

function StoryInfoTaskHC:StoryEvent(type, params)
    if StoryConfig.CustomEventType.ShowBoss == type then
        self.data.story_event:Set{id = self.info.id}
    end
end

function StoryInfoTaskHC:StoryEnd()
    ViewMgr:MainViewVisible(true)
end

function StoryInfoTaskHC:NeedTrigger()
    return StoryInfo.SceneIdVerify(108)
end