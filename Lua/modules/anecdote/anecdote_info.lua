-------------------------奇闻任务-----------------------
TaskInfoAnecdote = TaskInfoAnecdote or BaseClass(TaskInfo)

function TaskInfoAnecdote:__init()
    self.is_add_npc = false
    self.is_add_other = false
end

function TaskInfoAnecdote:TaskInfo()
    TaskInfo.TaskInfo(self)
    self.is_follow = true
    self.is_add_npc = false
    self.is_add_other = false
    return self
end

function TaskInfoAnecdote:TaskVo(vo)
    self.vo = vo
    self.task_id = vo.task_id
    self.list_type = vo.list_type
    self.progress = self.vo.progress
    self.is_all_info = self.vo.is_all_info
    self.has_finish = vo.has_finish or 0

    self.info = Config.anecdotes_tasklist_auto.task_list[self.task_id]
    if self.info == nil then
        LogError("配置不存在" .. self.task_id)
        return
    end
    self.task_type = self.info.task_type
    self.stranger_task_type = self.info.stranger_task_type
    self.is_accepted = TaskConfig.ListType.accepted == self.list_type
    self.is_acceptable = TaskConfig.ListType.acceptable == self.list_type
    self.is_finished = self.is_accepted and self.progress > (tonumber(self.info.c_param3) or 0)
    self.npc_seq = self:GetNpcSeq()

    self:DynamicNpc()
    self:OtherNpc()
    self:DefaultEvent()
    self:DialogEvent()
    self:AutoCommit()
    self:AutoTalk()
end

function TaskInfoAnecdote:State()
    if self.is_acceptable then
        return TaskConfig.TaskState.undo
    elseif self.is_finished then
        return TaskConfig.TaskState.done
    else
        return TaskConfig.TaskState.doing
    end
end

function TaskInfoAnecdote:RemoveTask()
    self:ClearShowNpc()
    self:ClearOtherNpc()
end

function TaskInfoAnecdote:DynamicNpc()
    if StringIsEmpty(self.info.is_show_npc) then
        return
    end
    if TaskConfig.TaskState.done ~= self:State() and not self.is_add_npc then
        self:ExecuteNpc(true,self.info.is_show_npc)
        self.is_add_npc = true
    end
    if TaskConfig.TaskState.done == self:State() and self.is_add_npc then
        self:ExecuteNpc(false,self.info.is_show_npc)
        self.is_add_npc = false
    end
end

function TaskInfoAnecdote:OtherNpc()
    if StringIsEmpty(self.info.show_other_npcseq) then
        return
    end
    if TaskConfig.TaskState.done ~= self:State() and not self.is_add_other then
        self:ExecuteNpc(true,self.info.show_other_npcseq)
        self.is_add_other = true
    end
    if TaskConfig.TaskState.done == self:State() and self.is_add_other then
        self:ExecuteNpc(false,self.info.show_other_npcseq)
        self.is_add_other = false
    end
end

function TaskInfoAnecdote:DefaultEvent()
    if not StringIsEmpty(self.info.is_show_npc) then
        return
    end
    if StringIsEmpty(self.info.is_show_accpet) then
        return
    end
    if TaskConfig.TaskState.done == self:State() then
        SceneCtrl.Instance:DeleteNpc(self.info.commit_npc,true)
    end
    if self.is_accepted then
        SceneCtrl.Instance:DeleteNpc(self.info.accept_npc,true)
    end
    if self.npc_seq ~= 0 and TaskConfig.TaskState.done ~= self:State()then
        -- 推荐任务天下奇闻特殊逻辑
        if 59053 ~= self.npc_seq then
            SceneCtrl.Instance:AddNpc(self.npc_seq,true)
        end
    end
end

--强制清理一波NPC
function TaskInfoAnecdote:ClearShowNpc()
    if StringIsEmpty(self.info.is_show_npc) then
        return
    end
    self:ExecuteNpc(false,self.info.is_show_npc)
    self.is_add_npc = false
end

function TaskInfoAnecdote:ClearOtherNpc()
    if StringIsEmpty(self.info.show_other_npcseq) then
        return
    end
    self:ExecuteNpc(false,self.info.show_other_npcseq)
    self.is_add_other = false
end

--默认自动提交
function TaskInfoAnecdote:AutoCommit()
    self:RequestCommit()
end

--自动任务对话
function TaskInfoAnecdote:AutoTalk()
    if self:IsAutoTalk() then
        if self.info.loading_talk ~= 0 then
            CutChapterCtrl.Instance:OpenCutChapter(self.info.loading_talk,function()
                self:AutoToTalkNpc(true)
            end)
        else
            self:AutoToTalkNpc(false)
        end
    end
end

--自动到NPC
function TaskInfoAnecdote:AutoToTalkNpc(is_loading_talk)
    if self.npc_seq ~= 0 then
        TimeHelper:AddDelayTimer(function()
            SceneLogic.Instance:AutoToNpcs(self.npc_seq)
        end, is_loading_talk == false and 1 or 0)
    else
        self:NpcDialog()
    end
