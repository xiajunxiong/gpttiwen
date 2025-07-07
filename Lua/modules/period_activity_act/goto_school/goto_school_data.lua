GotoSchoolData = GotoSchoolData or BaseClass()

function GotoSchoolData:__init()
    if GotoSchoolData.Instance ~= nil then
        Debuger.LogError("[GotoSchoolData] attempt to create singleton twice!")
        return
    end
    GotoSchoolData.Instance = self
    self.cfg = Config.go_to_school_auto
    self.info_data = SmartData.New({reward_times = 0})
    self.tip_data = SmartData.New({tip = GotoSchoolCtrl.Tip.None})

    Cfg.Register("GotoSchoolRoute",Cfg.RealTimeSearch2Key(self.cfg.route, "route_seq", "position"))
end

function GotoSchoolData:ClearData()
    if ViewMgr:IsOpen(GotoSchoolGameView) then
        ViewMgr:CloseView(GotoSchoolGameView)
    end
end

function GotoSchoolData:IsOpen()
    return false
end

function GotoSchoolData:NpcId()
    return self.cfg.other[1].npc_id
end

function GotoSchoolData:SceneId()
    return self.cfg.other[1].scene_id
end

-- 获取小孩配置
function GotoSchoolData:GetChildCfg(child_id)
    for i,v in pairs(self.cfg.children_type) do
        if v.children == child_id then
            return v
        end
    end
end

-- 随机拿一个小孩配置
function GotoSchoolData:GetRandChildCfg()
    local index = GameMath.Ramdon(1, #self.cfg.children_type)
    return self.cfg.children_type[index]
end

-- 获取路线配置
function GotoSchoolData:GetRouteCfg(route_seq, position)
    return Cfg.GotoSchoolRoute(route_seq, position)
end

-- 获取路线终点配置
function GotoSchoolData:GetEndRouteCfg(route_seq)
    local cfg
    for i,v in pairs(self.cfg.route) do
        if v.route_seq == route_seq then
            cfg = v
        end
    end
    return cfg
end

function GotoSchoolData:SetTip(tip)
    self.tip_data.tip = tip
end

-- 剩余奖励次数
function GotoSchoolData:GetRemainRewardCount()
    local count = self.info_data.reward_times or 0
    return self:RewardCountCfg() - count
end

-- 活动奖励次数
function GotoSchoolData:RewardCountCfg()
    return self.cfg.other[1].reward_times
end

-- 红点
function GotoSchoolData:RemindNum()
    if not self:IsOpen() then
        return 0
    end
    if self:GetRemainRewardCount() > 0 then
        return 1
    end
    return 0
end
