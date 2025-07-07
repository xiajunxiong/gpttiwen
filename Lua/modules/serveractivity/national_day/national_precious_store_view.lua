PreciousStoreView = PreciousStoreView or DeclareView("PreciousStoreView", "serveractivity/national_day/precious_store_view",Mod.RandActivity.Precious)

function PreciousStoreView:PreciousStoreView()
    self.data = ServerActivityData.Instance
    if self.data:PreciousStoreLookFlag() == nil then
        self.data:PreciousStoreLookFlag(true)
    end
end

function PreciousStoreView:LoadCallback()
    self.PanelLst[self.data:GetPreciousStorePhase()]:SetActive(true)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_PRECIOUS_STORE, 
    TimeType.Type_Special_4, Language.NationalPrecious.BuyTime)
end

function PreciousStoreView:OnClickClose()
    ViewMgr:CloseView(PreciousStoreView)
end

-- ---------------------PreciousStoreItem---------------------
PreciousStoreItem = PreciousStoreItem or DeclareMono("PreciousStoreItem", UIWidgetBaseItem)

function PreciousStoreItem:FlushItemView(data)
    local times = ServerActivityData.Instance:GetPreciousStoreHasTimes(data)
    local list = ServerActivityData.Instance:GetPreciousStoreRewardList(data.reward_group)
    if data.gold_num > 0 then  --需要展示元宝
        if not data.is_create then
            data.is_create = true
            local item = {
                is_bind = 0,
                num = data.gold_num,
                item_id = CommonItemId.Gold
            }
            table.insert(list,item)
        end
    end
    self.Grid:SetData(list)
    if self.Name then
        UH.SetText(self.Name,data.gift_name or [[]])
    end
    UH.SetGold(self.IconSp, data.price_type == 3 and 999 or data.price_type)
    UH.SetText(self.BuyVal, data.price_type == 3 and data.price / 10 or data.price)
    UH.SetText(self.SellQing, times <= 0 and Language.NationalMiBao.Sell or [[]])
    UH.SetText(self.BuyLimit, Format(Language.NationalMiBao.Limit[data.limit_type], 
    times, data.limit_type == 1 and data.buy_times or data.today_times))
    self.BtnInter.Interactable = times > 0
    self.Buy:SetActive(times > 0)
end

-- ---------------------热售商品---------------------
PreciousHotSaleView = PreciousHotSaleView or DeclareMono("PreciousHotSaleView", UIWFlushPanel)
function PreciousHotSaleView:PreciousHotSaleView()
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.data.national_precious_data, func = self.OnFlush, init_call = true},
        {data = self.data:GetCmdBuyItemCheckRet(),func = self.SendBuy, init_call = false}
    }
end

function PreciousHotSaleView:OnFlush()
    local data_list = self.data:GetPreciousStoreList()
    self.RecommendItem:SetData(data_list[1] or {})
    self.Time:StampTime(self.data:GetPreciousStoreStampTime(),
    TimeType.Type_Special_4,Language.NationalPrecious.BuyTime)
    self.HotSaleLst:SetData(DataHelper.FormatList(data_list,2))
    UH.SetText(self.TabTex,Language.NationalPrecious.TabText)
end

function PreciousHotSaleView:OnBuyClick(data)
    local fee = data.price / 10
    ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_NATIONAL_ZHEN_PIN_SHANG_PU,fee,ACTIVITY_TYPE.RAND_PRECIOUS_STORE, 1, data.seq)
    self.click_config = data
end

function PreciousHotSaleView:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_NATIONAL_ZHEN_PIN_SHANG_PU, self.click_config.price)
end

-- ---------------------PreciousHotSaleItem---------------------
PreciousHotSaleItem = PreciousHotSaleItem or DeclareMono("PreciousHotSaleItem", UIWidgetBaseItem)

function PreciousHotSaleItem:SetData(data)
    UH.SetText(self.PropVal, Format(Language.NationalMiBao.Value, data.value))
    UIWidgetBaseItem.SetData(self, data)
    PreciousStoreItem.FlushItemView(self,data)
    self:FlushLabelView(data)
end

function PreciousHotSaleItem:FlushLabelView(data)
    if self.TabBg ~= nil then
        local type = data.recommend_type
        for i = 1, self.TabBg:Length() do
            self.TabBg[i]:SetActive(i == type or type == 3)
        end
        self.OutLine.effectColor = 1 == type and COLORS.Red7 or COLORS.Red14
        UH.SetText(self.MarkTex,Language.NationalPrecious.MarkTex[type])
    else
        UH.SetText(self.MarkTex,Format(Language.NationalPrecious.RecommendMark,data.mark))
    end
end

-- ---------------------限时特惠---------------------
PreciousTimeLimitView = PreciousTimeLimitView or DeclareMono("PreciousTimeLimitView", UIWFlushPanel)
function PreciousTimeLimitView:PreciousTimeLimitView()
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.data.national_precious_data, func = self.OnFlush, init_call = true},
        {data = self.data:GetCmdBuyItemCheckRet(),func = self.SendBuy, init_call = false}
    }
end

function PreciousTimeLimitView:OnFlush()
    self.LimitTimeLst:SetData(self.data:GetPreciousStoreList())
end

function PreciousTimeLimitView:OnBuyClick(data)
    local fee = data.price / 10
    ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_NATIONAL_ZHEN_PIN_SHANG_PU,fee,ACTIVITY_TYPE.RAND_PRECIOUS_STORE, 1, data.seq)
    self.click_config = data
