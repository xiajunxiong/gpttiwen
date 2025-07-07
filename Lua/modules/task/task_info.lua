TaskInfo = TaskInfo or BaseClass()

function TaskInfo:__init()
    self.is_new = SmartData.New {val = false}
    self.block_view = false
    self.can_ab = false
end

-- virtual func = InfoConfig/TaskInfo/RemoveTask/CanAbandoned/State
function TaskInfo:InfoConfig(config)
    self.config = config
    return self
end

function TaskInfo:TaskInfo()
    self.is_follow = true
    self.is_remove = false
    self.can_ab = false
    return self
end

function TaskInfo:RemoveTask()
    self.is_remove = true
end

function TaskInfo:CanAbandoned(value)
    self.can_ab = value == 1
end

function TaskInfo:State()
    return TaskConfig.TaskState.undo
end

-- abstract func = TaskVo/GoTask/RequestAccept/RequestCommit
function TaskInfo:GoTask()
end

function TaskInfo:RequestAccept()
end

function TaskInfo:RequestCommit()
end

function TaskInfo:RequestDoing()
end

function TaskInfo:IsRequestDoing()
    return false
end

function TaskInfo:RequestGiveUp()
end

function TaskInfo:IsRequestDoing()
    return false
end

function TaskInfo:TaskStampTime()
    return 0
end

function TaskInfo:TaskTimeEnd()
end

function TaskInfo:GetTimeDesc(t)
    return ""
end

function TaskInfo:TaskVo(vo)
    self.vo = vo
    -- vo = {
    -- 	task_id = int(primary key),
    -- 	list_type = TaskConfig.ListType
    -- 	task_type = TaskConfig.TaskType
    -- 	task_name = string
    --  task_desc = string
    --  task_goal = string
    --  npc_id = int
    --  expire_time = int or 0
    -- 	reward_list = table or {}
    --  task_goal_menu 新加的宝图和真中用到 其他不用 任务链也用到
    -- }
    self.task_id = vo.task_id
    self.list_type = vo.list_type
    -- self.task_type = vo.task_type
    self.task_name = vo.task_name
    self.task_desc = vo.task_desc
    self.task_goal = vo.task_goal
    self.npc_id = vo.npc_id
    self.expire_time = vo.expire_time
    self.reward_list = vo.reward_list
    self.task_goal_menu = vo.task_goal_menu

    self.task_type_sort = TaskConfig.TaskTypeSort[self.task_type]
end

-- 是否在Npc名字版上显示标记 默认显示 添加显示限制条件可以重写
function TaskInfo:IsShowNpcBoardSign()
    return true
end

-- 是否在Npc对话中显示该任务按钮 默认显示 添加显示限制条件可以重写
function TaskInfo:IsShowNpcDialogButton()
    return true
end

function TaskInfo:TaskGoal(is_menu)
    return is_menu and self.task_goal_menu or self.task_goal
end

function TaskInfo:TaskStateLabel()
    return TaskConfig.TaskState.done == self:State() and Language.Task.TaskState.done or Language.Task.TaskState.undone
end

function TaskInfo:IsFollow()
    local follow_id = TaskData.Instance.task_trace_list[self.task_type]
    return follow_id == self.task_id or TaskConfig.TaskType.mainline == self.task_type or TaskConfig.TaskType.branch == self.task_type or TaskConfig.TaskType.branch_serial == self.task_type or TaskConfig.TaskType.branch_marbas == self.task_type or TaskConfig.TaskType.branch_relic == self.task_type or TaskConfig.TaskType.reco == self.task_type or TaskConfig.TaskType.research == self.task_type or TaskConfig.TaskType.peak_road == self.task_type or TaskConfig.TaskType.pet_soar == self.task_type 
end

function TaskInfo:IsTempFollow()
    local follow_id = TaskData.Instance.task_temp_trace_list.val
    return follow_id == self.task_id
end

-- 主界面任务菜单 暂时只有主线任务等级不够 达到xx级可领取
function TaskInfo:TaskMenuCondition()
    return false, ""
end

function TaskInfo:TaskMenuConditionPause()
    return false
end

function TaskInfo:IsTaskMenuCondition()
    return false
end

function TaskInfo:SubItems()
    return {}
end

function TaskInfo:SubCurrency()
    return {}
end

function TaskInfo:IsNew(value)
    if nil ~= value then
        self.is_new.val = value
    end
    return self.is_new.val
end

function TaskInfo:DetailInfo()
end

function TaskInfo:TaskSeq()
    return self.info and self.info.seq or 0
end

function TaskInfo:IsSkipTalk()
    return self.is_skip_talk
end

function TaskInfo:IsShowTaskButton()
    return false
end

function TaskInfo:MenuTitle()
    local bounty_task_progress = TaskData.Instance.bounty_task_progress
    local task_name = self.task_name
    if TaskConfig.TaskType.bounty == self.task_type and 1 == bounty_task_progress.has_finish_first_task then
        task_name = task_name .. Format("(%s/%s)", bounty_task_progress.cur_task_idx + 1, bounty_task_progress.today_task_num)
    elseif TaskConfig.TaskType.prestige == self.task_type then
        local task_star = PrestigeData.Instance:GetPrestigeTaskStarByTaskId(self.task_id)
        local cur_val, max_val = PrestigeData.Instance:GetPrestigeTaskProg()
        task_name = Format(Language.Task.PrestigeTask.Star, task_star) .. task_name
        task_name = task_name .. string.format("(%s/%s)", cur_val + 1, max_val)
    elseif TaskConfig.TaskType.fly_up == self.task_type then
        local cur_num, max_num = FlyUpData.Instance:GetCurrTaskNum()
        task_name = task_name .. Format("(%s/%s)", cur_num, max_num)
    end
    local color = self.skin_type or TaskConfig.TaskMenuTypeColor[self.task_type]
    local task_title = Format(Language.Task.Menu.TaskName,color, Language.Task.TaskType[self.task_type], task_name)
    return task_title
end

function TaskInfo:AutoAudioTask()
    return false
end

function TaskInfo:PatnerFSId()
    return TaskConfig.TaskType.partner_soar == self.task_type and self.info.partner_id or 0
end

function TaskInfo:ShowExpObj()
    return TaskConfig.TaskType.branch == self.task_type or TaskConfig.TaskType.reco == self.task_type or TaskConfig.TaskType.branch_serial == self.task_type or TaskConfig.TaskType.branch_marbas == self.task_type or TaskConfig.TaskType.branch_relic == self.task_type
end

TaskInfoNormal = TaskInfoNormal or BaseClass(TaskInfo)

function TaskInfoNormal:__init()
end

function TaskInfoNormal:TaskInfo()
    TaskInfo.TaskInfo(self)
    self.is_add_npc = false
    self.is_add_other = false
    self.can_ab = true
    return self
end

function TaskInfoNormal:TaskVo(vo, task_event)
    self.vo = vo
    self.task_id = vo.task_id
    self.list_type = vo.list_type
    self.task_event = task_event
    self.task_effect = false
    self.info =
        Config.tasklist_auto.task_list[self.task_id] or 
        Config.challengetask_list_auto.task_list[self.task_id] or 
        Config.medaltask_list_auto.task_list[self.task_id] or
        Config.strange_tasklist_auto.task_list[self.task_id] or
        Config.advanced_tasks_auto.task_list[self.task_id] or
        Config.bounty_tasklist_auto.task_list[self.task_id] or
        Config.transaction_task_auto.task_list[self.task_id] or
        Config.feisheng_tasklist_auto.task_list[self.task_id] or
        Config.pet_fs_tasklist_auto.task_list[self.task_id] or
        Config.partner_fs_tasklist_auto.task_list[self.task_id] or
        Config.huanshi_challenge_auto.task_list[self.task_id] or 
        Config.family_daily_mission_auto.mission_list[self.task_id] or
        PrestigeData.Instance:PrestigeTaskCfg(self.task_id)
    if nil == self.info then
        BuglyReport("TaskErrorNotExitId "..self.task_id)
        return
    end
    self.task_type = self.info.task_type
    self.task_type_sort = TaskConfig.TaskTypeSort[self.task_type]
    self.task_name = self.info.task_name
    self.task_desc = self.info.progress_desc
    self.expire_time = vo.expire_time
    local progress_t = string.split(self.info.c_param3, "|")
    self.progress = tonumber(progress_t[1])
    self.min_level = self.info.min_level

    local obj_desc = self.info.obj_desc
    local obj_desc_1 = self.info.obj_desc_1
    -- if self.info.condition == 39 then
    --     obj_desc,obj_desc_1 = RolePromoteData.Instance:GetTaskDesc(self.info)
    -- end
    if self:GoalWithProgress(progress_t) then
        self.task_goal = string.format("%s(%s/%s)", obj_desc, vo.progress, self.progress)
        self.task_goal_menu = string.format("%s(%s/%s)", obj_desc_1 or obj_desc, vo.progress, self.progress)
    else
        self.task_goal = obj_desc
        self.task_goal_menu = obj_desc_1 or obj_desc
    end

    self.is_finished = (TaskConfig.ListType.accepted == self.list_type) and ((0 == self.progress) or (vo.progress >= self.progress))
    self.npc_seq = TaskConfig.ListType.accepted == self.list_type and self.info.commit_npc or self.info.accept_npc
    self.npc_vo = Config.npc_dynamic_auto.npc_dynamic_cfg[self.npc_seq] or {}
    self.npc_id = self.npc_vo.id or 0

    self.is_skip_talk = false
    if self.is_finished and self.info.task_type == TaskConfig.TaskType.mainline and self.info.manual_status == 1 and self.progress > 0 and not IS_STANDALONE() then
        self:MainLineTaskNeedClick(true)
    end
    self:CheckRewardList(Config.tasklist_auto.show_award[self.task_id])
    self:DynamicNpc()
    self:OtherNpc(true)
    self:DefaultEvent()
    self:TaskBehavior()

    self:AutoTask()
    self:OnPromoteMenuCondition()
