-- 替换
PetWeaponView = PetWeaponView or DeclareView("PetWeaponView","pet/pet_weapon/pet_weapon_view", Mod.PetWeapon.Main)

function PetWeaponView:PetWeaponView()
    self.BG:SetData(self:GetType(),Language.PetWeapon.Title1, Vector2.New(1081, 646))
end

function PetWeaponView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[148].desc)
end

PetWeaponReplacePanel = PetWeaponReplacePanel or DeclareMono("PetWeaponReplacePanel", UIWFlushPanel)
function PetWeaponReplacePanel:PetWeaponReplacePanel()
    self.data = PetWeaponData.Instance
    self.data_cares = {
        {data = self.data.replace_sort_info, func = self.FlushList,},--, init_call = false},
        {data = PetData.Instance.pet_list, func = self.FlushPanel, init_call = false},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.PetWeapon), func = self.FlushPanel, init_call = false},
    }
    self.sel_index = 1
    self.sel_data = nil
    if self.ui_obj == nil then
		self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
			{
				ui_obj = self.ui_obj,
                view = self,
                show_shadow = false,
			}
		)
	end
end

function PetWeaponReplacePanel:FlushSelData()
    if self.sel_data then
        if self.sel_data:PetIndex() then
            if self.sel_data:GridPos() == 0 then
                self.sel_data = PetData.Instance:GetPetInfo(self.sel_data:PetIndex()):Weapon()
            else
                self.sel_data = PetData.Instance:GetPetInfo(self.sel_data:PetIndex()):Weapon2()
            end
            -- self.sel_data = PetData.Instance:GetPetInfo(self.sel_data:PetIndex()):Weapon()
        elseif self.sel_data.index >= 0 then
            self.sel_data = BagData.Instance:GetItemByIndex(ItemColumnType.PetWeapon, self.sel_data.index)
        end
    end
end

function PetWeaponReplacePanel:FlushPanel()
    local pet = PetData.Instance:GetCurSelect()
    if PetWeaponData.Instance:GetWeaponIdx() == 0 then
        self.pet_weapon = pet and pet:Weapon() or nil
    else
        self.pet_weapon = pet and pet:Weapon2() or nil
    end
    -- self.pet_weapon = pet and pet:Weapon() or nil
    self.is_first = false
    self:FlushSelData()
    self:FlushSel()
    self:FlushList()
    self:FlushInfo()
end

function PetWeaponReplacePanel:FlushSel()
    local sort_info = self.data.replace_sort_info
    self.UpSortToggle.isOn = sort_info.sort_type == 2
    self.DownSortToggle.isOn = sort_info.sort_type == 1
    local str = Language.PetWeapon.QuaDesc[sort_info.sel_qua]
    if sort_info.sel_qua > 0 then
        str = ColorStr(str, QualityColorStr[sort_info.sel_qua])
    end
    UH.SetText(self.SelText, str)
end

function PetWeaponReplacePanel:FlushList()
    local list_data = self.data:GetAllWeaponListData(true)
    if self.sel_data == nil then
        self.sel_data = list_data[1]
    end
    local len = #list_data
    if len < 9 then
        for i = 1, 9 - len do
            table.insert(list_data, {item_id = 0})
        end
    elseif len % 3 ~= 0 then
        for i = 1, 3 - len % 3 do
            table.insert(list_data, {item_id = 0})
        end
    end
    self.List:SetData(list_data)
end

function PetWeaponReplacePanel:FlushInfo()
    if self.sel_data == nil then
        return
    end
    local pet = PetData.Instance:GetCurSelect()
    if PetWeaponData.Instance:GetWeaponIdx() == 0 then
        self.pet_weapon = pet and pet:Weapon() or nil
    else
        self.pet_weapon = pet and pet:Weapon2() or nil
    end
    local btn_text = ""
    if self.pet_weapon ~= nil then
        if self.sel_data:PetIndex() ~= nil then
            btn_text = Language.PetWeapon.Used
        else
            btn_text = Language.PetWeapon.Replace
        end
    else
        btn_text = Language.PetWeapon.Use
    end
    UH.SetText(self.BtnText, btn_text)

    local skill_title = self.sel_data:WeaponType() == PetWeaponType.Only and Language.PetWeapon.OnelySkill or Language.PetWeapon.Skill
    UH.SetText(self.SkillTitle, skill_title)

    UH.SetText(self.Name, self.sel_data:Name())
    self.ui_obj:SetData(DrawerHelper.GetWeaponPath(self.sel_data:BaseCfg().pet_weapon_res))
    local effect_desc = self.sel_data:RefineEffectAndAttriDesc()
    local sp_id = self.sel_data:SpId()
    if sp_id > 0 then
        UH.SetText(self.NoEffectDesc, "")
        UH.SetText(self.EffectDesc, effect_desc)
    else
        UH.SetText(self.NoEffectDesc, Language.PetWeapon.Tip6)
        UH.SetText(self.EffectDesc, "")
    end
    self:FlushAttriDesc()

    self.Interactor.Interactable = self.sel_data:PetIndex() == nil

    self.BtnRefineObj:SetActive(self.sel_data:NextRefineCfg() ~= nil or self.sel_data:RefineLevel() > 0)

    self.ChangeBtn:SetActive(self.sel_data:QuaType() > 4)

    self:FlushRemind()
