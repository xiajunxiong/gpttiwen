
RuinsRank = RuinsRank or DeclareView("RuinsRank", "god_areas_ruins/ruins_rank")

function RuinsRank:RuinsRank()
    self.Board:SetData(self:GetType(), Language.GodAreasActs.Rank)
end

function RuinsRank:LoadCallback(param_t)
    
end

RuinsRankPanel = RuinsRankPanel or DeclareMono("RuinsRankPanel", UIWFlushPanel)
function RuinsRankPanel:RuinsRankPanel()
    self.data = RuinsData.Instance
    self.s_rank = {3, 4}
end
function RuinsRankPanel:onFlush()
    self:OnClickRank(1)
end

function RuinsRankPanel:OnClickRank(value)
    local rank_type = self.s_rank[value]
    local data = self.data:GetRankList(rank_type)
    if #data > 0 then
        self.List:SetData(data)
    else
        data = self.data:GetRankingList(value - 1)
        self.List:SetData(data)
    end
    local my_rank = self.data:GetMyRank(rank_type)
    LogDG(my_rank)
    if next(my_rank) then
        my_rank.rank_type = rank_type
        self.MyItem:SetData(my_rank)
    else
        self.MyItem:SetData({})
    end
end

RuinsRankItem = RuinsRankItem or DeclareMono("RuinsRankItem", UIWidgetBaseItem)
function RuinsRankItem:RuinsRankItem()
    self.ruins_data = RuinsData.Instance
    self.rank_sp = {
        "DiYiMing", "DiErMing", "DiSanMing", "DiNMingDi"
    }
    self.c_rank = {[3] = 0, [4] = 1}
end
function RuinsRankItem:SetData(data)
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
        UH.SetText(self.JiFen, "--")
        -- UH.SetText(self.Reward, data.reward_score)
        local value = DataHelper.FormatItemList(data.ranking_reward_show)
        self.List:SetData(value)
    else
        if self.IsMine == false then
            if data.rank < 4 then
                UH.SetText(self.RankTex, "")
                UH.SpriteName(self.RankSp, self.rank_sp[data.rank])
                self.RankSp:SetObjActive(true)
            else
                self.RankSp:SetObjActive(false)
                UH.SetText(self.RankTex, data.rank)
                -- UH.SpriteName(self.RankSp, self.rank_sp[4])
            end
            if data.p1 then
                UH.SetText(self.Name, self.ruins_data:ServerName(data.server_id))
                UH.SetText(self.JiFen, data.p1)
            else
                UH.SetText(self.Name, data.role_name)
                UH.SetText(self.JiFen, data.sore)
            end 
            local value = self.ruins_data:GetRankingListReward(self.c_rank[data.rank_type], data.rank)
            self.List:SetData(DataHelper.FormatItemList(value))
        end
    end
    if self.IsMine == true then
        local c_rank_type = self.c_rank[data.rank_type]
        if next(data) == nil then
            self.RankSp:SetObjActive(false)
            UH.SetText(self.RankTex, Language.GodAreasActs.NoRank)
            UH.SetText(self.Name, c_rank_type == 0 and LoginData.Instance:ServerName() or RoleData.Instance:GetRoleName())
            UH.SetText(self.JiFen, "--")
        else
            UH.SetText(self.Name, c_rank_type == 0 and LoginData.Instance:ServerName() or RoleData.Instance:GetRoleName())
            if data.rank and data.rank > 0 and data.rank < 4 then
                UH.SetText(self.RankTex, "")
                UH.SpriteName(self.RankSp, self.rank_sp[data.rank])
                self.RankSp:SetObjActive(true)
            else
                self.RankSp:SetObjActive(false)
                UH.SetText(self.RankTex, data.rank == 0 and Language.GodAreasActs.NoRank or data.rank)
            end
            UH.SetText(self.JiFen, data.value)
            local value = self.ruins_data:GetRankingListReward(c_rank_type, data.rank)
            self.List:SetData(DataHelper.FormatItemList(value))
        end
    end
end