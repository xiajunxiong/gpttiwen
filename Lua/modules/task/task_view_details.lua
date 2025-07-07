
TaskViewDetails = TaskViewDetails or DeclareMono("TaskViewDetails", UIWFlushPanel)

function TaskViewDetails:TaskViewDetails()
    self.data = TaskCtrl.Instance:Data()
    self.language = Language.Task.Detail

    self.data_cares = {
        {data = self.data:GetTaskList(), func = self.FlushDetail},
		{data = self.data.cur_sel_task, func = self.FlushDetail},
	}
end

function TaskViewDetails:FlushDetail()
    TimeHelper:CancelTimer(self.timer_ct)
    self.timer_ct = nil

    local data = self.data.cur_sel_task
    local is_empty = nil == next(data)
    self.InfoObj:SetActive(not is_empty)
    self.EmptyObj:SetActive(is_empty)

    self.TaskName.text = data.task_name
    self.TaskDesc.text =  data.task_desc
    self.TaskGoal.text = not is_empty and data.task_goal  or ""
    local reward_list = data.reward_list
    if TaskConfig.TaskType.guild == data.task_type then reward_list = TaskInfoGuild.RewardList(data.task_id - CUSTOM_OBJID.GUILD_TASK_BEGIN) end
    if Config.tasklist_auto.recommended_rewards[data.task_id] then
        reward_list = {}
        local prof_type = RoleData.Instance:ProfType()
        local co = Cfg.RewardJobBagByIdType(Config.tasklist_auto.recommended_rewards[data.task_id].reward, prof_type)
        table.insert(reward_list, Item.Init(co.new_id))
    end
    self.RewardList:SetData(reward_list or {})

    local expire_time = data.expire_time or 0
    self.TimerObj:SetActive(TaskConfig.ListType.accepted == data.list_type and expire_time ~= 0)
    if expire_time - TimeHelper.GetServerTime() > 0 then
        self.timer_ct = TimeHelper:AddCountDownCT(BindTool.Bind(self.TimerCTUpdate, self, expire_time), BindTool.Bind(self.TimerCTComplete, self), expire_time, 1)
    else
        self.Timer.text = self.language.timeout
    end

    if not is_empty then
        self.FollowSel:SetActive(data:IsFollow())
    end
    self.FollowObj:SetActive(TaskConfig.ListType.accepted == data.list_type)

    TimeHelper:CancelTimer(self.timer_ct28)
    if not is_empty and data:TaskMenuConditionPause() then
		local time_24 = TimeCtrl.Instance:ChangDataTime24(TimeHelper.GetServerTime())
		local task_desc28 = string.gsub(data.task_desc, "time_next_day", "%%s")
		local task_goal28 = string.gsub(data.task_goal, "time_next_day", "%%s")
		self.timer_ct28 = TimeHelper:AddCountDownCT(function ()
			local ft = TimeHelper.FormatDHMS(time_24 - TimeHelper.GetServerTime())
			self.TaskDesc.text = string.format(task_desc28, string.format(self.language.goal28, ft.hour, ft.min))
			self.TaskGoal.text = string.format(task_goal28, string.format(self.language.goal28, ft.hour, ft.min))
		end, function ()
			self.TaskDesc.text = string.format(task_desc28, string.format(self.language.goal28, 0, 0))
			self.TaskGoal.text = string.format(task_goal28, string.format(self.language.goal28, 0, 0))
		end, time_24)
	end
end

function TaskViewDetails:OnDestroy()
    -- LogError("TaskViewDetails:OnDestroy")
    TaskCtrl.Instance:CSTaskTraceStatus()
    TimeHelper:CancelTimer(self.timer_ct)
    TimeHelper:CancelTimer(self.timer_ct28)
    self.timer_ct = nil
    UIWFlushPanel.OnDestroy(self)
end

function TaskViewDetails:TimerCTUpdate(expire_time)
    local last_time = expire_time - TimeHelper.GetServerTime()
    local ft = TimeHelper.FormatDHMS(last_time)
    self.Timer.text = string.format(self.language.timeformat, ft.hour, ft.min, ft.s)
end

function TaskViewDetails:TimerCTComplete()
    self.TimerObj:SetActive(false)
end

function TaskViewDetails:OnClickFollow()
    local cur_sel_task = self.data.cur_sel_task
    -- local is_follow = TaskConfig.TaskType.mainline == cur_sel_task.task_type and true or not self.data.cur_sel_task.is_follow
    -- -- cur_sel_task.is_follow = TaskConfig.TaskType.mainline == cur_sel_task.task_type and true or not self.data.cur_sel_task.is_follow
    -- self.data:GetTaskInfoByTaskId(cur_sel_task.task_id).is_follow = is_follow
    -- if is_follow then
    --     GuildData.Instance:FollowOnly(cur_sel_task.task_id)
    -- end
    self.data:TraceTask(cur_sel_task.task_type, (not cur_sel_task:IsFollow() or TaskConfig.TaskType.mainline == cur_sel_task.task_type) and cur_sel_task.task_id or nil)
    self.FollowSel:SetActive(self.data.cur_sel_task:IsFollow())
end
