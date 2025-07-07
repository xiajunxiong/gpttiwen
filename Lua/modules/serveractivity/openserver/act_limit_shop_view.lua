ActLimitShopView = ActLimitShopView or DeclareMono("ActLimitShopView", UIWFlushPanel)

function ActLimitShopView:ActLimitShopView()
    self.data = ServerActivityData.Instance 
    self.data_cares = {
        {data = self.data.limit_shop_data,func = self.FlushShopView},
    }
end

function ActLimitShopView:Awake()
    UIWFlushPanel.Awake(self)
    self.Time:CreateActTime(ACTIVITY_TYPE.KF_LIMIT_SHOP,TimeType.Type_Special_4)
    self.Grid:SetData(DataHelper.FormatItemList(ServerActivityData.Instance:GetOtherConfig("discount_reward_item")))
end

function ActLimitShopView:FlushShopView()
    self.List:SetData(self:GetShopList(self.data:GetActivityOpenCfg("open_discount_store","seq")))
end

function ActLimitShopView:GetShopList(list)
    table.sort(list,function(a,b)
		a.weight = a.seq
        b.weight = b.seq
        a.buy_times = a.buy_times == 0 and a.total_exchange or a.buy_times
        b.buy_times = b.buy_times == 0 and b.total_exchange or b.buy_times
		if a.buy_times > 0 and a.buy_times - self.data:GetLimitShopTimes(a.seq) <= 0 then
			a.weight = 1000 + a.seq
		end
		if b.buy_times > 0 and b.buy_times - self.data:GetLimitShopTimes(b.seq) <= 0 then
			b.weight = 1000 + b.seq
		end
		return a.weight < b.weight
    end)
    return list
end

----------------------------ActLimitShopItem----------------------------
ActLimitShopItem = ActLimitShopItem or DeclareMono("ActLimitShopItem", UIWidgetBaseItem)

function ActLimitShopItem:SetData(data)
    data.item = Item.New(data.item)
    self.Call:SetData(data.item)
    data.price = data.need_gold
    data.price_type = CurrencyType.Gold
    UH.SetText(self.Name,data.item:QuaName(true))
    UH.SetText(self.OPrice,data.old_gold)

    local LimitShop = Language.OpenAct.LimitShop
    UH.SetText(self.PPrice,data.price == 0 and LimitShop.Free or data.price)
    self.BtnFree:SetActive(data.price == 0)
    UH.SetText(self.Count,Format(LimitShop.Discount,data.discount / 10))
    self.Discount:SetActive(data.discount ~= 0)

    data.buy_times = data.buy_times == 0 and data.total_exchange or data.buy_times
    data.limit_type = data.buy_times ~= 0 and 1 or 0
    local times = ServerActivityData.Instance:GetLimitShopTimes(data.seq)
    data.limit_times = data.buy_times - times
    local limit_shop = LimitShop.Times[data.total_exchange == 0 and 1 or 2]
    UH.SetText(self.Times,Format(limit_shop,times,data.buy_times))

    self.BtnInter.Interactable = data.limit_times ~= 0
    self.BtnFreeInter.Interactable = data.limit_times ~= 0
    self.RedPos:SetNum(self.BtnInter.Interactable and 1 or 0)

    UIWidgetBaseItem.SetData(self, data)
end

function ActLimitShopItem:OnClickItem()
    if self.data.need_gold == 0 then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.KF_LIMIT_SHOP,1,self.data.seq,1)
        return
    end
    self.data.confirm_func = function(make_num)
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.KF_LIMIT_SHOP,1,self.data.seq,make_num)
    end
    ViewMgr:OpenView(BatchBuyView,self.data)
end