BattleShotEffect = BattleShotEffect or BaseClass()
function BattleShotEffect:__init(shot_id, end_func, play)
    self.is_done = false
    self.is_loaded = false
    self.shot_effect = nil
    self.shot_id = shot_id
    self.end_func = end_func
    ResMgr:LoadGameObject(
        DrawerHelper.GetStory(self.shot_id),
        function(obj)
            if obj then
                self.shot_effect = obj
                self.is_loaded = true
                if play == true then
                    self:Play()
                end
            end
        end,
        true,
        false
    )
end
function BattleShotEffect:SetEndFunc(end_func)
    self.end_func = end_func
end
function BattleShotEffect:IsLoaded()
    return self.is_loaded
end
function BattleShotEffect:IsDone()
    return self.is_done
end
function BattleShotEffect:Play()
    if self.shot_effect then
        local mono = self.shot_effect:GetComponent(typeof(Game.TimelineEvent))
        mono:Play()
        mono.OnStopped = function()
            UnityGameObj.Destroy(self.shot_effect)
            self.is_done = true
            if self.end_func then
                self.end_func()
                self.end_func = nil
            end
        end
    else
        self.is_done = true
        if self.end_func then
            self.end_func()
            self.end_func = nil
        end
    end
end
