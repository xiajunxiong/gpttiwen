-- 精炼
PetWeaponRefineView = PetWeaponRefineView or DeclareView("PetWeaponRefineView", "pet/pet_weapon/pet_weapon_refine_view")
function PetWeaponRefineView:PetWeaponRefineView()
    self.BG:SetData(self:GetType(),Language.PetWeapon.Title3, Vector2.New(856, 536))
end

PetWeaponRefinePanel = PetWeaponRefinePanel or DeclareMono("PetWeaponRefinePanel", UIWFlushPanel)

function PetWeaponRefinePanel:PetWeaponRefinePanel()
    self.data = PetWeaponData.Instance
    self.data_cares = {
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.PetWeapon), func = self.FlushPanel, init_call = false},
        {data = PetData.Instance.pet_list, func = self.FlushPanel, init_call = false},
    }
end

function PetWeaponRefinePanel:Awake()
    UIWFlushPanel.Awake(self)

    self.StuffItem:SetRemoveEvent(function ()
        self.stuff_item = nil
        self:FlushStuff()
    end)
    self.RemindToggle.isOn = self.data:GetRefineToggleFlag()
    self:FlushPanel()
end

function PetWeaponRefinePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SetCurCtrlWeapon(nil)
    self.data.sel_refine_stuff = nil
end

function PetWeaponRefinePanel:FlushWeapon()
    if self.weapon then
        self.old_weapon = self.weapon
    end

    if self.weapon == nil then
        self.weapon = self.data:GetCurCtrlWeapon()
    elseif self.weapon:PetIndex() then
        if self.weapon:GridPos() == 0 then
            self.weapon = PetData.Instance:GetPetInfo(self.weapon:PetIndex()):Weapon()
        else
            self.weapon = PetData.Instance:GetPetInfo(self.weapon:PetIndex()):Weapon2()
        end
    elseif self.weapon.index >= 0 then
        self.weapon = BagData.Instance:GetItemByIndex(ItemColumnType.PetWeapon, self.weapon.index)
    end

    if self.oper_refine and self.old_weapon and self.old_weapon ~= self.weapon then
        self.oper_refine = false
        local param_t = {
            data = {
                old_weapon = self.old_weapon,
                cur_weapon = self.weapon,
            }
        }
        ViewMgr:OpenView(PetWeaponUpShowView, param_t)
    end
end

function PetWeaponRefinePanel:FlushPanel()
    self:FlushWeapon()
    self.CurCell:SetData(self.weapon)
    UH.SetText(self.CurName, self.weapon:QuaName(true))
    self:FlushStuff()
    self:FlushDesc()
end

function PetWeaponRefinePanel:FlushStuff()
    local weapon = self.weapon
    local is_max = weapon:RefineLevel() == weapon:MaxRefineLevel()

    self.Interactor.Interactable = not is_max

    self.StuffItem:SetData(self.stuff_item)
    if self.stuff_item then
        UH.SetText(self.StuffName, string.format("%s: %s",self.stuff_item:Name(),DataHelper.ConsumeNum(1, 1, true)))
    else
        UH.SetText(self.StuffName, Language.PetWeapon.Tip5)
    end
    local remind_num = self.data:GetRefineRemind(weapon)
    self.RedPoint:SetNum(remind_num)
end

function PetWeaponRefinePanel:FlushDesc()
    local weapon = self.weapon
    local cur_refine_level = weapon:RefineLevel()
    UH.SetText(self.CurStepLevel, string.format(Language.PetWeapon.Step, DataHelper.GetDaXie(cur_refine_level)))
    local cur_desc = string.format("%s\n%s",weapon:RefineEffectDesc(),weapon:RefineAttriDesc())
    UH.SetText(self.CurStepDesc, cur_desc)

    local next_refine_cfg = weapon:NextRefineCfg()
    self.NextEffectObj:SetActive(not TableIsEmpty(next_refine_cfg))
    if TableIsEmpty(next_refine_cfg) then
        return
    end
    UH.SetText(self.NextStepLevel, string.format(Language.PetWeapon.Step, DataHelper.GetDaXie(cur_refine_level + 1)))
    local effect_desc = Cfg.SpecialEffect(next_refine_cfg.skill_buff).description
    local next_attri_list = PetWeapon.GetRefineAttriList(next_refine_cfg)
    local next_attri_desc = PetWeapon.GetRefineAttriDesc(next_attri_list)
    local next_desc = string.format("%s\n%s",effect_desc,next_attri_desc)
    UH.SetText(self.NextStepDesc, next_desc)
end

function PetWeaponRefinePanel:OnAddRefineStuffToggle()
    if self.weapon:RefineLevel() == self.weapon:MaxRefineLevel() then
        PublicPopupCtrl.Instance:Center(Language.PetWeapon.Error7)
        return
    end

    local list = self.data:GetAllRefineStuffList(self.weapon)
    if TableIsEmpty(list) then
        PetWeaponCtrl.Instance:NoWeaponTip(Language.PetWeapon.NoWeaponTip2)
        return
    end
    self.StuffSelObj:SetActive(true)
    self.StuffList:SetData(list)
end

function PetWeaponRefinePanel:OnCloseStuffSelClick()
    self.StuffSelObj:SetActive(false)
end

function PetWeaponRefinePanel:OnStuffSelClick(data)
    self.StuffSelObj:SetActive(false)
    self.stuff_item = data
    self.data.sel_refine_stuff = self.stuff_item
    self:FlushStuff()
end

function PetWeaponRefinePanel:OnRefineClick()
    local weapon = self.weapon
    if weapon:RefineLevel() == weapon:MaxRefineLevel() then
        PublicPopupCtrl.Instance:Error(Language.PetWeapon.Error7)
        return
    end
    if not self.stuff_item then
        return
    end
    if weapon then
        local is_dialog = self.stuff_item:RefineLevel() >= weapon:RefineLevel() and self.stuff_item:RefineLevel() > 0
        local is_time_dialog = self.stuff_item:Level() > 1
        local func = function ()
            self.data:ClearSelectedBagCell()
            local is_bag = weapon:PetIndex() ~= nil and 0 or 1
            local index = is_bag == 1 and weapon.index or weapon:PetIndex()
            PetCtrl.SendPetOp(PET_OP_TYPE.RefineWeapon, is_bag, index, self.stuff_item.index, weapon:GridPos())
            self.stuff_item = nil
            self.data.sel_refine_stuff = self.stuff_item
            self.StuffItem:SetData(nil)
            self.EffectTool:Play(3161024)
            self.EffectTool:Play(3161025)

            self.oper_refine = true
        end
        if is_dialog then
            PublicPopupCtrl.Instance:AlertTip(Language.PetWeapon.RefineTip,func)
        elseif is_time_dialog then
            local info = {
                content = Language.PetWeapon.Tip12,
                confirm = {
                        func = function ()
                            func()
                            ViewMgr:CloseView(DialogTipsView)
                        end,
                        wait_time = 5
                   },
               }
           PublicPopupCtrl.Instance:DialogTips(info)
        else
            func()
        end
    end
end

function PetWeaponRefinePanel:OnRemindToggleClick(value)
    self.data:SetRefineToggleFlag(value)
    self:FlushStuff()
    self.data.remind_data:Notify()
end

PetWeaponRefineStuffItem = PetWeaponRefineStuffItem or DeclareMono("PetWeaponRefineStuffItem", UIWidgetBaseItem)
function PetWeaponRefineStuffItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ItemCell:SetData(data)
end