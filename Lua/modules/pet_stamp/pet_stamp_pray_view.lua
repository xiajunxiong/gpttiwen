local is_open_pet_stamp_show_view = false

-- =============== 祈神界面 ===============
PetStampPrayView = PetStampPrayView or DeclareView("PetStampPrayView", "pet/pet_stamp/pet_stamp_store/pet_stamp_pray_view", Mod.PetStampPray.Main)
VIEW_DECLARE_LEVEL(PetStampPrayView, ViewLevel.L0)
function PetStampPrayView:PetStampPrayView()
    -- self.Currency:DefaultSet()
    self.Currency:SetData(CurrencyType.Gold, CurrencyType.PetStampPrayItem)
end

function PetStampPrayView:LoadCallback()
    
end

function PetStampPrayView:CloseCallback()
    PetStampData.Instance:ClearResloveData()
end

function PetStampPrayView:OnConvertClick()
    ViewMgr:OpenView(PetStampConvertView)
end

function PetStampPrayView:OnLogClick()
    ViewMgr:OpenView(PetStampPrayRecordView)
end

function PetStampPrayView:OnCloseClick()
    ViewMgr:CloseView(PetStampPrayView)
end

function PetStampPrayView:OnResloveClick()
    if PetStampData.Instance.is_pray_animation then
        PublicPopupCtrl.Instance:Center(Language.PetStampPray.Tip9)
        return
    end
    ViewMgr:OpenView(PetStampResolveSetView)
end

function PetStampPrayView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[150].desc)
end

PetStampPrayPanel = PetStampPrayPanel or DeclareMono("PetStampPrayPanel", UIWFlushPanel)
function PetStampPrayPanel:PetStampPrayPanel()
    GuideManager.Instance:RegisterGetGuideUi("PetStampPrayBtn", function ()
        return self.PrayBtnObj, function ()
            self:OnPrayClick()
        end
    end)

    self.data = PetStampData.Instance
    self.data_cares = {
        {data = self.data.pray_data, func = self.FlushPanel, init_call = false}
    }
    self.chip_item_id = self.data.stamp_cfg.other[1].exchange_item_id
    self.sel_speed = 0.1
    self.curr_type = 1  -- 1是铜币，2是元宝
end

function PetStampPrayPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.cur_index = self.data:GetPrayPosIndex()
    self.PosToggleList[self.cur_index].isOn = true
    self.data:SetResloveData(0, true) -- 祈神时强化材料自动分解
    self:FlushDisplay()
    self:FlushPanel()
    self:FlushChipNum()
end

function PetStampPrayPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.run_ht ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_ht)
    end
    self.data.pray_data.result_seq_list = nil
    TimeHelper:CancelTimer(self.touch_ht)
    self.data:SetNotif(true)

    self:ShowResult()

    GuideManager.Instance:UnRegisterGetGuideUi("PetStampPrayBtn")
end

function PetStampPrayPanel:GetMaxSetCurrNum()
    if self.curr_type == 1 then
        return self.data.stamp_cfg.other[1].auto_coin_max
    else
        return self.data.stamp_cfg.other[1].auto_gold_max
    end
end

function PetStampPrayPanel:GetCurrType()
    return self.curr_type == 1 and CurrencyType.PetStampPrayItem or CurrencyType.Gold
end

function PetStampPrayPanel:GetCurrName()
    return self.curr_type == 1 and Language.PetStampPray.PrayItem or Language.PetStampPray.Gold
end

function PetStampPrayPanel:FlushPanel()
    self:FlushPosToggle()
end

