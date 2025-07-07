TombRaiderRankView = TombRaiderRankView or DeclareView("TombRaiderRankView", "tomb_raider/tomb_raider_rank")
function TombRaiderRankView:TombRaiderRankView() end
function TombRaiderRankView:OnClickClose()
    ViewMgr:CloseView(TombRaiderRankView)
end
--===========================TombRaiderRankPanel===========================
TombRaiderRankPanel = TombRaiderRankPanel or DeclareMono("TombRaiderRankPanel", UIWFlushPanel)

function TombRaiderRankPanel:TombRaiderRankPanel()
    self.data = TombRaiderData.Instance
   
    local rank_info = self.data:GetRankInfo()
    self.RankList:SetData(rank_info.rank_list or {})

    self.empty:SetActive(rank_info.rank_list == nil or #rank_info.rank_list == 0)

    if rank_info.my_pos and rank_info.my_pos > 0 then 
        self.MyCell:SetData(rank_info.rank_list[rank_info.my_pos])
    else 
        self.MyCell:SetData({index = 0})
    end 
end

--============================TombRankItem=========================================
TombRankItem = TombRankItem or DeclareMono("TombRankItem", UIWidgetBaseItem)
function TombRankItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    if data.index == 0 then 
        UH.SetText(self.Name,RoleData.Instance:GetBaseData().name)
        UH.SetText(self.Feats,0)
        self.RankNum.gameObject:SetActive(false)
        self.RankShow.gameObject:SetActive(false)
    else 
        UH.SetText(self.Name,data.role_name)
        UH.SetText(self.Feats,data.rank_value)
        UH.SetText(self.RankNum,data.index)
        if TombRaiderConfig.RankSp[data.index] then 
            UH.SpriteName(self.RankShow, TombRaiderConfig.RankSp[data.index])
        end 
    
        local flag = data.index < 4
        self.RankNum.gameObject:SetActive(not flag)
        self.RankShow.gameObject:SetActive(flag)
    
        local reward_cfg = TombRaiderData.Instance:GetRewardByRank(data.index)
        local list = {}
        for k,v in pairs(reward_cfg ~= nil and reward_cfg.item_list or {}) do 
            local t = Item.Create(v)
            table.insert( list, t )
        end 
        self.Reward:SetData(list)
    end  

    if self.is_self then 
        self.NoRank:SetActive(data.index == 0)
    end 
end