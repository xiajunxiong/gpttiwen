TaskData = TaskData or BaseClass()

function TaskData:__init()
    if TaskData.Instance ~= nil then
        Debuger.LogError("[TaskData] attempt to create singleton twice!")
        return
    end
    TaskData.Instance = self
    self:TaskConfig()
    self:TaskData()

    self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete,BindTool.Bind(self.RoleNoticeComplete, self))
    self.login_base_sm_care = LoginData.Instance:BaseData():Care(function (  )
        self.is_init_list = false
    end, "is_on_crosssever")
    
end

function TaskData:__delete()
    EventSys:UnBind(self.complete_handle)
    EventSys:UnBind(self.auto_task_chain_event)
    self.complete_handle = nil
    LoginData.Instance:BaseData():Uncare(self.login_base_sm_care)
end

function TaskData:TaskConfig()
    self.config = Config.tasklist_auto
    self.config_task_go_on = self.config.task_go_on
    self.chain_config_item_group = Config.task_chain_auto.item_group
    self.other_config = self.config.other[1]
	ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoTaoYuanYiSanShi,function()
            Cfg.UnRegister("TaskHelpOthersCoById")
            -- Cfg.Register("TaskHelpOthersCoById",Cfg.RealTimeSearch(Config.may_day_activity_auto.bravery_task,"task_id"))
            Cfg.Register("TaskHelpOthersCoById",Cfg.RealTimeSearch(Config.taoyuanyisanshi_auto.task_list,"task_id"))
            -- self.config_help_others = Config.may_day_activity_auto.bravery_task
            -- self.config_help_others_other = Config.may_day_activity_auto.other[1]
            self.config_help_others = Config.taoyuanyisanshi_auto.task_list
            self.config_help_others_other = Config.taoyuanyisanshi_auto.other[1]
            if TaskCtrl.Instance.protocol_help_others ~= nil then
                TaskCtrl.Instance:OnRAHelpOthersInfo(TaskCtrl.Instance.protocol_help_others)
            end
            Cfg.UnRegister("ShanHaiOriginTaskId")
            Cfg.Register("ShanHaiOriginTaskId", Cfg.RealTimeSearch(Config.shanhai_origin_auto.task_list,"task_id"))
            self.config_shanhai_other = Config.shanhai_origin_auto.other[1]
            if TaskCtrl.Instance.protocol_shanhai_info ~= nil then
                TaskCtrl.Instance:OnRAShanHaiOriginInfo(TaskCtrl.Instance.protocol_shanhai_info)
            end
        end,true)


    self.task_sub_list = {}
    for _, v in pairs(self.config_task_go_on) do
        self.task_sub_list[v.last_task] = self.task_sub_list[v.last_task] or {}
        table.insert(self.task_sub_list[v.last_task], v.word_task)
    end
    self.chain_task_reward_list = {}
    for _, v in pairs(self.chain_config_item_group) do
        self.chain_task_reward_list[v.reward_item.item_id] = v
    end

    self.task_guide_auto_list = {
        [TaskConfig.TaskGuideAutoType.mod_key] = {},
        [TaskConfig.TaskGuideAutoType.scene_mode] = {},
    }
    for k, v in pairs(Config.function_guide_auto.close_auto) do
        if nil ~= self.task_guide_auto_list[v.type] then
            self.task_guide_auto_list[v.type][v.param] = self.task_guide_auto_list[v.type][v.param] or {}
            self.task_guide_auto_list[v.type][v.param][v.task_id] = v
        end
    end

    Cfg.Register("ChainPetGroupByPI",Cfg.RealTimeSearch(Config.task_chain_auto.pet_group,"pet_id"))
    Cfg.Register("TaskListChainShowByGroupChapter",Cfg.RealTimeSearch2Key(Config.tasklist_auto.chain_show,"task_group","chain_chapter"))
end

function TaskData:TaskData()
    self.task_list = SmartData.New()
    self.task_wait_list = SmartData.New()
    self.task_cache_list = {}
    self.task_demand_list = {}

    self.cur_sel_task = SmartData.New()
    self.is_init_list = false

    -- 用于对比和监听
    self.task_id_list =
        SmartData.New(
        {
            accepted_list = SmartData.New(),
            completed_list = {},
            finish_list = {}
            --当前变化
        }
    )

    self.task_trace_list = SmartData.New()
    self.task_trace_list:Trace(7)
    self.task_temp_trace_list = SmartData.New()
    self.complete_list_effect = {}
    self.memu_effect_pos = SmartData.New {val = 0}
    self.menu_guide =
        SmartData.New {
        val = 0,
        arrow = true,
        pre_val = 0,
        is_show = true,
        is_flush = false,
    }
    self.menu_to_task = SmartData.New{
        to_task_id = 0,
    }
    self.menu_adventure = SmartData.New {}
    self.bounty_task_info = SmartData.New()
    self.chain_bounty_task_info = SmartData.New()
    self.bounty_task_progress = {
        cur_task_idx = 0,
        today_task_num = 10
    }
    self.task_chain_sub_continue = false
    self.task_chain_data = SmartData.New({cur = 0, total = 0, count = 0})
    self.task_ing_eff = SmartData.New({val = 0})

    self.complete_listeners = {}        --key:func(task_id),value:true or nil
    self.task_id_complete_listenters = {}   --key:task_id,value:{ handle({ task_ids, func(task_id) }) }
    self.bounty_task_breake_altar_stamp = 0

    self.branch_serial_task_sm = SmartData.New{
        info = SmartData.New{chain_chapter = 0}
    }
    self.branch_marbas_task_sm = SmartData.New{
        info = SmartData.New{chain_chapter = 0}
    }
    self.branch_relic_task_sm = SmartData.New{
        info = SmartData.New{chain_chapter = 0}
    }
end
function TaskData:Clear()
    EventSys:UnBind(self.auto_task_chain_event)
    self.task_trace_list:Set({})
    self.task_id_list:Set(
        {
            accepted_list = SmartData.New(),
            completed_list = {},
            finish_list = {}
            --当前变化
        }
    )
    self.task_list:Set {}
    self.task_wait_list:Set {}
    self.menu_adventure:Set {}
    self.is_init_list = false
    VoMgr:ClearNpc()
    self.task_chain_data.cur = 0
    self.task_chain_data.total = 0
    self.task_chain_data.count = 0
    self.chain_bounty_task_info:Set({})
end

