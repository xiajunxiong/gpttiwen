PetWeaponUpShowType = {
    UpRefine = 1,   -- 精炼提升
    ConvertGet = 2, -- 兑换获得
}

PetWeaponUpShowView = PetWeaponUpShowView or DeclareView("PetWeaponUpShowView", "pet/pet_weapon/pet_weapon_up_show_view")
-- VIEW_DECLARE_LEVEL(PetWeaponUpShowView, ViewLevel.L0)
function PetWeaponUpShowView:PetWeaponUpShowView()

end

-- param_t : {data: {old_weapon:PetWeapon, cur_weapon:PetWeapon}}
function PetWeaponUpShowView:LoadCallback(param_t)
    param_t = param_t or {}
    self.data = param_t.data
    if self.ui_obj == nil then
		self.ui_obj = UIObjDrawer.New()
        self.ModeShow:SetShowData(
			{
				ui_obj = self.ui_obj,
                view = self,
                show_shadow = false,
			}
		)
    end

    self.show_type = param_t.show_type or PetWeaponUpShowType.UpRefine

    TimeHelper:CancelTimer(self.time_ht)
    self.time_ht = Invoke(function ()
        self.EffectObj:SetActive(false)
    end,1.5)
    self:FlushPanel()
end

function PetWeaponUpShowView:FlushPanel()
    if TableIsEmpty(self.data) then
        return
    end
    self:FlushCommon()

    local info_mono = self.Infos[self.show_type]
    info_mono:SetObjActive(true)
    if self.show_type == PetWeaponUpShowType.UpRefine then
        self:FlushRefineInfo()
    elseif self.show_type == PetWeaponUpShowType.ConvertGet then
        self:FlushConvertInfo()
    end
end

function PetWeaponUpShowView:FlushCommon()
    UH.SetText(self.Name, self.data.cur_weapon:Name())
    self.ui_obj:SetData(DrawerHelper.GetWeaponPath(self.data.cur_weapon:ModelId()))
end

function PetWeaponUpShowView:FlushRefineInfo()
    local info_mono = self.Infos[PetWeaponUpShowType.UpRefine]
    info_mono:SetData(self.data)
end

function PetWeaponUpShowView:FlushConvertInfo()
    local info_mono = self.Infos[PetWeaponUpShowType.ConvertGet]
    info_mono:SetData(self.data)
end

function PetWeaponUpShowView:CloseCallback()
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
    end
    TimeHelper:CancelTimer(self.time_ht)
end

function PetWeaponUpShowView:OnCloseClick()
    ViewMgr:CloseView(PetWeaponUpShowView)
end

PetWeaponUpRefineInfo = PetWeaponUpRefineInfo or DeclareMono("PetWeaponUpRefineInfo",UIWidgetBaseItem)
-- data: {old_weapon:PetWeapon, cur_weapon:PetWeapon}
function PetWeaponUpRefineInfo:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local old_weapon = data.old_weapon
    local cur_weapon = data.cur_weapon
    UH.SetText(self.OldLevel, string.format(Language.PetWeapon.Step,DataHelper.GetDaXie(old_weapon:RefineLevel())))
    local desc = old_weapon:RefineEffectAndAttriDesc()
    UH.SetText(self.OldDesc, desc)

    UH.SetText(self.CurLevel, string.format(Language.PetWeapon.Step,DataHelper.GetDaXie(cur_weapon:RefineLevel())))
    UH.SetText(self.CurDesc, cur_weapon:RefineEffectAndAttriDesc())
end

PetWeaponGetInfo = PetWeaponGetInfo or DeclareMono("PetWeaponGetInfo", UIWidgetBaseItem)
function PetWeaponGetInfo:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local weapon = data.cur_weapon
    local attri_list = weapon:AttriListSort()
    self.Attri1:SetData(attri_list[1])
    self.Attri2:SetData(attri_list[2])

    UH.SetText(self.EffectDesc, weapon:RefineEffectDesc())

    local pet_id = weapon:BaseCfg().pet_weapon_pet_id
    if pet_id and pet_id > 0 then
        local pet = Item.Create({item_id = pet_id})
        self.Cell:SetData(pet)
        UH.SetText(self.CellName, pet:Name())
    end
end