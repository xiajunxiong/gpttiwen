HundredGodBossData = HundredGodBossData or BaseClass()
function HundredGodBossData:__init()
    if HundredGodBossData.Instance ~= nil then
		Debuger.LogError("[HundredGodBossData] attempt to create singleton twice!")
		return
	end
	HundredGodBossData.Instance = self
    self.all_config = Config.hundred_god_boss_auto
    self.target_cfg = self.all_config.target
    self.info = SmartData.New({
        is_finish = false,
        count = 0,
        reward_flag = {},
        time = 0,
    })
    self.fly_pos = nil
    self.cache_effect_played = {}
end
function HundredGodBossData:__delete()
    
end
function HundredGodBossData:Clear()
    self.cache_effect_played = {}
end
function HundredGodBossData:Info()
    return self.info
end
function HundredGodBossData:TargetConfig()
    return self.target_cfg
end
function HundredGodBossData:IsGet(id)
    return (self.info.reward_flag[id] or 0) == 1
end
function HundredGodBossData:Count()
    return self.info.count
end
function HundredGodBossData:IsFinish(info)
    local cond_type = info.cond_type
    local param = info.param
    if cond_type == 1 then
        return RoleData.Instance:GetRoleLevel() >= param
    elseif cond_type == 2 then
        return TaskData.Instance:GetTaskIsCompleted(param)
    elseif cond_type == 3 then
        return self.info.count >= param
    end
end
function HundredGodBossData:UnFinishTaskShow()
    local config = self.target_cfg
    for i, v in ipairs(config) do
        if not self:IsFinish(v) then
            return v.task_show
        end
    end
end
function HundredGodBossData:AllFinish()
    local config = self.target_cfg
    for i, v in ipairs(config) do
        if not self:IsFinish(v) then
            return false
        end
    end
    return true
end

function HundredGodBossData:ParnerId()
    return self.all_config.other[1].partner_id
end
function HundredGodBossData:FinalItem()
    return Item.Create({item_id = self.all_config.other[1].show_id})
end
function HundredGodBossData:CanGet(info)
    return self:IsFinish(info) and not self:IsGet(info.id)
end
function HundredGodBossData:GetRemindNum()
    local config = self.target_cfg
    for i, v in ipairs(config) do
        if self:CanGet(v) then
            return 1
        end
    end
    if self:AllFinish() and self.info.is_finish == false then
        return 1
    end
    return 0
end
function HundredGodBossData:TimeOut()
    if self.info.time == 0 then
        return true
    end
    return (self.info.time - TimeHelper.GetServerTime() <= 0) or self.info.is_finish
end
function HundredGodBossData:SetFlyPos(pos, first_open)
    self.fly_pos = ViewMgr:GetMainCanvasRect():InverseTransformPoint(pos)
    if first_open == true then
        if not self:CacheEffectPlayed(6) then
            GuideManager.Instance:Start(self:GuideId())
            self:CacheEffectPlayed(6, true)
        end
    end
end
function HundredGodBossData:StartPos()
    return self.fly_pos
end

function HundredGodBossData:PerTask()
    return self.all_config.other[1].prepose_task
end
function HundredGodBossData:GuideId()
    return self.all_config.other[1].guide_id
end
function HundredGodBossData:CacheEffectPlayed(id, set)
    if set then
        UnityPlayerPrefs.SetInt(PrefKeys.HundredGodBossGuide(id), 1)
        -- self.cache_effect_played[id] = true
    else
        return UnityPlayerPrefs.GetInt(PrefKeys.HundredGodBossGuide(id)) == 1--self.cache_effect_played[id]
    end
end