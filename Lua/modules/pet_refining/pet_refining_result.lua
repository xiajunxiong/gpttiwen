PetRefiningResult = PetRefiningResult or DeclareView('PetRefiningResult', 'pet_refining/pet_refining_result')
VIEW_DECLARE_MASK(PetRefiningResult, ViewMask.BlockClose)
PetRefiningResultPanel = PetRefiningResultPanel or DeclareMono('PetRefiningResultPanel', UIWFlushPanel)
function PetRefiningResultPanel:PetRefiningResultPanel()
    self.pet_data = PetData.Instance
    self.data = PetRefiningData.Instance
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({
            ui_obj = self.ui_obj,
            view = self,
            show_shadow = true,
            shadow_dir = ModelUIShow.ShadowCameraDir.Pet,
        })
    end
end
function PetRefiningResultPanel:onFlush()
    --获取到两只宠物 否则无法预览
    local pets = PetData.Instance:GetPetList()
    -- local pet = PetData.Instance:GetPetList()[2]-- PetRefiningData.Instance:Pet1()
    local pet = self.data:ResultPet()
    if pet == nil then
        LogError("获取不到炼妖结果")
        return
    end
    local res_id = pet:ResId()
    if self.pet_data:GetPetSkinCfgList(pet:ID()) then --皮肤标签
        if self.pet_data.cur_select_skin.skin_data ~= nil and self.pet_data.cur_select_skin.try_skin then --该宠物皮肤为试穿或装备状态
            res_id = self.pet_data.cur_select_skin.skin_data.res_id
        end
    end
    self.showed_skin = true
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(res_id), pet:FeiShengTimes())

    UH.SetText(self.Name, pet:Name())
    UH.SetText(self.Name2, pet:Name())
    UH.SetText(self.Level, string.format("Lv.%s",pet:Level()))
    UH.SpriteName(self.ImgJie, PetData.PetLevel2[pet:Quality()])
    UH.SpriteName(self.ImgJieBg, string.format("JieBie%d", pet:Quality()))
    --[[ UH.SpriteName(self.PetTypeImg, string.format('Race%s', pet:RaceId()))
    UH.SpriteName(self.PetTypeNameImg, string.format('_LocRaceName%s', pet:RaceId()))
    UH.SpriteName(self.LevelSprite, PetData.PetLevel[pet.config.quality])
    local fly_flag = pet:FeiShengTimes()
    self.NameFrame.enabled = fly_flag == 0
    UH.SpriteName(self.FlyBgSet, (fly_flag == 0 and '' or ('ChongWuMingChenFeiSheng' .. fly_flag))) ]]

    local main_skill = pet:GetMainSkillList({}, true)
    local skills = pet:GetLearnedGiftList({})
    skills = pet:LianYaoSkillList(skills)
    self.List2:SetData(skills)
    self.List1:SetData(main_skill)
    
    self.AttrList:SetPet(pet)

    local weapon_data = {}
    local pet_weapon = pet:Weapon()
    weapon_data.is_show = true
    weapon_data.pet_weapon = pet_weapon
    self.SoulEquip1:SetData(weapon_data)

    local weapon_data = {}
    local pet_weapon = pet:Weapon2()
    weapon_data.is_show = true
    weapon_data.pet_weapon = pet_weapon
    self.SoulEquip2:SetData(weapon_data)
    --[[ local config = pet:ConfigGrowthList()
    local total = pet:TotalGrowthList()
    local index = 1
    for key, value in pairs(config) do
        index = PetData.GetZZIndex(key)
        UH.SetText(self.ValueTexts[index], string.format("%s/%s", total[key], value))
    end ]]

end

function PetRefiningResultPanel:OnClickGift(cell)
    PetGiftItem.CommonClickFunc(cell)
end
function PetRefiningResultPanel:OnClickClose()
    ViewMgr:CloseView(PetRefiningResult)
end
function PetRefiningResultPanel:OnDestroy()
    local pet = self.data:ResultPet()
    for i, v in ipairs(pet.info.lian_yao_passive_list) do
        v.new_flag = false
    end 
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
    self.data:ViewData().refining_ing = false
end