end

function PreciousTimeLimitView:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_NATIONAL_ZHEN_PIN_SHANG_PU, self.click_config.price)
end

-- ---------------------PreciousTimeLimitItem---------------------
PreciousTimeLimitItem = PreciousTimeLimitItem or DeclareMono("PreciousTimeLimitItem", UIWidgetBaseItem)

function PreciousTimeLimitItem:SetData(data)
    UH.SetText(self.MarkTex, data.mark)
    --UH.SetText(self.GiftName, Format(Language.NationalPrecious.GiftName,
    --data.price_type == 3 and data.price / 10 or data.price))
    UH.SetText(self.GiftName,data.gift_name)
    PreciousStoreItem.FlushItemView(self, data)
    UIWidgetBaseItem.SetData(self, data)
    self:FlushEffect(data)
end

function PreciousTimeLimitItem:FlushEffect(data)
    if self.effect_gift then
        self.EffectTool:Stop(self.effect_gift)
    end
    local effect_id = ServeractivityConfig.PreciousLimitGiftEffect[data.gift_icon]
    if effect_id then
        self.effect_gift = self.EffectTool:Play(effect_id)
    end
end

-- ---------------------超值---------------------
PreciousSuperValueView = PreciousSuperValueView or DeclareMono("PreciousSuperValueView", UIWFlushPanel)
function PreciousSuperValueView:PreciousSuperValueView()
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.data.national_precious_data, func = self.OnFlush, init_call = true},
        {data = self.data:GetCmdBuyItemCheckRet(),func = self.SendBuy, init_call = false}
    }
end

function PreciousSuperValueView:OnFlush()
    self.SuperValLst:SetData(self.data:GetPreciousStoreList())
end

function PreciousSuperValueView:OnBuyClick(item)
    if item ~= nil then
        local data = item:GetData()
        if not item:IsSelectAll() then
            PublicPopupCtrl.Instance:Center(Language.NationalPrecious.SelectGoodsTip)
            return 
        end
        local fee = data.price / 10
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_NATIONAL_ZHEN_PIN_SHANG_PU,fee,ACTIVITY_TYPE.RAND_PRECIOUS_STORE, 1, data.seq, bit:b2d(item:GetSelectData()))
        self.click_config = data
    end
end

function PreciousSuperValueView:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_NATIONAL_ZHEN_PIN_SHANG_PU, self.click_config.price)
end

function PreciousSuperValueView:OnSelect(data)
    self.Parent:SetActive(false)
    if data.item ~= nil then
        data.item:SetDataList(data.item_data)
    end
end

-- ---------------------PreciousSuperValueItem---------------------
PreciousSuperValueItem = PreciousSuperValueItem or DeclareMono("PreciousSuperValueItem", UIWidgetBaseItem)
function PreciousSuperValueItem:SetData(data)
    UH.SetText(self.MarkTex, Format(Language.NationalPrecious.SuoerValTab, data.mark))
    self.reward_list = ServerActivityData.Instance:GetPreciousStoreRewardList(data.reward_group)
    PreciousStoreItem.FlushItemView(self, data)
    UIWidgetBaseItem.SetData(self, data)
    self.select_list = self:GetSelectList()
    self.List:SetData(self.select_list)
end

function PreciousSuperValueItem:OnClickBlock()
    self.Parent:SetActive(false)
end

function PreciousSuperValueItem:SetDataList(data)
    self.select_list[self.select_index].item = data
    self.List:SetData(self.select_list)
end

function PreciousSuperValueItem:OnAddItem(data)
    local reward_list = self:GetRewrdList()
    if #reward_list >= 1 then
        self.select_index = data.index or 1
        self.ParentList:SetData(reward_list)
        UH.LocalPos(self.Rect,self:GetPos())
        UH.SetEdgePosition(self.Rect,self.Root)
        self.Parent:SetActive(true)
    end
end

function PreciousSuperValueItem:GetSelectList()
    local list = {}
    for i=1,self.data.reward_num do
        table.insert(list,{index = i})
    end
    return list
end

function PreciousSuperValueItem:GetRewrdList()
    local list = {}
    for i,v in ipairs(self.reward_list) do
        if not self:FindData(v) then
            table.insert(list,{item = self,item_data = v})
        end
    end
    return list
end

function PreciousSuperValueItem:GetSelectData()
    local list = {}
    for i,v in ipairs(self.reward_list) do
        list[33 - i] = self:FindData(v) and 1 or 0
    end
    return list
end

function PreciousSuperValueItem:IsSelectAll()
    local num = 0
    for k,v in pairs(self.select_list) do
        if v.item ~= nil then
            num = num + 1
        end
    end
    return num >= #self.select_list
end

function PreciousSuperValueItem:FindData(data)
    for k,v in pairs(self.select_list) do
        if v.item and v.item == data then
            return true
        end
    end
end

function PreciousSuperValueItem:GetPos()
    return UH.GetRootPosition(self.gameObject) + Vector3.New(0,100,0)
end

-- ---------------------PreciousSuperValueSelectItem---------------------
PreciousSuperValueSelectItem = PreciousSuperValueSelectItem or DeclareMono("PreciousSuperValueSelectItem", UIWidgetBaseItem)
function PreciousSuperValueSelectItem:SetData(data)
    if data.item then
        self.Call:SetData(Item.New(data.item))
    end
    self.Call:SetObjActive(data.item ~= nil)
    UIWidgetBaseItem.SetData(self, data)
end