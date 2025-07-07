GuideDragon = GuideDragon or BaseClass()
function GuideDragon:__init()
    self.step = 0
end
function GuideDragon:__delete()
    -- statements
end
function GuideDragon:Start(member_change)
    --检查是否满足一条龙先决条件
    local team = TeamData.Instance:GetTeamInfo()
    if team.info == nil then
        return
    end
    --[[ if DEBUG_MODE then
        LogError("一条龙检查",team.info.team_type)
    end ]]

    local start_func = function (do_func)
        if team == nil or team.info == nil then
            return
        end
        --判断队伍类型
        local value = 0
        if team.info.team_type == 10 then
            self.step = 1
            --判断队伍人数
            local num = TeamData.Instance:MemberNumRole()
            local leader = TeamData.Instance:IsLeader()
            if num >= 3 and leader then
                self.step = 2
                local dragon_task = TaskData.Instance.task_trace_list[TaskConfig.TaskType.dragon]
                if CUSTOM_OBJID.FightDen == dragon_task then
                    value = CUSTOM_OBJID.FightDen
                    self:StartFightDen(do_func)
                    return value
                elseif CUSTOM_OBJID.FantasyFB == dragon_task then
                    value = CUSTOM_OBJID.FantasyFB
                    self:StartFantasyFB(do_func)
                    return value
                end
                local surplus = self:StartResearchTask(do_func)
                if surplus == 0 then
                    local count = self:StartFantasyFB(do_func)
                    if count >= 4 then
                        local pass_max = self:StartFightDen(do_func)
                        if pass_max then
                            self:StartFightDen(do_func)
                            value = CUSTOM_OBJID.FightDen
                        else
                            TeamCtrl.Instance:SendTeamMatch(nil,nil,TeamConfig.MatchType.cancel)
                        end
                    else
                        value = CUSTOM_OBJID.FantasyFB
                    end
                else
                    value = CUSTOM_OBJID.RESEARCH_TASK
                end
            end
        elseif team.info.team_type == 2 then
            self.step = 1
            --判断队伍人数
            local num = TeamData.Instance:MemberNumRole()
            local leader = TeamData.Instance:IsLeader()
            if num >= 3 and leader then
                self.step = 2
                local surplus = self:StartResearchTask(do_func)
                if surplus == 0 then
                    -- 做完了隐藏一条龙
                    -- LogError("三界做完了隐藏一条龙")
                    TaskData.Instance.task_trace_list[TaskConfig.TaskType.dragon] = nil
                    TeamCtrl.Instance:SendTeamMatch(nil,nil,TeamConfig.MatchType.cancel)
                    --[[ self.step = 3
                    local count = self:StartFantasyFB()
                    if count >= 4 then
                        self:StartFightDen()
                    end ]]
                    -- TaskData.Instance:TraceTask(TaskConfig.TaskType.dragon, CUSTOM_OBJID.RESEARCH_TASK)
                else
                    value = CUSTOM_OBJID.RESEARCH_TASK
                end
            end
        elseif team.info.team_type == 11 then
            local num = TeamData.Instance:MemberNumRole()
            local leader = TeamData.Instance:IsLeader()
            if num >= 3 and leader then
                local count = self:StartFantasyFB(do_func)
                if count >= 4 then
                    -- LogError("四象做完了隐藏一条龙")
                    TaskData.Instance.task_trace_list[TaskConfig.TaskType.dragon] = nil
                    TeamCtrl.Instance:SendTeamMatch(nil,nil,TeamConfig.MatchType.cancel)
                    --[[ local pass_max = self:StartFightDen()
                    if pass_max then
                        self:StartResearchTask()
                    end ]]
                else
                    value = CUSTOM_OBJID.FantasyFB
                end
                return value
            end
        elseif team.info.team_type == 5 then
            local num = TeamData.Instance:MemberNumRole()
            local leader = TeamData.Instance:IsLeader()
            if num >= 3 and leader then
                local pass_max = self:StartFightDen(do_func)
                -- local max_wave = FightDenData.Instance:GetMaxWave()
                if not pass_max then
                    -- LogError("万妖做完了隐藏一条龙")
                    TaskData.Instance.task_trace_list[TaskConfig.TaskType.dragon] = nil
                    TeamCtrl.Instance:SendTeamMatch(nil,nil,TeamConfig.MatchType.cancel)
                    --[[ local surplus = self:StartResearchTask()
                    if surplus == 0 then
                        self:StartFantasyFB()
                    end ]]
                else
                    value = CUSTOM_OBJID.FightDen
                end
            end
        end
        return value
    end
    
    if member_change then
        local value = start_func(false)
        if value > 0 and TeamData.Instance:MemberNumRole() >= 5 and TeamData.Instance.dragon_task_tips
        and not BattleData.Instance:IsServerFighting() then
            PublicPopupCtrl.Instance:DialogTips({
                content = string.format(Language.Task.DragConfirmShow, Language.Task.DragNames[value] or ""),
                confirm = {
                    func = function ()
                        TeamData.Instance.dragon_task_tips = false
                        ViewMgr:CloseView(DialogTipsView)
                        start_func(true)
                    end,
                    time = 5
                }
            })
        end
    else
        start_func(true)
    end