end

function TaskInfoNormal:IsAutoTask()
    return 1 == self.info.auto_task
end

function TaskInfoNormal:AutoTask()
    if IS_AUDIT_VERSION then
        return
    end
    if self.task_event.new_task and self.task_event.init_list then
        self.task_event.new_task = nil
        if 1 == self.info.auto_task and SceneData.Instance:GetMode() == SceneModeType.None then
            AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(self)}, 1)
            if TaskConfig.TaskType.prestige == self.task_type then
                BehaviorData.Instance.auto_behavior = PrestigeData.Instance:GetPrestigeTaskAutoAwit()
            end
        elseif TaskConfig.TaskType.bounty == self.task_type and not self:IsFirstBountyTask() and self:BountyAutoGo() then
            AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(self)}, 1)
            BehaviorData.Instance.auto_behavior = Config.bounty_tasklist_auto.other[1].auto_wait
        elseif TaskConfig.TaskType.prestige == self.task_type then
            AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(self)}, 1)
            BehaviorData.Instance.auto_behavior = PrestigeData.Instance:GetPrestigeTaskAutoAwit()
        elseif TaskConfig.TaskType.fly_up == self.task_type then
            AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(self)}, 1)
            BehaviorData.Instance.auto_behavior = 3
        elseif TaskConfig.TaskType.partner_soar == self.task_type then
            if ViewMgr:IsOpen(PartnerSoarView) and SceneData.Instance:GetMode() == SceneModeType.None then
                self:GoTask()
                ViewMgr:CloseView(PartnerSoarView)
                ViewMgr:CloseView(PartnerView)
            end       
        elseif not self.task_event.init_list then
            AutoInvokerCtrl.Instance:ClearCommands()
        end
    end
    if TaskConfig.TaskType.silk_road == self.task_type then
        local hide = TaskData.Instance:TaskState(self.task_type)
        self.dont_show = hide
    end
end

function TaskInfoNormal:RemoveTask(task_event)
    self:OtherNpc(false)
    if self.info and 26 ~= self.info.condition then
        SceneLogic.Instance:ClearMoveCache(true)
    end
    self.is_remove = true
    self.task_event = task_event
    self:DefaultEvent()
    if TaskConfig.TaskType.adventure == self.task_type then
        TaskData.Instance.menu_adventure:Set({})
    end
end

function TaskInfoNormal:CheckRewardList(info_sp)
    local info = Config.tasklist_auto.task_award[self.task_id] or Config.challengetask_list_auto.task_award[self.task_id] or
     Config.medaltask_list_auto.task_award[self.task_id] or  Config.strange_tasklist_auto.task_award[self.task_id] or 
     Config.bounty_tasklist_auto.task_award[self.task_id] or Config.pet_fs_tasklist_auto.task_award[self.task_id] or 
     Config.partner_fs_tasklist_auto.task_award[self.task_id] or  Config.huanshi_challenge_auto.team_challenge_reward[self.task_id] or
     Config.family_daily_mission_auto.mission_reward[self.task_id]
    local t, r = {}
    if nil == info then
        self.reward_list, self.reward_show =  t, r
        return t
    end

    local function CheckReward(reward, item_id)
        if reward ~= 0 then
            local item_data = Item.New()
            item_data.item_id = item_id
            item_data.num = reward
            item_data.is_bind = 0
            table.insert(t, item_data)
        end
    end

    CheckReward(info.coin_bind, CommonItemId.Coin)
    CheckReward(info.exp, CommonItemId.Exp)
    CheckReward(info.immortal, CommonItemId.Immortal)
    CheckReward(info.gold, CommonItemId.Gold)

    if info_sp then
        local prof_type = RoleData.Instance:ProfType()
        local rewards = string.split(info_sp.show_award, "|")
        local item_id = tonumber(rewards[prof_type] or rewards[1])
        if 0 == item_id then return end
        local vo = {}
        vo.item_id = item_id
        vo.num = 1
        vo.is_bind = 1
        vo.show_key = "workshop_equip"
        local item = Item.Create(vo)
        table.insert(t, item)
        r = item
    else
        local itli = info.item_list2 or info.item_list
        if itli ~= nil then
            for k, v in pairs(itli) do
                local vo = {}
                vo.item_id = v.item_id
                vo.num = v.num
                vo.is_bind = v.is_bind
                vo.show_key = "workshop_equip"
                local item = Item.Create(vo)
                table.insert(t, item)
                r = item
            end
        end
    end
    self.reward_list, self.reward_show =  t, r
end

function TaskInfoNormal:DynamicNpc()
    if TaskConfig.TaskState.doing == self:State() and not self.is_add_npc then
        self.is_add_npc = true
        self.show_npcs = string.split(self.info.is_show_npc, "|")
        for i = 1, #self.show_npcs do
            SceneCtrl.Instance:AddNpc(tonumber(self.show_npcs[i]))
        end
    end
    if TaskConfig.TaskState.done == self:State() and self.is_add_npc then
        self.is_add_npc = false
        for i = 1, #self.show_npcs do
            SceneCtrl.Instance:DeleteNpc(tonumber(self.show_npcs[i]))
        end
    end
end

function TaskInfoNormal:OtherNpc(is_add)
    if is_add and not self.is_add_other then
        self.is_add_other = true
        self.other_npcs = string.split(self.info.show_other_npcseq, "|")
        for i = 1, #self.other_npcs do
            SceneCtrl.Instance:AddNpc(tonumber(self.other_npcs[i]))
        end
    end
    if not is_add and self.is_add_other then
        self.is_add_other = false
        for i = 1, #self.other_npcs do
            SceneCtrl.Instance:DeleteNpc(tonumber(self.other_npcs[i]))
        end
    end
end

function TaskInfoNormal:DefaultEvent()
    if self.task_event.new_task then
        SceneCtrl.Instance:AddNpc(self.npc_seq)
    end
    if self.task_event.is_remove then
        SceneCtrl.Instance:DeleteNpc(self.npc_seq)
    end
    if self.task_event.is_accpet then
        SceneCtrl.Instance:AddNpc(self.info.commit_npc)
        SceneCtrl.Instance:DeleteNpc(self.info.accept_npc)
    end
    if self.task_event.is_complete then
        AudioMgr:PlayEffect(AudioType.UI, "task_complete")
    end
end

