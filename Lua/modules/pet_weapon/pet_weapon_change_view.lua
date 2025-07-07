PetWeaponChangeView = PetWeaponChangeView or DeclareView("PetWeaponChangeView", "pet/pet_weapon/pet_weapon_change_view", Mod.PetWeapon.Change)
function PetWeaponChangeView:PetWeaponChangeView()
    self.BG:SetData(self:GetType(),Language.PetWeapon.Title4, Vector2.New(929, 500))
end

-- param_t:{pet_weawpon:PetWeapon}
function PetWeaponChangeView:LoadCallback(param_t)
    param_t = param_t or {}
    if param_t.pet_weawpon then
        PetWeaponData.Instance.sel_change_weapon.pet_weapon = param_t.pet_weawpon
    end
end

function PetWeaponChangeView:CloseCallback()
    PetWeaponData.Instance.sel_change_weapon.pet_weapon = {}
end

PetWeaponChangePanel = PetWeaponChangePanel or DeclareMono("PetWeaponChangePanel", UIWFlushPanel)
function PetWeaponChangePanel:PetWeaponChangePanel()
    self.data = PetWeaponData.Instance
    self.cur_weapon = {}
    self.new_weapon = {}
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = true},
        {data = self.data.sel_change_weapon, func = self.FlushData, init_call = false},
    }
end

function PetWeaponChangePanel:FlushData()
    self.cur_weapon.pet_weapon = self.data.sel_change_weapon.pet_weapon or {}
    self.new_weapon = {}
    self:FlushPanel()
end

function PetWeaponChangePanel:FlushPanel()
    self.LeftInfo:SetData(self.cur_weapon)
    self.RightInfo:SetData(self.new_weapon)

    if TableIsEmpty(self.cur_weapon.pet_weapon) then
        self.StuffInfoObj:SetActive(false)
        return
    else
        self.StuffInfoObj:SetActive(true)
    end
    local cfg = self.cur_weapon.pet_weapon:BaseCfg()
    local stuff = Item.Create({item_id = cfg.change_item})
    self.StuffCell:SetData(stuff)
    UH.SetText(self.StuffName, stuff:Name())
    local has_num = Item.GetNum(stuff.item_id)
    local need_num = cfg.change_item_num
    local num_str
    if has_num >= need_num then
        num_str = ColorStr(tostring(has_num), COLORSTR.Green4)
    else
        num_str = ColorStr(tostring(has_num), COLORSTR.Red8)
    end
    UH.SetText(self.StuffNum, string.format("%s/%s",num_str,need_num))

    self.stuff = stuff
    self.need_stuff = need_num
end

function PetWeaponChangePanel:OnSelChangeItemClick(value, mono)
    if not value then
        self.new_weapon = {}
        self:FlushPanel()
        return
    end
    local data = mono:GetData()
    if mono.Toggle then
        self.sel_toggle = mono.Toggle
    end
    local info = {
        item_id = data.pet_weapon_item,
        param = {
            step = self.cur_weapon.pet_weapon.param.step,
            refine = self.cur_weapon.pet_weapon.param.refine,
            level = self.cur_weapon.pet_weapon.param.level,
        },
    }
    local weapon = Item.Create(info)
    local attri_info = self.data:GetWeaponMaxAttri(weapon, info.param.level, info.param.step)
    weapon.param.attri_list = attri_info
    self.new_weapon.pet_weapon = weapon

    self:FlushPanel()
end

function PetWeaponChangePanel:OnOpenSelPanelClick()
    if TableIsEmpty(self.cur_weapon.pet_weapon) then
        PublicPopupCtrl.Instance:Center(Language.PetWeapon.Tip8)
        return
    end
    self.SelectPanelObj:SetActive(true)
    self.change_list_data = self.data:GetChangeList(self.cur_weapon.pet_weapon)
    self.SelList:SetData(self.change_list_data)
end

function PetWeaponChangePanel:OnOpenSelViewClick()
    local list = PetWeaponData.Instance:GetChangeWeaponList()
    if TableIsEmpty(list) then
        PetWeaponCtrl.Instance:NoWeaponTip(Language.PetWeapon.NoWeaponTip1)
        return
    end
    ViewMgr:OpenView(PetWeaponChangeSelView)
end

function PetWeaponChangePanel:OnCloseSelPanelClick()
    self.SelectPanelObj:SetActive(false)
end

