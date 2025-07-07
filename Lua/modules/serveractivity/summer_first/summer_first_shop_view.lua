SummerFirstShopView = SummerFirstShopView or DeclareView("SummerFirstShopView", "serveractivity/summer_first/summer_first_shop", Mod.SummerFirst.SummerFirstShop)
VIEW_DECLARE_LEVEL(SummerFirstShopView, ViewLevel.L1)
VIEW_DECLARE_MASK(SummerFirstShopView, ViewMask.BgBlock)
function SummerFirstShopView:SummerFirstShopView()
    self.Currency:SetCurrency(CurrencyType.CoinBind, CurrencyType.Gold)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_SUMMER_FIRST_SHOP, TimeType.Type_Time_3)
    SummerFirstShopData.Instance:SetData(
        Config.burning_summer_auto.summer_store, 
        Config.burning_summer_auto.other[1], 
        Config.burning_summer_auto.store_name, 
        ACTIVITY_TYPE.RAND_SUMMER_FIRST_SHOP,
        BUY_ITEM_TYPE.TYPE_SUMMER_STORE)
end

function SummerFirstShopView:OnCloseClick()
    ViewMgr:CloseView(SummerFirstShopView)
end

function SummerFirstShopView:OnCloseCallback()
    SummerFirstShopData.Instance:SetData()
end