function TaskInfoNormal:TaskBehavior()
    self.beh_cfg = {}
    local fly_scenes = string.split(self.info.fly_to, "|")
    self:BehaviorAC(TaskConfig.TaskState.undo, self.info.accept_dialog_beh, self.info.accept_npc, tonumber(fly_scenes[1]))
    self:BehaviorAC(TaskConfig.TaskState.done, self.info.commit_dialog_beh, self.info.commit_npc, tonumber(fly_scenes[3]))
    if type(self.info.tasks_dialog_beh) == "string" and not StringIsEmpty(self.info.tasks_dialog_beh) then
        local params = string.split(self.info.tasks_dialog_beh, "|")
        local npc_seq = tonumber(params[5]) or 0
        local end_beh = {
            behavior_type = tonumber(params[1]),
            param1 = tonumber(params[2]) or params[2],
            param2 = tonumber(params[3]) or params[3],
            param3 = tonumber(params[4]) or params[4],
            npc_seq = tonumber(params[5]) or 0,
            fly_scenes = fly_scenes,
            task_info = self
        }
        self.npc_id = (npc_seq > 0 and (201 == end_beh.behavior_type or TaskConfig.TaskType.fly_up)) and NpcHelper.NpcSeqToId(npc_seq) or self.npc_id
        self.beh_cfg[TaskConfig.TaskState.doing] = npc_seq > 0 and
        function()
            self:AutoToNpc(tonumber(fly_scenes[2]), npc_seq, end_beh)end or 
        function()
            BehaviorCtrl.ExeByCfgVo(end_beh)
        end
    end
end

function TaskInfoNormal:BehaviorAC(state, dialog_beh, npc_seq, fly_scene)
    if StringIsEmpty(dialog_beh) then
        Debuger.LogError(self.task_id .. " accept_dialog_beh/commit_dialog_beh 其中有个是空")
        dialog_beh = 0
    end
    if dialog_beh > 0 then
        local npc_vo = Config.npc_dynamic_auto.npc_dynamic_cfg[npc_seq] or {}
        local npc_id = npc_vo.id or 0
        self.beh_cfg[state] = function(skip_move)
            local end_beh = {
                behavior_type = 1,
                param1 = dialog_beh,
                param2 = "",
                param3 = "",
                task_info = self
            }
            if skip_move then
                BehaviorCtrl.ExeByCfgVo(end_beh)
            else
                self:AutoToNpc(fly_scene, npc_seq, end_beh)
            end
        end
    elseif TaskConfig.TaskState.undo == state then
        self.beh_cfg[state] = function()
            self:AutoToNpc(fly_scene, npc_seq)
        end
    end
end

function TaskInfoNormal:AutoToNpc(scene_id, npc_seq, end_beh)
    if 0 == npc_seq and self.list_type == TaskConfig.ListType.acceptable then
        self:RequestAccept()
    elseif  UN_OVERWRITE_TASK [self.task_type] then --不重写npc行为
         SceneLogic.Instance:AutoToNpc(npc_seq, nil,nil,0 ~= scene_id)
    else
        if TaskConfig.TaskType.mainline == self.task_type then
            TaskCtrl.Instance:SendRoleTaskPathFindingStart(self.task_id)
        end
        SceneLogic.Instance:AutoToNpc(npc_seq,
        function(npc_obj)
            if nil == npc_obj then
                LogError(string.format("任务npc_obj为空 task_id:%s task_state:%s", self.task_id, self:State()))
                return
            end
            if TaskConfig.TaskType.mainline == self.task_type then
                TaskCtrl.Instance:SendRoleTaskPathFindingStop()
            end
            if TaskConfig.TaskType.bounty == self.task_type and self:IsFirstBountyTask() then
                BehaviorCtrl.ExeByVo(npc_obj.vo)
            else
                BehaviorCtrl.ExeByCfgVo(end_beh, npc_obj.vo)
            end    
        end,nil,0 ~= scene_id)
    end
end

function TaskInfoNormal:IsFollow()
    return (not self.dont_show) and TaskInfo.IsFollow(self)
end

function TaskInfoNormal:TaskGoal(is_menu)
    if self.mainline_need_click then
        return Language.Task.ClickToGetMainLineReward
    else
        return TaskInfo.TaskGoal(self, is_menu)
    end
end

function TaskInfoNormal:MainLineTaskNeedClick(need)
    if need ~= nil then
        self.mainline_need_click = need
    end
end

function TaskInfoNormal:GoTask(skip_move)
    --LogError("GoTask task_id",self.task_id)
    if self.task_type == TaskConfig.TaskType.mainline then
        if self.mainline_need_click then
            return
        end
    end
    if self.is_finished and self.info.task_type == TaskConfig.TaskType.mainline and self.info.manual_status == 1 and self.progress > 0 and IS_STANDALONE() then
        self:RequestCommit()
        return
    end
    local stamp_time = self:TaskStampTime()
    if stamp_time and stamp_time ~= 0 then
        PublicPopupCtrl.Instance:Center(Language.Task.WaitTips)
        return
    end
    if self.info and self.info.group_limited_1 and self.info.group_limited_1 == 1 and TeamData.Instance:InTeam() then
        -- PublicPopupCtrl.Instance:Center(ErrorText[169])
        TeamData.Instance:CheckSingle(function ()
            if TaskConfig.TaskType.bounty == self.task_type then 
                BehaviorData.Instance.auto_behavior = self:IsFirstBountyTask() and 0 or Config.bounty_tasklist_auto.other[1].auto_wait 
                if 14 == self.info.condition then
                    MallData.IsHasBountySubmission = true
                    ReputationCtrl.Instance:SetSelectTaskCache(self.task_type)
                end
            elseif TaskConfig.TaskType.prestige == self.task_type then
                BehaviorData.Instance.auto_behavior = PrestigeData.Instance:GetPrestigeTaskAutoAwit()
                if 14 == self.info.condition then
                    ReputationCtrl.Instance:SetSelectTaskCache(self.task_type)
                end
            end
            if self:IsSubTask() then
                local sub_items = self:SubItems()
                ItemsSubData.Instance:SetSubInfo(sub_items,function(sel_items)
                    return self:SubItems(sel_items)
                end)
            end
            if self:IsSubCurrencyTask() then
                local sub_currency = self:SubCurrency()
                ItemsSubData.Instance:SetSubCurrencyInfo(sub_currency,function()
                    return self:SubCurrency(sub_currency)
                end)
            end
            self:OnTaskDialogBeh(skip_move)
        end, ErrorText[169])
        return
    end
    if TeamData.Instance:InTeam() and not TeamData.Instance:IsLeader() and TeamData.Instance:IsFollow() then
        return 
    end
    if TaskConfig.TaskType.bounty == self.task_type then
        BehaviorData.Instance.auto_behavior = self:IsFirstBountyTask() and 0 or Config.bounty_tasklist_auto.other[1].auto_wait 
        if 14 == self.info.condition then
            MallData.IsHasBountySubmission = true
            ReputationCtrl.Instance:SetSelectTaskCache(self.task_type)
        end
    elseif TaskConfig.TaskType.prestige == self.task_type then
        BehaviorData.Instance.auto_behavior = PrestigeData.Instance:GetPrestigeTaskAutoAwit()
        if 14 == self.info.condition then
            ReputationCtrl.Instance:SetSelectTaskCache(self.task_type)
        end
    end
    if self:IsSubTask() then
        local sub_items = self:SubItems()
        ItemsSubData.Instance:SetSubInfo(sub_items,function(sel_items)
            return self:SubItems(sel_items)
        end)
    end
    if self:IsSubCurrencyTask() then
        local sub_currency = self:SubCurrency()
        ItemsSubData.Instance:SetSubCurrencyInfo(sub_currency,function()
            return self:SubCurrency(sub_currency)
        end)
    end
    self:OnTaskDialogBeh(skip_move)
end

function TaskInfoNormal:IsLevelEnough()
    local role_level = RoleData.Instance:GetRoleLevel()
    return role_level >= self.info.min_level
end

function TaskInfoNormal:State()
    if TaskConfig.ListType.acceptable == self.list_type then
        return TaskConfig.TaskState.undo
    elseif self.is_finished then
        return TaskConfig.TaskState.done
    else
        return TaskConfig.TaskState.doing
    end
end

function TaskInfoNormal:AutoCfg()
    return self.info
end

function TaskInfoNormal:RequestAccept()
    self.is_skip_talk = true
    TaskCtrl.Instance:SendTaskAccept(self.task_id)
end

function TaskInfoNormal:RequestCommit()
    self.is_skip_talk = true
    TaskCtrl.Instance:SendTaskCommit(self.task_id)
end

