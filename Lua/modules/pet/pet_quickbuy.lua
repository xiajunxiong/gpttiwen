PetQuickBuy = PetQuickBuy or DeclareView("PetQuickBuy", "pet/pet_quickbuy")
function PetQuickBuy:PetQuickBuy()
end
function PetQuickBuy:LoadCallback()
    self.Alert:SetData(self:GetType(), Vector2.New(480, 380))
    self.buy_num = 1
end

function PetQuickBuy:OnFlush()
    local item_id = PetData.Instance.pet_other_cfg.exp_pill
    self.info = ShopData.GetFastBuyData(item_id)
    self.CostCell:SetDataByItemId(self.info.item_id)
    UH.SetText(self.CostNum,string.format(Language.Workshop.CostUpgrade, self.info.price * self.buy_num, Language.Common.HuoBi[self.info.money_type]) )
    UH.SetText(self.NeedNum, self.buy_num)
    UH.SetText(self.CostName, self.CostCell:GetData():Name())
end
function PetQuickBuy:OnClickAdd(count)
    self.buy_num = self.buy_num + count
    if self.buy_num < 1 then
        self.buy_num = 1
    elseif self.buy_num > 99 then
        self.buy_num = 99
    end
    self:Flush()
end
function PetQuickBuy:OnClickCancel()
    ViewMgr:CloseView(PetQuickBuy)
end
function PetQuickBuy:OnClickOk()
    RoleCtrl.FastPurchase(self.info.item_id, self.buy_num)
    ViewMgr:CloseView(PetQuickBuy)
end
