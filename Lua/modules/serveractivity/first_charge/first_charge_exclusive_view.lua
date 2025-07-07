FirstChargeExclusiveView = FirstChargeExclusiveView or DeclareView("FirstChargeExclusiveView", "serveractivity/openserver/first_charge_exclusive", Mod.RandActivity.FirstChargeExclusive)
VIEW_DECLARE_MASK(FirstChargeExclusiveView, ViewMask.Block)
VIEW_DECLARE_LEVEL(FirstChargeExclusiveView, ViewLevel.L1)

function FirstChargeExclusiveView:FirstChargeExclusiveView()

end

function FirstChargeExclusiveView:OnCloseClick()
    ViewMgr:CloseView(FirstChargeExclusiveView)
end

function FirstChargeExclusiveView:OnGoClick()
    ViewMgr:CloseView(FirstChargeExclusiveView)
    ViewMgr:OpenViewByKey(Mod.Activity.TimeLimit)
end


--------------------------FirstChargeExclusivePanel--------------------------
FirstChargeExclusivePanel = FirstChargeExclusivePanel or DeclareMono("FirstChargeExclusivePanel", UIWFlushPanel)
function FirstChargeExclusivePanel:FirstChargeExclusivePanel()
    self.Data = FirstChargeData.Instance
    self.data = self.Data.exclusive_info
    self.language = Language.FirstCharge
    self.data_cares = {
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
        {data = self.data, func = self.FlushShow, init_call = false},
        {data = RoleData.Instance.base_data, func = self.FlushShow, init_call = false, keys = {"level"}},
        {data = self.Data.panel_select, func = self.FlushPanelShow, init_call = false},
    }
    self.Data.panel_select.now_select_id = 0
    self.is_first = true
    self:FlushShow()
    self:FlushPanelShow()
    self.is_first = false
end

function FirstChargeExclusivePanel:FlushShow()
    -- 判断是否都开启了
    local state_list = self.Data:GetPanelStateList()
    -- 0就是一个都没开
    if state_list.open_first == 0 then
        ViewMgr:CloseView(FirstChargeExclusiveView)
        return
    end
    -- 刷新当前显示哪个页签
    if self.Data.panel_select.now_select_id == 0 or not state_list.list[self.Data.panel_select.now_select_id] then
        self.Data.panel_select.now_select_id = state_list.open_first
    end
    self.LeftBtn[self.Data.panel_select.now_select_id].Selector.isOn = true
    self.LeftBtn[self.Data.panel_select.now_select_id]:OnSelect(true)
    -- 隐藏掉未激活的按钮
    for i = 1, self.LeftBtn:Length() do
        self.LeftBtn[i]:SetData({})
        self.LeftBtn[i]:SetObjActive(state_list.list[i])
    end

    self:FlushPanel1()
    self:FlushPanel2()
end

function FirstChargeExclusivePanel:FlushPanelShow()
    for i = 1, self.PanelList:Length() do
        self.PanelList[i]:SetActive(self.Data.panel_select.now_select_id == i)
    end
end

