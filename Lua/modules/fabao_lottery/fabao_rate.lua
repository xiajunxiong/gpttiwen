FabaoRate = FabaoRate or  DeclareView("FabaoRate", "fabao_lottery/fabao_rate")
function FabaoRate:FabaoRate()
    -- body
end
function FabaoRate:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Fabao.RateReward)
end
function FabaoRate:OnFlush()
    local data = FabaoLotteryData.Instance:ConsumeReward()
    self.List:SetData(data)
end
FabaoRateReardItem = FabaoRateReardItem or DeclareMono("FabaoRateReardItem", UIWidgetBaseItem)
function FabaoRateReardItem:FabaoRateReardItem()
    self.fabao_data = FabaoLotteryData.Instance
end
local sp_name = {
    [26412] = "_LocLanSeFaBao",
    [26413] = "_LocZiSeFaBao",
    [26414] = "_LocChengSeFaBao",
}
function FabaoRateReardItem:SetData(data)    
   UH.SpriteName(self.TitleSp, sp_name[data])
   local  item = Item.Init(data)
   UH.SetIcon(self.IconSp, item:IconId())
   local rate = self.fabao_data:UpPercent(data)
   UH.SetText(self.RateTex, string.format(Language.Fabao.UpRate, rate.probability_up))
   local list = self.fabao_data:RewardList(data)
   self.List:SetData(list)
end