
ChaoZhiGiftView = ChaoZhiGiftView or DeclareView("ChaoZhiGiftView","serveractivity/chaozhi_gift", Mod.RandActivity.ChaoZhiGift)
VIEW_DECLARE_LEVEL(ChaoZhiGiftView, ViewLevel.L1)

function ChaoZhiGiftView:LoadCallback()
end

function ChaoZhiGiftView:OnClickClose()
    ViewMgr:CloseView(ChaoZhiGiftView)
end

ChaoZhiGiftViewF = ChaoZhiGiftViewF or DeclareMono("ChaoZhiGiftViewF", UIWFlushPanel)

function ChaoZhiGiftViewF:ChaoZhiGiftViewF()
    self.data = ServerActivityCtrl.Instance:Data()
    self.act_data = ActivityData.Instance
    self.language = Language.ChaoZhiGift

    self.data_cares = {
        {data = self.data.chaozhi_gift_info, func = self.FlushInfo},
        {data = self.act_data.activity_status_event, func = self.FlushState},
        {data = self.data:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false}
    }
end

function ChaoZhiGiftViewF:Awake()
    UIWFlushPanel.Awake(self)

    self:FlushRemainTime()
end

function ChaoZhiGiftViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_ct)
end

function ChaoZhiGiftViewF:FlushInfo()
    local info = self.data.chaozhi_gift_info
    self.BtnBuyObj:SetActive(0 == info.gift_act_flag)
    self.PriceShow.text = self.data:GetChaoZhiGiftPrice() / 10
    
    local show_list = self.data:GetChaoZhiGiftShowList()
    self.TipsShow.text = string.format(self.language.TipsShow, #show_list)
    self.ShowItem:SetData(show_list[#show_list])
    show_list[#show_list] = nil
    self.ShowList:SetData(show_list)
end

function ChaoZhiGiftViewF:FlushState()
    if self.act_data.activity_status_event.act_type ~= ACTIVITY_TYPE.RANK_CHAOZHI_GIFT then return end
    local activity_status = self.act_data:GetActivityStatusInfo(ACTIVITY_TYPE.RANK_CHAOZHI_GIFT)
    local status = activity_status and activity_status.status or ActStatusType.Close
    if ActStatusType.Open == status then
        self:FlushRemainTime()
    else
        ViewMgr:CloseView(ChaoZhiGiftView)
    end
end

function ChaoZhiGiftViewF:FlushRemainTime()
    TimeHelper:CancelTimer(self.timer_ct)
    local activity_status = self.act_data:GetActivityStatusInfo(ACTIVITY_TYPE.RANK_CHAOZHI_GIFT)
    local end_time = activity_status and activity_status.param_2 or 0
    local start_time = activity_status and activity_status.param_1 or 0
    local buy_times = self.data:GetChaoZhiGiftOtherBuyTimes()
    if end_time - TimeHelper.GetServerTime() <= 0 then
        self.RemainTime.text = string.format(self.language.RemainTime, 0, 0, 0)
        return
    end

    self.timer_ct = TimeHelper:AddCountDownCT(function ()
        local buy_remain = start_time + buy_times * 86400 - TimeHelper.GetServerTime()
        local last_time = buy_remain > 0 and buy_remain or (end_time - TimeHelper.GetServerTime())
        local ft = TimeHelper.FormatDHMS(last_time)
        self.RemainTime.text = string.format(buy_remain > 0 and self.language.RemainTime2 or self.language.RemainTime, ft.day, ft.hour, ft.min)
    end, function ()
        self.RemainTime.text = string.format(self.language.RemainTime, 0, 0, 0)
    end, end_time, 1)
end

function ChaoZhiGiftViewF:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_CHAOZHI_GIFT, self.data:GetChaoZhiGiftPrice())
end

function ChaoZhiGiftViewF:OnClickBuy()
    ServerActivityCtrl.Instance:SendActivityChaoZhiGiftReqBuy()
end

ChaoZhiGiftViewItem = DeclareMono("ChaoZhiGiftViewItem", UIWidgetBaseItem)

function ChaoZhiGiftViewItem:ChaoZhiGiftViewItem()
    self.act_data = ServerActivityCtrl.Instance:Data()
    self.langauge = Language.ChaoZhiGift
end

function ChaoZhiGiftViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    
    local open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RANK_CHAOZHI_GIFT)
    local info = self.act_data.chaozhi_gift_info
    local is_get = info.gift_num_list[data.active_times] > 0
    self.DayShow.text = string.format(self.langauge.DayShow, DataHelper.GetDaXie(data.active_times))
    self.ShowCell:SetData(Item.Init(data.item_id, 1, data.is_bind))
    self.BtnGetInter:SetObjActive(not is_get)
    self.BtnGetInter.Interactable = (not is_get) and (open_day >= data.active_times)
    self.GetedObj:SetActive(is_get)
    if is_get then
        self.NumShow.text = string.format(self.langauge.NumShow1, info.gift_num_list[data.active_times])
    else
        if data.num_down == data.num_up then
            self.NumShow.text = string.format(self.langauge.NumShow1, data.num_down)
        else
            self.NumShow.text = string.format(self.langauge.NumShow2, data.num_down, data.num_up)
        end
    end
    self.EffectItem:SetActive((1 == info.gift_act_flag) and (not is_get) and (open_day >= data.active_times))
    self.red_point_item = self.red_point_item or UiRedPoint.New(self.BtnGetInter.gameObject, Vector3.New(50,15,0))
    self.red_point_item:SetNum(((1 == info.gift_act_flag) and (not is_get) and (open_day >= data.active_times)) and 1 or 0)
end

function ChaoZhiGiftViewItem:OnClickGet()
    ServerActivityCtrl.Instance:SendActivityChaoZhiGiftReqGet(self.data.seq)
end