function TaskInfoNormal:RequestDoing(is_not_auto)
    self.is_skip_talk = true
    if not is_not_auto and TaskConfig.TaskType.bounty == self.task_type then
        if 14 == self.info.condition then
            TaskData.shop_auto_close = true
            MallCtrl.Instance:Jump(self.info.c_param1)
        end
        if 15 == self.info.condition then
            SceneCtrl.Instance:RequestGroupIdBattle(self.info.is_show_npc, self.info.c_param1)
        end
        if 37 == self.info.condition then
            --门派任务专用
            if type(self.info.tasks_dialog_beh) == "string" and not StringIsEmpty(self.info.tasks_dialog_beh) then
                local params = string.split(self.info.tasks_dialog_beh, "|")
                if tonumber(params[1]) == 3 then
                    --这个没有用了
                else
                    local beh_ids = string.split(params[3], ":")
                    if beh_ids and beh_ids[1] then
                        local co = Cfg.NpcButtonListByButtonId(tonumber(beh_ids[1]))
                        if co then BehaviorCtrl.Exe(co.button_behavior) end
                    end
                end
            end
        end
    elseif not is_not_auto and TaskConfig.TaskType.prestige == self.task_type then
        if 14 == self.info.condition then
            TaskData.shop_auto_close = true
            MallCtrl.Instance:Jump(self.info.c_param1)
        end
    elseif not is_not_auto and TaskConfig.TaskType.fly_up == self.task_type then
        if 15 == self.info.condition then
            SceneCtrl.Instance:RequestGroupIdBattle(self.info.is_show_npc, self.info.c_param1)
        elseif self:IsSubCurrencyTask() then
            self:SubCurrency(self:SubCurrency())
        end
    elseif not is_not_auto and TaskConfig.TaskType.family_daily == self.task_type then
        if 14 == self.info.condition then
            TaskData.shop_auto_close = true
            MallCtrl.Instance:Jump(self.info.c_param1)
        end
    end
end

function TaskInfoNormal:IsRequestDoing()
    return TaskConfig.TaskType.bounty == self.task_type and 15 == self.info.condition and TaskConfig.TaskState.doing == self:State()
end

function TaskInfoNormal:RequestGiveUp()
    TaskCtrl.Instance:SendTaskGiveUp(self.task_id)
end

function TaskInfoNormal:TaskStateLabel()
    return self:IsLevelEnough() and (TaskConfig.TaskState.done == self:State() and Language.Task.TaskState.done or Language.Task.TaskState.undone) or ""
end

function TaskInfoNormal:TaskMenuCondition()
    if self:OnPromoteMenuCondition() then
        return true,Language.Role.PromoteTaskDescTips
    end
    if not self:IsLevelEnough() and self.task_id == TaskData.Instance:GetOtherCfg().special_dec_task_id then
        return true, string.format(Language.Task.Tips.ExpTip, self.info.min_level)
    end
    return not self:IsLevelEnough(), string.format(Language.Task.Tips.ContinueTips, self.info.min_level)
end

--晋阶的任务特殊处理
function TaskInfoNormal:OnPromoteMenuCondition()
    if TaskConfig.TaskType.upgrade_item == self.task_type then
        if self.info.condition == 39 then
            local config = RolePromoteData.GetProfConfig(RoleData.Instance:GetRoleProfType() + self.info.c_param1)
            if config then
                self.task_goal = Format(self.info.obj_desc,config.job_name,self.vo.progress, self.progress)
                self.task_goal_menu = Format(self.info.obj_desc_1,config.job_name,self.vo.progress, self.progress)
            end
        end
        return true
    end
end

function TaskInfoNormal:TaskMenuConditionPause()
    return 31 == self.info.condition
end

function TaskInfoNormal:IsTaskMenuCondition(condition)
    return condition == self.info.condition
end

function TaskInfoNormal:IsShowNpcBoardSign()
    return self:IsLevelEnough()
end

function TaskInfoNormal:IsShowNpcDialogButton()
    return self:IsLevelEnough()
end

function TaskInfoNormal:SubItems(sub_items)
    if nil == sub_items then
        local sub_items = {}
        if 6 == self.info.condition then
            local co = Config.tasklist_auto.item_group[self.info.c_param1] or Config.medaltask_list_auto.item_group[self.info.c_param1]
            if co then
                local items = string.split(co.item_id, "|")
                for i = 1, #items do
                    local item_data = Item.New()
                    item_data.item_id = tonumber(items[i])
                    item_data.sub_num = self.info.c_param2
                    table.insert(sub_items, item_data)
                end
            end
        else
            if self.info.c_param1 then
                local item_data = Item.New()
                item_data.item_id = self.info.c_param1
                item_data.sub_num = self.info.c_param3
                table.insert(sub_items, item_data)
            end
        end
        return sub_items
    else
        local is_enough = true
        for _, item in pairs(sub_items) do
            if item.sub_num > BagData.Instance:GetNumByItemId(item.item_id) then
                is_enough = false
            end
        end
        if not is_enough then
            
            BehaviorData.Instance.auto_behavior = 0
            PublicPopupCtrl.Instance:Center(Language.Task.SubItemsTips)
            return false
        end

        if TaskConfig.TaskType.prestige == self.task_type  then
            TimeHelper:AddDelayTimer(function()
                TaskCtrl.Instance:SendGiveNPCTaskNeedItems {
                    task_id = self.task_id,
                    commit_list = sub_items
                }
            end, 0.2)
        else
            TaskCtrl.Instance:SendGiveNPCTaskNeedItems {
                task_id = self.task_id,
                commit_list = sub_items
            }
        end
        return true
    end
end

function TaskInfoNormal:IsSubTask()
    return 11 == self.info.condition or 21 == self.info.condition or 6 == self.info.condition
end

function TaskInfoNormal:SubCurrency(sub_currency)
    if nil == sub_currency then
        return {
            currency_type = self.info.c_param1,
            currency_num = self.info.c_param2,
        }
    else
        local is_enough = RoleData.Instance:GetCurrency(sub_currency.currency_type) >= sub_currency.currency_num
        if not is_enough then
            
            BehaviorData.Instance.auto_behavior = 0
            PublicPopupCtrl.Instance:Center(string.format(Language.Task.SubCurrencyTips, Language.Common.HuoBi[sub_currency.currency_type] or ""))
            return false
        end
        TaskCtrl.Instance:SendTaskCommitMoneyReq(self.task_id)
        return true
    end
 end
 
 function TaskInfoNormal:IsSubCurrencyTask()
    return 40 == self.info.condition
 end

--执行任务中的行为 end_beh可以是嵌套任务的beh_cfg
function TaskInfoNormal:TaskDialogBeh(beh_cfg,end_beh)
    if beh_cfg.npc_seq > 0 then
        self:AutoToNpc(tonumber(beh_cfg.fly_scenes[2]), beh_cfg.npc_seq, end_beh or beh_cfg)
    else
        BehaviorCtrl.ExeByCfgVo(end_beh or beh_cfg)
    end 
end

--执行任务当前状态的行为
function TaskInfoNormal:OnTaskDialogBeh(skip_move)
    if self:IsLevelEnough() then
        if self.beh_cfg[self:State()] then
            self.beh_cfg[self:State()](skip_move)
        else
            LogError(string.format("没有改状态的执行方法状态为1应该清理掉自动任务的缓存:TaskInfoState=%s task_id:%s",tostring(self:State()),tostring(self.task_id)))
        end
    else
        PublicPopupCtrl.Instance:Center(Language.Task.Tips.LevelTips)
        if RoleData.Instance:GetRoleLevel() >= TaskConfig.NORMAL_TASK_NOT_ENOUGH_LEVEL then
            ViewMgr:OpenViewByKey(Mod.Activity.Daily)
        end
    end
end

function TaskInfoNormal:TaskEffect()
    if self.task_effect then
        return true
    end
    self.task_effect = true
    return false
end

function TaskInfoNormal:GoalWithProgress(progress_t)
    local value = TaskConfig.ListType.accepted == self.list_type and 0 < self.progress and nil == progress_t[2] and 31 ~= self.info.condition and 45 ~= self.info.condition
    if TaskConfig.TaskType.bounty == self.task_type then
        if 14 == self.info.condition then
            SilkRoadData.Instance:CheckBountySubmission({item_id = self.info.c_param1,num = self.progress})
        end
        value = value and 11 ~= self.info.condition 
    end
    return value
end