function TaskData:TaskInfoGet()
    if next(self.task_cache_list) then
        return table.remove(self.task_cache_list, #self.task_cache_list):TaskInfo()
    else
        return TaskInfoNormal.New():TaskInfo()
    end
end

function TaskData:GetOtherCfg()
    return self.other_config
end

function TaskData:TaskInfoRecycle(smart_task)
    local task = smart_task:Val()
    table.insert(self.task_cache_list, task)
end

function TaskData:SetTaskDetailedInfoList(protocol)
    for key, value in pairs(protocol.task_id_accepted_list) do
        self.task_id_list.accepted_list[key] = value
    end
    local new_task, is_accpet
    for i = 1, protocol.task_count do
        local task_id = protocol.task_list[i].task_id
        local list_type_new = protocol.task_list[i].list_type
        if nil == self.task_list[task_id] then
            new_task = true
            self.task_list[task_id] = SmartData.New(self:TaskInfoGet())
        else
            local list_type_old = self.task_list[task_id].list_type
            if TaskConfig.ListType.accepted == list_type_new and TaskConfig.ListType.acceptable == list_type_old then
                is_accpet = true
            end
        end
        self.task_list[task_id]:TaskVo(protocol.task_list[i], {new_task = new_task, is_accpet = is_accpet, init_list = self.is_init_list})
        local task_type = self.task_list[task_id].task_type
        if TaskConfig.ListType.accepted == list_type_new then
            self:TraceTaskInit(task_type, task_id)
            if TaskConfig.TaskType.mainline == task_type then
                self.menu_guide.val = 0
            end
            if TaskConfig.TaskType.mainline == task_type and self:MainLineArrowFlag() and not self.task_list[task_id]:IsAutoTask()
             and self.task_list[task_id]:IsLevelEnough() then
                self.menu_guide.pre_val = task_id
                self.menu_guide.val = task_id
            end
            if TaskConfig.TaskType.mainline == task_type and self.task_list[task_id]:TaskMenuConditionPause() and RoleData.Instance:GetRoleLevel() == GuideData.Instance:GetTaskEffectLevel() then
               self:TaskIngEffect(0)
            end
            if TaskConfig.TaskType.adventure == task_type then
                self.menu_adventure:Set(self.task_list[task_id]:Val())
            end
            if TaskConfig.TaskType.prestige == task_type then
                PrestigeData.Instance:UpdatePrestigeTask()
            end
        end
    end
    self.is_init_list = true

    if self.task_id_list.accepted_list[MAIN_LINE_SP_TASK] then
        SceneModeNormal.CheckMainLine()
    end
    SceneModeNormal.CheckBountyTask()
end

function TaskData:SetTaskDeleteNotifyList(protocol)
    for i = 1, protocol.task_count do
        local task_id = protocol.task_list[i].task_id
        local reason = protocol.task_list[i].reason
        if nil ~= self.task_list[task_id] then
            if self.complete_list_effect[task_id] and 1 == reason then
                self.memu_effect_pos.val = self.complete_list_effect[task_id]
            end
            self:TaskInfoRecycle(self.task_list[task_id])
            self.task_list[task_id]:RemoveTask {is_remove = true, is_complete = 1 == reason}
            self.task_list[task_id] = nil
            --删除已接受任务中的任务
            self.task_id_list.accepted_list[task_id] = nil
            table.insert(ChallengeData.Instance.complete_task_list, task_id)
            ChallengeData.Instance:SetFinalTask()
            StoryCtrl.Instance:PlayStoryByTaskId(task_id)
        end
        self:TraceTaskDel(task_id)
        self:TraceTempTaskDel(task_id)
        if 1 == reason then
            self.task_id_list.completed_list[task_id] = 1
            self.task_id_list.finish_list = {[task_id] = 1}
            --其他任务需要监听
            if task_id == 60012 then
                PrestigeData.IsGuideUi = true
            end
            --通知外部监听者任务完成
            local id_listeners = self.task_id_complete_listenters[task_id]
            if id_listeners ~= nil then
                -- self.task_id_complete_listenters[task_id] = nil --回调完就自动注销
                for hand,_ in pairs(id_listeners) do
                    hand.func(task_id)
                end
            end
            for func,_ in pairs(self.complete_listeners) do
                func(task_id)
            end
        end
    end
end

function TaskData:SetTaskWaitingList(protocol)
    self.task_wait_list:Set(protocol.task_wait_list)
end

function TaskData:TaskInfoUpdateByTaskId(task_id, task_info, dont_trace)
    self.task_list[task_id] = task_info
    if task_info and not task_info.dont_show and not dont_trace then
        self:TraceTaskInit(task_info.task_type, task_id)
    end
end

function TaskData:TaskInfoDeleteByTaskId(task_id)
    self.task_list[task_id] = nil
    self:TraceTaskDel(task_id)
end

function TaskData:GetTaskInfoByTaskId(task_id)
    return self.task_list[task_id]
end

function TaskData:GetTaskList()
    return self.task_list
end

function TaskData:GetTaskListWithListType(list_type)
    local list = {}
    for _, info in pairs(self.task_list) do
        if list_type == info.list_type and info.task_type_sort then
            table.insert(list, info:Val())
        end
    end

    table.sort(
        list,
        function(a, b)
            if not b then
                return a
            end
            if a.task_type == b.task_type then
                return a.task_id < b.task_id
            end
            return a.task_type_sort < b.task_type_sort
        end
    )
    return list
end

function TaskData:GetTaskListAcceptedFollowMenu()
    local list = {}
    local accepted_list = self:GetTaskListWithListType(TaskConfig.ListType.accepted)
    for i = 1, #accepted_list do
        if accepted_list[i]:IsFollow() or accepted_list[i]:IsTempFollow() then
            table.insert(list, accepted_list[i])
        end
    end
    self.complete_list_effect = {}
    for pos, task in pairs(list) do
        if pos < 5 then
            self.complete_list_effect[task.task_id] = pos
        end
    end
    return list
end

function TaskData:GetTaskShowList(list_type)
    local list = self:GetTaskListWithListType(list_type)
    local level = RoleData.Instance:GetRoleLevel()
    local function new_parent(task_type, task_type_sort)
        return {
            name = Language.Task.TaskType[task_type],
            children = {},
            task_type_sort = task_type_sort
        }
    end

    local show_list = {}
    local show_list_sort = {}
    for _, v in pairs(list) do
        if not v.dont_show and v.task_type_sort and not v.block_view
        and (not v.show_level or (v.show_level and level >= v.show_level)) then
            show_list[v.task_type] = show_list[v.task_type] or new_parent(v.task_type, v.task_type_sort)
            table.insert(show_list[v.task_type].children, v)
            table.sort(
                show_list[v.task_type].children,
                function(a, b)
                    if not a or not b then
                        return a
                    end
                    if a.min_level then
                        return a.min_level < b.min_level
                    else
                        return a.task_id < b.task_id
                    end
                end
            )
        end
    end
    for _, v in pairs(show_list) do
        table.insert(show_list_sort, v)
    end
    table.sort(
        show_list_sort,
        function(a, b)
            return a.task_type_sort < b.task_type_sort
        end
    )
    return show_list_sort
end

function TaskData:GetNextInfoWithType(list_type)
    local list = self:GetTaskListWithListType(list_type)
    return list[next(list)]
end

function TaskData:GetMainlineAccepted()
    local list = self:GetTaskListWithListType(TaskConfig.ListType.accepted)
    for _, info in pairs(list) do
        if info.task_type == TaskConfig.TaskType.mainline then
            return info
        end
    end
end

function TaskData:GetChallengeAcceptableList()
    local list = {}
    for _, info in pairs(self:GetTaskListWithListType(TaskConfig.ListType.acceptable)) do
        if info.task_type == TaskConfig.TaskType.mainline then
            table.insert(list, info)
        end
    end
    return list
end

function TaskData:GetNpcDialogTaskButtonListByNpcId(npc_id)
    local sort_flag = false
    local list = {}
    for _, info in pairs(self.task_list) do
        if info.npc_id == npc_id and info:IsShowNpcDialogButton() then
            table.insert(list, info:Val())
            if info.sort_key then
                sort_flag = true
            end
        end
    end
    if sort_flag then
        table.sort(
            list,
            function(a, b)
                local a_key = a.sort_key or 0
                local b_key = b.sort_key or 0
                return a_key > b_key
            end
        )
    end
    return list
end

function TaskData:GetMainViewFuncImgByNpcId(npc_id, img_t)
    local is_show = false
    local task_info = nil
    local func_img = TaskConfig.MainViewFuncImgConvert
    for _, info in pairs(self.task_list) do
        if info.npc_id == npc_id and not info.is_remove and info:IsShowNpcBoardSign() then
            is_show = true
            if nil == task_info then
                task_info = info:Val()
            else
                if info.list_type > task_info.list_type or (info.list_type == task_info.list_type and info.task_id < task_info.task_id) then
                    task_info = info:Val()
                end
            end
        end
    end
    if nil ~= task_info then
        -- img_t.sp = "RenWu" .. func_img.task_type[task_info.task_type] .. func_img.list_type[task_info.list_type]
        img_t.sp = func_img.list_type_new[task_info.list_type]
    end
    return is_show
end

function TaskData:SetTaskCanAbandonedList(protocol)
    for _, info in pairs(protocol.task_list) do
        if self.task_list[info.task_id] then 
            self.task_list[info.task_id]:CanAbandoned(info.task_state)
        end 
    end
end

function TaskData:DemandTaskState(task_id, func)
    self.task_demand_list[task_id] = self.task_demand_list[task_id] or {}
    local need_call = nil == next(self.task_demand_list[task_id])
    table.insert(self.task_demand_list[task_id], func)
    return need_call
end

function TaskData:SetTaskSingleStatusInfo(protocol)
    local task_info = protocol.task_info
    for _, func in pairs(self.task_demand_list[task_info.task_id] or {}) do
        func(task_info.task_state)
    end
    self.task_demand_list[task_info.task_id] = {}
    --已完成暂时这样处理
    if task_info.task_state == 1 then
        self.task_id_list.completed_list[task_info.task_id] = 1
        self.task_id_list.finish_list = {[task_info.task_id] = 1}
    end
end

function TaskData:GetTaskConfig(task_id)
    return Config.tasklist_auto.task_list[task_id]
end

function TaskData:SetTaskTraceStatus(list)
    local task_trace_list = {}
    for _, v in pairs(list) do
        local task_type = v % 100
        local task_id = math.floor(v / 100)
        if task_type ~= TaskConfig.TaskType.dragon then
            task_trace_list[task_type] = task_id
        end
    end
    if self.task_trace_list[TaskConfig.TaskType.dragon] then
        task_trace_list[TaskConfig.TaskType.dragon] = self.task_trace_list[TaskConfig.TaskType.dragon]
    end
    self.task_trace_list:Set(task_trace_list)
end

-- 仅开放单个临时追踪
function TaskData:SetTaskTempTrace(task_id)
    self.task_temp_trace_list.val = task_id
end

function TaskData:TraceTempTaskDel(task_id)
    local task_type
    for tt, v in pairs(self.task_trace_list) do
        if v == task_id then
            return 
        end
    end

    self:SetTaskTempTrace()
end

function TaskData:GetTaskTraceStatusList()
    local list = {}
    for task_type, task_id in pairs(self.task_trace_list) do
        if task_type ~= TaskConfig.TaskType.dragon then
        table.insert(list, task_id * 100 + task_type)
    end
    end
    return list
end

function TaskData:TraceTaskInit(task_type, task_id)
    if nil == self.task_trace_list[task_type] or CUSTOM_OBJID.GUILD_TASK_BEGIN == self.task_trace_list[task_type] then
        self:TraceTask(task_type, task_id)
    end

    if nil ~= self.task_trace_list[task_type] and task_type == TaskConfig.TaskType.challenge then 
        self:SetTaskTempTrace(task_id)
    end 
end

function TaskData:TraceTaskDel(task_id)
    local task_type
    for tt, v in pairs(self.task_trace_list) do
        if v == task_id then
            task_type = tt
            self:TraceTask(task_type, nil)
        end
    end
    -- 挑战任务如果完成了子任务就追踪下个子任务 不手动追踪则追踪新下发的任务
    if TaskConfig.TaskType.challenge == task_type and ChallengeTaskData.Instance:IsLastChallengeTask(task_id) then
        return
    end
    -- 支线任务如果完成了子任务就追踪下个子任务 不手动追踪则追踪新下发的任务
    if TaskConfig.TaskType.branch == task_type and self:IsLastTask(task_id) then
        return
    end
    -- 如果追踪的任务做完了 而且已接列表还有该类型的任务 则自动追踪下一个等级最低任务并保存
    if task_type then
        local task_id, min_level = nil, 999
        for _, info in pairs(self.task_list) do
            if info.task_type == task_type and TaskConfig.ListType.accepted == info.list_type and not info.dont_show then
                local min_level_info = info.min_level and info.min_level or 999
                min_level = min_level_info <= min_level and min_level_info or min_level
                task_id = min_level_info <= min_level and info.task_id or task_id
            end
        end
        if task_id then
            self:TraceTask(task_type, task_id)
        end
    else 

    end
end

function TaskData:TraceTask(task_type, task_id)
    if nil == task_type then return end
    self.task_trace_list[task_type] = task_id
    TaskCtrl.Instance:CSTaskTraceStatus()
end

function TaskData:GetAcceptCoin()
    local level = RoleData.Instance:GetRoleLevel()
    for i, v in ipairs(Config.task_chain_auto.task_chain_config) do
        if v.min_level <= level and level <= v.max_level then
            return v.coin
        end
    end
    return 0
end

function TaskData:UpdateTaskChainFakeTask(info)
    if self.task_list[CUSTOM_OBJID.TASK_CHAIN] then
        self.task_list[CUSTOM_OBJID.TASK_CHAIN] = nil
        --[[ NpcDialogCtrl.Instance:NpcDialogView{
            talk_list = {
                {
                    talker = Config.task_chain_auto.other[1].start_npc,
                    talk_text = Language.TaskChain.AllFinished,
                }
            },
            beh_btn_list = {}
        } ]]
    end

    --local role_level = RoleData.Instance:GetRoleLevel()
    if 1 == info.task_count then
        if self.task_chain_btn_handle then
            BehaviorData.Instance:RemoveCustomNpcTalkBtnsId(self.task_chain_btn_handle)
            BehaviorData.Instance:RemoveCustomNpcTalkBtnsSeq(self.task_chain_btn_handle)
            self.task_chain_btn_handle = nil
        end
    else
--        add_btn()
    end
    -- if role_level >= 50 then
        self:TaskChainFakeTask()
    -- else
    --     self.task_list[CUSTOM_OBJID.TASK_CHAIN] = nil
    --     if self.add_task_chain_btn_handle then
    --         RoleCtrl.Instance.data:GetBaseData():Uncare(self.add_task_chain_btn_handle)
    --         self.add_task_chain_btn_handle = nil
    --     end
    --     self.add_task_chain_btn_handle =
    --         RoleCtrl.Instance.data:GetBaseData():KeysCare(
    --         {"level"},
    --         function()
    --             local role_level = RoleData.Instance:GetRoleLevel()
    --             if role_level >= 50 then
    --                 add_btn()
    --                 RoleCtrl.Instance.data:GetBaseData():Uncare(self.add_task_chain_btn_handle)
    --                 self.add_task_chain_btn_handle = nil
    --             end
    --         end
    --     )
    -- end
end
function TaskData:TaskChainFakeTask()
    if self.task_chain_btn_handle then
        BehaviorData.Instance:RemoveCustomNpcTalkBtnsId(self.task_chain_btn_handle)
        BehaviorData.Instance:RemoveCustomNpcTalkBtnsSeq(self.task_chain_btn_handle)
        self.task_chain_btn_handle = nil
    end
    self.task_chain_btn_handle =
        BehaviorData.Instance:CustomNpcTalkBtnsId(
        Config.task_chain_auto.other[1].start_npc,
        function(btn_list, obj_vo)
            local button =
                NpcDialogBehBtnData.FuncCreate(
                Language.TaskChain.Name[self.task_chain_data.count == 0 and 0 or 6],
                function()
                    if self.task_chain_data.count == 0 then
                        PublicPopupCtrl.Instance:DialogTips(
                            {
                                content = string.format(Language.TaskChain.AcceptTip, self:GetAcceptCoin()),
                                confirm = {
                                    func = function()
                                        TaskCtrl.Instance:CSTaskChainOp(TASK_CHAIN_OP_TYPE.ACCEPT)
                                        ViewMgr:CloseView(DialogTipsView)
                                    end
                                }
                            }
                        )
                    else
                        TaskCtrl.Instance:CSTaskChainOp(TASK_CHAIN_OP_TYPE.ACCEPT)
                    end
                end,
                true,
                self.task_chain_data.count == 0 and NpcDialogData.CreateDetailInfo1{act_type = ActType.TaskChain, tips = Language.TaskChain.DetailTips, rewards = NpcDialogData.GetDetailInfoRewardsByActType(ActType.TaskChain)}
                or nil
            )
            button:SetIconType(NpcIconType.Act)
            table.insert(btn_list, button)
        end
    )
end
function TaskData:StartTaskChain()
    local taskvo = CommonStruct.CustomTask()
    taskvo.task_id = CUSTOM_OBJID.TASK_CHAIN_START
    taskvo.task_name = Language.Task.TaskName.TaskChain
    taskvo.task_desc = Language.Task.TaskGoal.TaskChainStart
    taskvo.task_goal = Language.Task.TaskGoal.TaskChainStart
    return taskvo
end
function TaskData:TaskChainStage(stage)
    local next_lv = Config.task_chain_auto.level_times[stage].max_level + 1
--根据当前阶段显示
    local taskvo = CommonStruct.CustomTask()
    taskvo.task_id = CUSTOM_OBJID.TASK_CHAIN_START
    taskvo.task_name = Language.Task.TaskName.TaskChain
    taskvo.task_desc = string.format(Language.Task.TaskGoal.TaskChainStage, next_lv)
    taskvo.task_goal = string.format(Language.Task.TaskGoal.TaskChainStage, next_lv)
    return taskvo
end
function TaskData:TaskChainContinue()
    local taskvo = CommonStruct.CustomTask()
    taskvo.task_id = CUSTOM_OBJID.TASK_CHAIN_START
    taskvo.task_name = Language.Task.TaskName.TaskChain
    taskvo.task_desc = Language.Task.TaskGoal.TaskChainContinue
    taskvo.task_goal = Language.Task.TaskGoal.TaskChainContinue
    return taskvo
end
function TaskData:TaskChainReward(cur_num)
    local taskvo = CommonStruct.CustomTask()
    taskvo.task_id = CUSTOM_OBJID.TASK_CHAIN_START
    taskvo.task_name = Language.Task.TaskName.TaskChain
    taskvo.task_desc = Language.Task.TaskGoal.TaskChainReward
    taskvo.task_goal = Language.Task.TaskGoal.TaskChainReward
    return taskvo
end
local function concatTaskChainText(language, ...)
    return string.format(language[GameMath.Ramdon(1, #language)], ...)
end

local function concatTaskChainTextLT(language, limit_time, ...)
    return string.format(language[limit_time % (#language)+ 1], ...)
end

function TaskData:UpdateTaskChain(info)
    local total_num = info.cur_task_max_count --Config.task_chain_auto.other[1].weekly_num * 150
    local cur_num = info.task_pass_count + 1
    -- ActivityData.Instance:SetActivityFinishInfo(ActType.TaskChain, info.task_count * total_num + info.pass_count, total_num)
    if self.task_chain_btn_handle then
        BehaviorData.Instance:RemoveCustomNpcTalkBtnsId(self.task_chain_btn_handle)
        BehaviorData.Instance:RemoveCustomNpcTalkBtnsSeq(self.task_chain_btn_handle)
        self.task_chain_btn_handle = nil
    end
    if self.task_chain_talk_func then
        BehaviorData.Instance:RemoveCustomNpcTalkListSeq(self.task_chain_talk_func)
        self.task_chain_talk_func = nil
    end

    local taskvo = CommonStruct.CustomTask()
    taskvo.task_id = CUSTOM_OBJID.TASK_CHAIN

    if not self.task_list[CUSTOM_OBJID.TASK_CHAIN] then
        self.task_list[CUSTOM_OBJID.TASK_CHAIN] = SmartData.New(TaskInfoChain.New():TaskInfo())
        taskvo.task_name = Language.TaskChain.Name[0]
        taskvo.task_desc = ""
        taskvo.task_goal = ""
    end
    if self.bag_data_care_handle_item then
        BagData.Instance:UnCare(self.bag_data_care_handle_item)
        self.bag_data_care_handle_item = nil
    end

    local task = self.task_list[CUSTOM_OBJID.TASK_CHAIN]
    local pre_info = task.task_info
    local accept_text
    local cfg = Config.task_chain_auto
    BehaviorData.Instance.auto_behavior = 0
    -- 描述
    if info.task_type == TASK_CHAIN_TASK_TYPE.TALK then
        BehaviorData.Instance.auto_behavior = 2
        self:UpdateTaskChainTalk(task, taskvo, info, pre_info)
    elseif info.task_type == TASK_CHAIN_TASK_TYPE.ITEM then
        BehaviorData.Instance.auto_behavior = 2
        self:UpdateTaskChainItem(task, taskvo, info, pre_info)
    elseif info.task_type == TASK_CHAIN_TASK_TYPE.FIGHT then
        BehaviorData.Instance.auto_behavior = 2
        self:UpdateTaskChainFight(task, taskvo, info, pre_info)
    elseif info.task_type == TASK_CHAIN_TASK_TYPE.PET then
        BehaviorData.Instance.auto_behavior = 2
        self:UpdateTaskChainPet(task, taskvo, info, pre_info)
    elseif info.task_type == TASK_CHAIN_TASK_TYPE.WAIT then
        BehaviorData.Instance.auto_behavior = 2
        self:UpdateTaskChainWait(task, taskvo, info, pre_info)
                                    end

    -- 奖励
    local reward_list = {}
    local next_completion_times = info.pass_count + 1
    if next_completion_times % 10 == 0 then
        for i, v in ipairs(cfg.reward_config) do
            if next_completion_times == v.completion_times then
                local item = Item.Create(v.reward_item)
                table.insert(reward_list, item)
            end
        end
    end
    local role_level = RoleData.Instance:GetRoleLevel()
    for i, v in ipairs(cfg.exp_config) do
        if v.min_num <= next_completion_times and next_completion_times <= v.max_num and v.min_level <= role_level and role_level <= v.max_level then
            local item = Item.New()
            item.item_id = CommonItemId.Exp
            item.is_bind = false
            item.num = v.exp
            table.insert(reward_list, item)
        end
    end
    taskvo.reward_list = reward_list

    task.task_info = info
    task:TaskVo(taskvo)
    if info.is_new == 1 and info.pass_count == 0 then
        task.is_new.val = true
        self:TraceTask(TaskConfig.TaskType.task_chain, CUSTOM_OBJID.TASK_CHAIN)
    else
        task.is_new.val = false
    end
end
function TaskData:GetDetailInfoWish()
    local role_level = RoleData.Instance:GetRoleLevel()
    -- local config = self.acts_config_wish_wish_reward[role_level] or {}
    local list = {}
    local item_data = Item.New()
    item_data.item_id = CommonItemId.Exp
    item_data.num = 0
    table.insert(list, item_data)
    local item_data = Item.New()
    item_data.item_id = CommonItemId.Coin
    item_data.num = 0
    table.insert(list, item_data)
    return list
end

function TaskData:SetCompleteList(list)
    self.task_id_list.completed_list = list
end

function TaskData:SetCompleteTaskId(task_id)
    if task_id == 0 then
        return
    end
    self.task_id_list.completed_list[task_id] = 1
end

function TaskData:GetTaskIsCompleted(task_id)
    if task_id == 0 then
        return true
    end
    return self.task_id_list.completed_list[task_id] and (self.task_id_list.completed_list[task_id] == 1)
end

function TaskData:GetTaskIsAccepted(task_id)
    return self.task_id_list.accepted_list[task_id] ~= nil
end

function TaskData:GoChallengeTask(t_tasks, last_task)
    local list_type = TaskConfig.ListType.accepted
    local task_type = TaskConfig.TaskType.challenge
    local challenge_task_list = {}
    for _, info in pairs(self.task_list) do
        if info.list_type == list_type and info.task_type == task_type then
            table.insert(challenge_task_list, info:Val())
        end
    end
    for _, task_id in pairs(t_tasks) do
        for _, info in pairs(challenge_task_list) do
            if info.task_id == task_id then
                self:TraceTask(info.task_type, info.task_id)
                info:GoTask()
                return
            end
        end
    end
    Debuger.LogError("challengetask_list_auto/challenge_go_on last_task:%s 找不到当前进行中的子任务", last_task)
end

----------------------------用于监听-------------
function TaskData:CareTaskChange(func)
    return self.task_id_list:Care(func)
end
function TaskData:CareAcceptedTaskChange(func)
    return self.task_id_list:Care(func, "accepted_list")
end

function TaskData:CareCompletedTaskChange(func)
    return self.task_id_list:Care(func, "finish_list")
end

function TaskData:UnCareAcceptedTaskChange(ht)
    self.task_id_list:Uncare(ht)
end


--添加任务完成监听通知 
--func(task_id)
--return register handle
function TaskData:ListenTaskComplete(func)
    self.complete_listeners[func] = true
    return func
end

function TaskData:UnlistenTaskComplete(handle)
    if handle ~= nil then
        self.complete_listeners[handle] = nil
    end
end

--添加指定任务的完成监听通知 
function TaskData:ListenTaskCompleteById(func,task_id)
    return self:ListenTaskCompleteByIds(func,{task_id})
end

--同时添加多条指定任务的完成监听通知 {task_id1,task_id2,...}
function TaskData:ListenTaskCompleteByIds(func,task_ids)
    local listen_handle = nil
    for _,v in pairs(task_ids) do
        -- if  self:GetTaskIsCompleted(v) == false then   --任务未完成才注册监听
            local task_handles = self.task_id_complete_listenters[v]
            if task_handles == nil then
                task_handles = {}
                self.task_id_complete_listenters[v] = task_handles
            end
            if listen_handle == nil then
                listen_handle = {task_ids = task_ids,func = func}
            end
            task_handles[listen_handle] = true
        -- end
    end
    return listen_handle
end

function TaskData:UnlistenTTaskCompleteById(handle)
    if handle == nil then
        return
    end
    for _,task_id in pairs(handle.task_ids) do
        local task_id_handles = self.task_id_complete_listenters[task_id]
        if task_id_handles ~= nil then
            task_id_handles[handle] = nil
        end
    end
end


function TaskData:AcceptedTask()
    return self.task_id_list.accepted_list
end

function TaskData:CompletedTask()
    return self.task_id_list.completed_list
end

function TaskData:FinishTask()
    return self.task_id_list.finish_list
end

function TaskData:HasPromoteTask()
    for k, v in pairs(self.task_id_list.accepted_list) do
        local config = self:GetAdvancedTaskConfig(k)
        if config and config.task_type == 3 then
            return true
        end
    end
end

--获取当前进阶任务
function TaskData:GetCurPromoteTaskData()
    local pre_task_id = 41100
    for k, v in pairs(self.task_id_list.accepted_list) do
        if tonumber(k) >= pre_task_id then
            local config = self:GetAdvancedTaskConfig(k)
            if config and config.task_type == 3 then
                return config
            end
        end
    end
    return self:GetAdvancedTaskConfig(pre_task_id)
end

function TaskData:GetAdvancedTaskConfig(task_id)
    return Config.advanced_tasks_auto.task_list[task_id]
end

function TaskData:IsLastTask(task_id)
    return nil == self.task_sub_list[task_id]
end

function TaskData:HaveSubTask(last_task)
    local value = false
    local sub_tasks = self.task_sub_list[last_task] or {}
    for _, word_task in pairs(sub_tasks) do
        value = value or (nil ~= self.task_list[word_task])
    end
    return value
end
function TaskData:SetChainTaskTurnTableRewardInfo(protocol)
    self.chain_bounty_task_info:Set(protocol.bounty_task_info)
end
function TaskData:SetBountyTaskTurnTableRewardInfo(protocol, is_taskchain)
    local bounty_task_info = protocol.bounty_task_info
    self.bounty_task_info:Set(bounty_task_info)
    if bounty_task_info.group_id > 0 then
        local reward_list = {}
        if is_taskchain == true then
            for index, value in ipairs(Config.task_chain_auto.rewar_group) do
                if bounty_task_info.group_id == value.reward_group then
                    table.insert(reward_list, Item.Create(value.reward))
                end
            end
        else
            for _, v in pairs(Config.bounty_tasklist_auto.reward_group) do
                if bounty_task_info.group_id == v.group_id then
                    local reward = v.reward
                    table.insert(reward_list, Item.Init(reward.item_id, reward.num, reward.is_bind))
                end
            end
        end
        self.bounty_task_info.reward_type = is_taskchain and BountyTaskRewardGetType.TaskChain or 0
        self.bounty_task_info.reward_list = reward_list
        self.bounty_task_info.reward = reward_list[self.bounty_task_info.reward_idx + 1]
    end
end

function TaskData:SetBountyTaskData(param_t)
    self.bounty_task_info:Set(param_t)
end

function TaskData:GetBountyTaskData()
    return self.bounty_task_info
end

function TaskData:SetBountyTaskInfo(protocol)
    self.bounty_task_progress = protocol.bounty_task_progress
    local cur_task_idx = self.bounty_task_progress.cur_task_idx
if self.bounty_task_progress.has_finish_first_task == 0 then
        self.bounty_task_progress.cur_task_idx = -1
        self.bounty_task_progress.today_task_num = 10
    end
    self.bounty_task_progress.cur_task_idx = 10 == self.bounty_task_progress.cur_task_idx and 9 or self.bounty_task_progress.cur_task_idx
    ActivityData.Instance:SetActivityFinishInfo(ActType.BountyTask, cur_task_idx, self.bounty_task_progress.today_task_num)
    self.is_finish_bounty_task = cur_task_idx >= self.bounty_task_progress.today_task_num
end

function TaskData:SetRAHelpOthersInfo(protocol)
    local info = protocol and protocol.info or {}
    local not_open = ActStatusType.Open ~= ActivityData.Instance:GetActivityStatus(ACTIVITY_TYPE.RAND_HELP_OTHERS)
    local level_not_enough = RoleData.Instance:GetRoleLevel() < self.config_help_others_other.level_open
    local task_prefix1 = ""
    local task_prefix2 = ""
    local show_items = {}
    if nil ~= self.task_help_others_handle then
        BehaviorData.Instance:RemoveCustomNpcTalkBtnsSeq(self.task_help_others_handle)
        self.task_help_others_handle = nil
    end
    -- local show_cfg = ServerActivityData.Instance:GetOpenConfig(ACTIVITY_TYPE.RAND_HELP_OTHERS, Config.taoyuanyisanshi_auto.task_prefix)
    local show_cfg = ServerActivityData.Instance:GetOpenConfigStartTime(ACTIVITY_TYPE.RAND_HELP_OTHERS, Config.taoyuanyisanshi_auto.task_prefix)
    if show_cfg ~= nil and show_cfg[1] ~= nil then
        task_prefix1 = show_cfg[1].task_prefix_1
        task_prefix2 = show_cfg[1].task_prefix_2
        show_items = show_cfg[1].show_item
    end
    if not_open or TaskConfig.HelpOthersStatus.finish == info.task_status or level_not_enough or StringIsEmpty(task_prefix1) then
        if self.task_list[CUSTOM_OBJID.HELP_OTHERS] then
            self.task_list[CUSTOM_OBJID.HELP_OTHERS]:RemoveHandle()
        end
        self:TaskInfoDeleteByTaskId(CUSTOM_OBJID.HELP_OTHERS)
        return
    end
    if 0 == info.task_id then
        info.task_id = 1
        info.task_status = TaskConfig.HelpOthersStatus.commit
    end
    local co = Cfg.TaskHelpOthersCoById(info.task_id)
    co.task_prefix1 = task_prefix1
    co.task_prefix2 = task_prefix2
    if 1 == info.task_id then
        local start_npc = co.commit_npc
        if start_npc and start_npc > 0 then
            self.task_help_others_handle = BehaviorData.Instance:CustomNpcTalkBtnsSeq(start_npc, function (btn_list,obj_vo)
                if obj_vo and obj_vo.seq == start_npc and btn_list then
                    local rewards = {}
                    for i = 0, #show_items do
                        local item_data = show_items[i]
                        table.insert(rewards, Item.Init(item_data.item_id, item_data.num, item_data.is_bind))
                    end
                    local detail_tips = Language.Task.HelpOthers.detail_tips
                    local all_count = self.config_help_others_other.ring_up
                    local button = NpcDialogBehBtnData.FuncCreate(
                        task_prefix1,
                        BindTool.Bind(TaskCtrl.Instance.SendRAHelpOthersStart, TaskCtrl.Instance),
                        -- true, NpcDialogData.CreateDetailInfo1{tips = {string.format(detail_tips[1], all_count), string.format(detail_tips[2], all_count)}, rewards = rewards}
                        true, NpcDialogData.CreateDetailInfo1{tips = {string.format(detail_tips[1], all_count)}, rewards = rewards}
                    )
                    table.insert(btn_list, button)
                end 
            end)
        end
    elseif TaskConfig.HelpOthersStatus.accept == info.task_status then
        local accept_npc = co.accept_npc
        if accept_npc and accept_npc > 0 then
            self.task_help_others_handle = BehaviorData.Instance:CustomNpcTalkBtnsSeq(accept_npc, function (btn_list,obj_vo)
                if obj_vo and obj_vo.seq == accept_npc and btn_list then
                    local button = NpcDialogBehBtnData.FuncCreate(
                        co.task_name,
                        function ()
                            NpcDialogCtrl.Instance:NpcDialogView(
                                {
                                    talk_id = co.accept_talk_id,
                                },
                                BindTool.Bind(TaskCtrl.Instance.SendRAHelpOthersAccept, TaskCtrl.Instance)
                            )
                        end,
                        false
                    )
                    table.insert(btn_list, button)
                end 
            end)
        end
    elseif TaskConfig.HelpOthersStatus.commit == info.task_status then
        local commit_npc = co.commit_npc
        if commit_npc and commit_npc > 0 then
            local auto_close_npc_dialog = function ()
                local value = false
                if 4 == co.task_type then
                    local pet_id = co.param_1
                    local items = {}
                    if pet_id > 0 then
                        for _, pet in pairs(PetData.Instance.pet_list) do
                            local pet_state = pet:State()
                            if pet:ID() == pet_id and PetState.Relax == pet_state and not pet:ArenaState() then
                                table.insert(items, pet)
                            end
                        end
                    end
                    value = not next(items)
                elseif 2 == co.task_type or 5 == co.task_type then
                    local item_id = co.param_1
                    value = not (BagData.Instance:GetNumByItemId(item_id) >= co.param_2)
                end
                return value
            end
            local end_func = function ()
                if 1 == co.task_type then
                    NpcDialogCtrl.Instance:NpcDialogView(
                        {
                            talk_id = co.commit_talk_id,
                        },
                        BindTool.Bind(TaskCtrl.SendRAHelpOthersTalk,TaskCtrl.Instance)
                    )
                elseif 2 == co.task_type or 4 == co.task_type or 5 == co.task_type then
                    local is_enough = false
                    local item_id, pet_id = 0, 0
                    if 4 == co.task_type then
                        pet_id = co.param_1
                        local pet_name = Pet.New{pet_id = pet_id}:Name()
                        local items = {}
                        if pet_id > 0 then
                            for _, pet in pairs(PetData.Instance.pet_list) do
                                local pet_state = pet:State()
                                if pet:ID() == pet_id and PetState.Relax == pet_state and not pet:ArenaState() then
                                    table.insert(items, pet)
                                end
                            end
                        end
                        is_enough = next(items)
                        if not is_enough then
                            local storage_have = false
                            if pet_id > 0 then
                                for _, pet in pairs(PetData.Instance.pet_storage_list) do
                                    if pet:ID() == pet_id then
                                        storage_have = true
                                        break
                                    end
                                end
                            end
                            local co = Cfg.ChainPetGroupByPI(pet_id)
                            if nil == co then
                                LogError(string.format("商人的烦恼 pet_id:%s 找不到巡逻场景", pet_id))
                                return
                            end
                            local scene_id = co.scene_id
                            local goto_scene = function ()
                                BehaviorCtrl.ExeByCfgVo({
                                    behavior_type = 9,    --behavior_type 是 beh_id_to_tab.key
                                    param1 = scene_id,
                                    param2 = "",
                                    param3 = "",
                                })
                                ViewMgr:OpenView(FightAutoSealSetView,{["scene_id"] = scene_id})
                                ViewMgr:CloseView(DialogTipsView)
                            end
                            if storage_have == false then
                                PublicPopupCtrl.Instance:DialogTips{
                                    content = string.format(Language.TaskChain.NotEnoughTips[4], pet_name, SceneData.Instance:GetSceneCfg(scene_id).name),
                                    confirm = {func = goto_scene}
                                }
                            else
                                PublicPopupCtrl.Instance:DialogTips{
                                    content = string.format(Language.TaskChain.NotEnoughTips[5], pet_name, SceneData.Instance:GetSceneCfg(scene_id).name),
                                    confirm = {
                                        func = function ()
                                            ViewMgr:CloseView(DialogTipsView)
                                            ViewMgr:OpenView(PetStorage)
                                        end
                                    },
                                    cancel = {func = goto_scene}
                                }
                            end
                        end
                    else
                        item_id = co.param_1
                        is_enough = BagData.Instance:GetNumByItemId(item_id) >= co.param_2
                        if not is_enough then
                            local is_medicine = ItemType.Medicine == Item.GetType(item_id)
                            local has_get_way = "number" == type(co.get_way)
                            local co_get_way = has_get_way and Config.getway_cfg_auto.get_way[co.get_way] or {}
                            PublicPopupCtrl.Instance:DialogTips{
                                content = string.format(Language.TaskChain.NotEnoughTips[2][has_get_way and 100 or (is_medicine and 1 or 0)], Item.GetQuaName(item_id), has_get_way and co_get_way.desc or ""),
                                confirm = {
                                    func = function()
                                        if is_medicine then
                                            WorkshopData.Instance:SetLifeItemQuickJump(item_id)
                                            ViewMgr:OpenViewByKey(Mod.Workshop.NeoCompose)
                                            ViewMgr:CloseView(DialogTipsView)
                                            return
                                        end
                                        if has_get_way then
                                            GetWayData.OpenPanel(co_get_way, Item.Create({item_id = item_id, need_num = co.param_2}))
                                            ViewMgr:CloseView(DialogTipsView)
                                            return
                                        elseif Item.GetType(item_id) == 18 then
                                            TradeData.Instance:SetSearchItemId(item_id)
                                            ViewMgr:OpenViewByKey(Mod.Mall.Trade)
                                            ViewMgr:CloseView(DialogTipsView)
                                            return
                                        end
                                        TaskData.shop_auto_close = true
                                        MallCtrl.Instance:Jump(item_id)
                                        ViewMgr:CloseView(DialogTipsView)
                                    end
                                }
                            }
                        end
                    end
                    if is_enough then
                        TaskChainItemSel.item_id = item_id
                        TaskChainItemSel.pet_id = pet_id
                        TaskChainItemSel.from_type = TaskConfig.TaskChainItemSelFrom.help_others
                        NpcDialogCtrl.Instance:NpcDialogView(
                                {
                                    talk_id = co.commit_talk_id,
                                },
                                BindTool.Bind(ViewMgr.OpenView, ViewMgr, TaskChainItemSel)
                            )
                    end
                elseif 3 == co.task_type then
                    NpcDialogCtrl.Instance:NpcDialogView(
                        {
                            talk_id = co.commit_talk_id,
                        },
                        BindTool.Bind(TaskCtrl.SendRAHelpOthersBattle,TaskCtrl.Instance)
                    )
                end
            end
            self.task_help_others_handle = BehaviorData.Instance:CustomNpcTalkBtnsSeq(commit_npc, function (btn_list,obj_vo)
                if obj_vo and obj_vo.seq == commit_npc and btn_list then
                    local button = NpcDialogBehBtnData.FuncCreate(
                        co.task_name,
                        end_func,
                        auto_close_npc_dialog
                    )
                    table.insert(btn_list, button)
                end 
            end)
        end
    end
    local task = self.task_list[CUSTOM_OBJID.HELP_OTHERS] or SmartData.New(TaskInfoHelpOthers.New():TaskInfo())
    local progress = string.format("(%s/%s)", TaskConfig.HelpOthersStatus.accept == info.task_status and info.pass_count or (info.pass_count + 1), self.config_help_others_other.ring_up)
    task:TaskVo(info, co, progress)
    self:TaskInfoUpdateByTaskId(CUSTOM_OBJID.HELP_OTHERS, task)

    --56638
    if 3 == co.task_type and info.progress >= co.param_2 then
        AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(task)}, 1)
    end
end

function TaskData:GoTaskByTaskType(task_type)
    local list = self:GetTaskListWithListType(TaskConfig.ListType.accepted)
    for _, task in pairs(list) do
        if task.task_type == task_type then
            task:GoTask()
            self:AfterGoTask(task_type)
            break
        end
    end
end
function TaskData:AfterGoTask(task_type)
    if task_type == TaskConfig.TaskType.bounty or task_type == TaskConfig.TaskType.task_chain or task_type == TaskConfig.TaskType.prestige then
        local hide = self:TaskState(task_type)
        if hide then
            self:StopTaskState(task_type)
        end
    end
end
function TaskData:GoTraceTaskByTaskType(task_type)
    local task_id = self.task_trace_list[task_type]
    if task_id then
        self.task_list[task_id]:GoTask()
        return true
    end
    return false
end

function TaskData:RoleNoticeComplete()
    for task_id, co in pairs(Config.tasklist_auto.show_award) do
        if self.task_list[task_id] then
            self.task_list[task_id]:CheckRewardList(co)
        end
    end
end

function TaskData:GetBountyTaskRewardList()
    return RewardData.Instance:GetActivityBackRewardList(Config.bounty_tasklist_auto.reward_find, Config.bounty_tasklist_auto.reward_find_group)
end

-- 初始化所有该类型任务
function TaskData:DelAllTypeTask(task_type)
    for task_id, info in pairs(self.task_list) do
        if info.task_type == task_type and task_id ~= CUSTOM_OBJID.GUILD_TASK_BEGIN then self.task_list[task_id] = nil end
    end
end

function TaskData:GetTaskInfoByTaskType(task_type)
    for task_id, info in pairs(self.task_list) do
        if info.task_type == task_type then
            return info
        end
    end
end

-- 队伍目标的任务是否已经完成
function TaskData:IsTeamTargetTaskComplete(target, ignore_full)
    local is_complete = false
    if TeamConfig.TeamTargetType.BranchTask == target.team_type then
        is_complete = (ignore_full or TeamData.Instance:IsTeamTargetShowSprite(1)) and not self:HaveSubTask(target.task_id) and RoleData.Instance:GetRoleLevel() >= target.low_level
    elseif TeamConfig.TeamTargetType.ChallengeTask == target.team_type then
        is_complete = (ignore_full or TeamData.Instance:IsTeamTargetShowSprite(2)) and not ChallengeTaskData.Instance:HaveChallengeTask(target.task_id) and RoleData.Instance:GetRoleLevel() >= target.low_level
    elseif TeamConfig.TeamTargetType.UpgradeTask == target.team_type then
        is_complete = self:GetTaskIsCompleted(target.task_id) and not TeamData.Instance:GetSalaryIsComplete(TeamConfig.SalaryType.knightly, 3)
    end
    return is_complete
end

-- 队伍目标的任务是否显示侠义值
function TaskData:IsTeamTargetTaskComplete2(target, ignore_full)
    local is_complete = false
    if TeamConfig.TeamTargetType.BranchTask == target.team_type then
        is_complete = (ignore_full or TeamData.Instance:IsTeamTargetShowSprite(1)) and not self:HaveSubTask(target.task_id) and RoleData.Instance:GetRoleLevel() >= target.low_level
    elseif TeamConfig.TeamTargetType.ChallengeTask == target.team_type then
        is_complete = (ignore_full or TeamData.Instance:IsTeamTargetShowSprite(2)) and not ChallengeTaskData.Instance:HaveChallengeTask(target.task_id) and RoleData.Instance:GetRoleLevel() >= target.low_level
    elseif TeamConfig.TeamTargetType.RareFB == target.team_type then
        is_complete = 0 == FightData.Instance.fight_crystal_sweep.new_xiyou_play_times_remain and not TeamData.Instance:GetSalaryIsComplete(TeamConfig.SalaryType.knightly, 8)
    elseif TeamConfig.TeamTargetType.EliteFB == target.team_type then
        local co = Cfg.CrystalSceneNameByTeamTarget(target.target_id)
        if co then
            is_complete = 0 == Item.GetNum(co.expend_item) and not TeamData.Instance:GetSalaryIsComplete(TeamConfig.SalaryType.knightly, 9)
        end
    elseif TeamConfig.TeamTargetType.UpgradeTask == target.team_type then
        is_complete = self:GetTaskIsCompleted(target.task_id) and not TeamData.Instance:GetSalaryIsComplete(TeamConfig.SalaryType.knightly, 3)
    elseif TeamConfig.TeamTargetType.Conspiracy == target.team_type then
        is_complete = ConspiracyData.Instance:TargetAllKill(target.target_id) and not TeamData.Instance:GetSalaryIsComplete(TeamConfig.SalaryType.knightly, 4)
	elseif TeamConfig.TeamTargetType.BigDipper == target.team_type then
        is_complete = BigDipperData.Instance:BossIsPassBySn(target.target_id - 201) and not TeamData.Instance:GetSalaryIsComplete(TeamConfig.SalaryType.knightly, 10)
    elseif TeamConfig.TeamTargetType.HuanShiChallenge == target.team_type then
        is_complete=TaskData.Instance:GetTaskIsCompleted(target.task_id)
    end
    is_complete = is_complete and RoleData.Instance:GetRoleLevel() >= target.low_level
    return is_complete
end

function TaskData.GetBountyTaskDetailInfoRewardShow()
    local level = RoleData.Instance:GetRoleLevel()
    local co
    local t = {}
    for k, v in pairs(Config.bounty_tasklist_auto.reward_show) do
        if v.min_level <= level and level <= v.max_level then
            co = v
        end
    end
    if co then
        local function CheckReward(reward, item_id)
            if reward ~= 0 then
                local item_data = Item.New()
                item_data.item_id = item_id
                item_data.num = reward
                item_data.is_bind = 0
                table.insert(t, item_data)
            end
        end
        CheckReward(co.exp, CommonItemId.Exp)
        CheckReward(co.coin, CommonItemId.Coin)
        for k, v in pairs(co.reward or RO_EMTPY_TAB) do
            table.insert(t, Item.Init(v.item_id, v.num, v.is_bind))
        end
    end
    return t
end

function TaskData:TaskChainSubContinue(continue)
    if self.task_chain_sub_continue then
        self.task_chain_sub_continue = continue or false 
        local task = self.task_list[CUSTOM_OBJID.TASK_CHAIN]
        if task == nil then 
            self.task_chain_sub_continue = false
            return 
        end
        local info = task.task_info
        if info.task_type == TASK_CHAIN_TASK_TYPE.ITEM and BagData.Instance:GetNumByItemId(info.task_param[2]) ~= 0 then
            task:GoTask()
        elseif info.task_type == TASK_CHAIN_TASK_TYPE.PET then
            local pet_id = info.task_param[3]
            local mutant_pet = PetData.Instance:GetMutantPetCfgByNormalId(pet_id).pet_id or 0
            if pet_id > 0 then
                local items = {}
                for _, pet in pairs(PetData.Instance.pet_list) do
                    if (pet:ID() == pet_id or pet:ID() == mutant_pet) and pet:CanSubmit() then
                        table.insert(items, pet)
                    end
                end
                if next(items) then
                    self.task_chain_sub_continue = false
                    if MainViewData.Instance:IsBattle() then
                        if nil ~= self.handle_task_chain_go then
                            MainViewData.Instance:BaseData():Uncare(self.handle_task_chain_go)
                            self.handle_task_chain_go = nil
                        end
                        self.handle_task_chain_go = MainViewData.Instance:BaseData():Care(function ()
                            MainViewData.Instance:BaseData():Uncare(self.handle_task_chain_go)
                            self.handle_task_chain_go = nil
                            task:GoTask()
                        end, "is_battle")
                    else
                        task:GoTask()
                    end
                end
            end
        end
    end
end

function TaskData:DragonTaskTrace()
    for i = CUSTOM_OBJID.RESEARCH_TASK, CUSTOM_OBJID.FightDen do
        if CUSTOM_OBJID.TASK_CHAIN_START ~= i then
            if self.task_list[i] and not self.task_list[i].dont_show then
                self.task_trace_list[TaskConfig.TaskType.dragon] = i
                break
            else
                self.task_trace_list[TaskConfig.TaskType.dragon] = nil
            end
        end
    end
end

function TaskData:GoTaskHelpOthers()
    if nil ~= self.task_list[CUSTOM_OBJID.HELP_OTHERS] then
        self.task_list[CUSTOM_OBJID.HELP_OTHERS]:GoTask()
        return true
    end
    return false
end
function TaskData:GoTaskShanHai()
    if nil ~= self.task_list[CUSTOM_OBJID.SHAN_HAI_ORIGIN] then
        self.task_list[CUSTOM_OBJID.SHAN_HAI_ORIGIN]:GoTask()
        return true
    end
    return false
end

function TaskData:TaskCloseViewGuide(mod_key)
    local mod_key = mod_key or 0
    local tasks = self.task_guide_auto_list[TaskConfig.TaskGuideAutoType.mod_key][mod_key]
    if tasks == nil and type(mod_key) == "table"  then 
        for k,v in pairs(mod_key) do
            tasks = self.task_guide_auto_list[TaskConfig.TaskGuideAutoType.mod_key][v]
            if tasks ~= nil then break end 
        end 
    end 
    if tasks then
        for _, info in pairs(tasks) do
            local task = self.task_list[info.task_id]
            if task then
                if 1 == info.num then
                    local key = PrefKeys.TaskGuideAuto(info.seq)
                    local flag = UnityPlayerPrefs.GetInt(key)
                    if 0 == flag then
                        AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(task)}, 1)
                        UnityPlayerPrefs.SetInt(key, 1)
                        break
                    end
                else
                    AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(task)}, 1)
                    break
                end
            else
                -- LogDG("关闭界面指引触发，任务列表中没有这个任务所以无法触发，task_id=", info.task_id, "seq = ", info.seq)
            end
        end
    else
        -- LogDG("当前切换没有指引")
    end
end

function TaskData:TaskLevelSceneGuide(scene_mode)
    local scene_mode = scene_mode or 0
    local tasks = self.task_guide_auto_list[TaskConfig.TaskGuideAutoType.scene_mode][scene_mode]
    TimeHelper:CancelTimer(self.timer_dt_ls)
    if tasks then
        self.timer_dt_ls = TimeHelper:AddDelayTimer(function ()
            for _, info in pairs(tasks) do
                local task = self.task_list[info.task_id]
                if task then
                    if 1 == info.num then
                        local key = PrefKeys.TaskGuideAuto(info.seq)
                        local flag = UnityPlayerPrefs.GetInt(key)
                        if 0 == flag then
                            AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(task)}, 1)
                            UnityPlayerPrefs.SetInt(key, 1)
                            break
                        end
                    else
                        AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(task)}, 1)
                        break
                    end
                else
                    -- LogDG("切换场景触发指引任务，任务表没有这个任务，task_id=",info.task_id, "seq = ", info.seq)
                end
            end
        end, 1)
    else
        -- LogDG("当前切换没有指引")
    end
