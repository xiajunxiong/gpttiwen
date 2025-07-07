GoldStoreView = GoldStoreView or DeclareMono("GoldStoreView", UIWFlushPanel)

function GoldStoreView:GoldStoreView()
    self.data = StoreData.Instance
end

function GoldStoreView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    local list = self.data:GetGoldItemData(GOLD_STORE_TYPE)
    self.List:SetData(list)
    self:JumpItem(DataHelper.GetSearch(list,[[item_id]],self.data:GetJumpItemId()))
end

function GoldStoreView:JumpItem(item_data)
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

function GoldStoreView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.show_time)
end

GoldStoreItem = GoldStoreItem or DeclareMono("GoldStoreItem", UIWidgetBaseItem)
function GoldStoreItem:SetData(data)
    data.item = Item.New(data)
    self.Cell:SetData(data.item)
    UH.SetText(self.Price, data.price)
    UH.SetText(self.Name, data.item:Name())
    if data.type_text then
        self.TypeObj:SetActive(true)
        UH.SetText(self.TypeText, data.type_text)
    else
        self.TypeObj:SetActive(false)
    end
    UIWidgetBaseItem.SetData(self, data)
end

--购买或者兑换
function GoldStoreItem:OnClickItem()
    self.data.content_func = function(data)
        ShoppingCtrl.Instance:SendStoreBuyReq(data)
        ViewMgr:CloseView(BatchBuyView)
    end
    ViewMgr:OpenView(BatchBuyView, self.data)
end