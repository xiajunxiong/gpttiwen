WenCangBookStoreView = WenCangBookStoreView or DeclareView("WenCangBookStoreView", "period_activity_act/store/wen_cang_book_store", Mod.PeriodActivity.BookStore)
function WenCangBookStoreView:OnCloseClick()
    ViewMgr:CloseView(WenCangBookStoreView)
end



WenCangBookStorePanel = WenCangBookStorePanel or DeclareMono("WenCangBookStorePanel", UIWFlushPanel)
function WenCangBookStorePanel:WenCangBookStorePanel()
    self.data = PeriodActivityStoreData.Instance
    self.data_cares = {
        {data = self.data.wen_cang_shop_data, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance.item_grid, func = self.FlushItem, init_call = false}
    }
end

function WenCangBookStorePanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
    self:FlushItem()
    local timestamp = PeriodActivityData.Instance:GetActivityEndTime()
    self.Timer:StampTime(timestamp, TimeType.Type_Special_9, Language.PeriodActivity.ActTime[2])
end

function WenCangBookStorePanel:FlushPanel()
    local list_data = self.data:GetWenCangShopList()
    self.List:SetData(list_data)
end

function WenCangBookStorePanel:FlushItem()
    local item_id = self.data:GetWenCangItemId()
    local icon_id = Item.GetIconId(item_id)
    UH.SetIcon(self.CurrIcon, icon_id, ICON_TYPE.ITEM)
    UH.SetText(self.ItemNum, Item.GetNum(item_id))
end

function WenCangBookStorePanel:OnItemClick()
    local item_id = self.data:GetWenCangItemId()
    local info = {
        item_list = {Item.Create({item_id = item_id})}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
end


WenCangBookStoreItem = WenCangBookStoreItem or DeclareMono("WenCangBookStoreItem", UIWidgetBaseItem)
function WenCangBookStoreItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create(data.reward)
    UH.SetText(self.ItemName, item:Name())
    self.ItemCell:SetData(item)
    local curr_item_id = data.conver_item_id
    UH.SetIcon(self.CurrIcon, Item.GetIconId(curr_item_id))
    UH.SetText(self.PriceNum, data.price)
end

function WenCangBookStoreItem:OnBuyClick()
    if not MallCtrl.IsNotCurrency(-1, self.data.price, self.data.conver_item_id) then
        return
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
    PeriodActivityStoreCtrl.Instance:SendReq(PeriodActivityStoreCtrl.ReqType.TYPE_BUY, self.data.seq)
end