end

--用于设置第一天是否点过任务的前往且是否要在任务栏显示任务
function TaskData:TaskState(task_type)
    local t0 = TimeManager.TimeDayEnd(os.time(), 1)
    --大于0说明这一天还没过去
    local key = PrefKeys.DailyTask(task_type, TimeManager.DayStart())
    local flag = UnityPlayerPrefs.GetInt(key)
    -- LogError(task_type, t0- os.time(), key, flag)
    
    if (t0 - os.time()) > 0 and flag == 0 then
        -- local key = PrefKeys.DailyTask(task_type)
        -- UnityPlayerPrefs.SetInt(key,1)
        return true
    else
        return false
    end
end
function TaskData:StopTaskState(task_type, task_id)
    --[[ if not self:TaskState(task_type) then
        return
    end ]]
    if task_type == TaskConfig.TaskType.bounty or task_type == TaskConfig.TaskType.silk_road 
    or task_type == TaskConfig.TaskType.task_chain or task_type == TaskConfig.TaskType.prestige then
        local info = TaskData.Instance:GetTaskInfoByTaskType(task_type)
        if info then
            info.dont_show = false
            TaskData.Instance:TraceTask(task_type, info.task_id)
        end
    else
        TaskData.Instance:TraceTask(task_type, task_id)
    end
    local key = PrefKeys.DailyTask(task_type, TimeManager.DayStart())
    UnityPlayerPrefs.SetInt(key,1)
