
local PriceType2Id = {
    [-1] = 99999,
    [0] = 65534,
    [16] = 65698,
}

HorcruxTianchengView = HorcruxTianchengView or DeclareView("HorcruxTianchengView","serveractivity/horcrux_tiancheng/horcrux_tiancheng", Mod.RandActivity.HorcruxTiancheng)
VIEW_DECLARE_LEVEL(HorcruxTianchengView, ViewLevel.L0)

function HorcruxTianchengView:LoadCallback()
    self.Currency:SetCurrency(CurrencyType.HorcruxTianchengKey)
end

function HorcruxTianchengView:OnClickClose()
    ViewMgr:CloseView(HorcruxTianchengView)
end

HorcruxTianchengViewF = HorcruxTianchengViewF or DeclareMono("HorcruxTianchengViewF", UIWFlushPanel)

function HorcruxTianchengViewF:HorcruxTianchengViewF()
    self.data = ServerActivityCtrl.Instance:Data()
    self.act_data = ActivityData.Instance
    self.language = Language.HorcruxTiancheng

    self.data_cares = {
        {data = self.data.horcrux_tiancheng_info, func = self.FlushPool, keys = {"pool"}},
        {data = self.data.horcrux_tiancheng_info, func = self.FlushDraw, keys = {"draw"}, init_call = false},
        {data = self.data.horcrux_tiancheng_info, func = self.FlushRedPoint, keys = {"role"}},
        {data = self.data.horcrux_tiancheng_info.info, func = self.FlushModel, keys = {"model_index"}},
        {data = self.act_data.activity_status_event, func = self.FlushState},
    }

    HorcruxTianchengViewF.is_rolling = false
    self.SkipToggle.isOn = 1 == UnityPlayerPrefs.GetInt(PrefKeys.HorcruxTianchengSkip())
    self.rolling_index = 1

    self.show_list = self.data:GetHorcruxTianchengModelList()
    self.data:SetHorcruxTianchengModelIndex(1)

    self.SHOW_ITEM_MAX = 8
    self.INTERVAL_MIN = 0.1
    self.INTERVAL_MAX = 0.2
    self.INTERVAL_PER = 0.05
end

function HorcruxTianchengViewF:Awake()
    UIWFlushPanel.Awake(self)

    self:FlushRewards()
    self:FlushRemainTime()
end

function HorcruxTianchengViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    TimeHelper:CancelTimer(self.timer_ct)
    TimeHelper:CancelTimer(self.timer_rt)
    TimeHelper:CancelTimer(self.timer_rt2)
end

function HorcruxTianchengViewF:FlushRewards()
    local list = self.data:GetHorcruxTianchengRewardList()
    for i = 1, self.ShowItems:Length() do
        if list[i] then
            self.ShowItems[i]:SetData(list[i])
        end
    end

    self.CostNum.text = self.data:GetHorcruxTianchengCostNum()
    self.ShowItems[self.rolling_index]:SetSelected(true)
    self.ShowTogList:SetDataList(self.show_list)
    TimeHelper:CancelTimer(self.timer_rt2)
    self.timer_rt2 = TimeHelper:AddRunTimer(function ()
        local to_index = self.data:GetHorcruxTianchengModelIndex() + 1
        to_index = to_index > #self.show_list and 1 or to_index
        self.data:SetHorcruxTianchengModelIndex(to_index)
    end, 3, nil, false)
    self.BtnExchangeObj:SetActive(1 == self.data:GetHorcruxTianchengIsOpen())
end

function HorcruxTianchengViewF:FlushModel()
    local index = self.data:GetHorcruxTianchengModelIndex()
    if self.ShowTogList:GetItem(index) then
        self.ShowTogList:GetItem(index):SetSelected(true)
    end
    local show_item = self.show_list[index]
    local res_path = DrawerHelper.GetWeaponPath(show_item:ModelId())
    local uo = self.ModelShow:UiObj()
    if uo == nil then
        uo = UIObjDrawer.CreateByPath(res_path)
        self.ModelShow:SetShowData(
            {
                ui_obj = uo,
                view = HorcruxTianchengView,
                show_shadow = false,
                -- load_callback = BindTool.Bind(self.ModelCallback, self),
            }
        )
    else
        uo:SetData(res_path) 
    end
    self.ShowName.text = show_item:Name()
    local weapon_type, pet_id = show_item:WeaponType()
    self.ShowCell:SetData(Pet.New({id = pet_id, str_level = 0}))
