RewardMonthCardView = RewardMonthCardView or DeclareMono("RewardMonthCardView", UIWFlushPanel)
function RewardMonthCardView:RewardMonthCardView()
    self.data = RewardData.Instance
    self.lang = Language.MonthCard
    self.data_cares = {
        {data = self.data.month_card_data, func = self.FlushView},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false}
    }
    self.cur_type = RewardConfig.MonthCardType.TYPE_MONTH
    local function func_delay()
        self.open_timer = nil
        self:SetCardActive(true)
        if self.data:IsWeekCardActived() and not self.data:IsMonthCardActived() then
            self:OnClickBack()
        end
    end
    self.open_timer = TimeHelper:AddDelayFrameTimer(func_delay, 2)
    if self.data:GetMonthCardTodayRemind() then
        self.data:SetMonthCardTodayRemind()
        self.data:CheckMonthCardRemind()
    end
end

function RewardMonthCardView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.open_timer ~= nil then
        TimeHelper:CancelTimer(self.open_timer)
        self.open_timer = nil
    end
end

function RewardMonthCardView:OnEnable()
    for i, v in pairs(self.ShopList.item_list) do
        v:ResetAnim()
    end
end

function RewardMonthCardView:FlushView()
    local show_list = self.data:GetMonthCardShopItems()
    for i, v in ipairs(show_list) do
        v.item_index = i
    end
    if IS_AUDIT_VERSION and GLOBAL_CONFIG:Flags().show_month_and_fortunebag then
        show_list = DataHelper.TableCopy(show_list)
        for i=#show_list,1,-1 do
            if show_list[i] and show_list[i].price ~= 0 then
                table.remove(show_list,i)
            end
        end
    end
    self.ShopList:SetData(show_list)

    self.WeekCard:SetData(self.data:GetMonthCardCfg(RewardConfig.MonthCardType.TYPE_WEEK))
    self.MonthCard:SetData(self.data:GetMonthCardCfg(RewardConfig.MonthCardType.TYPE_MONTH))
end

function RewardMonthCardView:OnClickBack()
    --改位置。
    self:SetCardActive(false)
    self:ChangeParent(self.CardList[self.cur_type + 1], self.BackNode)
    self.cur_type = self.cur_type == RewardConfig.MonthCardType.TYPE_MONTH and RewardConfig.MonthCardType.TYPE_WEEK or RewardConfig.MonthCardType.TYPE_MONTH
    self:ChangeParent(self.CardList[self.cur_type + 1], self.MainNode)
    self:SetCardActive(true)

    -- for i, v in pairs(self.ShopList.item_list) do
    --     v:FlushType()
    -- end
end

function RewardMonthCardView:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_MONTH_CARD, self.buy_money)
end

function RewardMonthCardView:CurCardType()
    return self.cur_type
end

function RewardMonthCardView:SetCardActive(active)
    for i = 1, 2 do
        self.CardList[i]:SetActive(active)
    end
end
function RewardMonthCardView:ChangeParent(obj, parent)
    obj:SetParent(parent)
    UH.LocalPos(obj.transform, Vector3.New(180, 300, 0))
    UH.LocalScale(obj.transform, Vector3.one)
    UH.LocalEulerAngles(obj.transform, Vector3.zero)
end

----------------------------RewardMonthCardDayItem----------------------------
RewardMonthCardBaseItem = RewardMonthCardBaseItem or DeclareMono("RewardMonthCardBaseItem", UIWidgetBaseItem)
function RewardMonthCardBaseItem:RewardMonthCardBaseItem()
    self.reward_data = RewardData.Instance
    self.lang = Language.MonthCard
    local function func_time(t)
        if self.data == nil then
            return ""
        end
        local end_time = TimeCtrl.Instance:ChangDataTime24(self.reward_data:CardEndTimeStamp(self.data.card_type))
        local time = end_time - TimeHelper.GetServerTime()
        return string.format(self.lang.TimeStr, math.floor(time / 86400))
    end
    self.Timer:SetShowCallBack(func_time)
    local function func_finish()
        self.reward_data.month_card_data.flush = not self.reward_data.month_card_data.flush
        self.Timer:CloseCountDownTime()
    end
    self.Timer:SetCallBack(func_finish)
end

function RewardMonthCardBaseItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local is_actived = self.reward_data:IsCardActived(self.data.card_type)

    local show_list = {}
    for i = 0, #self.data.day_reward_item do
        if i == 0 then  --第一个灵玉要显示数量
            local base_item = self.data.day_reward_item[i]
            local item_cfg = Item.GetConfig(base_item.item_id)
            local item = {item_id = CommonItemId.ImmortalCoin, num = base_item.num * item_cfg.param2}
            table.insert(show_list, {card_type = self.data.card_type, item = item})
        else
            table.insert(show_list, {card_type = self.data.card_type, item = self.data.day_reward_item[i]})
        end
    end
    self.ItemList:SetData(show_list)

    local btn_str = RechargeData.Instance:GoldToMoney(self.data.buy_money)
    if is_actived then
        btn_str = btn_str .. self.lang.XuFei
        UH.LocalPosG(self.BuyIcon.gameObject, Vector3.New(-60, 0, 0))
    else
        UH.LocalPosG(self.BuyIcon.gameObject, Vector3.New(-35, 0, 0))
    end
    UH.SetText(self.Price, btn_str)
    UH.SetText(self.RewardVal, self.data.buy_reward_gold)
    UH.SetText(self.ReturnVal, string.format(self.lang.ReturnStr, self.data.return_multiple))

    if is_actived then
        self.Timer.gameObject:SetActive(true)
        self.Timer:StampTime(self.reward_data:CardEndTimeStamp(self.data.card_type))
    else
        self.Timer.gameObject:SetActive(false)
    end
    local is_get = RewardData.Instance:IsMonthCardDayRewardGet(self.data.card_type)
    self.Geted:SetActive(is_get)
end

function RewardMonthCardBaseItem:OnClickBuy()
    if self.data == nil then
        return
    end
    local function func_ok()
        local fee = self.data.buy_money / 10
        if ServerActivityCtrl.Instance:IsCanItemBuy(BUY_ITEM_TYPE.TYPE_MONTH_CARD, fee) then
            ServerActivityCtrl.Instance:SendBuyCmdReq(BUY_ITEM_TYPE.TYPE_MONTH_CARD,fee,self.data.card_type)
        else
            self.MainView.buy_money = self.data.buy_money
            RewardCtrl.Instance:SendMonthCardReq(RewardConfig.MonthCardReqType.TYPE_BUY_CARD_CHECK, self.data.card_type, self.reward_data:MonthCardVerCfg())
        end
    end
    local is_actived = self.reward_data:IsCardActived(self.data.card_type)
    if is_actived then
        local param_t = {
            card_type = self.data.card_type,
            func_ok = func_ok
        }
        ViewMgr:OpenView(MonthCardAlert, param_t)
        return
    else
        func_ok()
    end
end

----------------------------每日奖励----------------------------
RewardMonthCardDayItem = RewardMonthCardDayItem or DeclareMono("RewardMonthCardDayItem", UIWidgetBaseItem)
function RewardMonthCardDayItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ItemCell:SetData(Item.New(data.item))
    local is_actived = RewardData.Instance:IsCardActived(self.data.card_type)
    local is_get = RewardData.Instance:IsMonthCardDayRewardGet(self.data.card_type)
    self.Block:SetActive(is_actived and not is_get)
end

function RewardMonthCardDayItem:OnClickGet()
    if self.data ~= nil then
        RewardCtrl.Instance:SendMonthCardReq(RewardConfig.MonthCardReqType.TYPE_FETCH_DAY_REWARD, self.data.card_type, 0)
    end
end

----------------------------商品----------------------------
RewardMonthCardShopItem = RewardMonthCardShopItem or DeclareMono("RewardMonthCardShopItem", UIWidgetBaseItem)
function RewardMonthCardShopItem:RewardMonthCardShopItem()
    self.reward_data = RewardData.Instance
    self.lang = Language.MonthCard
end

function RewardMonthCardShopItem:OnRelease()
    UIWidgetBaseItem.OnRelease(self)
    TimeHelper:CancelTimer(self.time_handle)
    self.time_handle = nil
end

function RewardMonthCardShopItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.gameObject.name = "item" .. data.item_index

    self.item = Item.New(self.data.reward_item)
    UH.SetText(self.Name, self.item:Name())
    -- self.ItemCell:SetData(item)
	local icon_id = self.item.IconId and self.item:IconId() or 0
    UH.SetIcon(self.ItemIcon, icon_id)

    if self.data.price == 0 then
        self.FreeNode:SetActive(true)
        self.PriceNode:SetActive(false)
        self:SetTagActive(false)
    else
        self.FreeNode:SetActive(false)
        self.PriceNode:SetActive(true)
        self:SetTagActive(true)
        if self.data.price_type == 3 then
            UH.SetText(self.Price, RechargeData.Instance:GoldToMoney(self.data.price))
        else
            UH.SetText(self.Price, self.data.price)
        end
        UH.SetGold(self.Icon, self.data.price_type, RewardConfig.MonthCardPriceType)
        UH.SetText(self.BasePrice, self.data.value)
        UH.SetText(self.Discount, string.format(self.lang.Discount, self.data.discount / 10))
    end
    local buy_times = RewardData.Instance:GetMonthCardBuyTimes(self.data.seq) or 0
    local max_times = self.data.limit_type == 1 and self.data.buy_times or self.data.today_times
    local remain_times = max_times - buy_times
    UH.SetText(self.Limit, string.format(self.lang.BuyType[self.data.limit_type], remain_times, max_times))

    local is_actived = self.reward_data:IsWeekCardActived() or self.reward_data:IsMonthCardActived()
    self.ClickObj:SetActive(is_actived)
    if is_actived then
        self.Mask:SetActive(false)
    else
        self.Mask:SetActive(true)
        self:SetTagActive(false)
    end
    self:FlushType()

    local is_free = is_actived and self.data.price == 0 and self.reward_data:GetMonthCardBuyTimes(self.data.seq) < self.data.limit_times
    self.RedPoint:SetNum(is_free and 1 or 0)

    local is_over = remain_times < 1
    -- self.ItemInter.Interactable = not is_over
    self.Geted:SetActive(is_over)
    if is_over then
        self:SetTagActive(false)
    end
end

function RewardMonthCardShopItem:FlushType()
    -- local card_type = self.MainView:CurCardType()
    UH.SetText(self.MaskText, self.lang.UnlockType)
end

function RewardMonthCardShopItem:OnClickBuy()
    local is_unlock = self.reward_data:IsWeekCardActived() or self.reward_data:IsMonthCardActived()
    if not is_unlock then
        return
    end
    if self.data.price_type == 3 then
        local fee = self.data.price / 10
        if ServerActivityCtrl.Instance:IsCanItemBuy(BUY_ITEM_TYPE.TYPE_MONTH_CARD, fee) then
            ServerActivityCtrl.Instance:SendBuyCmdReq(BUY_ITEM_TYPE.TYPE_MONTH_CARD, fee,self.data.seq, 1)  --param2 - type(0:p2填card_type 1:商品seq)
        else
            self.MainView.buy_money = self.data.price
            RewardCtrl.Instance:SendMonthCardReq(RewardConfig.MonthCardReqType.TYPE_BUY_CMD_CHECK, self.data.seq, self.reward_data:MonthCardVerCfg())
        end 
    else
        RewardCtrl.Instance:SendMonthCardReq(RewardConfig.MonthCardReqType.TYPE_BUY_ITEM, self.data.seq, 0)
    end
end

function RewardMonthCardShopItem:SetTagActive(is_active)
    if is_active then
        local function func_open()
            self.time_handle = nil
            self.TagNode:SetActive(true)
            self.TagNodeAnim:Play()
        end
        if self.time_handle == nil then
            self.time_handle = TimeHelper:AddDelayTimer(func_open, self.data.item_index * 0.3 - 0.1)
        end
    else
       self.TagNode:SetActive(false)
       TimeHelper:CancelTimer(self.time_handle)
       self.time_handle = nil
    end
end

function RewardMonthCardShopItem:ResetAnim()
    if GetActive(self.TagNode) then
       TimeHelper:CancelTimer(self.time_handle)
       local function func_open()
            self.time_handle = nil
            self.TagNode:SetActive(true)
            self.TagNodeAnim:Play()
        end
        self.time_handle = TimeHelper:AddDelayTimer(func_open, self.data.item_index * 0.3 - 0.1)
    end
end

function RewardMonthCardShopItem:OnClickItem()
    if self.item ~= nil then
        CellClicks.BagGridNoButton({GetData = function()
            return self.item
        end})
    end
end