end
function TaskData:ClearDragon()
    self.task_trace_list[TaskConfig.TaskType.dragon] = nil
end

function TaskData:AutoMainLineTask()
    local task = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.mainline)
    local role_level = RoleData.Instance:GetRoleLevel()
    if nil == task or 30 ~= role_level then return end
    local min_level = task.min_level or 0
    if min_level > (role_level - 1) and role_level >= min_level then
        task:GoTask()
    end
end

function TaskData:MainLineArrowFlag()
    local role_lv = RoleData.Instance:GetRoleLevel()
    return role_lv <= GuideData.Instance:GetArrowLevelMax() and role_lv >= GuideData.Instance:GetArrowLevelMin()
end

function TaskData:FlushMenuGuide()
    local main_line_accepted = self:GetMainlineAccepted()
    if main_line_accepted == nil then
        self.menu_guide.val = 0
        return
    end
    if self:MainLineArrowFlag() then
        self.menu_guide.val = main_line_accepted.task_id
    else
        self.menu_guide.val = 0
    end

    -- self.menu_guide.pre_val = self.menu_guide.val
    -- TaskConfig.ListType.accepted
    -- if self.menu_guide.val > 0 then
        -- self.menu_guide.val = RoleData.Instance:GetRoleLevel() <= GuideData.Instance:GetArrowLevel() and self.menu_guide.val or 0
    -- end
