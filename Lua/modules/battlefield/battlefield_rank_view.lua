----------------------------BattlefieldRankView----------------------------
BattlefieldRankView = BattlefieldRankView or DeclareView("BattlefieldRankView", "battlefield/battlefield_rank")

function BattlefieldRankView:LoadCallback(param_t)
    self.BtnList[param_t].isOn = true
end

function BattlefieldRankView:OnClickType(type)
    local rank_list = BattlefieldData.Instance:GetRankingList(type)
    self.List:SetData(rank_list)
    UH.SetText(self.Title,Language.Battlefield.RankList[type] or "")
    self.NotHasTip:SetActive(#rank_list == 0)
    local rank,score = self:GetIndex(rank_list,RoleData.Instance:GetRoleId())
    self:FlushRoleItemView(rank,score)
end

function BattlefieldRankView:GetIndex(list,role_id)
    for k,v in pairs(list or {}) do
        if v.role_id == role_id then
            return k,v.score
        end
    end
end

function BattlefieldRankView:FlushRoleItemView(rank,score)
    self.RoleItem:SetData({
        rank = rank or 0,
        score = score or 0,
        name = RoleData.Instance:GetRoleName(),
        prof = RoleData.Instance:GetRoleProf(),
    })
end

function BattlefieldRankView:OnClickClose()
    ViewMgr:CloseView(BattlefieldRankView)
end

----------------------------BattlefieldRankItem----------------------------
BattlefieldRankItem = BattlefieldRankItem or DeclareMono("BattlefieldRankItem", UIWidgetBaseItem)

function BattlefieldRankItem:SetData(data)
    if data.rank > 3 or data.rank == 0 then
        UH.SetText(self.RinkNum,data.rank == 0 and "--" or data.rank)
        self:FlushRinkView(data.rank % 2 == 0 and "WengZiDi1" or "WengZiDi2")
    else
        self:FlushRinkView("RankBg"..data.rank)
        UH.SpriteName(self.ImgNum,"RankNum"..data.rank)
    end
    self.RinkNum:SetObjActive(data.rank > 3 or data.rank == 0)
    UH.SetText(self.Name,data.name)
    UH.SpriteName(self.Prof,ProfSettingB[data.prof])
    if data.score ~= 0 then
        local config = BattlefieldData.Instance:GetCurRankConfig(data.score)
        UH.SetText(self.Integral,Format("%s %s",config.ranking_list,data.score))
    else
        UH.SetText(self.Integral,"暂无积分")
    end
end

function BattlefieldRankItem:FlushRinkView(name)
    if self.ImgRink then
        UH.SpriteName(self.ImgRink,name)
    end
end

----------------------------BattlefieldRankingView----------------------------
BattlefieldRankingView = BattlefieldRankingView or DeclareView("BattlefieldRankingView", "battlefield/battlefield_ranking")

function BattlefieldRankingView:BattlefieldRankingView()
    self.data = BattlefieldData.Instance
end

function BattlefieldRankingView:LoadCallback()
    UH.SetText(self.Help,Config.language_cfg_auto.textdesc[66].desc)
    self.ranking = self.data:GetRankingNum()
    if BattlefieldData.Instance:GetTopRace() == 0 then
        self.ranking = 0
    end
    self:FlushBtnView()
    self:FlushRankView()
    self:FlushRankingView()
    if BattlefieldData.Instance:GetTopRace() == 0 then
        self.Top:SetActive(false)
        self.Progress:SetProgress(0)
        self.Progress:SetText("0/0")
    end
end

function BattlefieldRankingView:OnClickNum(num)
    self.ranking = self.ranking + num
    self:FlushBtnView()
    self:FlushRankView()
end

function BattlefieldRankingView:FlushBtnView()
    self.BtnLeft:SetActive(self.ranking > 0)
    self.BtnRight:SetActive(self.ranking < 25)
end

function BattlefieldRankingView:FlushRankView()
    self.ranking = self.ranking >= 25 and 25 or self.ranking
    local cur_config = self.data:GetRankingConfig(self.ranking)
    UH.SpriteName(self.Rank,"Rank" .. cur_config.rank)
    local score = cur_config.score == 0 and self.data:GetOtherConfig().basics_score or cur_config.score
    local reward_config = self.data:GetRewardConfig(cur_config.ranking)
    local rank_name = ColorStr(reward_config.ranking_list,BattlefieldConfig.BattlefieldRankingColorShallow[cur_config.rank])
    UH.SetText(self.RankName,Format(Language.Battlefield.RankName,rank_name,score))
    self.Grid:SetData(reward_config.reward_list)

    if self.ItemList[cur_config.rank] then
        self.ItemList[cur_config.rank]:Trigger()
    end
end

function BattlefieldRankingView:FlushRankingView()
    local ranking = self.data:GetRankingNum()
    local cur_config = self.data:GetRankingConfig(ranking)
    local next_config = self.data:GetRankingConfig(cur_config.rank * 5) or cur_config

    local cur_score = self.data:GetRankScore()
    UH.SetText(self.Integral,Format(Language.Battlefield.MyIntegral,ColorStr(cur_score,COLORSTR.Red6)))
    UH.SetText(self.Ranking,Format(Language.Battlefield.MyRanking,BattlefieldData.QuaName(cur_config,true)))

    self.Progress:SetProgress(cur_score / next_config.score)
    self.Progress:SetText(Format("%s/%s",cur_score,next_config.score))

    for i=1,self.ItemList:Length() do
        self.ItemList[i]:SetData(self.data:GetRankingConfig((i - 1) * 5))
    end
end

function BattlefieldRankingView:OnClickItem(rank)
    self.ranking = rank
    self:FlushBtnView()
    self:FlushRankView()
end

function BattlefieldRankingView:OnClickClose()
    ViewMgr:CloseView(BattlefieldRankingView)
end

----------------------------BattlefieldRankingItem----------------------------
BattlefieldRankingItem = BattlefieldRankingItem or DeclareMono("BattlefieldRankingItem", UIWidgetBaseItem)

function BattlefieldRankingItem:SetData(data)
    data.score = data.score == 0 and BattlefieldData.Instance:GetOtherConfig().basics_score or data.score
    UH.SetText(self.Score,data.score)
    UIWidgetBaseItem.SetData(self, data)
end

function BattlefieldRankingItem:Trigger()
	self.Toggle.isOn = true
end