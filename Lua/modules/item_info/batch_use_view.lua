BatchUseView = BatchUseView or DeclareView("BatchUseView", "item_info/batch_use")

VIEW_DECLARE_MASK(BatchUseView, ViewMask.BgBlockClose)
function BatchUseView:BatchUseView()
    self.Board:SetData(self:GetType(), Language.ItemInfo.BatchTitle, Vector2.New(378, 450))
end

function BatchUseView:LoadCallback(param_t)
    self.param_t = param_t
    self.use_func = self.param_t.use_func    
    self.item = self.param_t.item
    self.use_num = self.item:Num()
    UH.SetText(self.Name, self.item:QuaName())
    self.Cell:SetData(self.item)
    self:FlushInfo()
end
function BatchUseView:OnClickJian()
    self.use_num = self.use_num - 1
    if self.use_num < 0 then
        self.use_num = 0
    end
    self:FlushInfo()
end
function BatchUseView:FlushInfo()
    UH.SetText(self.Num, self.use_num)
    UH.SetText(self.Desc, string.format(self.param_t.desc , self.use_num))
end
function BatchUseView:OnClickAdd()
    self.use_num = self.use_num + 1
    if self.use_num > self.item:Num() then
        self.use_num = self.item:Num()
        PublicPopupCtrl.Instance:Center(Language.ItemInfo.MaxNum)
    end
    self:FlushInfo()
end
function BatchUseView:OnClickUse()
    if self.use_num <= 0 then
        PublicPopupCtrl.Instance:Center(Language.ItemInfo.LessOne)
        return
    end
    if self.use_func then
        self.use_func(self.use_num)
    end
end