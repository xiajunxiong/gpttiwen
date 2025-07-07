FILE_REQ("modules/task/task_config")
FILE_REQ("modules/task/task_info")
FILE_REQ("modules/task/task_data")
FILE_REQ("modules/task/task_chain")
VIEW_REQ("modules/task/task_view")
VIEW_REQ("modules/task/task_other_info")
FILE_REQ("modules/daily_act_guide/daily_act_guide_task")

VIEW_REQ("modules/task/task_menu_view")
VIEW_REQ("modules/task/taskchain_item_sel")
VIEW_REQ("modules/task/bounty_task_reward_get_view")
VIEW_REQ("modules/task/task_prestige_view")
VIEW_REQ("modules/task/element_guide_view")
VIEW_REQ("modules/task/task_prestige_star_view")

VIEW_REQ("modules/task/items_sub_data")
VIEW_REQ("modules/task/items_sub_view")

VIEW_REQ("modules/task/task_preview_ml_view")

VIEW_REQ("modules/task/branch_serial_task_view")
VIEW_REQ("modules/task/branch_marbas_task_view")
VIEW_REQ("modules/task/branch_relic_task_view")

FILE_REQ("modules/task/du_chuang_zei_ying/du_chuang_zei_ying_ctrl")
FILE_REQ("modules/task/wen_xiang_lou/wen_xiang_lou_ctrl")
FILE_REQ("modules/task/treasure_task/treasure_task_ctrl")
FILE_REQ("modules/task/bounty_task/bounty_task_ctrl")

TaskCtrl = TaskCtrl or BaseClass(BaseCtrl)

function TaskCtrl:__init()
    if TaskCtrl.Instance ~= nil then
        Debuger.LogError("[TaskCtrl] attempt to create singleton twice!")
        return
    end
    TaskCtrl.Instance = self
    self.data = TaskData.New()
    self.items_sub_data = ItemsSubData.New()

    self:RegisterAllProtocols()
    self:InitTaskCfg()
end

function TaskCtrl:InitTaskCfg()
    self.task_cfgs = {
        [1] = Config.tasklist_auto.task_list,
        [2] = Config.challengetask_list_auto.task_list,
        [3] = Config.medaltask_list_auto.task_list, 
        [4] = Config.strange_tasklist_auto.task_list,
        [5] = Config.advanced_tasks_auto.task_list, 
        [6] = Config.bounty_tasklist_auto.task_list, 
        [7] = Config.transaction_task_auto.task_list, 
        [8] = Config.feisheng_tasklist_auto.task_list,
        [9] = Config.pet_fs_tasklist_auto.task_list, 
        [10] = Config.partner_fs_tasklist_auto.task_list, 
        [11] = Config.huanshi_challenge_auto.task_list, 
        [12] = Config.family_daily_mission_auto.mission_list,
        [13] = Config.prestige_task_auto.task_line,
    }
    for i, v in pairs(self.task_cfgs) do
        local name = "GetTaskCfgByNpcSeq" .. i
        Cfg.Register(name, Cfg.RealTimeSearch(v, "is_show_npc"))
    end
end

function TaskCtrl:GetTaskCfgByNpcSeq(is_show_npc)
    local task_cfg={}
    for i,v in pairs(self.task_cfgs) do
        local cfg=Cfg["GetTaskCfgByNpcSeq" .. i](is_show_npc,false)
        if cfg then
            task_cfg= cfg
            break
        end
    end
    return task_cfg
end

function TaskCtrl:OnInit()
    ActivityData.Instance:CustomClickHandle(ActType.BountyTask, BindTool.Bind(self.data.GoTaskByTaskType, self.data, TaskConfig.TaskType.bounty))
    -- ActivityRandData.Instance:CustomClickHandle(ACTIVITY_TYPE.RAND_HELP_OTHERS, BindTool.Bind(self.data.GoTaskHelpOthers, self.data))
    ActivityData.Instance:SetActivityFinishInfo(ActType.TaskChain, 0, 50)
    self.handle_level_change = EventSys:Bind(GlobalEventKeys.RoleLevelChange, BindTool.Bind(self.RoleLevelChange, self))
    self.handle_role_notice = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.RoleNoticeComplete, self))
    self.handle_act = ActivityData.Instance.activity_status_event:KeysCare({"act_type", "status"},BindTool.Bind(self.OnActEvent, self))
    RoleData.Instance:GetBaseData():Care(BindTool.Bind(self.TryBeginAutoTaskImmediately,self),"level")
    ActivityData.Instance:RegisterCondition(ActType.TaskChain,function()
        return self:TaskChainCondition()
	end)
    self.role_pos_event = EventSys:Bind(GlobalEventKeys.RolePosChange, BindTool.Bind(self.OnMainRolePosChange, self))
end

function TaskCtrl:__delete()
    if TaskCtrl.Instance == self then
        TaskCtrl.Instance = nil
    end
    EventSys:UnBind(self.handle_level_change)
    EventSys:UnBind(self.handle_role_notice)
    self:UnRegisterAllProtocols()
    TimeHelper:CancelTimer(self.handle_delay)
end

function TaskCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSTaskInfoSingleHandle)
    self:RegisterProtocol(CSTalkWithAppointedNPC)
    self:RegisterProtocol(CSGiveNPCTaskNeedItems)
    self:RegisterProtocol(CSTaskCommitMoneyReq)
    self:RegisterProtocol(CSRoleTaskPathFinding)
    self:RegisterProtocol(CSOnekeyFinish)
    self:RegisterProtocol(CSTaskSkipReq)

    self:RegisterProtocol(SCTaskDetailedInfoList, "OnTaskDetailedInfoList", true)
    self:RegisterProtocol(SCTaskDeleteNotifyList, "OnTaskDeleteNotifyList", true)
    self:RegisterProtocol(SCTaskCanAbandonedList, "OnTaskCanAbandonedList")
    self:RegisterProtocol(SCTaskSingleStatusInfo, "OnTaskSingleStatusInfo")

    self:RegisterProtocol(SCTaskWaitingList, "OnTaskWaitingList")

    self:RegisterProtocol(CSTaskTraceStatus)
    self:RegisterProtocol(SCTaskTraceStatus, "SCTaskTraceStatus")

    self:RegisterProtocol(CSTaskChainOp)
    self:RegisterProtocol(SCTaskChainInfo, "SCTaskChainInfo")
    self:RegisterProtocol(SCTaskChainMonsterInfo, "SCTaskChainMonsterInfo")
    self:RegisterProtocol(SCTaskChainTurnTabInfo, "SCTaskChainTurnTabInfo")

    self:RegisterProtocol(SCTaskItemLackForCommitNotice, "OnTaskItemLackForCommitNotice")
    self:RegisterProtocol(SCTaskItemCommitNotice, "OnTaskItemCommitNotice")

    self:RegisterProtocol(CSBountyTaskTurnTableRewardFetch)
    self:RegisterProtocol(SCBountyTaskTurnTableRewardInfo, "OnBountyTaskTurnTableRewardInfo")
    self:RegisterProtocol(SCBountyTaskInfo, "OnBountyTaskInfo")

    -- self:RegisterProtocol(SCRAHelpOthersInfo, "OnRAHelpOthersInfo")
    self:RegisterProtocol(SCRAHelpOthersInfo2, "OnRAHelpOthersInfo")
    self:RegisterProtocol(SCRAShanHaiOriginInfo, "OnRAShanHaiOriginInfo")

    --声望任务
    self:RegisterProtocol(SCPrestigeTaskInfo, "OnSCPrestigeTaskInfo")
    self:RegisterProtocol(SCPrestigeTaskFinalNotice, "OnSCPrestigeTaskFinalNotice")
    self:RegisterProtocol(CSPrestigeTaskSelectVillage)
    self:RegisterProtocol(CSPrestigeTaskReq)
    
    -- 任务小游戏
    self:RegisterProtocol(CSTaskGameReq)
end
function TaskCtrl:OnUnloadGameLogic()
    self.data:Clear()
    ActivityData.Instance:SetActivityFinishInfo(ActType.TaskChain, 0, 50)
    -- EventSys:UnBind(self.handle_level_change)
    -- EventSys:UnBind(self.handle_role_notice)
    TimeHelper:CancelTimer(self.handle_delay)
    self:CancelAutoMainTimeHandle()
    self.data:SetRAHelpOthersInfo({ info = 
        {
            task_status = TaskConfig.HelpOthersStatus.finish
        }
    })
    self.data:SetRAShanHaiOriginInfo({ info = 
        {
            task_status = TaskConfig.HelpOthersStatus.finish
        }
    })
end

function TaskCtrl:RoleNoticeComplete()
    self:RoleLevelChange1()
    self.data:FlushMenuGuide()
end

function TaskCtrl:RoleLevelChange()
    self:RoleLevelChange1()
    self:RoleLevelChange2()
    self:OnRAHelpOthersInfo(self.protocol_help_others)
    self:OnRAShanHaiOriginInfo(self.protocol_shanhai_info)
    -- local auto_main, wait_s = GuideData.Instance:GetAutoMainTask()
    -- self:TryBeginAutoTaskImmediately()
end

function TaskCtrl:RoleLevelChange2()
    self.data:AutoMainLineTask()
end

function TaskCtrl:OnMainRolePosChange()
        self.last_move_frame = Time.frameCount
--     local auto_main, wait_s = GuideData.Instance:GetAutoMainTask()
--     if self.auto_main_time_handle ~= nil then
--         self.auto_main_time_handle = TimeHelper:CancelTimer(self.auto_main_time_handle)
--     end
--     LogError("OnMainRolePosChange===",auto_main)

--     if auto_main and 
--         SceneData.Instance:SceneConfig() and SceneData.Instance:SceneConfig().scene_type == SCENE_TYPE.COMMON_SCENE then
--         self.auto_main_time_handle = TimeHelper:AddDelayTimer(BindTool.Bind(self.AutoTaskImmediately, self), wait_s) 
--     end
end

function TaskCtrl:TryBeginAutoTaskImmediately()
    local auto_main, wait_s = GuideData.Instance:GetAutoMainTask()
    if self.auto_main_time_handle ~= nil then
        if auto_main == false then
            self:CancelAutoMainTimeHandle()
        end
        return
    end
    if auto_main then
        self.auto_main_time_handle = TimeHelper:AddRunTimer(BindTool.Bind(self.AutoTaskImmediately, self),wait_s,-1,true)
    end
