QuickBuyView = QuickBuyView or DeclareView("QuickBuyView", "main_other/quick_buy")

function QuickBuyView:QuickBuyView()
    self.Alert:SetData(self:GetType(), Vector2.New(466, 287))
end

--item_data 查找到的物品配置
--need_num 需要的数量
function QuickBuyView:LoadCallback(param_t)
    self.data = param_t.item_data
    local item = param_t.item or Item.New(self.data)
    self.make_num = param_t.need_num or 1
    UH.SetGold(self.Gold,self.data.price_type)
    UH.SetText(self.Content,Format(Language.MainOther.QuickBuyContent,
    self.make_num,item:QuaName()))
    UH.SetText(self.Price,self.data.price * self.make_num)
end

function QuickBuyView:OnClickCancel()
    ViewMgr:CloseView(QuickBuyView)
end

--仅限于商会的购买弹窗
function QuickBuyView:OnClickConfirm()
    ShopCtrl.Instance:SendGoldShopBuyReq(self.data.index,self.make_num)
    ViewMgr:CloseView(QuickBuyView)
end