function PetStampPrayPanel:FlushDisplay()
    local curr_tip = self.curr_type == 1 and Language.PetStampPray.CurrTip1 or Language.PetStampPray.CurrTip2
    UH.SetText(self.CurrTip, curr_tip)
    local curr_icon = self.curr_type == 1 and CommonItemId.PetStampPrayItem or CommonItemId.Gold
    for i = 1, self.CurrIconList:Length() do
        local v = self.CurrIconList[i]
        UH.SpriteName(v, curr_icon)
    end
    for i = 1, self.PriceList:Length() do
        local num = self.curr_type == 1 and self.data.stamp_cfg.pray[i].expend_coin or self.data.stamp_cfg.pray[i].expend_gold
        UH.SetText(self.PriceList[i], num)
    end
    self.coin_add = self.curr_type == 1 and self.data.stamp_cfg.other[1].auto_coin or self.data.stamp_cfg.other[1].auto_gold
    self.coin_num = self.coin_add
    self:FlushCoin()
end

function PetStampPrayPanel:FlushCoin()
    if self.curr_type == 1 then
        UH.SetText(self.CoinText, self.coin_num)
    else
        UH.SetText(self.GoldText, self.coin_num)
    end
end

function PetStampPrayPanel:FlushChipNum()
    local num = Item.GetNum(self.chip_item_id)
    UH.SetText(self.ChipNum, string.format(Language.PetStampPray.ChipNum1,num))
end

function PetStampPrayPanel:FlushPosToggle()
    self:PlayPosToggle()
end

function PetStampPrayPanel:ShowResult()
    self.data.is_pray_animation = false
    local item_list, other_list = self.data:GetPrayResultItemList()
    if not TableIsEmpty(item_list) then
        ViewMgr:OpenView(PetStampPrayResultView, {item_list = item_list, other_list = other_list})
    else
        if not TableIsEmpty(other_list) then
            PublicPopupCtrl.Instance:ShowCustom(other_list)
            PetStampCtrl.Instance:ReleaseAutoGetExp()
        end
    end
end

function PetStampPrayPanel:PlayPosToggle()
    self.result_list = self.data.pray_data.result_seq_list or {}
    self.stamp_index_list = self.data.pray_data.pet_stamp_index_list or {}
    self.len = #self.result_list
    if self.len < 1 then
        return
    end
    self.result_list_index = 1
    self.dt = 1
    self.pray_coin = 0
    self.is_praying = true
    is_open_pet_stamp_show_view = false
    self.data.is_pray_animation = true
    if self.run_ht ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_ht)
    end
    self.run_ht = Runner.Instance:RunUntilTrue(BindTool.Bind(self.RunEvent,self))
end

function PetStampPrayPanel:RunEvent()
    if is_open_pet_stamp_show_view then
        return
    end
    if self.result_list_index > self.len then
        self:RunEndEvent()
        return true
    end

    self.dt = self.dt + Time.deltaTime
    if self.dt >= self.sel_speed then
        self.cur_index = self.result_list[self.result_list_index]
        self.PosToggleList[self.cur_index].isOn = true
        self.dt = 0
        self.pray_coin = self.pray_coin + self.data.stamp_cfg.pray[self.cur_index].expend_coin

        -- 判断下当前奖励是否牛逼
        local reward_index = self.stamp_index_list[self.result_list_index]
        if reward_index and reward_index > 0 then
            local stamp = self.data:GetPetStampByIndex(reward_index)
            if stamp and stamp:QuaLevel() >= 5 then
                is_open_pet_stamp_show_view = true
                ViewMgr:OpenView(PetStampShowView, {pet_stamp = stamp})
            end
        end

        self.result_list_index = self.result_list_index + 1
    end
end

function PetStampPrayPanel:RunEndEvent()
    self:FlushChipNum()
    self.cur_index = self.data:GetPrayPosIndex()
    --LogError(string.format("客户端计算花费 %s 服务端计算%s",self.pray_coin,self.data.pray_data.consume_coin))
    self.data.pray_data.result_seq_list = nil
    self:ShowResult()
    self.data:SetNotif(true)
    self.is_praying = false
end

function PetStampPrayPanel:OnPrayClick()
    if self.is_praying then
        return
    end
    local coin_num = self.data:GetPrayCoin(self.cur_index, self.curr_type)
    self:OnPray(coin_num)
