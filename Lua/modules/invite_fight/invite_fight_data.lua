InviteFightData = InviteFightData or BaseClass()
InviteFightData.State = {
    Start = 1,
    CanBegin = 2,
    Fighting = 3,
    WaitGetReward = 4,
    End = 5,
}
function InviteFightData:__init()
    if InviteFightData.Instance ~= nil then
        Debuger.LogError("[InviteFightData] attempt to create singleton twice!")
        return
    end
    InviteFightData.Instance = self

    self.robot = SmartData.New({
        name = "",
        -- avatar_type = 0,
        appearance = {},
    })

    self.base = SmartData.New(
        {
            begin_time = 0,             --邀战开启时间
            end_time = 0,               --邀战结束时间
            state = InviteFightData.State.Start,--模块状态
            ret_score = 0,              --邀战结束时玩家战力
            ret_robot_score = 0,        --邀战结束时机器人战力
        }
    )
    self.is_waiting_data = false
end

function InviteFightData:__delete()
    InviteFightData.Instance = nil
end

function InviteFightData:OnSCCapacityTargetInfo(protocol)
    self.robot.name = protocol.role_name
    -- self.robot.avatar_type = protocol.avatar_type

    local app = {}
    app.advance_time = protocol.advance_times 
    app.avatar_type = protocol.avatar_type
    app.color = protocol.color
    app.prof_base = protocol.prof
    app.weapon_id = protocol.weapon_id
    app.sub_weapon_id = protocol.sub_weapon_id
    app.avatar_id = protocol.avatar_id
    self.robot.appearance = app


    self.base.begin_time = protocol.capacity_target_begin_time
    self.base.end_time = protocol.capacity_target_end_time
    self.base.ret_score = protocol.capacity_target_value
    self.base.ret_robot_score = protocol.capacity_target_robot_value
    if protocol.capacity_target_is_fetch_reward then
        self.base.state = InviteFightData.State.End
    elseif protocol.capacity_target_is_finish then
        self.base.state = InviteFightData.State.WaitGetReward
    elseif protocol.capacity_target_end_time > TimeHelper:GetServerTime() then
        self.base.state = InviteFightData.State.Fighting 
    elseif protocol.capacity_target_can_start then
        self.base.state = InviteFightData.State.CanBegin
    else
        self.base.state = InviteFightData.State.Start
    end
end

function InviteFightData:EndTime()
    return self.base.end_time
end

function InviteFightData:CurRobotScore()
    if self:CurState() < InviteFightData.State.WaitGetReward then
        local fight_time = TimeHelper:GetServerTime() - self.base.begin_time
        if fight_time < 0 then fight_time = 0 end
        local robot_score_cfg = Config.robot_auto.invite_war
        for i = #robot_score_cfg,1,-1 do
            local cfg = robot_score_cfg[i]
            if fight_time >= cfg.challenge_time then
                return cfg.robot_rank
            end
        end
    else
        return self.base.ret_robot_score
    end
    return 0
end

function InviteFightData:VectorRewardCfg()
    local cfg = Config.robot_auto.other[1].victor_reward
    local item_list = {}
    for _,v in pairs(cfg) do
        table.insert(item_list,Item.Create(v))
    end
    return item_list
end

function InviteFightData:FailRewardCfg()
    local cfg = Config.robot_auto.other[1].fail_reward
    local item_list = {}
    for _,v in pairs(cfg) do
        table.insert(item_list,Item.Create(v))
    end
    return item_list
end

function InviteFightData:Robot()
    return self.robot
end

function InviteFightData:Base()
    return self.base
end

function InviteFightData:CurState()
    return self.base.state
end


function InviteFightData:PlayerScore()
    local score = self:CurState() <= InviteFightData.State.WaitGetReward and 
        RoleData.Instance:GetSumScore() or self.base.ret_score
    return score
end

function InviteFightData:MarkWaitData()
    self.is_waiting_data = true
end

function InviteFightData:CheckAndClearkWaitDataFlag()
    local re_val = self.is_waiting_data
    self.is_waiting_data = false
    return re_val
end