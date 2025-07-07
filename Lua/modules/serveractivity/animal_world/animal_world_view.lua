AnimalWorldView = AnimalWorldView or DeclareView("AnimalWorldView", "animal_world/animal_world", Mod.AnimalWorld.Main)
VIEW_DECLARE_LEVEL(AnimalWorldView, ViewLevel.L1)
VIEW_DECLARE_MASK(AnimalWorldView, ViewMask.BgBlock)
function AnimalWorldView:AnimalWorldView()
    self.ObjBgEffect:SetActive(true)
    self.Data = AnimalWorldData.Instance
    self.data = self.Data.now_show_panel
    self.data.mod_id = Mod.AnimalWorld.Main
    self.ht = self.data:Care(BindTool.Bind(self.FlushShowPanel,self))
    self.ht2 = self.Data.info_data:Care(BindTool.Bind(self.JudgeIsOpen,self), "base")
    self:SelectEffectShow()
    self:JudgeIsOpen()
    TimeHelper:CancelTimer(self.handler)
    self.handler = Invoke(function()
        self.ObjBgEffect:SetActive(false)
    end, 3.5)
    TimeHelper:CancelTimer(self.handler2)
    self.handler2 = Invoke(function()
        self.ObjBg:SetActive(true)
        self.ObjTab:SetActive(true)
    end, 2)
end

function AnimalWorldView:FlushShowPanel()
    if self.data.mod_id == Mod.AnimalWorld.Main then
        UH.SetToggle(self.Toggles[1], true)
    elseif self.data.mod_id == Mod.AnimalWorld.Shop then
        UH.SetToggle(self.Toggles[2], true)
    elseif self.data.mod_id == Mod.AnimalWorld.Task then
        UH.SetToggle(self.Toggles[3], true)
    end
end

function AnimalWorldView:JudgeIsOpen()
    if not self.exchange_group then
        self.exchange_group = self.Data:ExchangeGroup()
    elseif self.exchange_group ~= self.Data:ExchangeGroup() then
        PublicPopupCtrl.Instance:Center(Language.AnimalWorld.ActChangeTip)
        ViewMgr:CloseView(AnimalWorldView)
        ViewMgr:CloseView(AnimalWorldDetailView)
    elseif not self.Data:IsOpen() then
        PublicPopupCtrl.Instance:Center(Language.AnimalWorld.ActCloseTip)
        ViewMgr:CloseView(AnimalWorldView)
        ViewMgr:CloseView(AnimalWorldDetailView)
    end
end

-- 根据交换类型显示特效，兑换界面和详情界面需要这样处理
function AnimalWorldView:SelectEffectShow()
    local exchange_group = self.Data:ExchangeGroup()
    for i = 1, self.Effects:Length() do
        self.Effects[i]:SetActive(i == exchange_group)
    end
end

function AnimalWorldView:CloseCallback()
    self.data:Uncare(self.ht)
    self.Data.info_data:Uncare(self.ht2)
    TimeHelper:CancelTimer(self.handler)
    TimeHelper:CancelTimer(self.handler2)
end


----------------------------AnimalWorldMain----------------------------
AnimalWorldMain = AnimalWorldMain or DeclareMono("AnimalWorldMain", UIWFlushPanel)
function AnimalWorldMain:AnimalWorldMain()
    self.Data = AnimalWorldData.Instance
    self.language = Language.AnimalWorld
    self.data = self.Data.info_data
    -- self.data_cares = {
    --     {data = self.data, func = self.FlushAll, init_call = false},
    -- }
end

function AnimalWorldMain:Awake()
    UIWFlushPanel.Awake(self)
    local time_cfg = self.Data:TimeInfo()
    local pet_show_info = self.Data:PetShowInfo()
    UH.SetText(self.TxtTimeShow, Format(self.language.ActTime, time_cfg.time_show))

    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.Model:SetShowData({ui_obj = self.ui_obj, view = self})
    end
    local pet_cfg = Cfg.PetById(pet_show_info.pet_id)
    UH.SetText(self.TxtBtn, Format(self.language.Introduce, pet_cfg.name))
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(pet_cfg.res_id))
end

