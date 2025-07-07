--发送公告
--==============================NoticeView===============================
NoticeView = NoticeView or DeclareView("NoticeView", "mall/NoticeView")

VIEW_DECLARE_MASK(NoticeView,ViewMask.Block)

function NoticeView:NoticeView()
    self.Alert:SetData(self:GetType(), Vector2.New(500, 255))
end

function NoticeView:LoadCallback(data)
    if data.item == nil then return end
    self.sale_index = data.sale_index
    if data.item:IsPet() then
        data.item_data.param.index = -1
        data.pet = Pet.New(data.item_data.param)
        self.Call:SetData(data.pet)
        UH.SetText(self.Name, data.pet:Name())
        UH.SetText(self.Type, Language.Pet.RaceClass[data.pet.config.race])
    else
        self.Call:SetData(data.item)
        UH.SetText(self.Name, data.item:Name())
        UH.SetText(self.Type, data.item:TypeName())
    end
    local free = TradeData.Instance:GetNoticeFree(data.price,data.price_type)
    UH.SetText(self.Num, math.floor(free))
end

function NoticeView:OnClickSend()
    TradeCtrl.Instance:SendAdvertiseReq(self.sale_index)
    ViewMgr:CloseView(NoticeView)
end

--==============================RuleView===============================
RuleView = RuleView or DeclareView("RuleView", "mall/RuleView")

function RuleView:RuleView()
    self.Board:SetData(self:GetType(), Language.Mall.RuleTitle,Vector2.New(831, 433))
end

function RuleView:LoadCallback()
   self:FlushRuleView(1)
end

function RuleView:OnClickColumn(type)
    self:FlushRuleView(type)
end

function RuleView:FlushRuleView(type)
    UH.SetText(self.Title,Format(Language.Mall.RuleName,Language.Mall.RuleType[type]))
    UH.SetText(self.Content,Config.language_cfg_auto.textdesc[type+5].desc)
end

--交易记录
--==============================RecordView===============================
RecordView = RecordView or DeclareView("RecordView", "mall/RecordView")

VIEW_DECLARE_MASK(RecordView,ViewMask.BgBlockClose)

function RecordView:RecordView()
    self.Board:SetData(self:GetType(), Language.Mall.SellRecord,Vector2.New(973, 609))
    self.care_handle = TradeData.Instance.record_data:Care(BindTool.Bind(self.FlushRecordList,self))
end

function RecordView:LoadCallback()
    TradeCtrl.Instance:SendRoleRecordReq()
end

