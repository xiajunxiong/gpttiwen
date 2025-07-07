PeriodActivityPosterView = PeriodActivityPosterView or DeclareMono("PeriodActivityPosterView", UIWFlushPanel)

function PeriodActivityPosterView:PeriodActivityPosterView()
    self.data = PeriodActivityData.Instance
end

function PeriodActivityPosterView:Awake()
    UIWFlushPanel.Awake(self)
    UH.SetListVisible(self.InfoList,self.data:GetPeriodType())
end


PeriodActivityPosterInfo = PeriodActivityPosterInfo or DeclareMono("PeriodActivityPosterInfo")
function PeriodActivityPosterInfo:PeriodActivityPosterInfo()
    self.data = PeriodActivityData.Instance
end

function PeriodActivityPosterInfo:Awake()
    UIWFlushPanel.Awake(self)
    local begin_time,end_time = self.data:GetActivityTime()
    local begin_month,end_month = os.date("%m",begin_time),os.date("%m",end_time)
    local open_day,end_day = os.date("%d",begin_time),os.date("%d",end_time-1)
    UH.SetText(self.Time,Format(Language.PeriodActivity.PosterFormat,begin_month,open_day,end_month,end_day))
end

function PeriodActivityPosterInfo:OnClickEnter()
    ViewMgr:OpenViewByKey(Mod.PeriodActivity.Main)
    ViewMgr:CloseView(PosterView)
end

function PeriodActivityPosterInfo:OnClickClose()
    ViewMgr:CloseView(PosterView)
end