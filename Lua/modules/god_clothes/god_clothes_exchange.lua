GodClothesExchangeView = GodClothesExchangeView or DeclareView("GodClothesExchangeView", "god_clothes/god_clothes_exchange")
VIEW_DECLARE_MASK(GodClothesExchangeView, ViewMask.BgBlock)
function GodClothesExchangeView:GodClothesExchangeView()
    
end

function GodClothesExchangeView:LoadCallback(param_t)
    
end

function GodClothesExchangeView:OnCloseClick()
    ViewMgr:CloseView(GodClothesExchangeView)
end

----------------------------GodClothesExchangePanel----------------------------
GodClothesExchangePanel = GodClothesExchangePanel or DeclareMono("GodClothesExchangePanel", UIWFlushPanel)
function GodClothesExchangePanel:GodClothesExchangePanel()
    self.Data = GodClothesData.Instance
    self.language = Language.GodClothes
    self.data = self.Data.info_data
    self.item_id = self.Data:ExchangeItemId()
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
    }
    -- 刷新道具数量
    if self.item_listen == nil then
        self.item_listen = BagData.Instance:ListenItems(self.Data:ExchangeItem(), BindTool.Bind(self.FlushAll, self))
    end
end

function GodClothesExchangePanel:Awake()
    UIWFlushPanel.Awake(self)
    UH.SetIcon(self.Icon, Item.GetIconId(self.item_id), ICON_TYPE.ITEM)
    self:FlushAll()
end

function GodClothesExchangePanel:FlushAll()
    UH.SetText(self.TxtNum, Item.GetNum(self.item_id))
    self.List:SetData(self.Data:ExchangeData())
end

function GodClothesExchangePanel:OpenGetWay()
    ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = self.item_id})})
end

function GodClothesExchangePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    BagData.Instance:RemoveListenItems(self.item_listen)
end

--------------GodClothesExchangeItem--------------
GodClothesExchangeItem = GodClothesExchangeItem or DeclareMono("GodClothesExchangeItem", UIWidgetBaseItem)
function GodClothesExchangeItem:GodClothesExchangeItem()
    self.Data = GodClothesData.Instance
    self.language = Language.GodClothes
end

function GodClothesExchangeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.New(data.reward_item)
    UH.SetText(self.TxtName, item:QuaName(true))
    UH.SetIcon(self.Icon, Item.GetIconId(data.recharge_item), ICON_TYPE.ITEM)
    UH.SetText(self.TxtPrice, data.num)
    self.can_times = self.Data:CanExchangeTimes(data)
    UH.SetText(self.TxtLimit, Format(self.language.LimitExchange[data.limit_type], self.can_times))
    self.Cell:SetData(item)
    self.Remind:SetNum(self.Data:ExchangeRemind(data))
    self.IHandler.Interactable = self.can_times > 0
    self.ObjEffect:SetActive(self.can_times > 0)
end

function GodClothesExchangeItem:OnExchangeClick()
    if self.can_times > 0 then
        local item_id = self.Data:ExchangeItemId()
        local has = Item.GetNum(item_id)
        if has >= self.data.num then
            GodClothesCtrl.Instance:SendReq(GodClothesReqType.Exchange, self.data.seq)
        else
            PublicPopupCtrl.Instance:Center(self.language.LimitTip[1])
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = item_id})})
        end
    else
        PublicPopupCtrl.Instance:Center(self.language.LimitTip[2])
    end
end