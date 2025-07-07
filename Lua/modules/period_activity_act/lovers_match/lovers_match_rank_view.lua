LoversMatchRankView = LoversMatchRankView or DeclareView("LoversMatchRankView", "period_activity_act/lovers_match/lovers_match_rank")
function LoversMatchRankView:LoversMatchRankView()
    LoversMatchCtrl.Instance:SendRankInfo()
end

function LoversMatchRankView:OnClickClose()
    ViewMgr:CloseView(LoversMatchRankView)
end

LoversMatchRankPanel = LoversMatchRankPanel or DeclareMono("LoversMatchRankPanel", UIWFlushPanel)
function LoversMatchRankPanel:LoversMatchRankPanel()
    self.data_cares = {
        {data = LoversMatchData.Instance.rank_info, func = self.FlushView}
    }
end

function LoversMatchRankPanel:Awake()
    UIWFlushPanel.Awake(self)
    local function func_delay()
        self.DownObj:SetActive(true)
    end
    Invoke(func_delay, 0.5)
end

function LoversMatchRankPanel:FlushView()
    local rank_list = LoversMatchData.Instance:GetRankList()
    self.RankList:SetData(rank_list)
    self.NotHasTip:SetActive(#rank_list == 0)

    local rank, config = DataHelper.SimpleCondIndex(rank_list, function(data)
        return data.uid == RoleData.Instance:GetRoleId()
    end)
    UH.SetText(self.MyRank, config ~= nil and config.show_rank or Language.LoversMatch.NoRank)
    if config ~= nil then
        UH.SetText(self.MyRank, Language.LoversMatch.MyRank .. config.show_rank)
        local item_list = LoversMatchData.Instance:GetRankReward(config.show_rank)
        if item_list then
            self.MyItemList:SetData(DataHelper.FormatItemList(item_list))
            self.MyItemList:SetObjActive(true)
            self.MyRewardObj:SetActive(true)
        else
            self.MyItemList:SetObjActive(false)
            self.MyRewardObj:SetActive(false)
        end
    else
        UH.SetText(self.MyRank, Language.LoversMatch.MyRank .. Language.LoversMatch.NoRank)
        self.MyItemList:SetObjActive(false)
        self.MyRewardObj:SetActive(false)
    end
end

LoversMatchRankItem = LoversMatchRankItem or DeclareMono("LoversMatchRankItem", UIWidgetBaseItem)
function LoversMatchRankItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local rank = data.show_rank
    local is_str = rank > 3 or rank == 0
    self.RinkNum:SetObjActive(is_str)
    self.ImgNum:SetObjActive(not is_str)
    if is_str then
        UH.SetText(self.RinkNum, rank)
    else
        UH.SpriteName(self.ImgNum,"RankNum"..rank)
    end
    UH.SetText(self.Name, data.name)
    UH.SetText(self.Score, data.score)
    UH.SetText(self.Time, Format("%02d\'%02d\'\'", math.floor(data.pass_time / 60), data.pass_time % 60))
    local item_list = LoversMatchData.Instance:GetRankReward(rank)
    if item_list then
        self.ItemList:SetData(DataHelper.FormatItemList(item_list))
    end
end
