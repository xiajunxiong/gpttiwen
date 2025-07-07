LunJianAdvertisingView = LunJianAdvertisingView or DeclareMono("LunJianAdvertisingView", UIWFlushPanel)

function LunJianAdvertisingView:Awake()
    UIWFlushPanel.Awake(self)
    local year = os.date("%Y")
    local month = os.date("%m")
    local month_max_day_count = os.date("%d",os.time({year=year,month=month+1,day=0}))
    local cur_season = LunJianData.Instance:CurSeason()
    UH.SetText(self.YearTxt,tostring(year))
    UH.SetText(self.CountTxt,string.format(Language.LunJian.Advertising.SeasonCount,DataHelper.GetDaXie(cur_season,false)))
    UH.SetText(self.TimeTxt,string.format(Language.LunJian.Advertising.Time,month,month,month_max_day_count))
    UH.SetText(self.BtnTxt,LunJianData.Instance:SeasonStatus() <= LunJianData.Status.Schedule and 
    Language.LunJian.Advertising.SeeDetails or Language.LunJian.Advertising.SeeSchedule)
end

function LunJianAdvertisingView:OnClickGetMore()
    if LunJianData.Instance:SeasonStatus() <= LunJianData.Status.Schedule then
        ViewMgr:OpenView(LunJianSignupView)
    else
        LunJianCtrl.OpenMainView(LunJianMainView.Tab.Schedule)
    end
    ViewMgr:CloseView(LunJianAdvertisingView)
end

function LunJianAdvertisingView:OnClickClose()
    ViewMgr:CloseView(PosterView)
end