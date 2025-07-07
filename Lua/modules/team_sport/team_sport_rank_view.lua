TeamSportRankView = TeamSportRankView or DeclareMono("TeamSportRankView",UIWFlushPanel)

function TeamSportRankView:TeamSportRankView()
    self.data_cares = {
        {data = TeamSportData.Instance.rank_data,func = self.FlushRank,keys = {"is_change"}},
        {data = TeamSportData.Instance.score_data,func = self.FlushInfo,keys = {"is_change"}},
    }
end

function TeamSportRankView:FlushInfo()
    self.rank_txt.text = TeamSportData.Instance.score_data.data.rank
    local score = TeamSportData.Instance:GetScoreValue()
    if score < 0 then score = 0 end
    self.score_txt.text = score
end

function TeamSportRankView:FlushRank()
    self.data = TeamSportData.Instance.rank_data.data
    if self.data[1] == nil then
        self.tipsoj:SetActive(true)
    else
        self.listoj:SetActive(true)
        for k, v in pairs(self.data)do
            v.rank = k
        end
        self.rank_list:SetData(self.data)
    end
end

TeamSportRankItem = TeamSportRankItem or DeclareMono("TeamSportRankItem",UIWidgetBaseItem)
--reward_list======UIWidgetList[TeamSportRankRewardItem]

function TeamSportRankItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.name.text = data.name
    if data.score < 0 then data.score = 0 end
    self.score.text = data.score
    self.rank_normal.text = data.rank
    self.win_rate.text = string.format(Language.TeamSport.Rank2,data.win_rate) 
    self:ShowOj(data.rank)
    local item_data = self:GetRewardItems(data.rank,data.level)
    self.ContentSize.enabled = #item_data > 3
    self.reward_list:SetData(item_data)
end

function TeamSportRankItem:GetRewardItems(rank,level)
    local reward_list = TeamSportData.Instance:GetRankReward(rank,level) or {}
    return reward_list
end


function TeamSportRankItem:ShowOj(rank)
    for i=1,self.itemOj_array:Length() do
        self.itemOj_array[i]:SetActive(false)
    end
    if rank <=3 and rank >= 1 then
        self.itemOj_array[rank]:SetActive(true)
    else
        self.itemOj_array[4]:SetActive(true)
        self.itemOj_array[5]:SetActive(true)
    end
end

TeamSportRankRewardItem = TeamSportRankRewardItem or DeclareMono("TeamSportRankRewardItem",UIWidgetBaseItem)
function TeamSportRankRewardItem:SetData(data)
    self.cell:SetData(data)
end