end

function PetWeaponReplacePanel:FlushAttriDesc()
    local attri_list = self.sel_data:AttriListSort() or {}
    for i,v in pairs(attri_list) do
        if i > 2 then
            break
        end
        local value = v.attri_value
        if DataHelper.IsPercent(v.attri_type) then
            value = string.format("+%s%%",v.attri_value/100)
        end
        UH.SetText(self["AttriDesc" .. i], string.format("%s    %s",Language.AttrItems.AttrNames[v.attri_type], ColorStr(value, COLORSTR.Green5)))
    end
end

function PetWeaponReplacePanel:FlushRemind()
    --由item里计算并存储改为再算一遍，item中对data的修改（添加了remind_data）会导致item_grid[ItemGridType.PetWeapon]触发smartData从而导致界面监听的smartData套娃，循环触发
    local data = self.sel_data
    local pet = PetData.Instance:GetCurSelect()
    local pet_weapon -- = pet and pet:Weapon() or nil
    if PetWeaponData.Instance:GetWeaponIdx() == 0 then
        pet_weapon = pet and pet:Weapon() or nil
    else
        pet_weapon = pet and pet:Weapon2() or nil
    end
    local remind_num,item_id,item_id_2 = PetWeaponData.Instance:GetReplaceRemind(pet_weapon)
    local remind_data = {}
    if remind_num > 0 and (item_id and data.item_id == item_id) or (item_id_2 and data.item_id == item_id_2) then
        remind_data.rep_num = 1
    elseif PetWeaponData.Instance:GetIntensifyRemind(data) > 0 then
        remind_data.ins_num = 1
    elseif PetWeaponData.Instance:GetRefineRemind(data) > 0 then
        remind_data.ref_num = 1
    end
    
    self.InsRed:SetNum(remind_data.ins_num or 0)
    self.RefRed:SetNum(remind_data.ref_num or 0)
    self.RepRed:SetNum(remind_data.rep_num or 0)
end

function PetWeaponReplacePanel:OnDestroy()
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end

    UIWFlushPanel.OnDestroy(self)
end

function PetWeaponReplacePanel:OnSortSelClick(sort_type)
    self.data.replace_sort_info.sort_type = sort_type
end

function PetWeaponReplacePanel:OnSelToggleItemClick(data)
    UH.SetText(self.SelText, data.str)
    self.SelToggle.isOn = false
    self.data.replace_sort_info.sel_qua = data.qua
end

function PetWeaponReplacePanel:OnSelToggleEvent()
    if not self.set_sel_list then
        self.set_sel_list = true
        local list_data = self.data:GetSelListData(true, true)
        self.SelList:SetData(list_data)
    end
end

function PetWeaponReplacePanel:OnSelClick(data)
    if data ~= self.sel_data or not self.is_first then
        self.sel_data = data
        self.is_first = true
        self:FlushInfo()
    end
end

function PetWeaponReplacePanel:OnReplaceClick()
    if self.sel_data == nil then
        return
    end
    if self.sel_data:PetIndex() ~= nil then
        if self.pet_weapon == nil then
            PublicPopupCtrl.Instance:Center(Language.PetWeapon.Tip7)
        end
        return
    end
    PetWeaponCtrl.Instance:OnUse(self.sel_data)
    ViewMgr:CloseView(PetWeaponView)
end

function PetWeaponReplacePanel:OnIntensifyClick()
    if self.sel_data == nil then
        return
    end
    if self.sel_data:IsFullLevel() and not self.sel_data:IsNeedBreak() then
        PublicPopupCtrl.Instance:Center(Language.PetWeapon.Error8)
    else
        PetWeaponData.Instance:SetCurCtrlWeapon(self.sel_data)
        ViewMgr:OpenView(PetWeaponIntensifyView)
    end
end

