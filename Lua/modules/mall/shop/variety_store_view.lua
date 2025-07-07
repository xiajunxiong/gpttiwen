VarietyStoreView = VarietyStoreView or DeclareMono("VarietyStoreView", UIWFlushPanel)

function VarietyStoreView:VarietyStoreView()
    self.data = StoreData.Instance
    self.data_cares = {
        {data = self.data.select_data,func = self.FlushStoreList,init_call = false},
        {data = self.data.count_data, func = self.FlushStoreList,init_call = false},
        {data = self.data.red_point_data, func = self.FlushRedNumView,init_call = false},
        {data = RoleData.Instance.currencys,func = self.FlushCurrency,init_call = false},
    }
end

function VarietyStoreView:Awake()
    UIWFlushPanel.Awake(self)
    self.BtnList:SetDataList(self.data.btn_list,self.data:GetSelectType())
end

function VarietyStoreView:FlushStoreList()
    local data = self.data:GetSelectData()
    self.StoreList:SetData(self.data:GetPageData(data.page))
    self:FlushTopView(data,Language.Store.PriceYype[data.price_type])
end

function VarietyStoreView:FlushRedNumView()
    for k,item in pairs(self.BtnList.item_list) do
        item:FlushRedNumView()
    end
end

function VarietyStoreView:FlushTopView(data,btn_text)
    if data.price_type ~= 0 then
        self:FlushCurrency(data)
    else
        UH.SpriteName(self.StoreTop,[[StoreTop]]..data.top_type)
    end
    UH.SetText(self.TipBtnText,btn_text)
    self.LimitShop:SetActive(data.price_type == 0)
    self.TipBtnObj:SetActive(data.price_type ~= 0 and btn_text ~= nil)
end

function VarietyStoreView:FlushCurrency(data)
    data = data.price_type and data or self.data:GetSelectData()
    UH.SetGold(self.Gold,data.price_type)
    local name = Item.GetName(HuoBi[data.price_type])
    UH.SetText(self.Currency,RoleData.Instance:GetCurrency(data.price_type))
    UH.SetText(self.ShowTip,data.top_desc ~= "" and data.top_desc or Format(Language.Store.ShowTip,name))
end

function VarietyStoreView:OnClickHelp()
    local data = self.data:GetSelectData()
    if data.help_desc and data.help_desc ~= 0 then
        ViewMgr:OpenView(TipPopupView,Config.language_cfg_auto.textdesc[data.help_desc].desc)
    end
end

function VarietyStoreView:OnClickTip()
    local data = self.data:GetSelectData()
    if data == nil then return end
    MallCtrl.Instance:OpenGetView(data.price_type)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
end

function VarietyStoreView:OnGoldClick()
    MainOtherCtrl.Instance:GetWayView({item = Item.Init(HuoBi[self.data:GetSelectData().price_type])})
end

-----------------------------StoreBtnItem---------------------------------
StoreBtnItem = StoreBtnItem or DeclareMono("StoreBtnItem", UIWidgetBaseItem)

function StoreBtnItem:SetData(data)
    UH.SetText(self.Name,data.name)
    UH.SpriteName(self.IconOn,[[StoreIconOn]]..data.page)
    UH.SpriteName(self.IconOff,[[StoreIconOff]]..data.page)
    self.Inter.Interactable = RoleLevel() >= data.limit_level
    self.Blank:SetActive(RoleLevel() < data.limit_level)
    UIWidgetBaseItem.SetData(self, data)
    self:FlushRedNumView()
end

function StoreBtnItem:FlushRedNumView()
    self.RedPos:SetNum(StoreData.Instance:GetBtnRedNum(self.data))
end

function StoreBtnItem:OnClickItem()
    StoreData.Instance:SetSelectData(self.data)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
end

function StoreBtnItem:OnClickBlank()
    PublicPopupCtrl.Instance:Center(Format(Language.Shop.LevelLimitTip,self.data.limit_level or 0))
end

function StoreBtnItem:Click()
    self.ToggleClick:Trigger()
end

-----------------------------StoreItem-----------------------------
StoreItem = StoreItem or DeclareMono("StoreItem", UIWidgetBaseItem)