end

function TaskData:PreviewMainLineDesc()
    if nil == self.preview_main_line_desc then
        self.preview_main_line_desc = {}
        for index, desc in ipairs(Language.Task.MainLinePreviewDesc) do
            local str = ""
            local t = DataHelper.GetStringWordToTable(desc)
            for _, w in ipairs(t) do
                str = str .. w .. "\n"
            end
            self.preview_main_line_desc[index] = str
        end 
    end
    return self.preview_main_line_desc
end

function TaskData:TaskIngEffect(task_id)
    if 0 == task_id and RoleData.Instance:GetRoleLevel() == GuideData.Instance:GetTaskEffectLevel() then
        local task_accepted_list = self:GetTaskListAcceptedFollowMenu()
        -- if task_accepted_list[1] and task_accepted_list[1]:TaskMenuConditionPause() then
            if task_accepted_list[2] then task_id = task_accepted_list[2].task_id end
        -- end
    end

    self.task_ing_eff.val = task_id
end

function TaskData:CheckMenuGuideFlush()
    self.menu_guide.is_flush = not self.menu_guide.is_flush
end

function TaskData:MenuToTaskId(task_id)
    TaskData.Instance.menu_to_task:Set{to_task_id = task_id}
end

function TaskData:StartTaskChain()
    local taskvo = CommonStruct.CustomTask()
    taskvo.task_id = CUSTOM_OBJID.TASK_CHAIN_START
    taskvo.task_name = Language.Task.TaskName.TaskChain
    taskvo.task_desc = Language.Task.TaskGoal.TaskChainStart
    taskvo.task_goal = Language.Task.TaskGoal.TaskChainStart
    return taskvo
