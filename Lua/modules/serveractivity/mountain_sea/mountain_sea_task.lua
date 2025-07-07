MountainSeaTask = MountainSeaTask or DeclareView("MountainSeaTask", "serveractivity/mountain_sea/mountain_sea_task", Mod.MountainSea.Task)
function MountainSeaTask:MountainSeaTask()
    
end
function MountainSeaTask:LoadCallback()
    
end
function MountainSeaTask:OnFlush()
    UH.SetText(self.Title, ActivityRandData.GetName(ACTIVITY_TYPE.RAND_MOUNTAIN_SEA_TASK))
    -- self.Timer:CreateActTime(ACTIVITY_TYPE.RAND_HELP_OTHERS, TimeType.Type_Special_4)

    local end_timestamp = ServerActivityData.Instance:GetShanHaiNextTime() or 0
    if end_timestamp == nil then
        self.Timer:CloseCountDownTime()
        self.Timer:SetTime(Language.Activity.NotOpenTip)
    else
        self.Timer:StampTime(end_timestamp, TimeType.Type_Special_4, Language.MountainSeaTask.ActTime, Language.Activity.NotOpenTip)
    end

    local show_cfg = ServerActivityData.Instance:GetShanHaiCurTaskPrefix()
    if show_cfg ~= nil then
        -- UH.SetText(self.TaskName, show_cfg.story_title)
        UH.SetText(self.TaskDesc, show_cfg.desc)
        self.ItemList:SetData(DataHelper.FormatItemList(show_cfg.show_item))
    end
    local task_info = TaskData.Instance:GetTaskInfoByTaskId(CUSTOM_OBJID.SHAN_HAI_ORIGIN)
    -- self.FinishObj:SetActive(task_info == nil)
    self.BtnGo:SetActive(task_info ~= nil)
end
function MountainSeaTask:CloseCallback()
    
end
function MountainSeaTask:OnClickGoTask()
    if ViewMgr:IsOpen(MountainSeaTotalView) then
		ViewMgr:CloseView(MountainSeaTotalView)
	end
    TaskData.Instance:GoTaskShanHai()
    ViewMgr:CloseView(MountainSeaTask)
end
function MountainSeaTask:OnClickClose()
    ViewMgr:CloseView(MountainSeaTask)
end