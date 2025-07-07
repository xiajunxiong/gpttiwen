ClothAromaBuyTips = ClothAromaBuyTips or DeclareView("ClothAromaBuyTips","serveractivity/cloth_aroma_buy_tips")
function ClothAromaBuyTips:LoadCallback()
    self.ctrl = ClothAromaCtrl.Instance
    self.data = ClothAromaData.Instance

    self.cfg = self.data:GetBuyCfg()
    local item_id = self.data:StuffItemId()
    local num = self.cfg.draw_buy
    local item = Item.Create({item_id = item_id, num = num, is_bind = 1})
    self.Call:SetData(item)

    local price = self.cfg.price
    local sprite_name
    if self.cfg.price_type == 3 then
        price = price / 10
        sprite_name = "99999"
    else
        sprite_name = HuoBi[self.cfg.price_type] .. ""
    end

    UH.SetText(self.text,price)
    UH.SpriteName(self.CurrImg, sprite_name)
end

function ClothAromaBuyTips:OnClickConfirm()

    if self.cfg.price_type ~= 3 then
        if MallCtrl.IsNotCurrency(self.cfg.price_type, self.cfg.price) then
            self.ctrl:SendOtherBuy()
        end
        ViewMgr:CloseView(ClothAromaBuyTips)
        return
    end

    local fee = self.data:GetDrawCfg().price/10

    if ServerActivityCtrl.Instance:IsCanItemBuy(ACTIVITY_TYPE.RAND_JIN_YI_TIAN_XIANG, fee) then
        ServerActivityCtrl.Instance:SendBuyCmdReq(ACTIVITY_TYPE.RAND_JIN_YI_TIAN_XIANG, fee)
    else
        self.ctrl:SendBuy()
    end
    ViewMgr:CloseView(ClothAromaBuyTips)
end

function ClothAromaBuyTips:OnClickCancel()
    ViewMgr:CloseView(ClothAromaBuyTips)
end