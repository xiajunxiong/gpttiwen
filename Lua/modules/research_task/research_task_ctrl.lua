RESEARCH_TASK_OPERATE_TYPE = {
    ACCEPT = 0,
    COMMIT = 1,
    ONE_KEY = 2
}

FILE_REQ("modules/research_task/research_task_data")
--真中研究
ResearchTaskCtrl = ResearchTaskCtrl or BaseClass(BaseCtrl)

local npcId = 40106

function ResearchTaskCtrl:__init()
    if ResearchTaskCtrl.Instance ~= nil then
        Debuger.LogError("[ResearchTaskCtrl] attempt to create singleton twice!")
        return
    end
    ResearchTaskCtrl.Instance = self
    self.data = ResearchTaskData.New()
    self:RegisterProtocol(CSResearchTaskRequest)
    self:RegisterProtocol(CSResearchTaskAuto)
    self:RegisterProtocol(SCResearchTaskInfo, "RecvResearchTaskInfo")
    self:RegisterProtocol(SCResearchTaskNoticInfo, "OnResearchTaskNoticInfo")
    ActivityData.Instance:CustomClickHandle(ActType.ResearchTask, BindTool.Bind(self.GotoResearchTask, self))
    BehaviorData.Instance:CustomNpcTalkBtnsId(npcId, BindTool.Bind(self.SetupBehaviourBtn, self), true)
end

function ResearchTaskCtrl:GotoResearchTask(source_view)
    local act = ActivityData.Instance:GetActivityInfo(ActType.ResearchTask)
    if act and act.info and act.info.fun_name then
        -- if TeamData.Instance:InTeam() then
            BehaviorCtrl.ExeByCfgVo(BehaviorCtrl.GetCfg(act.info.fun_name, true))
        -- else
        --     if source_view == ActiveDailyView then
        --         GuideManager.Instance:Start(ConstGuideId.DailyActResearchTask)
        --     end
        --     TeamData.Instance.auto_sel_target = true
        --     TeamData.Instance.cur_platform_target_id = 2
        --     ViewMgr:OpenView(TeamPlatformView)
        -- end
    end
    TaskData.Instance:StopTaskState(TaskConfig.TaskType.dragon, CUSTOM_OBJID.RESEARCH_TASK)
end

function ResearchTaskCtrl:__delete()
    TimeHelper:CancelTimer(self.handle_run1)
    TimeHelper:CancelTimer(self.handle_run2)
    if self.handle_runer then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runer)
        self.handle_runer = nil
    end
end

function ResearchTaskCtrl:OnUnloadGameLogic()
    TimeHelper:CancelTimer(self.handle_run1)
    TimeHelper:CancelTimer(self.handle_run2)
    if self.handle_runer then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runer)
        self.handle_runer = nil
    end
end

function ResearchTaskCtrl:SetupBehaviourBtn(btn_list, obj_vo)
    if obj_vo and obj_vo.id == npcId and btn_list then
        local data = ActiveData.Instance:GetActiveConfig(2)
        if data == nil or RoleData.Instance:CreatedDay() < data.open_day then
            return {}
        end
        for i,v in ipairs(btn_list) do
            if v.detail_info ~= nil then
                self:SetupBehaviourBtnDetail(btn_list[i])
                break
            end
        end
    --[[ table.insert(btn_list,NpcDialogBehBtnData.FuncCreate(
            -- 判断有没有队伍 或者人数不足
            Language.Task.TaskBtn.OneKey,function()
                -- 弹出消耗元宝
                if self.data:Time() <= 0 then
                    PublicPopupCtrl.Instance:Center(ErrorText[73])
                    return
                end
                local reward = self.data:RewardNum()
                PublicPopupCtrl.Instance:DialogTips({
                    content = string.format(ErrorText[72], self.data:Cost()*self.data:Time(),self.data:Time(),reward[2],reward[1]),
                    confirm = {
                        func = function ()
                            self:SendResearchTaskReq({op_type = RESEARCH_TASK_OPERATE_TYPE.ONE_KEY})
                            PublicPopupCtrl.Instance:CloseDialogTips()
                        end
                    }
                })
            end)) ]]
    end
