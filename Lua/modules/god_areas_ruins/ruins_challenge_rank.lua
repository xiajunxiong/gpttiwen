RuinsChallengeRank = RuinsChallengeRank or DeclareView("RuinsChallengeRank", "god_areas_ruins/ruins_challenge_rank")

function RuinsChallengeRank:RuinsChallengeRank()
end

function RuinsChallengeRank:LoadCallback(param_t)
    
end

RuinsChallengeRankPanel = RuinsChallengeRankPanel or DeclareMono("RuinsChallengeRankPanel", UIWFlushPanel)
function RuinsChallengeRankPanel:RuinsChallengeRankPanel()
    self.data = RuinsData.Instance
end
function RuinsChallengeRankPanel:onFlush()
    self.boss_cfg = self.data:BossConfig()
    for i = 1, 3 do
        for j = 1, 2 do
            UH.SetText(self["ToggleTex"..i][j], self.boss_cfg[i].button_name)
        end
    end
    self:OnClickTabbar(1)
    -- local data = self.data:GetBossReward(0)
    -- self.List:SetData(data)
    -- self.MyItem:SetData({})
end

function RuinsChallengeRankPanel:OnClickTabbar(value)
    local rank_type = value - 1
    UH.SetText(self.Desc2, Language.GodAreasActs.BattleName[rank_type])
    local data = self.data:GetRankList(rank_type)
    if #data <= 0 then
        data = self.data:GetBossReward(rank_type)
        self.List:SetData(data)
    else
        self.List:SetData(data)
    end
    local my_rank = self.data:GetMyRank(rank_type)
    if my_rank then
        my_rank.boss_type = rank_type
        self.MyItem:SetData(my_rank)
    else
        self.MyItem:SetData({})
    end
    --根据type显示
    local config = self.data:GetBossConfig(rank_type)
    local is_die = self.data:BossIsDie(rank_type)
    if is_die then
        UH.SetText(self.Desc, string.format(Language.GodAreasActs.RankTip2, config.boss_name))
    else
        UH.SetText(self.Desc, string.format(Language.GodAreasActs.RankTip, config.boss_name))
    end
end
function RuinsChallengeRankPanel:OnClickClose()
    ViewMgr:CloseView(RuinsChallengeRank)
end


RuinsChallengeRankItem = RuinsChallengeRankItem or DeclareMono("RuinsChallengeRankItem", UIWidgetBaseItem)
function RuinsChallengeRankItem:RuinsChallengeRankItem()
    self.ruins_data = RuinsData.Instance
    self.rank_sp = {
        "DiYiMing", "DiErMing", "DiSanMing",
    }
end
function RuinsChallengeRankItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    --预览
    if data.reward_seq ~= nil then
        if data.min_rank_pos == data.max_rank_pos then
            UH.SpriteName(self.RankSp, self.rank_sp[data.min_rank_pos])
            UH.SetText(self.RankTex, "")
            self.RankSp:SetObjActive(true)
        else
            UH.SetText(self.RankTex, string.format("%s-%s", data.min_rank_pos, data.max_rank_pos))
            self.RankSp:SetObjActive(false)
        end
        UH.SetText(self.Name, Language.GodAreasActs.NoName)
        UH.SetText(self.Total, "--")
        UH.SetText(self.Reward, data.reward_score)
    else
        if self.IsMine == false then
            if data.rank < 4 then
                UH.SetText(self.RankTex, "")
                UH.SpriteName(self.RankSp, self.rank_sp[data.rank])
                self.RankSp:SetObjActive(true)
            else
                self.RankSp:SetObjActive(false)
                UH.SetText(self.RankTex, data.rank)
            end
            UH.SetText(self.Name, self.ruins_data:ServerName(data.server_id))
            UH.SetText(self.Total, data.p1)
            local value = self.ruins_data:GetBossRewardScore(data.boss_type, data.rank)
            UH.SetText(self.Reward, value)--data.reward_score)
        end
    end
    if self.IsMine == true then
        if next(data) == nil then
            self.RankSp:SetObjActive(false)
            UH.SetText(self.RankTex, Language.GodAreasActs.NoRank)
            UH.SetText(self.Name, RoleData.Instance:GetRoleName())
            UH.SetText(self.Total, "--")
            UH.SetText(self.Reward, "0")
        else
            self.RankSp:SetObjActive(false)
            if data.rank > 0 and data.rank < 4 then
                UH.SetText(self.RankTex, "")
                UH.SpriteName(self.RankSp, self.rank_sp[data.rank])
                self.RankSp:SetObjActive(true)
            else
                self.RankSp:SetObjActive(false)
                UH.SetText(self.RankTex, data.rank == 0 and Language.GodAreasActs.NoRank or data.rank)
            end
            LogDG(data.boss_type)
            UH.SetText(self.Name, LoginData.Instance:ServerName())--RoleData.Instance:GetRoleName())
            UH.SetText(self.Total, data.value)
            local value = self.ruins_data:GetBossRewardScore(data.boss_type, data.rank)
            UH.SetText(self.Reward, value)
        end
    end
end