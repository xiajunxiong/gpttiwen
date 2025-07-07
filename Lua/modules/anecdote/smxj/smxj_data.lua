SmxjData = SmxjData or BaseClass()
function SmxjData:__init()
    if SmxjData.Instance ~= nil then
        Debuger.LogError("[SmxjData] attempt to create singleton twice!")
        return
    end
    SmxjData.Instance = self
    self:SmxjData()
end

function SmxjData:SmxjData()
    self.task_list = Config.smxj_story_auto.talk_list
end

--离开场景清下数据
function SmxjData:OnLeaveScene()
    self.talking = false
    if self.cur_task ~= nil then
        self:ExecuteNpc(false, self.cur_task.commit_npc)
        self.cur_task = nil
    end
    if self.delay_timer ~= nil then
        TimeHelper:CancelTimer(self.delay_timer)
        self.delay_timer = nil
    end
end

function SmxjData:ExecuteNpc(is_add, show_npcs)
    if not StringIsEmpty(show_npcs) then
        for i, v in ipairs(Split(show_npcs, "|")) do
            if is_add == true then
                SceneCtrl.Instance:AddNpc(tonumber(v), true)
            else
                SceneCtrl.Instance:DeleteNpc(tonumber(v), true)
            end
        end
    end
end

function SmxjData:ToNextTask()
    if self.cur_task ~= nil then
        self:ExecuteNpc(false, self.cur_task.commit_npc)
    end
    self.task_id = self.task_id + 1
    self.cur_task = self.task_list[self.task_id]
    if self.cur_task ~= nil then
        self:ExecuteNpc(true, self.cur_task.commit_npc)
    end
end

function SmxjData:IsSelfNpc(param_t, obj_vo)
    local task_info = AnecdoteData.Instance:GetTaskInfoByType(AnecdoteType.StarLife)
    if task_info ~= nil and self.cur_task ~= nil then
        if obj_vo.seq == self.cur_task.commit_npc then
            SmxjData.Instance:NpcDialog()
            return true 
        end
        return false
    end
    return false
end

function SmxjData:NpcDialog()
    local beh_btn_list = {}
    if not StringIsEmpty(self.cur_task.commit_btn_txt) then
        for i, v in ipairs(Split(self.cur_task.commit_btn_txt, "|")) do
            local function func_btn()
                AnecdoteData.Instance:SetSelectIndex(AnecdoteType.StarLife, i)
            end
            table.insert(beh_btn_list, NpcDialogBehBtnData.FuncCreate(v, func_btn))
        end
    end
    local commit_dialog_beh = self.cur_task.commit_dialog_beh
    if type(commit_dialog_beh) == "string" then
        --多选题
        local beh_list = Split(commit_dialog_beh, "|")
        local task_select = AnecdoteData.Instance:GetSelectIndex(self.info.stranger_task_type)
        commit_dialog_beh = tonumber(beh_list[task_select] or beh_list[1])
    end
    local not_block_close = beh_btn_list[2] ~= nil
    local param_t = {talk_id = commit_dialog_beh, beh_btn_list = beh_btn_list, not_block_close = not_block_close}
    local end_func = BindTool.Bind(self.OnDialogEnd, self)
    NpcDialogData.Instance:SetCurTaskInfo(nil)
    NpcDialogCtrl.Instance:NpcDialogView(param_t, end_func)
end

function SmxjData:OnDialogEnd()
    if not self.talking then
        return
    end
    self:ToNextTask()
    self:AutoToTalkNpc()
    if self.cur_task == nil then
        self.talking = false
        local task_info = AnecdoteData.Instance:GetTaskInfoByType(AnecdoteType.StarLife)
        if task_info ~= nil then
            AnecdoteCtrl.Instance:SendTaskActionReq({param1 = task_info.info.c_param1})
            if SmxjData.SendTaskFinishReq then
                SmxjData.SendTaskFinishReq()
            end
        end
    end
end

function SmxjData:StartStory()
    self.task_id = 0
    self.talking = true
    self:ToNextTask()
    self:AutoToTalkNpc(3)
end

function SmxjData:AutoToTalkNpc(time)
    if self.cur_task == nil or self.delay_timer ~= nil then
        return
    end
    local function func_delay()
        self.delay_timer = nil
        if self.cur_task.commit_npc ~= "" then
            SceneLogic.Instance:AutoToNpc(self.cur_task.commit_npc)
        else
            self:NpcDialog()
        end
    end
    self.delay_timer = TimeHelper:AddDelayTimer(func_delay, time or 1)
end

function SmxjData:SmxjSpTaskId()
    return Config.smxj_story_auto.other[1].task_id + 1
end
