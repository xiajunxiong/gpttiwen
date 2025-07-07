
TeamSportTipsView = TeamSportTipsView or DeclareView("TeamSportTipsView","team_sport/team_sport_tips_view")

function TeamSportTipsView:TeamSportTipsView()
    self.TipString.text = Language.TeamSport.TipString
end

function TeamSportTipsView:Close()
    ViewMgr:CloseView(TeamSportTipsView)
end