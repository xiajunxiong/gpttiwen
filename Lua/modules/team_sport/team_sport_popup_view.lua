
TeamSportPopupView = TeamSportPopupView or DeclareView("TeamSportPopupView","team_sport/team_sport_popup")
VIEW_DECLARE_MASK(TeamSportPopupView,ViewMask.BgBlockCloseNoAnim)

function TeamSportPopupView:LoadCallback()
    if TeamSportData.Instance.record_time.time ~= 0 then
        self.desc.text = Language.TeamSport.MatchingLeaveHint
    else
        self.desc.text = TeamCtrl.Instance:Data():IsLeader() and Language.TeamSport.PopupTips or Language.TeamSport.ExitTips
    end 
    self.confirm.text = Language.TeamSport.Confirm
    TimeHelper:CancelTimer(self.ht)
    local time = 20
    self.ht = TimeHelper:AddRunTimer(function()
        self.cancel.text = Language.TeamSport.Cancel.."("..time..")"
        time = time - 1
        if time == -1 then 
            ViewMgr:CloseView(TeamSportPopupView)
            self.ht = nil
        end
    end, 1,20)
end

function TeamSportPopupView:CancelClick()
    ViewMgr:CloseView(TeamSportPopupView)
end

function TeamSportPopupView:ConfirmClick()
    SceneCtrl.Instance:RequestLeaveFb()
    LoginCtrl.Instance:SendBackToOriginServer()
    ViewMgr:CloseView(TeamSportPopupView)
end

function TeamSportPopupView:CloseCallback()
    if self.ht ~= nil then
        TimeHelper:CancelTimer(self.ht)
        self.ht = nil
    end
end