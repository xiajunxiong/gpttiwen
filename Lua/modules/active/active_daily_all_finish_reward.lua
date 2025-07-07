ActiveDailyAllFinishRewardView = ActiveDailyAllFinishRewardView or DeclareView("ActiveDailyAllFinishRewardView", "active/active_daily_all_finish_reward")
VIEW_DECLARE_MASK(ActiveDailyAllFinishRewardView, ViewMask.BgBlockClose)
function ActiveDailyAllFinishRewardView:ActiveDailyAllFinishRewardView()
    self.language = Language.ActiveDailyAllFinish
end

function ActiveDailyAllFinishRewardView:LoadCallback(param_t)
    self.List:SetData(DataHelper.FormatItemList(param_t.item_list))
    UH.SetText(self.TxtDesc, Format(self.language.Desc, param_t.task_num))
    UH.SetText(self.TxtExp, param_t.exp)
end

function ActiveDailyAllFinishRewardView:CloseCallback()

end

function ActiveDailyAllFinishRewardView:OnCloseClick()
    ViewMgr:CloseView(ActiveDailyAllFinishRewardView)
end