YunZeNiceGiftView = YunZeNiceGiftView or DeclareView("YunZeNiceGiftView", "serveractivity/yun_ze/yun_ze_nice_gift", Mod.YunZe.YunZeNiceGift)
YunZeNiceGiftView.OP_TYPE = {
    PLAY = 1,   -- 砸蛋 p1 0银锤，1金锤
    BUY = 2,    -- 买捶捶
}
function YunZeNiceGiftView:OnCloseClick()
    ViewMgr:CloseView(YunZeNiceGiftView)
end


YunZeNiceGiftPanel = YunZeNiceGiftPanel or DeclareMono("YunZeNiceGiftPanel", UIWFlushPanel)
function YunZeNiceGiftPanel:YunZeNiceGiftPanel()
    self.act_type = ACTIVITY_TYPE.RAND_YZ_NICE_GIFT
    self.data = YunZeData.Instance
    self.act_info = self.data.nice_gift_data
    self.act_cfg = self.data:NiceGiftCfg()
    self.data_cares = {
        {data = self.act_info, func = self.FlushPanel, init_call = false},
    }
    self.lang = Language.YunZeNiceGift
end

function YunZeNiceGiftPanel:Awake()
    UIWFlushPanel.Awake(self)
    local num = self:GetGoldHammerNum()
    if num > 0 then
        self:SetHammer(1)
        self.Sel1.isOn = true
    else
        self:SetHammer(0)
        self.Sel2.isOn = true
    end
    UH.SetText(self.ActTimeDesc, string.format(self.lang.Txt1,self:GetActTimeDesc()))
    local rate = self.act_cfg.other[1].rate_added / 100
    rate = ColorStr(string.format("+%s%%", rate), COLORSTR.Green9)
    local rate_desc = self.act_cfg.other[1].desc or self.lang.Txt2
    UH.SetText(self.Tips1, rate_desc .. rate)
    self.times = {}
    self.time_str = ""
    for i,v in pairs(self.act_cfg.other) do
        local h = math.floor(v.time / 100)
        local m = v.time - h * 100
        local value = h * 3600 + m * 60
        table.insert(self.times, value)
        self.time_str = self.time_str .. h .. self.lang.Txt11
        if m > 0 then
            self.time_str = self.time_str .. m .. self.lang.Txt12
        end
        if i ~= #self.act_cfg.other then
            self.time_str = self.time_str .. "/"
        end
    end
    UH.SetText(self.Tips2, string.format(self.lang.Txt3,self.time_str))
    self:SetTime()

    self:FlushPanel()
end

function YunZeNiceGiftPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.Time:CloseCountDownTime()
    TimeHelper:CancelTimer(self.time_ht)
end

function YunZeNiceGiftPanel:FlushPanel()
    UH.SetText(self.Num1, self:GetGoldHammerNum())
    UH.SetText(self.Num2, self.act_info.info.prop_free_num)
end

function YunZeNiceGiftPanel:SetTime()
    local time_num = self:GetTime()
    self.Time:CloseCountDownTime()
    self.Time:SetObjActive(time_num > 0)
    if time_num > 0 then
        self.Time:TotalTime(time_num,TimeType.Type_Time_7,self.lang.Txt4)
        self.Time:SetCallBack(function ()
            self:SetTime()
        end)
    end
end

function YunZeNiceGiftPanel:SetHammer(num)
    self.hammer = num
    local img = num == 1 and "JinChuiChui_Da" or "YinChuiChui_Da"
    UH.SpriteName(self.Hammer, img)
end

-- 金锤锤数量
function YunZeNiceGiftPanel:GetGoldHammerNum()
    local item_id = self.act_cfg.other[1].prop_added
    local num = Item.GetNum(item_id)
    return num
end

function YunZeNiceGiftPanel:GetActTimeDesc()
    local act_type = self.act_type
    if not ActivityData.IsOpen(act_type) then
        return Language.Activity.NotOpenTip
    end
    local act_info = ActivityData.Instance:GetActivityStatusInfo(act_type)
    local open_time = act_info.param_1
    local end_time = act_info.param_2 - 1
    local str1 = os.date(self.lang.Txt5, open_time)
    local str2 = os.date(self.lang.Txt5, end_time)
    return string.format("%s ~ %s", str1, str2)
end

function YunZeNiceGiftPanel:GetTime()
    local cur_time = TimeCtrl.Instance:GetServerTime()
    local date = TimeCtrl.Instance:ChangDataTimeDetail(cur_time)
    local h = date.hour
    local m = date.min
    local s = date.sec
    local time_value = h * 3600 + m * 60 + s
    for i = 1, #self.times do
        local time = self.times[i]
        if time_value < time then
            return time - time_value
        end
    end
    
    local day_end_time = TimeCtrl:ChangDataTime24(cur_time)
    local act_info = ActivityData.Instance:GetActivityStatusInfo(self.act_type)
    local end_time = act_info.param_2 - 1
    if day_end_time > end_time then
        return 0
    else
        return self.times[1] + (24 * 3600) - time_value
    end
