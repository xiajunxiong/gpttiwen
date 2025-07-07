-- 炼妖确认弹框
PetRefiningDialog = PetRefiningDialog or DeclareView("PetRefiningDialog", "pet_refining/pet_refining_dialog")
function PetRefiningDialog:PetRefiningDialog()
    
end
-- param_t === {context,pet}
function PetRefiningDialog:LoadCallback(param_t)
    UH.SetText(self.Context, param_t.context or "")
    if param_t.pet then
        self.PetItem:SetData(param_t.pet)
    end
    self.cancel_func = param_t.cancel_func
    self.ok_func = param_t.ok_func
end

function PetRefiningDialog:OnCancelClick()
    ViewMgr:CloseView(PetRefiningDialog)
    if self.cancel_func then
        self.cancel_func()
    end
end

function PetRefiningDialog:OnOkClick()
    if self.ok_func then
        self.ok_func()
    else
        ViewMgr:CloseView(PetRefiningDialog)
    end
end


PetRefiningDialogPetItem = PetRefiningDialogPetItem or DeclareMono("PetRefiningDialogPetItem", UIWidgetBaseItem)
-- data === pet
function PetRefiningDialogPetItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.PetCell:SetData(data)
    UH.SetText(self.Name, data:Name())
    local score_num, score_level = data:GetScore()
    UH.SetText(self.ScoreNum, score_num)
    UH.SpriteName(self.ScoreLevel, "scoreLevel"..tostring(score_level))
end