function FirstChargeExclusivePanel:FlushPanel1()
    self.CumulativeList:SetData(self.Data:GetPeriConfig())
    UH.SetText(self.TxtNum, Format(self.language.Num, self.data.progress))

    self.ActState:CloseCountDownTime()
    local act_list = self.Data:GetExclusiveActList()
    local today_wday = ActivityData.Instance:GetWeekNum()
    local next_wday = self.Data:GetNextActWDay()
    if not TableIsEmpty(act_list[today_wday]) then
        local today_act_state = ActivityData.Instance:GetActivityStatus(act_list[today_wday].act_type)
        local today_act_info = ActivityData.Instance:GetActivityInfo(act_list[today_wday].act_type)
        UH.SetText(self.TxtDayActStr, self.language.DayActStr[1])
        UH.SetText(self.TxtActName, today_act_info.info.name_text)
        -- 没开的话要判断是还没开还是开完了
        if today_act_state ~= ActStatusType.Open then
            local start_time_stamp = 0
            if TimeCtrl.Instance:GetServerTime() < ActivityData.GetRemindTimeStamp(act_list[today_wday].act_type) then
                start_time_stamp = ActivityData.GetRemindTimeStamp(act_list[today_wday].act_type)
            else
                local next_act_info = ActivityData.Instance:GetActivityInfo(act_list[next_wday].act_type)
                UH.SetText(self.TxtDayActStr, self.language.DayActStr[2])
                UH.SetText(self.TxtActName, next_act_info.info.name_text)
                start_time_stamp = ActivityData.GetRemindTimeStamp(act_list[next_wday].act_type)
            end
            self.ActState:StampTime(start_time_stamp, TimeType.Type_Time_1, self.language.ActState[1], self.language.ActState[2])
        else
            self.ActState:SetTime(self.language.ActState[2])
        end
    else
        local next_act_info = ActivityData.Instance:GetActivityInfo(act_list[next_wday].act_type)
        UH.SetText(self.TxtDayActStr, self.language.DayActStr[2])
        UH.SetText(self.TxtActName, next_act_info.info.name_text)
        start_time_stamp = ActivityData.GetRemindTimeStamp(act_list[next_wday].act_type)
    end
end

function FirstChargeExclusivePanel:FlushPanel2()
    local is_buy = self.Data:GetCmdBuyFlag() == 1
    self.IHandlerBuy.Interactable = not is_buy
    self.ObjBuyEffect:SetActive(not is_buy)
    -- 成长道具里面会配需要转换职业的道具
    local grow_config = TableCopy(self.Data:GetGrowConfig())
    for k, v in pairs(grow_config) do
        for l, w in pairs(v.reward_item) do
            w.item_id = BagData.GetNewItemId(w.item_id)
        end
    end
    table.sort(grow_config, DataHelper.WeightSort("type", function(data)
        return FirstChargeData.Instance:GetExclusiveFlag(data.type) == 1 and 100 or 0
    end))
    self.ExclusiveLevelList:SetData(grow_config)
    -- 这块显示逻辑只需要做一次
    if self.is_first then
        UH.SetText(self.TxtBuy, is_buy and self.language.Buyed or self.Data.other.buy_money / 10)
        local gift_item = TableCopy(self.Data.other.buy_reward_item)
        local gift_num = table.nums(gift_item)
        local tab1 = {}
        local tab2 = {}
        -- 要判断第一行放几个
        local one_row_num = gift_num < 4 and 1 or 2
        for i = 0, gift_num - 1 do
            -- 直购道具里面会配需要转换职业的道具
            gift_item[i].item_id = BagData.GetNewItemId(gift_item[i].item_id)
            if i < one_row_num then
                table.insert(tab1, gift_item[i])
            else
                table.insert(tab2, gift_item[i])
            end
        end
        if TableIsEmpty(tab2) then
            UH.LocalPosG(self.ObjRewardList1, Vector3.New(440, -40, 0))
        else
            UH.LocalPosG(self.ObjRewardList1, Vector3.New(440, 10, 0))
        end
        self.RewardList1:SetData(DataHelper.FormatItemList(tab1))
        self.RewardList2:SetData(DataHelper.FormatItemList(tab2))
    end
    self.ActiveTime:CloseCountDownTime()
    if is_buy then
        self.ActiveTime:SetTime(self.language.Actived)
    else
        if TimeCtrl.Instance:GetServerTime() < self.data.end_timestamp then
            self.ActiveTime:StampTime(self.data.end_timestamp, TimeType.Type_Time_2, self.language.ActiveTime)
            self.ActiveTime:SetCallBack(BindTool.Bind(self.FlushShow, self))
        end
    end
end

function FirstChargeExclusivePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.ActState:CloseCountDownTime()
    self.ActiveTime:CloseCountDownTime()
end