end

-- 立刻执行自动任务
function TaskCtrl:AutoTaskImmediately()
    if self.last_move_frame ~= nil and math.abs(Time.frameCount - self.last_move_frame) <= 2 then   --两帧内有移动则不自动
        return
    end
    if SceneData.Instance:SceneConfig() == nil or SceneData.Instance:SceneConfig().scene_type ~= SCENE_TYPE.COMMON_SCENE then
        return
    end
    local task = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.mainline)
    --LogError("AutoTaskImmediately====", GuideManager.Instance:IsGuide(),ViewMgr:IsOpenLtNotAutoTaskView(),task == nil and "NIL" or task.task_id)
    if task and not GuideManager.Instance:IsGuide() and not ViewMgr:IsOpenLtNotAutoTaskView() and 170 ~= task.task_id
    and not BattleData.Instance:IsServerFighting() then
        task:GoTask()
    -- else
    --     self:OnMainRolePosChange()
    end
end

function TaskCtrl:CancelAutoMainTimeHandle()
    -- LogError("CancelAutoMainTimeHandle")
    if self.auto_main_time_handle ~= nil then
        self.auto_main_time_handle = TimeHelper:CancelTimer(self.auto_main_time_handle)
        self.auto_main_time_handle = nil
    end
end

function TaskCtrl:RoleLevelChange1()
    --判断等级和是否有任务
    local level = RoleData.Instance:GetRoleLevel()
    if level >= Config.task_chain_auto.other[1].level then
        local info = self.data:GetTaskInfoByTaskType(TaskConfig.TaskType.task_chain)
        local bounty_task_info = self.data.chain_bounty_task_info
        if (info == nil or info.TaskKey == "TaskInfoChainStage" or info.TaskKey == "TaskInfoChainReward") and bounty_task_info.group_id and bounty_task_info.group_id == 0 then--满足等级且没有奖励可领
        --判断是否满足下一个阶段
            local data = self.data.task_chain_data
            --等级不足以做下一阶段
            if data.count > 0 then
                local next_lv = Config.task_chain_auto.level_times[data.count].max_level + 1
                if level >= next_lv then
                    local task = SmartData.New()
                    local task_info = TaskInfoChainStart.New()
                    task:Set(task_info:TaskInfo())
                    task:TaskVo(self.data:TaskChainContinue())
                    self.data:TaskInfoUpdateByTaskId(CUSTOM_OBJID.TASK_CHAIN_START, task)
                elseif data.count < 3 then
                    local task = SmartData.New()
                    local task_info = TaskInfoChainStage.New()
                    task:Set(task_info:TaskInfo())
                    task:TaskVo(self.data:TaskChainStage(data.count))
                    self.data:TaskInfoUpdateByTaskId(CUSTOM_OBJID.TASK_CHAIN_START, task)
                end
            else
                local task = SmartData.New()
                local task_info = TaskInfoChainStart.New()
                task:Set(task_info:TaskInfo())
                task:TaskVo(self.data:StartTaskChain())
                self.data:TaskInfoUpdateByTaskId(CUSTOM_OBJID.TASK_CHAIN_START, task)
            end
        end
    else
        self.data:TaskInfoDeleteByTaskId(CUSTOM_OBJID.TASK_CHAIN_START)
    end
end

function TaskCtrl:OnActEvent()
    if ACTIVITY_TYPE.RAND_HELP_OTHERS == ActivityData.Instance.activity_status_event.act_type then
        local activity_status = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_HELP_OTHERS)
        local status = activity_status and activity_status.status or ActStatusType.Close
        if ActStatusType.Close == status then
            self.data:SetRAHelpOthersInfo()
        end
    end
    if ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_TASK == ActivityData.Instance.activity_status_event.act_type then
        local activity_status = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_TASK)
        local status = activity_status and activity_status.status or ActStatusType.Close
        if ActStatusType.Close == status then
            self.data:SetRAShanHaiOriginInfo()
        end
    end
end

function TaskCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(SCTaskDetailedInfoList)
    self:UnRegisterProtocol(SCTaskDeleteNotifyList)
    self:UnRegisterProtocol(SCTaskCanAbandonedList)
    self:UnRegisterProtocol(SCTaskSingleStatusInfo)
end

function TaskCtrl:CSTaskChainOp(type, param1, param2)
    local protocol = GetProtocol(CSTaskChainOp)
    protocol.type = type
    protocol.param1 = param1 or 0
    protocol.param2 = param2 or 0
    SendProtocol(protocol)
end