function StoreItem:SetData(data)
    data.item = Item.Create(data)
    self.Call:SetData(data.item)
    UH.SetText(self.Price,data.price)
    UH.SetText(self.Name,data.item:Name())
    data.limit_times = StoreData.Instance:GetHasTimes(data)
    data.btn_name = self:GetShowText()
    
    self.RedPos:SetNum(StoreData.Instance:GetBtnItemNum(data))
    self.Label:SetActive(data.discount and data.discount > 0)
    UH.SetText(self.Discount,Format(Language.Shop.Discount,data.discount or 0))

    self:FlushIconView(data)
    self:FlushTimesView(data)
    self:FlushFreeView(data)
    self:FlushCondition(data)

    UIWidgetBaseItem.SetData(self, data)
end

function StoreItem:OnDestroy()
	TimeHelper:CancelTimer(self.ShowTimer)
end

--购买或者兑换
function StoreItem:OnClickItem()
    if MallCtrl.IsCanRequire(self.data.require_type,self.data.require_value) then
        PublicPopupCtrl.Instance:Center(MallData.GetRequireValue(self.data.require_type,self.data.require_value))
        return
    end
    if self.data.limit_type ~= 0 and self.data.limit_times == 0 then
        local limit_show = Format(Language.Store.LimitTypeTip[self.data.limit_type],self.data.btn_name)
        PublicPopupCtrl.Instance:Center(limit_show .. Language.Shop.NotHasTip)
        return
    end
    self.data.content_func = function(data)
        ShoppingCtrl.Instance:SendStoreBuyReq(data)
        ViewMgr:CloseView(BatchBuyView)
    end
    ViewMgr:OpenView(BatchBuyView,self.data)
end

--刷新次数条件
function StoreItem:FlushTimesView(data)
    local type,value = data.require_type,data.require_value
    if MallCtrl.IsCanRequire(type,value) and not self:IsSellOut(data) then
        UH.SetText(self.Limit,MallData.GetRequireValue(type,value))
    else
        UH.SetText(self.Limit,data.limit_type ~= 0 and Format("%s：%s",self:GetLimitShow(data.limit_type),data.limit_times) or "")
    end
    self:FlushSellOut(self:IsSellOut(data))
end

--免费道具
function StoreItem:FlushFreeView(data)
    if data.price == 0 then
        TimeHelper:CancelTimer(self.ShowTimer)
        self.ShowTimer = Invoke(function()
            self.BtnFree:SetActive(StoreData.Instance:GetLimitTimes(data.conver_type,data.seq) == 0)
        end)
        UH.SetText(self.Price,Language.Common.Free)
    else
        self.BtnFree:SetActive(false)
    end
end

--设置货币图标
function StoreItem:FlushIconView(data)
    local item_id = data.conver_item_id
    if item_id ~= 0 then
        UH.SetIcon(self.Icon,Item.GetIconId(item_id),ICON_TYPE.ITEM)
    end
    self.Icon:SetObjActive(item_id ~= 0)
    UH.SetGold(self.Gold,data.price_type)
end

-- 设置是否售罄的状态
function StoreItem:FlushSellOut(value)
    self.Limit:SetObjActive(not value)
    self.SellOut:SetActive(value)
end

--刷新前置条件的Item
function StoreItem:FlushCondition(data)
    local config = StoreData.Instance:GetItemConfigSeq(data.conver_type,data.last_seq)
    if config ~= nil then
        self:SetObjActive(StoreData.Instance:GetHasTimes(config) == 0)
    end
end

--是否售罄
function StoreItem:IsSellOut(data)
    return data.limit_type ~= 0 and data.limit_convert_count ~= 0 and data.limit_times == 0
end

--获取限购文字
function StoreItem:GetLimitShow(limit_type)
    return limit_type == 4 and Language.Shop.StoreLimitTip[StoreData.Instance:GetShowType()] or Language.Store.LimitTypeTips[limit_type]
end

--获取兑换/购买
function StoreItem:GetShowText()
    return Language.Store.ShowType[StoreData.Instance:GetShowType()]
end