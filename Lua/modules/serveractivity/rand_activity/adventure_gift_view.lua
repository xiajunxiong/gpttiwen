AdventureGiftView = AdventureGiftView or DeclareView("AdventureGiftView", "serveractivity/adventure_gift",Mod.RandActivity.AdventureGift)
VIEW_DECLARE_MASK(AdventureGiftView,ViewMask.BgBlockClose)

--TimeMeter=======UITimeMeter
--HintTxt===========Text
--RewardList==========UIWSubstitute[UIWItemCell]
--RewardList=======Text
--Hint2Txt=========Text
--OldPriceTxt======Text
--Title============Text
function AdventureGiftView:AdventureGiftView()
    self.data = ServerActivityData.Instance
end

function AdventureGiftView:LoadCallback()
    UH.SetText(self.Title,ActivityRandData.GetName(ACTIVITY_TYPE.RAND_ADVENTURE_GIFT))
    UH.SetText(self.Hint2Txt,Language.AdventureGift.HintTxt2)
    local data = ServerActivityData.Instance.adventure_gift_data
    self.gift_info = self.data:GetAdventureGiftData(data.gift_seq)
    UH.SetText(self.HintTxt,Format(Language.AdventureGift.HintTxt,COLORSTR.Yellow25,self.gift_info.price))
    UH.SetText(self.OldPriceTxt,self.gift_info.price_old)
    self.TimeMeter:SetCallBack(function()ViewMgr:CloseView(AdventureGiftView)end)
    self.TimeMeter:StampTime(data.end_time,TimeType.Type_Time_0)
    local item_list = {}
    local gift_items = BagData.Instance:GetGiftItemList(self.gift_info.item.item_id)
    for _,v in pairs(gift_items.item_list) do
        table.insert(item_list,Item.Init(v.item_id,v.num,v.is_bind))
    end
    self.RewardList:SetData(item_list)
end

function AdventureGiftView:OnClickBuy()
    if not MallCtrl.IsNotCurrency(CurrencyType.Gold,self.gift_info.price) then
        return
    end
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ADVENTURE_GIFT,1)
    ViewMgr:CloseView(AdventureGiftView)
end

function AdventureGiftView:OnClickCLose()
    ViewMgr:CloseView(AdventureGiftView)
end