HelpOthersView = HelpOthersView or DeclareView("HelpOthersView", "serveractivity/help_others_view", Mod.PlanActivity.HelpOthers)
VIEW_DECLARE_MASK(HelpOthersView, ViewMask.BgBlockClose)

function HelpOthersView:LoadCallback()
    UH.SetText(self.TitleText, ActivityRandData.GetName(ACTIVITY_TYPE.RAND_HELP_OTHERS))
    -- self.Timer:CreateActTime(ACTIVITY_TYPE.RAND_HELP_OTHERS, TimeType.Type_Special_4)

    local end_timestamp = ServerActivityData.Instance:GetHelpOthersNextTime() or 0
    if end_timestamp == nil then
        self.Timer:CloseCountDownTime()
        self.Timer:SetTime(Language.Activity.NotOpenTip)
    else
        self.Timer:StampTime(end_timestamp, TimeType.Type_Special_4, nil, Language.Activity.NotOpenTip)
    end

    local show_cfg = ServerActivityData.Instance:GetHelpOthersCurTaskPrefix()
    if show_cfg ~= nil then
        UH.SetText(self.TaskName, show_cfg.story_title)
        UH.SetText(self.TaskDesc, show_cfg.desc)
        self.ItemList:SetData(DataHelper.FormatItemList(show_cfg.show_item))
    end
    local task_info = TaskData.Instance:GetTaskInfoByTaskId(CUSTOM_OBJID.HELP_OTHERS)
    self.FinishObj:SetActive(task_info == nil)
    self.BtnGo:SetActive(task_info ~= nil)
end

function HelpOthersView:OnClickClose()
    ViewMgr:CloseView(HelpOthersView)
end

function HelpOthersView:OnClickGo()
	if ViewMgr:IsOpen(PlanActivityView) then
		ViewMgr:CloseView(PlanActivityView)
	end
    TaskData.Instance:GoTaskHelpOthers()
    ViewMgr:CloseView(HelpOthersView)
end