end

function TaskInfoAnecdote:DialogEvent()
    if StringIsEmpty(self.info.commit_btn_txt) and self.info.commit_dialog_beh ~= 0 and self.is_all_info == false then
        NpcDialogData.Instance:SetCurTaskInfo(self)
    end
end

function TaskInfoAnecdote:AutoCfg()
    return self.info
end

function TaskInfoAnecdote:IsShowNpcBoardSign()
    return false
end

--接受任务
function TaskInfoAnecdote:RequestAccept()
    if TaskConfig.TaskState.undo == self:State() then
        AnecdoteCtrl.Instance:SendTaskAccept(self.task_id)
    end
end

--提交任务
function TaskInfoAnecdote:RequestCommit()
    if TaskConfig.TaskState.done == self:State() then
        AnecdoteCtrl.Instance:SendTaskCommit(self.task_id)
    end
end

--提交挑战行为
function TaskInfoAnecdote:RequestAction()
    AnecdoteCtrl.Instance:SendTaskActionReq({action_type = 2,param1 = self.stranger_task_type})
end

--提交任务中的行为
function TaskInfoAnecdote:RequestDoing()
    self:DoDialogBeh()
end

--任务中的行为
function TaskInfoAnecdote:TaskDialogBeh()
    if StringIsEmpty(self.info.tasks_dialog_beh) then
        return
    end
    local params = Split(self.info.tasks_dialog_beh, "|")
    local npc_seq = tonumber(params[5]) or 0
    local end_beh = {
        behavior_type = tonumber(params[1]),
        param1 = tonumber(params[2]) or params[2],
        param2 = tonumber(params[3]) or params[3],
        param3 = tonumber(params[4]) or params[4],
        task_info = self
    }
    if npc_seq > 0 then
        self:AutoToNpc(tonumber(self.info.fly_to) or 0, npc_seq, end_beh)
    else
        if end_beh.behavior_type == 4 or end_beh.behavior_type == 8 then
            if TeamCtrl.Instance:Data():InTeam() then
                PublicPopupCtrl.Instance:AlertTip(Language.Anecdote.PopupTeamTip,function()
                    TeamCtrl.Instance:SendExitTeam()
                    BehaviorCtrl.ExeByCfgVo(end_beh,Cfg.NpcBySeq(self.npc_seq))
                end)
                return
            end
        end
        BehaviorCtrl.ExeByCfgVo(end_beh,Cfg.NpcBySeq(self.npc_seq))
    end
end

function TaskInfoAnecdote:AutoToNpc(scene_id, npc_seq, end_beh)
    SceneLogic.Instance:AutoToNpc(npc_seq,function(npc_obj)
        if npc_obj == nil then
            npc_obj = {vo = Cfg.NpcBySeq(npc_seq)}
        end
        BehaviorCtrl.ExeByCfgVo(end_beh, npc_obj.vo)
    end,nil,0 ~= scene_id)
end

--几个对话任务的特殊逻辑
function TaskInfoAnecdote:NpcDialog(param_t,obj_vo)
    local beh_btn_list = param_t and param_t.beh_btn_list or {}
    if self:IsNotItem() then
        local talk_id = AnecdoteData.Instance:GetNormalTalkId(self.stranger_task_type)
        if talk_id ~= 0 then
            NpcDialogCtrl.TalkNpcDialog(talk_id,beh_btn_list)
        else
            NpcDialogCtrl.Instance:NpcDialogView(param_t,nil,true, obj_vo and obj_vo.id or nil)
        end
    else
        if self.is_acceptable then
            if self.info.accept_dialog_beh ~= 0 then
                table.insert(beh_btn_list,NpcDialogBehBtnData.FuncCreate(self.info.accept_btn_txt,function()
                    if self.info.accept_npc ~= 0 then
                        self:RequestAccept()
                    end
                    self:DoCommitDialogBeh()
                end,false))
                NpcDialogCtrl.TalkNpcDialog(self.info.accept_dialog_beh,beh_btn_list)
            end
        elseif self.is_accepted or self.info.accept_dialog_beh == 0 then
            self:DoCommitDialogBeh(beh_btn_list)
        end
    end
end

function TaskInfoAnecdote:DoCommitDialogBeh(beh_btn_list)
    beh_btn_list = beh_btn_list or {}
    if not StringIsEmpty(self.info.commit_btn_txt) then
        for i, v in ipairs(Split(self.info.commit_btn_txt, "|")) do
            table.insert(beh_btn_list,NpcDialogBehBtnData.FuncCreate(v, function()
                AnecdoteData.Instance:SetSelectIndex(self.info.stranger_task_type, i)
                self:DoDialogBeh()
            end))
        end
        NpcDialogData.Instance:SetCurTaskInfo(nil)
    else
        NpcDialogData.Instance:SetCurTaskInfo(self)
    end
    local commit_dialog_beh = self.info.commit_dialog_beh
    if type(commit_dialog_beh) == "string" then
        local beh_list = Split(self.info.commit_dialog_beh, "|")
        local task_select = AnecdoteData.Instance:GetSelectIndex(self.info.stranger_task_type)
        commit_dialog_beh = tonumber(beh_list[task_select] or beh_list[1])
    end
    local param_t = {talk_id = commit_dialog_beh, beh_btn_list = beh_btn_list, not_block_close = self:NotBlockClose()}
    NpcDialogCtrl.Instance:NpcDialogView(param_t, function ()
        if AnecdoteType.ArrayGhost == self.stranger_task_type and AnecdoteData.IsLastTask(AnecdoteType.ArrayGhost, self.task_id) then
            SceneCtrl.Instance:RequestLeaveFb()
        end
    end)
