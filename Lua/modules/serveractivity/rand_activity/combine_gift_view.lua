CombineGiftView = CombineGiftView or DeclareView("CombineGiftView", "serveractivity/labour_day/combine_gift_view", Mod.LabourDay.CombineGift)
VIEW_DECLARE_LEVEL(CombineGiftView, ViewLevel.L1)
VIEW_DECLARE_MASK(CombineGiftView, ViewMask.BgBlock)

function CombineGiftView:OpenCallback()
    -- ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_COMBINE_GIFT, 0, 0)
    if ServerActivityData.Instance:GetCombineGiftRemind() == 1 then
        ServerActivityData.Instance:SetCombineGiftRemind()
    end
end

function CombineGiftView:OnClickClose()
    ViewMgr:CloseView(CombineGiftView)
end

--[[1.不可同时选中同一礼包两次
    2.取消选中是点击右边的礼包取消。如果右边选中两个礼包了，如果没有取消选中礼包，再点击左边礼包则默认替换最后选择的礼包
    3.货币类型只有元宝或直购，不会出现两种货币购买方式同时存在的情况 (0元宝3直购)
]]
CombineGiftPanel = CombineGiftPanel or DeclareMono("CombineGiftPanel", UIWFlushPanel)
function CombineGiftPanel:CombineGiftPanel()
    self.lang = Language.CombineGift
    self.cur_tab = 1
    self.select_item = {}
    self.select_data = {}
    self.do_open_show = true --是否动画
    self.do_open_time = 0.5 --动画时间

    self.data_cares = {
        {data = ServerActivityData.Instance.combine_gift_data, func = self.onFlush},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false}
    }

    self.NextTime:SetShowCallBack(function(t)
        return Format(self.lang.Str1, t.day, t.hour, t.min)
    end)
    self.buy_info = nil
end

function CombineGiftPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.list_timer ~= nil then
        TimeHelper:CancelTimer(self.list_timer)
        self.list_timer = nil
    end
    if self.block_timer ~= nil then
        TimeHelper:CancelTimer(self.block_timer)
        self.block_timer = nil
    end
end

function CombineGiftPanel:onFlush()
    self:FlushTabs()
    self.TopBlock:SetActive(false)
    local phase = ServerActivityData.Instance:GetCombineGiftPhase()
    if self.cur_tab == 1 or self.cur_tab == 3 then
        if self.cur_tab == 1 then
            self.show_list = ServerActivityData.Instance:GetCombineGiftShowListByPrase(phase)
        else
            self.show_list = ServerActivityData.Instance:GetCombineGiftShowListByPrase(phase - 1)
        end
        for i, v in ipairs(self.show_list) do
            v.list_index = i
        end
        self.GiftList:SetData(self.show_list)
        if self.do_open_show then
            self:DoOpenShow()
        end
        self:ResetSelectList()
    else
        self:JudgeHaveNext()
    end
end

function CombineGiftPanel:OnSelectToggle(param)
    self.cur_tab = param
    local index = self.cur_tab ~= 2 and 1 or 2
    for i = 1, 2 do
        self.PanelList[i]:SetActive(index == i)
    end
    self.Tab1Part:SetActive(index == 1)

    self.do_open_show = self.cur_tab ~= 2
    self.do_open_time = 0.2
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
    self:onFlush()
end

function CombineGiftPanel:OnClickBuy()
    if self.cur_tab ~= 1 or table.nums(self.select_data) == 0 then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        return
    end
    if self.show_list == nil or self.show_list[1] == nil then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        return
    end
    local price_type = self.show_list[1].price_type
    local select_num = table.nums(self.select_data)
    local cur_phase = ServerActivityData.Instance:GetCombineGiftPhase()

    local show_val = 0
    local bit_t = bit:d64b(0)
    for i = 1, 2 do
        if self.select_data[i] ~= nil then
            show_val = show_val + self.select_data[i].price
            bit_t[64 - self.select_data[i].type] = 1
        end
    end
    local param1 = bit:b64d(bit_t)

    if select_num == 2 then
        local off_cfg = ServerActivityData.Instance:GetCombineGiftOff(cur_phase)
        show_val = math.floor(show_val * (off_cfg.discount / 100))
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    if price_type == 3 then
        self.buy_info = {money = show_val, param1 = BUY_ITEM_TYPE.TYPE_COMBINE_GIFT_2}
        -- [2,订单效验], [3,取消订单]
        -- ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_COMBINE_GIFT, 3, 0, 0, 0)
        local fee = self.buy_info.money / 10
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_COMBINE_GIFT_2,fee,ACTIVITY_TYPE.RAND_COMBINE_GIFT, 2, param1, 0, 0)
    elseif price_type == 0 then
        if not MallCtrl.IsNotCurrency(CurrencyType.Gold, show_val) then
            return
        end
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_COMBINE_GIFT, 1, param1, 0, 0)
    end
    self.BtnBuyInter.Interactable = false
    --防止请求太快
    self.TopBlock:SetActive(true)
    local function func_delay()
        self.TopBlock:SetActive(false)
        self.BtnBuyInter.Interactable = true
    end
    self.block_timer = Invoke(func_delay, 1.5)
