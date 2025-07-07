MARKET_BUY_MAX_NUM = 999

MarketView = MarketView or DeclareMono("MarketView", UIWFlushPanel)

function MarketView:MarketView()
    self.data = MarketData.Instance
    self.data_cares = {
        {data = self.data.count_data,  func = self.FlushShopView,init_call = false,keys = {"val"}},
        {data = self.data.select_data, func = self.FlushShopItem,init_call = false,keys = {"val"}},
    }
    self:GuideInit()
end

function MarketView:Awake()
    UIWFlushPanel.Awake(self)
    self.Toggle[self.data:GetSelectType()].isOn = true
    self:OnClickType(self.data:GetSelectType())
    LoginCtrl.Instance:RoleSubscribe(RoleSubscribeTopicType.Market)
    self.InputField.onValueChanged:AddListener(BindTool.Bind(self.OnClickNumSelect, self))
    if BehaviorData.Instance.auto_behavior > 0 then
        self.timer_auto_beh = TimeHelper:AddDelayTimer(function ()
            self:OnClickPurchase()
        end,
        BehaviorData.Instance.auto_behavior)
    end
end

function MarketView:GuideInit()
    GuideManager.Instance:RegisterGetGuideUi("MarketSelectSellStone", function ()
        if self.SellList and self.SellList.index_item then
            local index = self:GetGuideIndex()
            if index and index > 8 then
                local height = (math.ceil(index / 2) - 5) * 120 + 68
                self.SellList:ForceJumpVertical(height)
            end
            if self.SellList.index_item[index] then
                self.SellList.index_item[index]:OnSelect(true)
                return self.SellList.index_item[index]
            end
        end
    end)
end

function MarketView:GetGuideIndex()
    local list = self.data:GetSellList()
    for i,v in ipairs(list) do
        if Item.GetType(v.item_id) == ItemType.Gem and WorkshopData.Instance:GetProfNotRecommendAllPart(v.item_id) then
            return i
        end
    end
end

function MarketView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
	LoginCtrl.Instance:RoleSubscribeCancel(RoleSubscribeTopicType.Market)
    TimeHelper:CancelTimer(self.ShowTimer)
    TimeHelper:CancelTimer(self.JumpTimer)
    TimeHelper:CancelTimer(self.timer_auto_beh)
    TaskData.shop_auto_close = nil
    if TaskData.is_shop_auto_close then
        TaskData.is_shop_auto_close()
        TaskData.is_shop_auto_close = nil
    end
    self.data:Initialization()
    self.data:SetJumpData()
end

function MarketView:OnClickType(type)
    if type == 2 then
        self:FlushSellList()
    else
        self:OnClickItem(self.sub_data)
        self.ItemContent:SetActive(true)
    end
    self.select_type = type
end

function MarketView:OnClickItem(sub_data)
    if sub_data == nil then
        local cfg = self.data.shop_title
        local data_list = {}
        for i,v in pairs(cfg) do
            if v.id ~= 8 or ShengQiData.Instance:IsOpen() then
                -- if v.id ~= 9 or FlyUpData.Instance:GetStage() >= FlyUpStageType.Low then
                    data_list[i] = v
                -- end
            end
        end

        self.TypeList:SetData(data_list,true)
        local jump_data = self.data:GetJumpData()
        if jump_data and jump_data.parent and jump_data.child then
            local child_data = self:GetChildData(data_list,jump_data)
            if child_data then
                self.TypeList:ClickChild(child_data)
            end
        end
    else
        self:FlushShopList(sub_data)
    end
end

function MarketView:GetChildData(data_list,data)
    for k,v in pairs(data_list[data.parent].children or {}) do
        if v.sub_type == data.child then
            return v
        end
    end
end

