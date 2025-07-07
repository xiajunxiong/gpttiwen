YunZeChainBoxView = YunZeChainBoxView or DeclareView("YunZeChainBoxView", "serveractivity/yun_ze_chain_box_view", Mod.YunZe.YunZeChainBox)
VIEW_DECLARE_LEVEL(YunZeChainBoxView, ViewLevel.L0)

function YunZeChainBoxView:LoadCallback(param_t)
    UH.SetText(self.Title, ActivityRandData.GetName(ACTIVITY_TYPE.RAND_YUN_ZE_CHAIN_BOX))
end

function YunZeChainBoxView:OnClickClose()
    ViewMgr:CloseView(YunZeChainBoxView)
end

YunZeChainBoxPanel = YunZeChainBoxPanel or DeclareMono("YunZeChainBoxPanel", UIWFlushPanel)
function YunZeChainBoxPanel:YunZeChainBoxPanel()
    self.cur_page, self.min_page, self.max_page = 1, 1, 1
    self.is_ordering = false
    self.data_cares = {
        {data = YunZeChainBoxData.Instance.chain_box_data, func = self.onFlush},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false}
    }
    local function func_delay()
        self.open_timer = nil
        self.ItemAnim.enabled = true
    end
    self.open_timer = Invoke(func_delay, 0.8)
end

function YunZeChainBoxPanel:Awake()
	UIWFlushPanel.Awake(self)
    self.Timer:CreateActTime(ACTIVITY_TYPE.RAND_YUN_ZE_CHAIN_BOX, TimeType.Type_Time_3, Language.RAUstomizedBox.Txt2)
end

function YunZeChainBoxPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.open_timer ~= nil then
        TimeHelper:CancelTimer(self.open_timer)
        self.open_timer = nil
    end
    TimeHelper:CancelTimer(self.flush_timer)
    self.flush_timer = nil
end

function YunZeChainBoxPanel:onFlush()
    self.is_ordering = false
    local base_list = YunZeChainBoxData.Instance:GetChainBoxShowList()
    self.first_data = base_list[1]
    self.show_list = DataHelper.FormatList(base_list, 2)
    self.max_page = math.ceil(#self.show_list / 2)
    --晚两帧执行，防止item的UiEffectTool还未初始化
    TimeHelper:CancelTimer(self.flush_timer)
    self.flush_timer = TimeHelper:AddDelayFrameTimer(BindTool.Bind(self.FlushPageItem, self), 2)
    self:FlushFreeGift()
end

function YunZeChainBoxPanel:FlushPageItem()
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

function YunZeChainBoxPanel:OnClickPage(param)
    if (param == -1 and self.cur_page == self.min_page) or (param == 1 and self.cur_page == self.max_page) then
        return
    end
    self.cur_page = self.cur_page + param
    self.ItemAnim:SetTrigger("show")
    self:FlushPageItem()
end

function YunZeChainBoxPanel:OpenShowView(data)
    local show_list = DataHelper.FormatList(data.reward_item)
    if data.gold_num ~= 0 then
        table.insert(show_list, Item.Init(HuoBi[CurrencyType.Gold], data.gold_num))
    end
	
	local count = #show_list
	local image_size = 748
	local offset = 0
	local list_size = 720
	local scroll_enable = count >= 7
	if count < 7 and count > 3 then
		image_size = 600
		list_size = 600
		offset = 15
	end
	if count < 4 then
		image_size = 320
		list_size = 320
		offset = 20
	end
	self.ImageRect:SetSizeWithCurrentAnchors(0, image_size)
	self.ScrollRect.enabled = scroll_enable
	self.SelectListRect:SetSizeWithCurrentAnchors(0,list_size)
	UH.LocalPosG(self.SelectListRect, Vector3.New(offset, -4, 0))
	
    self.ShowListView:SetData(show_list)
    self.ShowNode:SetActive(true)
end

function YunZeChainBoxPanel:CloseShowView()
    self.ShowNode:SetActive(false)
end

function YunZeChainBoxPanel:DoOrdering()
    self.is_ordering = true
    self:FlushPageItem()
end
function YunZeChainBoxPanel:IsOrdering()
    return self.is_ordering
end

function YunZeChainBoxPanel:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_YUN_ZE_CHAIN_BOX, self.buy_money)
end

function YunZeChainBoxPanel:SaveMoney(buy_money)
    self.buy_money = buy_money
end

function YunZeChainBoxPanel:FlushFreeGift()
    self.FreeGiftNode:SetActive(not YunZeChainBoxData.Instance:IsFreeGiftFetched())
end

function YunZeChainBoxPanel:OnClickFreeGift()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_YUN_ZE_CHAIN_BOX, 2)
end

-----商品列表
YunZeChainBoxItem = YunZeChainBoxItem or DeclareMono("YunZeChainBoxItem", UIWidgetBaseItem)
function YunZeChainBoxItem:YunZeChainBoxItem()
    self.lang = Language.RAChainBox
    self.effect_id = 0
    self.open_effect = nil
    self.effect_timer = nil
end

function YunZeChainBoxItem:OnDestroy()
    if self.effect_timer ~= nil then
        TimeHelper:CancelTimer(self.effect_timer)
        self.effect_timer = nil
    end
end

function YunZeChainBoxItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:FlushAllInfo()
end