function TaskCtrl:SCTaskChainInfo(protocol)
    if protocol.info.is_new == 1 then
        if self.task_chain_monster_vo then
            VoMgr:DelMonster(self.task_chain_monster_vo.obj_id)
            self.task_chain_monster_vo = nil
        end
    end
    self.need_create_monster = protocol.info.task_type == TASK_CHAIN_TASK_TYPE.FIGHT
    TimeHelper:CancelTimer(self.handle_delay)
    self.handle_delay = TimeHelper:AddDelayFrameTimer(function ()
        self:CreateTaskChainMonster()
    end, 1)

    local total_num = protocol.info.cur_task_max_count --Config.task_chain_auto.other[1].weekly_num * 150
    local cur_num = protocol.info.task_pass_count--protocol.info.task_count * total_num + protocol.info.pass_count
    if (cur_num - self.data.task_chain_data.cur) == 1 and protocol.info.task_type == 0 then
        local npc_id = Config.task_chain_auto.other[1].start_npc
        NpcDialogCtrl.Instance:NpcDialogView(
            {
                talk_list = {
                    {
                        talker =  npc_id,
                        talk_text = string.format(Language.TaskChain.FinishTips, cur_num)
                    },
                    talker = npc_id,
                },
                beh_btn_list = {}
            },
            function ()
                SceneCtrl.Instance:Logic():AutoToNpc(npc_id, function ()
                    -- ViewMgr:OpenView(BountyTaskRewardGetView, {title = Language.Task.TaskChainReward })
                    TaskCtrl.Instance:CSTaskChainOp(TASK_CHAIN_OP_TYPE.FETCH_REWARD)
                end, false, true)
            end
        )
    end
    self.data.task_chain_data.cur = cur_num
    self.data.task_chain_data.total = total_num
    self.data.task_chain_data.count = protocol.info.task_count

    ActivityData.Instance:SetActivityFinishInfo(ActType.TaskChain, cur_num , 50)--注意

    if cur_num >= total_num then
        self.data:TaskInfoDeleteByTaskId(CUSTOM_OBJID.TASK_CHAIN_START)
    end
    if protocol.info.task_type == 0 then
        self.data:UpdateTaskChainFakeTask(protocol.info)
        ActivityData.Instance:CustomClickHandle(
            ActType.TaskChain,
            function()
                if self.data.task_list[CUSTOM_OBJID.TASK_CHAIN_START] then
                    self.data.task_list[CUSTOM_OBJID.TASK_CHAIN_START]:GoTask()
                    self.data:AfterGoTask(TaskConfig.TaskType.task_chain)
                    return
                end
                local cfg = BehaviorCtrl.GetCfg(4, true)
                local npc_id = Config.task_chain_auto.other[1].start_npc
                cfg.param1 = npc_id
                cfg.behavior_type = 6
                BehaviorCtrl.ExeByCfgVo(cfg)
            end
        )
        if ViewMgr:IsOpen(BountyTaskRewardGetView) then
            ViewMgr:CloseView(BountyTaskRewardGetView)
        end
    else
        self.data:TaskInfoDeleteByTaskId(CUSTOM_OBJID.TASK_CHAIN_START)
        self.data:UpdateTaskChain(protocol.info)
        ActivityData.Instance:CustomClickHandle(
            ActType.TaskChain,
            function()
                self.data.task_list[CUSTOM_OBJID.TASK_CHAIN]:GoTask()
            end
        )
    end
    self:RoleLevelChange1()
end

function TaskCtrl:CSTaskTraceStatus()
    local protocol = GetProtocol(CSTaskTraceStatus)
    protocol.list = self.data:GetTaskTraceStatusList()
    SendProtocol(protocol)
end

function TaskCtrl:SCTaskTraceStatus(protocol)
    self.data:SetTaskTraceStatus(protocol.list)
end

function TaskCtrl:OnTaskDetailedInfoList(protocol)
    self.data:SetTaskDetailedInfoList(protocol)
    RoleData.Instance:ChangePromoteRedPiont()
end

function TaskCtrl:OnTaskDeleteNotifyList(protocol)
    self.data:SetTaskDeleteNotifyList(protocol)
    RoleData.Instance:ChangePromoteRedPiont()
    ConspiracyData.Instance:CheckDaiyInfo()
end

function TaskCtrl:OnTaskCanAbandonedList(protocol)
    self.data:SetTaskCanAbandonedList(protocol)
end

function TaskCtrl:OnTaskSingleStatusInfo(protocol)
    self.data:SetTaskSingleStatusInfo(protocol)
    RoleData.Instance:ChangePromoteRedPiont()
    RoleData.Instance:ChangeSikllRedPiont()
end

function TaskCtrl:OnTaskWaitingList(protocol)
    self.data:SetTaskWaitingList(protocol)
end

function TaskCtrl:OnTaskItemLackForCommitNotice(protocol)
    for i = 1, protocol.count do
        local info = protocol.item_list[i]
        PublicPopupCtrl.Instance:Center(string.format(Language.Task.SubTips.fail, Item.GetQuaName(info.item_id), info.num))
    end
end

function TaskCtrl:OnTaskItemCommitNotice(protocol)
    for i = 1, protocol.count do
        local info = protocol.item_list[i]
        local item = Item.Init(info.item_id)
        local content = string.format(Language.Task.SubTips.succ, item:QuaName(), info.num)
        PublicPopupCtrl.Instance:Center(content)
        ChatCtrl.Instance:ChannelChatCustom {task_sub = {item = item, num = info.num}}
    end
end

