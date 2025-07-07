GiftBoxView = GiftBoxView or DeclareView("GiftBoxView", "serveractivity/gift_box_view", Mod.Splendid.GiftBox)
VIEW_DECLARE_LEVEL(GiftBoxView, ViewLevel.L1)
function GiftBoxView:LoadCallback(param_t)
    local style_type = ServerActivityData.Instance:GetGiftBoxShowStyle()
    self.Board:SetData(self:GetType(), ActivityRandData.GetName(ACTIVITY_TYPE.RAND_GIFT_BOX), style_type)
    self:FlushTopView(style_type)
end

function GiftBoxView:FlushTopView(index)
    for i = 1, self.Tops:Length() do
        self.Tops[i]:SetActive((index + 1) == i)
    end
end

function GiftBoxView:OnClickClose()
    ViewMgr:CloseView(GiftBoxView)
end

GiftBoxPanel = GiftBoxPanel or DeclareMono("GiftBoxPanel", UIWFlushPanel)
function GiftBoxPanel:GiftBoxPanel()
    self.cur_page, self.min_page, self.max_page = 1, 1, 1
    self.is_ordering = false
    self.data_cares = {
        {data = ServerActivityData.Instance.gift_box_data, func = self.onFlush},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
    local function func_delay()
        self.open_timer = nil
        self.ItemAnim.enabled = true
        self.ItemList[1].Effect:SetActive(true)
    end
    self.open_timer = Invoke(func_delay, 0.8)
end

function GiftBoxPanel:Awake()
	UIWFlushPanel.Awake(self)
    self.Timer:CreateActTime(ACTIVITY_TYPE.RAND_GIFT_BOX, TimeType.Type_Time_3, Language.RAUstomizedBox.Txt2)
end

function GiftBoxPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.open_timer ~= nil then
        TimeHelper:CancelTimer(self.open_timer)
        self.open_timer = nil
    end
    TimeHelper:CancelTimer(self.flush_timer)
    self.flush_timer = nil
end

function GiftBoxPanel:onFlush()
    self.is_ordering = false
    local base_list = ServerActivityData.Instance:GetGiftBoxShowList()
    self.first_data = base_list[1]
    self.show_list = DataHelper.FormatList(base_list, 2)
    self.max_page = math.ceil(#self.show_list / 2)
    --晚两帧执行，防止item的UiEffectTool还未初始化
    TimeHelper:CancelTimer(self.flush_timer)
    self.flush_timer = TimeHelper:AddDelayFrameTimer(BindTool.Bind(self.FlushPageItem, self), 2)
    self:FlushFreeGift()
end

function GiftBoxPanel:FlushPageItem()
    if self.show_list ~= nil then
        self.ItemObjList[1]:SetActive(true)
        self.ItemList[1]:SetData(self.first_data)
        for i = 1, 2 do
            local data = self.show_list[(self.cur_page - 1) * 2 + i]
            self.ItemObjList[i + 1]:SetActive(data ~= nil)
            if data ~= nil then
                self.ItemList[i + 1]:SetData(data)
            end
        end
    end
    self.PageLeft:SetActive(self.cur_page ~= self.min_page)
    self.PageRight:SetActive(self.cur_page ~= self.max_page)
end

function GiftBoxPanel:OnClickPage(param)
    if (param == -1 and self.cur_page == self.min_page) or (param == 1 and self.cur_page == self.max_page) then
        return
    end
    self.cur_page = self.cur_page + param
    self.ItemAnim:SetTrigger("show")
    self:FlushPageItem()
end

function GiftBoxPanel:DoOrdering()
    self.is_ordering = true
    self:FlushPageItem()
end
function GiftBoxPanel:IsOrdering()
    return self.is_ordering
end

function GiftBoxPanel:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_XIAN_SHI_BAO_HE_NEW, self.buy_money)
end

function GiftBoxPanel:SaveMoney(buy_money)
    self.buy_money = buy_money
end

function GiftBoxPanel:FlushFreeGift()
    self.FreeGiftNode:SetActive(not ServerActivityData.Instance:IsGiftBoxFreeGiftFetched())
end

function GiftBoxPanel:OnClickFreeGift()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_GIFT_BOX, 2)
end

GiftBoxItem = GiftBoxItem or DeclareMono("GiftBoxItem", UIWidgetBaseItem)
function GiftBoxItem:GiftBoxItem()
    self.lang = Language.RAGiftBox
    self.effect_id = 0
end

function GiftBoxItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:FlushAllInfo()
end

