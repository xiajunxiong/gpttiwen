
AutoCommandState = {
    None = 0,
    Task = 1,
    Gather = 2,
    Escort = 3,
}

FILE_REQ("modules/auto_command/commands/auto_command")
FILE_REQ("modules/auto_command/commands/auto_command_task")
FILE_REQ("modules/auto_command/commands/auto_command_gather")
FILE_REQ("modules/auto_command/commands/auto_command_escort")

AutoInvokerCtrl = AutoInvokerCtrl or BaseClass(BaseCtrl)
function AutoInvokerCtrl:__init()
    if AutoInvokerCtrl.Instance ~= nil then
        Debuger.LogError("[AutoInvokerCtrl] attempt to create singleton twice!")
        return
    end
    AutoInvokerCtrl.Instance = self

    self:AutoInvokerCtrl()
    self.timer_dt = nil
end

function AutoInvokerCtrl:AutoInvokerCtrl()
    self.auto_state = SmartData.New({state = AutoCommandState.None})
    self:ResetInvoker()
end

function AutoInvokerCtrl:ResetInvoker()
    self.command_list = {}
    self.total_times = 1
    self.cur_time = 1
    self.cur_index = 1
    self.auto_state.state = AutoCommandState.None
    SceneLogic.Instance:ClearMoveCache()--没有指令才清楚 其他情况只是停下来
    BehaviorData.Instance.auto_behavior = 0
end

function AutoInvokerCtrl:OnLoadedSceneIdChange()
    if self:CheckScene() then return end
    TimeHelper:CancelTimer(self.timer_dt)
    self.timer_dt = TimeHelper:AddDelayTimer(function ()
        if self.command_list[self.cur_index] and not self.command_list[self.cur_index].switch then
            self.auto_state.state = self.command_list[self.cur_index]:Execute()
        end
    end, 1)
end

function AutoInvokerCtrl:CheckScene()
    local value = false
    if 0 == SceneMgr:LoadedSceneId() then value = true end
    if MainViewData.Instance:IsBattle() then value = true end
    if ViewMgr:IsOpen(NpcDialogView) then value = true end
    return value
end

function AutoInvokerCtrl:CommandFinish()
    self.cur_index = self.cur_index + 1
    if self.cur_index > #self.command_list then
        self.cur_time = self.cur_time + 1    
        self.cur_index = 1
        if 0 == self.total_times or self.total_times >= self.cur_time then
            self.auto_state.state = self.command_list[self.cur_index]:Execute(BindTool.Bind(self.CommandFinish, self))
        else
            self:ClearCommands()
        end
    else
        self.auto_state.state = self.command_list[self.cur_index]:Execute(BindTool.Bind(self.CommandFinish, self))
    end
end

function AutoInvokerCtrl:AutoState()
    return self.auto_state.state
end

-- 注意 如果total_times不等于0(无限) 那么在具体自动行为之前应该清一下ClearCommands 等做成功了清的话失败就会死磕
function AutoInvokerCtrl:SetCommands(command_list, total_times)
    self:ClearCommands()
    self.command_list = command_list
    self.total_times = total_times
    self.scene_care_handle = SceneMgr:CareLoadedSceneIdChange(BindTool.Bind(self.OnLoadedSceneIdChange,self))
    self.npc_dialog_close_handle = EventSys:Bind(GlobalEventKeys.CloseDialogView, BindTool.Bind(self.OnLoadedSceneIdChange,self))
    self.cur_time = 1
    self.cur_index = 1
    if self:CheckScene() then return end
    self.auto_state.state = self.command_list[self.cur_index]:Execute(BindTool.Bind(self.CommandFinish, self))
end

function AutoInvokerCtrl:ClearCommands()
    self:StopCommands()
    if nil ~= self.scene_care_handle then
        SceneMgr:UncareLoadedSceneIdChange(self.scene_care_handle)
        self.scene_care_handle = nil
    end
    if nil ~= self.npc_dialog_close_handle then
        EventSys:UnBind(self.npc_dialog_close_handle)
        self.scene_care_handle = self.npc_dialog_close_handle
    end
    self:ResetInvoker()
end

function AutoInvokerCtrl:StopCommands()
    TimeHelper:CancelTimer(self.timer_dt)
    if self.command_list[self.cur_index] and self.command_list[self.cur_index].switch then
        self.command_list[self.cur_index]:Stop()
    end
end

function AutoInvokerCtrl:ExecuteCommands()
    if table.nums(self.command_list) > 0 then
        self.auto_state.state = self.command_list[self.cur_index]:Execute(BindTool.Bind(self.CommandFinish, self))
    end
end