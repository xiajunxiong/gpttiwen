PalacesChallengeRank = PalacesChallengeRank or DeclareView("PalacesChallengeRank", "god_areas_palaces/palaces_challenge_rank")

function PalacesChallengeRank:PalacesChallengeRank()
end

function PalacesChallengeRank:LoadCallback(param_t)
    
end

PalacesChallengeRankPanel = PalacesChallengeRankPanel or DeclareMono("PalacesChallengeRankPanel", UIWFlushPanel)
function PalacesChallengeRankPanel:PalacesChallengeRankPanel()
    self.data = PalacesData.Instance
end
function PalacesChallengeRankPanel:onFlush()
    self.boss_cfg = self.data:BossConfig()
    for i = 1, 4 do
        UH.SetText(self.Names[i], self.boss_cfg[i].button_name)
        UH.SetIcon(self.Icons[i], Config.npc_auto.npc_list[self.boss_cfg[i].npc_id].npc_icon)
    end
    self.Toggles[1].isOn = true
    self:OnClickTabbar(1)
    -- local data = self.data:GetBossReward(0)
    -- self.List:SetData(data)
    -- self.MyItem:SetData({})
end

function PalacesChallengeRankPanel:OnClickTabbar(value)
    local rank_type = value - 1
    UH.SetText(self.Desc2, Language.GodAreasActs.BattleName2[rank_type])
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
function PalacesChallengeRankPanel:OnClickClose()
    ViewMgr:CloseView(PalacesChallengeRank)
end


PalacesChallengeRankItem = PalacesChallengeRankItem or DeclareMono("PalacesChallengeRankItem", UIWidgetBaseItem)
function PalacesChallengeRankItem:PalacesChallengeRankItem()
    self.ruins_data = RuinsData.Instance
    self.rank_sp = {
        "DiYiMing", "DiErMing", "DiSanMing",
    }
end
function PalacesChallengeRankItem:SetData(data)
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
            UH.SetText(self.Name, Language.GodAreasActs.Space[data.server_id])--self.ruins_data:ServerName(data.server_id))
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
            UH.SetText(self.Name, Language.GodAreasActs.Space[GodAreasActsData.Instance:FactionId()])--LoginData.Instance:ServerName())--RoleData.Instance:GetRoleName())
            UH.SetText(self.Total, data.value)
            local value = self.ruins_data:GetBossRewardScore(data.boss_type, data.rank)
            UH.SetText(self.Reward, value)
        end
    end
end