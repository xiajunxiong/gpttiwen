--情缘小铺
LoversShopView = LoversShopView or DeclareView("LoversShopView", "period_activity_act/lovers_shop/lovers_shop", Mod.PeriodActivity.LoversShop)
function LoversShopView:OnClickClose()
    ViewMgr:CloseView(LoversShopView)
end

LoversShopPanel = LoversShopPanel or DeclareMono("LoversShopPanel", UIWFlushPanel)
function LoversShopPanel:LoversShopPanel()
    self.data_cares = {
        {data = LoversShopData.Instance.act_info, func = self.FlushView},
        {data = BagData.Instance.item_grid, func = self.FlushView, init_call = false},
    }
end

function LoversShopPanel:Awake()
    UIWFlushPanel.Awake(self)
    local timestamp = PeriodActivityData.Instance:GetActivityEndTime()
    self.ActTimer:StampTime(timestamp, TimeType.Type_Special_9)
end

function LoversShopPanel:FlushView()
    self.ItemList:SetData(LoversShopData.Instance:GetShowList())

    local item_id = LoversShopData.Instance:GetCoinItemId()
    UH.SetText(self.ItemNum, Item.GetNum(item_id))
    UH.SetIcon(self.ItemIcon, Item.GetIconId(item_id), ICON_TYPE.ITEM)
end

function LoversShopPanel:OnClickStuff()
    MainOtherCtrl.Instance:GetWayViewById(LoversShopData.Instance:GetCoinItemId())
    -- CellClicks.BagGridNoButton({GetData = function()
    --     return Item.Create({item_id = LoversShopData.Instance:GetCoinItemId()})
    -- end})
end

LoversShopItem = LoversShopItem or DeclareMono("LoversShopItem", UIWidgetBaseItem)
function LoversShopItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Price, data.price)
    local item = Item.Create({item_id = data.item_id, num = data.num, is_bind = data.is_bind})
    self.ItemCell:SetData(item)
    UH.SetText(self.ItemName, item:Name())

    local item_id = LoversShopData.Instance:GetCoinItemId()
    UH.SetIcon(self.ItemIcon, Item.GetIconId(item_id), ICON_TYPE.ITEM)
end

function LoversShopItem:OnClickBuy()
    if not MallCtrl.IsNotCurrency(-1, self.data.price, LoversShopData.Instance:GetCoinItemId()) then
        return
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
    LoversShopCtrl.Instance:SendBuyItem(self.data.seq)
end
