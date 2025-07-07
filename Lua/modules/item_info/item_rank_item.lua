
ItemRankItem = ItemRankItem or DeclareMono("ItemRankItem")

function ItemRankItem:SetRank(rank)
    self.RankShow.text = rank
end

function ItemRankItem:SetActive(is_active)
    self.ItemObj:SetActive(is_active)
end