end

function PetStampPrayPanel:OnAutoPrayClick()
    if self.is_praying then
        return
    end
    if not MallCtrl.IsNotCurrency(self:GetCurrType(), self.coin_num) then
        return
    end
    local curr_name = self:GetCurrName()
    PublicPopupCtrl.Instance:AlertTip(string.format(Language.PetStampPray.Tip7,self.coin_num,curr_name),function ()
        self:OnPray(self.coin_num)
    end)
end

function PetStampPrayPanel:OnPray(coin_num)
    coin_num = coin_num or 0
    if not MallCtrl.IsNotCurrency(self:GetCurrType(), coin_num) then
        return
    end
    local min_num = self.data:GetPrayCoin(self.cur_index, self.curr_type)
    if coin_num < min_num then
        PublicPopupCtrl.Instance:Error(string.format(Language.PetStampPray.Tip1,min_num,self:GetCurrName()))
    end
    
    local is_auto_reslove_stuff = self.data:GetResloveData(0) and 1 or 0
    --LogError("祈神了",PET_OP_TYPE.PetStampPray, self.curr_type - 1, coin_num, is_auto_reslove_stuff)
    PetCtrl.SendPetOp(PET_OP_TYPE.PetStampPray, self.curr_type - 1, coin_num, is_auto_reslove_stuff)
    self.data:SetNotif(false)
end

function PetStampPrayPanel:OnCoinClick(value)
    local has_coin = RoleData.Instance:GetCurrency(self:GetCurrType())
    self.coin_num = self.coin_num + (value * self.coin_add)
    if self.coin_num < self.coin_add then
        self.coin_num = self.coin_add
    elseif self.coin_num > has_coin then
        PublicPopupCtrl.Instance:Error(string.format(Language.PetStampPray.Error1, self:GetCurrName()))
        self.coin_num = has_coin
        TimeHelper:CancelTimer(self.touch_ht)
    elseif self.coin_num > self:GetMaxSetCurrNum() then
        PublicPopupCtrl.Instance:Error(string.format(Language.PetStampPray.Error2, self:GetCurrName()))
        self.coin_num = self:GetMaxSetCurrNum()
        TimeHelper:CancelTimer(self.touch_ht)
    end
    self:FlushCoin()
end

function PetStampPrayPanel:OnUpEvent()
    TimeHelper:CancelTimer(self.touch_ht)
end

function PetStampPrayPanel:OnDownEvent(value)
    TimeHelper:CancelTimer(self.touch_ht)
	self.touch_ht = TimeHelper:AddRunTimer(BindTool.Bind(self.OnCoinClick,self,value),0.2)
end

function PetStampPrayPanel:OnCurSelClick(value)
    if value then
        self.curr_type = 2
    else
        self.curr_type = 1
    end
    self:FlushDisplay()
end

-- ============== 祈神结果 ===============
PetStampPrayResultView = PetStampPrayResultView or DeclareView("PetStampPrayResultView", "pet/pet_stamp/pet_stamp_store/pet_stamp_pray_result")
function PetStampPrayResultView:PetStampPrayResultView()
    self.data = PetStampData.Instance
end

function PetStampPrayResultView:LoadCallback(param_t)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.JinJie1)
    param_t = param_t or {}
    self.item_list = param_t.item_list or {}
    local len = #self.item_list
    self.List:SetObjActive(len <= 10 and len > 1)
    self.GridList:SetObjActive(len > 10)
    self.OneCell:SetObjActive(len == 1)
    if len <= 10 and len > 1 then
        self.List:SetData(self.item_list)
    elseif len > 10 then
        self.GridList:SetData(self.item_list)
    elseif len == 1 then
        self.OneCell:SetData(self.item_list[1])
        TimeHelper:CancelTimer(self.time_ht)
        self.time_ht = Invoke(function ()
            local info = {
                item_list = {self.item_list[1]}
            }
            ItemInfoCtrl.Instance:ItemInfoView(info)
            ItemInfoData.Instance:SetClickDataPos(Vector2.New(338,75))
        end,1)
    end

    if param_t.other_item then
        table.insert(self.item_list,param_t.other_item)
    end
    PublicPopupCtrl.Instance:ShowCustom(self.item_list)

    if param_t.other_list then
        PublicPopupCtrl.Instance:ShowCustom(param_t.other_list)
        self.other_list = param_t.other_list
    end

