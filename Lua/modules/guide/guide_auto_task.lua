GuideAutoTask = GuideAutoTask or BaseClass(GuideBase)

function GuideAutoTask:Start(step_cfg, callback)
    local task_id  = step_cfg.step_param_1
    local task = TaskData.Instance.task_list[task_id]
    if task then
        AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(task)}, 1)
    end
    self.callback = callback
    self:Continue()
end 

function GuideAutoTask:Continue()
    if self.callback then
        self.callback()
    end
    self.callback = nil
    self.step_cfg = nil
end

function GuideAutoTask:Finish()
    self.callback = nil
    self.step_cfg = nil
end