function TaskInfoNormal:DetailInfo()
    if TaskConfig.TaskType.bounty == self.task_type and self:IsFirstBountyTask() then
        local info = RewardData.Instance:GetRewardActivityBackInfoByFindType(FindType.BountyTask, TaskConfig.BountyTaskMax)
        local price = RewardData.Instance:GetRewardActivityBackFindPrice(FindType.BountyTask)
        local cost = TaskConfig.BountyTaskMax * price
        return NpcDialogData.CreateDetailInfo1{tips = Language.Task.DetailTips[self.task_type], rewards = TaskData.GetBountyTaskDetailInfoRewardShow(), custom_type = DetailInfo1Custom.back, custom_show = Language.NpcDialog.BackOneKey, act_type = ActType.BountyTask, custom_func = BindTool.Bind(ViewManager.OpenView, ViewMgr, DialogTipsBackView,
            {cost = cost, name = Language.Task.BountyTaskName, rewards = info.reward_list[0], confirm_func = function ()
                TaskCtrl.Instance:SendOnekeyFinish(OnekeyFinishType.BountyTask)
                if RoleData.Instance:CreatedDay() <= 1 then
                    TaskData.Instance:StopTaskState(TaskConfig.TaskType.silk_road)
                end
            end})
        }
    end
end

function TaskInfoNormal:IsFirstBountyTask()
    local co = Config.bounty_tasklist_auto.task_level[self.task_id]
    if co and 1 == co.is_first_task then
        local hide = TaskData.Instance:TaskState(self.task_type)
        self.dont_show = hide
    end
    return co and 1 == co.is_first_task
end

--是否只显示任务按钮
function TaskInfoNormal:IsShowTaskButton()
    if self.info.is_show_task_button then
        return self.info.is_show_task_button == 1
    end
    return false
end

function TaskInfoNormal:AutoAudioTask()
    return TaskConfig.TaskType.mainline == self.task_type or TaskConfig.TaskType.branch == self.task_type or TaskConfig.TaskType.branch_serial == self.task_type or TaskConfig.TaskType.branch_marbas == self.task_type or TaskConfig.TaskType.branch_relic == self.task_type or TaskConfig.TaskType.fly_up == self.task_type or TaskConfig.TaskType.fly_up_pre == self.task_type
end

--自动对话的时候 点跳过对话时对话界面会关闭 导致玩家无法手动点击按钮
function TaskInfoNormal:IsNotCloseDialog()
    return TaskConfig.TaskType.fly_up == self.task_type
end

function TaskInfoNormal:BountyAutoGo()
    local bounty_task_progress = TaskData.Instance.bounty_task_progress
    return (nil == TaskData.Instance:GetTaskInfoByTaskId(510) and nil == TaskData.Instance:GetTaskInfoByTaskId(502)) or (2 ~= (bounty_task_progress.cur_task_idx + 1))
end

function TaskInfoNormal:TaskStampTime()
    local wait_time = TaskData.Instance.task_wait_list[self.task_id] or 0
    return wait_time > TimeHelper.GetServerTime() and wait_time or 0
end

function TaskInfoNormal:GetTimeDesc(t, task_goal, is_end)
    if is_end then
        return task_goal
    else
        if t.day > 0 then
            return Format(Language.Task.TimeTip1, t.day or 0,t.hour or 0)
        else
            return Format(Language.Task.TimeTip2, t.hour or 0,t.min or 0)
        end
    end
end

-------------------------宝图任务-----------------------
TaskInfoMap = TaskInfoMap or BaseClass(TaskInfo)

function TaskInfoMap:__init()
end

function TaskInfoMap:TaskVo(vo, auto)
    self.task_type = TaskConfig.TaskType.treasure_map
    TaskInfo.TaskVo(self, vo)
    if auto then
        self:AutoTask()
    end
end

function TaskInfoMap:GoTask()
    TreasureMapCtrl.Instance:AutoTask()
end

function TaskInfoMap:RequestGiveUp()
    TreasureMapCtrl.Instance:SendTreasureMapReq({req_type = TREASURE_MAP_REQ_TYPE.GIVE_UP})
    TreasureMapData.Instance.cache_task_id = 0
end
function TaskInfoMap:AutoTask()
    AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(self)}, 1)
end
-------------------------真中任务-----------------------
-- TaskInfoResearch = TaskInfoResearch or BaseClass(TaskInfo)

-- function TaskInfoResearch:__init()
--     -- self.is_new.val = true
--     self.can_ab = false
-- end
-- function TaskInfoResearch:TaskInfo()
--     TaskInfo.TaskInfo(self)
--     self.can_ab = false
--     return self
-- end
-- function TaskInfoResearch:TaskVo(vo, auto)
--     self.task_type = TaskConfig.TaskType.research
--     TaskInfo.TaskVo(self, vo)
--     --[[ if auto then
--     end ]]
--     self:AutoTask()
-- end

-- function TaskInfoResearch:GoTask()
--     if TeamData.Instance:IsLeader() then
--         ResearchTaskCtrl.Instance:AutoTask()
--     end
-- end

-- function TaskInfoResearch:AutoTask()
--     if TeamData.Instance:IsLeader() then
--         AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(self)}, 1)
--     end
-- end

-- function TaskInfoResearch:RequestGiveUp()
--     -- ResearchTaskCtrl.Instance:SendTreasureMapReq({req_type = TREASURE_MAP_REQ_TYPE.GIVE_UP})
-- end

-------------------------门派任务-----------------------
TaskInfoGuild = TaskInfoGuild or BaseClass(TaskInfo)

function TaskInfoGuild:__init(is_main)
    self.is_new.val = true
    self.is_main = is_main
end

function TaskInfoGuild:TaskVo(vo)
    self.task_type = TaskConfig.TaskType.guild
    TaskInfo.TaskVo(self, vo)
end
function TaskInfoGuild:IsShowNpcDialogButton()
    return false
end
function TaskInfoGuild:GoTask()
    if self.is_main then
        ViewMgr:CloseView(NpcDialogView)
        ViewMgr:OpenView(DailyGuildTask)
    elseif self.is_finished then
        if ViewMgr:IsOpen(NpcDialogView) then
            -- 界面打开则提交任务
            NpcDialogCtrl.Instance:NpcDialogView(
                {
                    talk_list = {
                        {
                            talker = self.npc_id,
                            talk_text = self.task_finish_desc
                        }
                    },
                    beh_btn_list = {}
                }
            )
            GuildCtrl.Instance:CSGuildTaskReq(0, GuildData.Instance:GetGuildTaskIndexById(self.real_id))
        else
            -- local cfg = {
            --     behavior_id = 9999,
            --     behavior_type = 6,
            --     param1 = GuildCtrl.Instance.data:GetGuildTaskNpcId(),
            --     param2 = "",
            --     param3 = ""
            -- }
            -- BehaviorCtrl.ExeByCfgVo(cfg)
            SceneLogic.Instance:AutoToNpc(GuildCtrl.Instance.data:GetGuildTaskNpcId(), nil, false, true)
        end
    else
        if self.target_scene then
            local cfg = {
                behavior_id = 9999,
                behavior_type = 9,
                param1 = self.target_scene,
                param2 = 822,
                param3 = ""
            }
            if MainViewData.Instance:IsBattle() then
                if nil ~= self.handle_js then
                    MainViewData.Instance:BaseData():Uncare(self.handle_js)
                    self.handle_js = nil
                end
                self.handle_js = MainViewData.Instance:BaseData():Care(function ()
                    MainViewData.Instance:BaseData():Uncare(self.handle_js)
                    self.handle_js = nil
            BehaviorCtrl.ExeByCfgVo(cfg)
                end, "is_battle")
            else
                BehaviorCtrl.ExeByCfgVo(cfg)
        end
    end
end
end

function TaskInfoGuild:SetCustomStr(str)
    self.custom_str = str
end

function TaskInfoGuild:IsFollow()
    return (not self.dont_show) and TaskInfo.IsFollow(self)
end

function TaskInfoGuild.RewardList(id)
    local cfg = GuildData.Instance.guild_task_cfg.task[id]
    local reward_list = {}
    local mission_reward_cfg = GuildData.Instance.guild_task_cfg.mission_reward[RoleData.Instance:GetRoleLevel()]
    if mission_reward_cfg then
        table.insert(reward_list, Item.New({item_id = CommonItemId.Exp, num = mission_reward_cfg.exp}))
        table.insert(reward_list, Item.New({item_id = CommonItemId.Coin, num = mission_reward_cfg.coin}))
        table.insert(reward_list, Item.New({item_id = CommonItemId.Prestige, num = mission_reward_cfg.reputation}))
    end
    local group_list = string.split(cfg.reward_show, "|")
    for index, value in ipairs(group_list) do
        local item_struct = string.split(value, ":")
        local item_id = tonumber(item_struct[1])
        if item_id ~= CommonItemId.Exp and item_id ~= CommonItemId.Coin and item_id ~= CommonItemId.Prestige then
            -- table.insert(reward_list, Item.New({item_id = item_id, num = item_struct[2]}))
            table.insert(reward_list, Item.New({item_id = item_id, num = 0}))
        end
    end
    return reward_list