function YunZeChainBoxItem:FlushAllInfo()
    UH.SetText(self.NameText, self.data.gift_des)
    if self.data.limit_type == 1 then
        local buy_times = YunZeChainBoxData.Instance:GetChainBoxBuyTimes(self.data.seq)
        local max_times = self.data.buy_times
        self:TimesStr(self.lang.Limit1, buy_times, max_times)
    else
        local buy_times = YunZeChainBoxData.Instance:GetChainBoxDayBuyTimes(self.data.seq)
        local max_times = self.data.today_times
        self:TimesStr(self.lang.Limit2, buy_times, max_times)
    end
    UH.SetText(self.BtnBuyText, self:MoneyPrice())
    UH.SpriteName(self.MarkImg, "BiaoQian" .. (self.data.gift_mark or 1))
    UH.SetText(self.MarkText, self.data.gift_name)

    local is_open = YunZeChainBoxData.Instance:GetChainBoxBuyIsOpen(self.data.seq)
    self.OpenNode:SetActive(is_open)
    self.CloseNode:SetActive(not is_open)
    if self.MaskObj ~= nil then
        self.MaskObj:SetActive(not is_open)
    end
    if is_open then
        local have_times = YunZeChainBoxData.Instance:IsChainBoxBuyHaveTimes(self.data)
        if have_times then
            self.BtnBuyInter.Interactable = not self.BoxPanel:IsOrdering()
        else
            self.BtnBuyInter.Interactable = false
        end
        if self.open_effect == nil and self.effect_timer == nil then
			self.effect_timer = TimeHelper:AddDelayTimer(BindTool.Bind(self.PlayOpenEffect, self), 0.3)
        end
        -- self.BGInter.Interactable = true
    else
        UH.SpriteName(self.BoxImg, "BoxClose" .. self.data.gift_icon)
        self.BtnBuyInter.Interactable = false
        -- self.BGInter.Interactable = false
    end
    if is_open then
        self:ShowItems()
    end
    if not is_open then
        self:ShowEffect()
    end
end

function YunZeChainBoxItem:PlayOpenEffect()
    self.open_effect = self.EffectTool:Play(100108162)
end

function YunZeChainBoxItem:ShowItems()
    local show_list = {}
    for i = 0, #self.data.reward_item do
        table.insert(show_list, self.data.reward_item[i])
    end
    if self.data.gold_num ~= 0 then
        table.insert(show_list, Item.Init(HuoBi[CurrencyType.Gold], self.data.gold_num))
    end
    for i = 1, 5 do
        if show_list[i] ~= nil then
            self.ItemNodeList[i]:SetActive(true)
            self.CellNodeList[i]:SetActive(true)
            self.CellList[i]:SetData(Item.Create(show_list[i]))
        else
            self.ItemNodeList[i]:SetActive(false)
        end
    end
end

function YunZeChainBoxItem:MoneyPrice()
    return RechargeData.Instance:GoldToMoney(self.data.buy_money)
end

function YunZeChainBoxItem:OnClickBuy()
    local is_open = YunZeChainBoxData.Instance:GetChainBoxBuyIsOpen(self.data.seq)
    if not is_open then
        PublicPopupCtrl.Instance:Center(self.lang.BuyError1)
        return
    elseif not YunZeChainBoxData.Instance:IsChainBoxBuyHaveTimes(self.data) then
        PublicPopupCtrl.Instance:Center(self.lang.BuyError2)
        return
    elseif self.BoxPanel:IsOrdering() then
        PublicPopupCtrl.Instance:Error(self.lang.BuyError3)
		return
    else
        self.BoxPanel:SaveMoney(self.data.buy_money)
        local fee = self.data.buy_money / 10
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_YUN_ZE_CHAIN_BOX, fee, ACTIVITY_TYPE.RAND_YUN_ZE_CHAIN_BOX, 1, self.data.seq, 0, 0)
        self.BtnBuyInter.Interactable = false
        self.BoxPanel:DoOrdering()
    end
end

function YunZeChainBoxItem:TimesStr(label, buy_times, max_times)
    local color = buy_times >= max_times and COLORSTR.Red7 or COLORSTR.Green2
    local times_str = Format(Language.Common.Slash, (max_times - buy_times), max_times)
    local show_str = ColorStr(label, COLORSTR.Red1) .. ColorStr(times_str, color)
    UH.SetText(self.NumText, show_str)
end

function YunZeChainBoxItem:ShowEffect()
    local effect_id = self:GetEffectId(self.data.gift_icon)
    if self.effect_id ~= effect_id then
        if self.cur_effect ~= nil then
            self.EffectTool:Stop(self.cur_effect)
            self.cur_effect = nil
        end
        if effect_id ~= 0 then
            self.cur_effect = self.EffectTool:Play(effect_id)
        end
        self.effect_id = effect_id
    end
end

function YunZeChainBoxItem:GetEffectId(icon_type)
    return 100108156 + icon_type - 1
end

-----预览展示
YunZeChainBoxShowItem = YunZeChainBoxShowItem or DeclareMono("YunZeChainBoxShowItem", UIWidgetBaseItem)
function YunZeChainBoxShowItem:YunZeChainBoxShowItem()
    self.lang = Language.RAChainBox
end

function YunZeChainBoxShowItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:FlushAllInfo()
end

function YunZeChainBoxShowItem:FlushAllInfo()
    local item = Item.Create(self.data)
    self.Cell:SetData(item)
    UH.SetText(self.NameText, ColorStr(item:Name(), item:ColorStr()))
    UH.SetText(self.NumText, self.data.num)
end