end

function TaskData:SetRAShanHaiOriginInfo(protocol)
    local info = protocol and protocol.info or {}
    local not_open = ActStatusType.Open ~= ActivityData.Instance:GetActivityStatus(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_TASK)
    local level_not_enough = RoleData.Instance:GetRoleLevel() < self.config_shanhai_other.level_open
    local task_prefix1 = ""
    local task_prefix2 = ""
    local show_items = {}
    if nil ~= self.task_shanhai_origin_handle then
        BehaviorData.Instance:RemoveCustomNpcTalkBtnsSeq(self.task_shanhai_origin_handle)
        self.task_shanhai_origin_handle = nil
    end
    local show_cfg = ServerActivityData.Instance:GetOpenConfigStartTime(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_TASK, Config.shanhai_origin_auto.task_prefix)
    if show_cfg ~= nil and show_cfg[1] ~= nil then
        task_prefix1 = show_cfg[1].task_prefix_1
        task_prefix2 = show_cfg[1].task_prefix_2
        show_items = show_cfg[1].show_item
    end
    if not_open or TaskConfig.HelpOthersStatus.finish == info.task_status or level_not_enough or StringIsEmpty(task_prefix1) then
        if self.task_list[CUSTOM_OBJID.SHAN_HAI_ORIGIN] then
            self.task_list[CUSTOM_OBJID.SHAN_HAI_ORIGIN]:RemoveHandle()
        end
        self:TaskInfoDeleteByTaskId(CUSTOM_OBJID.SHAN_HAI_ORIGIN)
        return
    end
    if 0 == info.task_id then
        info.task_id = 1
        info.task_status = TaskConfig.HelpOthersStatus.commit
    end
    local co = Cfg.ShanHaiOriginTaskId(info.task_id)
    co.task_prefix1 = task_prefix1
    co.task_prefix2 = task_prefix2
    if 1 == info.task_id then
        local start_npc = co.commit_npc
        if start_npc and start_npc > 0 then
            self.task_shanhai_origin_handle = BehaviorData.Instance:CustomNpcTalkBtnsSeq(start_npc, function (btn_list,obj_vo)
                if obj_vo and obj_vo.seq == start_npc and btn_list then
                    local rewards = {}
                    for i = 0, #show_items do
                        local item_data = show_items[i]
                        table.insert(rewards, Item.Init(item_data.item_id, item_data.num, item_data.is_bind))
                    end
                    local detail_tips = Language.Task.HelpOthers.detail_tips2
                    local all_count = self.config_shanhai_other.ring_up
                    local button = NpcDialogBehBtnData.FuncCreate(
                        task_prefix1,
                        BindTool.Bind(TaskCtrl.Instance.SendRAShanHaiOriginStart, TaskCtrl.Instance),
                        true, NpcDialogData.CreateDetailInfo1{tips = {string.format(detail_tips[1], all_count), string.format(detail_tips[2], all_count)}, rewards = rewards}
                    )
                    table.insert(btn_list, button)
                end 
            end)
        end
    elseif TaskConfig.HelpOthersStatus.accept == info.task_status then
        local accept_npc = co.accept_npc
        if accept_npc and accept_npc > 0 then
            self.task_shanhai_origin_handle = BehaviorData.Instance:CustomNpcTalkBtnsSeq(accept_npc, function (btn_list,obj_vo)
                if obj_vo and obj_vo.seq == accept_npc and btn_list then
                    local button = NpcDialogBehBtnData.FuncCreate(
                        co.task_name,
                        function ()
                            NpcDialogCtrl.Instance:NpcDialogView(
                                {
                                    talk_id = co.accept_talk_id,
                                },
                                BindTool.Bind(TaskCtrl.Instance.SendRAShanHaiOriginAccept, TaskCtrl.Instance)
                            )
                        end,
                        false
                    )
                    table.insert(btn_list, button)
                end 
            end)
        end
    elseif TaskConfig.HelpOthersStatus.commit == info.task_status then
        local commit_npc = co.commit_npc
        if commit_npc and commit_npc > 0 then
            local auto_close_npc_dialog = function ()
                local value = false
                if 4 == co.task_type then
                    local pet_id = co.param_1
                    local items = {}
                    if pet_id > 0 then
                        for _, pet in pairs(PetData.Instance.pet_list) do
                            local pet_state = pet:State()
                            if pet:ID() == pet_id and PetState.Relax == pet_state and not pet:ArenaState() then
                                table.insert(items, pet)
                            end
                        end
                    end
                    value = not next(items)
                elseif 2 == co.task_type or 5 == co.task_type then
                    local item_id = co.param_1
                    value = not (BagData.Instance:GetNumByItemId(item_id) >= co.param_2)
                end
                return value
            end
            local end_func = function ()
                if 1 == co.task_type then
                    NpcDialogCtrl.Instance:NpcDialogView(
                        {
                            talk_id = co.commit_talk_id,
                        },
                        BindTool.Bind(TaskCtrl.SendRAShanHaiOriginTalk,TaskCtrl.Instance)
                    )
                elseif 2 == co.task_type or 4 == co.task_type or 5 == co.task_type then
                    local is_enough = false
                    local item_id, pet_id = 0, 0
                    if 4 == co.task_type then
                        pet_id = co.param_1
                        local pet_name = Pet.New{pet_id = pet_id}:Name()
                        local items = {}
                        if pet_id > 0 then
                            for _, pet in pairs(PetData.Instance.pet_list) do
                                local pet_state = pet:State()
                                if pet:ID() == pet_id and PetState.Relax == pet_state and not pet:ArenaState() then
                                    table.insert(items, pet)
                                end
                            end
                        end
                        is_enough = next(items)
                        if not is_enough then
                            local storage_have = false
                            if pet_id > 0 then
                                for _, pet in pairs(PetData.Instance.pet_storage_list) do
                                    if pet:ID() == pet_id then
                                        storage_have = true
                                        break
                                    end
                                end
                            end
                            local co = Cfg.ChainPetGroupByPI(pet_id)
                            if nil == co then
                                LogError(string.format("商人的烦恼 pet_id:%s 找不到巡逻场景", pet_id))
                                return
                            end
                            local scene_id = co.scene_id
                            local goto_scene = function ()
                                BehaviorCtrl.ExeByCfgVo({
                                    behavior_type = 9,    --behavior_type 是 beh_id_to_tab.key
                                    param1 = scene_id,
                                    param2 = "",
                                    param3 = "",
                                })
                                ViewMgr:OpenView(FightAutoSealSetView,{["scene_id"] = scene_id})
                                ViewMgr:CloseView(DialogTipsView)
                            end
                            if storage_have == false then
                                PublicPopupCtrl.Instance:DialogTips{
                                    content = string.format(Language.TaskChain.NotEnoughTips[4], pet_name, SceneData.Instance:GetSceneCfg(scene_id).name),
                                    confirm = {func = goto_scene}
                                }
                            else
                                PublicPopupCtrl.Instance:DialogTips{
                                    content = string.format(Language.TaskChain.NotEnoughTips[5], pet_name, SceneData.Instance:GetSceneCfg(scene_id).name),
                                    confirm = {
                                        func = function ()
                                            ViewMgr:CloseView(DialogTipsView)
                                            ViewMgr:OpenView(PetStorage)
                                        end
                                    },
                                    cancel = {func = goto_scene}
                                }
                            end
                        end
                    else
                        item_id = co.param_1
                        is_enough = BagData.Instance:GetNumByItemId(item_id) >= co.param_2
                        if not is_enough then
                            local is_medicine = ItemType.Medicine == Item.GetType(item_id)
                            local has_get_way = "number" == type(co.get_way)
                            local co_get_way = has_get_way and Config.getway_cfg_auto.get_way[co.get_way] or {}
                            PublicPopupCtrl.Instance:DialogTips{
                                content = string.format(Language.TaskChain.NotEnoughTips[2][has_get_way and 100 or (is_medicine and 1 or 0)], Item.GetQuaName(item_id), has_get_way and co_get_way.desc or ""),
                                confirm = {
                                    func = function()
                                        if is_medicine then
                                            WorkshopData.Instance:SetLifeItemQuickJump(item_id)
                                            ViewMgr:OpenViewByKey(Mod.Workshop.NeoCompose)
                                            ViewMgr:CloseView(DialogTipsView)
                                            return
                                        end
                                        if has_get_way then
                                            GetWayData.OpenPanel(co_get_way, Item.Create({item_id = item_id, jump_need_num = co.param_2}))
                                            ViewMgr:CloseView(DialogTipsView)
                                            return
                                        elseif Item.GetType(item_id) == 18 then
                                            TradeData.Instance:SetSearchItemId(item_id)
                                            ViewMgr:OpenViewByKey(Mod.Mall.Trade)
                                            ViewMgr:CloseView(DialogTipsView)
                                            return
                                        end
                                        TaskData.shop_auto_close = true
                                        MallCtrl.Instance:Jump(item_id)
                                        ViewMgr:CloseView(DialogTipsView)
                                    end
                                }
                            }
                        end
                    end
                    if is_enough then
                        TaskChainItemSel.item_id = item_id
                        TaskChainItemSel.pet_id = pet_id
                        TaskChainItemSel.from_type = TaskConfig.TaskChainItemSelFrom.shan_hai
                        NpcDialogCtrl.Instance:NpcDialogView(
                                {
                                    talk_id = co.commit_talk_id,
                                },
                                BindTool.Bind(ViewMgr.OpenView, ViewMgr, TaskChainItemSel)
                            )
                    end
                elseif 3 == co.task_type then
                    NpcDialogCtrl.Instance:NpcDialogView(
                        {
                            talk_id = co.commit_talk_id,
                        },
                        BindTool.Bind(TaskCtrl.SendRAShanHaiOriginBattle,TaskCtrl.Instance)
                    )
                end
            end
            self.task_shanhai_origin_handle = BehaviorData.Instance:CustomNpcTalkBtnsSeq(commit_npc, function (btn_list,obj_vo)
                if obj_vo and obj_vo.seq == commit_npc and btn_list then
                    local button = NpcDialogBehBtnData.FuncCreate(
                        co.task_name,
                        end_func,
                        auto_close_npc_dialog
                    )
                    table.insert(btn_list, button)
                end
            end)
        end
    end
    local task = self.task_list[CUSTOM_OBJID.SHAN_HAI_ORIGIN] or SmartData.New(TaskInfoHelpOthers.New():TaskInfo())
    local progress = string.format("(%s/%s)", TaskConfig.HelpOthersStatus.accept == info.task_status and info.pass_count or (info.pass_count + 1), self.config_shanhai_other.ring_up)
    info.task_type = CUSTOM_OBJID.SHAN_HAI_ORIGIN
    task:TaskVo(info, co, progress)
    self:TaskInfoUpdateByTaskId(CUSTOM_OBJID.SHAN_HAI_ORIGIN, task)

    --56638
    if 3 == co.task_type and info.progress >= co.param_2 then
        AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(task)}, 1)
    end