end

-------------------------任务链-----------------------
TaskInfoChain = TaskInfoChain or BaseClass(TaskInfo)

function TaskInfoChain:__init()
    -- self.is_new.val = true
end

function TaskInfoChain:TaskInfo()
    TaskInfo.TaskInfo(self)
    self.can_ab = false
    return self
end
function TaskInfoChain:IsFollow()
    return not self.dont_show
end
function TaskInfoChain:TaskVo(vo)
    self.task_type = TaskConfig.TaskType.task_chain
    TaskInfo.TaskVo(self, vo)
    TimeHelper:CancelTimer(self.chain_task_timer)
    local info = self.task_info
    if info.task_type == TASK_CHAIN_TASK_TYPE.ITEM and info.task_limit_timestamp > 0 then
        local special = info.special
        self.chain_task_timer = TimeHelper:AddCountDownCT(function ()
            local remain = info.task_limit_timestamp - TimeHelper.GetServerTime()
            local t = TimeHelper.FormatDHMS(remain)
            self.task_desc = special.desc .. special.desc_t .. string.format(special.desc_tf, t.min, t.s)
            self.task_goal = special.desc .. special.desc_t .. string.format(special.desc_tf, t.min, t.s)
            self.task_goal_menu = special.desc_menu .. special.desc_menu_t .. string.format(special.desc_menu_tf, t.min, t.s)
        end,
        function ()
            self.task_desc = special.desc
            self.task_goal = special.desc
            self.task_goal_menu = special.desc_menu
        end, self.task_info.task_limit_timestamp, 1)
    elseif info.task_type == TASK_CHAIN_TASK_TYPE.WAIT and 0 == BagData.Instance:GetNumByItemId(info.task_param[2]) and info.task_param[3] > TimeHelper.GetServerTime() then
        local special = info.special
        self.chain_task_timer = TimeHelper:AddCountDownCT(function ()
            local remain = info.task_param[3] - TimeHelper.GetServerTime()
            local t = TimeHelper.FormatDHMS(remain)
            self.task_desc = special.desc .. string.format(special.desc_tf, t.min, t.s)
            self.task_goal = special.desc .. string.format(special.desc_tf, t.min, t.s)
            self.task_goal_menu = special.desc_menu .. string.format(special.desc_menu_tf, t.min, t.s)
        end,
        function ()
            self.task_desc = special.desc
            self.task_goal = special.desc
            self.task_goal_menu = special.desc_menu
            if BehaviorData.Instance.auto_behavior > 0 then
                self:GoTask()
            end
        end, info.task_param[3], 1)
    end
    if self.task_info and self.task_info.accept_task_level then
        self.reward_task_chain = self:CheckRewardTaskChain(self.task_info.pass_count, self.task_info.accept_task_level)
    end
end

--这里还要处理打开界面后的按钮，按钮和npcid挂钩
function TaskInfoChain:GoTask()
    local info = self.task_info
    if info.task_type == TASK_CHAIN_TASK_TYPE.TALK then
        --[[ local npc_seq = info.task_param[1]
        local cfg = {
            behavior_type = 6,
            param1 = npc_seq
        }
        BehaviorCtrl.ExeByCfgVo(cfg) ]]
        TaskData.Instance:doTaskChainTalk(info)
    elseif info.task_type == TASK_CHAIN_TASK_TYPE.ITEM then
        --[[ local npc_seq = info.task_param[1]
        local cfg = {
            behavior_type = 6,
            param1 = npc_seq
        }
        BehaviorCtrl.ExeByCfgVo(cfg) ]]
        TaskData.Instance:doTaskChainItem(info)
    elseif info.task_type == TASK_CHAIN_TASK_TYPE.FIGHT then
        --[[ local scene_id = info.task_param[2]
        local x = math.floor(info.task_param[3] / 10000)
        local y = info.task_param[3] % 10000
        SceneLogic.Instance:AutoToPos(
            scene_id,
            nil,
            function()
                Runner.Instance:RunUntilTrue(
                    function()
                        if TaskCtrl.Instance.task_chain_monster_vo then
                            SceneLogic.Instance:AutoToNpc(TaskCtrl.Instance.task_chain_monster_vo, nil, true, true)
                            return true
                        end
                    end
                )
            end,
            true,
            true
        ) ]]
        TaskData.Instance:doTaskChainFight(info)
    elseif info.task_type == TASK_CHAIN_TASK_TYPE.PET then
        -- local npc_seq = info.task_param[1]
        -- SceneLogic.Instance:AutoToNpc(npc_seq, nil, nil, true)
        TaskData.Instance:doTaskChainPet(info)
    elseif info.task_type == TASK_CHAIN_TASK_TYPE.WAIT then
        --[[ local npc_to_seq = info.task_param[1]
        local npc_from_seq = info.task_param[4]
        local cfg = {
            behavior_type = 6,
            param1 = 0 == BagData.Instance:GetNumByItemId(info.task_param[2]) and npc_from_seq or npc_to_seq
        }
        BehaviorCtrl.ExeByCfgVo(cfg) ]]
        TaskData.Instance:doTaskChainWait(info)
    end
end

function TaskInfoChain:SetCustomStr(str)
    self.custom_str = str
end

function TaskInfoChain:RequestGiveUp()
    PublicPopupCtrl.Instance:DialogTips(
        {
            content = Language.TaskChain.GiveUpTip,
            confirm = {
                func = function()
                    TaskCtrl.Instance:CSTaskChainOp(TASK_CHAIN_OP_TYPE.GIVEUP)
                    ViewMgr:CloseView(DialogTipsView)
                end
            }
        }
    )
end

function TaskInfoChain:CostTypeNum()
    return self.task_info.price_type, self.task_info.price
end

function TaskInfoChain:CanSkip()
    return 1 == self.task_info.can_skip
end

function TaskInfoChain:SkipBtn()
    if self:CanSkip() then
        return true, NpcDialogBehBtnData.FuncCreate(
            Language.TaskChain.SkipBtn[self.task_info.task_type],
            function()
                local cost_type, cost_num = self:CostTypeNum()
                local price = string.format(Language.TaskChain.CostPrice[cost_type], cost_num)
                PublicPopupCtrl.Instance:DialogTips(
                    {
                        content = string.format(Language.TaskChain.SkipTips[self.task_info.task_type], price),
                        confirm = {
                            func = function()
                                TaskCtrl.Instance:CSTaskChainOp(TASK_CHAIN_OP_TYPE.SKIP)
                                ViewMgr:CloseView(DialogTipsView)
                            end
                        }
                    }
                )
            end,
            true
        )
    end
    return false
end

function TaskInfoChain:CheckRewardTaskChain(pass_count, level)
    local cur_count = pass_count + 1
    for _, co in ipairs(Config.task_chain_auto.turn_table) do
        if cur_count <= co.completion_times and level >= co.min_level_turn and level <= co.max_level_turn then
            local reward = co.reward_item
            return Item.Init(reward.item_id, co.num, co.is_bind)
        end
    end
end

function TaskInfoChain:AutoNpcDialogChain()
    
    BehaviorData.Instance.auto_behavior = 0
    local info = self.task_info
    if info.task_type == TASK_CHAIN_TASK_TYPE.TALK then

    elseif info.task_type == TASK_CHAIN_TASK_TYPE.ITEM then

    elseif info.task_type == TASK_CHAIN_TASK_TYPE.FIGHT then
        BattleCtrl.BattleModeFightReq(
            {
                battle_mode = BATTLE_MODE.BATTLE_MODE_TASK_CHAIN
            }
        )
    elseif info.task_type == TASK_CHAIN_TASK_TYPE.PET then

    elseif info.task_type == TASK_CHAIN_TASK_TYPE.WAIT then

    end
end

TaskInfoChainStart = TaskInfoChainStart or BaseClass(TaskInfo)

