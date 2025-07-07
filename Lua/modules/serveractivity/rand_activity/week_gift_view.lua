WeekGiftView = WeekGiftView or DeclareView("WeekGiftView","serveractivity/week_gift_view",Mod.RandActivity.WeekGift)

VIEW_DECLARE_LEVEL(WeekGiftView, ViewLevel.L1)

function WeekGiftView:WeekGiftView()
    self.data = ServerActivityData.Instance
    self.care_handle = self.data:GetCmdBuyItemCheckRet():Care(BindTool.Bind(self.SendBuy,self))
    self.handle_care = self.data.week_gift_data:Care(BindTool.Bind(self.OnFlush,self))
    self.make_num = 1
    self.Time:SetCallBack(function()
        ViewMgr:CloseView(WeekGiftView)
        self.data.week_gift_data.val = not self.data.week_gift_data.val
    end)
end

function WeekGiftView:CloseCallback()
	self.data:GetCmdBuyItemCheckRet():Uncare(self.care_handle)
	self.data.week_gift_data:Uncare(self.handle_care)
	self.care_handle = nil
	self.handle_care = nil
end

function WeekGiftView:LoadCallback(param_t)
    self.info_list = self.data:GetWeedendGiftInfo()
    self.max_num = #self.info_list
    -- 打开需要显示最后一个的情况
    if param_t.show_last then
        self.make_num = self.max_num
    end
    self:FlushBtnView()
    self:FlushInfoView()
    if self.max_num == 1 and self.data:IsHasExclusiveGift() then
        self.BtnRight:SetActive(true)
    end
end

function WeekGiftView:OnClickNum(num)
    if self.max_num == 1 and self.data:IsHasExclusiveGift() then
        ViewMgr:OpenViewByKey(Mod.RandActivity.ExclusiveGift)
        ViewMgr:CloseView(WeekGiftView)
        return
    end
    self.make_num = self.make_num + num
    self:FlushBtnView()
    self:FlushInfoView()
end

function WeekGiftView:OnFlush()
    self.info_list = self.data:GetWeedendGiftInfo()
    self:FlushInfoView()
end

function WeekGiftView:FlushBtnView()
    self.BtnLeft:SetActive(self.make_num > 1)
    self.BtnRight:SetActive(self.make_num < self.max_num)
end

function WeekGiftView:FlushInfoView()
    self.select_data = self.info_list[self.make_num] or {}
    self:FlushItemView(self.select_data)
end

function WeekGiftView:FlushItemView(data)
    local config = self.data:GetWeedendGiftData(data.index)
    if config ~= nil then
        self.buy_money = config.buy_money
        self.List:SetData(config.reward_item)
        UH.SetText(self.Price,self.buy_money / 10)
        UH.SetText(self.OldPrice,Format(Language.WeekGift.Price,RechargeData.Instance:GetLocalSign() .. config.price_des))
        self.Time:StampTime(data.end_time,TimeType.Type_Special_9,Language.WeekGift.TimeTip)
        UH.SetInter(self.BtnInter,data.times < config.limit_num)
        self.Effect:SetActive(data.times < config.limit_num)
    end
end

function WeekGiftView:OnClickPurchase()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RANK_WEEK_GIFT, 0, self.select_data.index)
end

function WeekGiftView:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_WEEK_GIFT, self.buy_money)
    ViewMgr:CloseView(WeekGiftView)
end

function WeekGiftView:OnClickClose()
    ViewMgr:CloseView(WeekGiftView)
end