end

function ResearchTaskCtrl:SetupBehaviourBtnDetail(params)
        -- 找回表的固定显示（Config.day_reward_auto.find_reward
        -- local info = RewardData.Instance:GetFindConfig(FindType.ResearchTask)
        -- local cost = self.data:CostType() == 0 and info.coin_price or info.gold_price
        -- 21821修改为直接用价格表（Config.day_reward_auto.find_price
        local cost = RewardData.Instance:GetRewardActivityBackFindPrice(FindType.ResearchTask)
        cost = cost * self.data:Time()
        --对第一个按钮进行修改
        params.detail_info.act_type = ActType.ResearchTask
        params.detail_info.custom_type = DetailInfo1Custom.back
        params.detail_info.custom_show = Language.Task.TaskBtn.OneKey
            --NpcDialogData.GetDetailInfoBtnBackName(self.data:Cost() * self.data:Time())
        if self.data:Time() == 0 then
            params.detail_info.custom_func = function ()
                PublicPopupCtrl.Instance:Center(ErrorText[73])
            end
        else
        params.detail_info.custom_func =
            BindTool.Bind(
            ViewManager.OpenView,
            ViewMgr,
            DialogTipsBackView,
            {
                cost = cost,
                cost_type = self.data:CostType(),
                name = Language.Task.ResearchTask,
                rewards = self.data:Reward(),--params.detail_info.rewards,
                confirm_func = BindTool.Bind(
                    TaskCtrl.SendOnekeyFinish,
                    TaskCtrl.Instance,
                    OnekeyFinishType.ResearchTask
                )
            }
        )
    end
end

function ResearchTaskCtrl:CheckGuaJiAuto()
    local team = TeamData.Instance:GetTeamInfo()
    if team.info == nil then
        return
    end
    --判断队伍类型
    if team.info.team_type ~= 2 then
        return
    end
    SceneCtrl.Instance:CurScene().free_time = Time.realtimeSinceStartup
    --开始20秒检查
    TimeHelper:CancelTimer(self.handle_run1)
    TimeHelper:CancelTimer(self.handle_run2)
    self.handle_run1 =
        TimeHelper:AddRunTimer(
        function()
            local free_time = SceneCtrl.Instance:CurScene():FreeTime()
            local surplus = ResearchTaskData.Instance:Time()
            local level = Config.daily_activity_auto.daily[ActType.ResearchTask].level_restrict
            local role_level = RoleData.Instance:GetRoleLevel()
            local is_leader = TeamData.Instance:IsLeader()
            local is_guaji = SceneLogic.Instance:MoveCache().guaji
            if
                not is_guaji and is_leader and role_level > level and surplus == 0 and
                    TaskData.Instance:GetTaskInfoByTaskId(CUSTOM_OBJID.RESEARCH_TASK) == nil and
                    free_time >= 18
             then --秒后去挂机1
                FightData.Instance:AutoToGuaJi(true)
                SceneCtrl.Instance:CurScene().free_time = Time.realtimeSinceStartup
                --成功那就开启3分钟检查
                self.handle_run2 =
                    TimeHelper:AddRunTimer(
                    function()
                        free_time = SceneCtrl.Instance:CurScene():FreeTime()
                        if free_time >= 178 then
                            FightData.Instance:AutoToGuaJi(false)
                        end
                    end,
                    1,
                    180
                )
            end
        end,
        1,
        20
    )
end

function ResearchTaskCtrl:CheckDragon()
    --开始检查
    if self.handle_runer then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runer)
        self.handle_runer = nil
    end
    self.handle_runer = Runner.Instance:RunUntilTrue(function ()
        if MainViewData.Instance:BaseData().is_battle == false then
            local dragon = TeamCtrl.Instance:Dragon()
            if dragon then
                dragon:Start()
            else
                if DEBUG_MODE then
                    LogError("没有一条龙服务1111")
                end
            end
            --[[ if dragon and dragon:Step() > 2 and dragon:TeamType() == 10 then
                local count = FightData.Instance:GetTodayFantasyFetch()
                if count < 4 then
                    dragon:Step(6)
                    TaskData.Instance:TraceTask(TaskConfig.TaskType.dragon, CUSTOM_OBJID.FantasyFB)
                    BehaviorCtrl.ExeByCfgVo(BehaviorCtrl.GetCfg(5063))
                else
                    local pass_wave = FightDenData.Instance:PassWave()
                    local max_wave = FightDenData.Instance:GetMaxWave()
                    if pass_wave < max_wave then
                        dragon:Step(7)
                        TaskData.Instance:TraceTask(TaskConfig.TaskType.dragon, CUSTOM_OBJID.FightDen)
                        BehaviorCtrl.ExeByCfgVo(BehaviorCtrl.GetCfg(820))
                    end
                end
            end ]]
            return true
        end
    end)
