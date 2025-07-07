
PetStuffBuyDialog = PetStuffBuyDialog or DeclareView("PetStuffBuyDialog", "pet/pet_stuff_buy_dialog")

function PetStuffBuyDialog:LoadCallback(param_t)
    self.context = param_t.context or ""
    self.item = param_t.item
    self.toggle_key = param_t.toggle_key
    self.l_btn_text = param_t.l_btn_text or Language.PetStuffBuy.BtnL
    self.r_btn_text = param_t.r_btn_text or Language.PetStuffBuy.BtnR
    self.l_func = param_t.l_func
    self.r_func = param_t.r_func
    self.toggle_func = param_t.toggle_func
    self.show_toggle = param_t.show_toggle == nil and true or param_t.show_toggle 
    self.item_flush_func = param_t.item_flush_func

    self.care_ht = BagData.Instance:Care(BindTool.Bind(self.FlushItem, self))

    self:FlushPanel()
end

function PetStuffBuyDialog:CloseCallback()
    self:OnToggleEvent()
    BagData.Instance:UnCare(self.care_ht)
end

function PetStuffBuyDialog:FlushPanel()
    UH.SetText(self.Context, self.context)
    UH.SetText(self.LBtnText, self.l_btn_text)
    UH.SetText(self.RBtnText, self.r_btn_text)
    if self.item then
        self.ItemCell:SetData(self.item)
    end
    self.Toggle:SetObjActive(self.show_toggle)
    self.Toggle.isOn = false
end

function PetStuffBuyDialog:FlushItem()
    if self.item_flush_func and self.item then
        self.item_flush_func(self.item)
        self.ItemCell:SetData(self.item)
    end
end

function PetStuffBuyDialog:OnClickClose()
    ViewMgr:CloseView(PetStuffBuyDialog)
end

function PetStuffBuyDialog:OnLeftClick()
    if self.l_func then
        self.l_func()
    end
    self:OnClickClose()
end

function PetStuffBuyDialog:OnRightClick()
    if self.r_func then
        self.r_func()
    end
    self:OnClickClose()
end

function PetStuffBuyDialog:OnToggleEvent()
    if self.toggle_func then
        self.toggle_func(self.Toggle.isOn)
    end
end