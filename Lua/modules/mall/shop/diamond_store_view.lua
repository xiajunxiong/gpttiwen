DiamondStoreView = DiamondStoreView or DeclareMono("DiamondStoreView", UIWFlushPanel)

function DiamondStoreView:DiamondStoreView()
    self.data = StoreData.Instance
end

function DiamondStoreView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    local list = self.data:GetDiamondItemData() or {}
    self.List:SetData(list)
    self:JumpItem(list, self.data:GetJumpItemId())
end

function DiamondStoreView:JumpItem(list, jump_item_id)
    if list == nil or jump_item_id then
        return
    end
    local item_data = nil
    for k,v in pairs(list) do
        for i,j in pairs(v) do
            if j.item_id == jump_item_id then
                item_data = j
            end
        end
    end
    if item_data == nil then
        return
    end
    self.show_time = Invoke(function()
        item_data.content_func = function(data)
            ShoppingCtrl.Instance:SendStoreBuyReq(data)
            ViewMgr:CloseView(BatchBuyView)
        end
        ViewMgr:OpenView(BatchBuyView,item_data)
    end,0.5)
    self.data:SetJumpItemId()
end

function DiamondStoreView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.show_time)
end
-------------------------------------
DiamondRowItem = DiamondRowItem or DeclareMono("DiamondRowItem", UIWidgetBaseItem)
function DiamondRowItem:SetData(data)
    self.List:SetData(data)
end
-------------------------------------
DiamondStoreItem = DiamondStoreItem or DeclareMono("DiamondStoreItem", UIWidgetBaseItem)
function DiamondStoreItem:SetData(data)
    local item = Item.Init(data.item_id, data.num, data.is_bind)
    self.Cell:SetData(item)
    UH.SetText(self.Price, DataHelper.ConverMoney(data.price))
    UH.SetText(self.Name, item:Name())
    UIWidgetBaseItem.SetData(self, data)
end

--购买或者兑换
function DiamondStoreItem:OnClickItem()
    self.data.content_func = function(data)
        ShoppingCtrl.Instance:SendStoreBuyReq(data)
        ViewMgr:CloseView(BatchBuyView)
    end
    ViewMgr:OpenView(BatchBuyView, self.data)
end