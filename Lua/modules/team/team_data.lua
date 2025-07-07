
TeamData = TeamData or BaseClass()

function TeamData:__init()
    if TeamData.Instance ~= nil then
		Debuger.LogError("[TeamData] attempt to create singleton twice!")
		return
    end
    TeamData.Instance = self

    self:TeamConfig()
    self:TeamData()

    -- 战斗开始时需要飘侠义值表现的话这里为非空
    -- {
    --     seq = 0,
    --     salary = 0,
    -- }
    self.battle_salary_data = nil

    --通过target_type和param1查找对应的TeamTarget配置
    -- Cfg.Register("TeamTypeTarget",Cfg.RealTimeSearch2Key(Config.team_cfg_auto.team_target,"team_type","param_1"))
    Cfg.Register("TeamTypeTargetAct",Cfg.RealTimeSearch(Config.team_cfg_auto.team_target,"act_type"))
    Cfg.Register("TeamTypeTargetTask",Cfg.RealTimeSearch(Config.team_cfg_auto.team_target,"task_id"))
    Cfg.Register("TeamTypeTargetId",Cfg.RealTimeSearch(Config.team_cfg_auto.team_target,"target_id"))
    TeamConfig.AutoTime = self.config_other.auto_time
end

function TeamData:__delete()
end

function TeamData:TeamConfig()
    self.config = Config.team_cfg_auto
    self.config_other = self.config.other[1]
    self.config_recovery = self.config.recovery
    self.config_team_type = self.config.team_type
    self.config_team_type2 = self.config.team_type2
    self.config_team_target = self.config.team_target
    self.config_level_section = self.config.level_section

    self.config_sundries = Config.sundries_auto
    self.config_captain_salary = self.config_sundries.captain_salary
    self.config_chivalrous_value = self.config_sundries.chivalrous_value
    self.config_constant = self.config_sundries.constant[1]

    self.team_goal_list = {}
    self.team_type_list = {}
    self.team_type2_list = {}
    self.team_goal_type_list = {}
    for _, v in pairs(self.config_team_type) do
        self.team_type_list[v.team_type] = v
        self.team_goal_type_list[v.team_type] = {}
    end
    for _, v in pairs(self.config_team_type2) do
        self.team_type2_list[v.team_type] = v
    end
    for _, v in pairs(self.config_team_target) do
        if TeamConfig.TeamTargetType.EliteFB == v.team_type or TeamConfig.TeamTargetType.RareFB == v.team_type or TeamConfig.TeamTargetType.EliteFBFly == v.team_type then
            v.desc_fb = string.split(v.desc, " ")[1]
        end 
        self.team_goal_list[v.target_id] = v
        self.team_goal_type_list[v.team_type][v.target_id] = v
        if 1 == v.target_id then
            self.cur_sel_target = v
        end
    end

    self.salary_list = {
        [TeamConfig.SalaryType.salary] = SmartData.New(),
        [TeamConfig.SalaryType.knightly] = SmartData.New(),
    }
    for _, v in pairs(self.config_captain_salary) do
        self.salary_list[TeamConfig.SalaryType.salary][v.fuben_id] = {
            info = v,
            vo = {
                times = 0,
                total_salary = 0,
            }
        }
    end
    for _, v in pairs(self.config_chivalrous_value) do
        self.salary_list[TeamConfig.SalaryType.knightly][v.fuben_id] = {
            info = v,
            vo = {
                times = 0,
                total_salary = 0,
            }
        }
    end

    self.team_invited_list = SmartData.New()
end

function TeamData:ResetCurGoalGoal()
    self.cur_goal_goal:Set(self.team_goal_list[next(self.team_goal_type_list[self.cur_goal_type.team_type])])
end

function TeamData:ResetCurGoalLevel()
    self.cur_goal_level_low = self.cur_goal_goal.low_level
    self.cur_goal_level_high = self.cur_goal_goal.high_level
end

function TeamData:ReplaceGoalRealToCur()
    self.real_goal_type = self.cur_goal_type:Val()
    self.real_goal_goal = self.cur_goal_goal:Val()
    self.real_goal_level_low = self.cur_goal_level_low
    self.real_goal_level_high = self.cur_goal_level_high
end

function TeamData:ReplaceGoalCurToReal()
    self.cur_goal_type:Set(self.real_goal_type)
    self.cur_goal_goal:Set(self.real_goal_goal)
    self.cur_goal_level_low = self.real_goal_level_low
    self.cur_goal_level_high = self.real_goal_level_high
end

function TeamData:GetRealGoalData()
    return self.real_goal_goal
end