end

function PetStampPrayResultView:OnCloseClick()
    ViewMgr:CloseView(PetStampPrayResultView)
end

function PetStampPrayResultView:CloseCallback()
    TimeHelper:CancelTimer(self.time_ht)
    PetStampCtrl.Instance:ExcuteResloveSetEvent(self.item_list, {})
    PetStampCtrl.Instance:ReleaseAutoGetExp()
end


-- =============== 祈神记录 ===============
PetStampPrayRecordView = PetStampPrayRecordView or DeclareView("PetStampPrayRecordView", "pet/pet_stamp/pet_stamp_store/pet_stamp_pray_log_view")
function PetStampPrayRecordView:PetStampPrayRecordView()
    
end

function PetStampPrayRecordView:OnCloseClick()
    ViewMgr:CloseView(PetStampPrayRecordView)
end

PetStampPrayRecordPanel = PetStampPrayRecordPanel or DeclareMono("PetStampPrayRecordPanel", UIWFlushPanel)
function PetStampPrayRecordPanel:PetStampPrayRecordPanel()
    self.data = PetStampData.Instance
    self.data_cares = {
        {data = self.data.pray_log_data, func = self.FlushPanel, init_call = false},
    }
end

function PetStampPrayRecordPanel:Awake()
    UIWFlushPanel.Awake(self)
    -- 请求日志信息
    PetCtrl.SendPetOp(PET_OP_TYPE.PetPetStampLogList)
end

function PetStampPrayRecordPanel:FlushPanel()
    local list_data = self.data:GetPrayLogData()
    self.List:SetData(list_data)
end

PetStampPrayRecordItem = PetStampPrayRecordItem or DeclareMono("PetStampPrayRecordItem", UIWidgetBaseItem)
function PetStampPrayRecordItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Time, os.date("%Y-%m-%d",data.time))
    local stamp = PetStamp.New({item_id = data.item_id})
    UH.SetText(self.Name, stamp:Name())
    self.ItemCell:SetData(stamp)
end

function PetStampPrayRecordItem:OnLookInfoClick()
    PetCtrl.SendPetOp(PET_OP_TYPE.PetPetStampLogDetail, self.data.index)
end

-- =============== 神印兑换 ===============
PetStampConvertView = PetStampConvertView or DeclareView("PetStampConvertView", "pet/pet_stamp/pet_stamp_store/pet_stamp_canvert_view")
VIEW_DECLARE_LEVEL(PetStampConvertView, ViewLevel.L0)
function PetStampConvertView:LoadCallback(param_t)
    local is_open_red = FunOpen.Instance:IsFunOpen(Mod.PetStampPray.RedConvert)
    self.Interactor.Interactable = is_open_red == true
    local qua = PetStampData.Instance:DefaultSuitQua()
    self.GotoBtn:SetActive(qua >= ItemColor.Red)
end

function PetStampConvertView:OnCloseClick()
    ViewMgr:CloseView(PetStampConvertView)
end

function PetStampConvertView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[151].desc)
end

function PetStampConvertView:OnGotoClick()
    local is_open_red, open_tip = FunOpen.Instance:IsFunOpen(Mod.PetStampPray.RedConvert)
    if not is_open_red then
        if open_tip then
            PublicPopupCtrl.Instance:Center(open_tip)
        end
        return
    end
    ViewMgr:CloseView(PetStampConvertView)
    ViewMgr:OpenView(PetStampRedConvertView)