function PetWeaponReplacePanel:OnRefineClick()
    if self.sel_data == nil then
        return
    end
    if self.sel_data:NextRefineCfg() == nil and self.sel_data:RefineLevel() == 0 then
		PublicPopupCtrl.Instance:Error(Language.PetWeapon.NoWeaponTip3)
		return
	end
    PetWeaponData.Instance:SetCurCtrlWeapon(self.sel_data)
    ViewMgr:OpenView(PetWeaponRefineView)
end

function PetWeaponReplacePanel:OnBookClick()
    ViewMgr:OpenView(PetWeaponBookView)
end

function PetWeaponReplacePanel:OnChangeClick()
    ViewMgr:OpenView(PetWeaponChangeView, {pet_weawpon = self.sel_data})
end

PetWeaponReplaceItem = PetWeaponReplaceItem or DeclareMono("PetWeaponReplaceItem", UIWidgetBaseItem)
function PetWeaponReplaceItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    data.item_id = data.item_id or 0
    -- data.remind_data = {}
    self.Show:SetActive(data.item_id > 0)
    self.Null:SetActive(data.item_id == 0)
    if data.item_id == 0 then
        if self.RedPoint then
            self.RedPoint:SetNum(0)
        end
        return
    end
    self.Cell:SetData(data)
    self.UseFlag:SetActive(data:PetIndex() ~= nil)
    if self.ParenttPanel then
        if data == self.ParenttPanel.sel_data then
            self.Selector.isOn = true
        else
            self.Selector.isOn = false
        end
    end
    UH.SetText(self.Name, data:Name())

    if TableIsEmpty(data) or data.item_id == 0 then
        if self.RedPoint then
            self.RedPoint:SetNum(0)
        end
    else
        local pet = PetData.Instance:GetCurSelect()
        local pet_weapon -- = pet and pet:Weapon() or nil
        if PetWeaponData.Instance:GetWeaponIdx() == 0 then
            pet_weapon = pet and pet:Weapon() or nil
        else
            pet_weapon = pet and pet:Weapon2() or nil
        end
        local remind_num,item_id,item_id_2 = PetWeaponData.Instance:GetReplaceRemind(pet_weapon)
        if remind_num > 0 and (item_id and data.item_id == item_id) or (item_id_2 and data.item_id == item_id_2) then
            -- data.remind_data.rep_num = 1
            if self.RedPoint then
                self.RedPoint:SetNum(1)
            end
        elseif PetWeaponData.Instance:GetIntensifyRemind(data) > 0 then
            -- data.remind_data.ins_num = 1
            if self.RedPoint then
                self.RedPoint:SetNum(1)
            end
        elseif PetWeaponData.Instance:GetRefineRemind(data) > 0 then
            -- data.remind_data.ref_num = 1
            if self.RedPoint then
                self.RedPoint:SetNum(1)
            end
        else
            if self.RedPoint then
                self.RedPoint:SetNum(0)
            end
        end
    end
end


-- 材料放入
PetWeaponStuffItem = PetWeaponStuffItem or DeclareMono("PetWeaponStuffItem", UIWidgetBaseItem)
function PetWeaponStuffItem:SetData(stuff_item)
    UIWidgetBaseItem.SetData(self, stuff_item)
    self.RemoveBtnObj:SetActive(stuff_item ~= nil and stuff_item.item_id > 0)
    -- if stuff_item.item_id > 0 then
    --     self.Selector.isOn = false
    -- end
    self.Selector:SetObjActive(stuff_item == nil or stuff_item.item_id == 0)
    if stuff_item == nil or stuff_item.item_id == 0 then
        if self.IsBg then
            self.ItemCell:SetData(Item.Create({item_id = 0}))
        else
            self.ItemCell:SetObjActive(false)
        end
    else
        self.ItemCell:SetObjActive(true)
        self.ItemCell:SetData(stuff_item)
    end
end

function PetWeaponStuffItem:OnRemoveClick()
    self:SetData(nil)
    if self.remove_event then
        self.remove_event()
    end
end

function PetWeaponStuffItem:SetRemoveEvent(func)
    self.remove_event = func
end

-- 选择下拉条
PetWeaponSelectItem = PetWeaponSelectItem or DeclareMono("PetWeaponSelectItem", UIWidgetBaseItem)
-- data:{text, qua}
function PetWeaponSelectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local str = data.str or Language.PetWeapon.QuaDesc[data.qua]
    if data.qua > 0 then
        str = ColorStr(str, QualityColorStr[data.qua])
    end
    UH.SetText(self.Text, str)
    data.str = str
end