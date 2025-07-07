
TaskViewAccpetable = TaskViewAccpetable or DeclareMono("TaskViewAccpetable", UIWFlushPanel)

function TaskViewAccpetable:TaskViewAccpetable()
    self.data = TaskCtrl.Instance:Data()

    self.data_cares = {
		{data = self.data:GetTaskList(), func = self.FlushAccpetableList},
    }
end

function TaskViewAccpetable:FlushAccpetableList()
    local data_acceptable = self.data:GetTaskShowList(TaskConfig.ListType.acceptable)
    self.AccpetableList:SetData(data_acceptable, true, true)
    self.InfoObj:SetActive(nil ~= next(data_acceptable))
end

function TaskViewAccpetable:OnEnable()
    UIWFlushPanel.OnEnable(self)
    local data_accepted = self.data:GetTaskShowList(TaskConfig.ListType.acceptable)
    local parent = next(data_accepted)
    if nil ~= self.data.cur_sel_acceptable then
        self.cur_sel_task = self.data.cur_sel_acceptable
        self.data.cur_sel_acceptable = nil
    else
        if nil ~= parent then
            if nil == self.cur_sel_task then
                local children = data_accepted[parent].children
                self.cur_sel_task = children[next(children)]
            end
        else
            self.cur_sel_task = nil
        end
    end
    self:OnClickTaskItem(self.cur_sel_task)
    TimeHelper:AddDelayFrameTimer(function ()
        self.AccpetableList:ClickChild(self.cur_sel_task)
    end)
end

function TaskViewAccpetable:OnClickGet()
    if nil == self.cur_sel_task then return end
    -- TaskCtrl.Instance:GoTask(self.cur_sel_task)
	AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(self.cur_sel_task)}, 1)
    ViewMgr:CloseView(TaskView)
end

function TaskViewAccpetable:OnClickTaskItem(data)
    self.cur_sel_task = data
    self.data.cur_sel_task:Set(data or {})
end

TaskViewAccpetableAccordionItemParent = DeclareMono("TaskViewAccpetableAccordionItemParent", UIWidgetBaseItem)

function TaskViewAccpetableAccordionItemParent:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.TypeName.text = data.name
end

TaskViewAccpetableAccordionItemChild = DeclareMono("TaskViewAccpetableAccordionItemChild", UIWidgetBaseItem)

function TaskViewAccpetableAccordionItemChild:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.TaskName.text = data.task_name
end

function TaskViewAccpetableAccordionItemChild:Click()
    if self.Toggle then
		self.Toggle:Trigger()
	end
end