function TaskCtrl:OnBountyTaskTurnTableRewardInfo(protocol)
    if protocol.bounty_task_info.group_id > 0 then
        self.data:SetBountyTaskTurnTableRewardInfo(protocol)
        ViewMgr:OpenView(BountyTaskRewardGetView, {title = Language.Task.BountyTaskReward, close_func = function ()
            -- if RoleData.Instance:CreatedDay() <= 1 then
            --     -- TaskData.Instance:StopTaskState(TaskConfig.TaskType.silk_road)
            --     if FunOpen.Instance:GetFunIsOpened(Mod.TreasureTask.Main) then
            --         TreasureTaskData.Instance:AccpetGoTask()
            --     end
            -- end
        end})
    end
end

function TaskCtrl:OnBountyTaskInfo(protocol)
    self.data:SetBountyTaskInfo(protocol)
end

function TaskCtrl:OnRAHelpOthersInfo(protocol)
    self.protocol_help_others = protocol
    self.data:SetRAHelpOthersInfo(protocol)
end
function TaskCtrl:OnRAShanHaiOriginInfo(protocol)
    self.protocol_shanhai_info = protocol
    self.data:SetRAShanHaiOriginInfo(protocol)
end

function TaskCtrl:SCTaskChainTurnTabInfo(protocol)
    local info = self.data:GetTaskInfoByTaskType(TaskConfig.TaskType.task_chain)
    if (info == nil or info.TaskKey == "TaskInfoChainReward") and protocol.bounty_task_info.group_id == 0 then
        local data = self.data.task_chain_data
        if data.count < 3 and data.count > 0 then
            --等级不足以做下一阶段
            local level = RoleData.Instance:GetRoleLevel()
            local next_lv = Config.task_chain_auto.level_times[data.count].max_level + 1
            if level < next_lv then
                local task = SmartData.New()
                local task_info = TaskInfoChainStage.New()
                task:Set(task_info:TaskInfo())
                task:TaskVo(self.data:TaskChainStage(data.count))
                self.data:TaskInfoUpdateByTaskId(CUSTOM_OBJID.TASK_CHAIN_START, task)
            else
                -- self.data:UpdateTaskChainFakeTask()
                self.data:TaskInfoDeleteByTaskId(CUSTOM_OBJID.TASK_CHAIN_START)
            end
        elseif data.count == 3 then
            -- self.data:UpdateTaskChainFakeTask()
            self.data:TaskInfoDeleteByTaskId(CUSTOM_OBJID.TASK_CHAIN_START)
        end
    end
    self.data:SetChainTaskTurnTableRewardInfo(protocol)
    if protocol.bounty_task_info.group_id > 0 then
        self.data:SetBountyTaskTurnTableRewardInfo(protocol, true)
        --这里每50环之后都会下发，但是不主动打开界面提示可以领取奖励
        local task = SmartData.New()
        local task_info = TaskInfoChainReward.New()
        task:Set(task_info:TaskInfo())
        task:TaskVo(self.data:TaskChainReward(self.data.task_chain_data.cur))
        self.data:TaskInfoUpdateByTaskId(CUSTOM_OBJID.TASK_CHAIN_START, task)
        -- ViewMgr:OpenView(BountyTaskRewardGetView, {title = Language.Task.TaskChainReward })
    end
    self:RoleLevelChange1()
end
function TaskCtrl:SendTaskInfoSingleHandle(param_t)
    local protocol = GetProtocol(CSTaskInfoSingleHandle)
    protocol.oper_type = param_t.oper_type or 0
    protocol.task_id = param_t.task_id or 0
    SendProtocol(protocol)
end

function TaskCtrl:SendTaskAccept(task_id)
    LogError(" TaskCtrl:SendTaskAccept===",task_id)
    local param_t = {
        oper_type = TaskConfig.OperType.accept,
        task_id = task_id
    }
    self:SendTaskInfoSingleHandle(param_t)
end

function TaskCtrl:SendTaskGiveUp(task_id)
    local param_t = {
        oper_type = TaskConfig.OperType.give_up,
        task_id = task_id
    }
    self:SendTaskInfoSingleHandle(param_t)
end

function TaskCtrl:SendTaskCommit(task_id)
    local param_t = {
        oper_type = TaskConfig.OperType.commit,
        task_id = task_id
    }
    self:SendTaskInfoSingleHandle(param_t)
end

function TaskCtrl:SendTaskDemand(task_id)
    local param_t = {
        oper_type = TaskConfig.OperType.demand,
        task_id = task_id
    }
    self:SendTaskInfoSingleHandle(param_t)
end

function TaskCtrl:SendTaskPatrol(task_id)
    local param_t = {
        oper_type = TaskConfig.OperType.task_patrol,
        task_id = task_id
    }
    self:SendTaskInfoSingleHandle(param_t)
end

function TaskCtrl:SendTaskSkipBattle(task_id)
    local param_t = {
        oper_type = TaskConfig.OperType.skip_battle,
        task_id = task_id
    }
    self:SendTaskInfoSingleHandle(param_t)
end

function TaskCtrl:SendTalkWithAppointedNPC(param_t)
    local protocol = GetProtocol(CSTalkWithAppointedNPC)
    protocol.task_id = param_t.task_id or 0
    protocol.npc_seq = param_t.npc_seq or 0
    SendProtocol(protocol)
end

