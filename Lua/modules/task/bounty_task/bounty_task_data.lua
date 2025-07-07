
BountyTaskData = BountyTaskData or BaseClass()

function BountyTaskData:__init()
    if BountyTaskData.Instance ~= nil then
        Debuger.LogError("[BountyTaskData] attempt to create singleton twice!")
        return
    end
    BountyTaskData.Instance = self
    -- self:DuChuangZeiYingConfig()
    -- self:DuChuangZeiYingData()

    -- Cfg.Register("DuChuangZeiYingLayerBySceneId",Cfg.RealTimeSearch(Config.bandit_camp_auto.bandit_camp, "scene_id"))

    self:BountyTaskData()
end

function BountyTaskData:BountyTaskData()

end

function BountyTaskData:ClearCacheBountyTask()
    if nil ~= self.effect_array then
        SE:Stop(self.effect_array)
        self.effect_array = nil
    end
end

function BountyTaskData:PlayEffectArray(is_actived)
    if is_actived then
        local pos = string.split(Config.bounty_tasklist_auto.other[1].array_coordinate, "|")
        pos = SceneCtrl.Instance:CurScene():PosToClint(tonumber(pos[1]), tonumber(pos[2]))
        self:ClearCacheBountyTask()
        self.effect_array = SE:Play(SceneEffectConfig.EffectList.BountyTaskArrayEffect2, pos)
    elseif nil == self.effect_array then
        local pos = string.split(Config.bounty_tasklist_auto.other[1].array_coordinate, "|")
        pos = SceneCtrl.Instance:CurScene():PosToClint(tonumber(pos[1]), tonumber(pos[2]))
        self.effect_array = SE:Play(SceneEffectConfig.EffectList.BountyTaskArrayEffect1, pos)
    end
end