end

function YunZeNiceGiftPanel:OnPlayClick()
    if self.egg_ing then
        return
    end
    if self.hammer == 0 and self.act_info.info.prop_free_num < 1 then
        PublicPopupCtrl.Instance:Error(self.lang.Txt9)
        return
    elseif self.hammer == 1 and self:GetGoldHammerNum() < 1 then
        PublicPopupCtrl.Instance:Error(self.lang.Txt10)
        return
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    self.EggAnimation:Play("open_egg")
    TimeHelper:CancelTimer(self.time_ht)
    self.egg_ing = true
    self.time_ht = Invoke(function ()
        self.egg_ing = false
        --LogError("请求了砸蛋",self.act_type, YunZeNiceGiftView.OP_TYPE.PLAY, self.hammer)
        ServerActivityCtrl.Instance:SendActivityReq(self.act_type, YunZeNiceGiftView.OP_TYPE.PLAY, self.hammer)
        self.EggAnimation:Play("reset_egg")
    end, 2)
end

function YunZeNiceGiftPanel:OnOpenPreviewClick()
    self.Preview:SetActive(true)
    if not self.is_set_preview then
        self.is_set_preview = true
        local list_data = self.data:GetNiceRewardList()
        self.RewardList:SetData(list_data)
    end
end

function YunZeNiceGiftPanel:OnClosePreviewClick()
    self.Preview:SetActive(false)
end

function YunZeNiceGiftPanel:OnHammerClick(value)
    self:SetHammer(value)
end

function YunZeNiceGiftPanel:OnOpenBuyClick(value)
    if value == 0 then
        local str = string.format(self.lang.Txt6,self.time_str)
        PublicPopupCtrl.Instance:Center(str)
    elseif value == 1 then
        ViewMgr:OpenView(YunZeNiceGiftBuyDialog)
    end
end

YunZeNiceGiftRewardItem = YunZeNiceGiftRewardItem or DeclareMono("YunZeNiceGiftRewardItem", UIWidgetBaseItem)
-- data:Config.yunzehaoli_auto.reward
function YunZeNiceGiftRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create(data.reward_item)
    self.ItemCell:SetData(item)
    local desc = data.rate_desc / 100
    UH.SetText(self.Desc, desc .. "%")
end

YunZeNiceGiftBuyDialog = YunZeNiceGiftBuyDialog or DeclareView("YunZeNiceGiftBuyDialog", "serveractivity/yun_ze/yun_ze_nice_gift_buy_dialog")
function YunZeNiceGiftBuyDialog:YunZeNiceGiftBuyDialog()
    self.Curr:DefaultSet()
end

function YunZeNiceGiftBuyDialog:OnCloseClick()
    ViewMgr:CloseView(YunZeNiceGiftBuyDialog)
end

YunZeNiceGiftBuyPanel = YunZeNiceGiftBuyPanel or DeclareMono("YunZeNiceGiftBuyPanel", UIWFlushPanel)
function YunZeNiceGiftBuyPanel:YunZeNiceGiftBuyPanel()
    self.act_type = ACTIVITY_TYPE.RAND_YZ_NICE_GIFT
    self.data = YunZeData.Instance
    self.act_info = self.data.nice_gift_data
    self.cfg = self.data:NiceGiftCfg().other[1]
    self.data_cares = {
        {data = self.data.nice_gift_data, func = self.FlushPanel, init_call = false},
    }
end

function YunZeNiceGiftBuyPanel:Awake()
    UIWFlushPanel.Awake(self)
    local item = Item.Create(self.cfg.reward_item)
    UH.SetText(self.ItemName, item:Name())
    self.ItemCell:SetData(item)
    UH.SpriteName(self.CurrencyImg, HuoBi[self.cfg.price_type])
    UH.SetText(self.PriceText, self.cfg.price)
    self:FlushPanel()
end

function YunZeNiceGiftBuyPanel:FlushPanel()
    local str
    local max_num = self.cfg.quantity
    local cur_num = self.act_info.info.buy_count
    if self.cfg.buy_type == 1 then
        str = string.format(Language.YunZeNiceGift.Txt7, cur_num, max_num)
    else
        str = string.format(Language.YunZeNiceGift.Txt8, cur_num, max_num)
    end
    UH.SetText(self.BuyLimitText, str)
end

function YunZeNiceGiftBuyPanel:OnBuyClick()
    ServerActivityCtrl.Instance:SendActivityReq(self.act_type, YunZeNiceGiftView.OP_TYPE.BUY)
end