end


-- ============= 神印红色兑换 ============
PetStampRedConvertView = PetStampRedConvertView or DeclareView("PetStampRedConvertView", "pet/pet_stamp/pet_stamp_store/pet_stamp_red_convert_view", Mod.PetStampPray.RedConvert)
VIEW_DECLARE_LEVEL(PetStampRedConvertView, ViewLevel.L0)
function PetStampRedConvertView:OnCloseClick()
    ViewMgr:CloseView(PetStampRedConvertView)
end

function PetStampRedConvertView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[151].desc)
end

function PetStampRedConvertView:OnGotoClick()
    ViewMgr:OpenView(PetStampConvertView)
    ViewMgr:CloseView(PetStampRedConvertView)
end

PetStampConvertPanel = PetStampConvertPanel or DeclareMono("PetStampConvertPanel", UIWFlushPanel)
function PetStampConvertPanel:PetStampConvertPanel()
    self.data = PetStampData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false},
    }
    self.cihp_item_id = self.data.stamp_cfg.other[1].exchange_item_id
    self.sel_index = 1
    self:SetList()
    self.max_sel_index = #self.convert_list
end

function PetStampConvertPanel:SetList()
    self.convert_list = self.data:GetConvertItemListData(5)
end

function PetStampConvertPanel:Awake()
    UIWFlushPanel.Awake(self)
    UH.SetIcon(self.ChipIcon, Item.GetIconId(self.cihp_item_id))
    self:FlushPanel()
    local qua = self.Qua or 5
    local goto_text = qua == 5 and Language.PetStampPray.GoToText1 or Language.PetStampPray.GoToText2
    UH.SetText(self.GoToText, goto_text)
end

function PetStampConvertPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data.is_converting = false
end

function PetStampConvertPanel:FlushPanel()
    local chip_num = Item.GetNum(self.cihp_item_id)
    UH.SetText(self.ChipNum, string.format(Language.PetStampPray.ChipNum2,chip_num))
    local list = self.convert_list[self.sel_index]
    self.RightBtnObj:SetActive(self.sel_index + 1 <= self.max_sel_index)
    self.LeftBtnObj:SetActive(self.sel_index - 1 >= 1)
    for i = 1, self.Items:Length() do
        local data = list[i]
        self.Items[i]:SetObjActive(data ~= nil)
        if data then
            self.Items[i]:SetData(data)
        end
    end
end

function PetStampConvertPanel:OnNextClick(value)
    self.sel_index = self.sel_index + value
    self:FlushPanel()
end

-- 红色神印兑换
PetStampRedConvertPanel = PetStampRedConvertPanel or DeclareMono("PetStampRedConvertPanel", PetStampConvertPanel)
function PetStampRedConvertPanel:SetList()
    self.convert_list = self.data:GetConvertItemListData(6)
end

PetStampConvertItem = PetStampConvertItem or DeclareMono("PetStampConvertItem", UIWidgetBaseItem)
function PetStampConvertItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local suit_id = data.god_group
    local qua = data.star_level
    local suit_group = PetStampData.Instance.suit_group[suit_id][qua]
    local chip_item_id = PetStampData.Instance.stamp_cfg.other[1].exchange_item_id
    UH.SetText(self.Name, suit_group[1].god_print_name)
    local num = Item.GetNum(chip_item_id)
    local price = DataHelper.ConsumeNum(num,data.expend, true)
    UH.SetText(self.Price, price)
    self.Interactor.Interactable = num >= data.expend
    UH.SpriteName(self.Img, data.god_big_icon)
    UH.SetIcon(self.ChipIcon, Item.GetIconId(chip_item_id))
end