function AnimalWorldMain:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Delete(self, "ui_obj")
end

function AnimalWorldMain:OnCloseClick()
    ViewMgr:CloseView(AnimalWorldView)
end

function AnimalWorldMain:OnIntroduceClick()
    AnimalWorldCtrl.Instance:OpenPetInfo()
end

function AnimalWorldMain:OnLeftClick()
    self.Data.now_show_panel.mod_id = Mod.AnimalWorld.Shop
end

function AnimalWorldMain:OnRightClick()
    self.Data.now_show_panel.mod_id = Mod.AnimalWorld.Task
end

----------------------------AnimalWorldShop----------------------------
AnimalWorldShop = AnimalWorldShop or DeclareMono("AnimalWorldShop", UIWFlushPanel)
function AnimalWorldShop:AnimalWorldShop()
    self.Data = AnimalWorldData.Instance
    self.language = Language.AnimalWorld
    self.data = self.Data.info_data
    self.exchange_cfg = self.Data:ExchangeConfig()
    self.pet_show_info = self.Data:PetShowInfo()
    self.Data.shop_data.index = 1
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
        {data = self.Data.shop_data, func = self.FlushSelect, init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
    -- 刷新道具数量
    if self.item_listen == nil then
        self.item_listen = BagData.Instance:ListenItems(self.Data:GetItemListen(), BindTool.Bind(self.FlushItem, self))
    end
end

function AnimalWorldShop:Awake()
    UIWFlushPanel.Awake(self)
    for i = 1, self.List:Length() do
        self.List[i]:SetData(self.exchange_cfg[i])
    end
    UH.SetToggle(self.List[1].Selector, true)

    local pet_cfg = Cfg.PetById(self.pet_show_info.pet_id)
    UH.SetText(self.TxtName, pet_cfg.name)
    UH.SpriteName(self.ImgPet, PetData.PetLevel2[pet_cfg.quality])
    self.PetSign:SetType(pet_cfg.limit_type)
    
    self.Timer:SetCallBack(function()
        PublicPopupCtrl.Instance:Center(self.language.ActCloseTip)
        ViewMgr:CloseView(AnimalWorldView)
    end)
    self:SelectEffectShow()
    self:FlushAll()
end

function AnimalWorldShop:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    BagData.Instance:RemoveListenItems(self.item_listen)
end

function AnimalWorldShop:SelectEffectShow()
    local exchange_group = self.Data:ExchangeGroup()
    for i = 1, self.Effects:Length() do
        self.Effects[i]:SetActive(i == exchange_group)
    end
end

function AnimalWorldShop:FlushAll()
    local index = self.Data.shop_data.index
    local show_cfg = self.exchange_cfg[index]
    self.item_id = show_cfg.item_id
    local time_cfg = self.Data:TimeInfo()
    self.Timer:StampTime(time_cfg.end_time, TimeType.Type_Time_8)
    self:FlushSelect()
    self:FlushItem()
    
end

function AnimalWorldShop:FlushItem()
    UH.SetText(self.TxtHasNum, Item.GetNum(self.item_id))
end

function AnimalWorldShop:FlushSelect()
    local index = self.Data.shop_data.index
    local show_cfg = self.exchange_cfg[index]
    local buy_cfg = self.Data:BuyConfig()
    local is_buy = self.data.info.is_buy == 1
    local is_empty = self.Data:IsEmptyCount(show_cfg.seq)
    self.IHandler.Interactable = not is_empty
    -- 兑换完

    self.ObjBtnNull:SetActive(is_empty)
    self.ObjBtnEffect:SetActive(not is_empty)
    -- 是否买了
    self.TxtMoneyBtn:SetObjActive(not is_buy)
    self.TxtConsume:SetObjActive(is_buy)
    UH.SetText(self.TxtMoneyBtn, Format(self.language.Price, buy_cfg.price / 10))
    UH.SetText(self.TxtConsume, Format(self.language.Price2, show_cfg.num))
    local config = Item.GetConfig(self.item_id)
    UH.SetIcon(self.Icon, config.icon_id, ICON_TYPE.ITEM)
    UH.SetIcon(self.Icon2, config.icon_id, ICON_TYPE.ITEM)
    UH.SetText(self.TxtTimes, Format(self.language.Times[show_cfg.limit_type], self.Data:ItemCanTimes(show_cfg.seq)))
    self:FlushProg()
end

function AnimalWorldShop:FlushProg()
    self.Progress:SetProgress(self.data.info.satisfy / self.Data:SatisfyFull())
    self.Progress:SetText(Format(Language.Common.Slash, self.data.info.satisfy, self.Data:SatisfyFull()))
end

function AnimalWorldShop:OnReturnClick()
    self.Data.now_show_panel.mod_id = Mod.AnimalWorld.Main
end

function AnimalWorldShop:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[313].desc)
end

