TeamSportMenuView = TeamSportMenuView or DeclareMono("TeamSportMenuView",UIWFlushPanel)
--Rank====Text
--Score===Text

function TeamSportMenuView:TeamSportMenuView()
    self.data_cares = {
        {data = TeamSportData.Instance.score_data,func = self.FlushScore},
        {data = TeamSportData.Instance.match_data,func = self.FlushMatch},
    }
end

function TeamSportMenuView:Awake()
    UIWFlushPanel.Awake(self)
    TeamSportCtrl.Instance:SendTeamFightMatchReq(TeamSportCtrl.ReqType.FullInfo)
end

function TeamSportMenuView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.Time:CloseCountDownTime()
	self.Time:CreateActTime(ActType.TeamSport,TimeType.Type_Special_3)
    self:FlushMatch()
end

function TeamSportMenuView:FlushScore()
    local score = TeamSportData.Instance:GetScoreValue()
    local rank_num = TeamSportData.Instance.score_data.data.rank
    if score < 0 then score = 0 end
    self.Score.text = score
    self.Rank.text = string.format(Language.TeamSport.RankNum,rank_num)
end

function TeamSportMenuView:FlushMatch()
    if TeamSportData.Instance.match_data.is_match == 1 then
        self.match.text = Language.TeamSport.Matching
        self.match.color = COLORS.Red8
    else
        self.match.text = Language.TeamSport.NoMatch
        self.match.color = COLORS.Green10
   end
end

function TeamSportMenuView:OnDisable()
    self.Time:CloseCountDownTime()
end

function TeamSportMenuView:ClickExit()
    ViewMgr:OpenView(TeamSportPopupView)
end

function TeamSportMenuView:OnRankClick()
    ViewMgr:OpenView(TeamSportView, {tab_index = 2})
end