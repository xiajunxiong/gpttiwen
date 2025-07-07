PetRefiningPreview = PetRefiningPreview or DeclareView('PetRefiningPreview', 'pet_refining/pet_refining_preview')
PetRefiningPreviewPanel = PetRefiningPreviewPanel or DeclareMono('PetRefiningPreviewPanel', UIWFlushPanel)
function PetRefiningPreviewPanel:PetRefiningPreviewPanel()
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
function PetRefiningPreviewPanel:onFlush()
    --获取到两只宠物 否则无法预览
    local pet = self.data:Pet1()
    local pet2 = self.data:Pet2()

    local res_id = pet:ResId()
    if self.pet_data:GetPetSkinCfgList(pet:ID()) then --皮肤标签
        if self.pet_data.cur_select_skin.skin_data ~= nil and self.pet_data.cur_select_skin.try_skin then --该宠物皮肤为试穿或装备状态
            res_id = self.pet_data.cur_select_skin.skin_data.res_id
        end
    end
    self.showed_skin = true
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(res_id), pet:FeiShengTimes())
    local weapon_data = {}
    local pet_weapon = pet:Weapon()
    weapon_data.is_show = true
    weapon_data.pet_weapon = pet_weapon
    self.SoulEquip1:SetData(weapon_data)

    local weapon_data = {}
    local pet_weapon = pet2:Weapon()
    weapon_data.is_show = true
    weapon_data.pet_weapon = pet_weapon
    self.SoulEquip2:SetData(weapon_data)

    self.NewObj:SetActive(not pet:IsLianYao())

    UH.SetText(self.Name, pet:Name())
    UH.SpriteName(self.PetTypeImg, string.format('Race%s', pet:RaceId()))
    UH.SpriteName(self.PetTypeNameImg, string.format('_LocRaceName%s', pet:RaceId()))
    UH.SpriteName(self.LevelSprite, PetData.PetLevel[pet.config.quality])
    local fly_flag = pet:FeiShengTimes()
    self.NameFrame.enabled = fly_flag == 0
    UH.SpriteName(self.FlyBgSet, (fly_flag == 0 and '' or ('ChongWuMingChenFeiSheng' .. fly_flag)))

    local main_skill = TableCopy(pet:SkillList())
    local main_skill2 = pet2:SkillList()
    for _, value in ipairs(main_skill2) do
        value.new_flag = true
        table.insert(main_skill, value)
    end
    local skills = pet:GetLearnedGiftList({})
    if #skills <= 4 then
        self.List2:SetData(skills)
        self.List3:SetData({})
    else
        local list = {}
        local list2 = {}--每5个一组
        local l2idx = 1
        for i = 1, #skills do
            if i <= 4 then
                table.insert(list, skills[i])
            else
                l2idx = math.max(math.floor((i - 4) / 5), l2idx)
                list2[l2idx] = list2[l2idx] or {}
                table.insert(list2[l2idx], skills[i])
            end
        end
        self.List2:SetData(list)
        self.List3:SetData(list2)
    end
    self.List1:SetData(main_skill)
    
    local config = pet:ConfigGrowthList()
    local total = pet:TotalGrowthList()
    local index = 1
    for key, value in pairs(config) do
        index = PetData.GetZZIndex(key)
        UH.SetText(self.ValueTexts[index], string.format("%s/%s", total[key], value))
    end
    --假如已经是炼妖宠，原有的要显示
    local list = {}
    if pet:IsLianYao() then
       list = pet:LianYaoSkillList(list)
    end
    local skills2 = pet2:GetLearnedGiftList({})
    local show_num = self.data.other_info.addskill_max
    if #list < show_num then
        for i = 1, show_num - #list do
            if skills2[i] then
                skills2[i].corner_icon = Language.PetRefining.GaiLv
                table.insert(list, skills2[i])
            end
        end
    end
    for i, v in ipairs(skills2) do
        v.is_yao_skill = true
    end
    --[[ if #skills2 < show_num then
        list = TableCopy(skills2)
    else
        for i = 1, show_num do
            list[i] = skills2[i]
        end
    end
    for index, value in ipairs(list) do
        value.corner_icon = Language.PetRefining.GaiLv
    end ]]
    self.NewList:SetData(list)
end

function PetRefiningPreviewPanel:OnClickGift(cell)
    PetGiftItem.CommonClickFunc(cell)
end
function PetRefiningPreviewPanel:OnClickClose()
    ViewMgr:CloseView(PetRefiningPreview)
end
function PetRefiningPreviewPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
end