function GiftBoxItem:FlushAllInfo()
    UH.SetText(self.NameText, self.data.gift_des)
    if self.data.limit_type == 1 then
        local buy_times = ServerActivityData.Instance:GetGiftBoxBuyTimes(self.data.seq)
        local max_times = self.data.buy_times
        self:TimesStr(self.lang.Limit1, buy_times, max_times)
    else
        local buy_times = ServerActivityData.Instance:GetGiftBoxDayBuyTimes(self.data.seq)
        local max_times = self.data.today_times
        self:TimesStr(self.lang.Limit2, buy_times, max_times)
    end
    UH.SetText(self.BtnBuyText, self:MoneyPrice())
    UH.SpriteName(self.MarkImg, "BiaoQian" .. (self.data.gift_mark or 1))
    UH.SetText(self.MarkText, self.data.gift_name)
    UH.SpriteName(self.BoxImg, "Box" .. self.data.gift_icon)

    local have_times = ServerActivityData.Instance:IsGiftBoxBuyHaveTimes(self.data)
    if have_times then
        self.BtnBuyInter.Interactable = not self.BoxPanel:IsOrdering()
    else
        self.BtnBuyInter.Interactable = false
    end
    self:ShowItems()
    self:ShowEffect()
end

function GiftBoxItem:ShowItems()
    local show_list = {}
    for i = 0, #self.data.reward_item do
        table.insert(show_list, self.data.reward_item[i])
    end
    if self.data.gold_num ~= 0 then
        table.insert(show_list, Item.Init(HuoBi[CurrencyType.Gold], self.data.gold_num))
    end
    for i = 1, 5 do
        if show_list[i] ~= nil then
            self.CellNodeList[i]:SetActive(true)
            self.CellList[i]:SetData(Item.Create(show_list[i]))
        else
            self.CellNodeList[i]:SetActive(false)
        end
    end
end

function GiftBoxItem:MoneyPrice()
    return RechargeData.Instance:GoldToMoney(self.data.buy_money)
end

function GiftBoxItem:OnClickBuy()
    if not ServerActivityData.Instance:IsGiftBoxBuyHaveTimes(self.data) then
        PublicPopupCtrl.Instance:Error(self.lang.BuyError2)
        return
    elseif self.BoxPanel:IsOrdering() then
        PublicPopupCtrl.Instance:Error(self.lang.BuyError3)
		return
    end
    self.BoxPanel:SaveMoney(self.data.buy_money)
    local fee = self.data.buy_money / 10
    ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_XIAN_SHI_BAO_HE_NEW,fee,ACTIVITY_TYPE.RAND_GIFT_BOX, 1, self.data.seq, 0, 0)

    self.BtnBuyInter.Interactable = false
    self.BoxPanel:DoOrdering()
end

function GiftBoxItem:TimesStr(label, buy_times, max_times)
    local color = buy_times >= max_times and COLORSTR.Red7 or COLORSTR.Green2
    local times_str = Format(Language.Common.Slash, (max_times - buy_times), max_times)
    local show_str = ColorStr(label, COLORSTR.Red1) .. ColorStr(times_str, color)
    UH.SetText(self.NumText, show_str)
end

function GiftBoxItem:ShowEffect()
    local effect_id = self:GetEffectId(self.data.gift_icon)
    if self.effect_id ~= effect_id then
        if self.cur_effect ~= nil then
            self.EffectTool:Stop(self.cur_effect)
            self.cur_effect = nil
        end
        if effect_id ~= 0 then
            self.cur_effect = self.EffectTool:Play(effect_id)
            if self.data.gift_icon == 1 then
                UH.LocalPosG(self.Effect, Vector3.New(-7, 180, 0))
            elseif self.data.gift_icon == 2 then
                UH.LocalPosG(self.Effect, Vector3.New(-8, 180, 0))
            elseif self.data.gift_icon == 3 then
                UH.LocalPosG(self.Effect, Vector3.New(-10, 180, 0))
            elseif self.data.gift_icon == 4 then
                UH.LocalPosG(self.Effect, Vector3.New(-10, 180, 0))
            elseif self.data.gift_icon == 5 then
                UH.LocalPosG(self.Effect, Vector3.New(-5, 181, 0))
            elseif self.data.gift_icon == 6 then
                UH.LocalPosG(self.Effect, Vector3.New(2, 165, 0))
            end
        end
        self.effect_id = effect_id
    end
end

function GiftBoxItem:GetEffectId(icon_type)
    if icon_type == 1 then
        return 100108155
    else
        return 100108150 + icon_type - 2
    end
    return 0
end