function FirstChargeExclusivePanel:OnBuyClick()
    -- 检查是否支持万能卡
    local fee = self.Data.other.buy_money / 10
    if ServerActivityCtrl.Instance:IsCanItemBuy(BUY_ITEM_TYPE.TYPE_FIRST_RECHARGE_EXCLUSIVE, fee) then
        ServerActivityCtrl.Instance:SendBuyCmdReq(BUY_ITEM_TYPE.TYPE_FIRST_RECHARGE_EXCLUSIVE, fee, FIRST_RECHARGE_CMD_TYPE.GROW)
        return
    end

    FirstChargeCtrl.Instance:SendFirstChargeReq(FIRST_RECHARGE_REQ_TYPE.BUY_CMD_CHECK, FIRST_RECHARGE_CMD_TYPE.GROW, self.Data.other.cfg_ver)
end

function FirstChargeExclusivePanel:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_FIRST_RECHARGE_EXCLUSIVE, self.Data.other.buy_money)
end


---------------------------------FirstChargeExclusiveLeftBtnItem---------------------------------
FirstChargeExclusiveLeftBtnItem = FirstChargeExclusiveLeftBtnItem or DeclareMono("FirstChargeExclusiveLeftBtnItem", UIWidgetBaseItem)
function FirstChargeExclusiveLeftBtnItem:FirstChargeExclusiveLeftBtnItem()
	UH.SetText(self.Name1, Language.FirstCharge.ExclusiveLeftBtn[self.BtnId or 1])
	UH.SetText(self.Name2, Language.FirstCharge.ExclusiveLeftBtn[self.BtnId or 1])
end

function FirstChargeExclusiveLeftBtnItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.RedPoint:SetNum(FirstChargeData.Instance:GetExclusivePanelRemind(self.BtnId))
end

function FirstChargeExclusiveLeftBtnItem:OnSelect(selected)
	UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        FirstChargeData.Instance.panel_select.now_select_id = self.BtnId
    end
end


---------------------------------CumulativeItem---------------------------------
CumulativeItem = CumulativeItem or DeclareMono("CumulativeItem", UIWidgetBaseItem)
function CumulativeItem:CumulativeItem()
	
end

function CumulativeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(Item.Create(data.reward_item[0]))
    UH.SetText(self.TxtTimes, Format(Language.FirstCharge.Num, data.parameter))
    local is_received = FirstChargeData.Instance:GetExclusiveFlag(data.type) == 1
    local can_receive = FirstChargeData.Instance:GetPeriProgress() >= data.parameter and not is_received
    self.ObjEffect:SetActive(can_receive)
    self.ObjMengBan:SetActive(is_received)
end

function CumulativeItem:OnReceiveClick()
    FirstChargeCtrl.Instance:SendFirstChargeReq(FIRST_RECHARGE_REQ_TYPE.FETCH_EXCLUSIVE, self.data.type)
end


---------------------------------ExclusiveLevelItem---------------------------------
ExclusiveLevelItem = ExclusiveLevelItem or DeclareMono("ExclusiveLevelItem", UIWidgetBaseItem)
function ExclusiveLevelItem:CumulativeItem()
	
end

function ExclusiveLevelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtLevelReach, Format(Language.FirstCharge.LevelReach, data.parameter))
    self.CellList:SetData(DataHelper.FormatItemList(data.reward_item))
    local is_buy = FirstChargeData.Instance:GetCmdBuyFlag() == 1
    local is_received = FirstChargeData.Instance:GetExclusiveFlag(data.type) == 1
    local can_receive = RoleData.Instance:GetRoleLevel() >= data.parameter and not is_received
    self.IHandler.Interactable = is_buy and not is_received and can_receive
    UH.SetText(self.TxtReceive, is_buy and Language.FirstCharge.Receive or Language.FirstCharge.NotBuyed)
    self.ObjReceived:SetActive(is_received)
    self.ObjEffect:SetActive(is_buy and can_receive and not is_received)
end

function ExclusiveLevelItem:OnReceiveClick()
    FirstChargeCtrl.Instance:SendFirstChargeReq(FIRST_RECHARGE_REQ_TYPE.FETCH_EXCLUSIVE, self.data.type)
end