SpringFestivalStoreView = SpringFestivalStoreView or DeclareView("SpringFestivalStoreView", "serveractivity/spring_festival/spring_festival_store_view", Mod.SpringFestival.Store)

VIEW_DECLARE_LEVEL(SpringFestivalStoreView, ViewLevel.L1)
VIEW_DECLARE_MASK(SpringFestivalStoreView, ViewMask.BgBlock)
function SpringFestivalStoreView:LoadCallback()
    self.Currency:DefaultSet()
end

function SpringFestivalStoreView:OnCloseClick()
    ViewMgr:CloseView(SpringFestivalStoreView)
end


SpringFestivalStoreBtn = {
    Now = 1,    -- 本期采购
    Next = 2,   -- 下期采购
    Last = 3,   -- 上期采购
}
----------------------------------------------------
--------------SpringFestivalStorePanel--------------
----------------------------------------------------
SpringFestivalStorePanel = SpringFestivalStorePanel or DeclareMono("SpringFestivalStorePanel", UIWFlushPanel)
function SpringFestivalStorePanel:SpringFestivalStorePanel()
    self.SAData = ServerActivityData.Instance
    self.data = self.SAData.spring_festival_store_data
    -- 防止手速比协议快
    if not self.data.info.open_day then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SPRING_STORE, 0)
        ViewMgr:CloseView(SpringFestivalStoreView)
    end

    self.NextTime:SetShowCallBack(function(t)
        return Format(Language.SpringFestivalStore.Str1, t.day, t.hour, t.min)
    end)
    self.data_cares = {
        {data = self.data, func = self.FlushShow, keys = {"info"}, init_call = true}
    }
end

-- 点击本期采购(1)和下期采购(2)和上期采购(3)，是否强制刷新
function SpringFestivalStorePanel:OnClickNow(param, flag)
    -- 防止一直点同一个
    if self.BtnUpLists[param].activeSelf and not flag then
        return
    end
    for i = 1, self.BtnUpLists:Length() do
        self.BtnUpLists[i]:SetActive(param == i)
    end
    self.Node:SetActive(param ~= SpringFestivalStoreBtn.Next)
    self.NodeNext:SetActive(param == SpringFestivalStoreBtn.Next)
    self.StoreListView:SetData(param == SpringFestivalStoreBtn.Now and self.config or self.last_config)
end

function SpringFestivalStorePanel:FlushShow()
    self.config = self.SAData:GetSpringFestivalStoreDataByPrase(self.data.info.phase)
    self.last_config = self.SAData:GetSpringFestivalStoreDataByPrase(self.data.info.phase - 1, true)
    self:JudgeHaveNext()
    self:OnClickNow(1, true)
end

-- 判断是否还有下期和上期
function SpringFestivalStorePanel:JudgeHaveNext()
    self.NextTime:CloseCountDownTime()
    self.is_have_next = self.data.info.phase ~= self.SAData:GetSpringFestivalStoreMaxPhase()
    self.is_have_last = self.data.info.phase ~= 1
    if self.is_have_next then
        self.NextTime:StampTime(self:CalculateNextTime(), TimeType.Type_Time_3)
    end
    self.BtnLists[SpringFestivalStoreBtn.Next]:SetActive(self.is_have_next)
    self.BtnLists[SpringFestivalStoreBtn.Last]:SetActive(self.is_have_last)
end

-- 计算下一阶段开始的时间戳
function SpringFestivalStorePanel:CalculateNextTime()
    local now_date = os.date("*t", TimeManager.GetServerTime())
    now_date.day = now_date.day + self.config[1].section_end - self.data.info.open_day + 1
    return TimeCtrl.Instance:ChangDataStampTime(now_date)
end


--------------------------------------
--------------年货节道具--------------
--------------------------------------
SpringFestivalStoreItem = SpringFestivalStoreItem or DeclareMono("SpringFestivalStoreItem", UIWidgetBaseItem)

function SpringFestivalStoreItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(Item.New(self.data.reward_item))
    UH.SetText(self.Name, Item.GetQuaName(self.data.reward_item.item_id, true))
    local str = Format(Language.SpringFestivalStore.Limit[self.data.limit_type], self.data.times, self.data.limit_type == 1 and self.data.buy_times or self.data.today_times)
    UH.SetText(self.Num, str)
    UH.SetText(self.TxtPrice, self.data.price_type == 3 and self.data.price / 10 or self.data.price)
    UH.SetText(self.Value, Format(Language.SpringFestivalStore.Value, self.data.value))
    UH.SetGold(self.PriceImg, self.data.price_type)
    self.PriceImg:SetObjActive(self.data.price_type ~= 3)
    self.PriceImg3:SetActive(self.data.price_type == 3)
    self.BtnBuy.Interactable = self.data.times ~= 0
    self.BtnBuy.gameObject:SetActive(not self.data.is_last)
    self.ObjBuyed:SetActive(self.data.times == 0 and not self.data.is_last)
end

function SpringFestivalStoreItem:OnBuyClick()
    -- 0是元宝，2是铜钱，3是直购
    if self.data.price_type == 3 then
        local fee = self.data.price / 10
        if ServerActivityCtrl.Instance:IsCanItemBuy(BUY_ITEM_TYPE.TYPE_SPRING_STORE, fee) then
            ServerActivityCtrl.Instance:SendBuyCmdReq(BUY_ITEM_TYPE.TYPE_SPRING_STORE,fee,self.data.seq)
        else
            if ChannelAgent ~= nil then
                ChannelAgent:RequestPay(self.data.price / 10, BUY_ITEM_TYPE.TYPE_SPRING_STORE, self.data.type)
            end
        end
    elseif self.data.price_type == 0 then
        if MallCtrl.IsNotCurrency(CurrencyType.Gold, self.data.price) then
            ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SPRING_STORE, 1, self.data.type, self.data.seq)
        end
    else
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_SPRING_STORE, 1, self.data.type, self.data.seq)
    end
end