function AnimalWorldShop:OnBtnClick()
    if self.data.info.is_buy == 1 then
        local index = self.Data.shop_data.index
        local show_cfg = self.exchange_cfg[index]
        if show_cfg.unlock_integral > self.data.info.satisfy then
            PublicPopupCtrl.Instance:AlertTip(self.language.SatisfyTip, function()
                self.Data.now_show_panel.mod_id = Mod.AnimalWorld.Task
            end)
        else
            local index = self.Data.shop_data.index
            local show_cfg = self.exchange_cfg[index]
            if MallCtrl.IsNotCurrency(nil, show_cfg.num, self.item_id) then
                AnimalWorldCtrl.Instance:Send(AnimalWorldType.Exchange, show_cfg.seq)
            end
        end
    else
        AnimalWorldCtrl.Instance:Send(AnimalWorldType.Buy)
    end
end

function AnimalWorldShop:SendBuy()
    local buy_cfg = self.Data:BuyConfig()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_ANIMAL_WORLD, buy_cfg.price)
end

function AnimalWorldShop:OnIntroduceClick()
    AnimalWorldCtrl.Instance:OpenPetInfo()
end

function AnimalWorldShop:OpenGetWay()
    ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = self.item_id})})
end

----------------------------AnimalWorldTask----------------------------
AnimalWorldTask = AnimalWorldTask or DeclareMono("AnimalWorldTask", UIWFlushPanel)
function AnimalWorldTask:AnimalWorldTask()
    self.Data = AnimalWorldData.Instance
    self.language = Language.AnimalWorld
    self.pet_show_info = self.Data:PetShowInfo()
    self.data = self.Data.info_data
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = true},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end

function AnimalWorldTask:Awake()
    UIWFlushPanel.Awake(self)
    local pet_cfg = Cfg.PetById(self.pet_show_info.pet_id)
    UH.SetText(self.TxtName, pet_cfg.name)
    UH.SpriteName(self.ImgPet, PetData.PetLevel2[pet_cfg.quality])

    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.Model:SetShowData({ui_obj = self.ui_obj, view = self})
    end
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(pet_cfg.res_id))

    local size = Vector2.New(758, self.Data:IsShowFB() and 345 or 540)
    self.RectDaily.sizeDelta = size
    local pos = Vector2.New(0, self.Data:IsShowFB() and -109.5 or -207)
    UH.AnchoredPosition(self.RectDaily, pos)

    self.ObjFB:SetActive(self.Data:IsShowFB())
end

function AnimalWorldTask:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Delete(self, "ui_obj")
end

function AnimalWorldTask:FlushAll()
    if self.data.info.is_buy == 0 then
        local buy_cfg = self.Data:BuyConfig()
        UH.SetText(self.TxtMoneyBtn, Format(self.language.Price, buy_cfg.price / 10))
    else
        UH.SetText(self.TxtMoneyBtn, self.language.Price3)
    end
    self.TaskList:SetData(self.Data:TaskConfig())
    self.FBList:SetData(self.Data:IsShowFB() and self.Data:FBConfig() or {})
    self.ObjBtnBuy:SetActive(self.data.info.is_buy == 0)
    self:FlushProg()
end

function AnimalWorldTask:OnReturnClick()
    self.Data.now_show_panel.mod_id = Mod.AnimalWorld.Main
end

function AnimalWorldTask:FlushProg()
    self.Progress:SetProgress(self.data.info.satisfy / self.Data:SatisfyFull())
    self.Progress:SetText(Format(Language.Common.Slash, self.data.info.satisfy, self.Data:SatisfyFull()))
