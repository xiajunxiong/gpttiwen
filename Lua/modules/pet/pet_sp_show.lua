-- 宠物新展示
PetSpShow = PetSpShow or DeclareView("PetSpShow", "pet/pet_sp_show")

function PetSpShow:PetSpShow()
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({
            ui_obj = self.ui_obj,
            view = PetBookPanel,
            show_shadow = true,
            shadow_dir = ModelUIShow.ShadowCameraDir.Pet,
        })
    end
end

function PetSpShow:OnCloseClick()
    ViewMgr:CloseView(PetSpShow)
end

function PetSpShow:LoadCallback(param_t)
    param_t = param_t or {}
    local pet = param_t.pet or Pet.New({pet_id = 50237})
    self.pet = pet
    self:FlushPanel(pet)
end

function PetSpShow:CloseCallback()
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
end

function PetSpShow:FlushPanel(pet)
    UH.SetText(self.Name, pet:Name())
    local qua = pet:Quality()
    UH.SpriteName(self.QuaIcon, "_LocJieType" .. qua)
    local pet_weapon = pet:ExclusiveWeapon()
    self.PetWeaponInfo:SetData({pet_weapon = pet_weapon, is_show = true})
    self.AttriView:SetPet(pet,true)
    local skill_list = pet:GetConfigGiftList()
    self.SkillList:SetData(skill_list)
    local main_list = pet:GetMainSkillList({})
    for i = 1, self.MainSkillList:Length() do
        local mono = self.MainSkillList[i]
        mono:SetObjActive(not TableIsEmpty(main_list[i]))
        if not TableIsEmpty(main_list[i]) then
            mono:SetData(main_list[i])
        end
    end
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(pet:ResId()), pet:FeiShengTimes())
    if not self.showed_skin then
        self.ui_obj:PlayAnim(ChAnimType.Show)
    end
    self.LimitItem:SetType(pet:LimitType())
    local attri_value = ColorStr(pet:MaxBp(), COLORSTR.Yellow21)
    UH.SetText(self.AttriValue, string.format(Language.Pet.PetSpShow1, attri_value))
end

function PetSpShow:OnTabClick(type)
    local pet = self.pet
    if type == 1 then
        pet = PetData.Instance:GetMutantPetCfgByNormalId(pet.config.prototype_id,PetData.MutantLv.One)
    end
    self:FlushPanel(pet)
end

function PetSpShow:OnClickGift(data)
    PetData.Instance:SetClickSkill(data)
    ViewMgr:OpenView(PetSkillTip)
end