function TaskCtrl:SendGiveNPCTaskNeedItems(param_t)
    local protocol = GetProtocol(CSGiveNPCTaskNeedItems)
    protocol.task_id = param_t.task_id or 0
    protocol.commit_list = param_t.commit_list or {}
    SendProtocol(protocol)
end

function TaskCtrl:SendTaskCommitMoneyReq(task_id)
    local protocol = GetProtocol(CSTaskCommitMoneyReq)
    protocol.task_id = task_id or 0
    SendProtocol(protocol)
end

function TaskCtrl:SendRoleTaskPathFinding(param_t)
    local protocol = GetProtocol(CSRoleTaskPathFinding)
    protocol.task_id = param_t.task_id or 0
    protocol.is_stop = param_t.is_stop
    SendProtocol(protocol)
end

function TaskCtrl:SendRoleTaskPathFindingStart(task_id)
    local param_t = {
        task_id = task_id,
        is_stop = 0
    }
    self:SendRoleTaskPathFinding(param_t)
end

function TaskCtrl:SendRoleTaskPathFindingStop()
    local param_t = {
        is_stop = 1
    }
    self:SendRoleTaskPathFinding(param_t)
end

function TaskCtrl:SendBountyTaskTurnTableRewardFetch()
    local protocol = GetProtocol(CSBountyTaskTurnTableRewardFetch)
    SendProtocol(protocol)
end

function TaskCtrl:SendOnekeyFinish(finish_type)
    local protocol = GetProtocol(CSOnekeyFinish)
    protocol.op_type = finish_type or 0
    SendProtocol(protocol)
end

function TaskCtrl:SendTaskSkipReq(task_id)
    local protocol = GetProtocol(CSTaskSkipReq)
    protocol.task_id = task_id or 0
    SendProtocol(protocol)
end