end

function CombineGiftPanel:SendBuy()
    if self.buy_info == nil then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(self.buy_info.param1, self.buy_info.money)
end

function CombineGiftPanel:CurTab()
    return self.cur_tab
end

function CombineGiftPanel:OnSelectChange()
    self:FlushPrice()
end

function CombineGiftPanel:FlushTabs()
    local cur_phase = ServerActivityData.Instance:GetCombineGiftPhase()
    local have_next = cur_phase < ServerActivityData.Instance:GetCombineGiftMaxPhase()
    local have_last = cur_phase > 1
    self.BtnToggleList[2].gameObject:SetActive(have_next)
    self.BtnToggleList[3].gameObject:SetActive(have_last)
    if (not have_next and self.BtnToggleList[2].isOn) or (not have_last and self.BtnToggleList[3].isOn) then
        self.BtnToggleList[1].isOn = true
    end
    local num = 1
    if have_next then
        num = num + 1
    end
    if have_last then
        num = num + 1
    end
    self.LineRect.sizeDelta = Vector2.New(5, 157 * num)
end

-- 3.货币类型只有元宝或直购，不会出现两种货币购买方式同时存在的情况 (0元宝3直购)
function CombineGiftPanel:FlushPrice()
    if self.cur_tab ~= 1 and self.cur_tab ~= 3 then
        return
    end
    if self.show_list == nil or self.show_list[1] == nil then
        return
    end
    for i = 1, 2 do
        self.SelectList[i]:SetData(self.select_data[i])
    end
    local select_num = table.nums(self.select_data)
    local price_type = self.show_list[1].price_type
    local prefix = ""
    if price_type == 3 then
        self.PriceIcon.gameObject:SetActive(false)
        self.BtnBuyIcon.gameObject:SetActive(false)
        prefix = RechargeData.Instance:GetLocalSign()
    else
        self.PriceIcon.gameObject:SetActive(true)
        UH.SetGold(self.PriceIcon, price_type)
        self.BtnBuyIcon.gameObject:SetActive(select_num > 0)
        UH.SetGold(self.BtnBuyIcon, price_type)
    end
    if self.BtnBuyIcon.gameObject.activeSelf then
        UH.LocalPosG(self.BtnBuyText.gameObject, Vector3.New(-15, 0, 0))
    else
        UH.LocalPosG(self.BtnBuyText.gameObject, Vector3.New(0, 0, 0))
    end

    local cur_phase = ServerActivityData.Instance:GetCombineGiftPhase()

    self.PriceList[4]:SetActive(select_num == 0)
    self.PriceList[3]:SetActive(select_num == 2)
    self.PriceList[2]:SetActive(select_num > 0)
    self.PriceList[1]:SetActive(select_num > 0)

    if select_num == 0 then --未选择，显示0
        self.BtnBuyInter.Interactable = false
        UH.SetText(self.BtnBuyText, self.lang.GouMai)
        self.OffNode:SetActive(false)
    else
        self.BtnBuyInter.Interactable = self.cur_tab ~= 3
        local phase = self.cur_tab == 3 and cur_phase - 1 or cur_phase

        local off_value = 100
        if select_num == 2 then
            local off_cfg = ServerActivityData.Instance:GetCombineGiftOff(phase)
            self.OffNode:SetActive(true)
            off_value = off_cfg.discount
            UH.SetText(self.OffText, Format(self.lang.Discount, off_cfg.discount / 10))
        else
            self.OffNode:SetActive(false)
        end

        local show_val = 0
        for i = 1, 2 do
            if self.select_data[i] ~= nil then
                show_val = show_val + self.select_data[i].price
            end
        end
        if price_type == 3 then
            show_val = self:MoneyPrice(show_val)
        end
        UH.SetText(self.PriceValue, prefix .. show_val)
        local off_value = math.floor(show_val * (off_value / 100))
        if select_num == 1 then
            UH.SetText(self.BtnBuyText, prefix .. off_value)
        else
            UH.SetText(self.BtnBuyText, self.lang.CombinePrice .. prefix .. off_value)
        end
    end
end

function CombineGiftPanel:MoneyPrice(value)
    return math.floor(value / 10)
end

function CombineGiftPanel:ResetSelectList()
    for i, v in pairs(self.select_item) do
        v:IsSelect(false)
    end
    self.select_data = {}
    self.select_item = {}
    self:OnSelectChange()
end
function CombineGiftPanel:PushList(item)
    if self.cur_tab == 1 then
        --不让选：购买次数不足，已选中
        local remain_times = ServerActivityData.Instance:GetCombineGiftRemainBuyTimes(item:GetData())
        if remain_times <= 0 then
            return
        end
    end
    for i, v in pairs(self.select_data) do
        if v.seq == item:GetData().seq then
            self:PopList(i)
            return
        end
    end
    if #self.select_item < 2 then
        table.insert(self.select_item, item)
        table.insert(self.select_data, item:GetData())
        item:IsSelect(true)
    else
        if self.select_item[2] then
            self.select_item[2]:IsSelect(false)
        end
        self.select_item[2] = item
        self.select_data[2] = item:GetData()
        item:IsSelect(true)
    end
    self:OnSelectChange()
