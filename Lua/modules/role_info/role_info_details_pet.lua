RoleInfoDetailsPet = RoleInfoDetailsPet or DeclareMono("RoleInfoDetailsPet", UIWFlushPanel)
--NextArrObj=====GameObject
--PrevArrObj=====GameObject
--FightingTag====GameObject
--WaitingTag=====GameObject
--JieTypeImg=====UIImageSpriteSet
--Score==========UIWScoreItem

function RoleInfoDetailsPet:RoleInfoDetailsPet()
    self.cur_show_index = -1
    -- self.flushed = false
    -- self:FlushPetInfo()
end

function RoleInfoDetailsPet:OnEnable()
    -- LogError("OnEnable@@@@@@@@")
    UIWFlushPanel.OnEnable(self)
    -- self.flushed = true

    self.pet_info_list = RoleInfoCtrl.Instance:Data():GetPetList()
    -- 计算共鸣等级
    local level_t = {}
    self.pet_list = {}
    for i,v in pairs(self.pet_info_list) do
        local pet = Pet.New(v)
        table.insert(self.pet_list, pet)
        table.insert(level_t, pet:StrLevel())
    end
    local buff_level = PetData.Instance:CalculateBuffLevel(level_t)
    for i,v in pairs(self.pet_list) do
        v:SetComLevel(buff_level)
    end

    self:FlushPetInfo(1)
end

function RoleInfoDetailsPet:FlushPetInfo(index)
    if index == self.cur_show_index then
        return
    end
    self.cur_show_index = index
    local pet = self.pet_list[self.cur_show_index]
    local pet_info = self.pet_info_list[self.cur_show_index]
    self:FlushPet(pet, pet_info)
    self.PrevArrObj:SetActive(self.cur_show_index ~= 1)
    self.NextArrObj:SetActive(self.cur_show_index < #self.pet_list)
end

function RoleInfoDetailsPet:FlushPet(pet,pet_info)
    -- if pet_info.pet_id == 0 then
    --     return
    -- end
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self,
                show_shadow = true,
                shadow_dir = ModelUIShow.ShadowCameraDir.Pet
            }
        )
    end

    --local pet = Pet.New(pet_info)
    UH.SetText(self.NameTex, pet:Name())
    UH.Color(self.NameTex, pet:IsLianYao() and COLORS.Pink2 or COLORS.Red2)
    if self.YaoObj then
        self.YaoObj:SetActive(pet:IsLianYao())
    end

    if pet:ResId() then
        self.ui_obj:SetData(DrawerHelper.GetNpcPath(pet:ResId()),pet:FeiShengTimes())
        self.SignNameItem:SetType(pet:LimitType())
    end

    UH.SetText(self.RaceTex, Language.Pet.RaceClass[pet.config.race])
    UH.SpriteName(self.RaceIcon, string.format("Race%s", pet:RaceId()))
    UH.SpriteName(self.JieTypeImg, PetData.PetLevel2[pet:Quality()])
    if pet:Rank() >= 0 and pet:Rank() <= 9 then
        self.PetRank:SetActive(true)
        self.PetRank:SetRank(pet:Rank() + 1)
    else
        self.PetRank:SetActive(false)
    end
    self.EleAttr:SetData(pet:EleAttr())
    self.AttrList:SetPet(pet, false)

    local v1 = pet:TotalGrowth()
    local v2 = pet:ConfigGrowth()
    UH.SetText(self.LblValue, string.format(Language.Pet.GrowthStr, COLORSTR.Gray1, COLORSTR.Red14, v1, v2))
    local total_attr = pet:TotalAttr()
    for k, v in pairs(total_attr) do
        if SimpleAttrNameForType[k] then
            UH.SetText(self["Lbl" .. SimpleAttrNameForType[k]], v)
        end
    end
    local gift_list = pet:GetSkillAndGiftListNoEmpty()
    for _, v in ipairs(gift_list) do
        v.owner_pet = pet
    end
    -- LogError("gift_list====",gift_list,pet_info.gift_list)
    self.SkillList:SetData(gift_list)

    self.FightingTag:SetActive(pet_info.pet_state == 1)
    self.WaitingTag:SetActive(pet_info.pet_state >= 2)
    self.Score:SetScore(pet:GetScore())
    PetData.Instance.SetStrLevel(nil,self.StrLevel,pet_info.str_level)

    local weapon_1 = pet:Weapon()
    if not TableIsEmpty(weapon_1) then
        weapon_1:SetPet(pet)
    end
    local weapon_2 = pet:Weapon2()
    if not TableIsEmpty(weapon_2) then
        weapon_2:SetPet(pet)
    end
    self.PetWeaponInfo:SetData({is_show = true, pet_weapon = weapon_1})
    self.PetWeaponInfo2:SetObjActive(pet:IsLianYao())
    self.PetWeaponInfo2:SetData({is_show = true, pet_weapon = weapon_2})

    self:FlushPetStamp(pet)
end

function RoleInfoDetailsPet:OnClickNext()
    self:FlushPetInfo(self.cur_show_index + 1)
end

function RoleInfoDetailsPet:OnClickPrev()
    self:FlushPetInfo(self.cur_show_index - 1)
end

function RoleInfoDetailsPet:OnDestroy()
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
    UIWFlushPanel.OnDestroy(self)
end


function RoleInfoDetailsPet:OnClickGift(cell)
    local data = cell:GetData()
    if data.skill_id and data.skill_id ~= 0 or (data.gift_id and data.gift_id ~= 0) then
        PetData.Instance:SetClickSkill(data)
        ViewMgr:OpenView(PetSkillTip)
    --[[ elseif data.show_lingwu then
        ViewMgr:OpenView(PetLingwuSkillTip) ]]
    end
end

function RoleInfoDetailsPet:OnClickOption()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

-- 神印信息
function RoleInfoDetailsPet:FlushPetStamp(pet)
    self.cur_blueprint = pet:StampBlueprint()
    self.cur_blueprint.stamp_index_list = {0,0,0,0,0}
    self.cur_blueprint.pet_index = nil
    self.PetStampDetailsInfo:SetData({blueprint = self.cur_blueprint})
end

PetStampDetailsInfo = PetStampDetailsInfo or DeclareMono("PetStampDetailsInfo", UIWidgetBaseItem)
-- data:{blueprint:PetStampBlueprint}
function PetStampDetailsInfo:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.cur_blueprint = data.blueprint
    local suit_list = PetStampData.Instance:GetBlueprintSuit(self.cur_blueprint)
    suit_list = suit_list or {}
    self.PetStampSuitInfoShow:SetData(suit_list)
    self.PetStampBlueprint:SetData(self.cur_blueprint)
end

function PetStampDetailsInfo:OnPetStampAttriClick()
    local attri_list = self.cur_blueprint:AttriList() or {}
    local param_t = {
        title = Language.PetStamp.Title3,
        attri_list = attri_list,
    }
    ViewMgr:OpenView(PetStampAttriAddition, param_t)
end