function MarketView:FlushShopView()
    if self.select_type == 2 then
        self:FlushSellList(#self.SellList.index_item)
    else
        for k,item in pairs(self.ShopList.index_item) do
            item:FlushNumView()
        end
    end
    self:FlushTotalPrice()
end

function MarketView:FlushTotalPrice()
    self.make_price = self:GetPrice(self.data:GetSelectData())
    UH.SetText(self.TotalPrice,self:GetTotalPrice())
end

function MarketView:GetSelectIndex()
    local data = self.data:GetSelectData()
    return DataHelper.SimpleCondIndex(self.data:GetSellList(),function(info)
        return data.param and data.param.index == info.param.index and data.param.column_type == info.param.column_type
    end)
end

function MarketView:FlushSellList(item_num)
    local list = self.data:GetSellList()
    local index = self:GetSelectIndex()
    if item_num and item_num == #list then
        MallData.JumpItemNum = self.make_num
    end
    if self.data:GetSelectType() ~= 2 and item_num == nil then
        self.SellList:ForceJumpVertical(-0.01)
        index = nil
    end
    if item_num and item_num ~= #list then
        index = self.select_sell_index
        if list[index] == nil then
            index = #list
        end
    end
    self.SellList:SetDataList(list,index)
    self.NotItemTip:SetActive(#list == 0)
    self.ItemContent:SetActive(#list ~= 0)
    if self.data:GetSelectType() == 2 then
        if index and index > 8 then
            local height = (math.ceil(index / 2) - 5) * 120 + 68
            self.SellList:ForceJumpVertical(height)
            TimeHelper:CancelTimer(self.JumpTimer)
            self.JumpTimer = Invoke(function()
                self.SellList:ForceJumpVertical(height+5)
            end,0.05)
        else
            self.SellList:ForceJumpVertical(-0.01)
        end
    end
    self.data:SetSelectType()
end

function MarketView:FlushShopList(sub_data)
    local data = self.data:GetSelectData()
    local list = self.data:GetSubData(sub_data.type,sub_data.sub_type)
    local index = DataHelper.SimpleIndex(list,"item_id",data.item_id)
    local jump_data = self.data:GetJumpData()
    if jump_data and jump_data.index then
        TimeHelper:CancelTimer(self.ShowTimer)
        self.ShowTimer = Invoke(function()
            if jump_data.index >= 5 then
                self.ShopList:JumpVerticalIndex(jump_data.index,120)
            end
            self.data:SetJumpData()
        end,0.01 * jump_data.index)
        self.ShopList:SetDataList(list,jump_data.index)
    else
        if index == nil then
            self.ShopList:ForceJumpVertical(-0.01)
        end
        self.ShopList:SetDataList(list,index)
    end
    self.sub_data = sub_data
end

function MarketView:FlushShopItem()
    local data = self.data:GetSelectData()
    local item = Item.Init(data.item_id)
    local BtnName = Language.Market.BtnName
    self.Call:SetData(item)
    UH.SetText(self.Name,item:QuaName())
    UH.SetText(self.Content,item:Desc())
    UH.SetGold(self.Gold,data.price_type)
    UH.SpriteName(self.Image,Item.GetColor(item.item_id))
    local type_desc = item:TypeDesc()
    if type(type_desc) == "table" then
        type_desc = type_desc[1]
    end
    UH.SetText(self.Type, Language.Mall.SellType .. (type_desc or ""))

    self.make_price,self.make_num = self:GetPrice(data),1
    self.item_num = data.param and data.param.num or nil

    UH.SetText(self.BtnName,BtnName[self.item_num and 1 or 2])

    -- 铸灵特殊处理
    local item_show_type = Item.GetShowType(item.item_id)
    local attri_data
    if item_show_type == ShowType.EquipSpirit then
        attri_data = EquipData.Instance:SpiritLevelAttr(item.param.item_id, item.param.level)
    elseif item_show_type == ShowType.LiveCard then
        attri_data = LiveData.Instance:FormatPropAttriList(item)
    end
    if not TableIsEmpty(attri_data) then
        self.AttrList:SetData(attri_data)
        UH.SetText(self.TxtContent2,item:Desc())
    end
    self.Content:SetObjActive(TableIsEmpty(attri_data))

    self:FlushNumView(MallData.JumpItemNum or self.item_num)
    self:FlushLimitView(data)
    MallData.JumpItemNum = nil
    self.select_sell_index = self:GetSelectIndex()
end

function MarketView:FlushNumView(make_num)
    self.make_num = make_num or 1
    UH.SetText(self.Num,self.make_num)
    UH.SetText(self.TotalPrice,self:GetTotalPrice())
end

function MarketView:FlushLimitView(data)
    if data.require_type ~= 0 then
        local is_limit,limit_tip = MarketCtrl.IsRequire(data.require_type,data.require_value)
        UH.SetText(self.LimitLevel,ColorStr(Format(Language.Reputation.RequireTip[data.require_type],data.require_value),COLORSTR.Red10))
        self.LimitLevel:SetObjActive(not is_limit)
    else
        self.LimitLevel:SetObjActive(false)
    end
end

function MarketView:OnClickAddNum(num)
    if self.make_num == nil then
        return
    end
    if self.make_num + num <= 0 then
        PublicPopupCtrl.Instance:Center(ErrorText[19])
        self:FlushNumView()
        return
    end
    if self.item_num and self.make_num + num > self.item_num then
        PublicPopupCtrl.Instance:Center(Language.Market.ItemLimitNumTip)
        self:FlushNumView(self.item_num)
        return
    end
    if self.make_num + num > MARKET_BUY_MAX_NUM then
        PublicPopupCtrl.Instance:Center(Language.Market.MaxLimitNumTip)
        self:FlushNumView(MARKET_BUY_MAX_NUM)
        return
    end
    if math.abs(num) == 10 then
        self:FlushNumView(math.floor(self.make_num / 10) * 10 + num)
    else
        self:FlushNumView(self.make_num + num)
    end
end

function MarketView:OnClickMinNum()
    self:FlushNumView()
end

function MarketView:OnClickMaxNum()
    self:FlushNumView(self.item_num or self:GetMaxNum())
end

function MarketView:OnClickNumSelect()
    if self.Num.text ~= "" then
        self:OnClickAddNum(tonumber(self.Num.text) - self.make_num)
    end
end

function MarketView:OnClickPurchase()
    local data = self.data:GetSelectData()
    if data ~= nil then
        local is_limit,limit_tip = MarketCtrl.IsRequire(data.require_type,data.require_value)
        if not is_limit then
            PublicPopupCtrl.Instance:Center(limit_tip)
            return
        end
        if self.item_num then
            local item = data.param
            if item:Type() == ItemType.FuShi then
                MarketCtrl.Instance:SendMarketReq(3,item.index)
            else
                MarketCtrl.Instance:SendMarketReq(2,data.param.column_type,data.param.index,self.make_num)
            end
        else
            if not MallCtrl.IsNotCurrency(CurrencyType.ImmortalCoin,self:GetTotalPrice()) then
                MallData.JumpItemNum = self.data:GetNeedGoldNum(self:GetTotalPrice())
                return
            end
            MarketCtrl.Instance:SendMarketReq(1,data.item_id,self.make_num)
        end
        if TaskData.shop_auto_close then
            ViewMgr:CloseView(MallView)
        end
        if TaskData.is_shop_auto_close then
            ViewMgr:CloseView(MallView)
        end
    end
end

function MarketView:GetMaxNum()
    local num = math.floor(RoleData.Instance:GetCurrency(CurrencyType.ImmortalCoin) / self.make_price)
    return (num <= 0 and 1 or (num > MARKET_BUY_MAX_NUM and MARKET_BUY_MAX_NUM or num))
end

function MarketView:GetTotalPrice()
    return (self.make_num or 1) * (self.make_price or self:GetPrice(self.data:GetSelectData()))
end

function MarketView:GetPrice(data)
    return data.param == nil and MarketData.Instance:GetPrice(data) or MarketData.Instance:GetSellPrice(data)
end

----------------------------MarketItem----------------------------
MarketItem = MarketItem or DeclareMono("MarketItem", UIWidgetBaseItem)

function MarketItem:SetData(data)
    local item = data.param or Item.Create(data)
    self.Call:SetData(item)
    UH.SetText(self.Name,item:Name())
    UH.SetGold(self.Gold,data.price_type)
    UIWidgetBaseItem.SetData(self, data)
    self:FlushNumView()
end

function MarketItem:FlushNumView()
    self.data.info_price = self:GetPrice(self.data)
    UH.SetText(self.Num,self.data.info_price)
    self:FlushFloat(MarketData.Instance:GetFloatValue(self.data))
end

function MarketItem:FlushFloat(value)
    for i=1,self.FloatList:Length() do
        local index = self:GetFloatIndex(value)
        if i == index then
            UH.SetText(self.Float[i],value * 100 .. "%")
        end
        self.FloatList[i]:SetActive(i == index)
    end
end

function MarketItem:GetPrice(data)
    return data.param == nil and MarketData.Instance:GetPrice(data) or MarketData.Instance:GetSellPrice(data)
end

function MarketItem:GetFloatIndex(value)
    return value == 0 and 0 or (value > 0 and 1 or 2)
end

function MarketItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self,selected)
    if selected then
        MarketData.Instance:SetSelectData(self.data)
    end
end


MarketViewAttriItem = MarketViewAttriItem or DeclareMono("MarketViewAttriItem", UIWidgetBaseItem)
function MarketViewAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local txt = data.text or data.attr_name .. ColorStr(" +" .. data.attr_percent_value, COLORSTR.Blue6)
    UH.SetText(self.TxtAttr, txt)
end