local function concatTaskChainText(language, ...)
    return string.format(language[GameMath.Ramdon(1, #language)], ...)
end

function TaskCtrl:SCTaskChainMonsterInfo(protocol)
    local config = Config.monster_group_auto.monster_group[protocol.monster_vo.monster_group_id]
    if config then
        if config.show_monster ~= protocol.monster_vo.monster_id then
            protocol.monster_vo.monster_id = config.show_monster
        end
    end
    self.delay_add_task_chain_monster_vo = CommonStruct.MonsterVo(protocol.monster_vo)
    self.delay_add_task_chain_monster_vo.obj_id = CUSTOM_OBJID.TASK_CHAIN
    self.delay_add_task_chain_monster_vo.beh_cfg = CommonStruct.BehCfg()
    self.delay_add_task_chain_monster_vo.beh_cfg.behavior_type = 1
    self.delay_add_task_chain_monster_vo.beh_cfg.param1 = 1
    self.delay_add_task_chain_monster_vo.beh_cfg.talk_list = {
        {
            talker = self.delay_add_task_chain_monster_vo.id,
            talk_text = concatTaskChainText(Language.TaskChain.Fight)
        },
        talker = self.delay_add_task_chain_monster_vo.id
    }
    self.delay_add_task_chain_monster_vo.beh_cfg.beh_btn_list = {
        NpcDialogBehBtnData.FuncCreate(
            Language.TaskChain.Btn[3],
            function()
                BattleCtrl.BattleModeFightReq(
                    {
                        battle_mode = BATTLE_MODE.BATTLE_MODE_TASK_CHAIN
                    }
                )
            end,
            true
        ),
        NpcDialogBehBtnData.FuncCreate(
            Language.TaskChain.SkipBtn[3],
            function()
                NpcDialogData.Instance:SetCurTaskInfo(nil)
                local task_info = TaskData.Instance.task_list[CUSTOM_OBJID.TASK_CHAIN]
                local cost_type, cost_num = task_info:CostTypeNum()
                local price = string.format(Language.TaskChain.CostPrice[cost_type], cost_num)
                PublicPopupCtrl.Instance:DialogTips(
                    {
                        content = string.format(Language.TaskChain.SkipTips[3], price),
                        confirm = {
                            func = function()
                                self:CSTaskChainOp(TASK_CHAIN_OP_TYPE.SKIP)
                                ViewMgr:CloseView(DialogTipsView)
                            end
                        }
                    }
                )
            end,
            true
        )
    }
    self.recv_monster_info = true
    TimeHelper:CancelTimer(self.handle_delay)
    self.handle_delay = TimeHelper:AddDelayFrameTimer(function ()
        self:CreateTaskChainMonster()
    end, 1)
end

function TaskCtrl:CreateTaskChainMonster()
    if self.recv_monster_info and self.need_create_monster then
        self.task_chain_monster_vo = self.delay_add_task_chain_monster_vo
        VoMgr:AddMonster(self.task_chain_monster_vo)

        self.delay_add_task_chain_monster_vo = nil
        self.recv_monster_info = false
    end
end

-- 查询指定任务的状态
-- 结果返回执行回调func(TaskConfig.TaskState)
function TaskCtrl:DemandTaskState(task_id, func)
    if self.data:DemandTaskState(task_id, func) then
        self:SendTaskDemand(task_id)
    end
end

function TaskCtrl:GetMainViewFuncImgByNpcId(npc_id, img_t)
    return self.data:GetMainViewFuncImgByNpcId(npc_id, img_t)
end

function TaskCtrl:GetNpcDialogTaskButtonListByNpcId(npc_id)
    return self.data:GetNpcDialogTaskButtonListByNpcId(npc_id)
end

function TaskCtrl:GetNpcDialogTaskByNpcId(npc_id)
    return self.data:GetNpcDialogTaskByNpcId(npc_id)
end


function TaskCtrl:GoTask(task_info)
    task_info:GoTask()
end
function TaskCtrl:ItemsSubView(need_check)
    if not ViewMgr:IsOpen(ItemsSubView) then
		--判断是否有其他界面打开
		if need_check == true and ViewMgr:ExistOtherOpenView() then
			self.is_need_chck = true
			return
		end
		ViewMgr:OpenView(ItemsSubView)
	end
end
function TaskCtrl:ViewCloseCheck(mod_key)
	if not ViewMgr:ExistOtherOpenView() and self.is_need_chck then
		ViewMgr:OpenView(ItemsSubView)
		self.is_need_chck = false
    end
    self.data:TaskCloseViewGuide(mod_key)
end


function TaskCtrl:SendRAHelpOthersReq(param_t)
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(
        {
            rand_activity_type = ACTIVITY_TYPE.RAND_HELP_OTHERS,
            opera_type = param_t.opera_type or 0,
            param_1 = param_t.param_1 or 0,
            param_2 = param_t.param_2 or 0,
            param_3 = param_t.param_3 or 0
        }
    )
end

function TaskCtrl:SendRAHelpOthersStart()
    self:SendRAHelpOthersReq(
        {
            opera_type = TaskConfig.HelpOthersOperType.start
        }
    )
end

function TaskCtrl:SendRAHelpOthersAccept()
    self:SendRAHelpOthersReq(
        {
            opera_type = TaskConfig.HelpOthersOperType.accept
        }
    )
end

function TaskCtrl:SendRAHelpOthersItem(column, index)
    self:SendRAHelpOthersReq(
        {
            opera_type = TaskConfig.HelpOthersOperType.item,
            param_1 = column,
            param_2 = index,
        }
    )
end

function TaskCtrl:SendRAHelpOthersTalk()
    self:SendRAHelpOthersReq(
        {
            opera_type = TaskConfig.HelpOthersOperType.talk
        }
    )
end

function TaskCtrl:SendRAHelpOthersPet(pet_index)
    self:SendRAHelpOthersReq(
        {
            opera_type = TaskConfig.HelpOthersOperType.pet,
            param_1 = pet_index,
        }
    )
end

function TaskCtrl:SendRAHelpOthersBattle()
    self:SendRAHelpOthersReq(
        {
            opera_type = TaskConfig.HelpOthersOperType.battle
        }
    )
end

function TaskCtrl:SendRAShanHaiOriginReq(param_t)
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(
        {
            rand_activity_type = ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_TASK,
            opera_type = param_t.opera_type or 0,
            param_1 = param_t.param_1 or 0,
            param_2 = param_t.param_2 or 0,
            param_3 = param_t.param_3 or 0
        }
    )
end

function TaskCtrl:SendRAShanHaiOriginStart()
    self:SendRAShanHaiOriginReq(
        {
            opera_type = TaskConfig.HelpOthersOperType.start
        }
    )
end

function TaskCtrl:SendRAShanHaiOriginAccept()
    self:SendRAShanHaiOriginReq(
        {
            opera_type = TaskConfig.HelpOthersOperType.accept
        }
    )
end

function TaskCtrl:SendRAShanHaiOriginItem(column, index)
    self:SendRAShanHaiOriginReq(
        {
            opera_type = TaskConfig.HelpOthersOperType.item,
            param_1 = column,
            param_2 = index,
        }
    )
end

function TaskCtrl:SendRAShanHaiOriginTalk()
    self:SendRAShanHaiOriginReq(
        {
            opera_type = TaskConfig.HelpOthersOperType.talk
        }
    )
end

function TaskCtrl:SendRAShanHaiOriginPet(pet_index)
    self:SendRAShanHaiOriginReq(
        {
            opera_type = TaskConfig.HelpOthersOperType.pet,
            param_1 = pet_index,
        }
    )
end

function TaskCtrl:SendRAShanHaiOriginBattle()
    self:SendRAShanHaiOriginReq(
        {
            opera_type = TaskConfig.HelpOthersOperType.battle
        }
    )
end

function TaskCtrl:TaskChainCondition()
    local cur_num = self.data.task_chain_data.cur
    local count = TaskData.Instance.task_chain_data.count
    count = count + 1
    local times_str = Format(Language.TaskChain.ActivityNum, DataHelper.GetDaXie(count))
    if count <= 3 then
        local level = RoleData.Instance:GetRoleLevel()
        local next_lv = Config.task_chain_auto.level_times[count].min_level
        if level < next_lv and count > 0 and TaskData.Instance.task_chain_data.total == 0 then
            return string.format(Language.TaskChain.NextNum,  next_lv, DataHelper.GetDaXie(count))
        end
    elseif count == 4 then
        times_str = Format(Language.TaskChain.ActivityNum, DataHelper.GetDaXie(3))
        cur_num = 50
    end
    ActivityData.Instance:SetActivityFinishInfo(ActType.TaskChain,cur_num,50)
    return times_str .. Format("%s/%s",cur_num,50)
end

function TaskCtrl:SubmitTaskChainItemSel()
    if nil == TaskChainItemSel.cur_sel_data then
        PublicPopupCtrl.Instance:Center(ErrorText[82])
    else
        local data = TaskChainItemSel.cur_sel_data
        if TaskChainItemSel.pet_id == data.pet_id or (PetData.Instance:GetMutantPetCfgByNormalId(TaskChainItemSel.pet_id).pet_id or 0) == data.pet_id then
            if data.level > 1 then
                ViewMgr:OpenView(PetGuiYuan,{data = data, fromAttr = true, close_callback = BindTool.Bind(ViewMgr.OpenView, ViewMgr, TaskChainItemSel)})
            else
                if TaskConfig.TaskChainItemSelFrom.task_chain == TaskChainItemSel.from_type then
                    TaskCtrl.Instance:CSTaskChainOp(TASK_CHAIN_OP_TYPE.PET, data.index)
                elseif TaskConfig.TaskChainItemSelFrom.help_others == TaskChainItemSel.from_type then
                    TaskCtrl.Instance:SendRAHelpOthersPet(data.index)
                elseif TaskConfig.TaskChainItemSelFrom.shan_hai == TaskChainItemSel.from_type then
                    TaskCtrl.Instance:SendRAShanHaiOriginPet(data.index)
                end
            end
        else
            if TaskConfig.TaskChainItemSelFrom.task_chain == TaskChainItemSel.from_type then
                TaskCtrl.Instance:CSTaskChainOp(TASK_CHAIN_OP_TYPE.ITEM, data.column_type, data.index)
            elseif TaskConfig.TaskChainItemSelFrom.help_others == TaskChainItemSel.from_type then
                TaskCtrl.Instance:SendRAHelpOthersItem(data.column_type, data.index)
            elseif TaskConfig.TaskChainItemSelFrom.shan_hai == TaskChainItemSel.from_type then
                TaskCtrl.Instance:SendRAShanHaiOriginItem(data.column_type, data.index)
            end
        end
    end
    ViewMgr:CloseView(TaskChainItemSel)
end

-----声望任务
function TaskCtrl:OnSCPrestigeTaskInfo(protocol)
    PrestigeData.Instance:SetPrestigeTaskInfo(protocol)
    if ViewMgr:IsOpen(TaskPrestigeView) and protocol.village_id ~= 0 and protocol.is_accept_task == 0 then
        ViewMgr:OpenView(TaskPrestigeStarView)
        ViewMgr:CloseView(TaskPrestigeView)
    end
end

function TaskCtrl:SendPrestigeTaskSelectVillage(village_id)
    local protocol = GetProtocol(CSPrestigeTaskSelectVillage)
    protocol.village_id = village_id
    SendProtocol(protocol)
end

function TaskCtrl:SendPrestigeTaskReq(req_type, param)
    local protocol = GetProtocol(CSPrestigeTaskReq)
    protocol.req_type = req_type
    protocol.param = param
    SendProtocol(protocol)
end

function TaskCtrl:OnSCPrestigeTaskFinalNotice(protocol)
    local village_cfg = PrestigeData.Instance:GetPrestigeTaskVillageCfg(protocol.village_id)
    local other_reward = {icon_id = village_cfg.pic}
    other_reward.num = protocol.total_add_prestige
    local desc = Format(Language.TaskPrestige.RewardTip, PrestigeData.Instance:GetPrestigeTaskNum())
    local item_list = PrestigeData.Instance:GetPrestigeTaskTotalRewards()
    PublicPopupCtrl.Instance:OpenRewardView(DataHelper.FormatItemList(item_list), other_reward, true, desc)
end

-- 任务小游戏 game_type :: TaskConfig.TaskGameList.ElementGuide
function TaskCtrl:SendTaskGameReq(game_type, p1, p2)
    local protocol = GetProtocol(CSTaskGameReq)
    if not game_type then
        return
    end
    protocol.game_type = game_type
    protocol.p1 = p1 or 0
    protocol.p2 = p2 or 0
    SendProtocol(protocol)
end

--=======================外部接口===============================
--提交物品任务道具物品框 param2 对话id(<0 前置 >0 后置)
function TaskCtrl:ItemsSubDiong(beh_cfg, beh_func)
    if beh_cfg.param2 < 0 then
        NpcDialogCtrl.Instance:NpcDialogView({talk_id = math.abs(beh_cfg.param2)},beh_func or function()
            TaskCtrl.Instance:ItemsSubView(true)
        end)
    else
        ItemsSubData.Instance.later_talk = beh_cfg.param2
        TaskCtrl.Instance:ItemsSubView(true)
    end
end

--外部添加一个任务CustomTask
function TaskCtrl:CustomTask(t)
    local task = (GetView(TaskConfig.NewType[t.task_type]) or TaskInfo).New()
    task = task:TaskInfo()
    task.task_type = t.task_type
    local vo = CommonStruct.CustomTask(t)
    task:TaskVo(vo)
    TaskData.Instance:TaskInfoUpdateByTaskId(t.task_id, SmartData.New(task))
end