end
function GuideDragon:StartResearchTask(do_func)
    local surplus = ResearchTaskData.Instance:Time()
    if surplus ~= 0 then
        if do_func then
            self.step = 3
            local task = TaskData.Instance:GetTaskInfoByTaskId(CUSTOM_OBJID.RESEARCH_TASK)
            if task and task.is_accept_task then
                self.step = 4
                task:AutoTask()
            else
                self.step = 5
                BehaviorCtrl.ExeByCfgVo(BehaviorCtrl.GetCfg(5010))
            end
            TaskData.Instance:TraceTask(TaskConfig.TaskType.dragon, CUSTOM_OBJID.RESEARCH_TASK)
        end
    end
    return surplus
end
function GuideDragon:StartFightDen(do_func)
    local pass_wave = FightDenData.Instance:PassWave()
    local max_wave = FightDenData.Instance:GetMaxWave()
    if pass_wave < max_wave then
        if do_func then
            self.step = 7
            TaskData.Instance:TraceTask(TaskConfig.TaskType.dragon, CUSTOM_OBJID.FightDen)
            BehaviorCtrl.ExeByCfgVo(BehaviorCtrl.GetCfg(820))
        end
    else
        --[[ if DEBUG_MODE then
            LogError("打满万妖谷波数停了")
        end ]]
    end
    return (pass_wave < max_wave)
end

function GuideDragon:StartFantasyFB(do_func)
    local count = FightData.Instance:GetTodayFantasyFetch()
    if count < 4 then
        if do_func then
            self.step = 6
            TaskData.Instance:TraceTask(TaskConfig.TaskType.dragon, CUSTOM_OBJID.FantasyFB)
            if SceneModeType.FightFantasy ~= SceneData.Instance:GetMode() then
                BehaviorCtrl.ExeByCfgVo(BehaviorCtrl.GetCfg(5063))
            end
        end
   --[[  else
        local pass_wave = FightDenData.Instance:PassWave()
        local max_wave = FightDenData.Instance:GetMaxWave()
        if pass_wave < max_wave then
            self.step = 7
            TaskData.Instance:TraceTask(TaskConfig.TaskType.dragon, CUSTOM_OBJID.FightDen)
            BehaviorCtrl.ExeByCfgVo(BehaviorCtrl.GetCfg(820))
        end ]]
    else
        --[[ if DEBUG_MODE then
            LogError("打满了四象")
        end ]]
    end
    return count
end

function GuideDragon:Step(step)
    if step then
        self.step = step
    end
    return self.step
end

function GuideDragon:TeamType()
    local team = TeamData.Instance:GetTeamInfo()
    if team.info == nil then
        return 0
    end
    return team.info.team_type
end