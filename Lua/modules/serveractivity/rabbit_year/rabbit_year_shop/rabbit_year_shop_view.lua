-- 福运商城
RabbitYearShopView = RabbitYearShopView or DeclareView("RabbitYearShopView", "serveractivity/rabbit_year/rabbit_year_shop", Mod.RabbitYear.FuYunShangCheng)
function RabbitYearShopView:OnCloseClick()
    ViewMgr:CloseView(RabbitYearShopView)
end


RabbitYearShopPanel = RabbitYearShopPanel or DeclareMono("RabbitYearShopPanel", UIWFlushPanel)
function RabbitYearShopPanel:RabbitYearShopPanel()
    self.data = RabbitYearShopData.Instance
    self.data_cares = {
        {data = self.data.act_info, func = self.FlushPanel, init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
    self.act_type = self.data.act_type
end

function RabbitYearShopPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.ActTime:CreateActTime(self.act_type,TimeType.Type_Special_9, Language.RabbitYearShop.ActTime)
    self.Currency:DefaultSet()
    self:FlushPanel()
end

function RabbitYearShopPanel:FlushPanel()
    local data_list = self.data:GetShopItemList()
    table.sort(data_list, function (a,b)
        local a_num = a.seq
        local b_num = b.seq
        if self.data:GetRemainBuyTiems(a) <= 0 then
            a_num = 10000
        end
        if self.data:GetRemainBuyTiems(b) <= 0 then
            b_num = 10000
        end
        return a_num < b_num
    end)
    self.List:SetData(data_list)
end

function RabbitYearShopPanel:SendBuy()
    if self.buy_data == nil then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_FU_YUN_SHANG_CHENG, self.buy_data.price)
end

function RabbitYearShopPanel:OnBuyClick(data)
    self.buy_data = data
    local remain_times = RabbitYearShopData.Instance:GetRemainBuyTiems(data)
    if remain_times <= 0 then
        PublicPopupCtrl.Instance:Error(Language.RabbitYearShop.Tip1)
        return
    end
    local p = data.cfg_or == 1 and RabbitYearShopCtrl.ReqType.Buy1 or RabbitYearShopCtrl.ReqType.Buy2
    if data.cfg_or == 0 then
        if MallCtrl.IsNotCurrency(data.price_type, data.price) then
            ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_FU_YUN_SHANG_CHENG, p,data.seq)
        end
    else
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_FU_YUN_SHANG_CHENG,data.price/10, ACTIVITY_TYPE.RAND_FU_YUN_SHANG_CHENG, p,data.seq)
    end
end


RabbitYearShopItem = RabbitYearShopItem or DeclareMono("RabbitYearShopItem", UIWidgetBaseItem)
function RabbitYearShopItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.gift_name)
    local reward_len = table.nums(data.reward_item)
    local item_list = DataHelper.FormatItemList(data.reward_item)
    self.CellList:SetObjActive(reward_len ~= 3)
    self.SpCellListObj:SetActive(reward_len == 3)
    if reward_len ~= 3 then
        self.CellList:SetData(item_list)
    else
        for i = 1, self.SPCellList:Length() do
            self.SPCellList[i]:SetData(item_list[i])
        end
    end
    UH.SetText(self.Price, string.format(Language.RabbitYearShop.Price, RechargeData:GoldToMoney(data.value)))
    local limit_text = data.limit_type == 2 and Language.RabbitYearShop.LimitDay or Language.RabbitYearShop.LimitAct
    local remain_times = RabbitYearShopData.Instance:GetRemainBuyTiems(data)
    local remain_times_str = remain_times
    self.Fee:SetObjActive(remain_times > 0)
    self.BtnText:SetObjActive(remain_times <= 0)
    if remain_times <= 0 then
        remain_times_str = ColorStr(remain_times_str, COLORSTR.Red8)
        UH.SetText(self.BtnText, Language.RabbitYearShop.ShouQing)
    else
        if data.cfg_or == 1 then
            UH.SetText(self.Fee, RechargeData.Instance:GoldToMoney(data.price))
        else
            UH.SetText(self.Fee, data.price)
        end
        UH.SetActGold(self.FeeIcon, data.price_type, data.cfg_or)
    end
    UH.SetText(self.LimitText, string.format(limit_text, remain_times_str, data.limit_type == 2 and data.day_times or data.limit_times))
    local is_give = data.give_item ~= 0 and not TableIsEmpty(data.give_item)
    self.GiveObj:SetActive(is_give)
    if is_give then
        UH.SetText(self.GiveNum, data.give_item.num)
        UH.SetIcon(self.ItemIcon, Item.GetIconId(data.give_item.item_id))
    end
    UH.SetInter(self.Inter, remain_times > 0)
    self.BtnEffect:SetActive(remain_times > 0)
end

function RabbitYearShopItem:OnGiveClick()
    ItemInfoCtrl.Instance:ShowItemInfo(self.data.give_item.item_id)
end