function RecordView:FlushRecordList()
    local list = TradeData.Instance:GetRoleRecordList()
    for i=1,2 do
        if #list[i] > 0 then
            self.SellList[i]:SetData(list[i])
        end
        self.NotItemTip[i]:SetActive(#list[i] == 0)
    end
    local gold_list = TradeData.Instance.record_data.gold_list or {}
    for i=1,2 do
        UH.SetText(self.Gold[i],gold_list[i]or 0)
    end
end

function RecordView:CloseCallback()
	TradeData.Instance.record_data:Uncare(self.care_handle)
	self.care_handle = nil
end

--成交记录
--==============================DealRecordView===============================
DealRecordView = DealRecordView or DeclareView("DealRecordView", "mall/DealRecordView")

function DealRecordView:DealRecordView()
    self.Alert:SetData(self:GetType(), Vector2.New(360, 490))
    self.care_handle = TradeData.Instance.deal_record_data:Care(BindTool.Bind(self.FlushRecordList,self))
end

function DealRecordView:LoadCallback(item_id)
    if item_id ~= nil and item_id ~= 0 then
        TradeCtrl.Instance:SendRecordReq(item_id)
    end
end

function DealRecordView:FlushRecordList()
    local info_list = TradeData.Instance:GetRecordList()
    if #info_list > 0 then
        self.List:SetData(info_list)
    end
    self.NotItemTip:SetActive(#info_list == 0)
end

function DealRecordView:CloseCallback()
	TradeData.Instance.deal_record_data:Uncare(self.care_handle)
	self.care_handle = nil
end

--批量购买
--==============================BatchBuyView===============================
BatchBuyView = BatchBuyView or DeclareView("BatchBuyView", "mall/BatchBuyView")

VIEW_DECLARE_MASK(BatchBuyView, ViewMask.BlockClose)
VIEW_DECLARE_LEVEL(BatchBuyView, ViewLevel.Ls)

function BatchBuyView:BatchBuyView()
    self.beh_data = BehaviorData.Instance

    self.start_time = 0
    self.long_press_times = 0
    -- 记录超过最小最大值的第一次
    self.is_first_reach_min = false
    self.is_first_reach_max = false
    self.InputField.onValueChanged:AddListener(BindTool.Bind(self.OnClickNumSelect, self))
end

function BatchBuyView:Awake()
    if self.beh_data.auto_behavior > 0 then
        self.timer_auto_beh = TimeHelper:AddDelayTimer(function ()
            ViewMgr:CloseView(MallView)
            self:OnClickBuy()
        end,
        self.beh_data.auto_behavior)
    end
end

function BatchBuyView:OnDestroy()
    TimeHelper:CancelTimer(self.timer_auto_beh)
end

function BatchBuyView:OnClickNumSelect()
    if self.Num.text ~= "" then
        self:OnClickAddNum(tonumber(self.Num.text) - self.make_num)
    end
end

function BatchBuyView:LoadCallback(data)
    self.data = data
    data.item = data.item or Item.Create(data)
    self.Call:SetData(data.item)
    UH.SetText(self.Name, data.item:Name())
    if data.conver_item_id and data.conver_item_id ~= 0 then
        UH.SetIcon(self.Icon1, Item.GetIconId(data.conver_item_id), ICON_TYPE.ITEM)
        UH.SetIcon(self.Icon2, Item.GetIconId(data.conver_item_id), ICON_TYPE.ITEM)
        self.conver_item_id = data.conver_item_id
    else
        UH.SetGold(self.Gold1, data.price_type)
        UH.SetGold(self.Gold2, data.price_type)
    end
    self.Icon1:SetObjActive(data.conver_item_id and data.conver_item_id ~= 0)
    self.Icon2:SetObjActive(data.conver_item_id and data.conver_item_id ~= 0)
    if data.btn_name then
        UH.SetText(self.BtnName,data.btn_name)
    end
    self.Name.color = QualityColor[data.item:Color() or 1]
    self.make_num = MallData.JumpItemNum or 1
    UH.SetText(self.TxtType, data.item:TypeDesc())
    self.Quality.SpriteName = ItemInfoConfig.QualitySp[data.item:Color()]

    -- 铸灵特殊处理
    local is_spirit = Item.GetBigType(data.item_id) == ITEM_BIG_TYPE.EQUIP_SPIRIT
    UH.SetText(self.TxtTitle, is_spirit and Language.Shop.SpiritDesc or Language.Shop.ItemDesc)
    self.AttrList:SetObjActive(is_spirit)
    self.TxtDesc:SetObjActive(not is_spirit)
    self.ObjBigDesc:SetActive(false)
    if is_spirit then
        local config = EquipData.Instance:SpiritLevelAttr(data.item.param.item_id, data.item.param.level)
        self.AttrList:SetData(config)
    else
        self.TxtDesc:SetObjActive(true)
        UH.SetText(self.TxtDesc, data.item:GiftDesc())
        -- 如果描述格子超过190，就使用滚动列表
        if self.TxtDesc.preferredHeight > 190 then
            self.ObjBigDesc:SetActive(true)
            self.TxtDesc:SetObjActive(false)
            UH.SetText(self.TxtBigDesc, data.item:GiftDesc())
        end
    end
    

    MallData.JumpItemNum = nil
    self:FlushNumView()
end

function BatchBuyView:OnClickGetWay()
    ItemInfoCtrl.Instance:ItemDeriveView(ItemGetWayView,
    {item_id = self.data.item.item_id, pos = UH.GetRootPosition(self.ObjWayPos) + Vector2.New(0,30)})
end

--数量改变
function BatchBuyView:OnClickAddNum(num)
    if self.data.limit_times and self.data.limit_type ~= 0 then
        self:OnClickAddLimitNum(num)
        return
    end
    if self.make_num + num <= 0 then
        if num == -1 then
            if self.is_first_reach_min == false then
                self.is_first_reach_min = true
                PublicPopupCtrl.Instance:Center(ErrorText[19])
            end
            self.make_num = 1
            self:FlushNumView()
            return
        else
            self.make_num = 1
        end
    elseif num == 10 then
        self.make_num = math.floor(self.make_num / 10) * 10 + num
        self:PopupTip()
    else
        self.is_first_reach_min = false
        self.make_num = self.make_num + num 
    end
    self:FlushNumView()
end

function BatchBuyView:FlushNumView()
    if self.make_num <= 0 then
        self.make_num = 1
    end
    UH.SetText(self.Num, self.make_num)
    UH.SetText(self.TxtNowPrice, self.data.price == 0 and Language.Shop.Free or self.data.price)
    UH.SetText(self.TxtTotalPrice, self.data.price == 0 and Language.Shop.Free or self.make_num * self.data.price)
end

function BatchBuyView:OnClickBuy()
    -- 等级判断
    TimeHelper:CancelTimer(self.timer_auto_beh)

    if self.data and self.data.require_type == 1 then
        if RoleData.Instance:GetRoleLevel() < self.data.require_value then
            PublicPopupCtrl.Instance:Center(Format(Language.Shop.RequireLevelTip, self.data.require_value))
            return
        end
    end
    self.data.make_num = self.make_num
    local price = self.make_num * self.data.price
    if not MallCtrl.IsNotCurrency(self.data.price_type,price,self.conver_item_id)then
        ViewMgr:CloseView(BatchBuyView)
        return
    end
    if self.data.content_func then
        self.data.content_func(self.data)
    end
    if self.data.confirm_func then
        self.data.confirm_func(self.make_num)
        ViewMgr:CloseView(BatchBuyView)
    end
    if TaskData.shop_auto_close then
        ViewMgr:CloseView(MallView)
        ViewMgr:CloseView(StoreView)
    end
end

function BatchBuyView:OnClickAddLimitNum(num)
    if self.make_num + num <= 0 then
        if num == -1 then
            if self.is_first_reach_min == false then
                self.is_first_reach_min = true
                PublicPopupCtrl.Instance:Center(ErrorText[19])
            end
            self.make_num = 1
            self:FlushNumView()
            return
        else
            self.make_num = 1
        end
    elseif self.make_num + num > self.data.limit_times then
        if num == 1 then
            if self.is_first_reach_max == false then
                self.is_first_reach_max = true
                PublicPopupCtrl.Instance:Center(Language.Shop.LimitTimesMaxTip)
            end
            return
        else
            self.make_num = self.data.limit_times
        end
    else
        if num == 10 then 
            self.make_num = math.floor(self.make_num / 10) * 10 + num
            self:PopupTip()
        else
            self.is_first_reach_min = false
            self.is_first_reach_max = false
            self.make_num = self.make_num + num
        end
    end
    self:FlushNumView()
end

function BatchBuyView:OnClickDown(num)
    TimeHelper:CancelTimer(self.time_func)
    self.start_time = 0
    self.time_func = TimeHelper:AddRunTimer(function()
        self.start_time = self.start_time + 0.1
        if self.start_time > 0.2 then
            self:OnClickAddNum(num)
        end
	end, 0.1)
end

function BatchBuyView:OnClickUp(num)
    TimeHelper:CancelTimer(self.time_func)
    self.start_time = 0
end

function BatchBuyView:CloseCallback()
    TimeHelper:CancelTimer(self.time_func)
    self.start_time = 0
    self.conver_item_id = nil
    ViewMgr:CloseView(ItemGetWayView)
end

function BatchBuyView:PopupTip()
    self.long_press_times = self.long_press_times + 1
    if self.long_press_times < 2 then
        return
    end
    local role_id = RoleData.Instance:GetRoleId()
    if UnityPlayerPrefs.GetInt("batch_buy_key".. role_id) == 0 then
        UnityPlayerPrefs.SetInt("batch_buy_key".. role_id, 1)
        PublicPopupCtrl.Instance:Center(Language.Mall.LongPressTip)
    end
end

function BatchBuyView:OnGoldClick()
    local item_data = Item.Create({item_id = HuoBi[self.data.price_type] or self.data.conver_item_id})
    MainOtherCtrl.Instance:GetWayView({item = item_data})
end


--声望不足
--==============================ShortageView===============================
ShortageView = ShortageView or DeclareView("ShortageView", "mall/ShortageView")

VIEW_DECLARE_MASK(ShortageView,ViewMask.BlockClose)

function ShortageView:LoadCallback()
    self.GatWay:SetData(ExchangeData.Instance:GetChivalrousWayList())
end

--==============================ShortageItem===============================
ShortageItem = ShortageItem or DeclareMono("ShortageItem", UIWidgetBaseItem)

function ShortageItem:SetData(data)
    UH.SetText(self.Name, data.desc)
    UH.SpriteName(self.Icon, data.icon_name)
    UIWidgetBaseItem.SetData(self, data)
end

function ShortageItem:OnClickItme()
    if self.data.open_panel ~= "" then
        ViewMgr:OpenViewByKey(self.data.open_panel)
    else
        LogError("配置缺少跳转open_panel")
    end
end

--===================================ShopDialogTipsView===================================
ShopDialogTipsView = ShopDialogTipsView or DeclareView("ShopDialogTipsView", "mall/ShopDialogTipsView")

function ShopDialogTipsView:LoadCallback(data)
    if data ~= nil and data.content then
        self.Call:SetData(data.item)
        UH.SetText(self.Content,data.content)
        UH.SetGold(self.Gold,data.money_type)
        UH.SetText(self.GoldNum,data.gold_num)
        self.content_func = data.content_func
    end
end

function ShopDialogTipsView:OnClickCancel()
    ViewMgr:CloseView(ShopDialogTipsView)
end

function ShopDialogTipsView:OnClickConfirm()
    if self.content_func then
        self.content_func()
    end
    ViewMgr:CloseView(ShopDialogTipsView)
end

--==============================TipPopupView===============================
TipPopupView = TipPopupView or DeclareView("TipPopupView", "mall/TipPopupView")
--不带黑幕的
VIEW_DECLARE_MASK(TipPopupView,ViewMask.BlockClose)

function TipPopupView:LoadCallback(content)
    self.Tips.text = content
    local is_slide = string.len(content) >= GameEnum.TIPS_POPUP_MAX_SHOW_NUM
    if is_slide then
        self.SlideTips.text = content
    end
    self.Slide:SetActive(is_slide)
    self.All:SetActive(not is_slide)
end

--==============================交易行守则===============================
TradeHelp = TradeHelp or DeclareView("TradeHelp", "mall/TradeHelp")

function TradeHelp:LoadCallback()
    self.List:SetData(TradeData.Instance:GetTradeHelpList())
end

function TradeHelp:OnClickClose()
    ViewMgr:CloseView(TradeHelp)
end

----------------------------TradeHelpItem----------------------------
TradeHelpItem = TradeHelpItem or DeclareMono("TradeHelpItem", UIWidgetBaseItem)

function TradeHelpItem:SetData(data)
    UH.SetText(self.Desc,data.desc)
end