end

function TaskData:BranchSerialTaskAutoSel()
    local task = self:GetTaskInfoByTaskType(TaskConfig.TaskType.branch_serial)
    if task then
        local task_id = task.task_id
        local co = Config.tasklist_auto.chain_go_on[task_id]
        if co then
            self.branch_serial_task_sm.info:Set(Cfg.TaskListChainShowByGroupChapter(co.task_group, co.chain_chapter))
        end
    end
end

function TaskData:BranchMarbasTaskAutoSel()
    local task = self:GetTaskInfoByTaskType(TaskConfig.TaskType.branch_marbas)
    if task then
        local task_id = task.task_id
        local co = Config.tasklist_auto.chain_go_on[task_id]
        if co then
            self.branch_marbas_task_sm.info:Set(Cfg.TaskListChainShowByGroupChapter(co.task_group, co.chain_chapter))
        end
    end
end

function TaskData:BranchRelicTaskAutoSel()
    local task = self:GetTaskInfoByTaskType(TaskConfig.TaskType.branch_relic)
    if task then
        local task_id = task.task_id
        local co = Config.tasklist_auto.chain_go_on[task_id]
        if co then
            self.branch_relic_task_sm.info:Set(Cfg.TaskListChainShowByGroupChapter(co.task_group, co.chain_chapter))
        end
    end
end

function TaskData:GetBranchTaskChainShow(task_group)
    local list = {}
    for k, v in ipairs(Config.tasklist_auto.chain_show) do
        if task_group == v.task_group then
            table.insert(list, v)
        end
    end
    return list
end