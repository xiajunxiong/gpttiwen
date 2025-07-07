GodClothesBuyView = GodClothesBuyView or DeclareView("GodClothesBuyView", "god_clothes/god_clothes_buy")
VIEW_DECLARE_MASK(GodClothesBuyView, ViewMask.BgBlock)
function GodClothesBuyView:GodClothesBuyView()
    self.Currency:DefaultSet()
end

function GodClothesBuyView:LoadCallback(param_t)
    
end

function GodClothesBuyView:OnCloseClick()
    ViewMgr:CloseView(GodClothesBuyView)
end

----------------------------GodClothesBuyPanel----------------------------
GodClothesBuyPanel = GodClothesBuyPanel or DeclareMono("GodClothesBuyPanel", UIWFlushPanel)
function GodClothesBuyPanel:GodClothesBuyPanel()
    self.Data = GodClothesData.Instance
    self.language = Language.GodClothes
    self.data = self.Data.info_data
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end

function GodClothesBuyPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.now_page = 0
    self:FlushAll()
end

function GodClothesBuyPanel:FlushAll()
    self.shop_data = self:DealData()
    self:OnPageChange(0)
end

function GodClothesBuyPanel:DealData()
    local shop_data = self.Data:ShopData()
    local list = {}
    local index = 0
    local num = 3
    for i, v in ipairs(shop_data) do
        local t1, t2 = math.floor((i - 1) / 3), (i - 1) % 3
        if TableIsEmpty(list[t1]) then
            list[t1] = {}
        end
        list[t1][t2] = v
    end
    self.max_page = table.nums(list)
    return list 
end

function GodClothesBuyPanel:OnPageChange(value)
    self.now_page = self.now_page + value
    if self.now_page < 0 then
        self.now_page = 0
    end
    if self.now_page >= self.max_page then
        self.now_page = self.max_page - 1
    end
    self.ObjLeft:SetActive(self.now_page ~= 0)
    self.ObjRight:SetActive(self.now_page ~= (self.max_page - 1))
    self.List:SetData(self.shop_data[self.now_page])
end

function GodClothesBuyPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function GodClothesBuyPanel:OnBuyClick(data)
    local can_times = self.Data:CanBuyTimes(data)
    if can_times > 0 then
        if data.cfg_or == 1 then
            -- 记录当前购买道具，用于校验
            self.now_buy_item = data
            ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_GOD_CLOTHES, data.price / 10, ACTIVITY_TYPE.RAND_GOD_CLOTHES, GodClothesReqType.MoneyBuy, data.seq)
        else
            if MallCtrl.IsNotCurrency(data.price_type, data.price) then
                GodClothesCtrl:SendReq(GodClothesReqType.Buy, data.seq)
            end
        end
    else
        PublicPopupCtrl.Instance:Center(self.language.LimitTip[3])
    end
end

function GodClothesBuyPanel:SendBuy()
    if self.now_buy_item == nil then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_GOD_CLOTHES, self.now_buy_item.price)
    self.now_buy_item = nil
end

--------------GodClothesBuyItem--------------
GodClothesBuyItem = GodClothesBuyItem or DeclareMono("GodClothesBuyItem", UIWidgetBaseItem)
function GodClothesBuyItem:GodClothesBuyItem()
    self.Data = GodClothesData.Instance
    self.language = Language.GodClothes
end

function GodClothesBuyItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.New(data.reward_item)
    UH.SetText(self.TxtName, item:QuaName(true))
    UH.SetActGold(self.Icon, data.price_type, data.cfg_or)
    UH.SetText(self.TxtPrice, data.cfg_or == 1 and data.price / 10 or data.price)
    self.can_times = self.Data:CanBuyTimes(data)
    UH.SetText(self.TxtLimit, Format(self.language.LimitBuy[data.limit_type], self.can_times, self.Data:GetTotalTimes(data)))
    self.Cell:SetData(item)
    self.IHandler.Interactable = self.can_times > 0
    self.ObjEffect:SetActive(self.can_times > 0)
    UH.SetText(self.TxtValue, Format(self.language.Value, data.value))
end