function PetStampConvertItem:OnConvertClick()
    local chip_item_id = PetStampData.Instance.stamp_cfg.other[1].exchange_item_id
    local num = Item.GetNum(chip_item_id)
    if num < self.data.expend then
        PublicPopupCtrl.Instance:Error(Language.PetStampPray.Error3)
        return
    end
    PetCtrl.SendPetOp(PET_OP_TYPE.PetPetStampExchange, self.data.exchange_seq)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    PetStampData.Instance.is_converting = true
end

function PetStampConvertItem:OnIconClick()
    ViewMgr:OpenView(PetStampSuitInfoDialog, {suit_id = self.data.god_group, qua = self.data.star_level})
end

-- =============== 分解设置 ===============
PetStampResolveSetView = PetStampResolveSetView or DeclareView("PetStampResolveSetView", "pet/pet_stamp/pet_stamp_resolve_view")
function PetStampResolveSetView:PetStampResolveSetView()
    self.BG:SetData(self:GetType(),Language.PetStampPray.Title1,Language.PetStampPray.Tip2)
    if ViewMgr:IsOpen(PetStampPrayView) then
        UH.SetText(self.BG.Title2,Language.PetStamp.ResloveTitle1)
    else
        UH.SetText(self.BG.Title2,Language.PetStamp.ResloveTitle2)
    end
end

function PetStampResolveSetView:OnCloseClick()
    ViewMgr:CloseView(PetStampResolveSetView)
end

PetStampResolveSetPanel = PetStampResolveSetPanel or DeclareMono("PetStampResolveSetPanel", UIWFlushPanel)
function PetStampResolveSetPanel:PetStampResolveSetPanel()
    self.data = PetStampData.Instance
    self.qua_list = {1,2,3,4,5,0}
    self.data_cares = {
        {data = self.data.exp_info, func = self.FlushExp, init_call = false},
    }
end

function PetStampResolveSetPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.List:SetData(self.qua_list)
    self:FlushExp()
end

function PetStampResolveSetPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if not ViewMgr:IsOpen(PetStampPrayView) then
        self.data:ClearResloveData()
    end
end

function PetStampResolveSetPanel:FlushExp()
    UH.SetText(self.ExpDesc, string.format(Language.PetStampPray.Tip3,self.data.exp_info.exp or 0))
end

function PetStampResolveSetPanel:OnToggleEvent(value, data)
    if value then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    end
    self.data:SetResloveData(data.qua, value)
    self.data:SetResloveRecord(data.qua, value)
end

function PetStampResolveSetPanel:OnResloveClick()
    PetStampCtrl.Instance:ExcuteResloveSetEvent()
end

PetStampResolveSetItem = PetStampResolveSetItem or DeclareMono("PetStampResolveSetItem", UIWidgetBaseItem)
function PetStampResolveSetItem:SetData(data)
    UIWidgetBaseItem.SetData(self, {qua = data})
    local name
    local is_on
    if data > 0 then
        name = string.format(Language.PetStampPray.Tip4,DataHelper.GetDaXie(data))
        is_on = PetStampData.Instance:GetResloveRecord(data) > 0--PetStampData.Instance:GetResloveData(data)
    else
        name = Language.PetStampPray.Tip8
        is_on = true
    end
    UH.SetText(self.Name, name)
    self.Seletor.isOn = is_on
end

-- 分解弹窗
PetStampResloveDialog = PetStampResloveDialog or DeclareView("PetStampResloveDialog", "pet/pet_stamp/pet_stamp_reslove_dialog")
function PetStampResloveDialog:PetStampResloveDialog()
    
end

function PetStampResloveDialog:LoadCallback(paramt)
    paramt = paramt or {}
    self.item = paramt.item
    self.func = paramt.func
    self.Cell:SetData(self.item)
    UH.SetText(self.Title, Language.PetStampPray.Tip5)
    local exp = PetStampData.Instance:GetStampPriceExp(self.item)
    UH.SetText(self.Tip,string.format(Language.PetStampPray.Tip6,exp))
end

function PetStampResloveDialog:OnOkClick()
    if self.func then
        self.func()
    end
    ViewMgr:CloseView(PetStampResloveDialog)