end

function AnimalWorldTask:OnBtnClick()
    if self.data.info.is_buy == 0 then
        AnimalWorldCtrl.Instance:Send(AnimalWorldType.Buy)
    end
end

function AnimalWorldTask:SendBuy()
    local buy_cfg = self.Data:BuyConfig()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_ANIMAL_WORLD, buy_cfg.price)
end

function AnimalWorldTask:OnIntroduceClick()
    AnimalWorldCtrl.Instance:OpenPetInfo()
end

function AnimalWorldTask:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[313].desc)
end

--------------AnimalWorldShopItem--------------
AnimalWorldShopItem = AnimalWorldShopItem or DeclareMono("AnimalWorldShopItem", UIWidgetBaseItem)
function AnimalWorldShopItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create(data.reward_item)
    UH.SetText(self.TxtName, item:Name())
    if self.Cell then
        self.Cell:SetData(item)
    end
end

function AnimalWorldShopItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        AnimalWorldData.Instance.shop_data.index = self.data.seq + 1
    end
end

--------------AnimalWorldTaskItem--------------
AnimalWorldTaskItem = AnimalWorldTaskItem or DeclareMono("AnimalWorldTaskItem", UIWidgetBaseItem)
function AnimalWorldTaskItem:AnimalWorldTaskItem()
    self.Data = AnimalWorldData.Instance
end

function AnimalWorldTaskItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, data.task_desc)
    self.Progress:SetProgress(self.Data:TaskParam(data.seq) / data.parameter)
    self.Progress:SetText(Format(Language.Common.Slash, self.Data:TaskParam(data.seq), data.parameter))
    self.ItemList:SetData(DataHelper.FormatItemList(data.reward_item))
    local is_can_receive = self.Data:TodayTaskFinish(data.seq) and not self.Data:IsGetReward(data.seq)
    self.ObjReceive:SetActive(self.Data:TodayTaskFinish(data.seq))
    UH.SetText(self.TxtReceive, Language.AnimalWorld.Receive[self.Data:IsGetReward(data.seq) and 1 or 0])
    if self.Data.info_data.info.is_buy == 1 then
        self.Remind:SetNum(is_can_receive and 1 or 0)
    else
        self.Remind:SetNum(0)
    end
    self.ObjEffect:SetActive(is_can_receive)
    self.IHandler.Interactable = not self.Data:IsGetReward(data.seq)
end

function AnimalWorldTaskItem:OnGotoClick()
    if self.data.jump_mod_key > 0 then
        ViewMgr:OpenViewByKey(self.data.jump_mod_key)
        ViewMgr:CloseView(AnimalWorldView)
    end
end

function AnimalWorldTaskItem:OnReceiveClick()
    if self.Data.info_data.info.is_buy == 0 then
        PublicPopupCtrl.Instance:Center(Language.AnimalWorld.NotActiveTip)
        return
    end
    if not self.Data:IsGetReward(self.data.seq) then
        AnimalWorldCtrl.Instance:Send(AnimalWorldType.Task, self.data.seq)
    end
end

--------------AnimalWorldFBItem--------------目前没有副本，也没有协议，未实现
AnimalWorldFBItem = AnimalWorldFBItem or DeclareMono("AnimalWorldFBItem", UIWidgetBaseItem)
function AnimalWorldFBItem:AnimalWorldFBItem()
    self.Data = AnimalWorldData.Instance
end

function AnimalWorldFBItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, data.secret_area_name)
    self.ItemList:SetData(DataHelper.FormatItemList(data.show_item))
    UH.SetText(self.TxtProgress, Format(Language.AnimalWorld.FBTimes, self.Data:FBCount(data.seq), 0))
end

function AnimalWorldFBItem:OnGotoClick()
    if self.data.jump_mod_key > 0 then
        ViewMgr:OpenViewByKey(self.data.jump_mod_key)
        ViewMgr:CloseView(AnimalWorldView)
    end
end

function AnimalWorldTaskItem:OnRewardClick()
    PublicPopupCtrl.Instance:Center("点击了奖励预览")
end