end

function HorcruxTianchengViewF:FlushPool()
    local pool = self.data.horcrux_tiancheng_info.pool
    if -1 == pool.pool_type then
        UH.SetIcon(self.PoolIcon, Item.GetIconId(pool.item_id), ICON_TYPE.ITEM)
    else
        self.PoolIcon.SpriteName = HuoBi[pool.pool_type]
    end
    self.PoolNum.text = pool.num
end

function HorcruxTianchengViewF:FlushDraw()
    if self.SkipToggle.isOn then
        for i = 1, self.ShowItems:Length() do
            self.ShowItems[i]:RollingEnd(self.data.horcrux_tiancheng_info.draw.seq)
        end
    else
        HorcruxTianchengViewF.is_rolling = true
        local result_index = 1
        for i = 1, self.ShowItems:Length() do
            result_index = self.ShowItems[i]:IsResultItem(self.data.horcrux_tiancheng_info.draw.seq) and i or result_index
        end
        self:Rolling(result_index, self.INTERVAL_MIN, false)
    end
end

function HorcruxTianchengViewF:Rolling(result_index, interval, init_call)
    TimeHelper:CancelTimer(self.timer_rt)
    self.timer_rt = TimeHelper:AddRunTimer(function ()
        if self.rolling_index == result_index then
            TimeHelper:CancelTimer(self.timer_rt)
            if interval > self.INTERVAL_MAX then
                self.ShowItems[self.rolling_index]:SetSelected(true)
                HorcruxTianchengViewF.is_rolling = false
                return
            end
            interval = interval + self.INTERVAL_PER
            local to_index = self.rolling_index + 1
            self.rolling_index = to_index > self.SHOW_ITEM_MAX and 1 or to_index
            self.ShowItems[self.rolling_index]:SetSelected(true)
            self:Rolling(result_index, interval, false)
        else
            local to_index = self.rolling_index + 1
            self.rolling_index = to_index > self.SHOW_ITEM_MAX and 1 or to_index
            self.ShowItems[self.rolling_index]:SetSelected(true)
        end
    end, interval, nil, init_call)
end

function HorcruxTianchengViewF:FlushRedPoint()
    self.red_point_item = self.red_point_item or UiRedPoint.New(self.BtnBuyObj, Vector3.New(77.7,24,0))
    self.red_point_item:SetNum(self.data:HorcruxTianchengRemind())
end

function HorcruxTianchengViewF:FlushState()
    if self.act_data.activity_status_event.act_type ~= ACTIVITY_TYPE.RAND_HORCRUX_TIANCHENG then return end
    local activity_status = self.act_data:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_HORCRUX_TIANCHENG)
    local status = activity_status and activity_status.status or ActStatusType.Close
    if ActStatusType.Open == status then
        self:FlushRemainTime()
    else
        ViewMgr:CloseView(HorcruxTianchengView)
    end
end

function HorcruxTianchengViewF:FlushRemainTime()
    TimeHelper:CancelTimer(self.timer_ct)
    local activity_status = self.act_data:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_HORCRUX_TIANCHENG)
    local end_time = activity_status and activity_status.param_2 or 0
    if end_time - TimeHelper.GetServerTime() <= 0 then
        self.RemainTime.text = string.format(self.language.RemainTime, 0, 0, 0)
        return
    end
    self.timer_ct = TimeHelper:AddCountDownCT(function ()
        local last_time = end_time - TimeHelper.GetServerTime()
        local ft = TimeHelper.FormatDHMS(last_time)
        self.RemainTime.text = string.format(self.language.RemainTime, ft.day, ft.hour, ft.min)
    end, function ()
        self.RemainTime.text = string.format(self.language.RemainTime, 0, 0, 0)
    end, end_time, 1)
end

function HorcruxTianchengViewF:OnClickBuy()
    ViewMgr:OpenView(HorcruxTianchengShopView)
end

function HorcruxTianchengViewF:OnClickExchange()
    ViewMgr:OpenView(HorcruxTianchengExchangeView)
end

function HorcruxTianchengViewF:OnClickStart()
    if HorcruxTianchengViewF.is_rolling then return end
    if  BagData.Instance:GetNumByItemId(CommonItemId.HorcruxTianchengKey) < self.data:GetHorcruxTianchengCostNum() then
        ViewMgr:OpenView(HorcruxTianchengShopView)
    end
    ServerActivityCtrl.Instance:SendActivityHorcruxTianChengReqRolling()