function TaskInfoChainStart:__init()
end
function TaskInfoChainStart:TaskVo(vo)
    self.task_type = TaskConfig.TaskType.task_chain
    TaskInfo.TaskVo(self, vo)
    self.dont_show = TaskData.Instance:TaskState(self.task_type)
end
function TaskInfoChainStart:GoTask()
    local cfg = CommonStruct.BehCfg()
    local npc_id = Config.task_chain_auto.other[1].start_npc
    cfg.param1 = npc_id
    cfg.behavior_type = 6
    -- cfg.behavior_id = 4--默认是打开活动界面
    BehaviorCtrl.ExeByCfgVo(cfg)
    TaskData.Instance:TaskChainFakeTask()
end
function TaskInfoChainStart:TaskInfo()
    -- self.is_follow = true
    return self
end
--[[ function TaskInfoChainStart:IsFollow()
    return self.is_follow
end ]]

TaskInfoChainReward = TaskInfoChainReward or BaseClass(TaskInfo)

function TaskInfoChainReward:__init()
    self.TaskKey = "TaskInfoChainReward"
end
function TaskInfoChainReward:TaskVo(vo)
    self.task_type = TaskConfig.TaskType.task_chain
    TaskInfo.TaskVo(self, vo)
end
function TaskInfoChainReward:GoTask()
    local npc_id = Config.task_chain_auto.other[1].start_npc
    SceneCtrl.Instance:Logic():AutoToNpc(npc_id, function ()
        -- ViewMgr:OpenView(BountyTaskRewardGetView, {title = Language.Task.TaskChainReward })
        TaskCtrl.Instance:CSTaskChainOp(TASK_CHAIN_OP_TYPE.FETCH_REWARD)
    end, false, true)
end
function TaskInfoChainReward:TaskInfo()
    self.is_follow = true
    return self
end
function TaskInfoChainReward:IsFollow()
    return self.is_follow
end

TaskInfoChainStage = TaskInfoChainStage or BaseClass(TaskInfo)

function TaskInfoChainStage:__init()
    self.TaskKey = "TaskInfoChainStage"
end
function TaskInfoChainStage:TaskVo(vo)
    self.task_type = TaskConfig.TaskType.task_chain
    TaskInfo.TaskVo(self, vo)
end
function TaskInfoChainStage:GoTask()
    local stage = TaskData.Instance.task_chain_data.count
    if stage and stage > 0 then
        local next_lv = Config.task_chain_auto.level_times[stage].max_level + 1
        PublicPopupCtrl.Instance:Center(string.format(Language.TaskChain.LevelTips, next_lv))
    end
end
function TaskInfoChainStage:TaskInfo()
    self.is_follow = true
    return self
end
function TaskInfoChainStage:IsFollow()
    return self.is_follow
end

TaskInfoMSLantern = TaskInfoMSLantern or BaseClass(TaskInfo)

function TaskInfoMSLantern:__init()
    self.is_follow = true
end

function TaskInfoMSLantern:TaskVo(vo)
    self.task_type = TaskConfig.TaskType.ms_lantern
    TaskInfo.TaskVo(self, vo)
    self.block_view = true
end

function TaskInfoMSLantern:GoTask()
    MSLanternCtrl.Instance:JudgeStartTask()
end

function TaskInfoMSLantern:TaskInfo()
    self.is_follow = true
    return self
end

function TaskInfoMSLantern:IsFollow()
    return true
end

TaskInfoImperialExam = TaskInfoImperialExam or BaseClass(TaskInfo)

function TaskInfoImperialExam:__init()
    self.is_follow = true
end

function TaskInfoImperialExam:TaskVo(vo)
    self.task_type = TaskConfig.TaskType.imperial_exam
    TaskInfo.TaskVo(self, vo)
    self.block_view = true
end

function TaskInfoImperialExam:GoTask()
    ImperialExamCtrl.Instance:JudgeStartTask()
end

function TaskInfoImperialExam:TaskInfo()
    self.is_follow = true
    return self
end

function TaskInfoImperialExam:IsFollow()
    return true
end

TaskInfoDragon = TaskInfoDragon or BaseClass(TaskInfo)

function TaskInfoDragon:__init()
    self.is_follow = false
end

function TaskInfoDragon:TaskVo(vo)
    self.task_type = TaskConfig.TaskType.dragon
    TaskInfo.TaskVo(self, vo)
    self.block_view = CUSTOM_OBJID.FantasyFB == self.task_id or CUSTOM_OBJID.FightDen == self.task_id
    self.dont_show = CUSTOM_OBJID.FantasyFB == self.task_id or CUSTOM_OBJID.FightDen == self.task_id
end

function TaskInfoDragon:GoTask()
    if self.show_level and self.show_level > RoleData.Instance:GetRoleLevel() then
        PublicPopupCtrl.Instance:Center(Language.Common.NotEnoughLevel)
        return
    end
    if self.task_id == CUSTOM_OBJID.RESEARCH_TASK then
        if TeamCtrl.Instance:Data():InTeam() then
			local param_t = {
			content = Language.ResearchTask.ChallengeInTeam,
			confirm = {
					func = function()
						ViewMgr:CloseView(DialogTipsView)
                        Call(self.go_task_func)
					end
				},
			}
			PublicPopupCtrl.Instance:DialogTips(param_t)    
		else
            Call(self.go_task_func)
        end
    else
        Call(self.go_task_func)
    end
end

function TaskInfoDragon:DragonTaskParam(func, limit_level)
    self.go_task_func = func
    self.show_level = limit_level
    self:CheckDargonShow()
    return RoleData.Instance.base_data:Care(BindTool.Bind(self.CheckDargonShow, self), "level")
end

function TaskInfoDragon:TaskInfo()
    -- self.is_follow = true
    return self
end

function TaskInfoDragon:IsFollow()
    -- LogError("TaskInfoDragon:IsFollow()", TaskData.Instance.task_trace_list[self.task_type])
    -- self:CheckDargonShow()
    return TaskData.Instance.task_trace_list[self.task_type] == self.task_id and CUSTOM_OBJID.FightDen ~= self.task_id
end

function TaskInfoDragon:CheckDargonShow()
    -- self.dont_show = true
    --[[ local hide = TaskData.Instance:TaskState(self.task_type)
    if hide == true then
        self.dont_show = hide
    else
        local dont_show = RoleData.Instance:GetRoleLevel() < self.show_level
        if dont_show ~= self.dont_show then
            self.dont_show = dont_show
            TaskData.Instance:DragonTaskTrace()
        end
        self.dont_show = dont_show
    end ]]
end

function TaskInfoDragon:AutoTask()
    if self.task_id == CUSTOM_OBJID.RESEARCH_TASK then
        AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(self)}, 1)
    else
        if TeamData.Instance:IsLeader() then
            AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(self)}, 1)
        end
    end
end

-------------------------义胆助人-----------------------
TaskInfoHelpOthers = TaskInfoHelpOthers or BaseClass(TaskInfo)

function TaskInfoHelpOthers:__init()
    self.block_view = true
end

function TaskInfoHelpOthers:TaskInfo()
    TaskInfo.TaskInfo(self)
    return self
end

function TaskInfoHelpOthers:RemoveHandle()
    if nil ~= self.item_handle then
        BagData.Instance.item_grid:Uncare(self.item_handle)
        self.item_handle = nil
    end
    if nil ~= self.pet_handle then
        PetData.Instance.pet_list:Uncare(self.pet_handle)
        self.pet_handle = nil
    end
end

function TaskInfoHelpOthers:TaskVo(info, co, progress)
    local vo = CommonStruct.CustomTask()
    vo.task_id = info.task_type or CUSTOM_OBJID.HELP_OTHERS
    vo.task_name = co.task_name
    vo.task_goal = TaskConfig.HelpOthersStatus.accept == info.task_status and co.progress_desc or co.obj_desc
    vo.npc_id = TaskConfig.HelpOthersStatus.accept == info.task_status and co.accept_npc or co.commit_npc
    self.task_vo = info
    self.task_type = TaskConfig.TaskType.help_others
    self.info = co
    TaskInfo.TaskVo(self, vo)
    self.task_id = info.task_type or CUSTOM_OBJID.HELP_OTHERS
    self.progress_pass = progress

    self:RemoveHandle()
    if TaskConfig.HelpOthersStatus.commit == info.task_status then
        if 2 == co.task_type or 5 == co.task_type then
            self.item_handle = BagData.Instance.item_grid:Care(BindTool.Bind(self.FlushGoalItem, self))
            self:FlushGoalItem()
        elseif 4 == co.task_type then
            self.pet_handle = PetData.Instance.pet_list:Care(BindTool.Bind(self.FlushGoalPet, self))
            self:FlushGoalPet()
        elseif 3 == co.task_type then
            self.task_goal = co.obj_desc .. string.format("(%s/%s)", math.min(info.progress, co.param_2), co.param_2)
        end
    end
