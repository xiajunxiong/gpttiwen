LoversMatchMaskView = LoversMatchMaskView or DeclareView("LoversMatchMaskView", "period_activity_act/lovers_match/lovers_match_mask")
VIEW_DECLARE_MASK(LoversMatchMaskView, ViewMask.None)

LoversMatchMaskPanel = LoversMatchMaskPanel or DeclareMono("LoversMatchMaskPanel", UIWFlushPanel)
function LoversMatchMaskPanel:LoversMatchMaskPanel()
    self.data_cares = {
        {data = LoversMatchData.Instance.fb_info, func = self.BeginTime, keys = {"begin_count_time"}, init_call = false}
    }
end

function LoversMatchMaskPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.Count:SetActive(false)
end

function LoversMatchMaskPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.count_timer)
    self.count_timer = nil
end

function LoversMatchMaskPanel:BeginTime()
    if self.count_timer ~= nil then
        return
    end
    self.Count:SetActive(true)
    self.play_time = 3
    local func_update = BindTool.Bind1(self.OnCountDownUpdate, self)
    local func_finish = BindTool.Bind1(self.OnCountDownFinish, self)
    self.count_timer = TimeHelper:AddCountDownTT(func_update, func_finish, self.play_time, 1, true)
end

function LoversMatchMaskPanel:OnCountDownUpdate()
    UH.SpriteName(self.Image, "DaoJiShiShuZi" .. self.play_time)
    self.play_time = self.play_time - 1
end

function LoversMatchMaskPanel:OnCountDownFinish()
    ViewMgr:CloseView(LoversMatchMaskView)
    LoversMatchData.Instance.fb_info:Notify()
end

LoversMatchTopView = LoversMatchTopView or DeclareView("LoversMatchTopView", "period_activity_act/lovers_match/lovers_match_top")
VIEW_DECLARE_MASK(LoversMatchTopView, ViewMask.None)

function LoversMatchTopView:LoadCallback()
    self.play_time = 10
    local func_update = BindTool.Bind1(self.OnTimeUpdate, self)
    local func_finish = BindTool.Bind1(self.OnTimeFinish, self)
    self.count_timer = TimeHelper:AddCountDownTT(func_update, func_finish, self.play_time - 3, 1, true)
end

function LoversMatchTopView:CloseCallback()
    TimeHelper:CancelTimer(self.count_timer)
    self.count_timer = nil
end

function LoversMatchTopView:OnTimeUpdate()
    if self.play_time == 3 then
        return
    end
    UH.SetText(self.TimeText, Format(Language.LoversMatch.BeginCountDown, self.play_time))
    self.play_time = self.play_time - 1
end
function LoversMatchTopView:OnTimeFinish()
    LoversMatchData.Instance.fb_info:SetDirty("begin_count_time")
    ViewMgr:CloseView(PeriodActivityHelpView)
    ViewMgr:CloseView(LoversMatchTopView)
end
