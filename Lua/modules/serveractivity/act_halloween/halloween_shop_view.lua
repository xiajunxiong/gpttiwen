HalloweenShopView = HalloweenShopView or DeclareView("HalloweenShopView", "serveractivity/act_halloween/halloween_shop", Mod.Act_Halloween.Shop)
VIEW_DECLARE_LEVEL(HalloweenShopView, ViewLevel.Lt)
VIEW_DECLARE_MASK(HalloweenShopView, ViewMask.BgBlock)

function HalloweenShopView:HalloweenShopView()
end

function HalloweenShopView:LoadCallback(param_t)
    ActHalloweenData.Instance:ClearShopFirstRemind()
    self.CurrencySub:SetCurrency(CurrencyType.CoinBind,CurrencyType.ImmortalCoin, CurrencyType.Gold)
end

function HalloweenShopView:OnCloseClick()
    ViewMgr:CloseView(HalloweenShopView)
end

--Panel
HalloweenShopPanel = HalloweenShopPanel or DeclareMono("HalloweenShopPanel", UIWFlushPanel)

function HalloweenShopPanel:HalloweenShopPanel()
    self.data = ActHalloweenData.Instance
    self.data_cares = {{data = self.data.shop_data, func = self.FlushAll, init_call = false}, 
    {data  = self.data.shop_data, func = self.FlushCurrNum, init_call = false,keys = {"curr_num"}},
    {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
}
    self:CheckOutView()
    self:FlushAll()
    self.Time:SetShowCallBack(function(t)
        return Format(Language.UiTimeMeter.TimeStr3, t.day, t.hour, t.min)
    end)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_WAN_SHENG_SHOP, TimeType.Type_Time_3)

    self.CTime:SetShowCallBack(function(t)
        return Format(Language.UiTimeMeter.TimeStr3, t.day, t.hour, t.min)
    end)
    self.CTime:CreateActTime(ACTIVITY_TYPE.RAND_WAN_SHENG_SHOP, TimeType.Type_Time_3)

    if self.c_target then 
        self.CurrencyObj:SetLocalPosition(Vector3.New(655,0,0))
    else
        self.CurrencyObj:SetLocalPosition(Vector3.New(432,0,0))
    end 
end


function HalloweenShopPanel:CheckOutView()
    local cfg = self.data:GetShopCfg()
    self.Npart:SetActive(Config.wanshengshangcheng_auto.activity_interface[1].is_show == 1)
    self.Cpart:SetActive(Config.wanshengshangcheng_auto.activity_interface[2].is_show == 1)
    self.c_target = Config.wanshengshangcheng_auto.activity_interface[1].is_show == 1
end 

function HalloweenShopPanel:Awake()
    UIWFlushPanel.Awake(self)
end


function HalloweenShopPanel:FlushAll()
    if self.c_target then 
        self.List:SetData(self.data:GetShopItemSortList())
    else 
        self.CList:SetData(self.data:GetShopItemSortList())
    end 
    
    self:FlushCurrNum()
end

function HalloweenShopPanel:FlushCurrNum()
    if self.c_target then 
        UH.SetText(self.TextGiftNum,self.data.shop_data.curr_num)
        self.RedPos:SetNum(self.data.shop_data.curr_num>=10 and 1 or 0)
    else 
        UH.SetText(self.CTextGiftNum,self.data.shop_data.curr_num)
        self.CRedPos:SetNum(self.data.shop_data.curr_num>=10 and 1 or 0)
    end
    
end

function HalloweenShopPanel:OnBuyClick(data)
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_WAN_SHENG_SHOP) then
        PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
        return
    end
    if self.data:GetBuyTime(data) <= 0 then
        -- 购买次数不足
        PublicPopupCtrl.Instance:Center(Language.Activity.ButLimitTip)
        return
    end
    if data.price_type == 3 then
        -- 直购
        local fee = data.price / 10
        self.now_buy_data = data
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_WAN_SHENG_SHOP, fee, ACTIVITY_TYPE.RAND_WAN_SHENG_SHOP, WAN_SHENG_SHOP_REQ_TYPE.BUY_ZHIGOU, data.seq)
    else
        -- 非直购购买
        if not MallCtrl.IsNotCurrency(data.price_type, data.price) then
            -- 物品不足
            return
        end
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_WAN_SHENG_SHOP, WAN_SHENG_SHOP_REQ_TYPE.BUY, data.seq)
    end
end

function HalloweenShopPanel:SendBuy()
    if TableIsEmpty(self.now_buy_data) then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_WAN_SHENG_SHOP, self.now_buy_data.price)
    self.now_buy_data = nil
end

function HalloweenShopPanel:GoToQiFu()
    if ViewMgr:IsOpen(HalloweenPrayView) then
        ViewMgr:CloseView(HalloweenPrayView)
    end
    self.open_view_timer=TimeHelper:AddDelayFrameTimer(function()
        ViewMgr:OpenView(HalloweenPrayView)
    end, 1)
end

function HalloweenShopPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.open_view_timer ~= nil then
        TimeHelper:CancelTimer(self.open_view_timer)
        self.open_view_timer = nil
    end
end

--Item
HalloweenShopItem = HalloweenShopItem or DeclareMono("HalloweenShopItem", UIWidgetBaseItem)

function HalloweenShopItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.language = Language.Halloween_Shop
    -- local cfg=ActHalloweenData.Instance:GetShopCfg()[data.seq+1]
    UH.SetText(self.TextGiftNum,data.give_item.num)
    UH.SetText(self.TextName,data.gift_name)
    local buyTime=ActHalloweenData.Instance:GetBuyTime(data)
    UH.SetText(self.TextBuyTimes,self.language.buyTimes[data.limit_type]..buyTime)
    self.ImageShouQing:SetActive(buyTime<=0)
    self.Inter.Interactable = buyTime>0
    if data.price_type == 3 then
        UH.SetText(self.TextProPrice,Language.Guild.PriceTips.. data.value/10)
        UH.SetText(self.TextPrice, data.price/10)
        UH.SpriteName(self.ImageCostIcon, HuoBi[999])
    else 
        UH.SetText(self.TextProPrice, Language.Guild.PriceTips.. data.value)
        UH.SetText(self.TextPrice, data.price)
        UH.SpriteName(self.ImageCostIcon, HuoBi[data.price_type])
    end
    local list1,list2=ActHalloweenData.Instance:GetShopRewardList(data)
    self.List1:SetData(DataHelper.FormatItemList(list1, 1))
    self.List2:SetData(DataHelper.FormatItemList(list2, 1))
    self:PlayEffect(buyTime>0)
end

function HalloweenShopItem:PlayEffect(is_play)
    if self.EffectHandle ~= nil then
        if is_play then
            self.EffectHandle = UH.PlayEffect(self.UiEffect, 3164008)
        else
            UH.StopEffect(self.UiEffect, self.EffectHandle)
        end
    end
end

function HalloweenShopItem:OnDestroy()
    UIWidgetBaseItem.OnDestroy(self)
    if self.EffectHandle ~= nil then
        UH.StopEffect(self.UiEffect, self.EffectHandle)
    end
end

function HalloweenShopItem:ShowCurInfo()
    ItemInfoCtrl.Instance:ItemInfoView({item_list = {Item.Init(self.data.give_item.item_id, 1, 0)}})
end


