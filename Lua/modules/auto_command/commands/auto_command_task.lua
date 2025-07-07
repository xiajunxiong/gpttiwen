
AutoCommandTask = AutoCommandTask or BaseClass(AutoCommand)
function AutoCommandTask:__init(task_info, from_click)
    self.auto_state = AutoCommandState.Task
    self.task_info = task_info
    self.from_click = from_click
end

function AutoCommandTask:DoExecute()
    -- TaskData.Instance.menu_guide.val = 0
    TaskData.Instance:TaskIngEffect(self.task_info.task_id)
    if self.task_info.task_type == TaskConfig.TaskType.mainline and self.task_info.mainline_need_click and self.from_click and not IS_STANDALONE() then
        self.task_info:MainLineTaskNeedClick(false)
        self.task_info:RequestCommit()
    else
        self.task_info:GoTask()
    end
end

function AutoCommandTask:DoStop()
    TaskData.Instance:TaskIngEffect(0)
    if TaskConfig.TaskType.mainline == self.task_info.task_type and TaskData.Instance:MainLineArrowFlag() then
        TaskData.Instance.menu_guide.val = TaskData.Instance.menu_guide.pre_val
    end
end