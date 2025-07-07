YunZeGiftBoxView = YunZeGiftBoxView or DeclareView("YunZeGiftBoxView", "serveractivity/yun_ze_gift_box_view", Mod.YunZe.YunZeGiftBox)
VIEW_DECLARE_LEVEL(YunZeGiftBoxView, ViewLevel.L0)

function YunZeGiftBoxView:LoadCallback(param_t)
    UH.SetText(self.Title, ActivityRandData.GetName(ACTIVITY_TYPE.RAND_YUN_ZE_GIFT_BOX))
end

function YunZeGiftBoxView:OnClickClose()
    ViewMgr:CloseView(YunZeGiftBoxView)
end

YunZeGiftBoxPanel = YunZeGiftBoxPanel or DeclareMono("YunZeGiftBoxPanel", UIWFlushPanel)
function YunZeGiftBoxPanel:YunZeGiftBoxPanel()
    self.cur_page, self.min_page, self.max_page = 1, 1, 1
    self.is_ordering = false
    self.data_cares = {
        {data = YunZeGiftBoxData.Instance.gift_box_data, func = self.onFlush},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
    local function func_delay()
        self.open_timer = nil
        self.ItemAnim.enabled = true
        self.ItemList[1].Effect:SetActive(true)
    end
    self.open_timer = Invoke(func_delay, 0.8)
end

function YunZeGiftBoxPanel:Awake()
	UIWFlushPanel.Awake(self)
    self.Timer:CreateActTime(ACTIVITY_TYPE.RAND_YUN_ZE_GIFT_BOX, TimeType.Type_Time_3, Language.RAUstomizedBox.Txt2)
end

function YunZeGiftBoxPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.open_timer ~= nil then
        TimeHelper:CancelTimer(self.open_timer)
        self.open_timer = nil
    end
    TimeHelper:CancelTimer(self.flush_timer)
    self.flush_timer = nil
end

function YunZeGiftBoxPanel:onFlush()
    self.is_ordering = false
    local base_list = YunZeGiftBoxData.Instance:GetGiftBoxShowList()
    self.first_data = base_list[1]
    self.show_list = DataHelper.FormatList(base_list, 2)
    self.max_page = math.ceil(#self.show_list / 2)
    --晚两帧执行，防止item的UiEffectTool还未初始化
    TimeHelper:CancelTimer(self.flush_timer)
    self.flush_timer = TimeHelper:AddDelayFrameTimer(BindTool.Bind(self.FlushPageItem, self), 2)
    self:FlushFreeGift()
end

function YunZeGiftBoxPanel:FlushPageItem()
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

function YunZeGiftBoxPanel:OnClickPage(param)
    if (param == -1 and self.cur_page == self.min_page) or (param == 1 and self.cur_page == self.max_page) then
        return
    end
    self.cur_page = self.cur_page + param
    self.ItemAnim:SetTrigger("show")
    self:FlushPageItem()
end

function YunZeGiftBoxPanel:DoOrdering()
    self.is_ordering = true
    self:FlushPageItem()
end
function YunZeGiftBoxPanel:IsOrdering()
    return self.is_ordering
end

function YunZeGiftBoxPanel:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_YUN_ZE_GIFT_BOX, self.buy_money)
end

function YunZeGiftBoxPanel:SaveMoney(buy_money)
    self.buy_money = buy_money
end

function YunZeGiftBoxPanel:FlushFreeGift()
    self.FreeGiftNode:SetActive(not YunZeGiftBoxData.Instance:IsFreeGiftFetched())
end

function YunZeGiftBoxPanel:OnClickFreeGift()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_YUN_ZE_GIFT_BOX, 2)
end

YunZeGiftBoxItem = YunZeGiftBoxItem or DeclareMono("YunZeGiftBoxItem", UIWidgetBaseItem)
function YunZeGiftBoxItem:YunZeGiftBoxItem()
    self.lang = Language.RAGiftBox
    self.effect_id = 0
end

function YunZeGiftBoxItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:FlushAllInfo()
end

function YunZeGiftBoxItem:FlushAllInfo()
    UH.SetText(self.NameText, self.data.gift_des)
    if self.data.limit_type == 1 then
        local buy_times = YunZeGiftBoxData.Instance:GetGiftBoxBuyTimes(self.data.seq)
        local max_times = self.data.buy_times
        self:TimesStr(self.lang.Limit1, buy_times, max_times)
    else
        local buy_times = YunZeGiftBoxData.Instance:GetGiftBoxDayBuyTimes(self.data.seq)
        local max_times = self.data.today_times
        self:TimesStr(self.lang.Limit2, buy_times, max_times)
    end
    UH.SetText(self.BtnBuyText, self:MoneyPrice())
    UH.SpriteName(self.MarkImg, "BiaoQian" .. (self.data.gift_mark or 1))
    UH.SetText(self.MarkText, self.data.gift_name)
    UH.SpriteName(self.BoxImg, "Box" .. self.data.gift_icon)

    local have_times = YunZeGiftBoxData.Instance:IsGiftBoxBuyHaveTimes(self.data)
    if have_times then
        self.BtnBuyInter.Interactable = not self.BoxPanel:IsOrdering()
    else
        self.BtnBuyInter.Interactable = false
    end
    self:ShowItems()
    self:ShowEffect()
end

function YunZeGiftBoxItem:ShowItems()
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

function YunZeGiftBoxItem:MoneyPrice()
    return RechargeData.Instance:GoldToMoney(self.data.buy_money)
end

function YunZeGiftBoxItem:OnClickBuy()
    if not YunZeGiftBoxData.Instance:IsGiftBoxBuyHaveTimes(self.data) then
        PublicPopupCtrl.Instance:Error(self.lang.BuyError2)
        return
    elseif self.BoxPanel:IsOrdering() then
        PublicPopupCtrl.Instance:Error(self.lang.BuyError3)
		return
    end
    self.BoxPanel:SaveMoney(self.data.buy_money)
    local fee = self.data.buy_money / 10
    ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_YUN_ZE_GIFT_BOX, fee, ACTIVITY_TYPE.RAND_YUN_ZE_GIFT_BOX, 1, self.data.seq, 0, 0)

    self.BtnBuyInter.Interactable = false
    self.BoxPanel:DoOrdering()
end

function YunZeGiftBoxItem:TimesStr(label, buy_times, max_times)
    local color = buy_times >= max_times and COLORSTR.Red7 or COLORSTR.Green2
    local times_str = Format(Language.Common.Slash, (max_times - buy_times), max_times)
    local show_str = ColorStr(label, COLORSTR.Red1) .. ColorStr(times_str, color)
    UH.SetText(self.NumText, show_str)
end

function YunZeGiftBoxItem:ShowEffect()
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

function YunZeGiftBoxItem:GetEffectId(icon_type)
    if icon_type == 1 then
        return 100108155
    else
        return 100108150 + icon_type - 2
    end
    return 0
end
