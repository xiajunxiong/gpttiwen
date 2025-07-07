ChiefRankView = ChiefRankView or DeclareView("ChiefRankView", "chief/ChiefRankView")

function ChiefRankView:ChiefRankView()
    self.data = ChiefElectionData.Instance
    self.TipsTime:SetCallBack(function()ViewMgr:CloseView(ChiefRankView)end)
    self.ConfirmTime:SetCallBack(function()ViewMgr:CloseView(ChiefRankView)end)
end

function ChiefRankView:OnClickClose()
    ViewMgr:CloseView(ChiefRankView)
end

function ChiefRankView:LoadCallback() 
    self.RinkList:SetData(self.data:GetRankList())
    local rank_num,rank_info = self.data:GetRoleRank()
    self.Grid:SetData(self.data:GetParticipationRewardData())
    self:FlushRoleView(rank_num)
    self:FlushTimeView(rank_info)
end

function ChiefRankView:FlushRoleView(rank)
    local role_info = self.data:GetPrimaryRoleInfo()
    UH.SetText(self.RankNum,Format(Language.ChiefElection.RankTips[1],rank or Language.Battlefield.NotHasRankTip,role_info.socre))
    local total_times = role_info.total_times == 0 and 1 or role_info.total_times
    local win_rate = math.floor((role_info.win_times) / total_times * 100)
    UH.SetText(self.WinNum,Format(Language.ChiefElection.RankTips[2],win_rate,role_info.win_times,role_info.total_times))
end

function ChiefRankView:FlushTimeView(rank_info)
    if rank_info == nil then
        return
    end
    local stamp_time = self.data:GetNextMatchStateTime()
    if rank_info.rank <= 16 then
        self.TipsTime:StampTime(stamp_time,TimeType.Type_Special_2)
    else
        self.ConfirmTime:StampTime(stamp_time,TimeType.Type_Special_2,Language.ChiefElection.RankTips[3])
    end
    self.Tips:SetActive(rank_info.rank <= 16)
end

----------------------------ChiefRankItem----------------------------
ChiefRankItem = ChiefRankItem or DeclareMono("ChiefRankItem", UIWidgetBaseItem)

function ChiefRankItem:SetData(data)
    UH.SetText(self.Rank,data.rank)
    UH.SetText(self.Name,data.role_name)
    UH.SetText(self.Score,data.socre)
    local win_rate = math.floor((data.win_times) / data.total_times * 100)
    if data.total_times == 0 then
        UH.SetText(self.WinNum,"--")
    else
        UH.SetText(self.WinNum,Format("%s%%(%s/%s)",win_rate,data.win_times,data.total_times))
    end
    self.Promoted:SetActive(data.rank <= 16)
end