-- =================== 宠物徽章合成 ======================
PetBagdeCpd = PetBagdeCpd or DeclareView("PetBagdeCpd", "pet/pet_bagde/pet_bagde_compound")
function PetBagdeCpd:PetBagdeCpd()
    self.Board:SetData(self:GetType(),Language.PetBagde.SpdTitle, Vector2.New(440, 536))
    self.data = PetBagdeData.Instance
end

function PetBagdeCpd:LoadCallback(param_t)
    param_t = param_t or {}
    local item_id = param_t.item_id
    self.Panel:SetItem(item_id)
end

PetBagdeCpdPanel = PetBagdeCpdPanel or DeclareMono("PetBagdeCpdPanel", UIWFlushPanel)
function PetBagdeCpdPanel:PetBagdeCpdPanel()
    self.data = PetBagdeData.Instance
    self.data_cares = {
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushPanel, init_call = false},
    }
end

function PetBagdeCpdPanel:SetItem(item_id)
    item_id = item_id or 0
    local cfg = self.data:GetBagdeQuaCfgByClipItem(item_id)
    if not cfg then
        return
    end
    self.item_id = item_id
    self.cfg = cfg
    self:FlushPanel()
end

function PetBagdeCpdPanel:FlushPanel()
    local item = Item.Create({item_id = self.cfg.item_id})
    self.ItemCell:SetData(item)
    UH.SetText(self.ItemName, item:QuaName(true))
    local qua = item:QuaLevel()

    UH.SetIcon(self.ItemIcon, Item.GetIconId(self.item_id))
    local desc = qua > 6 and Language.PetBagde.Txt4 or Language.PetBagde.Txt1
    local need_num = ColorStr(self.cfg.synthesis_num, COLORSTR.Yellow1)
    local rate_value = qua > 6 and self.cfg.rate or self.cfg.next_level_rate
    local rate = string.format("%s%%",rate_value / 100)
    rate = ColorStr(rate, COLORSTR.Yellow1)
    desc = string.format(desc, need_num, Item.GetQuaName(self.item_id, true), rate)
    UH.SetText(self.Desc, desc)

    local has_num = Item.GetNum(self.item_id)
    local color = has_num >= self.cfg.synthesis_num and COLORSTR.Green3 or COLORSTR.Red8
    UH.SetText(self.ItemNum, ColorStr(string.format("x%s", has_num), color))

    color = QualityColorStrShallow[Item.GetColor(self.item_id)]
    local item_tip = Language.PetBagde.Txt2
    local has_num_str = ColorStr(string.format("【%s】", Item.GetName(self.item_id)), color)
    UH.SetText(self.ItemTip, string.format(item_tip, has_num_str))
end

function PetBagdeCpdPanel:OnCancelClick()
    ViewMgr:CloseView(PetBagdeCpd)
end

function PetBagdeCpdPanel:OnOKClick()
    if self.cfg then
        local has_num = Item.GetNum(self.item_id)
        if has_num >= self.cfg.synthesis_num then
            --LogError("徽章碎片合成：",PET_OP_TYPE.PetBagdeCompose, self.cfg.item_id)
            PetCtrl.SendPetOp(PET_OP_TYPE.PetBagdeCompose, self.cfg.item_id)
        else
            local item_name = Item.GetName(self.item_id)
            PublicPopupCtrl.Instance:Center(string.format(Language.PetBagde.Tip1, item_name))
        end
    end
end