end

function HorcruxTianchengViewF:OnClickDetail()
    local show_item = self.show_list[self.data:GetHorcruxTianchengModelIndex()]
    ViewMgr:OpenView(PetWeaponConvertInfoView, {item_id = show_item.item_id})
end

function HorcruxTianchengViewF:OnClickRate()
    ViewMgr:OpenView(HorcruxTianchengRateView)
end

function HorcruxTianchengViewF:OnClickPool()
    local pool = self.data.horcrux_tiancheng_info.pool
    local item_data
    if -1 == pool.pool_type then
        item_data = Item.Init(pool.item_id)
    else
        item_data = Item.Init(HuoBi[pool.pool_type])
    end
    
    local info = {
		item_list = {item_data},
		func = function ()
			return get_other_func_table(item_data)
		end,
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

function HorcruxTianchengViewF:OnTogSkip()
    UnityPlayerPrefs.SetInt(PrefKeys.HorcruxTianchengSkip(), self.SkipToggle.isOn and 1 or 0)
end

HorcruxTianchengViewRewardItem = DeclareMono("HorcruxTianchengViewRewardItem", UIWidgetBaseItem)

function HorcruxTianchengViewRewardItem:HorcruxTianchengViewRewardItem()
    self.act_data = ServerActivityCtrl.Instance:Data()
    -- self.langauge = Language.HorcruxTiancheng.Shop
end

function HorcruxTianchengViewRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.NumItem:SetObjActive(1 == data.reward_type)
    self.Cell:SetObjActive(1 ~= data.reward_type)
    if 1 == data.reward_type then
        self.NumItem:ShowNum(data.reward_per)
    else
        if 2 == data.reward_type then
            self.Cell:SetData(Item.Create({item_id = CommonItemId.Gold, num = data.gold_num}))
        else
            self.Cell:SetData(Item.Create(data.reward_item))
        end
    end
end

function HorcruxTianchengViewRewardItem:RollingEnd(seq)
    if self.data and self.data.seq == seq then
        self:SetSelected(true)
    end
end

function HorcruxTianchengViewRewardItem:IsResultItem(seq)
    return self.data and self.data.seq == seq
end


HorcruxTianchengViewShowTogItem = DeclareMono("HorcruxTianchengViewShowTogItem", UIWidgetBaseItem)

function HorcruxTianchengViewShowTogItem:HorcruxTianchengViewShowTogItem()
    self.act_data = ServerActivityCtrl.Instance:Data()
end

function HorcruxTianchengViewShowTogItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
end

function HorcruxTianchengViewShowTogItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function HorcruxTianchengViewShowTogItem:OnSelect(value)
    if value then
        self.act_data:SetHorcruxTianchengModelIndex(self.data.hqtc_index)
    end
end


HorcruxTianchengShopView = HorcruxTianchengShopView or DeclareView("HorcruxTianchengShopView","serveractivity/horcrux_tiancheng/horcrux_tiancheng_shop")

function HorcruxTianchengShopView:LoadCallback()
    self.Currency:SetCurrency(CurrencyType.ImmortalCoin, CurrencyType.Gold)
end

function HorcruxTianchengShopView:OnClickClose()
    ViewMgr:CloseView(HorcruxTianchengShopView)
end

HorcruxTianchengShopViewF = HorcruxTianchengShopViewF or DeclareMono("HorcruxTianchengShopViewF", UIWFlushPanel)

function HorcruxTianchengShopViewF:HorcruxTianchengShopViewF()
    self.data = ServerActivityCtrl.Instance:Data()

    self.data_cares = {
        {data = self.data.horcrux_tiancheng_info, func = self.FlushShow, keys = {"role"}},
        {data = self.data:GetCmdBuyItemCheckRet(),func = self.SendBuy, init_call = false}
    }
end

function HorcruxTianchengShopViewF:FlushShow()
    local list1 = self.data:GetHorcruxTianchengShopList()
    local list2 = self.data:GetHorcruxTianchengConsumeRewardList()
    self.CostNum.text = self.data.horcrux_tiancheng_info.role.consume_gold
    self.ShowList:SetData(list1)
    self.RewardList:SetData(list2)

    self.red_point_item = self.red_point_item or UiRedPoint.New(self.BtnTab2Obj, Vector3.New(60,25,0))
    self.red_point_item:SetNum(self.data:HorcruxTianchengRemind())
end

function HorcruxTianchengShopViewF:SendBuy()
    if HorcruxTianchengShopViewF.sel_data then
        ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_HORCRUX_TIANCHENG, HorcruxTianchengShopViewF.sel_data.buy_money)
    end
