SummerDiscountShopView = SummerDiscountShopView or DeclareView("SummerDiscountShopView", "serveractivity/summer_first/summer_discount_shop", Mod.SummerFirst.SummerDiscountShop)
VIEW_DECLARE_LEVEL(SummerDiscountShopView, ViewLevel.L1)
VIEW_DECLARE_MASK(SummerDiscountShopView, ViewMask.BgBlock)
function SummerDiscountShopView:SummerDiscountShopView()
    self.Currency:SetCurrency(CurrencyType.CoinBind, CurrencyType.Gold)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_SUMMER_DISCOUNT_SHOP, TimeType.Type_Time_3)
    SummerFirstShopData.Instance:SetData(
        Config.cool_summer_auto.super_off_shop, 
        Config.cool_summer_auto.other[1], 
        Config.cool_summer_auto.store_name, 
        ACTIVITY_TYPE.RAND_SUMMER_DISCOUNT_SHOP,
        BUY_ITEM_TYPE.TYPE_DISCOUNT_STORE,
        Config.cool_summer_auto.random_group)
end

function SummerDiscountShopView:OnCloseClick()
    ViewMgr:CloseView(SummerDiscountShopView)
end

function SummerDiscountShopView:OnCloseCallback()
    SummerFirstShopData.Instance:SetData()
end
