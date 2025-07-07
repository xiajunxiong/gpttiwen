CommerceView = CommerceView or DeclareMono("CommerceView", UIWFlushPanel)

function CommerceView:CommerceView()
    self.data = CommerceData.Instance
	self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushGridList},
    }
end

function CommerceView:Awake()
    UIWFlushPanel.Awake(self)
    self.List:SetData(self.data.commerce_title_sort,true)
    local jump_data = self.data:GetJumpData()
    if jump_data and jump_data.parent and jump_data.child then
        local child_data = self:GetChildData(jump_data)
        if child_data then
            self.List:ClickChild(child_data)
        end
    end
end

function CommerceView:GetChildData(data)
    local data_list = self.data.commerce_title
    for k,v in pairs(data_list[data.parent].children or {}) do
        if v.sub_type == data.child then
            return v
        end
    end
end

function CommerceView:FlushGridList()
    self.GridList:SetData(BagData.Instance:GetScreenSellData())
end

function CommerceView:OnClickItem(data)
    if data == nil then return end
    local item_data = self.data:GetSubData(data.type,data.sub_type)
    local jump_data = self.data:GetJumpData()
    if jump_data and jump_data.index then
        self.ItemList:SetDataList(item_data,jump_data.index)
        if jump_data.index > 9 then
            self.show_time1 = TimeHelper:AddDelayTimer(function()
                self.ItemList:JumpVerticalIndex(jump_data.index,62)
            end, 0.2)
        end
        self.show_time2 = TimeHelper:AddDelayTimer(function()
            local data = item_data[jump_data.index]
            data.content_func = function(data)
                ShopCtrl.Instance:SendGoldShopBuyReq(data.index,data.make_num)
                ViewMgr:CloseView(BatchBuyView)
            end
            ViewMgr:OpenView(BatchBuyView,data)
            CommerceData.Instance:SetJumpData()
        end, 0.2)   
    else
        self.ItemList:SetData(item_data)
    end
end

function CommerceView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.show_time1)
	TimeHelper:CancelTimer(self.show_time2)
end

----------------------------CommerceItem----------------------------
CommerceItem = CommerceItem or DeclareMono("CommerceItem", UIWidgetBaseItem)

function CommerceItem:SetData(data)
    data.item = Item.New(data)
    UH.SetText(self.Name,data.item:Name())
    UH.SetText(self.Num,data.price)
    UH.SetIcon(self.Icon, Item.GetIconId(data.item_id))
    local index = data.sort % 2 == 0 and 2 or 1
    UH.SpriteName(self.BgSp,"RoundBg" .. index)
    local could = Language.Shop.LimitBuyTip[data.require_type]
    UH.SetText(self.Limit,Format(could,data.require_value))
    if RoleData.Instance:GetRoleLevel() >= data.require_value then
        UH.SetText(self.Limit,Language.Shop.LimitBuyTip[0])
    end
    UH.SetGold(self.Gold,data.price_type)
    UIWidgetBaseItem.SetData(self, data)
    if data.func then
        data.func()
    end
end

function CommerceItem:OnClickItem()
    self.data.content_func = function(data)
        ShopCtrl.Instance:SendGoldShopBuyReq(data.index,data.make_num)
        ViewMgr:CloseView(BatchBuyView)
    end
    if not MallCtrl.IsNotCurrency(self.data.price_type,self.data.price)then
        return
    end
    ViewMgr:OpenView(BatchBuyView,self.data)
end

function CommerceItem:OnClickCell()
    CellClicks.BagGridNoButton({GetData = function()
        return self.data.item
    end})
end