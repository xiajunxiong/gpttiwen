RewardFundExchangeView = RewardFundExchangeView or DeclareMono("RewardFundExchangeView", UIWFlushPanel)

function RewardFundExchangeView:RewardFundExchangeView()
    self.data = RewardData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.OnFlush,  init_call = true},
        {data = PartnerData.Instance.partner_pact_items, func = self.OnFlush,init_call = false},
    }
    RewardData.IsHasFundExchangeNum = true
end

function RewardFundExchangeView:OnFlush()
    local list = self.data:GetFundExchangeData()
    self.List:SetData(list)
    self.FollowTip:SetActive(#list == 0)
end

function RewardFundExchangeView:OnClickDetail()
    ViewMgr:OpenView(RewardFundExchangeDetailView)
end

----------------------------RewardFundExchangeItem----------------------------
RewardFundExchangeItem = RewardFundExchangeItem or DeclareMono("RewardFundExchangeItem", UIWidgetBaseItem)

function RewardFundExchangeItem:SetData(data)
    self.Call:SetData(Item.Init(data.item_id))
    self.Call:SetDataByItemIdWithNeed(data.item_id,data.num)
    self.Target:SetData(Item.New({item_id = CommonItemId.ImmortalCoin}))
    UH.SetText(self.Name,Item.GetName(data.item_id))
    UH.SetText(self.TargetName,Format(Language.Reward.ImmortalCoin,data.price))
    local num = Item.GetNum(data.item_id)
    self.Effect:SetActive(num >= data.num)
    UH.SetInter(self.BtnInter,num >= data.num)
    UIWidgetBaseItem.SetData(self, data)
end

function RewardFundExchangeItem:OnClickExchange()
    PublicPopupCtrl.Instance:AlertTip(Format(Language.Reward.FundExchangeTip,Item.GetName(self.data.item_id),self.data.price),function()
        RewardCtrl.Instance:SendFundExchangeReq(self.data.index)
    end,DTTodayNotRemind.RewardFundExchangeChek)
end