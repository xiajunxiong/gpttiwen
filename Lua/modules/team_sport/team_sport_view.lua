TeamSportView = TeamSportView or DeclareView("TeamSportView","team_sport/team_sport",Mod.TeamSport.Main)

VIEW_DECLARE_LEVEL(TeamSportView,ViewLevel.L1)

function TeamSportView:TeamSportView()
    for i=2,4 do
        TeamSportCtrl.Instance:SendTeamFightMatchReq(i)
    end
end

function TeamSportView:LoadCallback(param)
    param = param or {}
    self.Board:SetData(self:GetType(),"")
    self.Board:SetTabbarVisible(true)
    self.Board.Tabbar:SetData({
        {name = Language.TeamSport.AccuSport,tab_objs = {self.TeamSport}},
        {name = Language.TeamSport.Rank,tab_objs = {self.TeamSportRank}},
    })
    self.Board.Tabbar:ClickItem(param.tab_index or 1)
    self.Board:SetEffectActive(UIWBoard1.Effect.FengLing,false)
    UH.SetText(self.TitleTxt,Language.TeamSport.TeamSport)
    self.Currency:DefaultSet() 
end