end
function CombineGiftPanel:PopList(index)
    if self.select_item[index] ~= nil then
        self.select_item[index]:IsSelect(false)
        table.remove(self.select_item, index)
        table.remove(self.select_data, index)
    end
    self:OnSelectChange()
end

-- 判断是否还有下期
function CombineGiftPanel:JudgeHaveNext()
    self.NextTime:CloseCountDownTime()
    local phase = ServerActivityData.Instance:GetCombineGiftPhase()
    if phase < ServerActivityData.Instance:GetCombineGiftMaxPhase() then
        local show_list = ServerActivityData.Instance:GetCombineGiftShowListByPrase(phase)
        local now_date = os.date("*t", TimeManager.GetServerTime())
        local open_day = ServerActivityData.Instance:GetCombineGiftOpenDay()
        now_date.day = now_date.day + show_list[1].section_end - open_day + 1
        local time = TimeCtrl.Instance:ChangDataStampTime(now_date)
        self.NextTime:StampTime(time, TimeType.Type_Time_3)
    end
end

function CombineGiftPanel:DoOpenShow()
    if self.list_timer ~= nil then
        TimeHelper:CancelTimer(self.list_timer)
    end
    self.ScrollRect:StopMovement()
    self.GiftList:ForceJumpVertical(0)
    local function func_delay()
        self.do_open_show = false
        self.ListAnim:Stop()
        self.ListAnim:Play()
    end
    self.list_timer = Invoke(func_delay, self.do_open_time)
end

-------------------------商品列表
CombineGiftItem = CombineGiftItem or DeclareMono("CombineGiftItem", UIWidgetBaseItem)
function CombineGiftItem:CombineGiftItem()
    self.lang = Language.CombineGift
end

function CombineGiftItem:SetName()
    self.gameObject.name = "item" .. self.data.list_index --self.gameObject:GetSiblingIndex()
end

function CombineGiftItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:SetName()
    if self.BasePanel.do_open_show and data.list_index <= 3 then
        UH.LocalPos(self.Anim, Vector3.New(0, -500, 0))
    end
    self:ShowBase()
    self:ShowItem()
end

function CombineGiftItem:ShowBase()
    UH.SetText(self.Name, self.data.gift_des)

    local max_times = self.data.limit_type == 1 and self.data.buy_times or self.data.today_times
    local remain_times = max_times
    if self.BasePanel:CurTab() == 1 then
        remain_times = ServerActivityData.Instance:GetCombineGiftRemainBuyTimes(self.data)
    end
    local color = remain_times <= 0 and COLORSTR.Red7 or COLORSTR.Green2
    local times_str = Format(Language.Common.Slash, ColorStr(remain_times, color), max_times)
    local show_str = ColorStr(self.lang.Limit[self.data.limit_type], COLORSTR.Red1) .. times_str
    local is_over = remain_times <= 0
    UH.SetText(self.Limit, show_str)
    self.Over:SetActive(is_over)
    self.Limit.gameObject:SetActive(not is_over)
end
function CombineGiftItem:ShowItem()
    local config = BagData.Instance:GetGiftItemList(self.data.reward_item.item_id)
    local show_list = {}
    for i = 1, #config.item_list do
        table.insert(show_list, {index = i, item = config.item_list[i]})
    end
    self.ItemList:SetData(show_list)
end

function CombineGiftItem:OnClick()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    self.BasePanel:PushList(self)
end

function CombineGiftItem:IsSelect(is_select)
    self.Gou:SetActive(is_select)
end

-------------------------物品小展示
CombineGiftCell = CombineGiftCell or DeclareMono("CombineGiftCell", UIWidgetBaseItem)
function CombineGiftCell:CombineGiftCell()
end
function CombineGiftCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Icon:SetActive(data.index ~= 1)
    self.ItemCell:SetData(Item.Create(data.item))
end

-------------------------选中物品
CombineGiftSelectItem = CombineGiftSelectItem or DeclareMono("CombineGiftSelectItem", UIWidgetBaseItem)
function CombineGiftSelectItem:CombineGiftSelectItem()
    self.lang = Language.CombineGift
end

function CombineGiftSelectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local have_item = self.data ~= nil
    self.GiftNode:SetActive(have_item)
    self:ChangeBGAnim(not have_item)
    if have_item then
        UH.SetText(self.Name, self.data.gift_des)
    end
end

function CombineGiftSelectItem:OnClick()
    if self.data ~= nil then
        self.BasePanel:PopList(self.Index)
    end
end

function CombineGiftSelectItem:ChangeBGAnim(is_play)
    self.BGAnim.enabled = is_play
    if not is_play then
        self.BGAnim.gameObject.transform.localRotation = Vector3.zero
    end
end