function TeamData:TeamData()
    self.in_team = false
    self.is_leader = false
    self.is_cross = false
    self.is_follow = false
    self.need_auto_match = false
    self.is_auto_match = SmartData.New{
        value = false
    }
    self.member_index = -1
    self.platform_click_num = 0

    self.dragon_task_tips = true

    self.team_list = SmartData.New()
    self.team_cache_list = {}
    
    self.team_info = SmartData.New(TeamInfo.New())
    self.enter_fb_team_info_list = SmartData.New()
    self.enter_fb_info = SmartData.New()

    self.team_approve_list = {}
    self.team_approve_cache_list = {}

    self.cur_goal_type = SmartData.New()
    self.cur_goal_goal = SmartData.New()

    self.invite_type = SmartData.New{ val = TeamConfig.InviteType.friend }
    self.sort_index_first = SmartData.New{ val = -1 }

    self.new_approve_red_point = SmartData.New{ val = false }

    self.salary_info = SmartData.New{ total = 0 }
    self.knightly_info = SmartData.New{ total = 0, today = 0, today_a = self.config_constant.chivalrous}

    self.flush_menu = SmartData.New{ val = false }
    self.menu_arrow = SmartData.New{
        state = -1,
        arrow = true
    }
    self.heal_menu = SmartData.New{
        type = 0,
        member = 0,
        object = 0,
    }

    self.platform_fb_goal = SmartData.New()

    self.notice_time = SmartData.New{
        val = 0,
    }
    self.platform_click_num = 0
    self.target_tips = {}
    self.out_team_func = nil
    self.team_invite_flush = SmartData.New()
    self.team_invite_info = nil
end

function TeamData:ClearData()
    TimeHelper:CancelTimer(self.handle_delay_check)
    self.in_team = false
    self.is_leader = false
    self.is_cross = false
    self.is_follow = false
    self.need_auto_match = false
    self.is_auto_match:Set{value = false}
    self.member_index = -1

    self.dragon_task_tips = true

    self.team_list:Set{}
    self.team_cache_list = {}
    
    self.team_info:Set(TeamInfo.New())
    self.enter_fb_team_info_list:Set{}
    self.enter_fb_info:Set{}

    self.team_approve_list = {}
    self.team_approve_cache_list = {}

    self.cur_goal_type:Set{}
    self.cur_goal_goal:Set{}

    self.invite_type:Set{val = TeamConfig.InviteType.friend}
    self.sort_index_first:Set{ val = -1 }

    self.new_approve_red_point:Set{ val = false }

    self.salary_info:Set{ total = 0 }
    self.knightly_info:Set{ total = 0, today = 0, today_a = self.config_constant.chivalrous}

    self.battle_salary_data = nil
    self.target_tips = {}
    self:CancelRecheckAutoTargetTimer()
    self.out_team_func = nil
end