end


HorcruxTianchengShopViewItem = DeclareMono("HorcruxTianchengShopViewItem", UIWidgetBaseItem)

function HorcruxTianchengShopViewItem:HorcruxTianchengShopViewItem()
    self.act_data = ServerActivityCtrl.Instance:Data()
    self.langauge = Language.HorcruxTiancheng.Shop
end

function HorcruxTianchengShopViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if data.reward_item and data.reward_item[0] then
        local show_item = data.reward_item[0]
        self.Cell:SetData(Item.Init(show_item.item_id, show_item.num, show_item.is_bind))
    end
    local remain_times = data.today_times - self.act_data:GetHorcruxTianchengShopLimit(data.seq)
    self.Discount.text = string.format(self.langauge.discount, data.discount)
    self.Limit.text = string.format(self.langauge.limit[data.limit_type], 
        RichTextHelper.ColorStr(remain_times, remain_times > 0 and COLORSTR.Green9 or COLORSTR.Red8), data.today_times)
    self.Price.text = self.data.price_type >= 0 and data.buy_money or data.buy_money / 10
    self.PriceSp.SpriteName = PriceType2Id[data.price_type]
end

function HorcruxTianchengShopViewItem:OnClickBuy()
    if 0 == (self.data.today_times - self.act_data:GetHorcruxTianchengShopLimit(self.data.seq)) then
        PublicPopupCtrl.Instance:Center(self.langauge.max_times)
    end
    if self.data.price_type >= 0 then
        if 0 == self.data.price_type and not MallCtrl.IsNotCurrency(CurrencyType.Gold, self.data.buy_money) then
            return
        elseif 16 == self.data.price_type and not MallCtrl.IsNotCurrency(CurrencyType.ImmortalCoin, self.data.buy_money) then
            return
        end
        ServerActivityCtrl.Instance:SendActivityHorcruxTianChengReqBuy(self.data.seq)
    else
        HorcruxTianchengShopViewF.sel_data = self.data
        ServerActivityCtrl.Instance:SendActivityHorcruxTianChengReqMoneyCheck(self.data)
    end
end

HorcruxTianchengShopViewRewardItem = DeclareMono("HorcruxTianchengShopViewRewardItem", UIWidgetBaseItem)

function HorcruxTianchengShopViewRewardItem:HorcruxTianchengShopViewRewardItem()
    self.act_data = ServerActivityCtrl.Instance:Data()
    self.langauge = Language.HorcruxTiancheng.Shop
end

function HorcruxTianchengShopViewRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local info = self.act_data.horcrux_tiancheng_info.role
	local consume_gold = info.consume_gold or 0
    local is_get = self.act_data:GetHorcruxTianchengShopConsumeRewardIsGet(data.seq)
    local can_get = (not is_get) and (consume_gold >= data.consume_num)

    self.Name.text = string.format(self.langauge.reward_name, data.consume_num)
    self.GetedObj:SetActive(is_get)
    self.BtnGetObj:SetActive(not is_get)
    self.BtnGetInter.Interactable = can_get
    self.EffectObj:SetActive(can_get)
    if data.reward_item then
        local reward_items = {}
        for i = 0, #data.reward_item do
            local reward_item = data.reward_item[i]
            if reward_item then
                table.insert(reward_items, Item.Init(reward_item.item_id, reward_item.num, reward_item.is_bind))
            end
        end
        self.Rewards:SetData(reward_items)
    end
    self.red_point_item = self.red_point_item or UiRedPoint.New(self.CanvasRP, Vector3.New(62.7,24,0))
    self.red_point_item:SetNum(can_get and 1 or 0)
end

function HorcruxTianchengShopViewRewardItem:OnClickGet()
    ServerActivityCtrl.Instance:SendActivityHorcruxTianChengReqConsumeGet(self.data.seq)
end

HorcruxTianchengExchangeView = HorcruxTianchengExchangeView or DeclareView("HorcruxTianchengExchangeView","serveractivity/horcrux_tiancheng/horcrux_tiancheng_exchange")

function HorcruxTianchengExchangeView:LoadCallback()
    self.Currency:SetCurrency(CurrencyType.ImmortalCoin, CurrencyType.Gold)
end

