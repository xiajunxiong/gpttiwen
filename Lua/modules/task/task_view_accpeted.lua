
TaskViewAccpeted = TaskViewAccpeted or DeclareMono("TaskViewAccpeted", UIWFlushPanel)

function TaskViewAccpeted:TaskViewAccpeted()
    self.data = TaskCtrl.Instance:Data()

    self.data_cares = {
		{data = self.data:GetTaskList(), func = self.FlushAccpetedList},
    }
    self.language = Language.Task
end

function TaskViewAccpeted:FlushAccpetedList()
    local data_accepted = self.data:GetTaskShowList(TaskConfig.ListType.accepted)
    self.AccpetedList:SetData(data_accepted, true, true)
    self.InfoObj:SetActive(nil ~= next(data_accepted))
end

function TaskViewAccpeted:OnEnable()
    UIWFlushPanel.OnEnable(self)
    local data_accepted = self.data:GetTaskShowList(TaskConfig.ListType.accepted)
    local parent = next(data_accepted)
    if nil ~= self.data.cur_sel_accpeted then
        self.cur_sel_task = self.data.cur_sel_accpeted
        self.data.cur_sel_accpeted = nil
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
        self.AccpetedList:ClickChild(self.cur_sel_task)
    end)
end

function TaskViewAccpeted:OnClickGiveUp()
    if nil == self.cur_sel_task then return end
    if TaskConfig.TaskType.task_chain == self.cur_sel_task.task_type then
        self.cur_sel_task:RequestGiveUp()
    else
        PublicPopupCtrl.Instance:DialogTips{
            content = string.format(self.language.GiveUpTips, self.language.TaskType[self.cur_sel_task.task_type], self.cur_sel_task.task_name),
            confirm = {
                func = function()
                    self.cur_sel_task:RequestGiveUp()
                    ViewMgr:CloseView(DialogTipsView)
                end
            }
        }
    end
end

function TaskViewAccpeted:OnClickGo()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    if nil == self.cur_sel_task then return end
    local mode = SceneData.Instance:GetMode()
    if not FightConfig.IS_IN_FIGHTFB[mode] and mode ~= SceneModeType.None then
		PublicPopupCtrl.Instance:Center(Language.Chat.BackActTips)
        return 
    end
    self.data:TraceTask(self.cur_sel_task.task_type, self.cur_sel_task.task_id)
	AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(self.cur_sel_task)}, 1)
    ViewMgr:CloseView(MainTaskView)
end

function TaskViewAccpeted:OnClickTaskItem(data)
    self.cur_sel_task = data
    self.data.cur_sel_task:Set(data or {})
    self:FlushBtnGiveUp()
end

function TaskViewAccpeted:FlushBtnGiveUp()
    local can_ab = nil ~= self.cur_sel_task and self.cur_sel_task.can_ab
    self.BtnGiveUpObj:SetActive(can_ab)
end

TaskViewAccpetedAccordionItemParent = DeclareMono("TaskViewAccpetedAccordionItemParent", UIWidgetBaseItem)

function TaskViewAccpetedAccordionItemParent:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.TypeName.text = data.name
end

function TaskViewAccpetedAccordionItemParent:ParentOpen()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.MenuOpen)
end

function TaskViewAccpetedAccordionItemParent:ParentClose()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.MenuClose)
end

TaskViewAccpetedAccordionItemChild = DeclareMono("TaskViewAccpetedAccordionItemChild", UIWidgetBaseItem)

function TaskViewAccpetedAccordionItemChild:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.TaskName.text = data.task_name
end

function TaskViewAccpetedAccordionItemChild:Click()
    if self.Toggle then
		self.Toggle:Trigger()
	end
end

function TaskViewAccpetedAccordionItemChild:ChildClick()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end