TradeRoadRankPanel = TradeRoadRankPanel or DeclareMono("TradeRoadRankPanel", UIWFlushPanel)
function TradeRoadRankPanel:TradeRoadRankPanel()
    self.data_cares = {
        {data = TradeRoadData.Instance.rank_info, func = self.FlushView}
    }
    TradeRoadCtrl.Instance:SendRankListReq()
end

function TradeRoadRankPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushView()
end

function TradeRoadRankPanel:FlushView()
    local rank_list = TradeRoadData.Instance:GetRankList()
    self.EmptyObj:SetActive(#rank_list < 1)
    self.RankList:SetDataList(rank_list)
    self.OwnItem:FlushView()
end

TradeRoadRankItem = TradeRoadRankItem or DeclareMono("TradeRoadRankItem", UIWidgetBaseItem)
function TradeRoadRankItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.RankImg.gameObject:SetActive(data.index <= 3)
    self.RankNum:SetActive(data.index > 3)
    if data.index <= 3 then
        UH.SpriteName(self.RankImg, "RankNum" .. data.index)
    else
        UH.SetText(self.RankNumText, data.index)
    end

    self.Avatar:SetAvatar(data.avatar_type, data.avatar_id,data.avatar_quality)
    UH.SetText(self.Name, data.role_name)
    UH.SetText(self.Value, data.cumulation_coin_count)
    UH.SetText(self.Times, data.cumulation_trading_count)

    local reward_cfg = TradeRoadData.Instance:GetRankReward(data.index)
    self.ItemCell:SetData(Item.Create(reward_cfg.reward_item))
end

TradeRoadRankOwnItem = TradeRoadRankOwnItem or DeclareMono("TradeRoadRankOwnItem", UIWFlushPanel)
function TradeRoadRankOwnItem:FlushView()
    local rank = TradeRoadData.Instance:GetRank()

    self.RankOut:SetActive(rank == -1)
    if rank == -1 then
        self.RankImg.gameObject:SetActive(false)
        self.RankNum:SetActive(false)
    else
        self.RankImg.gameObject:SetActive(rank <= 3)
        self.RankNum:SetActive(rank > 3)
        if rank <= 3 then
            UH.SpriteName(self.RankImg, "RankNum" .. rank)
        else
            UH.SetText(self.RankNumText, rank)
        end
    end

    self.Avatar:SetAvatar(RoleData.Instance:GetRoleAvatarType(),RoleData.Instance:GetRoleAvatarId(),RoleData.Instance:GetRoleAvatarQuality())
    UH.SetText(self.Name, RoleData.Instance:GetRoleName())
    UH.SetText(self.Value, TradeRoadData.Instance:GetCumulationMoneyCount())
    UH.SetText(self.Times, TradeRoadData.Instance:GetCumulationTradingCount())

    if TradeRoadData.Instance:GetCumulationMoneyCount() > 0 then
        local reward_cfg = TradeRoadData.Instance:GetRankReward(rank, true)
        self.ItemCell:SetData(Item.Create(reward_cfg.reward_item))
        self.ItemCell.gameObject:SetActive(true)
    else
        self.ItemCell.gameObject:SetActive(false)
    end
end
