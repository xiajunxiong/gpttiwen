SaleShopView = SaleShopView or DeclareView("SaleShopView","serveractivity/sale_shop_view",Mod.YunZe.SaleShop)

VIEW_DECLARE_LEVEL(SaleShopView, ViewLevel.L1)

function SaleShopView:SaleShopView()
    local style_type = ActShopBoard1.GetShowStyle(Config.sale_shop_activity_auto.activity_interface)
    self.Board:SetData(self:GetType(),ActivityRandData.GetName(ACTIVITY_TYPE.RAND_SALE_SHOP),style_type)
    self:FlushTopView(style_type)
    self.Currency:DefaultSet()   
end

function SaleShopView:FlushTopView(index)
    local is_special = index > 0 and index <= 3 
    if is_special then
        UH.LocalPos(self.LeftRect,ServeractivityConfig.SaleShop.TopPos[index][1])
        UH.LocalPos(self.RightRect,ServeractivityConfig.SaleShop.TopPos[index][2])
        self.RightLine.effectColor = ServeractivityConfig.SaleShop.Outline[index]
        self.LeftLine.effectColor = ServeractivityConfig.SaleShop.Outline[index]
        self.RightGradient.topColor = ServeractivityConfig.SaleShop.Gradient[index][1]
        self.RightGradient.buttomColor = ServeractivityConfig.SaleShop.Gradient[index][2]
        self.LeftGradient.topColor = ServeractivityConfig.SaleShop.Gradient[index][1]
        self.LeftGradient.buttomColor = ServeractivityConfig.SaleShop.Gradient[index][2]
    end
    UH.SpriteName(self.TopSp,"Tu"..index)
    self.LightEffect:SetActive(index == 0)
    self.LeftTxt:SetActive(is_special)
    self.RightTxt:SetActive(is_special)
    self.ArtImage:SetActive(index == 5 or index == 6)
end

function SaleShopView:CloseCallback()
    ServerActivityData.Instance.sale_shop_select = SmartData.New()
end

function SaleShopView:OnClickClose()
    ViewMgr:CloseView(SaleShopView)
end

SaleShopPanel = SaleShopPanel or DeclareMono("SaleShopPanel", UIWFlushPanel)

function SaleShopPanel:SaleShopPanel()
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.data.sale_shop_count, func = self.FlushShopView, init_call = false},
        {data = self.data.sale_shop_select, func = self.FlushShopView, init_call = false},
    }
    self.data:CheckSaleShopList()
end

function SaleShopPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.BtnList:SetDataList(Config.sale_shop_activity_auto.species_name)
    self.BtnList:SetCompleteCallBack(function()
        self.Decorate:SetParent(self.BtnList.gameObject)
    end)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_SALE_SHOP,TimeType.Type_Special_4)
end

function SaleShopPanel:FlushShopView()
    local data = self.data.sale_shop_select
    self.List:SetData(self.data:GetSaleShopPageData(data.type))
end 

----------------------------SaleShopItem----------------------------
SaleShopItem = SaleShopItem or DeclareMono("SaleShopItem", UIWidgetBaseItem)

function SaleShopItem:SetData(data)
    data.item = Item.New(data.reward_item)
    self.Call:SetData(data.item)
    UH.SetText(self.Name,data.item:QuaName(true))
    
    UH.SetText(self.Num,data.price == 0 and Language.OpenAct.LimitShop.Free or data.price)
    UH.SetGold(self.Gold,data.price_type)
    UH.SetText(self.OldNum,Language.Guild.PriceTips .. data.old_gold)
    self.BtnFree:SetActive(data.price == 0)

    data.buy_times = data.limit_type == 1 and data.buy_times or data.today_times 
    data.limit_times = data.buy_times - ServerActivityData.Instance:GetSaleShopTimes(data.item_seq)
    UH.SetText(self.Times,Format(Language.Shop.LimitTimesList[data.limit_type],data.limit_times))

    self.EnterInter.Interactable = data.limit_times > 0
    self.BtnFreeInter.Interactable = data.limit_times > 0
    self.RedPos:SetNum(data.limit_times > 0 and 1 or 0)
    self.Effect:SetActive(data.limit_times > 0)
    
    UIWidgetBaseItem.SetData(self, data)
end

function SaleShopItem:OnClickBuy()
    if self.data.price == 0 then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SALE_SHOP,1,self.data.type,self.data.item_seq,1)
        return
    end
    self.data.confirm_func = function(make_num)
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SALE_SHOP,1,self.data.type,self.data.item_seq,make_num)
    end
    ViewMgr:OpenView(BatchBuyView,self.data)
end

----------------------------SaleShopBtn----------------------------
SaleShopBtn = SaleShopBtn or DeclareMono("SaleShopBtn", UIWidgetBaseItem)

function SaleShopBtn:SetData(data)
    UH.SetText(self.Name,data.name)
    UH.SetText(self.SelName,data.name)
    UIWidgetBaseItem.SetData(self, data)
    if data.is_recommend == 1 then
        if self.ht == nil then
            self.ht = ServerActivityData.Instance.sale_shop_count:Care(BindTool.Bind(self.FlushRedPoint, self))
        end
    else
        self.RedPos:SetNum(0)
    end
end

function SaleShopBtn:FlushRedPoint()
    self.RedPos:SetNum(ServerActivityData.Instance:GetSaleShopRemindNum())
end

function SaleShopBtn:OnClickItem()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SALE_SHOP,0,self.data.type)
    ServerActivityData.Instance.sale_shop_select:Set(self.data)
end

function SaleShopBtn:Click()
	if self.ToggleClick then
		self.ToggleClick:Trigger()
	end
end

function SaleShopBtn:OnDestroy()
    UIWidgetBaseItem.OnDestroy(self)
    if self.ht ~= nil then
        ServerActivityData.Instance.sale_shop_count:Uncare(self.ht)
    end
end