function TeamData:SetTeamList(protocol)
    for _, item in pairs(self.team_list) do
        table.insert(self.team_cache_list, item)
    end
    self.team_list:Set{}
    
    self.team_list_empty = true
    for _, info in pairs(protocol.team_list) do
        self.team_list_empty = false
        local index = info.team_index
        if next(self.team_cache_list) then
            self.team_list[index] = table.remove(self.team_cache_list, #self.team_cache_list)
        else
            self.team_list[index] = TeamInfo.New()
        end
        self.team_list[index]:TeamVo(info)
    end
end

function TeamData:SetTeamInfo(protocol)
    self.team_info:TeamVo(protocol.info)
    self.team_info:CheckMemberPosList()
    self.team_index = protocol.info.team_index
    local in_team = self.in_team
    self.in_team = true
    local leader = self.team_info.leader
    local role_id = RoleData.Instance:GetBaseData().role_id
    local is_leader = self.is_leader
    -- LogDG(role_id, is_leader, leader.info.uid)
    self.is_leader = leader.info.uid == role_id
    RoleData.Instance:SetIsLeader(self.is_leader)
    
    self.is_cross = self.team_info:IsCross()
    if not in_team then
	    ViewMgr:CloseView(TeamPlatformView)
	    ViewMgr:CloseView(ChatView)
        ChatData.Instance:EmptyChannelTeamChatList()
        MainViewData.Instance:ToggleTTTeam()
        if self.is_leader and ((self.platform_click_num or 1) > 1 or self.team_info.info.team_type > 1) then
            TeamCtrl.Instance:SendTeamRecruit()
            self.platform_click_num = 1
        end
    end
    if self.is_leader then
        if not is_leader then
            TeamCtrl.Instance:SendTeamLeaderNpcReq(TeamConfig.TeamLeaderNpcOperType.init, VoMgr:NpcVoList())
        end
        if self.need_auto_match and not self.is_auto_match.value then
            TeamCtrl.Instance:SendTeamMatch()
            self.need_auto_match = false
        end
        FormationCtrl.Instance:FlushFormation0()
    else
        SceneLogic.Instance:ClearMoveCache()
    end
    if is_leader and not self.is_leader then
        self:ClearApproveList()
        VoMgr:ClearNpcLeader()
    end
    local member_list = self.team_info.member_list

    self.member_index = -1
    self.member_num = 0
    self.member_num_role = 0
    self.member_num_role_real = 0
    for i = 1, TeamConfig.TEAM_MEMBER_MAX do
        local info = member_list[i].info
        if nil ~= info then
            self.member_num = self.member_num + 1
            if 0 == info.is_partner then 
                self.member_num_role = self.member_num_role + 1 
                if info.uid > 100000 and info.uid ~= role_id then
                    self.member_num_role_real = self.member_num_role_real + 1
                end
            end
        end
        if nil ~= info and role_id == info.uid then
            self.is_follow = info.is_follow == 1
            self.member_index = info.member_index
        end
    end
    self:ReplaceGolaReal(protocol.info)
    
    if MainPlayer then
        MainPlayer:FlushIsLeader()
    end
    RoleData.Instance:GetBaseData().is_in_team = self.in_team
    RoleData.Instance:GetBaseData().is_follow = self.is_follow
    -- MainPlayer:FlushFollowPet()
    SceneCtrl.Instance:Objs():CheckForVisbleRoleSetting()
    SceneCtrl.Instance:Objs():SetTouchAbleAboutAnswer()

    if self.is_auto_match.value then --人数达标停止匹配
        local target_id = self.team_info.info.team_type
        local target = TeamData.Instance:GetTeamGoalWithId(target_id)
        if target ~= nil and target.stop_match_num > 0 and self.member_num_role >= target.stop_match_num then
            TeamCtrl.Instance:SendTeamMatch(nil, nil, TeamConfig.MatchType.cancel)
        end
    end

    if self.is_leader then
        self:AutoTarget()
        TimeHelper:CancelTimer(self.handle_delay_check)
        self.handle_delay_check = TimeHelper:AddDelayTimer(function ()
            self:AutoTarget2()
        end, 0.5)
    end

    ShengQiCtrl.Instance:FlushShengQiFollow()
end

function TeamData:ReplaceGolaReal(info)
    self.real_goal_goal = self:GetTeamGoalWithId(info.team_type)
    self.real_goal_type = self:GetTeamTypeWithType(self.real_goal_goal.team_type)
    self.real_goal_level_low =  info.limit_level_low
    self.real_goal_level_high = info.limit_level_high
end

function TeamData:SetOutOfTeam(protocol)
    if self.out_team_func then
        self.out_team_func()
        self.out_team_func = nil
    end
    self:ClearApproveList()
    self.team_info:OutOfTeam()
    self.team_index = nil
    self.target_tips = {}
    self:CancelRecheckAutoTargetTimer()
    self.in_team = false
    self.is_leader = false
    self.is_follow = false
    self.is_auto_match.value = false
    self.member_index = -1
    self.member_num = 0
    self.member_num_role = 0
    self.member_num_role_real = 0
    self.dragon_task_tips = true
    ChatData.Instance:EmptyChannelTeamChatList()
    RoleData.Instance:SetIsLeader(false)
    MainPlayer:FlushIsLeader()
    RoleData.Instance:GetBaseData().is_in_team = self.in_team
    RoleData.Instance:GetBaseData().is_follow = self.is_follow
    VoMgr:ClearNpcLeader()
    SceneCtrl.Instance:Objs():SetTouchAbleAboutAnswer()
    self.cur_platform_target_id = nil
    -- MainPlayer:FlushFollowPet()
    ShengQiCtrl.Instance:FlushShengQiFollow()
end

function TeamData:AddApproveMember(protocol)
    local join_info = protocol.join_info
    local uid = join_info.uid

    if nil == self.team_approve_list[uid] then
        if next(self.team_approve_cache_list) then
            self.team_approve_list[uid] = table.remove(self.team_approve_cache_list, #self.team_approve_cache_list)
        else
            self.team_approve_list[uid] = TeamMemberInfo.New()
        end
        self.team_approve_list[uid]:TeamMemberInfo(join_info)
    else
        self.team_approve_list[uid]:TeamMemberInfo(join_info)
    end
    self:NewApproveRedPoint(true)
end

function TeamData:RemoveApproveMember(uid)
    if self.team_approve_list[uid] then
        table.insert(self.team_approve_cache_list, self.team_approve_list[uid])
		self.team_approve_list[uid] = nil
    end
    self:NewApproveRedPoint()
end

function TeamData:ClearApproveList()
    for uid, _ in pairs(self.team_approve_list) do
        table.insert(self.team_approve_cache_list, self.team_approve_list[uid])
    end
    self.team_approve_list = {}
    self:NewApproveRedPoint(false)
end

function TeamData:SetTeamMatchInfo(protocol)
    self.match_time = 0
    self.is_auto_match.value = protocol.is_match ~= 0
end

function TeamData:SetTeamInvitationNotice(protocol)
    local info = protocol.info
    self.team_invited_list[info.team_index] = TeamConfig.TeamInvitedNotice.remove ~= info.notice_type and info or nil
end

function TeamData:GetTeamInvitedList()
    local list = {}
    for _, info in pairs(self.team_invited_list:Val()) do
        table.insert(list, info)
    end
    table.sort(list, function (a, b)
        if not a or not b then return a end
        if a.add_chivalrous_value == b.add_chivalrous_value then
            local atg = TeamData.Instance:GetTeamGoalWithId(a.team_type)
            local btg = TeamData.Instance:GetTeamGoalWithId(b.team_type)
            local acs = atg and atg.chivalrous_seq or 999
            local bcs = btg and btg.chivalrous_seq or 999
            if acs == bcs then
                return a.is_cross > b.is_cross
            end
            return acs < bcs
        end
        return a.add_chivalrous_value > b.add_chivalrous_value
    end)
    return list
end

function TeamData:RemoveTeamInvitedInfoByTeamIndex(team_index)
    self.team_invited_list[team_index] = nil
end

function TeamData:TeamInvitedListFlush()
    for _, info in pairs(self.team_invited_list) do
        TeamCtrl.Instance:SendReqJoinTeamInfo(info.is_cross, info.team_index, info.leader.uid)
    end
end

function TeamData:TeamIndex()
    return self.team_index
end

function TeamData:InTeam()
    return self.in_team
end

function TeamData:MemberNum()
    return self.member_num
end

function TeamData:MemberNumRole()
    return self.member_num_role
end
function TeamData:MemberNumRoleReal()
    return self.member_num_role_real
end

function TeamData:IsLeader()
    return self.is_leader
end

function TeamData:IsCross()
    return self.is_cross
end

function TeamData:IsFollow()
    return self.is_follow
end

function TeamData:MemberIndex()
    return self.member_index
end

function TeamData:IsAutoMatch()
    return self.is_auto_match
end

function TeamData:GetTeamInfo()
    return self.team_info
end

function TeamData:GetEnterFBInfo()
    return self.enter_fb_info
end

function TeamData:GetEnterFBTeamInfoList()
    return self.enter_fb_team_info_list
end

function TeamData:NeedFollow()
    return self.in_team and self.is_follow
end

function TeamData:GetMemberList()
    local list = {}
    local member_list = self.team_info.member_list
    for i = 1, TeamConfig.TEAM_MEMBER_MAX do
        if member_list[i].info then
            list[i] = member_list[i]
        end
    end
    return list
end

function TeamData:GetMemberRoleList()
    local list = {}
    local member_list = self.team_info.member_list
    for i = 1, TeamConfig.TEAM_MEMBER_MAX do
        if member_list[i].info and member_list[i].info.is_partner == 0 then
            table.insert(list,member_list[i])
        end
    end
    return list
end

function TeamData:GetTeamList(cross_type)
    local list = {}
    self.team_list_empty = true
    for k, v in pairs(self.team_list) do
        if nil == cross_type then self.team_list_empty = false end
        if v:IsCross(cross_type) then
            self.team_list_empty = false
            list[k] = v
        end
    end
    return nil == cross_type and self.team_list or list
end

function TeamData:GetTeamApproveList()
    local list = DataHelper.TableCopy(self.team_approve_list)
    table.sort(list, function(a, b)
        return a.req_timestamp > b.req_timestamp
    end)
    return list
end

function TeamData:GetTeamTypeList()
    local list = {}
    for _, v in pairs(self.team_type_list) do
        for _, goal in pairs(self.team_goal_type_list[v.team_type]) do
            if self:CheckTeamTargetShow(goal, true) then
                table.insert(list,v)
                break
            end
        end
    end
    return list
end

function TeamData:GetTeamGoalListWithType(team_type, is_sort)
    local list = {}
    if is_sort then
        for _, v in pairs(self.team_goal_type_list[team_type]) do
            if self:CheckTeamTargetShow(v) then
                table.insert(list,v)
            end
        end
        table.sort(list, function (a, b)
            if not a or not b then return a end
            return a.index < b.index
        end)
    else
        list = self.team_goal_type_list[team_type]
    end
    return list
end

function TeamData:GetTeamLevelList()
    local list, t_low, t_high = {}
    local role_level = RoleData.Instance:GetRoleLevel()
    for level = self.cur_goal_goal.low_level, self.cur_goal_goal.high_level do
        local t = { level = level }
        list[level] = t
        t_low = level == role_level and t or t_low
        t_high = level == self.cur_goal_level_high and t or t_high
    end
    return list, t_low, t_high
end

function TeamData:GetTeamTypeWithType(team_type)
    return self.team_type_list[team_type]
end

function TeamData:GetTeamGoalWithId(target_id)
    return self.team_goal_list[target_id]
end

function TeamData:GetTeamPlatformShowList(target_id)
    local function new_parent(team_type2)
        return {
            name = self.team_type2_list[team_type2].team_target,
            children = {}
        }
    end
    local function new_detail(target)
        return {
            desc = Language.Team.Platform2Desc[target.team_type2][target.param_1],
            team_type2 = target.team_type2,
            param_1 = target.param_1,
            is_cross = target.is_cross,
            details = {},
        }
    end
    
    local list, cp, pp = {}, 0, 0
    local target = self.team_goal_list[target_id]

    for _, v in pairs(self.team_goal_list) do
        if self:CheckTeamTargetShow(v) then
            list[v.team_type2] = list[v.team_type2] or new_parent(v.team_type2)
            if Language.Team.Platform2Desc[v.team_type2] then
                if Language.Team.Platform2Desc[v.team_type2][v.param_1] then
                    list[v.team_type2].children[v.param_1] = list[v.team_type2].children[v.param_1] or new_detail(v)
                    table.insert(list[v.team_type2].children[v.param_1].details, v)
                else
                    list[v.team_type2].children[v.param_1] = v
                end
            else
                table.insert(list[v.team_type2].children, v)
            end
        end
    end

    -- 组队任务-远征任务特殊排序
    if list[3] and list[3].children and list[3].children[3] then
        table.sort(list[3].children[3].details, function (a, b)
            if not a or not b then return a end
            return a.index < b.index
        end)
    end
     -- 日常活动-精英试炼特殊排序
     if list[1] and list[1].children and list[1].children[2] then
        table.sort(list[1].children[2].details, function (a, b)
            if not a or not b then return a end
            return a.index < b.index
        end)
    end

    -- 团队秘境-上古遗迹特殊排序
    if list[4] and list[4].children and list[4].children[1] then
        table.sort(list[4].children[1].details, function (a, b)
            if not a or not b then return a end
            return a.index < b.index
        end)
    end
    

    if Language.Team.Platform2Desc[target.team_type2] and Language.Team.Platform2Desc[target.team_type2][target.param_1] then
        self.platform_fb_goal:Set(target)
        self.cur_sel_target = target
        target = list[target.team_type2].children[target.param_1]
    end

    for _, items in pairs(list) do
        table.sort(items.children, function (a, b)
            if not a or not b then return a end
            if a.param_1 == b.param_1 then
                return a.index < b.index
            else
                return a.param_1 < b.param_1
            end
        end)
    end

    for k, v in pairs(list) do
        for y, z in pairs(v.children) do
            if target == z then
                cp, pp = k, y
            end
        end
    end

    return list, target, cp, pp
end

function TeamData:CheckTeamTargetShow(target, quick)
    local value = not LoginData.Instance:IsOnCrossSever() or TeamConfig.CrossType.is_cross == target.is_cross
    value = value and 1 == target.is_show
    value = value and RoleData.Instance:GetRoleLevel() >= target.low_level
    value = value and FlyUpData.Instance:GetStage() >= target.is_feisheng
    if TeamConfig.TeamTargetType.YYAct == target.team_type then
        local activity_status = ActivityData.Instance:GetActivityStatusInfo(target.act_type)
        value = value and activity_status and ActStatusType.Open == activity_status.status
    end
    if quick then return value end
    -- if TeamConfig.TeamTargetType.LimitAct == target.team_type then
        -- if 0 ~= target.act_type then
        --     local is_complete2 = false
        --     local act_info = ActivityData.Instance:GetActivityInfo(target.act_type)
        --     if ActType.WorldBoss == target.param_1 then
        --         local act_info2 = ActivityData.Instance:GetActivityInfo(ActType.WorldBoss2)
        --         is_complete2 = nil ~= act_info2 and (not act_info2:TeamTargetShow()) or false
        --     end
        --     is_complete = (nil ~= act_info and (not act_info:TeamTargetShow()) or false) and is_complete2
        -- end
    -- end
    target.is_complete = TaskData.Instance:IsTeamTargetTaskComplete(target, true)
    return value
end

function TeamData:GetTeamPartnerList()
    local list = {}
    local partner_list = PartnerCtrl.Instance:Data():GetPartnerAllInfo()
    local lock_state = PartnerConfig.LockState
    local fight_state = PartnerConfig.FightState
    for _, info in pairs(partner_list) do
        local vo = info.vo
        if lock_state.unlock == vo.is_unlocked and fight_state.rest == vo.fight_state then
            table.insert(list, info)
        end
    end
    return list
end

function TeamData:CheckNoTeamPartnerOrder()
    local fight_order_list = PartnerCtrl.Instance:Data():GetPartnerFightOrderList()
    self.team_info:CheckNoTeamPartnerOrder(fight_order_list:Val())
end

function TeamData:SetTeamEnterFbRoute(protocol)
    local enter_fb_info = protocol.enter_fb_info
    enter_fb_info.agree_enter_total_time = enter_fb_info.agree_enter_end_time - TimeHelper.GetServerTime()
    self.enter_fb_info:Set(enter_fb_info)
    if 0 == enter_fb_info.enter_type then
        ViewMgr:CloseView(FightTeamView)
        ViewMgr:CloseView(FightTeamMusicGameView)
        ViewMgr:CloseView(FightTeamHarmonyHeavenView)
        return
	end
    self.enter_fb_team_info_list:Set({})

    --结义组免打开确认界面 自动确认
    --[[ 先去掉
        if SwornData.Instance:IsHaveSworn() then
            local is_sworn_member = true
            for i, v in ipairs(self.team_info:MemberListSort()) do
                if not SwornData.Instance:MemberInTeam(v.info.uid) then
                    is_sworn_member = false
                    break
                end
            end
            if is_sworn_member then
                TeamCtrl.Instance:SendTeamEnterFbRet(true)
                --万妖谷特殊处理 写死
                if enter_fb_info.enter_type == 2 and enter_fb_info.enter_type_param == 4 then
                    FightDenData.Instance:SetBattleMapId(FightDenData.Instance:GetLeaderLevel())
                end
                return
            end
        end
    ]]
    if 1 == enter_fb_info.enter_type and FB_TYPE.SHI_TU_CHUANG_GONG == enter_fb_info.enter_type_param then
        ViewMgr:OpenView(FightTeamMusicGameView)
    elseif 1 == enter_fb_info.enter_type and FB_TYPE.HARMONY_HEAVEN == enter_fb_info.enter_type_param then
        ViewMgr:OpenView(FightTeamHarmonyHeavenView)
    else
        ViewMgr:OpenView(FightTeamView)
    end
end

function TeamData:SetTeamEnterFbInfo(protocol)
    local enter_fb_team_info = protocol.enter_fb_team_info
    self.enter_fb_team_info_list[enter_fb_team_info.member_index] = enter_fb_team_info.is_determine
    self.enter_fb_team_info_list.is_refuse = TeamConfig.FbInfoTeamType.refuse == enter_fb_team_info.is_determine
end

function TeamData:GetInviteTypeList()
    local list = {}
    local type_names = Language.Team.InviteTypeNames
    if self.in_team or self.is_world_notice then
        table.insert(list , { type = TeamConfig.InviteType.friend, name = type_names[TeamConfig.InviteType.friend] })
        table.insert(list , { type = TeamConfig.InviteType.family, name = type_names[TeamConfig.InviteType.family] })
        -- if self.is_leader then
        --     table.insert(list , { type = TeamConfig.InviteType.partner, name = type_names[TeamConfig.InviteType.partner] })
        -- end
    else
        table.insert(list , { type = TeamConfig.InviteType.partner, name = type_names[TeamConfig.InviteType.partner] })
    end
    return list
end

function TeamData:SetTeamMemberAppearanceChange(protocol)
    self.team_info:MemberAppearanceChange(protocol.member_index, protocol.appearance)
end

function TeamData:SetTeamRemoveApply(protocol)
    self:RemoveApproveMember(protocol.remove_uid)
end

function TeamData:SetRoleSalaryInfo(protocol)
    for _, v in pairs(protocol.salary_list) do
        if self.salary_list[protocol.salary_type] and self.salary_list[protocol.salary_type][v.seq] then
            self.salary_list[protocol.salary_type][v.seq].vo = v
        else
            LogError(string.format("SetRoleSalaryInfo salary_type:%s seq:%s", protocol.salary_type, v.seq))
        end
    end

    if TeamConfig.SalaryType.salary == protocol.salary_type then
        self.salary_info.total = protocol.total_value
    elseif TeamConfig.SalaryType.knightly == protocol.salary_type then
        self.knightly_info.total = protocol.total_value
        self.knightly_info.today = 0
        for _, v in pairs(self.salary_list[TeamConfig.SalaryType.knightly]) do
            local vo = v.vo
            self.knightly_info.today = self.knightly_info.today + vo.total_salary
        end
    end
end

function TeamData:SetRoleSalaryDay(protocol)
    if TeamConfig.SalaryType.salary == protocol.salary_type then
        self.salary_info.day_val = protocol.day_value
    elseif TeamConfig.SalaryType.knightly == protocol.salary_type then
        self.knightly_info.day_val = protocol.day_value
    end
end

function TeamData:GetSalaryList()
    return self.salary_list[TeamConfig.SalaryType.salary]
end

function TeamData:GetSalaryInfo()
    return self.salary_info
end

function TeamData:GetKnightlyList()
    return self.salary_list[TeamConfig.SalaryType.knightly]
end

function TeamData:GetKnightlyInfo()
    return self.knightly_info
end

function TeamData:NewApproveRedPoint(approve_value)
    if nil ~= approve_value then
        self.new_approve_red_point.val = approve_value
        return
    end
    local arp = false
	for _, v in pairs(self.team_approve_list) do
		arp = true
	end
	self.new_approve_red_point.val = arp
end

function TeamData:SetUpdateMemberHPMP(protocol)
    self.team_info:UpdateMemberHPMP(protocol.role_info)
end

function TeamData:SetUpdateMemberPetHPMP(protocol)
    self.team_info:UpdateMemberPetHPMP(protocol.pet_info)
end

function TeamData:SetTeamLeaderNpcInfo(protocol)
    local tlnot = TeamConfig.TeamLeaderNpcOperType
    if tlnot.init == protocol.oper_type then
        VoMgr:ClearNpcLeader()
        for i = 1, #protocol.npc_list do
            VoMgr:AddNpcLeader(protocol.npc_list[i])
        end
    elseif tlnot.add == protocol.oper_type then
        for i = 1, #protocol.npc_list do
            VoMgr:AddNpcLeader(protocol.npc_list[i])
        end
    elseif tlnot.delete == protocol.oper_type then
        for i = 1, #protocol.npc_list do
            VoMgr:DeleteNpcLeader(protocol.npc_list[i])
        end
    end
end

function TeamData:SetTeamSimpleInfoChangeNotice(protocol)
    self.team_info:SetTeamSimpleInfoChangeNotice(protocol.info)
    self.is_cross = self.team_info:IsCross()
    self:ReplaceGolaReal(protocol.info)
end

function TeamData:SetTeamMemberBaseInfoChangeNotice(protocol)
    self.team_info:SetTeamMemberBaseInfoChangeNotice(protocol.member_info)
end

function TeamData:NeedFlushMenu()
    if self:InTeam() then return end
    self.flush_menu.val = not self.flush_menu.val
end

--获取队长信息
function TeamData:GetLeaderInfo()
    return self.team_info.leader.info
end

function TeamData:MenuArrow(state, pos)
    self.menu_arrow.state = state
    self.menu_arrow.pos = pos
end

function TeamData:HealCostGold()
    return self.config_other.recovery_need_gold
end

function TeamData:GetHealItems()
    local list = {}
    for k, v in pairs(self.config_recovery) do
        local num = Item.GetNum(v.item_id)
        if v.type == self.heal_menu.type and num > 0 then
            table.insert(list, Item.Init(v.item_id, num))
        end
    end
    return list
end

function TeamData:ResetHealMenu()
    self.heal_menu.type = 0
    self.heal_menu.member = 0
    self.heal_menu.object = 0
end

function TeamData:IsInFBTeam()
    return MainViewCtrl.Instance:IsInGameState(MainViewGameState.fight_crystal) 
        -- or MainViewCtrl.Instance:IsInGameState(MainViewGameState.fight_fantasy) 
        or MainViewCtrl.Instance:IsInGameState(MainViewGameState.madel_fb) 
        or MainViewCtrl.Instance:IsInGameState(MainViewGameState.advanced)
        or MainViewCtrl.Instance:IsInGameState(MainViewGameState.splace_anecdote) 
        -- or MainViewCtrl.Instance:IsInGameState(MainViewGameState.rush_dungeon)
end

function TeamData:HealType2ItemId()
    for k, v in pairs(self.config_recovery) do
        if v.type == self.heal_menu.type then
            return v.item_id
        end
    end
    return 0
end

function TeamData:MemberInTeam(name)
    if self:InTeam() == false then
        return false
    else
        local list = self:GetMemberList()
        for _, value in pairs(list) do
            if value.info.is_partner == 0 and value.info.name == name then
                return true
            end
        end
    end
    return false
end

function TeamData:NoticeTimer()
    TimeHelper:CancelTimer(self.notice_timer)
    local cd = ChatData.Instance:GetTeamInvitationCd(true)
    self.notice_time.val = cd + 1
    self.notice_timer = TimeHelper:AddCountDownTT(function ()
        self.notice_time.val = self.notice_time.val - 1
    end, function ()
        self.notice_time.val = 0
    end, cd)
end

function TeamData:ResetInvitedList()
    self.invited_list = {
        [TeamConfig.InviteType.partner] = {},
        [TeamConfig.InviteType.friend] = {},
        [TeamConfig.InviteType.family] = {},
        [TeamConfig.InviteType.shitu] = {},
    }
end

function TeamData:IsInInvitedList(invite_type, id)
    return self.invited_list[invite_type][id]
end

function TeamData:AddToInvitedList(invite_type, id)
    if nil ~= invite_type and nil ~= id and nil ~= self.invited_list[invite_type] then
        self.invited_list[invite_type][id] = true
    end
end

function TeamData:SetTeamItemCheckTarget(item_id)
    self.item_check = {}
    self.item_check.target = item_id
end

function TeamData:SetTeamItemCheckResult(protocol)
    self.item_check.member_list = protocol.member_item_check
end

function TeamData:GetTeamItemCHeckResult()
    if self.item_check == nil then return nil end 
    local result = true
    for k,v in pairs(self.item_check.member_list) do 
        if v.member_idx ~= -1 and v.item_num == 0 then 
            result = false
            break
        end 
    end 
    self.item_check.result = result

    return self.item_check
end

function TeamData:ClearItemCheck()
    self.item_check = nil
end

function TeamData:IsTeamTargetShowSprite(fuben_id)
    local kl = self.salary_list[TeamConfig.SalaryType.knightly]
    return nil ~= kl[fuben_id] and (kl[fuben_id].vo.times < kl[fuben_id].info.week_limit)
end

function TeamData:IsChallengeTaskTarget(target_id)
    local target = self.team_goal_list[target_id]
    return target and TeamConfig.TeamTargetType.ChallengeTask == target.team_type
end

function TeamData:SetCurSelTargetByAct(act_type)
    self.cur_sel_target = Cfg.TeamTypeTargetAct(act_type)
    self.cur_select_target = self.cur_sel_target
end

function TeamData:LimitLevelLow()
    return self.cur_select_target and self.cur_select_target.low_level or 0
end

function TeamData:GetSalaryIsComplete(type, value)
    local list = self.salary_list[type]
    if nil == list then return end
    local info = list[value]
    if nil == info then return end
    local total_salary = info.vo.total_salary
    local limit = info.info.week_limit * info.info.reward
    return total_salary >= limit
end

function TeamData:PushBattleSalaryDisplay(data)
    self.battle_salary_data = data
end

function TeamData:PopBattleSalaryDisplay()
    local re_val = self.battle_salary_data
    self.battle_salary_data = nil
    return re_val
end

-- 队伍中是否有人离队
function TeamData:IsHasNotFollow()
    if not self.in_team then
        return false
    end
    for k, v in pairs(self.team_info:MemberListSort()) do
        if not v:IsLeader() and not v:IsFollow() then
            return true
        end
    end
end

function TeamData:AutoTarget(ignore_tips_flag)
    if ViewMgr:IsOpen(LoadingView) or ViewMgr:IsOpen(RoleSelectView) then return end
    local target_id = self.team_info.info.team_type
    local co = Config.team_cfg_auto.remind_goto[target_id]
    if co and (ignore_tips_flag or nil == self.target_tips[co.type]) and self:MemberNumRole() >= co.goto_num then
        self.target_tips[co.type] = true
        local tips_func = TeamConfig.TARGET_TIPS_FUNC[co.type]
        if tips_func and tips_func(false) then
            self:CancelRecheckAutoTargetTimer()
            PublicPopupCtrl.Instance:DialogTips{
                content = string.format(Language.Team.TargetTips, co.goto_num),
                confirm = {
                    func =  function()
                        tips_func(true, target_id)
                        ViewMgr:CloseView(DialogTipsView)
                    end ,
                    time = 5
                },
                cancel = {
                    func = function()
                        -- if self.recheck_auto_target_timer == nil then
                        self.recheck_auto_target_timer = TimeHelper:AddDelayTimer(BindTool.Bind(self.RecheckAutoTarget,self),30)
                        -- end
                        ViewMgr:CloseView(DialogTipsView)
                    end
                }
            }
        end
    end
end
function TeamData:MemberNumOnlineAndFollow()
    if not self.in_team then
        return 0
    end
    for k, v in pairs(self.team_info:MemberListSort()) do
        if not v:IsLeader() and (not v:IsFollow() or not v:IsOnline()) then
            return 0
        end
    end
    return self:MemberNumRole()
end
function TeamData:AutoTarget2()
    if ViewMgr:IsOpen(LoadingView) or ViewMgr:IsOpen(RoleSelectView) or ViewMgr:IsOpen(WaitView) then return end
    if SceneData.Instance:GetMode() ~= SceneModeType.None then return end
    if self.team_info == nil or self.team_info.info == nil or self.team_info.info.team_type == nil then return end

    local target_id = self.team_info.info.team_type
    local co = Config.team_cfg_auto.remind_goto[target_id]
    if co and self:MemberNumOnlineAndFollow() >= co.goto_num then
        local tips_func = TeamConfig.TARGET_TIPS_FUNC[co.type]
        if tips_func then
            local flag, data = tips_func(false, target_id)
            if flag == false and data then                
                PublicPopupCtrl.Instance:DialogTips{
                    content = data.desc,
                    confirm = {
                        func =  function()
                            tips_func(true, target_id)
                            ViewMgr:CloseView(DialogTipsView)
                        end,
                    },
                    tnr = data.tnr,
                    trn_str = data.trn_str,
                }
            end
        end
    end
end

function TeamData:RecheckAutoTarget()
    self.recheck_auto_target_timer = nil
    if not self:InTeam() or not self:IsLeader() then
        -- self:TeamDataCancelRecheckAutoTargetTimer()
        return
    end
    self:AutoTarget(true)
end

function TeamData:CancelRecheckAutoTargetTimer()
    if self.recheck_auto_target_timer ~= nil then
        TimeHelper:CancelTimer(self.recheck_auto_target_timer)
        self.recheck_auto_target_timer = nil
    end
end

function TeamData:CheckSingle(func, desc)
    if self:InTeam() then
        self:PopupTips(func, desc)
    else
        if func then
            func()
        end
    end
end
function TeamData:PopupTips(func, desc)
    local info = {
        content = desc or Language.Team.SingleTip,
        confirm = {
            func = function()
                TeamCtrl.Instance:SendExitTeam()
                PublicPopupCtrl.Instance:CloseDialogTips()
                if func then
                    self.out_team_func = func
                    -- func()
                end
            end
        }
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end

--主界面弹窗
function TeamData:SetTeamInviteInfo(info)
    self.team_invite_info = info
    self.team_invite_flush:Notify()
end
function TeamData:GetTeamInviteInfo()
    return self.team_invite_info
end

function TeamData:IsCanSolo()
    local type = SceneData.Instance:GetMode() 
    
    return type == SceneModeType.FightFly2
end