function PetWeaponChangePanel:OnChangeClick()
    if self.cur_weapon.pet_weapon == nil then
        PublicPopupCtrl.Instance:Center(Language.PetWeapon.Tip8)
        return
    end
    if self.new_weapon.pet_weapon == nil then
        PublicPopupCtrl.Instance:Center(Language.PetWeapon.Tip8)
        self:OnOpenSelPanelClick()
        return
    end
    if self.stuff then
        local num = Item.GetNum(self.stuff.item_id)
        if num < self.need_stuff then
            PublicPopupCtrl.Instance:Error(Language.PetWeapon.Error10)
            return
        end
        local func = function ()
            local weapon = self.cur_weapon.pet_weapon
            local pet_index = weapon:PetIndex()
            local is_bag = pet_index == nil and 1 or 0
            local index = pet_index or weapon.index
            local change_weapon_item_id = self.new_weapon.pet_weapon.item_id
            PetCtrl.SendPetOp(PET_OP_TYPE.PetWeaponChange, is_bag, index, change_weapon_item_id, weapon.weapon_idx)
            -- ViewMgr:CloseView(PetWeaponChangeView)
            -- if ViewMgr:IsOpen(PetWeaponView) then
            --     ViewMgr:CloseView(PetWeaponView)
            -- end
            self.data.sel_change_weapon.pet_weapon = {}
            if self.sel_toggle then
                self.sel_toggle.isOn = false
            else
                self.new_weapon = {}
                self:FlushPanel()
            end
        end
        local tip = string.format(Language.PetWeapon.Tip9,self.cur_weapon.pet_weapon:QuaName(),self.new_weapon.pet_weapon:QuaName())
        PublicPopupCtrl.Instance:AlertTip(tip,func)
    end
end

PetWeaponChangeInfoItem = PetWeaponChangeInfoItem or DeclareMono("PetWeaponChangeInfoItem", UIWidgetBaseItem)
-- data:{pet_weapon:PetWeapon}
function PetWeaponChangeInfoItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local weapon = data.pet_weapon
    if not TableIsEmpty(weapon) then
        self.InfoObj:SetActive(true)
        self.AddInfoObj:SetActive(false)
        UH.SetText(self.SkillDesc, weapon:RefineEffectDesc())
        self.Cell:SetData(weapon)
        local attri_list = weapon:AttriListSort()
        local attri_str_list = {}
        for i,v in pairs(attri_list) do
            local str = string.format("%s：", Language.Common.AttrList[v.attri_type] or "")
            if DataHelper.IsPercent(v.attri_type) then
                local value = string.format("+%s%%",v.attri_value/10)
                value = ColorStr(value, COLORSTR.Green4)
                str = str .. value
            else
                local value = string.format("+%s",v.attri_value)
                value = ColorStr(value, COLORSTR.Green4)
                str = str .. value
            end
            table.insert(attri_str_list, str)
        end
        UH.SetText(self.Attri1, attri_str_list[1])
        UH.SetText(self.Attri2, attri_str_list[2])
    else
        self.InfoObj:SetActive(false)
        self.AddInfoObj:SetActive(true)
        UH.SetText(self.SkillDesc, "")
    end
end

PetWeaponChangeItem = PetWeaponChangeItem or DeclareMono("PetWeaponChangeItem", UIWidgetBaseItem)
function PetWeaponChangeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create({item_id = data.pet_weapon_item})
    self.ItemCell:SetData(item)
    UH.SetText(self.ItemName, item:QuaName())

    local pet_id = data.pet_weapon_pet_id
    if pet_id > 0 then
        local pet = Item.Create({item_id = pet_id})
        self.PetCell:SetData(pet)
        UH.SetText(self.PetName, pet:QuaName())
    end
end

PetWeaponChangeSelView = PetWeaponChangeSelView or DeclareView("PetWeaponChangeSelView", "pet/pet_weapon/pet_weapon_change_sel_view")
function PetWeaponChangeSelView:PetWeaponChangeSelView()
    self.BG:SetData(self:GetType(),Language.PetWeapon.Title5, Vector2.New(660, 520))
    self.sel_list = {}
end

function PetWeaponChangeSelView:LoadCallback(param_t)
    param_t = param_t or {}
    self.list_data = param_t.list_data or self:GetListData()
    self:FlushPanel()
end

function PetWeaponChangeSelView:CloseCallback()
    for i,v in pairs(self.sel_list)do
        if v then
            PetWeaponData.Instance.sel_change_weapon.pet_weapon = i
            return
        end
    end
end

function PetWeaponChangeSelView:GetListData()
    return PetWeaponData.Instance:GetChangeWeaponList()
end

function PetWeaponChangeSelView:FlushPanel()
    self.List:SetData(self.list_data)
end

function PetWeaponChangeSelView:OnSelClick(value, data)
    if value then
        self.sel_list[data] = true
    else
        if self.sel_list[data] then
            self.sel_list[data] = nil
        end
    end
end

function PetWeaponChangeSelView:OnOkClick()
    ViewMgr:CloseView(PetWeaponChangeSelView)
end