end

function ResearchTaskCtrl:RecvResearchTaskInfo(protocol)
    -- LogDG("除魔任务下发", protocol)
    self.data:SetTdayFinishTimes(protocol.today_finish_times)
    --当天完成时重置一下检查时间
    --[[ if self.data:Time() == 0 then
        -- self:CheckGuaJiAuto()
        self:CheckDragon()
    end ]]
    ActivityData.Instance:SetActivityFinishInfo(
        ActType.ResearchTask,
        protocol.today_finish_times,
        self.data.other_info.daily_times
    )
    self.data:disposeScene()
    self.data.is_accept_task = false
     --先清理
    local vo = CommonStruct.MonsterVo(protocol.task_info)
    vo.client_show_id = protocol.task_info.monster_id
    vo.use_monster_cfg = true
    self.data:SetMonsterInfo(vo)
    -- if protocol.task_info.task_id > 0 then
    --     MainViewData.Instance:ToggleTTTask()
    --     vo.obj_id = CUSTOM_OBJID.RESEARCH_TASK
    --     vo.behavior_type = 12
    --     vo.param1 = BATTLE_MODE.BATTLE_MODE_ZHEN_ZHONG
    --     self.data:initScene(SceneData.Instance:SceneId())
    --     local task = self.data:TaskInfo()
    --     local taskvo = self.data:TaskVo(vo)
    --     --taskvo.is_new = SmartData.New{val = (protocol.is_new ~= 0)}
    --     task:Set(TaskInfoResearch.New():TaskInfo())
    --     task:TaskVo(taskvo, self.data.cache_task_id ~= 0 and protocol.task_info.pos_y ~= self.data.cache_task_id)
    --     task:IsNew(protocol.is_new ~= 0)
    --     TaskCtrl.Instance.data:TaskInfoUpdateByTaskId(CUSTOM_OBJID.RESEARCH_TASK, task)
    --     self.data.cache_task_id = protocol.task_info.pos_y
    -- else
    --     TaskCtrl.Instance.data:TaskInfoDeleteByTaskId(CUSTOM_OBJID.RESEARCH_TASK)
    -- end
    if nil ~= self.dargon_task_role_level_care then
        RoleData.Instance.base_data:Uncare(self.dargon_task_role_level_care)
    end
    if protocol.today_finish_times == self.data.other_info.daily_times then
        TaskCtrl.Instance.data:TaskInfoDeleteByTaskId(CUSTOM_OBJID.RESEARCH_TASK)
    else
        local is_accept_task = protocol.task_info.task_id > 0
        local monster_desc = ""
        if is_accept_task then
            vo.obj_id = CUSTOM_OBJID.RESEARCH_TASK
            vo.behavior_type = 12
            vo.param1 = BATTLE_MODE.BATTLE_MODE_ZHEN_ZHONG
            self.data:initScene(SceneData.Instance:SceneId())
            self.data.cache_task_id = protocol.task_info.pos_y
            local npc_co = Cfg.MonsterById(vo.client_show_id, false) or Cfg.NpcById(vo.client_show_id, false)
            monster_desc = string.format(Language.Task.ResearchTaskMonsterDesc, npc_co.name)
        end
        local act_info = ActivityData.Instance:GetActivityInfo(ActType.ResearchTask)
        local task_progress = string.format("(%s/%s)", protocol.today_finish_times, self.data.other_info.daily_times)
        local task = TaskInfoDragon.New():TaskInfo()
        local task_vo = CommonStruct.CustomTask()
        task_vo.task_id = CUSTOM_OBJID.RESEARCH_TASK
        task_vo.task_name = Language.Task.DragNames[CUSTOM_OBJID.RESEARCH_TASK] .. (is_accept_task and task_progress or "")
        task_vo.reward_list = ResearchTaskData.Instance:Reward2()
        local desc_show = is_accept_task and monster_desc or string.format("%s%s", Language.Task.DragDescs[CUSTOM_OBJID.RESEARCH_TASK], task_progress) 
        task_vo.task_desc = desc_show
        task_vo.task_goal = desc_show
        task:TaskVo(task_vo)
        task.is_accept_task = is_accept_task
        self.data.is_accept_task = is_accept_task
        self.dargon_task_role_level_care = task:DragonTaskParam(is_accept_task and function ()
            ResearchTaskCtrl.Instance:AutoTask()
            --[[ if TeamData.Instance:IsLeader() then
            elseif not TeamData.Instance:InTeam() then
                TeamCtrl.Instance:TeamDetectByActType(ActType.ResearchTask, function ()
                end, nil, 2)
            end ]]
        end or function ()
            -- 这里修改如果当前有队伍且是一条龙队伍才进行一条龙逻辑
            -- 否则只能组三界类型的队伍
            ResearchTaskCtrl.Instance:SendResearchTaskReq({op_type = RESEARCH_TASK_OPERATE_TYPE.ACCEPT})
            -- if TeamData.Instance:InTeam() then
            --     local info = TeamData.Instance:GetTeamInfo()
            --     if info.info.team_type == 10 and TeamData.Instance:MemberNumRole() >= 3 then
            --         if TeamCtrl.Instance.guide_dragon == nil then
            --             TeamCtrl.Instance.guide_dragon = GuideDragon.New()
            --         end
            --         TeamCtrl.Instance.guide_dragon:Start()
            --         --[[ if TeamCtrl.Instance.guide_dragon:TeamType() == 10 then
            --             TeamCtrl.Instance.guide_dragon:Start()
            --         end ]]
            --     else
            --         TeamCtrl.Instance:TeamDetectByActType(ActType.ResearchTask, function ()
            --         end, nil, 2)
            --         if TeamCtrl.Instance.guide_dragon == nil then
            --             TeamCtrl.Instance.guide_dragon = GuideDragon.New()
            --         end
            --         TimeHelper:CancelTimer(self.handle_delay)
            --         self.handle_delay = TimeHelper:AddDelayTimer(function ()
            --             TeamCtrl.Instance.guide_dragon:Start()
            --         end, 0.2)
            --     end
            -- else
            --     TeamCtrl.Instance:TeamDetectByActType(ActType.ResearchTask, function ()
            --     end, nil, 2)
            -- end
        end, act_info:LevelRestrict())
        TaskData.Instance:TaskInfoUpdateByTaskId(CUSTOM_OBJID.RESEARCH_TASK, SmartData.New(task), true)
        if is_accept_task then task:AutoTask() end
    end
    -- TaskData.Instance:DragonTaskTrace()
    ResearchTaskCtrl.Instance:AutoTask()
end

function ResearchTaskCtrl:SendResearchTaskReq(param_t)
    local protocol = GetProtocol(CSResearchTaskRequest)
    protocol.op_type = param_t.op_type or 0
    SendProtocol(protocol)
end

function ResearchTaskCtrl.SendResearchTaskAuto()
    local protocol = GetProtocol(CSResearchTaskAuto)
    SendProtocol(protocol)
end

function ResearchTaskCtrl:AutoTask()
    SceneLogic.Instance:AutoToNpc(self.data:MonsterInfo(), nil, true, true)
end

--三界除魔奖励下发
function ResearchTaskCtrl:OnResearchTaskNoticInfo(protocol)
    ViewMgr:OpenView(GetRewardView,{
        exp = protocol.add_exp,
        is_not_show = true,
        desc = Format(Language.ResearchTask.RewardTip,10),
        item_list = DataHelper.FormatItemList(protocol.item_list),
        show_check_btn = true,
    })
end