end

function TaskInfoAnecdote:DoDialogBeh()
    if self.info.condition == 1 then
        if self.task_id ~= SmxjData.Instance:SmxjSpTaskId() then
            AnecdoteCtrl.Instance:SendTaskActionReq({param1 = self.info.c_param1})
        end
    elseif self:IsSubTask() then
        ItemsSubData.Instance:SetSubInfo(self:SubItems(),function(sel_items)
            return self:SubItems(sel_items)
        end)
    elseif 15 == self.info.condition then
        if TeamCtrl.Instance:Data():InTeam() then
            PublicPopupCtrl.Instance:AlertTip(Language.Anecdote.PopupTeamTip,function()
                TeamCtrl.Instance:SendExitTeam()
                SceneCtrl.Instance:RequestGroupIdBattle(self.info.is_show_npc, self.info.c_param1)
            end)
        else
            SceneCtrl.Instance:RequestGroupIdBattle(self.info.is_show_npc, self.info.c_param1)
        end
        return
    end
    if self.info.tasks_dialog_beh ~= 0 and not StringIsEmpty(self.info.tasks_dialog_beh)then
        self:TaskDialogBeh()
    end
end

--提交道具重写
function TaskInfoAnecdote:SubItems(sub_items)
    if nil == sub_items then
        local sub_items = {}
        if 6 == self.info.condition then
            local co = Config.tasklist_auto.item_group[self.info.c_param1] or 
            Config.anecdotes_tasklist_auto.item_group[self.info.c_param1] or 
            Config.medaltask_list_auto.item_group[self.info.c_param1]
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
                item_data.sub_num = self.info.c_param2
                table.insert(sub_items, item_data)
            end
        end
        return sub_items
    else
        local is_enough = true
        for _, item in pairs(sub_items) do
            if type(item:Num()) == "number" and type(item.sub_num) == "number" and item.sub_num > item:Num() then
                is_enough = false
            end
        end
        if not is_enough then
            PublicPopupCtrl.Instance:Center(Language.Task.SubItemsTips)
            return false
        end
        AnecdoteCtrl.Instance:SendGiveNPCTaskNeedItems(self.task_id,sub_items)
        return true
    end
end

function TaskInfoAnecdote:IsSubTask()
    return 11 == self.info.condition or 21 == self.info.condition or 6 == self.info.condition
end

function TaskInfoAnecdote:IsNotItem()
    if 11 == self.info.condition then
        return Item.GetNum(self.info.c_param1) < self.info.c_param2
    end
    return false
end

function TaskInfoAnecdote:IsAutoTalk()
    return self.info.is_auto_talk == 1 and self.is_accepted and self.progress == 0 and self.is_all_info == false
end

function TaskInfoAnecdote:IsLevelEnough()
    return RoleData.Instance:GetRoleLevel() >= self.info.min_level
end

function TaskInfoAnecdote:GetNpcSeq()
    local npc_seq = self.is_accepted and self.info.commit_npc or self.info.accept_npc
    self.npc_list = tonumber(npc_seq) == nil and Split(npc_seq,"|") or nil
    return self.npc_list and tonumber(self.npc_list[#self.npc_list]) or npc_seq
end

function TaskInfoAnecdote:ExecuteNpc(is_add,show_npcs)
    for i,v in ipairs(Split(show_npcs, "|")) do
        if is_add == true then
            SceneCtrl.Instance:AddNpc(tonumber(v),true)
        else
            SceneCtrl.Instance:DeleteNpc(tonumber(v),true)
        end
    end
end

--是否是当前任务的NPC
function TaskInfoAnecdote:IsTaskNpc(obj_vo)
    if self.npc_seq == 0 then
        return false
    end
    if self.npc_list then
        for k,v in pairs(self.npc_list) do
            if tostring(obj_vo.seq) == v then
                return true
            end
        end
    else
        return obj_vo.seq == self.npc_seq or obj_vo.id == self.npc_seq
    end
end

function TaskInfoAnecdote:NotBlockClose()
    if self.stranger_task_type == AnecdoteType.StarLife then
        if SceneData.Instance:GetMode() == SceneModeType.StarLife then
            if not StringIsEmpty(self.info.commit_btn_txt) then
                return Split(self.info.commit_btn_txt, "|")[2] ~= nil
            end
        end
    end
    return false
end