function HorcruxTianchengExchangeView:OnClickClose()
    ViewMgr:CloseView(HorcruxTianchengExchangeView)
end

HorcruxTianchengExchangeViewF = HorcruxTianchengExchangeViewF or DeclareMono("HorcruxTianchengExchangeViewF", UIWFlushPanel)

function HorcruxTianchengExchangeViewF:HorcruxTianchengExchangeViewF()
    self.data = ServerActivityCtrl.Instance:Data()

    self.data_cares = {
        {data = self.data.horcrux_tiancheng_info, func = self.FlushShow, keys = {"role"}}
    }
end

function HorcruxTianchengExchangeViewF:FlushShow()
    local list = self.data:GetHorcruxTianchengExchangeList()
    self.ShowList:SetData(list)
end


HorcruxTianchengExchangeViewItem = DeclareMono("HorcruxTianchengExchangeViewItem", UIWidgetBaseItem)

function HorcruxTianchengExchangeViewItem:HorcruxTianchengExchangeViewItem()
    self.act_data = ServerActivityCtrl.Instance:Data()
    self.langauge = Language.HorcruxTiancheng.Exchange
end

function HorcruxTianchengExchangeViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if data.exchange_item then
        local exchange_item = data.exchange_item
        local item_data = Item.Init(exchange_item.item_id, exchange_item.num, exchange_item.is_bind)
        self.Cell:SetData(item_data)
        self.Name.text = item_data:Name()
        self.Limit.text = string.format(self.langauge.limit[data.buy_type], data.today_times - self.act_data:GetHorcruxTianchengExchangeLimit(data.item_seq), data.today_times)
    end
    if data.consume_item then
        local consume_items = {}
        if data.gold > 0 then table.insert(consume_items, Item.Init(CommonItemId.Gold, data.gold)) end
        if data.jade > 0 then table.insert(consume_items, Item.Init(CommonItemId.ImmortalCoin, data.jade)) end
        for i = 0, #data.consume_item do
            local consume_item = data.consume_item[i]
            if consume_item then
                table.insert(consume_items, Item.Init(consume_item.item_id, consume_item.num, consume_item.is_bind))
            end
        end
        self.Consumes:SetData(consume_items)
    end
end

function HorcruxTianchengExchangeViewItem:OnClickExchange()
    if self.data.gold > 0 and RoleData.Instance:GetCurrency(CurrencyType.Gold) < self.data.gold then
        ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = HuoBi[CurrencyType.Gold]})})
    elseif self.data.jade > 0 and RoleData.Instance:GetCurrency(CurrencyType.ImmortalCoin) < self.data.jade then
        ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = HuoBi[CurrencyType.ImmortalCoin]})})
    end
    ServerActivityCtrl.Instance:SendActivityHorcruxTianChengReqExchange(self.data.item_seq)
end


HorcruxTianchengRateView = HorcruxTianchengRateView or DeclareView("HorcruxTianchengRateView","serveractivity/horcrux_tiancheng/horcrux_tiancheng_rate")
VIEW_DECLARE_MASK(HorcruxTianchengRateView,ViewMask.BgBlock)
function HorcruxTianchengRateView:HorcruxTianchengRateView()
    self.data = ServerActivityData.Instance
end

function HorcruxTianchengRateView:LoadCallback()
    self.Board:SetData(HorcruxTianchengRateView, Language.HorcruxTiancheng.PreviewTitle,Vector2.New(546, 410))
    self.ItemList:SetData(self.data:GetHorcruxTianchengRewardList())
end


HorcruxTianchengRateViewItem = HorcruxTianchengRateViewItem or DeclareMono("HorcruxTianchengRateViewItem",UIWidgetBaseItem)
function HorcruxTianchengRateViewItem:HorcruxTianchengRateViewItem()
end

function HorcruxTianchengRateViewItem:SetData(data)
    self.NumItem:SetObjActive(1 == data.reward_type)
    self.ItemCell:SetObjActive(1 ~= data.reward_type)
    if 1 == data.reward_type then
        self.NumItem:ShowNum(data.reward_per)
    else
        if 2 == data.reward_type then
            self.ItemCell:SetData(Item.Create({item_id = CommonItemId.Gold, num = data.gold_num}))
        else
            self.ItemCell:SetData(Item.Create(data.reward_item))
        end
    end
    UH.SetText(self.RateText, string.format(Language.Common.Percent, string.format("%0.2f", data.show_rate * 100)))
end