end

function PetStampResloveDialog:OnCancelClick()
    ViewMgr:CloseView(PetStampResloveDialog)
end



-- ========= 牛逼神印展示界面 =========
PetStampShowView = PetStampShowView or DeclareView("PetStampShowView", "pet/pet_stamp/pet_stamp_show_view")
VIEW_DECLARE_LEVEL(PetStampShowView, ViewLevel.L0)
function PetStampShowView:PetStampShowView()
    
end

-- @param{pet_stamp:PetStamp}
function PetStampShowView:LoadCallback(param)
    param = param or {}
    local pet_stamp = param.pet_stamp
    if not pet_stamp then
        return
    end
    is_open_pet_stamp_show_view = true
    self.pet_stamp = pet_stamp
    self:FlushPanel()
end

function PetStampShowView:CloseCallback()
    is_open_pet_stamp_show_view = false
end

function PetStampShowView:FlushPanel()
    self:FlushInfo()
    self:FlushSuit()
end

function PetStampShowView:FlushSuit()
    local suit_id = self.pet_stamp:SuitId() or 0
    if suit_id <= 0 then
        return
    end
    local qua = self.pet_stamp:QuaLevel()
    local suit_group = PetStampData.Instance.suit_group[suit_id][qua]
    self.DeskList[1]:SetObjActive(qua == 5)
    self.DeskList[2]:SetObjActive(qua == 6)
    local desk_info = {name = suit_group[1].god_print_name, icon = suit_group[1].big_god_print_icon}
    if qua == 5 then
        self.DeskList[1]:SetData(desk_info)
    elseif qua == 6 then
        self.DeskList[2]:SetData(desk_info)
    end
    local suit_desc = self.pet_stamp:SuitDesc()
    UH.SetText(self.SuitDesc, suit_desc)
end

function PetStampShowView:FlushInfo()
    local pos_index = self.pet_stamp:PosIndex()
    local pos_str = DataHelper.GetDaXie(pos_index,true)
    UH.SetText(self.PosText, pos_str)
    UH.SetText(self.PosDesc, string.format(Language.PetStamp.PosDesc,pos_str))
    local attri_list = self.pet_stamp:AttriListInfo()

    self.MainAttri:SetData(attri_list[1])
    local list = {}
    for i = 2, #attri_list do
        table.insert(list, attri_list[i])
    end
    self.AttriList:SetData(list)
end

function PetStampShowView:OnCloseClick()
    ViewMgr:CloseView(PetStampShowView)
end

PetStampShowAttriItem = PetStampShowAttriItem or DeclareMono("PetStampShowAttriItem", UIWidgetBaseItem)
function PetStampShowAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if not data.up_add_attri then
        local type_str = data.is_main and Language.PetStamp.Attri6 or Language.PetStamp.Attri7
        UH.SetText(self.Type, type_str)
        local name = Language.Common.AttrList[data.attri_type]
        UH.SetText(self.Name, name)
        local value_str
        if DataHelper.IsPercent(data.attri_type) then
            value_str = string.format("%s%%",data.attri_value/10)
        else
            value_str = string.format("%s",data.attri_value)
        end
        UH.SetText(self.Value, value_str)
        if self.TipText then
            UH.SetText(self.TipText,"")
        end
    else
        UH.SetText(self.Type, "")
        UH.SetText(self.Name, "")
        UH.SetText(self.Value, "")
        if self.TipText then
            UH.SetText(self.TipText,string.format(Language.PetStamp.Attri5, data.up_add_attri))
        end
    end
end

-- 展示界面的那个台子
PetStampShowViewDesk = PetStampShowViewDesk or DeclareMono("PetStampShowViewDeskItem")
-- data ::: {name,icon}
function PetStampShowViewDesk:SetData(data)
    UH.SetText(self.Name, data.name)
    UH.SpriteName(self.Icon, data.icon)
end