end

function TaskInfoHelpOthers:FlushGoalItem()
    local co = self.info
    self.task_goal = co.obj_desc .. string.format("(%s/%s)", math.min(BagData.Instance:GetNumByItemId(co.param_1), co.param_2), co.param_2)
end

function TaskInfoHelpOthers:FlushGoalPet()
    local co = self.info
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
    self.task_goal = co.obj_desc .. string.format("(%s/%s)", math.min(#items, co.param_2), co.param_2)
end

function TaskInfoHelpOthers:GoTask()
    if TaskConfig.HelpOthersStatus.commit == self.task_vo.task_status and 3 == self.info.task_type and self.task_vo.progress < self.info.param_2 then
        ViewMgr:OpenViewByKey(Mod.Fight.Field)
    else
        SceneLogic.Instance:AutoToNpc(self.npc_id, nil, nil, true)
    end
end

function TaskInfoHelpOthers:IsFollow()
    return true
end

function TaskInfoHelpOthers:MenuTitle()
    if 1 == self.info.task_id then
        return RichTextHelper.ColorStr(self.info.task_prefix1, TaskConfig.TaskMenuTypeColor[self.task_type])
    else
        return string.format(Language.Task.Menu.TaskName, TaskConfig.TaskMenuTypeColor[self.task_type], self.info.task_prefix2, self.task_name) .. RichTextHelper.ColorStr(self.progress_pass, TaskConfig.TaskMenuTypeColor[self.task_type])
    end
end

-------------------------巅峰之路-----------------------
TaskPeakRoad = TaskPeakRoad or BaseClass(TaskInfo)

function TaskPeakRoad:__init()
end

function TaskPeakRoad:TaskInfo()
    TaskInfo.TaskInfo(self)
    self.can_ab = false
    return self
end
function TaskPeakRoad:TaskVo(vo)
    TaskInfo.TaskVo(self, vo)
end
function TaskPeakRoad:MenuTitle()
    return RichTextHelper.ColorStr(self.task_name, TaskConfig.TaskMenuTypeColor[self.task_type])
end
function TaskPeakRoad:GoTask()
    ViewMgr:OpenView(PeakRoadReward)
end

-------------------------声望任务-----------------------
TaskInfoPrestigeStart = TaskInfoPrestigeStart or BaseClass(TaskInfo)
function TaskInfoPrestigeStart:TaskVo(vo)
    self.task_type = TaskConfig.TaskType.prestige
    TaskInfo.TaskVo(self, vo)
    self.dont_show = TaskData.Instance:TaskState(self.task_type)
    self.custom_str = Language.Task.TaskName.Prestige
end

function TaskInfoPrestigeStart:IsShowNpcDialogButton()
    return false
end

function TaskInfoPrestigeStart:GoTask()
    local cfg = CommonStruct.BehCfg()
    local npc_seq = PrestigeData.Instance:PrestigeTaskOtherCfg().task_npcseq
    cfg.behavior_type = 6
    cfg.param1 = npc_seq
    BehaviorCtrl.ExeByCfgVo(cfg)
    TaskData.Instance:TaskChainFakeTask()
end

function TaskInfoPrestigeStart:IsFollow()
    return (not self.dont_show) and TaskInfo.IsFollow(self)
end

function TaskInfoPrestigeStart:MenuTitle()
    local task_title = string.format(Language.Task.Menu.TaskName, TaskConfig.TaskMenuTypeColor[self.task_type], Language.Task.TaskType[self.task_type], self.task_name)
    return task_title
end

TaskInfoAllSoulsStorybook = TaskInfoAllSoulsStorybook or BaseClass(TaskInfo)
function TaskInfoAllSoulsStorybook:TaskVo(vo)
    --生成NPC 
    --点击寻路对话
    --或者对话战斗
    if self.task_vo then
        --老VO删除Npc
        SceneCtrl.Instance:DeleteNpc(tonumber(self.task_vo.npc_id))

    end
    self.task_vo = vo
    SceneCtrl.Instance:AddNpc(tonumber(self.task_vo.npc_id))
end
function TaskInfoAllSoulsStorybook:GoTask()
    --
    if self.task_vo.task_type == 1 then
        if self.task_vo.is_need_talk == 1 then
            ViewMgr:OpenView(Npcdra)
        end
    end
end


-- ======================= 生活系统的道具 =====================
LivePropTask = LivePropTask or BaseClass(TaskInfo)

function LivePropTask:__init()
    self.task_type = TaskConfig.TaskType.live_prop
    self.block_view = true
    self.remind = false
end

function LivePropTask:IsFollow()
    return true
end

function LivePropTask:TaskInfo()
    TaskInfo.TaskInfo(self)
    self.can_ab = true
    return self
end
function LivePropTask:TaskVo(vo)
    local vo = vo or CommonStruct.CustomTask()
    vo.task_id = vo.task_id or CUSTOM_OBJID.LIVE_PROP
    vo.task_type = TaskConfig.TaskType.live_prop
    vo.end_timestamp = vo.end_timestamp or 0
    self.task_id = vo.task_id
    TaskInfo.TaskVo(self, vo)
end
function LivePropTask:MenuTitle()
    return RichTextHelper.ColorStr(self.task_name, TaskConfig.TaskMenuTypeColor[self.task_type])
end
function LivePropTask:TaskGoal()
    local server_time = TimeCtrl.Instance:GetServerTime()
    local time = self.vo.end_timestamp - server_time
    local time_data = LiveCtrl.Instance:FormatDHMS(time)
    local time_str = ColorStr(string.format("%s:%s:%s", time_data.hour, time_data.min, time_data.s), COLORSTR.Green10)
    self.vo.task_goal = string.format(Language.Live.TaskTime, time_str)
    return self.vo.task_goal
end
function LivePropTask:GoTask()
    if self.live_task_type == LiveCtrl.TASK_TYPE.CARD then
        LiveCtrl.Instance:UseCard()
    else
        ViewMgr:OpenView(LiveView)
    end
end

-------------------------九州秘宝-----------------------
TaskInfoTreasureTask = TaskInfoTreasureTask or BaseClass(TaskInfo)

function TaskInfoTreasureTask:__init()
    self.block_view = true
end

function TaskInfoTreasureTask:TaskInfo()
    TaskInfo.TaskInfo(self)
    return self
end

function TaskInfoTreasureTask:RemoveHandle()
end

function TaskInfoTreasureTask:TaskVo(info, co, progress)
    local vo = CommonStruct.CustomTask()
    vo.task_id = CUSTOM_OBJID.TREASURE_TASK
    vo.task_name = co.task_name
    vo.task_goal = co.obj_desc
    self.task_vo = info
    self.task_type = TaskConfig.TaskType.treasure_task
    self.info = co
    TaskInfo.TaskVo(self, vo)
    self.task_id = CUSTOM_OBJID.TREASURE_TASK
    self.pos_id = info.pos_id
    self.progress_pass = progress
end

function TaskInfoTreasureTask:GoTask()
    local co = Cfg.TreasureTaskRewardCoordinateBySeq(self.pos_id)
    if co then
        SceneLogic.Instance:AutoToPos(co.scene, {x = co.x, y = co.y}, nil, true, true)
    end
end

function TaskInfoTreasureTask:IsFollow()
    return true
end

function TaskInfoTreasureTask:MenuTitle()
    return string.format(Language.Task.Menu.TaskName, TaskConfig.TaskMenuTypeColor[self.task_type], Language.Task.TaskType[self.task_type], self.task_name) .. RichTextHelper.ColorStr(self.progress_pass, TaskConfig.TaskMenuTypeColor[self.task_type])
end

function TaskInfoTreasureTask:IsAutoGo()
    return nil == TaskData.Instance:GetTaskInfoByTaskId(713) or 2 ~= self.task_vo.task_id
end

------ 家族每日任务 ------
TaskInfoCooperateTask = TaskInfoCooperateTask or BaseClass(TaskInfo)
function TaskInfoCooperateTask:GoTask()

end 