----------------------------SummerFirstShopPanel----------------------------
SummerFirstShopPanel = SummerFirstShopPanel or DeclareMono("SummerFirstShopPanel", UIWFlushPanel)
function SummerFirstShopPanel:SummerFirstShopPanel()
    self.Data = SummerFirstShopData.Instance
    self.other = self.Data.other_config
    self.language = Language.SummerFirstShop
    self.data = self.Data:GetNowSmartData()
    self.shop_item = self.Data.shop_item
    self.data_cares = {
        {data = self.data, func = self.FlushShop, init_call = true},
        {data = self.Data.shop_item, func = self.FlushItem, init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end

function SummerFirstShopPanel:Awake()
    UIWFlushPanel.Awake(self)
    -- 打开界面后取消每日红点
	local key = PrefKeys.ActivityIsRemind(RoleData.Instance:GetRoleId(), self.Data.act_type)
    local t0 = TimeManager.FormatUnixTime2Date(os.time())
    UnityPlayerPrefs.SetString(key, Format(self.language.RemindValue, ActivityData.Instance:GetActivityStatusInfo(self.Data.act_type).param_1, t0.year, t0.month, t0.day))
    self.Data.red_remind.flag = not self.Data.red_remind.flag
end

function SummerFirstShopPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function SummerFirstShopPanel:FlushShop()
    self.Data:CalculatePhase()
    self.config = self.Data.show_config
    self.ShopTabList:SetData(self.config)
    self.ShopTabList:SetectItem(1, true)
    self.ShopTabList:GetItem(1):OnSelect(true)
    self:FlushItem()
end

function SummerFirstShopPanel:FlushItem()
    self.ShopItemList:SetData(self.shop_item.now_use.phase_tab)
end

function SummerFirstShopPanel:SendBuy()
    if self.Data.now_buy_item == nil then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(self.Data.buy_item_type, self.Data.now_buy_item.price)
    self.Data.now_buy_item = nil
end


----------------------------SummerFirstShopTab----------------------------
SummerFirstShopTab = SummerFirstShopTab or DeclareMono("SummerFirstShopTab", UIWidgetBaseItem)
function SummerFirstShopTab:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.gameObject.name = "Item_" .. data.index
    UH.SetText(self.TxtName, SummerFirstShopData.Instance:GetShopName(data.phase))
    UH.SpriteName(self.ImgSetIcon1, SummerFirstShopData.Instance:GetShopIconName(data.phase))
    UH.SpriteName(self.ImgSetIcon2, SummerFirstShopData.Instance:GetShopIconName(data.phase))
    self.IHandler.Interactable = data.state ~= SummerFirstShopState.NotOpen
    self.ObjTime:SetActive(data.state == SummerFirstShopState.NotOpen)
    if self.data.open_time then
        UH.SetText(self.TxtTime, Format(Language.SummerFirstShop.ShopNameTip, data.open_time.month, data.open_time.day))
    end
end

function SummerFirstShopTab:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        SummerFirstShopData.Instance:DealPhaseTab(self.data)
        SummerFirstShopData.Instance.shop_item.now_use = self.data
    end
end

function SummerFirstShopTab:OnClick()
    if self.data.state == SummerFirstShopState.NotOpen and self.data.open_time then
        PublicPopupCtrl.Instance:Center(Format(Language.SummerFirstShop.NeedDayUnLock, self.data.open_time.month, self.data.open_time.day))
    end
end


----------------------------SummerFirstShopItem----------------------------
SummerFirstShopItem = SummerFirstShopItem or DeclareMono("SummerFirstShopItem", UIWidgetBaseItem)
function SummerFirstShopItem:SummerFirstShopItem()
    self.Data = SummerFirstShopData.Instance
end

function SummerFirstShopItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.gameObject.name = "Item_" .. data.item_index
    self.Cell:SetData(Item.New(self.data.reward_item))
    UH.SetText(self.TxtName, Item.GetQuaName(self.data.reward_item.item_id, true))
    UH.SetText(self.TxtValue, Format(Language.SummerFirstShop.Value, self.data.value))
    UH.SetText(self.TxtPrice, self.data.price_type == 3 and self.data.price / 10 or self.data.price)
    UH.SetGold(self.PriceImg, self.data.price_type == 3 and 999 or self.data.price_type)

    local color = data.times > 0 and COLORSTR.Green6 or COLORSTR.Red8
    local str = Format(Language.SummerFirstShop.Limit[self.data.limit_type], color, self.data.times, self.data.limit_type == 1 and self.data.buy_times or self.data.today_times)
    UH.SetText(self.TxtNum, str)
    self.BtnBuy.Interactable = self.data.times ~= 0

    self.BtnBuy.gameObject:SetActive(not self.data.is_last)
    self.ObjBuyed:SetActive(self.data.times == 0 and not self.data.is_last)
    self.ObjDiscount:SetActive(self.data.reward_group ~= nil)
end

function SummerFirstShopItem:OnBuyClick()
    -- 防止手速快过协议
    if self.data.price_type == 3 and self.Data.now_buy_item then
        PublicPopupCtrl.Instance:Center(Language.SummerFirstShop.BuyError)
        return
    end
    -- 这里不同活动分开写，怕看乱参数
    -- 0是元宝，2是铜钱，3是直购
    if ACTIVITY_TYPE.RAND_SUMMER_FIRST_SHOP == self.Data.act_type then
		self:SummerFirstShopBuy()
	elseif ACTIVITY_TYPE.RAND_SUMMER_DISCOUNT_SHOP == self.Data.act_type then
		self:SummerDiscountShopBuy()
	end
end

function SummerFirstShopItem:SummerFirstShopBuy()
    if self.data.price_type == 3 then
        -- 记录当前购买道具，用于校验
        self.Data.now_buy_item = self.data
        ServerActivityCtrl.Instance:SendActivityReq(self.Data.act_type, 2, self.data.seq, 0, 0, self.Data.other_config.cfg_ver)
    else
        if MallCtrl.IsNotCurrency(self.data.price_type == 0 and CurrencyType.Gold or CurrencyType.CoinBind, self.data.price) then
            ServerActivityCtrl.Instance:SendActivityReq(self.Data.act_type, 1, self.data.seq, 0, 0, self.Data.other_config.cfg_ver)
        end
    end
end

function SummerFirstShopItem:SummerDiscountShopBuy()
    if self.data.price_type == 3 then
        -- 记录当前购买道具，用于校验
        self.Data.now_buy_item = self.data
        ServerActivityCtrl.Instance:SendActivityReq(self.Data.act_type, 3, self.data.seq, self.data.reward_group and 0 or 1, 0, self.Data.other_config.cfg_ver)
    else
        if MallCtrl.IsNotCurrency(self.data.price_type == 0 and CurrencyType.Gold or CurrencyType.CoinBind, self.data.price) then
            if self.data.reward_group then
                ServerActivityCtrl.Instance:SendActivityReq(self.Data.act_type, 2, self.data.index, 0, 0, self.Data.other_config.cfg_ver)
            else
                ServerActivityCtrl.Instance:SendActivityReq(self.Data.act_type, 1, self.data.seq, 0, 0, self.Data.other_config.cfg_ver)
            end
            
        end
    end
end