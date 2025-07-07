DealDiscountView = DealDiscountView or DeclareView("DealDiscountView","serveractivity/deal_discount_view",Mod.Splendid.DealDiscount)
VIEW_DECLARE_LEVEL(DealDiscountView,ViewLevel.L1)

function DealDiscountView:OnClickClose()
    ViewMgr:CloseView(DealDiscountView)
end

----------------------------DealDiscountPanel----------------------------
DealDiscountPanel = DealDiscountPanel or DeclareMono("DealDiscountPanel", UIWFlushPanel)

function DealDiscountPanel:DealDiscountPanel()
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.data.deal_discount_data, func = self.FlushGoldView},
        {data = self.data.deal_discount_data, func = self.FlushTradeView},
    }
end

function DealDiscountPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_DEAL_DISCOUNT,TimeType.Type_Time_0)
    local post,list = DataHelper.GetValueLength(tostring(self.data:GetDealDiscountRate()))
    for i=1,post do
        UH.SpriteName(self.Image[i],list[i])
    end
end

function DealDiscountPanel:FlushTradeView()
    local list = self.data:GetDealDiscountItemList()
    for i=1,self.CallList:Length() do
        self.CallList[i]:SetData(list[i] or {item_id = 0})
    end
end

function DealDiscountPanel:FlushGoldView()
    local info = self.data:GetDealDiscountInfo()
    self.BtnInter.Interactable = info.remain_calaimable_gold > 0
    self.RedPos:SetNum(self.BtnInter.Interactable and 1 or 0)
    local param_t = {
        {index = 1,num = info.total_buy_item_consume_gold},
        {index = 2,num = info.total_calaimable_gold},
        {index = 3,num = info.remain_calaimable_gold},
    }
    for i=1,self.ItemList:Length() do
        self.ItemList[i]:SetData(param_t[i])
    end
end

function DealDiscountPanel:OnClickEnter()
    ViewMgr:OpenViewByKey(Mod.Mall.Trade)
    ViewMgr:CloseView(DealDiscountView)
end

function DealDiscountPanel:OnClickReward()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_DEAL_DISCOUNT,1)
end

----------------------------DealDiscountCall----------------------------
DealDiscountCall = DealDiscountCall or DeclareMono("DealDiscountCall", UIWidgetBaseItem)

function DealDiscountCall:SetData(data)
    if data.item_id ~= 0 then
        local item = Item.Create(data)
        if item:IsPet() then
            self.Call:SetData(Pet.New(data.param))
        else
            self.Call:SetData(item)
        end
        if string.len(item:Name()) > 20 then
            UH.SetText(self.Text,item:QuaName())
        else
            UH.SetText(self.Name,item:QuaName())
        end
    else
        UH.SetText(self.Name,Language.DealDiscount.HotIteTip)
    end
    self.Call:SetObjActive(data.item_id ~= 0)
end

----------------------------DealDiscountItem----------------------------
DealDiscountItem = DealDiscountItem or DeclareMono("DealDiscountItem", UIWidgetBaseItem)

function DealDiscountItem:SetData(data)
    UH.SetText(self.Tips,Language.DealDiscount.Tips[data.index])
    UH.SetText(self.Num,data.num)
end

----------------------------DealDiscountTradePanel----------------------------
DealDiscountTradePanel = DealDiscountTradePanel or DeclareMono("DealDiscountTradePanel", UIWFlushPanel)

function DealDiscountTradePanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    if ActivityData.IsOpen(ACTIVITY_TYPE.RAND_DEAL_DISCOUNT) then
        local rate = ServerActivityData.Instance:GetDealDiscountRate()
        UH.SetText(self.DiscountTip,Format(Language.DealDiscount.RateTip,rate))
    end
end

function DealDiscountTradePanel:OnClickTreasure()
    ViewMgr:OpenView(DealDiscountView)
end